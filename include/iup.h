/** \file
 * \brief User API
 * IUP - A Portable User Interface Toolkit
 * Tecgraf: Computer Graphics Technology Group, PUC-Rio, Brazil
 * http://www.tecgraf.puc-rio.br/iup  mailto:iup@tecgraf.puc-rio.br
 *
 * See Copyright Notice at the end of this file
 */
 
#ifndef __IUP_H 
#define __IUP_H

#include "iupkey.h"
#include "iupdef.h"

#ifdef __cplusplus
extern "C" {
#endif


#define IUP_NAME "IUP - Portable User Interface"
#define IUP_DESCRIPTION	"Multi-platform Toolkit for Building Graphical User Interfaces"
#define IUP_COPYRIGHT "Copyright (C) 1994-2018 Tecgraf/PUC-Rio"
#define IUP_VERSION "3.24"         /* bug fixes are reported only by IupVersion functions */
#define IUP_VERSION_NUMBER 324000
#define IUP_VERSION_DATE "2018/01/22"  /* does not include bug fix releases */

typedef struct Ihandle_ Ihandle;
typedef int (*Icallback)(Ihandle*);

#ifndef DOXYGEN_SHOULD_IGNORE_THIS
/** @cond DOXYGEN_SHOULD_IGNORE_THIS */
#ifndef IUP_EXPORT
	#ifdef IUP_BUILD_LIBRARY
		#ifdef __EMSCRIPTEN__
				#include <emscripten.h>
                #define IUP_EXPORT EMSCRIPTEN_KEEPALIVE
        #elif WIN32
                #define IUP_EXPORT __declspec(dllexport)
        #elif defined(__GNUC__) && __GNUC__ >= 4
                #define IUP_EXPORT __attribute__ ((visibility("default")))
        #else
                #define IUP_EXPORT
        #endif
	#else
        #define IUP_EXPORT
	#endif /* IUP_BUILD_LIBRARY */
#endif /* IUP_EXPORT */
/** @endcond DOXYGEN_SHOULD_IGNORE_THIS */
#endif /* DOXYGEN_SHOULD_IGNORE_THIS */

/************************************************************************/
/*                        Main API                                      */
/************************************************************************/

IUP_EXPORT int       IupOpen          (int *argc, char ***argv);
IUP_EXPORT void      IupClose         (void);
IUP_EXPORT void      IupImageLibOpen  (void);

IUP_EXPORT int       IupMainLoop      (void);
IUP_EXPORT int       IupLoopStep      (void);
IUP_EXPORT int       IupLoopStepWait  (void);
IUP_EXPORT int       IupMainLoopLevel (void);
IUP_EXPORT void      IupFlush         (void);
IUP_EXPORT void      IupExitLoop      (void);

IUP_EXPORT int       IupRecordInput(const char* filename, int mode);
IUP_EXPORT int       IupPlayInput(const char* filename);

IUP_EXPORT void      IupUpdate        (Ihandle* ih);
IUP_EXPORT void      IupUpdateChildren(Ihandle* ih);
IUP_EXPORT void      IupRedraw        (Ihandle* ih, int children);
IUP_EXPORT void      IupRefresh       (Ihandle* ih);
IUP_EXPORT void      IupRefreshChildren(Ihandle* ih);

IUP_EXPORT int       IupExecute(const char *filename, const char* parameters);
IUP_EXPORT int       IupExecuteWait(const char *filename, const char* parameters);
IUP_EXPORT int       IupHelp(const char* url);
IUP_EXPORT void      IupLog(const char* type, const char* format, ...);

IUP_EXPORT char*     IupLoad          (const char *filename);
IUP_EXPORT char*     IupLoadBuffer    (const char *buffer);

IUP_EXPORT char*     IupVersion       (void);
IUP_EXPORT char*     IupVersionDate   (void);
IUP_EXPORT int       IupVersionNumber (void);

IUP_EXPORT void      IupSetLanguage   (const char *lng);
IUP_EXPORT char*     IupGetLanguage   (void);
IUP_EXPORT void      IupSetLanguageString(const char* name, const char* str);
IUP_EXPORT void      IupStoreLanguageString(const char* name, const char* str);
IUP_EXPORT char*     IupGetLanguageString(const char* name);
IUP_EXPORT void      IupSetLanguagePack(Ihandle* ih);

IUP_EXPORT void      IupDestroy      (Ihandle* ih);
IUP_EXPORT void      IupDetach       (Ihandle* child);
IUP_EXPORT Ihandle*  IupAppend       (Ihandle* ih, Ihandle* child);
IUP_EXPORT Ihandle*  IupInsert       (Ihandle* ih, Ihandle* ref_child, Ihandle* child);
IUP_EXPORT Ihandle*  IupGetChild     (Ihandle* ih, int pos);
IUP_EXPORT int       IupGetChildPos  (Ihandle* ih, Ihandle* child);
IUP_EXPORT int       IupGetChildCount(Ihandle* ih);
IUP_EXPORT Ihandle*  IupGetNextChild (Ihandle* ih, Ihandle* child);
IUP_EXPORT Ihandle*  IupGetBrother   (Ihandle* ih);
IUP_EXPORT Ihandle*  IupGetParent    (Ihandle* ih);
IUP_EXPORT Ihandle*  IupGetDialog    (Ihandle* ih);
IUP_EXPORT Ihandle*  IupGetDialogChild(Ihandle* ih, const char* name);
IUP_EXPORT int       IupReparent     (Ihandle* ih, Ihandle* new_parent, Ihandle* ref_child);

IUP_EXPORT int       IupPopup         (Ihandle* ih, int x, int y);
IUP_EXPORT int       IupShow          (Ihandle* ih);
IUP_EXPORT int       IupShowXY        (Ihandle* ih, int x, int y);
IUP_EXPORT int       IupHide          (Ihandle* ih);
IUP_EXPORT int       IupMap           (Ihandle* ih);
IUP_EXPORT void      IupUnmap         (Ihandle* ih);

IUP_EXPORT void      IupResetAttribute(Ihandle* ih, const char* name);
IUP_EXPORT int       IupGetAllAttributes(Ihandle* ih, char** names, int n);
IUP_EXPORT Ihandle*  IupSetAtt(const char* handle_name, Ihandle* ih, const char* name, ...);
IUP_EXPORT Ihandle*  IupSetAttributes (Ihandle* ih, const char *str);
IUP_EXPORT char*     IupGetAttributes (Ihandle* ih);

IUP_EXPORT void      IupSetAttribute   (Ihandle* ih, const char* name, const char* value);
IUP_EXPORT void      IupSetStrAttribute(Ihandle* ih, const char* name, const char* value);
IUP_EXPORT void      IupSetStrf        (Ihandle* ih, const char* name, const char* format, ...);
IUP_EXPORT void      IupSetInt         (Ihandle* ih, const char* name, int value);
IUP_EXPORT void      IupSetFloat       (Ihandle* ih, const char* name, float value);
IUP_EXPORT void      IupSetDouble      (Ihandle* ih, const char* name, double value);
IUP_EXPORT void      IupSetRGB         (Ihandle* ih, const char* name, unsigned char r, unsigned char g, unsigned char b);

IUP_EXPORT char*     IupGetAttribute(Ihandle* ih, const char* name);
IUP_EXPORT int       IupGetInt      (Ihandle* ih, const char* name);
IUP_EXPORT int       IupGetInt2     (Ihandle* ih, const char* name);
IUP_EXPORT int       IupGetIntInt   (Ihandle* ih, const char* name, int *i1, int *i2);
IUP_EXPORT float     IupGetFloat    (Ihandle* ih, const char* name);
IUP_EXPORT double    IupGetDouble(Ihandle* ih, const char* name);
IUP_EXPORT void      IupGetRGB      (Ihandle* ih, const char* name, unsigned char *r, unsigned char *g, unsigned char *b);

IUP_EXPORT void  IupSetAttributeId(Ihandle* ih, const char* name, int id, const char *value);
IUP_EXPORT void  IupSetStrAttributeId(Ihandle* ih, const char* name, int id, const char *value);
IUP_EXPORT void  IupSetStrfId(Ihandle* ih, const char* name, int id, const char* format, ...);
IUP_EXPORT void  IupSetIntId(Ihandle* ih, const char* name, int id, int value);
IUP_EXPORT void  IupSetFloatId(Ihandle* ih, const char* name, int id, float value);
IUP_EXPORT void  IupSetDoubleId(Ihandle* ih, const char* name, int id, double value);
IUP_EXPORT void  IupSetRGBId(Ihandle* ih, const char* name, int id, unsigned char r, unsigned char g, unsigned char b);

IUP_EXPORT char*  IupGetAttributeId(Ihandle* ih, const char* name, int id);
IUP_EXPORT int    IupGetIntId(Ihandle* ih, const char* name, int id);
IUP_EXPORT float  IupGetFloatId(Ihandle* ih, const char* name, int id);
IUP_EXPORT double IupGetDoubleId(Ihandle* ih, const char* name, int id);
IUP_EXPORT void   IupGetRGBId(Ihandle* ih, const char* name, int id, unsigned char *r, unsigned char *g, unsigned char *b);

IUP_EXPORT void  IupSetAttributeId2(Ihandle* ih, const char* name, int lin, int col, const char* value);
IUP_EXPORT void  IupSetStrAttributeId2(Ihandle* ih, const char* name, int lin, int col, const char* value);
IUP_EXPORT void  IupSetStrfId2(Ihandle* ih, const char* name, int lin, int col, const char* format, ...);
IUP_EXPORT void  IupSetIntId2(Ihandle* ih, const char* name, int lin, int col, int value);
IUP_EXPORT void  IupSetFloatId2(Ihandle* ih, const char* name, int lin, int col, float value);
IUP_EXPORT void  IupSetDoubleId2(Ihandle* ih, const char* name, int lin, int col, double value);
IUP_EXPORT void  IupSetRGBId2(Ihandle* ih, const char* name, int lin, int col, unsigned char r, unsigned char g, unsigned char b);

IUP_EXPORT char*  IupGetAttributeId2(Ihandle* ih, const char* name, int lin, int col);
IUP_EXPORT int    IupGetIntId2(Ihandle* ih, const char* name, int lin, int col);
IUP_EXPORT float  IupGetFloatId2(Ihandle* ih, const char* name, int lin, int col);
IUP_EXPORT double IupGetDoubleId2(Ihandle* ih, const char* name, int lin, int col);
IUP_EXPORT void   IupGetRGBId2(Ihandle* ih, const char* name, int lin, int col, unsigned char *r, unsigned char *g, unsigned char *b);

IUP_EXPORT void      IupSetGlobal  (const char* name, const char* value);
IUP_EXPORT void      IupSetStrGlobal(const char* name, const char* value);
IUP_EXPORT char*     IupGetGlobal  (const char* name);

IUP_EXPORT Ihandle*  IupSetFocus     (Ihandle* ih);
IUP_EXPORT Ihandle*  IupGetFocus     (void);
IUP_EXPORT Ihandle*  IupPreviousField(Ihandle* ih);
IUP_EXPORT Ihandle*  IupNextField    (Ihandle* ih);

IUP_EXPORT Icallback IupGetCallback (Ihandle* ih, const char *name);
IUP_EXPORT Icallback IupSetCallback (Ihandle* ih, const char *name, Icallback func);
IUP_EXPORT Ihandle*  IupSetCallbacks(Ihandle* ih, const char *name, Icallback func, ...);

IUP_EXPORT Icallback IupGetFunction(const char *name);
IUP_EXPORT Icallback IupSetFunction(const char *name, Icallback func);

IUP_EXPORT Ihandle*  IupGetHandle    (const char *name);
IUP_EXPORT Ihandle*  IupSetHandle    (const char *name, Ihandle* ih);
IUP_EXPORT int       IupGetAllNames  (char** names, int n);
IUP_EXPORT int       IupGetAllDialogs(char** names, int n);
IUP_EXPORT char*     IupGetName      (Ihandle* ih);

IUP_EXPORT void      IupSetAttributeHandle(Ihandle* ih, const char* name, Ihandle* ih_named);
IUP_EXPORT Ihandle*  IupGetAttributeHandle(Ihandle* ih, const char* name);
IUP_EXPORT void      IupSetAttributeHandleId(Ihandle* ih, const char* name, int id, Ihandle* ih_named);
IUP_EXPORT Ihandle*  IupGetAttributeHandleId(Ihandle* ih, const char* name, int id);
IUP_EXPORT void      IupSetAttributeHandleId2(Ihandle* ih, const char* name, int lin, int col, Ihandle* ih_named);
IUP_EXPORT Ihandle*  IupGetAttributeHandleId2(Ihandle* ih, const char* name, int lin, int col);

IUP_EXPORT char*     IupGetClassName(Ihandle* ih);
IUP_EXPORT char*     IupGetClassType(Ihandle* ih);
IUP_EXPORT int       IupGetAllClasses(char** names, int n);
IUP_EXPORT int       IupGetClassAttributes(const char* classname, char** names, int n);
IUP_EXPORT int       IupGetClassCallbacks(const char* classname, char** names, int n);
IUP_EXPORT void      IupSaveClassAttributes(Ihandle* ih);
IUP_EXPORT void      IupCopyClassAttributes(Ihandle* src_ih, Ihandle* dst_ih);
IUP_EXPORT void      IupSetClassDefaultAttribute(const char* classname, const char *name, const char* value);
IUP_EXPORT int       IupClassMatch(Ihandle* ih, const char* classname);

IUP_EXPORT Ihandle*  IupCreate (const char *classname);
IUP_EXPORT Ihandle*  IupCreatev(const char *classname, void* *params);
IUP_EXPORT Ihandle*  IupCreatep(const char *classname, void* first, ...);

/************************************************************************/
/*                        Elements                                      */
/************************************************************************/

IUP_EXPORT Ihandle*  IupFill       (void);
IUP_EXPORT Ihandle*  IupRadio      (Ihandle* child);
IUP_EXPORT Ihandle*  IupVbox       (Ihandle* child, ...);
IUP_EXPORT Ihandle*  IupVboxv      (Ihandle* *children);
IUP_EXPORT Ihandle*  IupZbox       (Ihandle* child, ...);
IUP_EXPORT Ihandle*  IupZboxv      (Ihandle* *children);
IUP_EXPORT Ihandle*  IupHbox       (Ihandle* child, ...);
IUP_EXPORT Ihandle*  IupHboxv      (Ihandle* *children);

IUP_EXPORT Ihandle*  IupNormalizer (Ihandle* ih_first, ...);
IUP_EXPORT Ihandle*  IupNormalizerv(Ihandle* *ih_list);

IUP_EXPORT Ihandle*  IupCbox       (Ihandle* child, ...);
IUP_EXPORT Ihandle*  IupCboxv      (Ihandle* *children);
IUP_EXPORT Ihandle*  IupSbox       (Ihandle* child);
IUP_EXPORT Ihandle*  IupSplit      (Ihandle* child1, Ihandle* child2);
IUP_EXPORT Ihandle*  IupScrollBox  (Ihandle* child);
IUP_EXPORT Ihandle*  IupFlatScrollBox(Ihandle* child);
IUP_EXPORT Ihandle*  IupGridBox    (Ihandle* child, ...);
IUP_EXPORT Ihandle*  IupGridBoxv   (Ihandle* *children);
IUP_EXPORT Ihandle*  IupExpander   (Ihandle* child);
IUP_EXPORT Ihandle*  IupDetachBox  (Ihandle* child);
IUP_EXPORT Ihandle*  IupBackgroundBox(Ihandle* child);

IUP_EXPORT Ihandle*  IupFrame      (Ihandle* child);
IUP_EXPORT Ihandle*  IupFlatFrame  (Ihandle* child);

IUP_EXPORT Ihandle*  IupImage      (int width, int height, const unsigned char *pixmap);
IUP_EXPORT Ihandle*  IupImageRGB   (int width, int height, const unsigned char *pixmap);
IUP_EXPORT Ihandle*  IupImageRGBA  (int width, int height, const unsigned char *pixmap);

IUP_EXPORT Ihandle*  IupItem       (const char* title, const char* action);
IUP_EXPORT Ihandle*  IupSubmenu    (const char* title, Ihandle* child);
IUP_EXPORT Ihandle*  IupSeparator  (void);
IUP_EXPORT Ihandle*  IupMenu       (Ihandle* child, ...);
IUP_EXPORT Ihandle*  IupMenuv      (Ihandle* *children);

IUP_EXPORT Ihandle*  IupButton     (const char* title, const char* action);
IUP_EXPORT Ihandle*  IupFlatButton (const char* title);
IUP_EXPORT Ihandle*  IupFlatToggle (const char* title);
IUP_EXPORT Ihandle*  IupDropButton (Ihandle* dropchild);
IUP_EXPORT Ihandle*  IupFlatLabel  (const char* title);
IUP_EXPORT Ihandle*  IupFlatSeparator(void);
IUP_EXPORT Ihandle*  IupCanvas     (const char* action);
IUP_EXPORT Ihandle*  IupDialog     (Ihandle* child);
IUP_EXPORT Ihandle*  IupUser       (void);
IUP_EXPORT Ihandle*  IupLabel      (const char* title);
IUP_EXPORT Ihandle*  IupList       (const char* action);
IUP_EXPORT Ihandle*  IupText       (const char* action);
IUP_EXPORT Ihandle*  IupMultiLine  (const char* action);
IUP_EXPORT Ihandle*  IupToggle     (const char* title, const char* action);
IUP_EXPORT Ihandle*  IupTimer      (void);
IUP_EXPORT Ihandle*  IupClipboard  (void);
IUP_EXPORT Ihandle*  IupProgressBar(void);
IUP_EXPORT Ihandle*  IupVal        (const char *type);
IUP_EXPORT Ihandle*  IupTabs       (Ihandle* child, ...);
IUP_EXPORT Ihandle*  IupTabsv      (Ihandle* *children);
IUP_EXPORT Ihandle*  IupFlatTabs   (Ihandle* first, ...);
IUP_EXPORT Ihandle*  IupFlatTabsv  (Ihandle* *children);
IUP_EXPORT Ihandle*  IupTree       (void);
IUP_EXPORT Ihandle*  IupLink       (const char* url, const char* title);
IUP_EXPORT Ihandle*  IupAnimatedLabel(Ihandle* animation);
IUP_EXPORT Ihandle*  IupDatePick   (void);
IUP_EXPORT Ihandle*  IupCalendar   (void);
IUP_EXPORT Ihandle*  IupColorbar   (void);
IUP_EXPORT Ihandle*  IupGauge      (void);
IUP_EXPORT Ihandle*  IupDial       (const char* type);
IUP_EXPORT Ihandle*  IupColorBrowser(void);

/* Old controls, use SPIN attribute of IupText */
IUP_EXPORT Ihandle*  IupSpin       (void);
IUP_EXPORT Ihandle*  IupSpinbox    (Ihandle* child);


/************************************************************************/
/*                      Utilities                                       */
/************************************************************************/

/* String compare utility */
IUP_EXPORT int IupStringCompare(const char* str1, const char* str2, int casesensitive, int lexicographic);

/* IupImage utility */
IUP_EXPORT int IupSaveImageAsText(Ihandle* ih, const char* file_name, const char* format, const char* name);

/* IupText and IupScintilla utilities */
IUP_EXPORT void  IupTextConvertLinColToPos(Ihandle* ih, int lin, int col, int *pos);
IUP_EXPORT void  IupTextConvertPosToLinCol(Ihandle* ih, int pos, int *lin, int *col);

/* IupText, IupList, IupTree, IupMatrix and IupScintilla utility */
IUP_EXPORT int   IupConvertXYToPos(Ihandle* ih, int x, int y);

/* OLD names, kept for backward compatibility, will never be removed. */
IUP_EXPORT void IupStoreGlobal(const char* name, const char* value);
IUP_EXPORT void IupStoreAttribute(Ihandle* ih, const char* name, const char* value);
IUP_EXPORT void IupSetfAttribute(Ihandle* ih, const char* name, const char* format, ...);
IUP_EXPORT void IupStoreAttributeId(Ihandle* ih, const char* name, int id, const char *value);
IUP_EXPORT void IupSetfAttributeId(Ihandle* ih, const char* name, int id, const char* f, ...);
IUP_EXPORT void IupStoreAttributeId2(Ihandle* ih, const char* name, int lin, int col, const char* value);
IUP_EXPORT void IupSetfAttributeId2(Ihandle* ih, const char* name, int lin, int col, const char* format, ...);

/* IupTree utilities */
IUP_EXPORT int   IupTreeSetUserId(Ihandle* ih, int id, void* userid);
IUP_EXPORT void* IupTreeGetUserId(Ihandle* ih, int id);
IUP_EXPORT int   IupTreeGetId(Ihandle* ih, void *userid);
IUP_EXPORT void  IupTreeSetAttributeHandle(Ihandle* ih, const char* name, int id, Ihandle* ih_named); /* deprecated, use IupSetAttributeHandleId */


/************************************************************************/
/*                      Pre-defined dialogs                           */
/************************************************************************/

IUP_EXPORT Ihandle* IupFileDlg(void);
IUP_EXPORT Ihandle* IupMessageDlg(void);
IUP_EXPORT Ihandle* IupColorDlg(void);
IUP_EXPORT Ihandle* IupFontDlg(void);
IUP_EXPORT Ihandle* IupProgressDlg(void);

IUP_EXPORT int  IupGetFile(char *arq);
IUP_EXPORT void IupMessage(const char *title, const char *msg);
IUP_EXPORT void IupMessagef(const char *title, const char *format, ...);
IUP_EXPORT void IupMessageError(Ihandle* parent, const char* message);
IUP_EXPORT int IupMessageAlarm(Ihandle* parent, const char* title, const char *message, const char *buttons);
IUP_EXPORT int  IupAlarm(const char *title, const char *msg, const char *b1, const char *b2, const char *b3);
IUP_EXPORT int  IupScanf(const char *format, ...);
IUP_EXPORT int  IupListDialog(int type, const char *title, int size, const char** list,
                   int op, int max_col, int max_lin, int* marks);
IUP_EXPORT int  IupGetText(const char* title, char* text, int maxsize);
IUP_EXPORT int  IupGetColor(int x, int y, unsigned char* r, unsigned char* g, unsigned char* b);

typedef int (*Iparamcb)(Ihandle* dialog, int param_index, void* user_data);
IUP_EXPORT int IupGetParam(const char* title, Iparamcb action, void* user_data, const char* format,...);
IUP_EXPORT int IupGetParamv(const char* title, Iparamcb action, void* user_data, const char* format, int param_count, int param_extra, void** param_data);
IUP_EXPORT Ihandle* IupParam(const char* format);
IUP_EXPORT Ihandle*  IupParamBox(Ihandle* param, ...);
IUP_EXPORT Ihandle*  IupParamBoxv(Ihandle* *param_array);

IUP_EXPORT Ihandle* IupLayoutDialog(Ihandle* dialog);
IUP_EXPORT Ihandle* IupElementPropertiesDialog(Ihandle* elem);


#ifdef __cplusplus
}
#endif

