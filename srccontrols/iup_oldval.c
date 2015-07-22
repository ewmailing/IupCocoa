/** \file
 * \brief Valuator Control.
 *
 * See Copyright Notice in "iup.h"
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "iup.h"
#include "iupcbs.h"
#include "iupcontrols.h"
#include "iupkey.h"

#include <cd.h>
#include <cdiup.h>
#include <cddbuf.h>

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_stdcontrols.h"
#include "iup_controls.h"
#include "iup_cdutil.h"
#include "iup_register.h"



struct _IcontrolData
{
  iupCanvas canvas;  /* from IupCanvas (must reserve it) */

  /* mouse interaction control */
  int moving;

  /* visual appearance control */
  int w,h;
  long bgcolor,
       light_shadow,
       mid_shadow,
       dark_shadow;
  int has_focus; 

  /* attributes */
  int type;
  int show_ticks;
  double val,
         step,
         pagestep,
         vmin,
         vmax;

  unsigned char* himage;
  int himage_w;
  int himage_h;
  long int* himage_colors;
  int himage_numcolors;

  unsigned char* himageinactive;
  int himageinactive_w;
  int himageinactive_h;
  long int* himageinactive_colors;
  int himageinactive_numcolors;

  /* drawing canvas */
  cdCanvas *cddbuffer;
  cdCanvas *cdcanvas;

  /* Internal methods */
  void (*Draw)(Ihandle* ih);
  void (*CalcPosition)(Ihandle*,int,int);
};

#define IVAL_HANDLER_LONG  22  /* handler long side */
#define IVAL_HANDLER_SHORT 16  /* handler short side */
#define IVAL_TRAIL          4  /* with of the trail */
#define IVAL_TICK           3  /* size of the tick mark */
#define IVAL_TRAIL_START    (IVAL_HANDLER_LONG/2+1)
#define IVAL_TRAIL_END(_s)  (_s-1-IVAL_HANDLER_LONG/2-1)
#define IVAL_TRAIL_LONG(_s) (_s-1-IVAL_HANDLER_LONG-1)

enum {IVAL_VERTICAL, IVAL_HORIZONTAL};

static void iValUpdateImageBgColor(long int* colors, int num_colors, long bgcolor)
{
  int c;
  for (c = 0; c < num_colors; c++)
  {
    if (*colors == -1)
      *colors = bgcolor;
    colors++;
  }

}

static void iValSetImage(const char* name, unsigned char** image_out, long int** color_out,
                        int* w_out, int* h_out, int* numcolors_out)
{
  int x, y, c, width, height, max_color = 0;
  unsigned char* image_data;
  Ihandle *img = IupGetHandle(name);    

  *w_out = width = img->currentwidth;
  *h_out = height = img->currentheight;
  image_data = (unsigned char*)IupGetAttribute(img, "WID");

  *image_out = (unsigned char*) realloc(*image_out, width*height*sizeof(unsigned char));
  *color_out = (long int*) realloc(*color_out, 256*sizeof(long int));

  for(y = height-1 ; y >=0 ; y--) 
  {
    for(x = 0 ; x < width ; x++)  
    {
      (*image_out)[x+width*y] = *image_data;
      if (*image_data > max_color)
        max_color = *image_data;
      image_data++;
    }
  }

  max_color++;
  *numcolors_out = max_color;

  for(c = 0 ; c < max_color; c++) 
  {
    char colorstr[30];
    char *value;
    sprintf(colorstr, "%d", c);
    value = iupAttribGet(img, colorstr);      
    if (iupStrEqualNoCase(value, "BGCOLOR")) 
      (*color_out)[c] = -1;
    else
      (*color_out)[c] = cdIupConvertColor(value);
  }
}

static void iValRepaint(Ihandle* ih)
{
  if (!ih->data->cddbuffer)
    return;

  /* update render */
  cdCanvasBackground(ih->data->cddbuffer, ih->data->bgcolor);
  cdCanvasClear(ih->data->cddbuffer);
  ih->data->Draw(ih);

  /* update display */
  cdCanvasFlush(ih->data->cddbuffer);
  if (ih->data->has_focus)
    cdIupDrawFocusRect(ih, ih->data->cdcanvas, 0, 0, ih->data->w-1, ih->data->h-1);
}

