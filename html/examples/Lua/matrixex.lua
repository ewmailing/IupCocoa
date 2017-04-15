require( "iuplua" )
require( "iupluacontrols" )
require( "iupluamatrixex" )

mat = iup.matrixex {
  numcol=5, 
  numlin=3,
  numcol_visible=5, 
  numlin_visible=3, 
  widthdef=34, 
  numericquantity1 = "None", 
  numericquantity2 = "None",
  numericquantity3 = "None",
}

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
--mat:setcell(2,2,"1.3")
mat:setcell(3,2,"1.4")
dlg = iup.dialog{iup.vbox{mat; margin="10x10"}}

--iup.MatrixSetFormula(mat, 3, "cos(pi*lin/4)")
--iup.MatrixSetFormula(mat, 3, "cell(\"x\", 1)") -- error
--iup.MatrixSetFormula(mat, 3, "cell(lin, 1) < 3")
--mat.redraw = "Yes"

--iup.MatrixSetFormula(mat, 3, "cell(lin, 1) + cell(lin, 2)")
--mat:SetFormula(, 3, "cell(lin, 1) + cell(lin, 2)")

mat.cellnames = "Excel"
--mat.cellnames = "Matrix"
mat.edithideonfocus = "NO"
mat.editfitvalue = "yes"

mat:setcell(1,3,"=3+3")
mat:setcell(2,2,"=1.3 + cell(2,3)")
--mat:setcell(3,2,"=1.3 + L2C3")
mat:setcell(3,2,"=1.3 + C2")
mat:setcell(2,3,"=sin(pi/4)")
--iup.MatrixSetDynamic(mat)
mat:SetDynamic()

dlg:showxy(iup.CENTER, iup.CENTER)

if (iup.MainLoopLevel()==0) then
  iup.MainLoop()
end
