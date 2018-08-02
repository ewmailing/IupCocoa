/** \file
 * \brief Attributes Environment Management
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUP_ATTRIB_H 
#define __IUP_ATTRIB_H

#ifdef __cplusplus
extern "C" {
#endif

#include "iup_export.h"

/** \defgroup attrib Attribute Environment 
 * \par
 * When attributes are not stored at the control  
 * they are stored in a hash table (see \ref table).
 * \par
 * As a general rule use:
 * - IupGetAttribute, IupSetAttribute, ... : when care about control implementation, hash table, inheritance and default value
 * - iupAttribGetStr,Int,Float: when care about inheritance, hash table and default value
 * - iupAttribGet,... : ONLY access the hash table
 * These different functions have very different performances and results. So use them wiselly.
 * \par
 * See \ref iup_attrib.h 
 * \ingroup cpi */


/** Returns true if the attribute name if in the internal format "_IUP...".
 * \ingroup attrib */
#define iupATTRIB_ISINTERNAL(_name) ((_name[0] == '_' && _name[1] == 'I' && _name[2] == 'U' && _name[3] == 'P')? 1: 0)

/** Returns true if the attribute name is a known pointer. \n
 * \ingroup attrib */
IUP_EXPORTI int iupAttribIsNotString(Ihandle* ih, const char *name);

/** Returns true if the attribute name is a known Ihandle*. \n
* \ingroup attrib */
IUP_EXPORTI int iupAttribIsIhandle(Ihandle* ih, const char *name);



/** Sets the attribute only in the hash table as a pointer. \n
 * Only generic pointers and constant strings can be set as value. \n
 * It ignores children.
 * \ingroup attrib */
IUP_EXPORTI void iupAttribSet(Ihandle* ih, const char* name, const char* value);

/** Sets the attribute only in the hash table as a string. \n 
 * The string is internally duplicated. \n
 * It ignores children.
 * \ingroup attrib */
IUP_EXPORTI void iupAttribSetStr(Ihandle* ih, const char* name, const char* value);

/** Sets the attribute only in the hash table as a string. \n 
 * Use same format as sprintf. \n
 * It ignores children. \n
 * This is not supposed to be used for very large strings,
 * just for combinations of numeric data or constant strings.
 * \ingroup attrib */
IUP_EXPORTI void iupAttribSetStrf(Ihandle *ih, const char* name, const char* format, ...);

/** Sets an integer attribute only in the hash table. \n
 * It will be stored as a string. \n
 * It ignores children.
 * \ingroup attrib */
IUP_EXPORTI void iupAttribSetInt(Ihandle *ih, const char* name, int num);

/** Same as \ref iupAttribSet with an id.
 * \ingroup attrib */
IUP_EXPORTI void iupAttribSetId(Ihandle *ih, const char* name, int id, const char* value);

/** Same as \ref iupAttribSetStr with an id.
 * \ingroup attrib */
IUP_EXPORTI void iupAttribSetStrId(Ihandle *ih, const char* name, int id, const char* value);

/** Same as \ref iupAttribSet with lin and col.
 * \ingroup attrib */
IUP_EXPORTI void iupAttribSetId2(Ihandle *ih, const char* name, int lin, int col, const char* value);

/** Same as \ref iupAttribSetStr with lin and col.
 * \ingroup attrib */
IUP_EXPORTI void iupAttribSetStrId2(Ihandle *ih, const char* name, int lin, int col, const char* value);

/** Same as \ref iupAttribSetInt with an id.
 * \ingroup attrib */
IUP_EXPORTI void iupAttribSetIntId(Ihandle *ih, const char* name, int id, int num);

/** Same as \ref iupAttribSetInt with lin and col.
 * \ingroup attrib */
IUP_EXPORTI void iupAttribSetIntId2(Ihandle *ih, const char* name, int lin, int col, int num);