static void iValDrawVerticalTick(Ihandle* ih, int y)
{
  cdIupDrawHorizSunkenMark(ih->data->cddbuffer, 1, 1+IVAL_TICK, y, ih->data->light_shadow, ih->data->dark_shadow);
  cdIupDrawHorizSunkenMark(ih->data->cddbuffer, ih->data->w-2-IVAL_TICK, ih->data->w-2, y, ih->data->light_shadow, ih->data->dark_shadow);
}

static void iValDrawHorizontalTick(Ihandle* ih, int x)
{

  cdIupDrawVertSunkenMark(ih->data->cddbuffer, x, 1, 1+IVAL_TICK, ih->data->light_shadow, ih->data->dark_shadow);
  cdIupDrawVertSunkenMark(ih->data->cddbuffer, x, ih->data->h-2-IVAL_TICK, ih->data->h-2, ih->data->light_shadow, ih->data->dark_shadow);
}

static void iValDrawVerticalTicks(Ihandle* ih)
{
  int i;
  for (i = 0; i < ih->data->show_ticks-1; i++)
  {
    iValDrawVerticalTick(ih, (i*(IVAL_TRAIL_LONG(ih->data->h)-2))/(ih->data->show_ticks-1) + IVAL_TRAIL_START+1);
  }

  iValDrawVerticalTick(ih, IVAL_TRAIL_END(ih->data->h));
}

static void iValDrawHorizontalTicks(Ihandle* ih)
{
  int i;
  for (i = 0; i < ih->data->show_ticks-1; i++)
  {
    iValDrawHorizontalTick(ih, (i*(IVAL_TRAIL_LONG(ih->data->w)-2))/(ih->data->show_ticks-1) + IVAL_TRAIL_START+1);
  }

  iValDrawHorizontalTick(ih, IVAL_TRAIL_END(ih->data->w));
}

static void iValDrawVertical(Ihandle* ih)
{
  int y, x = (ih->data->w-IVAL_TRAIL)/2;
  double vn = (ih->data->val-ih->data->vmin)/(ih->data->vmax-ih->data->vmin);

  /* trail */
  cdIupDrawSunkenRect(ih->data->cddbuffer, x, IVAL_TRAIL_START, x+IVAL_TRAIL-1, IVAL_TRAIL_END(ih->data->h)+1, 
                      ih->data->light_shadow, ih->data->mid_shadow, ih->data->dark_shadow);

  if (ih->data->show_ticks > 2)
    iValDrawVerticalTicks(ih);

  if (iupdrvIsActive(ih) && ih->data->himage) 
  {
    x = (ih->data->w - ih->data->himage_w)/2;
    y = (int)(vn*(IVAL_TRAIL_LONG(ih->data->h)-2) + IVAL_TRAIL_START+1);
    cdCanvasPutImageRectMap(ih->data->cddbuffer,ih->data->himage_w, ih->data->himage_h, ih->data->himage, ih->data->himage_colors,
                      x, y-ih->data->himage_h/2, 0,0,0,0,0,0);
  } 
  else if (!iupdrvIsActive(ih) && ih->data->himageinactive) 
  {
    x = (ih->data->w - ih->data->himageinactive_w)/2;
    y = (int)(vn*(IVAL_TRAIL_LONG(ih->data->h)-2) + IVAL_TRAIL_START+1);
    cdCanvasPutImageRectMap(ih->data->cddbuffer,ih->data->himageinactive_w, ih->data->himageinactive_h, ih->data->himageinactive, 
                      ih->data->himageinactive_colors, x, y-ih->data->himageinactive_h/2, 0,0,0,0,0,0);
  } 
  else
  {
    /* handler border */
    x = (ih->data->w-IVAL_HANDLER_SHORT)/2;
    y = (int)(vn*(IVAL_TRAIL_LONG(ih->data->h)-2) + IVAL_TRAIL_START+1);
    cdIupDrawRaisenRect(ih->data->cddbuffer, x, y-IVAL_HANDLER_LONG/2-1, x+IVAL_HANDLER_SHORT-1, y+IVAL_HANDLER_LONG/2, 
                        ih->data->light_shadow, ih->data->mid_shadow, ih->data->dark_shadow);

    /* handler background */
    cdCanvasForeground(ih->data->cddbuffer, ih->data->bgcolor );
    cdCanvasBox(ih->data->cddbuffer,x+1, x+IVAL_HANDLER_SHORT-1-2, y-IVAL_HANDLER_LONG/2+1, y+IVAL_HANDLER_LONG/2-1);

    /* handler sunken mark */
    cdIupDrawHorizSunkenMark(ih->data->cddbuffer, x+2, x+IVAL_HANDLER_SHORT-4, y, ih->data->light_shadow, ih->data->dark_shadow);
  }
}
  
