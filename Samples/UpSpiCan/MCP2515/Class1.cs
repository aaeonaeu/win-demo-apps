using MCP2515;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Devices.Spi;

public interface ICanDevice
{
    void CanSend(CanFrame frame);   // Send CAN Frame
    void ClearRxBuffer(int bufferIndex);

    CanFrame? CanRecv();             // Receive CAN Frame
}

public struct CanFrame
{
    public uint Id;           // CAN ID (11-bit for Standard, 29-bit for Extended)
    public bool IsExtended;   // Is Extended ID?
    public bool IsRemote;     // Is RTR (Remote Transmission Request)
    public byte DLC;          // Data Length Code (0~8)
    public byte[] Data;       // Data (Up to 8 bytes)

    public CanFrame(uint id, bool isExtended, bool isRemote, byte dlc, byte[] data)
    {
        Id = id;
        IsExtended = isExtended;
        IsRemote = isRemote;
        DLC = dlc > 8 ? (byte)8 : dlc; // Maximum limit 8
        Data = new byte[8];
        Array.Copy(data, Data, DLC);
    }

    public override string ToString()
    {
        string idType = IsExtended ? "Extended" : "Standard";
        string rtr = IsRemote ? "RTR" : "Data";
        string dataStr = BitConverter.ToString(Data, 0, DLC);
        return $"ID: {Id:X}, Type: {idType}, {rtr}, DLC: {DLC}, Data: [{dataStr}]";
    }
}

class UPSPI
{
    private SpiController controller;
    private SpiConnectionSettings settings;

    private const byte RESET = 0xC0;
    private const byte READ = 0x03;
    private const byte WRITE = 0x02;
    private const byte RTS_TX0 = 0x81;
    private const byte READ_RXB0 = 0x90;

    public UPSPI(int bus)
    {
        UpBridge.Up upb = new UpBridge.Up();
        controller = SpiController.GetControllersAsync(UpBridge.UpSpiProvider.GetSpiProvider())
                          .GetAwaiter().GetResult()[bus];

        settings = new SpiConnectionSettings(0)
        {
            ClockFrequency = 1000000,
            DataBitLength = 8,
            Mode = SpiMode.Mode0,
            SharingMode = SpiSharingMode.Exclusive
        };
    }
    public byte ReadRegister(byte address)
    {
        byte[] Wbuffer = new byte[3];
        byte[] Rbuffer = new byte[3];

        Wbuffer[0] = READ;
        Wbuffer[1] = address;
        Wbuffer[2] = 0x00;
        controller.GetDevice(settings).TransferFullDuplex(Wbuffer, Rbuffer);
        return Rbuffer[2];
    }
    public void Reset()
    {
        byte[] data = { RESET };
        controller.GetDevice(settings).Write(data);
    }
    public void WriteRegister(byte address, byte value)
    {
        byte[] buffer = new byte[3] { WRITE, address, value };
        controller.GetDevice(settings).Write(buffer);
    }
    public void intoloop()
    {

        Reset();
        //TQ=(2*(BRP+1))/Fosc
        //BRP=0,8MHz crystal  ->  TQ=2/8M=250ns
        WriteRegister(0x2A, 0x00); // CNF1

        //t bit=t_SyncSeg   +  t_PropSeg +    t_PS1   +  t_PS2
        //1TQ+3TQ +1TQ +3TQ=8TQ
        //8TQ=8*250ns=2us
        //f=1/t -> 1/2us=500Khz
        WriteRegister(0x29, 0x90); // CNF2
        WriteRegister(0x28, 0x02); // CNF3

        // Receiver mask and filter (all enabled)
        WriteRegister(0x20, 0x00); // RXM0
        WriteRegister(0x24, 0x00); // RXM1
        WriteRegister(0x0F, 0x40);


        // RXB0 receives any message (bits 6:5 = 11)
        byte rxb0ctrl = ReadRegister(0x60);
        rxb0ctrl = (byte)((rxb0ctrl & ~(0b11 << 5)) | (0b11 << 5));
        WriteRegister(0x60, rxb0ctrl);


        // Enable receive interrupt
        WriteRegister(0x2B, 0x03); // RX0IE + RX1IE

        // Clear all interruption flags
        WriteRegister(0x2C, 0x00); // CANINTF = 0

    }
    public byte ReadRXBuffer(byte nm)
    {
        byte[] buffer = new byte[2];
        buffer[0] = (byte)(0x90 | (nm << 1));
        buffer[1] = 0x00;
        controller.GetDevice(settings).TransferFullDuplex(buffer, buffer);
        return buffer[1];
    }
    public void LoadTXBuffer(byte abc, byte data)
    {
        byte[] buffer = new byte[2];
        buffer[0] = (byte)(0x40 | abc);
        buffer[1] = data;
        controller.GetDevice(settings).Write(buffer);
    }
    public void SendRTS(byte T210)
    {
        byte[] buffer = new byte[1];
        buffer[0] = (byte)(0x80 | T210);
        controller.GetDevice(settings).Write(buffer);
    }
    public void Bit_Modify(byte addr, byte mask, byte data)
    {
        byte[] buffer = new byte[4] { 0x02, addr, mask, data };
        controller.GetDevice(settings).Write(buffer);
    }
    public byte[] Read_Status()
    {
        byte[] wbuffer = new byte[1] { 0xA0 };
        byte[] rbuffer = new byte[2];

        controller.GetDevice(settings).TransferSequential(wbuffer, rbuffer);

        return rbuffer;
    }
    public byte[] Rx_Status()
    {
        byte[] wbuffer = new byte[1] { 0xB0 };
        byte[] rbuffer = new byte[2];

        controller.GetDevice(settings).TransferSequential(wbuffer, rbuffer);
        return rbuffer;

    }
}


