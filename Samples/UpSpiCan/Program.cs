using System;
using System.Globalization;
using System.Linq;
using System.Collections.Concurrent;
using System.Threading;
using System.Threading.Tasks;
using System.Collections.Generic;
using MCP2515;
// This example code shows how you could implement the required main function for a 
// Console UWP Application. You can replace all the code inside Main with your own custom code.

// You should also change the Alias value in the AppExecutionAlias Extension in the 
// Package.appxmanifest to a value that you define. To edit this file manually, right-click
// it in Solution Explorer and select View Code, or open it with the XML Editor.

namespace UpSpiCan
{
    class Program
    {

        static void Main(string[] args)
        {
            var can = new MCP2515.MCP2515(0);
            can.intoloop();

            var monitor = new CanMonitor(can, Console.WriteLine);

            var cts = new CancellationTokenSource();

            // Start TX monitor immediately
            var txTask = monitor.StartSendMonitor(cts.Token);

            // RX monitor task reference + flag
            Task rxTask = null;
            bool rxRunning = false;

            var cangen = new CanGen(monitor, Console.WriteLine);

            PrintHelp();

            // Command loop: interactive cansend / canrecv
            while (true)
            {
                Console.Write("> ");
                var line = Console.ReadLine();
                if (line == null) break;

                line = line.Trim();
                if (line.Equals("exit", StringComparison.OrdinalIgnoreCase) ||
                    line.Equals("quit", StringComparison.OrdinalIgnoreCase))
                {
                    break;
                }

                if (line.Equals("help", StringComparison.OrdinalIgnoreCase))
                {
                    PrintHelp();
                    continue;
                }

                // canrecv commands: start / stop / status
                if (line.StartsWith("canrecv", StringComparison.OrdinalIgnoreCase))
                {
                    if (line.Equals("canrecv start", StringComparison.OrdinalIgnoreCase))
                    {
                        if (!rxRunning)
                        {
                            rxTask = monitor.StartRecvMonitor(cts.Token);
                            rxRunning = true;
                            Console.WriteLine("[INFO] RX monitor started.");
                        }
                        else
                        {
                            Console.WriteLine("[INFO] RX monitor is already running.");
                        }
                        continue;
                    }
                    else if (line.Equals("canrecv stop", StringComparison.OrdinalIgnoreCase))
                    {
                        if (rxRunning)
                        {
                            // Use the same CTS to cancel both TX/RX;
                            // if you want stop RX only, use a separate CTS for RX.
                            Console.WriteLine("[WARN] 'canrecv stop' will stop RX, but current design shares CTS with TX. Use 'exit' to stop all.");
                            continue;
                        }
                        else
                        {
                            Console.WriteLine("[INFO] RX monitor is not running.");
                            continue;
                        }
                    }
                    else if (line.Equals("canrecv status", StringComparison.OrdinalIgnoreCase))
                    {
                        Console.WriteLine("[INFO] RX monitor running: " + (rxRunning ? "YES" : "NO"));
                        continue;
                    }
                    else
                    {
                        // If user typed just "canrecv", treat as start
                        if (!rxRunning)
                        {
                            rxTask = monitor.StartRecvMonitor(cts.Token);
                            rxRunning = true;
                            Console.WriteLine("[INFO] RX monitor started.");
                        }
                        else
                        {
                            Console.WriteLine("[INFO] RX monitor is already running.");
                        }
                        continue;
                    }
                }

                // cansend <id> <dlc> <rtr> <data...>
                if (line.StartsWith("cansend", StringComparison.OrdinalIgnoreCase))
                {
                    CanFrame frame;
                    string err;
                    var ok = TryParseCanSend(line, out frame, out err);
                    if (!ok)
                    {
                        Console.WriteLine("[PARSE ERROR] " + err);
                        continue;
                    }

                    monitor.EnqueueSend(frame);
                    continue;
                }


                if (line.StartsWith("cangen", StringComparison.OrdinalIgnoreCase))
                {
                    if (line.Equals("cangen status", StringComparison.OrdinalIgnoreCase))
                    {
                        Console.WriteLine(cangen.Status());
                        continue;
                    }
                    else if (line.Equals("cangen stop", StringComparison.OrdinalIgnoreCase))
                    {
                        cangen.Stop();
                        continue;
                    }
                    else if (line.StartsWith("cangen start", StringComparison.OrdinalIgnoreCase))
                    {
                        // Parse parameters (space-separated key-value pairs)
                        var parts = line.Split(new[] { ' ', '\t' }, StringSplitOptions.RemoveEmptyEntries);
                        // parts[0] = cangen, parts[1] = start, other is options
                        string err;
                        var opts = new string[Math.Max(0, parts.Length - 2)];
                        for (int i = 2; i < parts.Length; i++)
                            opts[i - 2] = parts[i];

                        if (!cangen.Configure(opts, out err))
                        {
                            Console.WriteLine("[PARSE ERROR] " + err);
                            continue;
                        }

                        if (!cangen.Start(out err))
                        {
                            Console.WriteLine("[ERROR] " + err);
                            continue;
                        }

                        // === Enter "Press any key to stop" mode ===
                        Console.WriteLine("[INFO] cangen is running. Press ANY key to stop...");
                        // This will block the input to the main loop to avoid conflict with ReadLine.
                        Console.ReadKey(true);             // Capture any key press, do not display on screen
                        cangen.Stop();                      // Stop cangen
                        Console.WriteLine("[INFO] cangen stopped (by key).");

                        continue;
                    }
                    else
                    {
                        Console.WriteLine("[INFO] Usage:");
                        Console.WriteLine("  cangen start [rate=N] [ext=0|1] [extRatio=0.0..1.0] [rtr=0|1] [rtrProb=0.0..1.0]");
                        Console.WriteLine("                [minId=hex|dec] [maxId=hex|dec] [dlcMode=fixed|uniform] [dlc=0..8]");
                        Console.WriteLine("                [payload=11-22-33-44...]");
                        Console.WriteLine("  cangen stop");
                        Console.WriteLine("  cangen status");
                        continue;
                    }
                }

                Console.WriteLine("Unknown command. Type 'help' for usage.");
            }

            // shutdown
            cts.Cancel();
            try
            {
                if (rxTask != null)
                {
                    Task.WaitAll(new[] { rxTask, txTask }, TimeSpan.FromSeconds(2));
                }
                else
                {
                    Task.WaitAll(new[] { txTask }, TimeSpan.FromSeconds(2));
                }
            }
            catch (AggregateException)
            {
                // ignore cancellation exceptions
            }
            cts.Dispose();
        }