/************************************************************************/
/*                   Common Flags and Return Values                     */
/************************************************************************/
#define IUP_ERROR     1
#define IUP_NOERROR   0
#define IUP_OPENED   -1
#define IUP_INVALID  -1
#define IUP_INVALID_ID -10


/************************************************************************/
/*                   Callback Return Values                             */
/************************************************************************/
#define IUP_IGNORE    -1
#define IUP_DEFAULT   -2
#define IUP_CLOSE     -3
#define IUP_CONTINUE  -4

/************************************************************************/
/*           IupPopup and IupShowXY Parameter Values                    */
/************************************************************************/
#define IUP_CENTER        0xFFFF  /* 65535 */
#define IUP_LEFT          0xFFFE  /* 65534 */
#define IUP_RIGHT         0xFFFD  /* 65533 */
#define IUP_MOUSEPOS      0xFFFC  /* 65532 */
#define IUP_CURRENT       0xFFFB  /* 65531 */
#define IUP_CENTERPARENT  0xFFFA  /* 65530 */
#define IUP_TOP       IUP_LEFT
#define IUP_BOTTOM    IUP_RIGHT

/************************************************************************/
/*               SHOW_CB Callback Values                                */
/************************************************************************/
enum{IUP_SHOW, IUP_RESTORE, IUP_MINIMIZE, IUP_MAXIMIZE, IUP_HIDE};

