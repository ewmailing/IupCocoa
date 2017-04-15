/********************************************************************
 Example IupGLCanvas in C 
 Creates a OpenGL canvas and draws a line in it.
 *** by  Mark Stroetzel Glasberg, March 2001 ***
********************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include <iup_plus.h>          

#ifdef WIN32
  #include <windows.h>
#endif
#include <GL/gl.h> 


static int k_any(Ihandle*, int c)
{
  if(c == K_ESC)
    return IUP_CLOSE;
  else
    return IUP_DEFAULT;
}

static int redraw(Ihandle* ih)
{
  Iup::GLCanvas canvas(ih);

  int w, h;
  canvas.GetIntegerInteger("RASTERSIZE", w, h);

  canvas.MakeCurrent();
  
  glViewport(0, 0, w, h);
  glClearColor(1.0, 1.0, 1.0, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glColor3f(1.0,0.0,0.0);
  glBegin(GL_QUADS); 
    glVertex2f( 0.9f,  0.9f); 
    glVertex2f( 0.9f, -0.9f); 
    glVertex2f(-0.9f, -0.9f); 
    glVertex2f(-0.9f,  0.9f); 
  glEnd();

  canvas.SwapBuffers();

  return IUP_DEFAULT;
}

int main(int argc, char **argv)
{
  Iup::Open(argc, argv);
  Iup::GLCanvas::Open();

  Iup::GLCanvas canvas;
  canvas.SetCallback("ACTION", (Icallback)redraw);
  canvas.SetCallback("K_ANY", (Icallback)k_any);
  canvas.SetAttribute("BUFFER", "DOUBLE");
  canvas.SetAttribute("RASTERSIZE", "123x200");

  Iup::Hbox finale(Iup::Fill(),
                   canvas, 
                   Iup::Fill());

  Iup::Dialog dg(finale);
  dg.SetAttribute("TITLE", "IupGLCanvas");

  dg.Show();

  Iup::MainLoop();

  Iup::Close();

  return 0;
}
