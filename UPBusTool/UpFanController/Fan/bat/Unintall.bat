@echo off
wmic process where "name='fan.exe'" delete &&^
schtasks /Delete /TN fan /F &&^
timeout 5 &&^
rd /s /q "C:\Program Files\UpFanController" &&^
rd /s /q "C:\Users\%username%\AppData\Local\fan"