/************************************************************************/
/*               SCROLL_CB Callback Values                              */
/************************************************************************/
enum{IUP_SBUP,   IUP_SBDN,    IUP_SBPGUP,   IUP_SBPGDN,    IUP_SBPOSV, IUP_SBDRAGV, 
     IUP_SBLEFT, IUP_SBRIGHT, IUP_SBPGLEFT, IUP_SBPGRIGHT, IUP_SBPOSH, IUP_SBDRAGH};

/************************************************************************/
/*               Mouse Button Values and Macros                         */
/************************************************************************/
#define IUP_BUTTON1   '1'
#define IUP_BUTTON2   '2'
#define IUP_BUTTON3   '3'
#define IUP_BUTTON4   '4'
#define IUP_BUTTON5   '5'

#define iup_isshift(_s)    (_s[0]=='S')
#define iup_iscontrol(_s)  (_s[1]=='C')
#define iup_isbutton1(_s)  (_s[2]=='1')
#define iup_isbutton2(_s)  (_s[3]=='2')
#define iup_isbutton3(_s)  (_s[4]=='3')
#define iup_isdouble(_s)   (_s[5]=='D')
#define iup_isalt(_s)      (_s[6]=='A')
#define iup_issys(_s)      (_s[7]=='Y')
#define iup_isbutton4(_s)  (_s[8]=='4')
#define iup_isbutton5(_s)  (_s[9]=='5')

