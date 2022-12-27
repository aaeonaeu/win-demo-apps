using System;
using Windows.Devices.I2c;
using System.Threading.Tasks;
// This example code shows how you could implement the required main function for a 
// Console UWP Application. You can replace all the code inside Main with your own custom code.

// You should also change the Alias value in the AppExecutionAlias Extension in the 
// Package.appxmanifest to a value that you define. To edit this file manually, right-click
// it in Solution Explorer and select View Code, or open it with the XML Editor.

namespace UpI2cTestTool
{
    class Program
    {
        static string Usage =
          "UpI2CTestTool: Command line I2C testing utility\n" +
          "commands:\n" +
          "\n" +
          "  i2cdetect    List the available address on the default I2C controller.\n" +
          "  i2cdump      dump I2C address data\n" +
          "  i2cset       set I2C address data\n" +
          "  i2cget       get I2C address data\n" +
          "  StandardMode set I2C bus speed\n" +
          "  FastMode     set I2C bus speed\n" +
          "  help         show commands\n" +
          "  Example:     %s> <commands>\n" +
          "  exit         exit I2C test\n" +
          "\n";

        static I2cBusSpeed[] speed= { I2cBusSpeed.StandardMode, I2cBusSpeed.StandardMode } ;

        static async Task i2cdetect(string[] input)
        {               
            try
             {
            if(input.Length==2)
            {
                UpBridge.Up upb = new UpBridge.Up();
                int index = Int32.Parse(input[1]);
                if(index>2)
                {
                        Console.WriteLine("i2c have 0 & 1");
                        throw new InvalidOperationException("i2c have 0 & 1");                      
                }
                I2cController controller = (await I2cController.GetControllersAsync(UpBridge.UpI2cProvider.GetI2cProvider()))[index];
                I2cConnectionSettings Settings = new I2cConnectionSettings(0x00);
                Settings.BusSpeed = speed[index];
                Console.WriteLine("     0    1   2   3   4   5   6   7   8   9   a   b   c   d   e   f");
                byte[] writebuf = { 0x00 };
                byte[] readbuf = new byte[1];
                for (uint i = 0; i < 256; i += 16)
                {
                    if (i == 0)
                    {
                        Console.Write("00:  ");
                    }
                    else
                    {
                        Console.Write(Convert.ToString(i, 16) + ":  ");
                    }
                    for (uint j = 0; j < 16; j++)
                    {
                        Settings.SlaveAddress = (int)(i + j);
                        try
                        {
                            controller.GetDevice(Settings).WriteRead(writebuf,readbuf);
                            if (i + j < 0x10)
                            {
                                Console.Write("0" + Convert.ToString(i + j, 16) + "  ");
                            }
                            else
                            {
                                Console.Write(Convert.ToString(i + j, 16) + "  ");
                            }
                        }
                        catch (Exception e)
                        {
                            Console.Write("--" + "  ");
                        }
                    }
                    Console.Write("\n");
                }
            }
            else
            {
                Console.WriteLine("command error,plese refer to below example \n" + "i2cdetect {busnumber}\n");
                Console.WriteLine(Usage);
            }
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }

        static async Task i2cdump(string[] input)
        {
            if (input.Length == 3)
            {
                int slave= Convert.ToInt32(input[2], 16);
                UpBridge.Up upb = new UpBridge.Up();
                int index = Int32.Parse(input[1]);
                if (index > 2)
                {
                    Console.WriteLine("i2c have 0 & 1");
                    throw new InvalidOperationException("i2c have 0 & 1");
                }
                I2cController controller = (await I2cController.GetControllersAsync(UpBridge.UpI2cProvider.GetI2cProvider()))[index];
                I2cConnectionSettings Settings = new I2cConnectionSettings(slave);
                Settings.BusSpeed = speed[index];
                Console.WriteLine("     0    1   2   3   4   5   6   7   8   9   a   b   c   d   e   f");
                byte[] writebuf = new byte[1];
                byte[] readbuf = new byte[1];
                try
                {
                    for (uint i = 0; i < 256; i += 16)
                    {
                        if (i == 0)
                        {
                            Console.Write("00:  ");
                        }
                        else
                        {
                            Console.Write(Convert.ToString(i, 16) + ":  ");

                        }
                        for (uint j = 0; j < 16; j++)
                        {
                            writebuf[0] = (byte)(i + j);
                            controller.GetDevice(Settings).WriteRead(writebuf, readbuf);
                            if (readbuf[0] < 0x10)
                                Console.Write("0" + Convert.ToString(readbuf[0], 16) + "  ");
                            else
                                Console.Write(Convert.ToString(readbuf[0], 16) + "  ");  
                        }
                        Console.Write("\n");
                    }
                }
                catch(Exception)
                {
                    Console.Write("NOT to read Data" + "\n");
                }

            }
            else
            {
                Console.WriteLine("command error,plese refer to below example \n" +
                     "i2cdump {bus number} {i2c address}\n");
                Console.WriteLine(Usage);
            }
        }
        static async Task i2cset(string[] input)
        {
            if (input.Length == 5)
            {
                int slave = Convert.ToInt32(input[2], 16);
                UpBridge.Up upb = new UpBridge.Up();
                int index = Int32.Parse(input[1]);
                if (index > 2)
                {
                    Console.WriteLine("i2c have 0 & 1");
                    throw new InvalidOperationException("i2c have 0 & 1");
                }
                I2cController controller = (await I2cController.GetControllersAsync(UpBridge.UpI2cProvider.GetI2cProvider()))[index];

                I2cConnectionSettings Settings = new I2cConnectionSettings(slave);
                Settings.BusSpeed = speed[index];
                byte[] writebuf = new byte[2];
                writebuf[0] = Convert.ToByte(input[3],16);
                writebuf[1] = Convert.ToByte(input[4],16);

                try
                {
                    controller.GetDevice(Settings).Write(writebuf);
                    Console.WriteLine("Sucess to set data,\n");
                }
                catch (Exception e)
                {
                    Console.WriteLine("error to set data\n");
                }

            }
            else
            {
                Console.WriteLine("command error,plese refer to below example \n" +
                                    "i2cset {bus number} {i2c address} {i2c register} {i2cdata}\n");
                Console.WriteLine(Usage);
            }
        }
        static async Task i2cget(string[] input)
        {
            if (input.Length == 4)
            {
                int slave = Convert.ToInt32(input[2], 16);
                UpBridge.Up upb = new UpBridge.Up();
                int index = Int32.Parse(input[1]);
                if (index > 2)
                {
                    Console.WriteLine("i2c have 0 & 1");
                    throw new InvalidOperationException("i2c have 0 & 1");
                }
                I2cController controller = (await I2cController.GetControllersAsync(UpBridge.UpI2cProvider.GetI2cProvider()))[index];
                I2cConnectionSettings Settings = new I2cConnectionSettings(slave);
                Settings.BusSpeed = speed[index];
                byte[] writebuf = new byte[1];
                writebuf[0] = Convert.ToByte(input[3],16);
                byte[] readbuf = new byte[1];
                try
                {
                    controller.GetDevice(Settings).WriteRead(writebuf, readbuf);
                    Console.WriteLine("Sucess to get data,\n" +
                    Convert.ToString(readbuf[0], 16));
                }
                catch (Exception e)
                {
                    Console.WriteLine("error to get data\n");
                }

            }
            else
            {
                Console.WriteLine("command error,plese refer to below example \n" +
                                    "i2cget {bus number} {i2c address} {i2c register}\n");
                Console.WriteLine(Usage);
            }
        }
        static void i2cspeed(string[] input)
        {
            try
            {
                if (input.Length == 2)
                {
                    int index = Int32.Parse(input[1]);
                    if (index > 2)
                    {
                        Console.WriteLine("i2c have 0 & 1");
                        throw new InvalidOperationException("i2c have 0 & 1");
                    }
                    if (input[0] == "StandardMode")
                        speed[index] = I2cBusSpeed.StandardMode;
                    else if (input[0] == "FastMode")
                        speed[index] = I2cBusSpeed.FastMode;
                    else
                        Console.WriteLine("StandardMode {bus number} or FastMode  {bus number}");
                }
                else
                {
                    Console.WriteLine("StandardMode {bus number} or FastMode  {bus number}");
                }
            }
            catch (Exception)
            {
                Console.Write("NOT to set speed" + "\n");
            }
            
        }
        static void Main(string[] args)
        {
            string input = "";
            bool exit = input.Equals("exit");
            UpBridge.Up upb = new UpBridge.Up();
            Console.WriteLine("UWP console I2C test:");

            //print Board INFO
            Console.WriteLine(upb.BoardGetManufacture() + "\n"
                        + "Board Name:  " + upb.BoardGetName() + "\n"
                        + "BIOS Ver:    " + upb.BoardGetBIOSVersion() + "\n"
                        + "Firmware Ver:" + upb.BoardGetFirmwareVersion().ToString("X") + "\n");

            Console.WriteLine(Usage);
            while (exit == false)
            {
                Console.Write(">");
                input = Console.ReadLine();
                string[] inputnum = input.Split(' ');
                switch (inputnum[0])
                {
                    case "i2cdetect":
                        i2cdetect(inputnum).Wait();
                        break;
                    case "i2cdump":
                        i2cdump(inputnum).Wait();
                        break;
                    case "i2cset":
                        i2cset(inputnum).Wait();
                        break;
                    case "i2cget":
                        i2cget(inputnum).Wait();
                        break;
                    case "StandardMode":
                    case "FastMode":
                        i2cspeed(inputnum);
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
