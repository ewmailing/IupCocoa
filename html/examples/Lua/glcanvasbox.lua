-- Example IupGLCanvas in Lua
-- Creates a OpenGL canvas and draws a line in it.
-- This example uses gllua binding of OpenGL to Lua.

require("iuplua")
require("iupluagl")
require("iupluaglcontrols")
require("luagl")

cnv = iup.glcanvasbox{buffer="DOUBLE", rastersize = "300x300"}

function cnv:action(x, y)
  iup.GLMakeCurrent(self)
  
  gl.ClearColor(1.0, 1.0, 1.0, 1.0)
  gl.Clear(gl.COLOR_BUFFER_BIT)
  
  gl.Begin( gl.LINES )
  gl.Color(1.0, 0.0, 0.0)
  gl.Vertex(0.0, 0.0)
  gl.Vertex(10.0, 10.0)
  gl.End()
  
  iup.GLSwapBuffers(self)
end

function cnv:resize_cb(width, height)
  iup.GLMakeCurrent(self)
  
  gl.Viewport(0, 0, width, height)
  
  gl.MatrixMode(gl.PROJECTION)
  gl.LoadIdentity()
  gl.MatrixMode(gl.MODELVIEW)
  gl.LoadIdentity()
end

img_release = iup.image {
      {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
      {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2},
      {1,1,3,3,3,3,3,3,3,3,3,3,3,3,2,2},
      {1,1,3,3,3,3,3,3,3,3,3,3,3,3,2,2},
      {1,1,3,3,3,3,3,3,3,3,3,3,3,3,2,2},
      {1,1,3,3,3,3,3,3,3,3,3,3,3,3,2,2},
      {1,1,3,3,3,3,3,3,3,3,3,3,3,3,2,2},
      {1,1,3,3,3,3,3,3,4,4,3,3,3,3,2,2},
      {1,1,3,3,3,3,3,4,4,4,4,3,3,3,2,2},
      {1,1,3,3,3,3,3,4,4,4,4,3,3,3,2,2},
      {1,1,3,3,3,3,3,3,4,4,3,3,3,3,2,2},
      {1,1,3,3,3,3,3,3,3,3,3,3,3,3,2,2},
      {1,1,3,3,3,3,3,3,3,3,3,3,3,3,2,2},
      {1,1,3,3,3,3,3,3,3,3,3,3,3,3,2,2},
      {1,1,3,3,3,3,3,3,3,3,3,3,3,3,2,2},
      {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
      {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};
      colors = { "215 215 215", "40 40 40", "30 50 210", "240 0 0" }
}

  glabel = iup.gllabel{ title = "Label",
    font = "Arial, 18",
    image = img_release}

  gbutton1 = iup.glbutton{ title = "Button", image = img_release,  tip = "Button Tip", padding = "5x5"}

  gtoggle = iup.gltoggle{ title = "Toggle"}
  gtoggle.padding = "5x5"

  gtoggle1 = iup.gltoggle{}
  gtoggle1.padding = "5x5"
  gtoggle1.image = img_release

  gsep1 = iup.glseparator{}

  glink = iup.gllink{ url = "http://www.tecgraf.puc-rio.br/iup", title = "IUP Toolkit"}

  pbar1 = iup.glprogressbar{}
  pbar1.value = "0.3"
  pbar1.show_text = "Yes"

  gval1 = iup.glval{
    value = "0.3",
    tip = "Val Tip"}

hbox = iup.hbox{glabel, gsep1, gbutton1, gtoggle, glink, pbar1, gval1, 
  alignment = "ACENTER",
  margin = "5x5",
  gap = "5",
  }

gframe = iup.glframe{hbox,
  title = "frame",
  horizontalalign = "ACENTER",  -- used in canvasbox
  verticalalign = "ATOP",  -- used in canvasbox
  }

iup.Append(cnv, gframe)

dlg = iup.dialog{cnv; title="IupGLCanvasBox Example"}

function cnv:k_any(c)
  if c == iup.K_q or c == iup.K_Esc then
    return iup.CLOSE
  else
    return iup.DEFAULT
  end
end

dlg:show()

dlg:show()
cnv.rastersize = nil -- reset minimum limitation

if (iup.MainLoopLevel()==0) then
  iup.MainLoop()
end
