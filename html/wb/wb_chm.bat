@echo off

set lng=en
REM set lng=%1

Echo Building...
lua5.1 make_hh.lua %lng%
Echo .
pause

Echo Preparing...
move wb_%lng%.hhp ..
move wb_tree_%lng%.hhc ..
cd ..
move download download.old
Echo .
pause

Echo Compiling...
hhc wb_%lng%.hhp
Echo .
pause     

Echo Finishing...     
move wb_%lng%.hhp wb
move wb_tree_%lng%.hhc wb
move download.old download
move /y *.chm wb
cd wb
Echo .

Echo Done.
