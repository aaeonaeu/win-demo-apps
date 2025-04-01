using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace UPWdtTestTool
{
    class Program
    {
        static UpBridge.Up upb = new UpBridge.Up();
        static string Usage =
          "UPWdtTestTool: Command line WDT testing utility\n" +
          "commands:\n" +
          "\n" +
          "  start <timeout>  - Start the watchdog with the specified timeout in seconds\n" +
          "  stop             - Stop the watchdog\n" +
          "  reload           - Reload the watchdog\n" +
          "  getTimeout       - Get the current watchdog timeout\n" +
          "  exit             - exit I2C test\n" +
          "\n";
        static void start(string[] input)
        {
            if (input.Length == 2)
            {
                int timeout = Convert.ToInt32(input[1], 10);
                if (timeout > 1024 || timeout < 1)
                {
                    Console.WriteLine("Current watchdog timeout support 1-1024 second");

                }
                else
                {
                    upb.WDTStart((uint)timeout);
                    Console.WriteLine("Current watchdog start ,set timeout :"+ timeout);
                }
            }
            else
            {
                Console.WriteLine("Invaild input");
            }
        }
        static void stop()
        {
            upb.WDTStop();
            Console.WriteLine("Current watchdog stop");
        }
        static void reload()
        {
            upb.WDTReload();
            Console.WriteLine("Current watchdog reload");
        }
        static void getTimeout()
        {
            upb.WDTGet(out uint timeout);
            Console.WriteLine("Current watchdog timeout: "+timeout);
        }
        static void Main(string[] args)
        {
            string input = "";
            bool exit = input.Equals("exit");
            
            Console.WriteLine("UWP console WDT test:");

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
                    case "start":
                        start(inputnum);
                        break;
                    case "stop":
                        stop();
                        break;
                    case "reload":
                        reload();
                        break;
                    case "getTimeout":
                        getTimeout();
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
