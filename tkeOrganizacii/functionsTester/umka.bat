@echo off
echo Begin Compiling
mingw32-make %*

echo Load Programm
..\..\bin\functionsTester.exe