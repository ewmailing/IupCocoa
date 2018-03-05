/** \file
 * \brief Scintilla control: Cursor and Zooming
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#include <Scintilla.h>

#include "iup.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"

#include "iupsci.h"

/***** CURSOR ****
SCI_SETCURSOR(int curType)
SCI_GETCURSOR

SCI_SETMOUSEDWELLTIME(int milliseconds)
SCI_GETMOUSEDWELLTIME
*/

static int iScintillaSetMouseDWellTimeAttrib(Ihandle *ih, const char *value)
{
  int time;
  if (iupStrToInt(value, &time))
    IupScintillaSendMessage(ih, SCI_SETMOUSEDWELLTIME, time, 0);
  else
    IupScintillaSendMessage(ih, SCI_SETMOUSEDWELLTIME, SC_TIME_FOREVER, 0);
  
  return 0;
}

static char* iScintillaGetMouseDWellTimeAttrib(Ihandle* ih)
{
  int time = (int)IupScintillaSendMessage(ih, SCI_SETMOUSEDWELLTIME, 0, 0);
  return iupStrReturnInt(time);
}

static char* iScintillaGetCursorAttrib(Ihandle *ih)
{
  if(IupScintillaSendMessage(ih, SCI_GETCURSOR, 0, 0) == SC_CURSORWAIT)
    return "WAIT";
  else
    return "NORMAL";
}

static int iScintillaSetCursorAttrib(Ihandle *ih, const char *value)
{
  if (iupStrEqualNoCase(value, "WAIT"))
    IupScintillaSendMessage(ih, SCI_SETCURSOR, (uptr_t)SC_CURSORWAIT, 0);
  else  /* NORMAL */
    IupScintillaSendMessage(ih, SCI_SETCURSOR, (uptr_t)SC_CURSORNORMAL, 0);

  return 0;
}

/***** ZOOMING ****
SCI_ZOOMIN
SCI_ZOOMOUT
SCI_SETZOOM(int zoomInPoints)
SCI_GETZOOM
*/

static int iScintillaSetZoomInAttrib(Ihandle *ih, const char *value)
{
  (void)value;
  IupScintillaSendMessage(ih, SCI_ZOOMIN, 0, 0);
  return 0;
}

static int iScintillaSetZoomOutAttrib(Ihandle *ih, const char *value)
{
  (void)value;
  IupScintillaSendMessage(ih, SCI_ZOOMOUT, 0, 0);
  return 0;
}

static int iScintillaSetZoomAttrib(Ihandle *ih, const char *value)
{
  int points;
  if (!iupStrToInt(value, &points))
    return 0;

  if(points >  20) points =  20;
  if(points < -10) points = -10;

  IupScintillaSendMessage(ih, SCI_SETZOOM, points, 0);

  return 0;
}

static char* iScintillaGetZoomAttrib(Ihandle* ih)
{
  int points = (int)IupScintillaSendMessage(ih, SCI_GETZOOM, 0, 0);
  return iupStrReturnInt(points);
}

void iupScintillaRegisterCursor(Iclass* ic)
{
  iupClassRegisterAttribute(ic, "CURSOR",  iScintillaGetCursorAttrib, iScintillaSetCursorAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MOUSEDWELLTIME", iScintillaGetMouseDWellTimeAttrib, iScintillaSetMouseDWellTimeAttrib, NULL, NULL, IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "ZOOMIN",  NULL, iScintillaSetZoomInAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ZOOMOUT", NULL, iScintillaSetZoomOutAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ZOOM",    iScintillaGetZoomAttrib, iScintillaSetZoomAttrib, NULL, NULL, IUPAF_NO_INHERIT);
}
