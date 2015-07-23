@echo off
REM This builds all the libraries for 1 uname

FOR %%u IN (src srccd srccontrols srcmatrixex srcgl srcglcontrols srcplot srcpplot srcmglplot srcscintilla srcim srcimglib srcole srctuio srcweb srcledc srcview test) DO call make_uname_lib.bat %%u %1 %2 %3 %4 %5 %6 %7 %8 %9

call make_uname_lib.bat srclua5 %1 %2 %3 %4 %5 %6 %7 %8 %9
call make_uname_lib.bat srcconsole %1 %2 %3 %4 %5 %6 %7 %8 %9

REM set USE_LUA51=Yes
REM call make_uname_lib.bat srclua5 %1 %2 %3 %4 %5 %6 %7 %8 %9
REM call make_uname_lib.bat srcconsole %1 %2 %3 %4 %5 %6 %7 %8 %9
REM set USE_LUA51=

REM set USE_LUA52=Yes
REM call make_uname_lib.bat srclua5 %1 %2 %3 %4 %5 %6 %7 %8 %9
REM call make_uname_lib.bat srcconsole %1 %2 %3 %4 %5 %6 %7 %8 %9
REM set USE_LUA52=

REM set USE_LUA53=Yes
REM call make_uname_lib.bat srclua5 %1 %2 %3 %4 %5 %6 %7 %8 %9
REM call make_uname_lib.bat srcconsole %1 %2 %3 %4 %5 %6 %7 %8 %9
REM set USE_LUA53=