static void iValDrawHorizontal(Ihandle* ih)
{
  int x, y = (ih->data->h-IVAL_TRAIL)/2;
  double vn = (ih->data->val-ih->data->vmin)/(ih->data->vmax-ih->data->vmin);

  /* trail */
  cdIupDrawSunkenRect(ih->data->cddbuffer, IVAL_TRAIL_START, y, IVAL_TRAIL_END(ih->data->w)+1, y+IVAL_TRAIL-1, 
                      ih->data->light_shadow, ih->data->mid_shadow, ih->data->dark_shadow);
  
  if (ih->data->show_ticks > 2)
    iValDrawHorizontalTicks(ih);

  if (iupdrvIsActive(ih) && ih->data->himage) 
  {
    y = (ih->data->h - ih->data->himage_h)/2+1;
    x = (int)(vn*(IVAL_TRAIL_LONG(ih->data->w)-2) + IVAL_TRAIL_START+1);
    cdCanvasPutImageRectMap(ih->data->cddbuffer,ih->data->himage_w, ih->data->himage_h, ih->data->himage, ih->data->himage_colors,
                      x-ih->data->himage_w/2, y, 0,0,0,0,0,0);
  } 
  else if (!iupdrvIsActive(ih) && ih->data->himageinactive) 
  {
    y = (ih->data->h - ih->data->himageinactive_h)/2+1;
    x = (int)(vn*(IVAL_TRAIL_LONG(ih->data->w)-2) + IVAL_TRAIL_START+1);
    cdCanvasPutImageRectMap(ih->data->cddbuffer,ih->data->himageinactive_w, ih->data->himageinactive_h, ih->data->himageinactive, ih->data->himageinactive_colors,
                      x-ih->data->himageinactive_w/2, y, 0,0,0,0,0,0);
  } 
  else 
  {
    /* handler border */
    y = (ih->data->h-IVAL_HANDLER_SHORT)/2;
    x = (int)(vn*(IVAL_TRAIL_LONG(ih->data->w)-2) + IVAL_TRAIL_START+1);
    cdIupDrawRaisenRect(ih->data->cddbuffer, x-IVAL_HANDLER_LONG/2-1, y, x+IVAL_HANDLER_LONG/2, y+IVAL_HANDLER_SHORT-1, 
                        ih->data->light_shadow, ih->data->mid_shadow, ih->data->dark_shadow);

    /* handler background */
    cdCanvasForeground(ih->data->cddbuffer, ih->data->bgcolor );
    cdCanvasBox(ih->data->cddbuffer,x-IVAL_HANDLER_LONG/2, x+IVAL_HANDLER_LONG/2-2, y+1, y+IVAL_HANDLER_SHORT-1-2);

    /* handler sunken mark */
    cdIupDrawVertSunkenMark(ih->data->cddbuffer, x, y+3, y+IVAL_HANDLER_SHORT-3, ih->data->light_shadow, ih->data->dark_shadow);
  }
}

static void iValCropValue(Ihandle* ih)
{
  if (ih->data->val > ih->data->vmax) 
    ih->data->val = ih->data->vmax;
  else if (ih->data->val < ih->data->vmin) 
    ih->data->val = ih->data->vmin;
}

static void iValSetVerticalPosition(Ihandle* ih, int x, int y)
{
  double vn;
  (void)x;
  y = cdIupInvertYAxis(y, ih->data->h);
  vn = ((double)(y-IVAL_TRAIL_START))/((double)IVAL_TRAIL_LONG(ih->data->h));
  ih->data->val = vn*(ih->data->vmax-ih->data->vmin) + ih->data->vmin;
  iValCropValue(ih);
  iValRepaint(ih);
}

static void iValSetHorizontalPosition(Ihandle* ih, int x, int y)
{
  double vn;
  (void)y;
  vn = ((double)(x-IVAL_TRAIL_START))/((double)IVAL_TRAIL_LONG(ih->data->w));
  ih->data->val = vn*(ih->data->vmax-ih->data->vmin) + ih->data->vmin;
  iValCropValue(ih);
  iValRepaint(ih);
}

