/**
 *     Simple exemple of use of 3D OpenGL and IUP.
 * 
 *  Creates a dialog with one canvas and draws a rotating cube in it.
 *  
 *  Author:  Marcelo Gattass, Nov09,2009. 
 *
 */
/*- Include lib interfaces: ANSI C, IUP and OpenGL ------*/
#include <stdio.h>
#include <stdlib.h>
#include <iup.h>        /* IUP functions*/
#include <iupgl.h>      /* IUP functions related to OpenGL (IupGLCanvasOpen,IupGLMakeCurrent and IupGLSwapBuffers) */

#ifdef WIN32
#include <windows.h>    /* includes only in MSWindows not in UNIX, needed by OpenGL in Windows */
#endif

#include <GL/gl.h>     /* OpenGL functions */
#include <GL/glu.h>    /* OpenGL utility functions */

/*- Program context: -------------------------------------------------*/
static Ihandle *canvas;                    /* canvas handle */
static float t=0;                          /* animation time */

/*------------------------------------------*/
/* Auxiliary functions to draw a color cube */
/*------------------------------------------*/

static void polygon(int a, int b, int c, int d)
{
  double vertices[][3]={{-1,-1, 1}, {-1, 1, 1}, { 1, 1, 1}, { 1,-1, 1},
                      {-1,-1,-1}, {-1, 1,-1}, { 1, 1,-1}, { 1,-1,-1}};
  glBegin(GL_POLYGON);
    glVertex3dv(vertices[a]);
    glVertex3dv(vertices[b]);
    glVertex3dv(vertices[c]);
    glVertex3dv(vertices[d]);
  glEnd();
}

static void colorCube( void)  
{
  glColor3f(1,0,0);
  glNormal3f(1,0,0);
  polygon(2,3,7,6);

  glColor3f(0,1,0);
  glNormal3f(0,1,0);
  polygon(1,2,6,5);

  glColor3f(0,0,1);
  glNormal3f(0,0,1);
  polygon(0,3,2,1);

  glColor3f(1,0,1);
  glNormal3f(0,-1,0);
  polygon(3,0,4,7);

  glColor3f(1,1,0);
  glNormal3f(0,0,-1);
  polygon(4,5,6,7);

  glColor3f(0,1,1);
  glNormal3f(-1,0,0);
  polygon(5,4,0,1);
}


/* function called when the canvas is exposed in the screen */
static int repaint_cb(Ihandle *self) 
{
  IupGLMakeCurrent(self);
  glClearColor(0.3f, 0.3f, 0.3f, 1.0f);  /* White */
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();  /* saves current model view in a stack */
      glTranslatef( 0.0f, 0.0f , 0.0f );
      glScalef( 1.0f, 1.0f, 1.0f );
      glRotatef(t,0,0,1);
      colorCube();
   glPopMatrix();
 
  IupGLSwapBuffers(self);  /* change the back buffer with the front buffer */

  return IUP_DEFAULT; /* returns the control to the main loop */
}

/* function called in the event of changes in the width or in the height of the canvas */
static int resize_cb(Ihandle *self, int width, int height)
{
  IupGLMakeCurrent(self);  /* Make the canvas current in OpenGL */

  /* define the entire canvas as the viewport  */
  glViewport(0, 0, width, height);

  /* transformation applied to each vertex */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();           /* identity, i. e. no transformation */

  /* projection transformation (orthographic in the xy plane) */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60,4./3.,1.,15);
  gluLookAt(3,3,3, 0,0,0, 0,0,1);

  return IUP_DEFAULT; /* return to the IUP main loop */
}

int idle_cd(void)
{
  t+=1;
  repaint_cb(canvas);
  return IUP_DEFAULT; /* return to the IUP main loop */
}

int exit_cb(void)
{
   printf("Function to free memory and do finalizations...\n");
   return IUP_CLOSE;
}

/*-------------------------------------------------------------------------*/
/* Incializa o programa.                                                   */
/*-------------------------------------------------------------------------*/

Ihandle* initDialog(void)
{
  Ihandle* dialog;   /* dialog containing the canvas */

  canvas = IupGLCanvas(NULL); /* create _canvas */

  IupSetAttribute(canvas,"RASTERSIZE","640x480");   /* define the initial size in pixels */
  IupSetAttribute(canvas, "BUFFER","DOUBLE");      /* define that this OpenGL _canvas has double buffer (front and back) */
  IupSetAttribute(canvas, "DEPTH_SIZE", "16");

  /* bind callback actions with callback functions */
  IupSetCallback(canvas, "ACTION", (Icallback) repaint_cb);  
  IupSetCallback(canvas, "RESIZE_CB",(Icallback) resize_cb); 

  /* create the dialog and set its attributes */
  dialog = IupDialog(canvas);
  IupSetAttribute(dialog, "TITLE", "IUP 3D OpenGL");

  IupSetCallback(dialog, "CLOSE_CB", (Icallback) exit_cb);
  IupSetFunction("IDLE_ACTION", (Icallback)idle_cd);

  return dialog;
}

/*-----------------------*/
/* Main function.        */
/*-----------------------*/
int main(int argc, char* argv[]) 
{
  Ihandle* dialog;

  IupOpen(&argc, &argv);                       /* opens the IUP lib */
  IupGLCanvasOpen();                          /* enable the use of OpenGL to draw in canvas */

  dialog = initDialog();                      /* local function to create a dialog with buttons and canvas */
  IupShowXY(dialog, IUP_CENTER, IUP_CENTER);  /* shows dialog in the center of screen */

  IupMainLoop();                              /* handle the program control to the IUP lib until a return IUP_CLOSE */

  IupClose();                               /* closes the IUP lib */ 
}
