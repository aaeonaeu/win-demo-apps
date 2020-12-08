// Copyright (c) AAEON. All rights reserved.
//
using System;
using Windows.Devices.Gpio;

namespace UpGpioTestTool
{
    class Program
    {
        static string Usage =
        "UpGpioTestTool: Command line GPIO testing utility\n" +
        "commands:\n" +
        "\n" +
        "  list         List the available pins on the default GPIO controller.\n" +
        "  pin %s       select pin to control\n" +
        "  status       status of select pin(0 is output/1 is input)\n" +
        "  input        set select pin as input\n" +
        "  output       set select pin as output\n" +
        "  high         set select pin to high\n" +
        "  low          set select pin to low\n" +
        "  read         read select pin value(0 is low/1 is high)\n" +
        "  help         show commands\n" +
        "  Example:     %s> <commands>\n" +
        "  %s>pin 7     \n" +
        "  7>output     \n" +
        "  7>high     \n" +
       "\n";

        static void Main(string[] args)
        {
            UpBridge.Up upb = new UpBridge.Up();
            
            Console.WriteLine(upb.BoardGetManufacture() + "\n"
                        + "Board Name:  " + upb.BoardGetName() + "\n"
                        + "BIOS Ver:    " + upb.BoardGetBIOSVersion() + "\n"
                        + "Firmware Ver:" + upb.BoardGetFirmwareVersion().ToString("X") + "\n");

            Console.WriteLine("Up UWP console GPIO test:");

            if (GpioController.GetDefault().PinCount > 0)
            {
                GpioPin gpioPin=null;
                int selpin = -1;
                while (true)
                {
                    string input;
                    if (selpin == -1)
                    {
                        Console.WriteLine("please select pin to control(pin %s)");
                    }
                    Console.Write(selpin.ToString() + ">");
                    input = Console.ReadLine();
                    string[] inArgs = input.Split(' ');
                    if (inArgs[0] == "pin")
                    {
                        if (inArgs.Length == 2)
                        {
                            selpin = int.Parse(inArgs[1]);
                            try
                            {
                                gpioPin = GpioController.GetDefault().OpenPin(selpin);
                            }catch(InvalidOperationException ie)
                            {
                                Console.WriteLine(ie.Message);
                                selpin = -1;
                            }
                        }
                        continue;
                    }

                    switch (input)
                    {
                        case "status":
                            Console.WriteLine(gpioPin.GetDriveMode().ToString());
                            break;
                        case "input":
                            gpioPin.SetDriveMode(GpioPinDriveMode.Input);
                            Console.WriteLine(gpioPin.GetDriveMode().ToString());
                            break;
                        case "output":
                            gpioPin.SetDriveMode(GpioPinDriveMode.Output);
                            Console.WriteLine(gpioPin.GetDriveMode().ToString());
                            break;
                        case "high":
                            gpioPin.Write(GpioPinValue.High);
                            Console.WriteLine(gpioPin.Read().ToString());
                            break;
                        case "low":
                            gpioPin.Write(GpioPinValue.Low);
                            Console.WriteLine(gpioPin.Read().ToString());
                            break;
                        case "read":
                            Console.WriteLine(gpioPin.Read().ToString());
                            break;
                        case "list":
                            Console.WriteLine("Available Pins:"+GpioController.GetDefault().PinCount.ToString() + " (start from 0)");
                            break;
                        case "help":
                        default:
                            Console.WriteLine(Usage);
                            break;

                    }
                }
            }
            else
            {
                Console.WriteLine("No available GPIO pins!");
                Console.WriteLine("Press any key to exit...");
                Console.ReadLine();
            }
        }
    }
}