static int iValMotion_CB(Ihandle *ih,int x,int y,char *r)
{
  IFnd cb;

  if (!iup_isbutton1(r)) return IUP_DEFAULT;
  if (!ih->data->moving) return IUP_DEFAULT;

  ih->data->CalcPosition(ih,x,y);

  cb = (IFnd) IupGetCallback(ih, "MOUSEMOVE_CB");
  if (cb) cb(ih,ih->data->val);

  return IUP_DEFAULT;
}

static int iValButton_CB(Ihandle *ih,int b,int m,int x,int y)
{
  char* cb_name;
  IFnd cb;
  if (b!=IUP_BUTTON1) return IUP_DEFAULT;

  if (m) 
  {
    ih->data->CalcPosition(ih, x, y);
    ih->data->moving = 1;
    cb_name = "BUTTON_PRESS_CB";
  }
  else
  {
    if (!ih->data->moving) return IUP_DEFAULT;

    iValRepaint(ih);
    ih->data->moving = 0;

    cb_name = "BUTTON_RELEASE_CB";
  }

  cb = (IFnd) IupGetCallback(ih, cb_name);
  if (cb) cb(ih,ih->data->val);

  return IUP_DEFAULT;
}

static int iValResize_CB(Ihandle* ih)
{
  if (!ih->data->cddbuffer)
  {
    /* update canvas size */
    cdCanvasActivate(ih->data->cdcanvas);

    /* this can fail if canvas size is zero */
    ih->data->cddbuffer = cdCreateCanvas(CD_DBUFFER, ih->data->cdcanvas);
  }

  if (!ih->data->cddbuffer)
    return IUP_DEFAULT;

  /* update size */
  cdCanvasActivate(ih->data->cddbuffer);
  cdCanvasGetSize(ih->data->cddbuffer, &ih->data->w, &ih->data->h, NULL, NULL);

  /* update render */
  cdCanvasBackground(ih->data->cddbuffer, ih->data->bgcolor);
  cdCanvasClear(ih->data->cddbuffer);
  ih->data->Draw(ih);

  return IUP_DEFAULT;
}

static int iValRedraw_CB(Ihandle* ih)
{
  if (!ih->data->cddbuffer)
    return IUP_DEFAULT;

  /* update display */
  cdCanvasFlush(ih->data->cddbuffer);
  if (ih->data->has_focus)
    cdIupDrawFocusRect(ih, ih->data->cdcanvas, 0, 0, ih->data->w-1, ih->data->h-1);

  return IUP_DEFAULT;
}

static int iValFocus_CB (Ihandle* ih, int focus)
{
  ih->data->has_focus = focus;
  iValRepaint(ih);
  return IUP_DEFAULT;
}

static int iValKeypress_CB(Ihandle *ih, int c, int press)
{
  char* cb_name;
  IFnd cb;

  if (c != K_LEFT && c != K_UP &&
      c != K_PGDN && c != K_PGUP  &&
      c != K_RIGHT && c != K_DOWN  &&
      c != K_END && c != K_HOME &&
      c != K_sLEFT && c != K_sUP &&
      c != K_sPGDN && c != K_sPGUP  &&
      c != K_sRIGHT && c != K_sDOWN  &&
      c != K_sEND && c != K_sHOME)
    return IUP_DEFAULT;

  if (press)
  {
    switch(c)
    {
    case K_sHOME:
    case K_HOME:
      ih->data->val = ih->data->vmax;
      break;
    case K_sEND:
    case K_END:
      ih->data->val = ih->data->vmin;
      break;
    case K_sLEFT:
    case K_sDOWN:
    case K_LEFT:
    case K_DOWN:
      ih->data->val -= ih->data->step*(ih->data->vmax-ih->data->vmin);
      iValCropValue(ih);
      break;
    case K_sRIGHT:
    case K_sUP:
    case K_RIGHT:
    case K_UP:
      ih->data->val += ih->data->step*(ih->data->vmax-ih->data->vmin);
      iValCropValue(ih);
      break;
    case K_sPGDN:
    case K_PGDN:
      ih->data->val -= ih->data->pagestep*(ih->data->vmax-ih->data->vmin);
      iValCropValue(ih);
      break;
    case K_sPGUP:
    case K_PGUP:
      ih->data->val += ih->data->pagestep*(ih->data->vmax-ih->data->vmin);
      iValCropValue(ih);
      break;
    }

    if (fabs(ih->data->val-ih->data->vmin) < 0.9*ih->data->step*(ih->data->vmax-ih->data->vmin))
      ih->data->val = ih->data->vmin;

    if (ih->data->moving)
      cb_name = "MOUSEMOVE_CB";
    else
    {
      ih->data->moving = 1;
      cb_name = "BUTTON_PRESS_CB";
    }
  }
  else
  {
    ih->data->moving = 0;
    cb_name = "BUTTON_RELEASE_CB";
  }

  iValRepaint(ih);

  cb = (IFnd) IupGetCallback(ih, cb_name);
  if (cb) cb(ih,ih->data->val);

  return IUP_IGNORE;  /* to avoid arrow keys being processed by the system */
}

