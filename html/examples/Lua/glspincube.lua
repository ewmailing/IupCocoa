require("iuplua")
require("iupluagl")
require("luagl")
require("luaglu")

--- Program context: -----------------------------------------
canvas = iup.glcanvas{buffer="DOUBLE", rastersize = "640x480"};	                 -- canvas handle 
local t=0;			                         									 -- animation time

--------------------------------------------
-- Auxiliary functions to draw a color cube 
--------------------------------------------

function polygon(a, b, c, d)
  local vertices = {{-1,-1, 1}, {-1, 1, 1}, { 1, 1, 1}, { 1,-1, 1},
                    {-1,-1,-1}, {-1, 1,-1}, { 1, 1,-1}, { 1,-1,-1}}
  gl.Begin(gl.POLYGON);
	gl.Vertex(vertices[a])
	gl.Vertex(vertices[b])
	gl.Vertex(vertices[c])
    gl.Vertex(vertices[d])
  gl.End();
end

function colorCube()  
  gl.Color(1,0,0);
  gl.Normal(1,0,0);
  polygon(3,4,8,7);

  gl.Color(0,1,0);
  gl.Normal(0,1,0);
  polygon(2,3,7,6);

  gl.Color(0,0,1);
  gl.Normal(0,0,1);
  polygon(1,4,3,2);

  gl.Color(1,0,1);
  gl.Normal(0,-1,0);
  polygon(4,1,5,8);

  gl.Color(1,1,0);
  gl.Normal(0,0,-1);
  polygon(5,6,7,8);

  gl.Color(0,1,1);
  gl.Normal(-1,0,0);
  polygon(6,5,1,2);
end


-- function called when the canvas is exposed in the screen
function canvas:action(x, y) 
  iup.GLMakeCurrent(self)
  gl.ClearColor(1, 1, 1, 1.0)  -- White
  gl.Clear("COLOR_BUFFER_BIT,DEPTH_BUFFER_BIT")
  gl.Enable(gl.DEPTH_TEST);

  gl.MatrixMode(gl.MODELVIEW);
  gl.PushMatrix();  -- saves current model view in a stack
      gl.Translate( 0.0, 0.0 , 0.0 );
      gl.Scale( 1.0, 1.0, 1.0 );
      gl.Rotate(t,0,0,1);
      colorCube();
   gl.PopMatrix();
 
  iup.GLSwapBuffers(self)  -- change the back buffer with the front buffer
end

-- function called in the event of changes in the width or in the height of the canvas
function canvas:resize_cb(width, height)
  iup.GLMakeCurrent(self)  -- Make the canvas current in OpenGL 

  -- define the entire canvas as the viewport  
  gl.Viewport(0, 0, width, height);

  -- transformation applied to each vertex
  gl.MatrixMode(gl.MODELVIEW);
  gl.LoadIdentity();           -- identity, i. e. no transformation

  -- projection transformation (orthographic in the xy plane)
  gl.MatrixMode(gl.PROJECTION);
  gl.LoadIdentity();
  glu.Perspective(60,4./3.,1.,15);
  glu.LookAt(3,3,3, 0,0,0, 0,0,1);

end

function idle_cd(void)
  t = t+1;
  canvas:action(canvas);
end

iup.SetIdle(idle_cd) 
dialog = iup.dialog{canvas; title = "IUP 3D OpenGL"}

function dialog:exit_cb(void)
   print("Function to free memory and do finalizations...");
   return iup.CLOSE;
end

dialog:show()

if (iup.MainLoopLevel()==0) then
  iup.MainLoop()
end