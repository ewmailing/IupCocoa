@echo off
REM This builds 1 library for 1 uname

cd %1
call make_uname %2 %3 %4 %5 %6 %7
cd ..