/** Sets an floating point attribute only in the hash table. \n
 * It will be stored as a string. \n
 * It ignores children.
 * \ingroup attrib */
IUP_EXPORTI void iupAttribSetFloat(Ihandle *ih, const char* name, float num);

/** Same as \ref iupAttribSetFloat with an id.
 * \ingroup attrib */
IUP_EXPORTI void iupAttribSetFloatId(Ihandle *ih, const char* name, int id, float num);

/** Same as \ref iupAttribSetFloat with lin and col.
 * \ingroup attrib */
IUP_EXPORTI void iupAttribSetFloatId2(Ihandle *ih, const char* name, int lin, int col, float num);


/** Sets an floating point attribute only in the hash table. \n
* It will be stored as a string. \n
* It ignores children.
* \ingroup attrib */
IUP_EXPORTI void iupAttribSetDouble(Ihandle *ih, const char* name, double num);

/** Same as \ref iupAttribSetDouble with an id.
* \ingroup attrib */
IUP_EXPORTI void iupAttribSetDoubleId(Ihandle *ih, const char* name, int id, double num);

/** Same as \ref iupAttribSetDouble with lin and col.
* \ingroup attrib */
IUP_EXPORTI void iupAttribSetDoubleId2(Ihandle *ih, const char* name, int lin, int col, double num);


/** Returns the attribute from the hash table only.  \n
 * NO inheritance, NO control implementation, NO defalt value here. \n
 * \ingroup attrib */
IUP_EXPORTI char* iupAttribGet(Ihandle* ih, const char* name);

/** Returns the attribute from the hash table as a string, 
 * but if not defined then checks in its parent tree if allowed by the control implementation, 
 * if still not defined then returns the registered default value if any.
 * NO control implementation, only checks inheritance and default value from it.
 * \ingroup attrib */
IUP_EXPORTI char* iupAttribGetStr(Ihandle* ih, const char* name);   

/** Same as \ref iupAttribGetStr but returns an integer number.
 * Checks also for boolean values.
 * \ingroup attrib */
IUP_EXPORTI int iupAttribGetInt(Ihandle* ih, const char* name);

/** Same as \ref iupAttribGetInt but returns two integer numbers. \n
 * Returns the number of valid integers: 0, 1 or 2.
 * \ingroup attrib */
IUP_EXPORTI int iupAttribGetIntInt(Ihandle *ih, const char* name, int *i1, int *i2, char sep);

/** Same as \ref iupAttribGetStr but checks for boolean values.
 * Use \ref iupStrBoolean.
 * \ingroup attrib */
IUP_EXPORTI int iupAttribGetBoolean(Ihandle* ih, const char* name);

/** Same as \ref iupAttribGetStr but returns an floating point number.
 * \ingroup attrib */
IUP_EXPORTI float iupAttribGetFloat(Ihandle* ih, const char* name);

/** Same as \ref iupAttribGetStr but returns an floating point number.
* \ingroup attrib */
IUP_EXPORTI double iupAttribGetDouble(Ihandle* ih, const char* name);

/** Same as \ref iupAttribGet but with an id.
 * \ingroup attrib */
IUP_EXPORTI char* iupAttribGetId(Ihandle* ih, const char* name, int id);

/** Same as \ref iupAttribGetInt but with an id.
 * \ingroup attrib */
IUP_EXPORTI int iupAttribGetIntId(Ihandle* ih, const char* name, int id);

/** Same as \ref iupAttribGetBoolean but with an id.
 * \ingroup attrib */
IUP_EXPORTI int iupAttribGetBooleanId(Ihandle* ih, const char* name, int id);

/** Same as \ref iupAttribGetFloat but with an id.
 * \ingroup attrib */
IUP_EXPORTI float iupAttribGetFloatId(Ihandle* ih, const char* name, int id);

/** Same as \ref iupAttribGet but with an lin and col.
 * \ingroup attrib */
