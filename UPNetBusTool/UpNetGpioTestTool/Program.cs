// Copyright (c) AAEON. All rights reserved.
//
using System;
using System.Diagnostics;
using System.Threading;
using System.Collections.Generic;
using Windows.Devices.Gpio;
using Windows.Devices.I2c;

using UpNetlib;
using NetUp.NetGPIO;

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
        "  pollout      set select pin polling output hi/lo in 1ms interval\n" +
        "  irq          display interrupt event from select pin\n" +
        "  help         show commands\n" +
        "  Example:     %s> <commands>\n" +
        "  %s>pin 7     \n" +
        "  7>output     \n" +
        "  7>high     \n" +
       "\n";

        //interrupt testing variable 
        public struct GpioState
        {
            public int tt { get; set; } //tick time
            public GpioPinValue pv { get; set; }
            public GpioPinEdge edge { get; set; }
        }
        static GpioController controller;
        static GpioState gs = new GpioState();
        static List<GpioState> lgs = new List<GpioState>();
        static int rising, falling;
        static int record_intpin;
        static async Task controllerinit()
        {
            try
            {
                var list = await GpioController.GetControllersAsync(UPGpioProvider.Instance);

                controller = list[0];
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }

        static void Main(string[] args)
        {
            if(GpioController.GetDefault()==null)
            {
                try
                {
                    UpNetBridge upb = new UpNetBridge();
                    Console.WriteLine(upb.getmanufacturename() + "\n"
                                + "Board Name:  " + upb.getboardname() + "\n"
                                + "BIOS Ver:    " + upb.getbiosname() + "\n"
                                + "Firmware Ver:" + upb.getfirmwarename() + "\n");
                    controllerinit().Wait();
                }
                catch (InvalidOperationException ie)
                {
                    Console.WriteLine(ie.Message);
                }

            }

            Console.WriteLine("Up UWP console GPIO test:");
            int pin_cnt = 0;
            if (GpioController.GetDefault() == null)
            {
                pin_cnt = controller.PinCount;
            }
            else
            {
                pin_cnt = GpioController.GetDefault().PinCount;
            }

            if (pin_cnt > 0)
            {
                GpioPin gpioPin=null;
                GpioPin[] igpio = new GpioPin[28];
                int[] gindex = new int[28] {3,5,7,8,10,11,12,13,15,16,18,19,21,22,23,24,26,27,28,29,31,32,33,35,36,37,38,40};
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
                                gpioPin = controller.OpenPin(selpin,GpioSharingMode.SharedReadOnly);
                            } catch (InvalidOperationException ie)
                            {
                                Console.WriteLine(ie.Message);
                                selpin = -1;
                            }
                        }
                        continue;
                    }
                    else if (inArgs[0] == "test")
                    {
                        
                        for (int index = 0; index < gindex.Length; index++)
                        {
                            try
                            {
                                igpio[index] = controller.OpenPin(gindex[index]-1, GpioSharingMode.SharedReadOnly);
                                igpio[index].SetDriveMode(GpioPinDriveMode.Input);
                                igpio[index].ValueChanged += UpGpioIntTest;
                                sleep(100);
                            }
                            catch (InvalidOperationException ie)
                            {
                                Console.WriteLine(ie.Message);
                            }
                        }
                        Timer t;
                        t = new Timer(new TimerCallback(UpGpioIrqProc));
                        t.Change(1000, 1000); //1sec interval to count 
                        Console.ReadLine();

                        for (int index = 0; index < gindex.Length; index++)
                        {
                            try
                            {
                                igpio[index].ValueChanged -= UpGpioIntTest;
                            }
                            catch (InvalidOperationException ie)
                            {
                                Console.WriteLine(ie.Message);
                            }
                        }

                        t.Dispose();

                    }

                    switch (input)
                    {
                        case "pollout":
                            string count;
                            string time;
                            Console.WriteLine("Enter polling test count in pin {0}...", selpin);
                            count=Console.ReadLine();
                            Console.WriteLine("Enter polling interval time in pin {0}...", selpin);
                            time = Console.ReadLine();

                            Console.WriteLine("polling output in pin {0}...", selpin);
                            gpioPin.SetDriveMode(GpioPinDriveMode.Output);
                            gpioPin.Write(GpioPinValue.High);
                            int s = Environment.TickCount;
                            for (int i=0;i< int.Parse(count); i++)
                            {
                                gpioPin.Write(GpioPinValue.Low);
                                sleep(int.Parse(time));
                                gpioPin.Write(GpioPinValue.High);
                                sleep(int.Parse(time));
                            }
                            Console.WriteLine("polling output...{0} times completed!!, total time:{1} s", count, (Environment.TickCount-s)/1000);
                            break;
                        case "irq":
                            Timer t;
                            Console.WriteLine("irq testing...please enter to exit");
                            gpioPin.SetDriveMode(GpioPinDriveMode.Input);
                            gpioPin.ValueChanged += UpGpioValueChanged;
                            t = new Timer(new TimerCallback(UpGpioIrqProc));
                            t.Change(1000, 1000); //1sec interval to count 
                            Console.ReadLine();
                            rising = falling = 0;
                            gpioPin.ValueChanged -= UpGpioValueChanged;
                            t.Dispose();
                            break;
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

        static Stopwatch sw = Stopwatch.StartNew();
        private static void sleep(double ms)
        {
            sw.Start();
            while (true)
            {
                if (sw.Elapsed.TotalMilliseconds >= ms)
                    break;
                Thread.Sleep(0);
            }
            sw.Stop();
            sw.Reset();
        }
        private static void UpGpioIrqProc(object state)
        {
            //int i, j;
            //for(i = 0;i < lgs.Count; i++)
            //{
            //    if (lgs[i].edge == GpioPinEdge.FallingEdge)
            //        falling++;
            //    if (lgs[i].edge == GpioPinEdge.RisingEdge)
            //        rising++;
            //}
            //for (j = 0; j < i; j++)
            //    lgs.RemoveAt(0);

            Console.Write("\rRising: {0}, falling: {1}", rising, falling);
        }

        private static void UpGpioValueChanged(GpioPin sender, GpioPinValueChangedEventArgs args)
        {
            //gs.edge = args.Edge;
            //gs.tt = Environment.TickCount; //ms
            //lgs.Add(gs);
            switch (args.Edge)
            {
                case GpioPinEdge.FallingEdge:
                    {
                        falling++;
                        break;
                    }
                case GpioPinEdge.RisingEdge:
                    {
                        rising++;
                        break;
                    }
                default:
                    break;
            }
            GC.Collect();

            //Console.Write("\rRising: {0}, falling: {1}", rising, falling);
        }
        private static void UpGpioIntTest(GpioPin sender, GpioPinValueChangedEventArgs args)
        {
            if(record_intpin!= sender.PinNumber)
            {
                Console.WriteLine();
                Console.WriteLine("Intterupt pin occurr change {0} to {1}", record_intpin, sender.PinNumber);
                record_intpin = sender.PinNumber;
                rising = falling = 0;
            }
            switch (args.Edge)
            {
                case GpioPinEdge.FallingEdge:
                    {
                        falling++;
                        break;
                    }
                case GpioPinEdge.RisingEdge:
                    {
                        rising++;
                        break;
                    }
                default:
                    break;
            }

        }
    }
}
