using System;
using Windows.Devices.Adc;
using System.Threading.Tasks;
using Windows.Devices.I2c;
// This example code shows how you could implement the required main function for a 
// Console UWP Application. You can replace all the code inside Main with your own custom code.

// You should also change the Alias value in the AppExecutionAlias Extension in the 
// Package.appxmanifest to a value that you define. To edit this file manually, right-click
// it in Solution Explorer and select View Code, or open it with the XML Editor.

using NetUp.NetADC;
using UpNetlib;

namespace UpAdcTestTool
{
    class Program
    {
        static int adcmax = 0;
        static string Usage =
         "UpAdcTestTool: Command line Adc testing utility\n" +
         "commands:\n" +
         "\n" +
         " read {adc number}         Read adc number value\n" +
         " max                       adc max value\n" +
         " min                       adc min value\n" +
         " count                     adc controller count\n" +
         " exit                      exit adc test\n" +
         "\n";
        static AdcController controller;
        static async Task controllerinit()
        {
            try
            {
                var list = await AdcController.GetControllersAsync(UPAdcProvider.Instance);

                controller = list[0];
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }
        static async Task adc(int channelint)
        {
            try
            {
                
                AdcChannel channel = controller.OpenChannel(channelint);
                Console.WriteLine(channel.ReadValue());
                channel.Dispose();
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }
        static async Task adcnum()
        {
            try
            {
                adcmax = controller.ChannelCount;
                Console.WriteLine("Get  " + adcmax + "  controller");
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }
        static async Task adcmaxvalue()
        {
            try
            {
                Console.WriteLine("Max Value    :  " + controller.MaxValue);
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }

        static async Task adcminvalue()
        {
            try
            {
                Console.WriteLine("Min Value    :  " + controller.MinValue);
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }


        static void Main(string[] args)
        {
            string input = "";
            bool exit = true;


            UpNetBridge upb = new UpNetBridge();

            Console.WriteLine(upb.getmanufacturename() + "\n"
                        + "Board Name:  " + upb.getboardname() + "\n"
                        + "BIOS Ver:    " + upb.getbiosname() + "\n"
                        + "Firmware Ver:" + upb.getfirmwarename() + "\n");
            controllerinit();

            Console.WriteLine("UWP console ADC test:");

            Console.WriteLine(Usage);
            adcnum().Wait();
            while (exit)
            {
                Console.Write(">");
                input = Console.ReadLine();
                string[] inputnum = input.Split(' ');
                switch (inputnum[0])
                {
                    case "read":
                        int index = Convert.ToInt32(inputnum[1]);
                        if (index < adcmax)
                        {
                            Console.WriteLine("select " + inputnum[1]);
                            adc(index).Wait();
                        }
                        else
                        {
                            Console.WriteLine("Not Get " + index + " controller");
                            break;
                        }
                        break;
                    case "count":
                        adcnum().Wait();
                        break;
                    case "max":
                        adcmaxvalue().Wait();
                        break;
                    case "min":
                        adcminvalue().Wait();
                        break;
                    case "exit":
                        exit = false;

                        break;
                    default:
                        break;
                }
            }


        }
    }
}
