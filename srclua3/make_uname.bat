@echo off
REM This builds all the libraries of the folder for 1 uname

call tecmake %1 %2 %3 %4 %5 %6 %7 %8
call tecmake %1 "MF=iupcd" %2 %3 %4 %5 %6 %7 %8
call tecmake %1 "MF=iupcontrols" %2 %3 %4 %5 %6 %7 %8
call tecmake %1 "MF=iup_pplot" %2 %3 %4 %5 %6 %7 %8
call tecmake %1 "MF=iupgl" %2 %3 %4 %5 %6 %7 %8
call tecmake %1 "MF=iupim" %2 %3 %4 %5 %6 %7 %8
call tecmake %1 "MF=iupole" %2 %3 %4 %5 %6 %7 %8
