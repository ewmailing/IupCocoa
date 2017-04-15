/** \file
 * \brief Scintilla control: Annotation
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


/***** Annotation *****
SCI_ANNOTATIONSETTEXT(int line, char *text)
SCI_ANNOTATIONGETTEXT(int line, char *text)
SCI_ANNOTATIONSETSTYLE(int line, int style)
SCI_ANNOTATIONGETSTYLE(int line)
-- SCI_ANNOTATIONSETSTYLES(int line, char *styles)
-- SCI_ANNOTATIONGETSTYLES(int line, char *styles)
-- SCI_ANNOTATIONGETLINES(int line)
SCI_ANNOTATIONCLEARALL
SCI_ANNOTATIONSETVISIBLE(int visible)
SCI_ANNOTATIONGETVISIBLE
SCI_ANNOTATIONSETSTYLEOFFSET(int style)
SCI_ANNOTATIONGETSTYLEOFFSET
*/

static int iScintillaSetAnnotationTextAttribId(Ihandle* ih, int line, const char* value)
{
  IupScintillaSendMessage(ih, SCI_ANNOTATIONSETTEXT, line, (sptr_t)value);
  return 0;
}

static char* iScintillaGetAnnotationTextAttribId(Ihandle* ih, int line)
{
  int len = IupScintillaSendMessage(ih, SCI_ANNOTATIONGETTEXT, line, 0);
  char* str = iupStrGetMemory(len+1);
  IupScintillaSendMessage(ih, SCI_ANNOTATIONGETTEXT, line, (sptr_t)str);
  return str;
}

static int iScintillaSetAnnotationStyleAttribId(Ihandle* ih, int line, const char* value)
{
  int style;
  iupStrToInt(value, &style);
  IupScintillaSendMessage(ih, SCI_ANNOTATIONSETSTYLE, line, style);
  return 0;
}

static char* iScintillaGetAnnotationStyleAttribId(Ihandle* ih, int line)
{
  int style = IupScintillaSendMessage(ih, SCI_ANNOTATIONGETSTYLE, line, 0);
  return iupStrReturnInt(style);
}

static int iScintillaSetAnnotationStyleOffsetAttrib(Ihandle* ih, const char* value)
{
  int style_offset;
  iupStrToInt(value, &style_offset);
  IupScintillaSendMessage(ih, SCI_ANNOTATIONSETSTYLEOFFSET, style_offset, 0);
  return 0;
}

static char* iScintillaGetAnnotationStyleOffsetAttrib(Ihandle* ih)
{
  int style = IupScintillaSendMessage(ih, SCI_ANNOTATIONGETSTYLEOFFSET, 0, 0);
  return iupStrReturnInt(style);
}

static int iScintillaSetAnnotationVisibleAttrib(Ihandle *ih, const char *value)
{
  if (iupStrEqualNoCase(value, "STANDARD"))
    IupScintillaSendMessage(ih, SCI_ANNOTATIONSETVISIBLE, ANNOTATION_STANDARD, 0);
  else if (iupStrEqualNoCase(value, "BOXED"))
    IupScintillaSendMessage(ih, SCI_ANNOTATIONSETVISIBLE, ANNOTATION_BOXED, 0);
  else  /* "HIDDEN" */
    IupScintillaSendMessage(ih, SCI_ANNOTATIONSETVISIBLE, ANNOTATION_HIDDEN, 0);
  return 0;
}

static char* iScintillaGetAnnotationVisibleAttrib(Ihandle* ih)
{
  if (IupScintillaSendMessage(ih, SCI_ANNOTATIONGETVISIBLE, 0, 0) == ANNOTATION_STANDARD)
    return "STANDARD";
  else if (IupScintillaSendMessage(ih, SCI_ANNOTATIONGETVISIBLE, 0, 0) == ANNOTATION_BOXED)
    return "BOXED";
  else  /* ANNOTATION_HIDDEN */
    return "HIDDEN";
}

static int iScintillaSetAnnotationClearAllAttrib(Ihandle* ih, const char* value)
{
  (void)value;
  IupScintillaSendMessage(ih, SCI_ANNOTATIONCLEARALL, 0, 0);
  return 0;
}

void iupScintillaRegisterAnnotation(Iclass* ic)
{
  iupClassRegisterAttributeId(ic, "ANNOTATIONTEXT", iScintillaGetAnnotationTextAttribId, iScintillaSetAnnotationTextAttribId, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "ANNOTATIONSTYLE", iScintillaGetAnnotationStyleAttribId, iScintillaSetAnnotationStyleAttribId, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic,   "ANNOTATIONSTYLEOFFSET", iScintillaGetAnnotationStyleOffsetAttrib, iScintillaSetAnnotationStyleOffsetAttrib, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic,   "ANNOTATIONVISIBLE", iScintillaGetAnnotationVisibleAttrib, iScintillaSetAnnotationVisibleAttrib, IUPAF_SAMEASSYSTEM, "HIDDEN", IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic,   "ANNOTATIONCLEARALL", NULL, iScintillaSetAnnotationClearAllAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
}
