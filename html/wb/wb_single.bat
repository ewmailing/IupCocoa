@echo off

set lng=en
REM set lng=%1

Echo Building...
lua5.1 make_single.lua %lng%
Echo .