IUP_EXPORTI char* iupAttribGetId2(Ihandle* ih, const char* name, int lin, int col);

/** Same as \ref iupAttribGetInt but with lin and col.
 * \ingroup attrib */
IUP_EXPORTI int iupAttribGetIntId2(Ihandle* ih, const char* name, int lin, int col);

/** Same as \ref iupAttribGetBoolean but with lin and col.
 * \ingroup attrib */
IUP_EXPORTI int iupAttribGetBooleanId2(Ihandle* ih, const char* name, int lin, int col);

/** Same as \ref iupAttribGetFloat but with lin and col.
 * \ingroup attrib */
IUP_EXPORTI float iupAttribGetFloatId2(Ihandle* ih, const char* name, int lin, int col);



/** Returns the attribute from the hash table only, 
 * but if not defined then checks in its parent tree. \n
 * NO control implementation, NO defalt value here. \n
 * Used for EXPAND and internal attributes inside a dialog.
 * \ingroup attrib */
IUP_EXPORTI char* iupAttribGetInherit(Ihandle* ih, const char* name);

/** Returns the attribute from the hash table of a native parent.
 * Don't check for default values. Don't check at the element.
 * Used for BGCOLOR and BACKGROUND attributes.
 * \ingroup attrib */
char* iupAttribGetInheritNativeParent(Ihandle* ih, const char* name);

/** Returns the attribute from the hash table as a string, 
 * but if not defined then checks in the control implementation, 
 * if still not defined then returns the registered default value if any. \n
 * NO inheritance here. Used only in the IupLayoutDialog.
 * \ingroup attrib */
char* iupAttribGetLocal(Ihandle* ih, const char* name);



/** Set an internal name to a handle.
 * \ingroup attrib */
IUP_EXPORTI void iupAttribSetHandleName(Ihandle *ih);

/** Returns the internal name if set.
 * \ingroup attrib */
IUP_EXPORTI char* iupAttribGetHandleName(Ihandle *ih);



/** Sets the attribute only at the element. \n
 * If set method is not defined will not be set,
 * neither will be stored in the hash table. \n
 * Only generic pointers and constant strings can be set as value. \n
 * It ignores children.
 * \ingroup attrib */
IUP_EXPORTI void iupAttribSetClassObject(Ihandle* ih, const char* name, const char* value);
IUP_EXPORTI void iupAttribSetClassObjectId(Ihandle* ih, const char* name, int id, const char* value);
IUP_EXPORTI void iupAttribSetClassObjectId2(Ihandle* ih, const char* name, int lin, int col, const char* value);
IUP_EXPORTI char* iupAttribGetClassObject(Ihandle* ih, const char* name);
IUP_EXPORTI char* iupAttribGetClassObjectId(Ihandle* ih, const char* name, int id);
IUP_EXPORTI char* iupAttribGetClassObjectId2(Ihandle* ih, const char* name, int lin, int col);



/* For all attributes in the evironment, call the class SetAttribute only.
 * Called only after the element is mapped, but before the children are mapped. */
void iupAttribUpdate(Ihandle* ih); 

/* For all registered inherited attributes, checks the parent tree and 
 * call the class SetAttribute if the attribute is defined.
 * Called only after the element is mapped, but before the children are mapped. */
void iupAttribUpdateFromParent(Ihandle* ih);

/* For all attributes in the evironment, call the class SetAttribute only for the children.
 * Called only after the element is mapped, and after the children are mapped. */
void iupAttribUpdateChildren(Ihandle* ih);


/* Other functions declared in <iup.h> and implemented here. 
IupGetAllAttributes
IupGetAttributes
IupSetAttributes
IupSetAttribute
IupStoreAttribute
IupGetAttribute
IupGetFloat
IupGetInt
IupGetInt2
IupSetfAttribute
IupSetAttributeHandle
IupGetAttributeHandle
*/


#ifdef __cplusplus
}
#endif

#endif
