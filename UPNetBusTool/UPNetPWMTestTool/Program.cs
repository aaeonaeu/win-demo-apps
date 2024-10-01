using System;
using System.Threading.Tasks;
using UpNetlib;
using NetUp.NetPWM;
using Windows.Devices.I2c;
using Windows.Devices.Pwm;

// This example code shows how you could implement the required main function for a 
// Console UWP Application. You can replace all the code inside Main with your own custom code.

// You should also change the Alias value in the AppExecutionAlias Extension in the 
// Package.appxmanifest to a value that you define. To edit this file manually, right-click
// it in Solution Explorer and select View Code, or open it with the XML Editor.

namespace UpPwmTestTool
{
    class Program
    {
        static string Usage =
          "UpPwmTestTool: Command line PWM testing utility\n" +
          "commands:\n" +
          "\n" +
          "  list           List the available address on the default PWM controller.\n" +
          "  set            Set Pwm Pin, default:\n" +
          "  get            Get PWM Info\n" +
          "  frequency      Set Pwm Pin frequency.....type:Double\n" +
          "  duty           Set Duty to Pwm pin...type:Double\n" +
          "  help           show commands\n" +
          "  Example:       %s> <commands>\n" +
          "  exit           exit PWM test\n" +
          "\n";

        struct PinStatus
        {
            public int pin;
            public double pin_ActualFrequency;
            public double pin_DutyCycle;
        };

        static PinStatus pin1;
        static PwmController controller = null;
        static async Task controllerinit()
        {
            try
            {
                var list = await PwmController.GetControllersAsync(UPPWMProvider.Instance);

                controller = list[0];
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }
        static async void pwmlist()
        {
            try
            {
                Console.WriteLine(controller.PinCount);
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }
        static void Main(string[] args)
        {
            var t = Task.Run(() => MainAsync());
            t.Wait();
        }
        static async Task MainAsync()
        {
            try
            {
                string input = "";
                bool exit = input.Equals("exit");

                UpNetBridge upb = new UpNetBridge();
                Console.WriteLine(upb.getmanufacturename() + "\n"
                            + "Board Name:  " + upb.getboardname() + "\n"
                            + "BIOS Ver:    " + upb.getbiosname() + "\n"
                            + "Firmware Ver:" + upb.getfirmwarename() + "\n");

                Console.WriteLine("UWP console PWM test:");
                controllerinit();

                pin1.pin = -1;
                PwmPin pin = null;

                Console.WriteLine(Usage);
                while (exit == false)
                {
                    Console.Write(pin1.pin.ToString() + ">");
                    input = Console.ReadLine();
                    string[] inputnum = input.Split(' ');
                    switch (inputnum[0].ToLower())
                    {
                        case "list":
                            pwmlist();
                            break;
                        case "set":
                            if (inputnum.Length == 2)
                            {
                                int pin_convert;
                                if (int.TryParse(inputnum[1], out pin_convert))
                                {
                                    if (pin_convert == pin1.pin)
                                    {
                                        Console.WriteLine("This Pin is currently setting");
                                    }
                                    else
                                    {
                                        if (pin1.pin != -1)
                                        {
                                            pin.Dispose();
                                        }
                                        pin1.pin = pin_convert;
                                        try
                                        {

                                            pin = controller.OpenPin(pin1.pin);
                                            pin.Start();
                                            Console.WriteLine("You select pin   " + pin1.pin + "   to set");
                                        }
                                        catch (Exception e)
                                        {
                                            pin1.pin = -1;
                                            Console.WriteLine(e.Message);
                                        }
                                    }
                                }
                                else
                                {
                                    Console.WriteLine("Please input : set {int}");
                                }
                            }
                            else
                            {
                                Console.WriteLine("Please input : set {int}");
                            }
                            break;
                        case "get":
                            try
                            {
                                if (inputnum.Length == 1)
                                {
                                    Console.WriteLine("Pin  " + pin1.pin + "\n");
                                    Console.WriteLine("Max Frequency    :   " + controller.MaxFrequency + "\n");
                                    Console.WriteLine("Min Frequency    :   " + controller.MinFrequency + "\n");
                                    Console.WriteLine("Actual Frequency :   " + controller.ActualFrequency + "\n");
                                    Console.WriteLine("Duty Cycle       :   " + pin.GetActiveDutyCyclePercentage() + "\n");
                                    Console.WriteLine("Pin Status       :   " + pin.IsStarted + "\n");
                                }
                                else
                                {
                                    Console.WriteLine("Please input : get");

                                }
                            }
                            catch (Exception e)
                            {
                                Console.WriteLine(e.Message);
                            }
                            break;
                        case "frequency":
                            if (inputnum.Length == 2)
                            {
                                try
                                {
                                    if (double.TryParse(inputnum[1], out pin1.pin_ActualFrequency))
                                    {
                                        Console.WriteLine("Frequency set : " + pin1.pin_ActualFrequency);
                                        controller.SetDesiredFrequency(pin1.pin_ActualFrequency);
                                        pin.Start();
                                    }
                                    else
                                    {
                                        Console.WriteLine("Please input : frequency {double}");
                                    }
                                }
                                catch (Exception e)
                                {
                                    Console.WriteLine(e.Message);
                                }

                            }
                            else
                            {
                                Console.WriteLine("Please input : frequency {double}");
                            }
                            break;
                        case "duty":
                            if (inputnum.Length == 2)
                            {
                                try
                                {
                                    if (double.TryParse(inputnum[1], out pin1.pin_DutyCycle))
                                    {
                                        Console.WriteLine("duty set : " + pin1.pin_DutyCycle);
                                        pin.SetActiveDutyCyclePercentage(pin1.pin_DutyCycle);
                                        pin.Start();
                                    }
                                    else
                                    {
                                        Console.WriteLine("Please input : duty {double}");
                                    }
                                }
                                catch (Exception e)
                                {
                                    Console.WriteLine(e.Message);
                                }
                            }
                            else
                            {
                                Console.WriteLine("Please input : duty {double}");
                            }
                            break;
                        case "exit":
                            exit = inputnum[0].Equals("exit");
                            pin.Dispose();
                            break;
                        case "help":
                        default:
                            Console.WriteLine(Usage);
                            break;
                    }
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }

        }
    }
}
