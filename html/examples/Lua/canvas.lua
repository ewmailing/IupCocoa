--IupCanvas Example in IupLua 

require( "iuplua" )

cv       = iup.canvas {size="300x100", xmin=0, xmax=99, posx=0, dx=10}
dg       = iup.dialog{iup.frame{cv}; title="IupCanvas"}

function cv:motion_cb(x, y, r)
  print(x, y, r)
end

function cv:k_any(c)
  print("c              = ", c)
  print("  XkeyBase(c)  = ", iup.XkeyBase(c))
  print("  isCtrlXkey(c)= ", iup.isCtrlXkey(c))
  print("  isAltXkey(c) = ", iup.isAltXkey(c))
end

function cv:action()
  cv:DrawBegin()
  local w, h = cv:DrawGetSize()

  cv.drawcolor = "255 255 255"
  cv.drawstyle = "FILL"
  cv:DrawRectangle(0, 0, w, h)

  cv.drawcolor = "255 0 0"
  cv:DrawLine(0, 0, w-1, h-1)
  cv:DrawLine(0, h-1, w-1, 0)
  
  cv.drawcolor = "0 0 0"
  cv.drawfont = "Times, 28"
  cv:DrawText("This is a test", w/2, h/2)
  
  cv:DrawEnd()
end

dg:showxy(iup.CENTER, iup.CENTER)

if (iup.MainLoopLevel()==0) then
  iup.MainLoop()
end
