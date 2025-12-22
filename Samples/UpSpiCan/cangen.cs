using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using UpSpiCan;
using MCP2515;

namespace UpSpiCan
{
    internal class CanGen
    {
        private readonly CanMonitor _monitor;
        private readonly Action<string> _log;
        private readonly Random _rng = new Random();
        private Task _task;
        private CancellationTokenSource _cts;
        private volatile bool _running;
        private long _txCount;

        // parameter
        private int _ratePerSec = 10;       // Number of items delivered per second
        private double _extRatio = 0.5;     // Extended ID probability (0..1)
        private double _rtrProb = 0.1;      // RTR probability (0..1)
        private bool _forceExt = false;     // If true: Use extended IDs for all.
        private bool _forceRtr = false;     // If true: Use RTR for all.
        private uint _minStdId = 0x000;
        private uint _maxStdId = 0x7FF;
        private uint _minExtId = 0x00000000;
        private uint _maxExtId = 0x1FFFFFFF;

        private string _dlcMode = "uniform"; // "uniform" and "fixed"
        private int _fixedDlc = 8;           // Use when dlcMode=fixed
        private byte[] _fixedPayload = null; // Fixed payload (optional), length requires matching DLC

        public CanGen(CanMonitor monitor, Action<string> logger)
        {
            if (monitor == null) throw new ArgumentNullException("monitor");
            _monitor = monitor;
            _log = logger ?? (s => { });
        }

        /// <summary>
        /// Parse the parameter string of cangen start
        /// </summary>
        public bool Configure(string[] args, out string error)
        {
            error = "";
            // Value reset (starts with the default value each time you start).
            _ratePerSec = 10;
            _extRatio = 0.5;
            _rtrProb = 0.1;
            _forceExt = false;
            _forceRtr = false;
            _minStdId = 0x000;
            _maxStdId = 0x7FF;
            _minExtId = 0x00000000;
            _maxExtId = 0x1FFFFFFF;
            _dlcMode = "uniform";
            _fixedDlc = 8;
            _fixedPayload = null;

            for (int i = 0; i < args.Length; i++)
            {
                var tok = args[i].Trim();
                if (tok.Length == 0) continue;

                var kv = tok.Split(new[] { '=' }, 2);
                var key = kv[0].ToLowerInvariant();
                var val = kv.Length > 1 ? kv[1] : "";

                if (key == "rate")
                {
                    int r;
                    if (!int.TryParse(val, NumberStyles.Integer, CultureInfo.InvariantCulture, out r) || r <= 0)
                    {
                        error = "Invalid rate: " + val;
                        return false;
                    }
                    _ratePerSec = r;
                }
                else if (key == "ext")
                {
                    if (val == "1" || val.ToLowerInvariant() == "true")
                    {
                        _forceExt = true;
                        _extRatio = 1.0;
                    }
                    else if (val == "0" || val.ToLowerInvariant() == "false")
                    {
                        _forceExt = false;
                    }
                    else
                    {
                        error = "Invalid ext: " + val;
                        return false;
                    }
                }
                else if (key == "extratio")
                {
                    double ratio;
                    if (!double.TryParse(val, NumberStyles.Float, CultureInfo.InvariantCulture, out ratio) ||
                        ratio < 0.0 || ratio > 1.0)
                    {
                        error = "Invalid extRatio: " + val;
                        return false;
                    }
                    _extRatio = ratio;
                }
                else if (key == "rtr")
                {
                    if (val == "1" || val.ToLowerInvariant() == "true")
                    {
                        _forceRtr = true;
                        _rtrProb = 1.0;
                    }
                    else if (val == "0" || val.ToLowerInvariant() == "false")
                    {
                        _forceRtr = false;
                    }
                    else
                    {
                        error = "Invalid rtr: " + val;
                        return false;
                    }
                }
                else if (key == "rtrprob")
                {
                    double p;
                    if (!double.TryParse(val, NumberStyles.Float, CultureInfo.InvariantCulture, out p) ||
                        p < 0.0 || p > 1.0)
                    {
                        error = "Invalid rtrProb: " + val;
                        return false;
                    }
                    _rtrProb = p;
                }
                else if (key == "minid")
                {
                    uint id;
                    if (!TryParseHexUInt(val, out id))
                    {
                        error = "Invalid minId: " + val;
                        return false;
                    }
                    // If it's in ext mode, nest it within the ext scope; otherwise, nest it within the std scope.
                    if (_forceExt || _extRatio > 0.0)
                        _minExtId = id;
                    else
                        _minStdId = id;
                }
                else if (key == "maxid")
                {
                    uint id;
                    if (!TryParseHexUInt(val, out id))
                    {
                        error = "Invalid maxId: " + val;
                        return false;
                    }
                    if (_forceExt || _extRatio > 0.0)
                        _maxExtId = id;
                    else
                        _maxStdId = id;
                }
                else if (key == "dlcmode")
                {
                    var v = val.ToLowerInvariant();
                    if (v == "uniform" || v == "fixed")
                        _dlcMode = v;
                    else
                    {
                        error = "Invalid dlcMode: " + val;
                        return false;
                    }
                }
                else if (key == "dlc")
                {
                    int d;
                    if (!int.TryParse(val, NumberStyles.Integer, CultureInfo.InvariantCulture, out d) ||
                        d < 0 || d > 8)
                    {
                        error = "Invalid dlc: " + val;
                        return false;
                    }
                    _fixedDlc = d;
                }
                else if (key == "payload")
                {
                    // Parsing format: For example, "11-22-33-44" or "0x11-0x22-33-44"
                    var parts = val.Split(new[] { '-' }, StringSplitOptions.RemoveEmptyEntries);
                    var buf = new byte[parts.Length];
                    for (int j = 0; j < parts.Length; j++)
                    {
                        byte b;
                        if (!TryParseHexByte(parts[j], out b))
                        {
                            error = "Invalid payload byte: " + parts[j];
                            return false;
                        }
                        buf[j] = b;
                    }
                    _fixedPayload = buf;
                }
                else
                {
                    error = "Unknown option: " + key;
                    return false;
                }
            }

            // Scope protection
            if (_minStdId > _maxStdId) { var t = _minStdId; _minStdId = _maxStdId; _maxStdId = t; }
            if (_minExtId > _maxExtId) { var t = _minExtId; _minExtId = _maxExtId; _maxExtId = t; }

            // Payload and DLC check (must be consistent only in fixed mode and when there is a payload)
            if (_dlcMode == "fixed" && _fixedPayload != null && _fixedPayload.Length != _fixedDlc)
            {
                error = "Payload length must equal fixed DLC (" + _fixedDlc + ")";
                return false;
            }

            return true;
        }

