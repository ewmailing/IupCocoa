/** \file
 * \brief Control tree hierarchy manager.  
 * implements also IupDestroy
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h> 

#include "iup.h"

#include "iup_object.h"
#include "iup_dlglist.h"
#include "iup_childtree.h"
#include "iup_class.h"
#include "iup_attrib.h" 
#include "iup_assert.h" 
#include "iup_str.h" 
#include "iup_drv.h" 


Ihandle* IupGetDialog(Ihandle* ih)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return NULL;

  for (ih = ih; ih->parent; ih = ih->parent)
    ; /* empty*/

  if (ih->iclass->nativetype == IUP_TYPEDIALOG)
    return ih;
  else if (ih->iclass->nativetype == IUP_TYPEMENU)
  {
    Ihandle *dlg;
    /* if ih is a menu then */
    /* searches all the dialogs that may have been associated with the menu. */
    for (dlg = iupDlgListFirst(); dlg; dlg = iupDlgListNext())
    {
      if (IupGetAttributeHandle(dlg, "MENU") == ih)
        return dlg;
    }
  }

  return NULL;
}

static void iChildTreeDetach(Ihandle* parent, Ihandle* child)
{
  Ihandle *c, 
          *c_prev = NULL;

  /* Removes the child entry inside the parent's child list */
  for (c = parent->firstchild; c; c = c->brother)
  {
    if (c == child) /* Found the right child */
    {
      if (c_prev == NULL)
        parent->firstchild = child->brother;
      else
        c_prev->brother = child->brother;
        
      child->brother = NULL;
      child->parent = NULL;
      return;
    }

    c_prev = c;
  }
}

void IupDetach(Ihandle *child)
{
  Ihandle *parent, *top_parent;
  int pos;

  iupASSERT(iupObjectCheck(child));
  if (!iupObjectCheck(child))
    return;

  IupUnmap(child);

  /* Not valid if does NOT has a parent */
  if (!child->parent)
    return;

  parent = child->parent;
  top_parent = iupChildTreeGetNativeParent(child);

  pos = IupGetChildPos(parent, child);

  iChildTreeDetach(parent, child);
  iupClassObjectChildRemoved(parent, child, pos);

  /* notify also internal parents up to the native parent 
     TODO: this is weird, do we still need it? */
  while (parent && parent != top_parent)
  {
    parent = parent->parent;
    if (parent)
      iupClassObjectChildRemoved(parent, child, pos);
  }
}

#ifdef IUP_ASSERT
static int iChildTreeFindRec(Ihandle* parent, Ihandle* child)
{
  Ihandle *c;

  /* Finds the reference child entry inside the parent's child list */
  for (c = parent->firstchild; c; c = c->brother)
  {
    if (c == child) /* Found the right child */
      return 1;

    if (iChildTreeFindRec(c, child))
      return 1;
  }

  return 0;
}

static int iChildTreeCheckInside(Ihandle* parent, Ihandle* child)
{
  /* top parent */
  while (parent->parent)
    parent = parent->parent;

  return iChildTreeFindRec(parent, child);
}
#endif  /* IUP_ASSERT */

static int iChildTreeFind(Ihandle* parent, Ihandle* child)
{
  Ihandle *c;

  /* Finds the reference child entry inside the parent's child list */
  for (c = parent->firstchild; c; c = c->brother)
  {
    if (c == child) /* Found the right child */
      return 1;
  }

  return 0;
}

static void iChildTreeInsert(Ihandle* parent, Ihandle* ref_child, Ihandle* child)
{
  Ihandle *c, 
          *c_prev = NULL;

  if (parent->firstchild == NULL)
  {
    parent->firstchild = child;
    child->parent = parent;
    return;
  }

  if (!ref_child)
    ref_child = parent->firstchild;

  /* Finds the reference child entry inside the parent's child list */
  for (c = parent->firstchild; c; c = c->brother)
  {
    if (c == ref_child) /* Found the right child */
    {
      child->parent = parent;
      child->brother = ref_child;

      if (c_prev == NULL)
        parent->firstchild = child;
      else
        c_prev->brother = child;

      return;
    }

    c_prev = c;
  }
}