/* Old definitions for backward compatibility */
#define isshift     iup_isshift
#define iscontrol   iup_iscontrol
#define isbutton1   iup_isbutton1
#define isbutton2   iup_isbutton2
#define isbutton3   iup_isbutton3
#define isdouble    iup_isdouble
#define isalt       iup_isalt
#define issys       iup_issys
#define isbutton4   iup_isbutton4
#define isbutton5   iup_isbutton5


/************************************************************************/
/*                      Pre-Defined Masks                               */
/************************************************************************/
#define IUP_MASK_FLOAT       "[+/-]?(/d+/.?/d*|/./d+)"
#define IUP_MASK_UFLOAT            "(/d+/.?/d*|/./d+)"
#define IUP_MASK_EFLOAT      "[+/-]?(/d+/.?/d*|/./d+)([eE][+/-]?/d+)?"
#define IUP_MASK_UEFLOAT           "(/d+/.?/d*|/./d+)([eE][+/-]?/d+)?"
#define IUP_MASK_FLOATCOMMA  "[+/-]?(/d+/,?/d*|/,/d+)"
#define IUP_MASK_UFLOATCOMMA       "(/d+/,?/d*|/,/d+)"
#define IUP_MASK_INT          "[+/-]?/d+"
#define IUP_MASK_UINT               "/d+"

