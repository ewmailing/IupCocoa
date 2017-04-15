@echo off  

if "%1"==""        goto iupexe32
if "%1"=="vc10"    goto iupexe32
if "%1"=="vc10_64" goto iupexe64
if "%1"=="cygw17"  goto iupexecygw
if "%1"=="all"     goto iupexeall
goto end

REM Must use dll* so USE_* will use the correct TEC_UNAME

:iupexe32
call tecmake dll10 %2 %3 %4 %5 %6 %7
goto end

:iupexe64
call tecmake dll10_64 %2 %3 %4 %5 %6 %7
goto end

:iupexecygw
call tecmake cygw17 %2 %3 %4 %5 %6 %7
goto end

:iupexeall
call make_uname vc10 %2 %3 %4 %5 %6 %7
call make_uname vc10_64 %2 %3 %4 %5 %6 %7
REM call make_uname cygw17 %2 %3 %4 %5 %6 %7
goto end

:end