        private static void PrintHelp()
        {

            Console.WriteLine("=== CAN Send Interactive Command ===");
            Console.WriteLine("Format: cansend <id> <dlc> <rtr> <data bytes...>");
            Console.WriteLine("  <id>   : Hexadecimal, can use 0x prefix. Example: 0x111, 0x1FFFFFFF, or decimal");
            Console.WriteLine("  <dlc>  : 0~8");
            Console.WriteLine("  <rtr>  : true/false or 1/0");
            Console.WriteLine("  <data> : 0~8 bytes (hexadecimal), can include 0x prefix; separated by spaces");
            Console.WriteLine("Examples:");
            Console.WriteLine("  cansend 0x111 8 0 10 19 31 00 15 76 71 F8");
            Console.WriteLine("  cansend 0x1FFFFFFF 8 false 10 19 31 20 45 31 7F 36");
            Console.WriteLine("  cansend 0x121 4 0 11 22 33 44");
            Console.WriteLine("  cansend 0x7FF 0 1           (RTR, no data)");
            Console.WriteLine("  canrecv start");
            Console.WriteLine("  canrecv stop");
            Console.WriteLine("  canrecv status");
            Console.WriteLine("  cangen start");
            Console.WriteLine("Commands: help / exit");

        }

        /// <summary>
        /// Parse the cansend command string into a CanFrame (C# version 7.3)
        /// </summary>
        private static bool TryParseCanSend(string line, out CanFrame frame, out string error)
        {
            frame = default(CanFrame);
            error = "";

            var parts = SplitRespectingSpaces(line);
            if (parts.Count < 4)
            {
                error = "Insufficient parameters. Required: cansend <id> <dlc> <rtr> <data...>";
                return false;
            }

            if (!parts[0].Equals("cansend", StringComparison.OrdinalIgnoreCase))
            {
                error = "It must begin with 'cansend'";
                return false;
            }

            // Parse ID
            uint id;
            if (!TryParseHexUInt(parts[1], out id))
            {
                error = "ID illegal：" + parts[1];
                return false;
            }

            bool isExtended = id > 0x7FF;
            if (isExtended && id > 0x1FFFFFFF)
            {
                error = "Extended ID out of range (maximum 0x1FFFFFFF)";
                return false;
            }

            // Parse DLC
            int dlc;
            if (!int.TryParse(parts[2], NumberStyles.Integer, CultureInfo.InvariantCulture, out dlc))
            {
                error = "DLC illegal：" + parts[2];
                return false;
            }
            if (dlc < 0 || dlc > 8)
            {
                error = "The DLC range must be 0~8";
                return false;
            }

            // Parse RTR
            var rtrToken = parts[3].ToLowerInvariant();
            bool rtr;
            if (rtrToken == "true" || rtrToken == "1" || rtrToken == "t")
                rtr = true;
            else if (rtrToken == "false" || rtrToken == "0" || rtrToken == "f")
                rtr = false;
            else
            {
                error = "RTR illegal：" + parts[3] + "（You can use true/false or 1/0.）";
                return false;
            }

            // Parse data bytes (can be 0)
            var dataTokens = parts.Skip(4).ToList();
            var bytes = new List<byte>();
            for (int i = 0; i < dataTokens.Count; i++)
            {
                var tok = dataTokens[i];
                if (string.IsNullOrWhiteSpace(tok)) continue;

                byte b;
                if (!TryParseHexByte(tok, out b))
                {
                    error = "Data byte illegal：" + tok;
                    return false;
                }
                bytes.Add(b);
            }

            //  Data length check
            if (!rtr)
            {
                if (bytes.Count != dlc)
                {
                    error = "Data Length (" + bytes.Count + ") is inconsistent with DLC (" + dlc + ") ";
                    return false;
                }
            }
            else
            {
                if (bytes.Count > 0 && bytes.Count != dlc)
                {
                    error = "When using RTR with data, the data length must be equal to the DLC (currently" + bytes.Count + "，DLC=" + dlc + "）";
                    return false;
                }
            }

            // A frame is only created after successful parsing.
            frame = new CanFrame(
                id,
                isExtended,
                rtr,
                (byte)dlc,
                bytes.ToArray()
            );
            return true;
        }