        public bool Start(out string error)
        {
            error = "";
            if (_running)
            {
                error = "cangen already running.";
                return false;
            }

            _cts = new CancellationTokenSource();
            _txCount = 0;
            _running = true;

            _task = Task.Run(async () =>
            {
                var ct = _cts.Token;

                // Interval between each cycle (milliseconds)
                double intervalMs = 1000.0 / Math.Max(1, _ratePerSec);

                while (!ct.IsCancellationRequested)
                {
                    try
                    {
                        var frame = BuildRandomFrame();
                        _monitor.EnqueueSend(frame);
                        _txCount++;

                        // Control rate
                        await Task.Delay(TimeSpan.FromMilliseconds(intervalMs), ct);
                    }
                    catch (OperationCanceledException)
                    {
                        break;
                    }
                    catch (Exception ex)
                    {
                        _log("[CANGEN ERROR " + Timestamp() + "] " + ex.Message);
                        //A small concession to avoid making a mistake.
                        await Task.Delay(10, ct);
                    }
                }
            }, _cts.Token);

            _log("[INFO] cangen started. rate=" + _ratePerSec + "/s, extRatio=" + _extRatio + ", rtrProb=" + _rtrProb + ", dlcMode=" + _dlcMode + ".");
            return true;
        }

        public void Stop()
        {
            if (!_running) return;
            try
            {
                _cts.Cancel();
                Task.WaitAll(new[] { _task }, TimeSpan.FromSeconds(2));
            }
            catch (AggregateException)
            {
            }
            finally
            {
                _cts.Dispose();
                _running = false;
                _log("[INFO] cangen stopped. total sent=" + _txCount);
            }
        }

        public string Status()
        {
            return "[INFO] cangen running: " + (_running ? "YES" : "NO") + ", total sent=" + _txCount;
        }

        private CanFrame BuildRandomFrame()
        {
            // Decide whether to use an extended ID
            bool isExt;
            if (_forceExt) isExt = true;
            else isExt = (_rng.NextDouble() < _extRatio);

            // Decision ID
            uint id;
            if (isExt)
            {
                uint range = _maxExtId - _minExtId + 1;
                id = _minExtId + (uint)(_rng.NextDouble() * range);
                if (id > 0x1FFFFFFF) id = 0x1FFFFFFF;
                if (id < 0) id = 0;
            }
            else
            {
                uint range = _maxStdId - _minStdId + 1;
                id = _minStdId + (uint)(_rng.NextDouble() * range);
                if (id > 0x7FF) id = 0x7FF;
                if (id < 0) id = 0;
            }

            // Decision RTR
            bool rtr;
            if (_forceRtr) rtr = true;
            else rtr = (_rng.NextDouble() < _rtrProb);

            // Decision DLC
            int dlc;
            if (_dlcMode == "fixed")
            {
                dlc = _fixedDlc;
            }
            else
            {
                dlc = _rng.Next(0, 9); // 0..8
            }

            // Decision Data
            byte[] data;
            if (rtr)
            {
                // RTRs generally do not include data, or the data length must be equal to the DLC (if required by the Library). Here, an empty array or a fixed payload is used.
                if (_fixedPayload != null)
                {
                    data = new byte[_fixedPayload.Length];
                    Array.Copy(_fixedPayload, data, _fixedPayload.Length);
                }
                else
                {
                    data = new byte[0];
                }
            }
            else
            {
                if (_fixedPayload != null)
                {
                    // Use a fixed payload (assuming the length has been checked to be consistent).
                    data = new byte[_fixedPayload.Length];
                    Array.Copy(_fixedPayload, data, _fixedPayload.Length);
                }
                else
                {
                    data = new byte[dlc];
                    for (int i = 0; i < dlc; i++)
                    {
                        data[i] = (byte)_rng.Next(0, 256);
                    }
                }
            }

            // Create a Frame (depending on your CanFrame constructor).
            var frame = new CanFrame(
                id,
                isExt,
                rtr,
                (byte)dlc,
                data
            );
            return frame;
        }

        private static string Timestamp()
        {
            return DateTime.Now.ToString("HH:mm:ss.fff");
        }

        // Tools: Parsing hex/dec
        private static bool TryParseHexUInt(string token, out uint value)
        {
            token = token.Trim();
            if (token.StartsWith("0x", StringComparison.OrdinalIgnoreCase))
            {
                return uint.TryParse(token.Substring(2), NumberStyles.HexNumber, CultureInfo.InvariantCulture, out value);
            }
            if (IsHexString(token))
            {
                return uint.TryParse(token, NumberStyles.HexNumber, CultureInfo.InvariantCulture, out value);
            }
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
}
