var oShell = new ActiveXObject("Shell.Application");
var commandtoRun = "C:\\Program Files\\UpFanController\\bat\\Unintall.bat";
oShell.ShellExecute(commandtoRun, "", "", "runas", 0);