static int iChildTreeCount(Ihandle* ih)
{
  int num = 0;

  ih = ih->firstchild;
  while(ih)
  {
    num++;
    ih = ih->brother;
  }

  return num;
}

Ihandle* IupInsert(Ihandle* parent, Ihandle* ref_child, Ihandle* child)
{
  /* ref_child can be NULL */

  iupASSERT(iupObjectCheck(parent));
  if (!iupObjectCheck(parent))
    return NULL;

  iupASSERT(iupObjectCheck(child));
  if (!iupObjectCheck(child))
    return NULL;

#ifdef IUP_ASSERT
  if (iChildTreeCheckInside(parent, child))
  {
    iupError("Duplicate Child Found!\n(type(%s) - name(%s))", child->iclass->name, IupGetName(child));
    return NULL;
  }
#endif


  if (parent->iclass->childtype == IUP_CHILDNONE)
    return NULL;
  if (parent->iclass->childtype > IUP_CHILDMANY && 
      iChildTreeCount(parent) == parent->iclass->childtype-IUP_CHILDMANY)
    return NULL;


  /* if already at the parent box, allow to move even if mapped */
  if (parent->iclass->nativetype == IUP_TYPEVOID &&
      iChildTreeFind(parent, child))
  {
    iChildTreeDetach(parent, child);
    iChildTreeInsert(parent, ref_child, child);
  }
  else
  {
    /* Not valid if it is mapped */
    if (child->handle)
      return NULL;

    iChildTreeInsert(parent, ref_child, child);
    iupClassObjectChildAdded(parent, child);
  }

  return parent;
}

void iupChildTreeAppend(Ihandle* parent, Ihandle* child)
{
  child->parent = parent;

  if (parent->firstchild == NULL)
    parent->firstchild = child;
  else
  {
    Ihandle* c = parent->firstchild;
    while (c->brother)
      c = c->brother;
    c->brother = child;
  }
}

Ihandle* IupAppend(Ihandle* parent, Ihandle* child)
{
  iupASSERT(iupObjectCheck(parent));
  if (!iupObjectCheck(parent))
    return NULL;

  iupASSERT(iupObjectCheck(child));
  if (!iupObjectCheck(child))
    return NULL;

#ifdef IUP_ASSERT
  if (iChildTreeCheckInside(parent, child))
  {
    iupError("Duplicate Child Found!\n(type(%s) - name(%s))", child->iclass->name, IupGetName(child));
    return NULL;
  }
#endif

  if (parent->iclass->childtype == IUP_CHILDNONE)
    return NULL;
  if (parent->iclass->childtype > IUP_CHILDMANY && 
      iChildTreeCount(parent) == parent->iclass->childtype-IUP_CHILDMANY)
    return NULL;


  /* if already at the parent box, allow to move even if mapped */
  if (parent->iclass->nativetype == IUP_TYPEVOID &&
      iChildTreeFind(parent, child))
  {
    iChildTreeDetach(parent, child);
    iupChildTreeAppend(parent, child);
  }
  else
  {
    /* Not valid if it is mapped */
    if (child->handle)
      return NULL;

    iupChildTreeAppend(parent, child);
    iupClassObjectChildAdded(parent, child);
  }

  return parent;
}

static void iChildTreeReparent(Ihandle* child, Ihandle* new_parent)
{
  Ihandle *c;

  /* Forward the reparent to all native children */

  for (c = child->firstchild; c; c = c->brother)
  {
    if (c->iclass->nativetype != IUP_TYPEVOID)
      iupdrvReparent(c);
    else
      iChildTreeReparent(c, new_parent);
  }
}