static int iValWheel_CB(Ihandle *ih, float delta)
{
  IFnd cb;

  ih->data->val += ((double)delta)*ih->data->step*(ih->data->vmax-ih->data->vmin);
  iValCropValue(ih);

  if (fabs(ih->data->val-ih->data->vmin) < 0.9*ih->data->step*(ih->data->vmax-ih->data->vmin))
    ih->data->val = ih->data->vmin;

  iValRepaint(ih);

  cb = (IFnd) IupGetCallback(ih, "BUTTON_PRESS_CB");
  if (cb) cb(ih,ih->data->val);

  cb = (IFnd) IupGetCallback(ih, "BUTTON_RELEASE_CB");
  if (cb) cb(ih,ih->data->val);

  return IUP_DEFAULT;
}


/*******************************************************************************/


static int iValSetTypeAttrib(Ihandle* ih, const char *value)
{
  /* valid only before map */
  if (ih->handle)
    return 0;

  if (iupStrEqualNoCase(value, "VERTICAL"))
  {
    IupSetAttribute(ih, "RASTERSIZE", "28x124");
    
    ih->data->Draw = iValDrawVertical;
    ih->data->CalcPosition = iValSetVerticalPosition;
    ih->data->type = IVAL_VERTICAL;
  }
  else /* "HORIZONTAL" */
  {
    IupSetAttribute(ih, "RASTERSIZE", "124x28");
     
    ih->data->Draw = iValDrawHorizontal;
    ih->data->CalcPosition = iValSetHorizontalPosition;
    ih->data->type = IVAL_HORIZONTAL;
  }

  return 0; /* do not store value in hash table */
}

static char* iValGetTypeAttrib(Ihandle* ih)
{
  if (ih->data->type == IVAL_HORIZONTAL)
    return "HORIZONTAL";
  else /* (ih->data->type == IVAL_VERTICAL) */
    return "VERTICAL";
}

static char* iValGetValueAttrib(Ihandle* ih)
{
  char* str = iupStrGetMemory(20);
  sprintf(str, "%g", ih->data->val);
  return str;
}

static int iValSetValueAttrib(Ihandle* ih, const char* value)
{
  if (!value)
    ih->data->val = 0;
  else
    ih->data->val = atof(value);
  iValCropValue(ih);
  iValRepaint(ih);
  return 0; /* do not store value in hash table */
}

static int iValSetShowTicksAttrib(Ihandle* ih, const char* showticks)
{
  int i;
  if (iupStrToInt(showticks, &i))
  {
    ih->data->show_ticks = i;
    iValRepaint(ih);
  }
  return 0; /* do not store value in hash table */
}

static char* iValGetShowTicksAttrib(Ihandle* ih)
{
  char* str = iupStrGetMemory(20);
  sprintf(str, "%d", ih->data->show_ticks);
  return str;
}

static int iValSetStepAttrib(Ihandle* ih, const char* step)
{
  ih->data->step = atof(step);
  iValRepaint(ih);
  return 0; /* do not store value in hash table */
}

static char* iValGetStepAttrib(Ihandle* ih)
{
  char* str = iupStrGetMemory(20);
  sprintf(str, "%g", ih->data->step);
  return str;
}

static int iValSetPageStepAttrib(Ihandle* ih, const char* pagestep)
{
  ih->data->step = atof(pagestep);
  iValRepaint(ih);
  return 0; /* do not store value in hash table */
}