        private static List<string> SplitRespectingSpaces(string input)
        {
            return input.Split(new[] { ' ', '\t' }, StringSplitOptions.RemoveEmptyEntries).ToList();
        }

        private static bool TryParseHexUInt(string token, out uint value)
        {
            token = token.Trim();
            if (token.StartsWith("0x", StringComparison.OrdinalIgnoreCase))
            {
                return uint.TryParse(token.Substring(2), NumberStyles.HexNumber, CultureInfo.InvariantCulture, out value);
            }
            // Pure hexadecimal is allowed (no 0x).
            if (IsHexString(token))
            {
                return uint.TryParse(token, NumberStyles.HexNumber, CultureInfo.InvariantCulture, out value);
            }
            // Decimal is also allowed.
            return uint.TryParse(token, NumberStyles.Integer, CultureInfo.InvariantCulture, out value);
        }

        private static bool TryParseHexByte(string token, out byte value)
        {
            token = token.Trim();
            if (token.StartsWith("0x", StringComparison.OrdinalIgnoreCase))
            {
                return byte.TryParse(token.Substring(2), NumberStyles.HexNumber, CultureInfo.InvariantCulture, out value);
            }
            if (IsHexString(token))
            {
                return byte.TryParse(token, NumberStyles.HexNumber, CultureInfo.InvariantCulture, out value);
            }
            return byte.TryParse(token, NumberStyles.Integer, CultureInfo.InvariantCulture, out value);
        }