int IupReparent(Ihandle* child, Ihandle* parent, Ihandle* ref_child)
{
  Ihandle* old_parent;
  int pos;

  iupASSERT(iupObjectCheck(parent));
  if (!iupObjectCheck(parent))
    return IUP_ERROR;

  iupASSERT(iupObjectCheck(child));
  if (!iupObjectCheck(child))
    return IUP_ERROR;

  if (ref_child)
  {
    /* can be NULL, but if not NULL must be a valid object */
    iupASSERT(iupObjectCheck(ref_child));
    if (!iupObjectCheck(ref_child))
      return IUP_ERROR;
  }

  /* can not be at the same place */
  if (parent == child->parent && (ref_child == child || (ref_child == NULL && child->brother == NULL)))
    return IUP_ERROR;

  if (parent->iclass->childtype == IUP_CHILDNONE)
    return IUP_ERROR;
  if (parent->iclass->childtype > IUP_CHILDMANY && 
      iChildTreeCount(parent) == parent->iclass->childtype-IUP_CHILDMANY)
    return IUP_ERROR;


  /* both must be already mapped or both unmapped */
  if ((!parent->handle &&  child->handle) ||
      ( parent->handle && !child->handle))
    return IUP_ERROR;


  /* detach from old parent */
  old_parent = child->parent;

  pos = IupGetChildPos(old_parent, child);

  iChildTreeDetach(old_parent, child);
  iupClassObjectChildRemoved(old_parent, child, pos);

 
  /* attach to new parent */
  if (ref_child)
    iChildTreeInsert(parent, ref_child, child);
  else
    iupChildTreeAppend(parent, child);
  iupClassObjectChildAdded(parent, child);


  /* no need to remap, just notify the native system */
  if (child->handle && parent->handle)
  {
    if (child->iclass->nativetype != IUP_TYPEVOID)
      iupdrvReparent(child);
    else
      iChildTreeReparent(child, parent);
  }

  return IUP_NOERROR;
}

Ihandle* IupGetChild(Ihandle* ih, int pos)
{
  int p;
  Ihandle* child;

  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return NULL;

  for (p = 0, child = ih->firstchild; child; child = child->brother, p++)
  {
    if (p == pos)
      return child;
  }

  return NULL;
}

int IupGetChildPos(Ihandle* ih, Ihandle* child)
{
  int pos;
  Ihandle* c;

  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return -1;

  for (pos = 0, c = ih->firstchild; c; c = c->brother, pos++)
  {
    if (c == child)
      return pos;
  }
  return -1;
}

Ihandle* iupChildTreeGetPrevBrother(Ihandle* ih)
{
  Ihandle *c, *prev = NULL;

  for (c = ih->parent->firstchild; c; c = c->brother)
  {
    if (c == ih)
      return prev;

    prev = c;
  }

  return NULL;
}

int IupGetChildCount(Ihandle* ih)
{
  int count = 0;
  Ihandle* child;

  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return -1;

  for (child = ih->firstchild; child; child = child->brother)
    count++;

  return count;
}

Ihandle* IupGetNextChild(Ihandle* ih, Ihandle* child)
{
  if (!child)
  {
    iupASSERT(iupObjectCheck(ih));
    if (!iupObjectCheck(ih))
      return NULL;

    return ih->firstchild;
  }
  else
  {
    iupASSERT(iupObjectCheck(child));
    if (!iupObjectCheck(child))
      return NULL;

    return child->brother;
  }
}

Ihandle* IupGetBrother(Ihandle* ih)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return NULL;

  return ih->brother;
}

Ihandle* IupGetParent(Ihandle *ih)
{
  iupASSERT(iupObjectCheck(ih));
  if (!iupObjectCheck(ih))
    return NULL;
    
  return ih->parent;
}

int iupChildTreeIsChild(Ihandle* ih, Ihandle* child)
{
  Ihandle* parent;

  if (ih == child)
    return 1;

  parent = child->parent;
  while (parent)
  {
    if (parent == ih)
      return 1;
    parent = parent->parent;
  }

  return 0;
}

Ihandle* iupChildTreeGetNativeParent(Ihandle* ih)
{
  Ihandle* parent = ih->parent;
  while (parent && parent->iclass->nativetype == IUP_TYPEVOID)
    parent = parent->parent;
  return parent;
}

InativeHandle* iupChildTreeGetNativeParentHandle(Ihandle* ih)
{
  Ihandle* native_parent = iupChildTreeGetNativeParent(ih);
  return (InativeHandle*)iupClassObjectGetInnerNativeContainerHandle(native_parent, ih);
}
