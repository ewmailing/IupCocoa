require( "iuplua" )
require( "iupluacontrols" )

mat = iup.matrix {numcol=5, numlin=3,numcol_visible=5, numlin_visible=3, widthdef=34}


mat.resizematrix = "YES"
mat:setcell(0,0,"Inflation")
mat:setcell(1,0,"Medicine")
mat:setcell(2,0,"Food")
mat:setcell(3,0,"Energy")
mat:setcell(0,1,"January 2000")
mat:setcell(0,2,"February 2000")
mat:setcell(1,1,"5.6")
mat:setcell(2,1,"2.2")
mat:setcell(3,1,"7.2")
mat:setcell(1,2,"4.6")
mat:setcell(2,2,"1.3")
mat:setcell(3,2,"1.4")

-- to show drop feedback
function mat:dropcheck_cb(lin, col)
  if (lin==3 and col==1) then
    return iup.DEFAULT
  end
  return iup.IGNORE
end

function mat:drop_cb(drop, lin, col)
  if (lin==3 and col==1) then
    drop[1] = "A - Test of Very Big String for Dropdown!"
    drop[2] = "B"
    drop[3] = "C"
    drop[4] = "XXX"
    drop[5] = "5"
    drop[6] = "6"
    drop[7] = "7"
    drop[8] = nil
    return iup.DEFAULT
  end
  return iup.IGNORE
end

function mat:dropselect_cb(lin, col,drop, t, i, v)
  print("lin="..lin..",col="..col..",i="..i..",state="..v..",value="..t)
end

dlg = iup.dialog{iup.vbox{mat; margin="10x10"}}
dlg:showxy(iup.CENTER, iup.CENTER)

if (iup.MainLoopLevel()==0) then
  iup.MainLoop()
end