static char* iValGetPageStepAttrib(Ihandle* ih)
{
  char* str = iupStrGetMemory(20);
  sprintf(str, "%g", ih->data->pagestep);
  return str;
}

static int iValSetMaxAttrib(Ihandle* ih, const char* max)
{
  ih->data->vmax = atof(max);
  iValCropValue(ih);
  iValRepaint(ih);
  return 0; /* do not store value in hash table */
}

static char* iValGetMaxAttrib(Ihandle* ih)
{
  char* str = iupStrGetMemory(20);
  sprintf(str, "%g", ih->data->vmax);
  return str;
}

static int iValSetMinAttrib(Ihandle* ih, const char* min)
{
  ih->data->vmin = atof(min);
  iValCropValue(ih);
  iValRepaint(ih);
  return 0; /* do not store value in hash table */
}

static char* iValGetMinAttrib(Ihandle* ih)
{
  char* str = iupStrGetMemory(20);
  sprintf(str, "%g", ih->data->vmin);
  return str;
}

static int iValSetHandlerImageAttrib(Ihandle* ih, const char* value)
{
  if (!value) 
  {
    if (ih->data->himage) 
    {
      free(ih->data->himage);
      free(ih->data->himage_colors);
      ih->data->himage = NULL;
      ih->data->himage_colors = NULL;
    }
  } 
  else 
  {
    iValSetImage(value, &(ih->data->himage), &(ih->data->himage_colors), 
                        &(ih->data->himage_w), &(ih->data->himage_h), &(ih->data->himage_numcolors));
  }

  iValRepaint(ih);
  return 1;
}

static int iValSetHandlerImageInactiveAttrib(Ihandle* ih, const char* value)
{
    if (value==NULL) 
    {
      if (ih->data->himageinactive) 
      {
        free(ih->data->himageinactive);
        free(ih->data->himageinactive_colors);
        ih->data->himageinactive = NULL;
        ih->data->himageinactive_colors = NULL;
      }
    } 
    else
    {
      iValSetImage(value, &(ih->data->himageinactive), &(ih->data->himageinactive_colors), 
                          &(ih->data->himageinactive_w), &(ih->data->himageinactive_h), &(ih->data->himageinactive_numcolors));
    }

  iValRepaint(ih);
  return 1;
}

static int iValSetBgColorAttrib(Ihandle* ih, const char* value)
{
  if (!value)
    value = iupControlBaseGetParentBgColor(ih);

  ih->data->bgcolor = cdIupConvertColor(value);

  cdIupCalcShadows(ih->data->bgcolor, &ih->data->light_shadow, &ih->data->mid_shadow, &ih->data->dark_shadow);
  if (!iupdrvIsActive(ih) && !ih->data->himage)
    ih->data->light_shadow = ih->data->mid_shadow;

  if (ih->data->himage)
    iValUpdateImageBgColor(ih->data->himage_colors, ih->data->himage_numcolors, ih->data->bgcolor);
  if (ih->data->himageinactive)
    iValUpdateImageBgColor(ih->data->himageinactive_colors, ih->data->himageinactive_numcolors, ih->data->bgcolor);
  
  iValRepaint(ih);
  return 1;
}

static int iValSetActiveAttrib(Ihandle* ih, const char* value)
{
  iupBaseSetActiveAttrib(ih, value);

  cdIupCalcShadows(ih->data->bgcolor, &ih->data->light_shadow, &ih->data->mid_shadow, &ih->data->dark_shadow);
  if (!iupdrvIsActive(ih) && !ih->data->himage)
    ih->data->light_shadow = ih->data->mid_shadow;

  iValRepaint(ih);
  return 0;   /* do not store value in hash table */
}


/************************************************************************************/


static int iValMapMethod(Ihandle* ih)
{
  ih->data->cdcanvas = cdCreateCanvas(CD_IUP, ih);
  if (!ih->data->cdcanvas)
    return IUP_ERROR;

  /* this can fail if canvas size is zero */
  ih->data->cddbuffer = cdCreateCanvas(CD_DBUFFER, ih->data->cdcanvas);

  return IUP_NOERROR;
}

static void iValUnMapMethod(Ihandle* ih)
{
  if (ih->data->cddbuffer)
  {
    cdKillCanvas(ih->data->cddbuffer);
    ih->data->cddbuffer = NULL;
  }

  if (ih->data->cdcanvas)
  {
    cdKillCanvas(ih->data->cdcanvas);
    ih->data->cdcanvas = NULL;
  }
}

