/** \file
 * \brief Tree Control
 *
 * See Copyright Notice in iup.h
 */

#define UNIMPLEMENTED printf("%s (%s %d) UNIMPLEMENTED\n",__func__,__FILE__,__LINE__);

#include <OutlineListView.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdarg.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_layout.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_stdcontrols.h"
#include "iup_key.h"
#include "iup_image.h"
#include "iup_array.h"
#include "iup_tree.h"

#include "iup_drvinfo.h"
#include "iuphaiku_drv.h"


class IUPListItem: public BStringItem
{
	public:
		IUPListItem(const char* title, int level)
			: BStringItem(title, level)
		{}

		int kind;
};


/*****************************************************************************/
/* MANIPULATING IMAGES                                                       */
/*****************************************************************************/
static void gtkTreeUpdateImages(Ihandle* ih, int mode)
{
	UNIMPLEMENTED
}

/*****************************************************************************/
/* ADDING ITEMS                                                              */
/*****************************************************************************/
void iupdrvTreeAddNode(Ihandle* ih, int id, int kind, const char* title, int add)
{
  BOutlineListView* list = (BOutlineListView*)ih->handle;
  int kindPrev = -1;
  int level = 0;

  IUPListItem* parent = (IUPListItem*)list->FullListItemAt(id);

  // Only the root node is allowed to have no parent
  if(parent == NULL && id != -1)
	return;
  else if(parent) {
    kindPrev = parent->kind;
  }

  if (kindPrev != -1)
  { 
    level = parent->OutlineLevel();
    // The parent has a valid kind
    if (kindPrev == ITREE_BRANCH && add)
	  level++; // Adding item inside the branch
    else
	  id += list->CountItemsUnder(parent, true); // Adding item as a sibling of 'parent'
  }
    
  IUPListItem* item = new IUPListItem(title, level);
  item->kind = kind;

  bool unlock = list->LockLooper();
  	// No need to lock if the list is currently unmapped (happens when it was
	// added to a BTabView, but that one is hidden)
  list->AddItem(item, id + 1);
  if (unlock) list->UnlockLooper();

  // TODO FGCOLOR, default images, ADDEXPANDED
}


/*****************************************************************************/
/* AUXILIAR FUNCTIONS                                                        */
/*****************************************************************************/


int iupdrvTreeTotalChildCount(Ihandle* ih, InodeHandle* node_handle)
{
  int count = 0;
  UNIMPLEMENTED
  return count;
}

InodeHandle* iupdrvTreeGetFocusNode(Ihandle* ih)
{
	UNIMPLEMENTED
  return NULL;
}

typedef struct _gtkTreeSelectMinMax
{
  Ihandle* ih;
  int id1, id2;
} gtkTreeSelectMinMax;

/*****************************************************************************/
/* GET AND SET ATTRIBUTES                                                    */
/*****************************************************************************/


static char* gtkTreeGetIndentationAttrib(Ihandle* ih)
{
	UNIMPLEMENTED
		return NULL;
}

static int gtkTreeSetIndentationAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 0;
}

static int gtkTreeSetTopItemAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 0;
}

static int gtkTreeSetSpacingAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
    return 1; /* store until not mapped, when mapped will be set again */
}

static int gtkTreeSetExpandAllAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 0;
}

static char* gtkTreeGetDepthAttrib(Ihandle* ih, int id)
{
	UNIMPLEMENTED
		return NULL;
}

static int gtkTreeSetMoveNodeAttrib(Ihandle* ih, int id, const char* value)
{
	UNIMPLEMENTED
  return 0;
}

static int gtkTreeSetCopyNodeAttrib(Ihandle* ih, int id, const char* value)
{
	UNIMPLEMENTED
  return 0;
}

static char* gtkTreeGetColorAttrib(Ihandle* ih, int id)
{
	UNIMPLEMENTED
		return NULL;
}

static int gtkTreeSetColorAttrib(Ihandle* ih, int id, const char* value)
{
	UNIMPLEMENTED
  return 0;
}

static char* gtkTreeGetParentAttrib(Ihandle* ih, int id)
{
	UNIMPLEMENTED
		return NULL;
}

static char* gtkTreeGetChildCountAttrib(Ihandle* ih, int id)
{
	UNIMPLEMENTED
		return NULL;
}

static char* gtkTreeGetKindAttrib(Ihandle* ih, int id)
{
	UNIMPLEMENTED
		return NULL;
}

static char* gtkTreeGetStateAttrib(Ihandle* ih, int id)
{
	UNIMPLEMENTED
  return NULL;
}

static int gtkTreeSetStateAttrib(Ihandle* ih, int id, const char* value)
{
	UNIMPLEMENTED
  return 0;
}

static char* gtkTreeGetTitleAttrib(Ihandle* ih, int id)
{
	UNIMPLEMENTED
    return NULL;
}

