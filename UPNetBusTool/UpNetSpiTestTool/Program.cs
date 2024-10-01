using System;
using Windows.Devices.Spi;
using System.Threading.Tasks;
using Windows.Devices.I2c;
using UpNetlib;
using NetUp.NetSPI;
// This example code shows how you could implement the required main function for a 
// Console UWP Application. You can replace all the code inside Main with your own custom code.

// You should also change the Alias value in the AppExecutionAlias Extension in the 
// Package.appxmanifest to a value that you define. To edit this file manually, right-click
// it in Solution Explorer and select View Code, or open it with the XML Editor.

namespace UpSpiTestTool
{
    class Program
    {

        static string Usage =
          "UpSpiTestTool: Command line SPI testing utility\n" +
          "commands:\n" +
          "\n" +
          "  write        Write bytes to device\n" +
          "  read         Read N bytes\n" +
          "  writeread    Write bytes then read N bytes\n" +
          "  fullduplex   Perform full duplex transfer\n" +
          "  set          Set devices infomation\n" +
          "  info         Display device information\n" +
          "  help         show commands\n" +
          "  Example:     %s> <commands>\n" +
          "  exit         exit SPI test\n" +
          "\n";

        struct spiinfo
        {
            public int bus;
            public int DataBitLength;
            public int ClockFrequency;
            public int ChipSelectLine;
            public SpiMode Mode;
            public SpiSharingMode SharingMode;
        }
        static spiinfo spi;
        static SpiController controller;
        static async Task<bool> controllerinit(int index)
        {
            try
            {
                var list = await SpiController.GetControllersAsync(UPSPIProvider.Instance);
                if ((index + 1) > list.Count)
                   return false;
                controller = list[index];

            }
            catch (Exception e)
            {
                throw new Exception(e.Message);
            }
            return true;

        }
        static void spiset()
        {
            try
            {
                Console.WriteLine("Select Bus:");
                spi.bus = Convert.ToInt32(Console.ReadLine());

                if(!controllerinit(spi.bus).Result)
                {
                    Console.WriteLine("No have SPI");
                    return;

                }

                Console.WriteLine("Set DataBitLength:");
                spi.DataBitLength = Convert.ToInt32(Console.ReadLine());
                Console.WriteLine("Set ClockFrequency:");
                spi.ClockFrequency = Convert.ToInt32(Console.ReadLine());
                Console.WriteLine("Set ChipSelectLine:");
                spi.ChipSelectLine = Convert.ToInt32(Console.ReadLine());
                while (true)
                {
                    int mode = -1;
                    Console.WriteLine("Set Mode:");
                    mode = Convert.ToInt32(Console.ReadLine());
                    if (mode == 0)
                    {
                        spi.Mode = SpiMode.Mode0;
                        break;
                    }
                    else if (mode == 1)
                    {
                        spi.Mode = SpiMode.Mode1;
                        break;
                    }
                    else if (mode == 2)
                    {
                        spi.Mode = SpiMode.Mode2;
                        break;
                    }
                    else if (mode == 3)
                    {
                        spi.Mode = SpiMode.Mode3;
                        break;
                    }
                }
                while (true)
                {
                    int mode = -1;
                    Console.WriteLine("Set SharingMode:     Exclusive = 0 , Shared = 1");
                    mode = Convert.ToInt32(Console.ReadLine());
                    if (mode == 0)
                    {
                        spi.SharingMode = SpiSharingMode.Exclusive;
                        break;
                    }
                    else if (mode == 1)
                    {
                        spi.SharingMode = SpiSharingMode.Shared;
                        break;
                    }
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }
        static async Task spiwrite(string[] input)
        {
            try
            {
                SpiConnectionSettings settings = new SpiConnectionSettings(spi.ChipSelectLine);
                settings.ClockFrequency = spi.ClockFrequency;
                settings.DataBitLength = spi.DataBitLength;
                settings.Mode = spi.Mode;
                settings.SharingMode = spi.SharingMode;
                byte[] wrtiebuf = new byte[input.Length - 1];
                for (int i = 1; i < input.Length; i++)
                {
                    wrtiebuf[i - 1] = Convert.ToByte(input[i], 16);
                }
                controller.GetDevice(settings).Write(wrtiebuf);
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }
        static async Task spiread(string[] input)
        {
            try
            {
                if (input.Length == 2)
                {
                    SpiConnectionSettings settings = new SpiConnectionSettings(spi.ChipSelectLine);
                    settings.ClockFrequency = spi.ClockFrequency;
                    settings.DataBitLength = spi.DataBitLength;
                    settings.Mode = spi.Mode;
                    settings.SharingMode = spi.SharingMode;
                    byte[] readbuf = new byte[Convert.ToInt32(input[1])];
                    controller.GetDevice(settings).Read(readbuf);
                    for (int i = 0; i < readbuf.Length; i++)
                    {
                        Console.WriteLine(i + " byte: " + readbuf[i].ToString("X"));
                    }
                }
                else
                {
                    Console.WriteLine("please input : read N");
                }

            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }
        static async Task spiwriteread(string[] input)
        {
            try
            {
                SpiConnectionSettings settings = new SpiConnectionSettings(spi.ChipSelectLine);
                settings.ClockFrequency = spi.ClockFrequency;
                settings.DataBitLength = spi.DataBitLength;
                settings.Mode = spi.Mode;
                settings.SharingMode = spi.SharingMode;
                byte[] wrtiebuf = new byte[input.Length - 2];
                for (int i = 1; i <= input.Length - 2; i++)
                {
                    wrtiebuf[i - 1] = Convert.ToByte(input[i], 16);
                }
                byte[] readbuf = new byte[Convert.ToInt32(input[input.Length - 1])];
                controller.GetDevice(settings).TransferSequential(wrtiebuf, readbuf);
                for (int i = 0; i < readbuf.Length; i++)
                {
                    Console.WriteLine(i + " byte: " + readbuf[i].ToString("X"));
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }
        static async Task spifullduplex(string[] input)
        {
            try
            {
                SpiConnectionSettings settings = new SpiConnectionSettings(spi.ChipSelectLine);
                settings.ClockFrequency = spi.ClockFrequency;
                settings.DataBitLength = spi.DataBitLength;
                settings.Mode = spi.Mode;
                settings.SharingMode = spi.SharingMode;
                byte[] wrtiebuf = new byte[input.Length - 1];
                byte[] readbuf = new byte[wrtiebuf.Length];
                for (int i = 1; i < input.Length; i++)
                {
                    wrtiebuf[i - 1] = Convert.ToByte(input[i], 16);
                }
                controller.GetDevice(settings).TransferFullDuplex(wrtiebuf, readbuf);
                for (int i = 0; i < readbuf.Length; i++)
                {
                    Console.WriteLine(i + " byte: " + readbuf[i].ToString("X"));
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }

        static void Main(string[] args)
        {
            string input = "";
            bool exit = input.Equals("exit");
            Console.WriteLine("UWP console SPI test:");

            UpNetBridge upb = new UpNetBridge();
            Console.WriteLine(upb.getmanufacturename() + "\n"
                        + "Board Name:  " + upb.getboardname() + "\n"
                        + "BIOS Ver:    " + upb.getbiosname() + "\n"
                        + "Firmware Ver:" + upb.getfirmwarename() + "\n");

            Console.WriteLine(Usage);
            spi.ChipSelectLine = 0;
            spi.ClockFrequency = 8000000;
            spi.DataBitLength = 8;
            spi.Mode = 0;
            spi.SharingMode = 0;
            while (exit == false)
            {
                Console.Write(">");
                input = Console.ReadLine();
                string[] inputnum = input.Split(' ');
                switch (inputnum[0].ToLower())
                {
                    case "set":
                        spiset();
                        break;
                    case "write":
                        spiwrite(inputnum).Wait();
                        break;
                    case "read":
                        spiread(inputnum).Wait();
                        break;
                    case "writeread":
                        spiwriteread(inputnum).Wait();
                        break;
                    case "fullduplex":
                        spifullduplex(inputnum).Wait();
                        break;
                    case "info":
                        Console.WriteLine("ChipSelectLine   :   " + spi.ChipSelectLine + "\n");
                        Console.WriteLine("ClockFrequency   :   " + spi.ClockFrequency + "\n");
                        Console.WriteLine("DataBitLength    :   " + spi.DataBitLength + "\n");
                        Console.WriteLine("Mode             :   " + spi.Mode + "\n");
                        Console.WriteLine("SharingMode      :   " + spi.SharingMode + "\n");
                        break;
                    case "exit":
                        exit = inputnum[0].Equals("exit");
                        break;
                    case "help":
                    default:
                        Console.WriteLine(Usage);
                        break;
                }
            }
        }
    }
}