/* Old definitions for backward compatibility */
#define IUPMASK_FLOAT     IUP_MASK_FLOAT
#define IUPMASK_UFLOAT    IUP_MASK_UFLOAT
#define IUPMASK_EFLOAT    IUP_MASK_EFLOAT
#define IUPMASK_INT	      IUP_MASK_INT
#define IUPMASK_UINT      IUP_MASK_UINT


/************************************************************************/
/*                   IupGetParam Callback situations                    */
/************************************************************************/
#define IUP_GETPARAM_BUTTON1 -1
#define IUP_GETPARAM_INIT    -2
#define IUP_GETPARAM_BUTTON2 -3
#define IUP_GETPARAM_BUTTON3 -4
#define IUP_GETPARAM_CLOSE   -5
#define IUP_GETPARAM_MAP     -6
#define IUP_GETPARAM_OK     IUP_GETPARAM_BUTTON1
#define IUP_GETPARAM_CANCEL IUP_GETPARAM_BUTTON2
#define IUP_GETPARAM_HELP   IUP_GETPARAM_BUTTON3

/************************************************************************/
/*                   Used by IupColorbar                                */
/************************************************************************/
#define IUP_PRIMARY -1
#define IUP_SECONDARY -2

/************************************************************************/
/*                   Record Input Modes                                 */
/************************************************************************/
enum {IUP_RECBINARY, IUP_RECTEXT};


/************************************************************************/
/*              Replacement for the WinMain in Windows,                 */
/*        this allows the application to start from "main".             */
/*        Used only for Watcom.                                         */
/************************************************************************/
#if defined (__WATCOMC__)
#ifdef __cplusplus
extern "C" {
int IupMain (int argc, char** argv); /* In C++ we have to declare the prototype */
}
#endif
#define main IupMain /* this is the trick for Watcom and MetroWerks */
#endif

/******************************************************************************
* Copyright (C) 1994-2018 Tecgraf/PUC-Rio.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/

#endif