static int gtkTreeSetTitleAttrib(Ihandle* ih, int id, const char* value)
{
  BOutlineListView* lv = (BOutlineListView*)ih->handle;
  BStringItem* item = (BStringItem*)lv->FullListItemAt(id);
  item->SetText(value);
  return 0;
}

static int gtkTreeSetTitleFontAttrib(Ihandle* ih, int id, const char* value)
{
	UNIMPLEMENTED
  return 0;
}

static char* gtkTreeGetTitleFontAttrib(Ihandle* ih, int id)
{
	UNIMPLEMENTED
    return NULL;
}

static char* gtkTreeGetValueAttrib(Ihandle* ih)
{
	UNIMPLEMENTED
    return NULL;
}

static char* gtkTreeGetMarkedNodesAttrib(Ihandle* ih)
{
	UNIMPLEMENTED
    return NULL;
}

static int gtkTreeSetMarkedNodesAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 0;
}

static int gtkTreeSetMarkAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 1;
} 

static int gtkTreeSetValueAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 0;
} 

static int gtkTreeSetMarkStartAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 1;
}

static char* gtkTreeGetMarkedAttrib(Ihandle* ih, int id)
{
	UNIMPLEMENTED
    return NULL;
}

static int gtkTreeSetMarkedAttrib(Ihandle* ih, int id, const char* value)
{
	UNIMPLEMENTED
  return 0;
}

static char* gtkTreeGetToggleValueAttrib(Ihandle* ih, int id)
{
	UNIMPLEMENTED
    return NULL;
}

static int gtkTreeSetToggleValueAttrib(Ihandle* ih, int id, const char* value)
{
	UNIMPLEMENTED
  return 0;
}

static int gtkTreeSetDelNodeAttrib(Ihandle* ih, int id, const char* value)
{
	UNIMPLEMENTED
  return 0;
}

static int gtkTreeSetRenameAttrib(Ihandle* ih, const char* value)
{  
	UNIMPLEMENTED
  return 0;
}

static int gtkTreeSetImageExpandedAttrib(Ihandle* ih, int id, const char* value)
{
	UNIMPLEMENTED
  return 1;
}

static int gtkTreeSetImageAttrib(Ihandle* ih, int id, const char* value)
{
	UNIMPLEMENTED
  return 0;
}

static int gtkTreeSetImageBranchExpandedAttrib(Ihandle* ih, const char* value)
{
  ih->data->def_image_expanded = iupImageGetImage(value, ih, 0);

  /* Update all images */
  gtkTreeUpdateImages(ih, ITREE_UPDATEIMAGE_EXPANDED);

  return 1;
}

static int gtkTreeSetImageBranchCollapsedAttrib(Ihandle* ih, const char* value)
{
  ih->data->def_image_collapsed = iupImageGetImage(value, ih, 0);

  /* Update all images */
  gtkTreeUpdateImages(ih, ITREE_UPDATEIMAGE_COLLAPSED);

  return 1;
}

static int gtkTreeSetImageLeafAttrib(Ihandle* ih, const char* value)
{
  ih->data->def_image_leaf = iupImageGetImage(value, ih, 0);

  /* Update all images */
  gtkTreeUpdateImages(ih, ITREE_UPDATEIMAGE_LEAF);

  return 1;
}

static int gtkTreeSetBgColorAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 1;
}

static int gtkTreeSetFgColorAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 1;
}

static int gtkTreeSetShowRenameAttrib(Ihandle* ih, const char* value)
{
	UNIMPLEMENTED
  return 0;
}

void iupdrvTreeUpdateMarkMode(Ihandle *ih)
{
  BListView* lv = (BListView*)ih->handle;
  if(ih->data->mark_mode == ITREE_MARK_SINGLE)
  {
	lv->SetListType(B_SINGLE_SELECTION_LIST);
  } else {
	lv->SetListType(B_MULTIPLE_SELECTION_LIST);
  }
}


/*****************************************************************************/

static int haikuTreeMapMethod(Ihandle* ih)
{
  ih->handle = (InativeHandle*)new BOutlineListView(BRect(0, 0, 10, 10), "IUPOutline");

  if(ih->data->show_toggle)
	UNIMPLEMENTED
  if (ih->data->show_rename)
	UNIMPLEMENTED
  if (ih->data->show_dragdrop)
	UNIMPLEMENTED

  iuphaikuBaseAddToParent(ih);

  /*
  if (!iupAttribGetBoolean(ih, "CANFOCUS"))
    iuphaikuSetCanFocus(ih->handle, 0);
  */

  if (iupAttribGetInt(ih, "ADDROOT"))
    iupdrvTreeAddNode(ih, -1, ITREE_BRANCH, "", 0);
  
  /* configure for DRAG&DROP of files */
  if (IupGetCallback(ih, "DROPFILES_CB"))
    iupAttribSet(ih, "DROPFILESTARGET", "YES");

//  IupSetCallback(ih, "_IUP_XY2POS_CB", (Icallback)gtkTreeConvertXYToPos);
  iupdrvTreeUpdateMarkMode(ih);

  return IUP_NOERROR;
}

