using System;
using System.Reflection;
using System.Threading.Tasks;
using Windows.UI;
using Windows.Devices.Lights;

// This example code shows how you could implement the required main function for a 
// Console UWP Application. You can replace all the code inside Main with your own custom code.

// You should also change the Alias value in the AppExecutionAlias Extension in the 
// Package.appxmanifest to a value that you define. To edit this file manually, right-click
// it in Solution Explorer and select View Code, or open it with the XML Editor.

namespace UpLedTestTool
{

    class Program
    {
        static byte[] ledpin =new byte[8]{
            0,      //LED 0
            3,      //LED 1
            6,      //LED 2
            9,      //LED 3
            12,     //LED 4
            15,     //LED 5
            18,     //LED 6
            21      //LED 7
        };

        static string Usage =
        "UpLedTestTool: Command line LEDs on/off testing utility\n" +
        "UP/UPX Board: yellow, green, red \n" +
        "UP2/APL03/EHL Board: blue, yellow, green, red \n" +
        "NUC-AMR Board: 0,1,2,3,4,5,6,7 \n" +
        "commands:\n" +
        "\n" +
        "  <color> on     set select Led to on\n" +
        "  <color> off    set select Led to off\n" +
        "  help           show commands\n" +
        "  Example:       LEDs> <color> on/off \n" +
        "  LEDs>yellow on \n" +
        "  LEDs>green on  \n" +
        "  LEDs>blue on   \n" +
        "  LEDs>red on    \n" +
        "  LEDs>{led pin} on \n" +
        "\n";

        static Lamp lamp=null;

        static async void Lamp_AvailabilityChanged(Lamp sender, LampAvailabilityChangedEventArgs args)
        {
            Console.WriteLine("{0} {1}", sender.Color.ToString(), args.IsAvailable.ToString());
        }

        static async Task LedSetting(string led,string cmd)
        {
            if(lamp==null)
                lamp = await Lamp.GetDefaultAsync();
            Console.WriteLine("Lamp ID:{0}", lamp.DeviceId.ToString());

            if (lamp != null)
            {
                bool result;
                int led_pin;
                result = Int32.TryParse(led,out led_pin);
                if (result)
                {
                    //for up have to set color every time then you can trun on/off correctly

                    lamp.Color = Color.FromArgb(    0,  0,   0, ledpin[led_pin]);

                        if (cmd.ToLower().Contains("on"))
                        {
                            lamp.IsEnabled = true;
                        }
                        else if (cmd.ToLower().Contains("off"))
                        {
                            lamp.IsEnabled = false;
                        }
                        else if (cmd.ToLower().Contains("get"))
                        {
                            Console.WriteLine("{0} LED is {1}", led, lamp.IsEnabled ? "on" : "off");
                        }
                        else
                        {
                            Console.WriteLine("unknow command {0}", cmd);
                            return;
                        }
                        Console.WriteLine("set {0} {1}", lamp.Color.ToString(), cmd);

                }
                else
                {
                    //compare Led color
                    foreach (var color in typeof(Colors).GetProperties())
                    {
                        if (led.ToLower().Contains(color.Name.ToLower()))
                        {
                            //for up have to set color every time then you can trun on/off correctly
                            lamp.Color = (Color)color.GetValue(new Color(), null);
                            if (cmd.ToLower().Contains("on"))
                            {
                                lamp.IsEnabled = true;
                            }
                            else if (cmd.ToLower().Contains("off"))
                            {
                                lamp.IsEnabled = false;
                            }
                            else if (cmd.ToLower().Contains("get"))
                            {
                                Console.WriteLine("{0} LED is {1}", led, lamp.IsEnabled ? "on" : "off");
                            }
                            else
                            {
                                Console.WriteLine("unknow command {0}", cmd);
                                return;
                            }
                            Console.WriteLine("set {0} {1}", lamp.Color.ToString(), cmd);
                        }
                    }
                }

            }
        }

        static void Main(string[] args)
        {
            while (true)
            {
                string input;
                Console.Write("LEDs" + ">");
                input = Console.ReadLine();
                string[] inArgs = input.Split(' ');
                switch (inArgs[0])
                {
                    case "help":
                        Console.WriteLine(Usage);
                        break;
                    default:
                        if (inArgs.Length > 1)
                        {
                            LedSetting(inArgs[0], inArgs[1]).Wait();
                        }
                        else if (inArgs.Length == 1)
                        {
                            LedSetting(inArgs[0], "get").Wait();
                        }
                        else
                            Console.WriteLine(Usage);
                        break;
                }

            }
        }
    }
}
