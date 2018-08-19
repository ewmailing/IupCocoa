/** \file
 * \brief Control Hierarchy Tree management.
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUP_CHILDTREE_H 
#define __IUP_CHILDTREE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "iup_export.h"

/** \defgroup childtree Child Tree Utilities
 * \par
 * Some native containers have an internal native child that 
 * will be the actual container for the children. This native container is 
 * returned by \ref iupClassObjectGetInnerNativeContainerHandle and
 * it is used in iupChildTreeGetNativeParentHandle.
 * \par
 * Some native elements need an extra parent, the ih->handle points to the main element itself, 
 * NOT to the extra parent. This extra parent is stored as "_IUP_EXTRAPARENT". 
 * In this case the native parent of ih->handle is the extra parent, 
 * and the extra parent is added as child to the element actual native parent.
 * \par
 * See \ref iup_childtree.h
 * \ingroup object */

/** Returns the native parent. It simply excludes containers that are from IUP_TYPEVOID classes.
 * \ingroup childtree */
IUP_EXPORTI Ihandle* iupChildTreeGetNativeParent(Ihandle* ih);

/** Returns the native parent handle. Uses \ref iupChildTreeGetNativeParent and \ref iupClassObjectGetInnerNativeContainerHandle.
 * \ingroup childtree */
InativeHandle* iupChildTreeGetNativeParentHandle(Ihandle* ih);

/** Adds the child directly to the parent tree.
 * \ingroup childtree */
IUP_EXPORTI void iupChildTreeAppend(Ihandle* parent, Ihandle* child);

/** Checks if the child belongs to the parent tree.
 * \ingroup childtree */
int iupChildTreeIsChild(Ihandle* ih, Ihandle* child);

/** Returns the previous brother if any.
 * \ingroup childtree */
Ihandle* iupChildTreeGetPrevBrother(Ihandle* ih);


/* Other functions declared in <iup.h> and implemented here. 
IupGetDialog
IupDetach
IupAppend
IupGetChild
IupGetNextChild
IupGetBrother
IupGetParent
*/


#ifdef __cplusplus
}
#endif

#endif
