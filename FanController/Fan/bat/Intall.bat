@echo off
schtasks /create /XML "C:\Program Files\UpFanController\xml\fan_before_start.xml" /TN fan_before_start /F
schtasks /run /TN fan_before_start
schtasks /create /XML "C:\Program Files\UpFanController\xml\fan.xml" /TN fan /F
schtasks /run /TN fan