namespace MCP2515
{
    public class MCP2515 : ICanDevice
    {
        private int bus;
        public MCP2515(int bus) {
            this.bus = bus;
        }
        // SPI Init...
        public void CanSend(CanFrame frame)
        {
            // Parse frame → MCP2515 TX buffer format
            // Set SIDH, SIDL, DLC, Data
            // then RTS
            var UPSPI = new UPSPI(bus);

            if (!frame.IsExtended)
            {
                // Standard 11-bit ID
                uint sid = frame.Id & 0x7FF; // 11 bits

                byte sidh = (byte)((sid >> 3) & 0xFF);               // SID[10:3]
                byte sidl = (byte)(((sid & 0x07) << 5)               // SID[2:0] -> bits 7:5
                                   | 0x00);                          // EXIDE=0, EID[17:16]=0

                UPSPI.WriteRegister(0x31, sidh);
                UPSPI.WriteRegister(0x32, sidl);
                UPSPI.WriteRegister(0x33, 0x00); // EID8 clear to 0
                UPSPI.WriteRegister(0x34, 0x00); // EID0 clear to 0
                UPSPI.WriteRegister(0x35, frame.DLC);
            }
            else
            {


                // Extended 29-bit ID: 11-bit SID + 18-bit EID
                uint raw = frame.Id & 0x1FFFFFFF; // 29 bits 
                uint sid = (raw >> 18) & 0x7FF;   // High 11 bits -> SID
                uint eid = raw & 0x3FFFF;         // Low 18 bits -> EID

                byte sidh = (byte)((sid >> 3) & 0xFF);                     // SID[10:3]
                byte sidl = (byte)(((sid & 0x07) << 5)                     // SID[2:0] -> bits 7:5
                                   | (1 << 3)                               // EXIDE=1
                                   | (eid >> 16) & 0x03);                   // EID[17:16] -> bits 1:0

                byte eid8 = (byte)((eid >> 8) & 0xFF);                      // EID[15:8]
                byte eid0 = (byte)(eid & 0xFF);                             // EID[7:0]
                
                

                UPSPI.WriteRegister(0x31, sidh);

                UPSPI.WriteRegister(0x32, sidl);

                UPSPI.WriteRegister(0x33, eid8);

                UPSPI.WriteRegister(0x34, eid0);

                UPSPI.WriteRegister(0x35, frame.DLC);

            }

            for (int i = 0; i < frame.DLC; i++)
                UPSPI.WriteRegister((byte)(0x36 + i), frame.Data[i]);


            //UPSPI.SendRTS(0x1); // RTS TX0
            byte ctrl = UPSPI.ReadRegister(0x30);
            ctrl |= (1 << 3); // TXREQ bit
            UPSPI.WriteRegister(0x30, ctrl);


        }

        public void ClearRxBuffer(int bufferIndex)
        {
            var UPSPI = new UPSPI(bus);

            byte canintf = UPSPI.ReadRegister(0x2C);

            if (bufferIndex == 0)
                canintf = (byte)(canintf & ~0x01); // clear RX0IF
            else if (bufferIndex == 1)
                canintf = (byte)(canintf & ~0x02); // clear RX1IF

            UPSPI.WriteRegister(0x2C, canintf);
        }

        public void intoloop()
        {
            var UPSPI = new UPSPI(bus);

            UPSPI.intoloop();
        }
        public CanFrame? CanRecv()
        {
            var UPSPI = new UPSPI(bus);
            byte canintf = UPSPI.ReadRegister(0x2C);
            int cnt = 0;
            while((canintf & 0x01) == 0)
            {

                if (cnt++ > 1000)
                    return null;
                canintf = UPSPI.ReadRegister(0x2C); // Reload!

            }


            // 讀取 RXB0
            byte sidh = UPSPI.ReadRegister(0x61);
            byte sidl = UPSPI.ReadRegister(0x62);
            byte eid8 = UPSPI.ReadRegister(0x63);
            byte eid0 = UPSPI.ReadRegister(0x64);

            byte rx_dlc = UPSPI.ReadRegister(0x65);

            byte dlc = UPSPI.ReadRegister(0x65);

            uint id;
            bool rtr = false; // If RTR support is required, it can be determined from RXBnCTRL/RXBnSIDL.


            bool isExtended = ((sidl & (1 << 3)) != 0); // SIDL.EXIDE

            bool isRemote = ((rx_dlc & (1 << 6)) != 0); // RTR bit of RXBnDLC


            byte[] data = new byte[dlc];

            if (!isRemote) // RTRs generally do not include data.
            {
                for (int i = 0; i < dlc; i++)
                    data[i] = UPSPI.ReadRegister((byte)(0x66 + i));
            }



            if (!isExtended)
            {

                // Standard: SID = SIDH/SIDL
                uint sid = (uint)((sidh << 3) | (sidl >> 5));
                id = sid & 0x7FF;
            }
            else
            {
                // Extended: SID and EID mixed
                uint sid = (uint)((sidh << 3) | (sidl >> 5)) & 0x7FF;         // SID[10:0]
                uint eid = (uint)(((sidl & 0x03) << 16) | (eid8 << 8) | eid0) & 0x3FFFF; // EID[17:0]
                id = ((sid & 0x7FF) << 18) | (eid & 0x3FFFF);                 // 29-bit ID
            }


            if (!isRemote)
            {
                for (int i = 0; i < dlc; i++)
                    data[i] = UPSPI.ReadRegister((byte)(0x66 + i));
            }

            byte canintf2 = UPSPI.ReadRegister(0x2C);
            UPSPI.WriteRegister(0x2C, (byte)(canintf2 & ~0x01)); // Clear RX0IF



            return new CanFrame((uint)id, isExtended, isRemote, dlc, data);

        }
    }
}
