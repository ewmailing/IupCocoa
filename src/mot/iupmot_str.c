/** \file
 * \brief Motif Base Functions
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <Xm/Xm.h>
#include <Xm/Text.h>

#include "iup.h"

#include "iup_object.h"
#include "iup_key.h"
#include "iup_str.h"

#include "iupmot_drv.h"


void iupmotSetMnemonicTitle(Ihandle *ih, Widget w, int pos, const char* value)
{
  char c;
  char* str;

  if (!value) 
    value = "";

  if (!w)
    w = ih->handle;

  str = iupStrProcessMnemonic(value, &c, -1);  /* remove & and return in c */
  if (str != value)
  {
    KeySym keysym = iupmotKeyCharToKeySym(c);
    XtVaSetValues(w, XmNmnemonic, keysym, NULL);   /* works only for menus, but underlines the letter */

    if (ih->iclass->nativetype != IUP_TYPEMENU)
      iupKeySetMnemonic(ih, c, pos);

    iupmotSetXmString(w, XmNlabelString, str);
    free(str);
  }
  else
  {
    XtVaSetValues (w, XmNmnemonic, NULL, NULL);
    iupmotSetXmString(w, XmNlabelString, str);
  }
}

void iupmotSetXmString(Widget w, const char *resource, const char* value)
{
  XmString xm_str = iupmotStringCreate(value);
  XtVaSetValues(w, resource, xm_str, NULL);
  XmStringFree(xm_str);
}

char* iupmotGetXmString(XmString str)
{
  return (char*)XmStringUnparse(str, NULL, XmCHARSET_TEXT, XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);
}

char* iupmotReturnXmString(XmString str)
{
  char* text = iupmotGetXmString(str);
  char* ret = iupStrReturnStr(text);
  XtFree(text);
  return ret;
}

/* TODO: UTF8 support would start here... */

XmString iupmotStringCreate(const char *value)
{
  return XmStringCreateLocalized((String)value);
}

void iupmotSetTitle(Widget w, const char *value)
{
  XtVaSetValues(w, XmNtitle, value, 
                   XmNiconName, value, 
                   NULL);
}

void iupmotTextSetString(Widget w, const char *value)
{
  XmTextSetString(w, (char*)value);
}