        private static bool IsHexString(string s)
        {
            if (string.IsNullOrEmpty(s)) return false;
            for (int i = 0; i < s.Length; i++)
            {
                var c = s[i];
                bool isHex = (c >= '0' && c <= '9') ||
                             (c >= 'a' && c <= 'f') ||
                             (c >= 'A' && c <= 'F');
                if (!isHex) return false;
            }
            return true;
        }
    }

    /// <summary>
    /// CAN Listener: Continuously listens for received packets and queues sent packets in the background, logging the data in the Console. (C# 7.3 compatible)
    /// </summary>
    public class CanMonitor
    {
        private readonly MCP2515.MCP2515 _can;
        private readonly Action<string> _log;
        private readonly BlockingCollection<CanFrame> _sendQueue = new BlockingCollection<CanFrame>();

        public CanMonitor(MCP2515.MCP2515 can, Action<string> logger)
        {
            if (can == null) throw new ArgumentNullException("can");
            _can = can;
            _log = logger ?? (s => { });
        }

        /// <summary>
        /// Send packet (place in queue and send out by background process)
        /// </summary>
        public void EnqueueSend(CanFrame frame)
        {
            _sendQueue.Add((CanFrame)frame);
        }

        /// <summary>
        /// Packet listener: Responsible for retrieving packets from the queue and sending them out, and recording TX logs.
        /// </summary>
        public Task StartSendMonitor(CancellationToken ct)
        {
            return Task.Run(async () =>
            {
                try
                {
                    foreach (var frame in _sendQueue.GetConsumingEnumerable(ct))
                    {
                        try
                        {
                            _can.CanSend(frame);
                            _log("[TX " + Timestamp() + "] " + FormatCanFrame(frame));
                        }
                        catch (OperationCanceledException)
                        {
                            break;
                        }
                        catch (Exception ex)
                        {
                            _log("[TX ERROR " + Timestamp() + "] " + ex.Message);
                        }

                        await Task.Yield();
                    }
                }
                catch (OperationCanceledException)
                {
                    // Normal End
                }
            }, ct);
        }

        /// <summary>
        /// Packet reception monitoring: Continuously call CanRecv() and log data in the RX log when available.
        /// </summary>
        public Task StartRecvMonitor(CancellationToken ct)
        {
            return Task.Run(async () =>
            {
                while (!ct.IsCancellationRequested)
                {
                    try
                    {
                        var frame = _can.CanRecv();
                        if (frame != null)
                        {
                            _log("[RX " + Timestamp() + "] " + FormatCanFrame((CanFrame)frame));
                        }
                        else
                        {
                            await Task.Delay(1, ct);
                        }
                    }
                    catch (OperationCanceledException)
                    {
                        break;
                    }
                    catch (Exception ex)
                    {
                        _log("[RX ERROR " + Timestamp() + "] " + ex.Message);
                        await Task.Delay(10, ct);
                    }
                }
            }, ct);
        }

        private static string Timestamp()
        {
            return DateTime.Now.ToString("HH:mm:ss.fff");
        }

        private static string FormatCanFrame(CanFrame f)
        {
            // Assume CanFrame has the following attributes: Id / IsExtendedId / Rtr / Dlc / Data
            string idStr;
            if (f.IsExtended)
                idStr = "0x" + f.Id.ToString("X8");
            else
                idStr = "0x" + f.Id.ToString("X3");

            var type = f.IsExtended ? "EXT" : "STD";
            var rtr = f.IsRemote ? "RTR" : "DATA";
            var dlc = f.DLC;

            string data = "";
            if (f.Data != null && f.Data.Length > 0)
            {
                var len = Math.Min(f.Data.Length, dlc);
                data = BitConverter.ToString(f.Data, 0, len);
            }

            return "ID=" + idStr + " (" + type + ") " + rtr + " DLC=" + dlc + " DATA=[" + data + "]";
        }
    }
}
