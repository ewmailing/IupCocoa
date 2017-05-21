@echo off
REM This builds all the libraries for 1 uname

FOR %%u IN (src srccd srccontrols srcmatrixex srcgl srcglcontrols srcplot srcmglplot srcscintilla srcim srcimglib srcole srctuio srcweb srcledc srcview) DO call make_uname_lib.bat %%u %1 %2 %3 %4 %5 %6 %7 %8 %9

REM call make_uname_lib.bat srclua5 %1 %2 %3 %4 %5 %6 %7 %8 %9
REM call make_uname_lib.bat srcconsole %1 %2 %3 %4 %5 %6 %7 %8 %9

call make_uname_lib.bat srclua5 %1 "USE_LUA51=Yes" %2 %3 %4 %5 %6 %7 %8 %9
call make_uname_lib.bat srcconsole %1 "USE_LUA51=Yes" %2 %3 %4 %5 %6 %7 %8 %9

call make_uname_lib.bat srclua5 %1 "USE_LUA52=Yes" %2 %3 %4 %5 %6 %7 %8 %9
call make_uname_lib.bat srcconsole %1 "USE_LUA52=Yes" %2 %3 %4 %5 %6 %7 %8 %9

call make_uname_lib.bat srclua5 %1 "USE_LUA53=Yes" %2 %3 %4 %5 %6 %7 %8 %9
call make_uname_lib.bat srcconsole %1 "USE_LUA53=Yes" %2 %3 %4 %5 %6 %7 %8 %9

call make_uname_lib.bat html\examples\tests %1 %2 %3 %4 %5 %6 %7 %8 %9
cd ..\..

call make_uname_lib.bat html\examples\tutorial\simple_paint2 %1 %2 %3 %4 %5 %6 %7 %8 %9
cd ..\..\..