static void haikuTreeUnMapMethod(Ihandle* ih)
{
  ih->data->node_count = 0;

  iupdrvBaseUnMapMethod(ih);
}

void iupdrvTreeInitClass(Iclass* ic)
{
  /* Driver Dependent Class functions */
  ic->Map = haikuTreeMapMethod;
  ic->UnMap = haikuTreeUnMapMethod;

  /* Visual */
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, gtkTreeSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "TXTBGCOLOR", IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "FGCOLOR", NULL, gtkTreeSetFgColorAttrib, IUPAF_SAMEASSYSTEM, "TXTFGCOLOR", IUPAF_DEFAULT);

  /* IupTree Attributes - GENERAL */
  iupClassRegisterAttribute(ic, "EXPANDALL", NULL, gtkTreeSetExpandAllAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "INDENTATION", gtkTreeGetIndentationAttrib, gtkTreeSetIndentationAttrib, NULL, NULL, IUPAF_DEFAULT);
  iupClassRegisterAttribute(ic, "SPACING", iupTreeGetSpacingAttrib, gtkTreeSetSpacingAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "TOPITEM", NULL, gtkTreeSetTopItemAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);

  /* IupTree Attributes - IMAGES */
  iupClassRegisterAttributeId(ic, "IMAGE", NULL, gtkTreeSetImageAttrib, IUPAF_IHANDLENAME|IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "IMAGEEXPANDED", NULL, gtkTreeSetImageExpandedAttrib, IUPAF_IHANDLENAME|IUPAF_WRITEONLY|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "IMAGELEAF",            NULL, gtkTreeSetImageLeafAttrib, IUPAF_SAMEASSYSTEM, "IMGLEAF", IUPAF_IHANDLENAME|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGEBRANCHCOLLAPSED", NULL, gtkTreeSetImageBranchCollapsedAttrib, IUPAF_SAMEASSYSTEM, "IMGCOLLAPSED", IUPAF_IHANDLENAME|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "IMAGEBRANCHEXPANDED",  NULL, gtkTreeSetImageBranchExpandedAttrib, IUPAF_SAMEASSYSTEM, "IMGEXPANDED", IUPAF_IHANDLENAME|IUPAF_NO_INHERIT);

  /* IupTree Attributes - NODES */
  iupClassRegisterAttributeId(ic, "STATE",  gtkTreeGetStateAttrib,  gtkTreeSetStateAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "DEPTH",  gtkTreeGetDepthAttrib,  NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "KIND",   gtkTreeGetKindAttrib,   NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "PARENT", gtkTreeGetParentAttrib, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "COLOR",  gtkTreeGetColorAttrib,  gtkTreeSetColorAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "NAME",   gtkTreeGetTitleAttrib,  gtkTreeSetTitleAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "TITLE",  gtkTreeGetTitleAttrib,  gtkTreeSetTitleAttrib, IUPAF_NO_INHERIT);
  
  /* Change the set method for GTK */
  iupClassRegisterReplaceAttribFunc(ic, "SHOWRENAME", NULL, gtkTreeSetShowRenameAttrib);

  iupClassRegisterAttributeId(ic, "CHILDCOUNT", gtkTreeGetChildCountAttrib, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "TITLEFONT",  gtkTreeGetTitleFontAttrib,  gtkTreeSetTitleFontAttrib, IUPAF_NO_INHERIT);

  /* IupTree Attributes - MARKS */
  iupClassRegisterAttributeId(ic, "TOGGLEVALUE", gtkTreeGetToggleValueAttrib, gtkTreeSetToggleValueAttrib, IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "MARKED", gtkTreeGetMarkedAttrib, gtkTreeSetMarkedAttrib, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute  (ic, "MARK",      NULL, gtkTreeSetMarkAttrib,      NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute  (ic, "STARTING",  NULL, gtkTreeSetMarkStartAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute  (ic, "MARKSTART", NULL, gtkTreeSetMarkStartAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute  (ic, "MARKEDNODES", gtkTreeGetMarkedNodesAttrib, gtkTreeSetMarkedNodesAttrib, NULL, NULL, IUPAF_NO_SAVE|IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute  (ic, "VALUE", gtkTreeGetValueAttrib, gtkTreeSetValueAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);

  /* IupTree Attributes - ACTION */
  iupClassRegisterAttributeId(ic, "DELNODE", NULL, gtkTreeSetDelNodeAttrib, IUPAF_NOT_MAPPED|IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "RENAME", NULL, gtkTreeSetRenameAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "MOVENODE", NULL, gtkTreeSetMoveNodeAttrib, IUPAF_NOT_MAPPED|IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "COPYNODE", NULL, gtkTreeSetCopyNodeAttrib, IUPAF_NOT_MAPPED|IUPAF_WRITEONLY|IUPAF_NO_INHERIT);

  /* IupTree Attributes - GTK Only */
  iupClassRegisterAttribute  (ic, "RUBBERBAND", NULL, NULL, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NO_INHERIT);
}