static void iValDestroyMethod(Ihandle* ih)
{
  if (ih->data->himage) free(ih->data->himage);
  if (ih->data->himage_colors) free(ih->data->himage_colors);
  if (ih->data->himageinactive) free(ih->data->himageinactive);
  if (ih->data->himageinactive_colors) free(ih->data->himageinactive_colors);
}

static int iValCreateMethod(Ihandle* ih, void **params)
{
  if (params && params[0])
    iupAttribStoreStr(ih, "TYPE", params[0]);

  /* free the data alocated by IupCanvas */
  if (ih->data) free(ih->data);
  ih->data = iupALLOCCTRLDATA();

  /* change the IupCanvas default values */
  iupAttribSetStr(ih, "BORDER", "NO");
  ih->expand = IUP_EXPAND_NONE;

  /* default values */
  iValSetTypeAttrib(ih, "HORIZONTAL");
  ih->data->vmax = 1;
  ih->data->step = 0.01;
  ih->data->pagestep = 0.1;

  IupSetCallback(ih,"RESIZE_CB",(Icallback)iValResize_CB);
  IupSetCallback(ih,"ACTION",(Icallback)iValRedraw_CB);
  IupSetCallback(ih,"BUTTON_CB",(Icallback)iValButton_CB);
  IupSetCallback(ih,"MOTION_CB",(Icallback)iValMotion_CB);
  IupSetCallback(ih,"FOCUS_CB",(Icallback)iValFocus_CB);
  IupSetCallback(ih,"WHEEL_CB",(Icallback)iValWheel_CB);
  IupSetCallback(ih,"KEYPRESS_CB",(Icallback)iValKeypress_CB);

  return IUP_NOERROR;    
}

static Iclass* iValGetClass(void)
{
  Iclass* ic = iupClassNew(iupCanvasGetClass());

  ic->name = "val";
  ic->format = "S"; /* one optional string */
  ic->nativetype = IUP_TYPECANVAS;
  ic->childtype = IUP_CHILDNONE;
  ic->is_interactive = 1;

  /* Class functions */
  ic->Create  = iValCreateMethod;
  ic->Destroy = iValDestroyMethod;
  ic->Map = iValMapMethod;
  ic->UnMap = iValUnMapMethod;
  
  /* IupVal Callbacks */
  iupClassRegisterCallback(ic, "MOUSEMOVE_CB", "d");
  iupClassRegisterCallback(ic, "BUTTON_PRESS_CB", "d");
  iupClassRegisterCallback(ic, "BUTTON_RELEASE_CB", "d");

  /* IupVal only */
  iupClassRegisterAttribute(ic, "VALUE", iValGetValueAttrib, iValSetValueAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "TYPE", iValGetTypeAttrib, iValSetTypeAttrib, IUPAF_SAMEASSYSTEM, "HORIZONTAL", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "SHOWTICKS", iValGetShowTicksAttrib, iValSetShowTicksAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "STEP", iValGetStepAttrib, iValSetStepAttrib, IUPAF_SAMEASSYSTEM, "0.01", IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "PAGESTEP", iValGetPageStepAttrib, iValSetPageStepAttrib, IUPAF_SAMEASSYSTEM, "0.1", IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "MAX", iValGetMaxAttrib, iValSetMaxAttrib, IUPAF_SAMEASSYSTEM, "1.0", IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "MIN", iValGetMinAttrib, iValSetMinAttrib, IUPAF_SAMEASSYSTEM, "0.0", IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "HANDLER_IMAGE", NULL, iValSetHandlerImageAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "HANDLER_IMAGE_INACTIVE", NULL, iValSetHandlerImageInactiveAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NOT_MAPPED);

  /* Overwrite IupCanvas Attributes */
  iupClassRegisterAttribute(ic, "ACTIVE", iupBaseGetActiveAttrib, iValSetActiveAttrib, IUPAF_SAMEASSYSTEM, "YES", IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "BGCOLOR", iupControlBaseGetBgColorAttrib, iValSetBgColorAttrib, NULL, "255 255 255", IUPAF_NO_INHERIT);    /* overwrite canvas implementation, set a system default to force a new default */

  return ic;
}

void IupOldValOpen(void)
{
  iupRegisterClass(iValGetClass());
}
