@echo off
g++ -o main.exe src/main.cpp
IF EXIST main.exe main.exe
py compression/comp.py images/result.qoi
