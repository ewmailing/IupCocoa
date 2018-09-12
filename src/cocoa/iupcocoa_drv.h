/** \file
 * \brief MAC Driver 
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUPMAC_DRV_H 
#define __IUPMAC_DRV_H

#ifdef __cplusplus
extern "C" {
#endif

#include "iup_export.h"

#include <asl.h>
#if __OBJC__
	#import <Foundation/Foundation.h>
#endif
	
// the point of this is we have a unique memory address for an identifier
extern const void* IHANDLE_ASSOCIATED_OBJ_KEY;


	
// TODO: Move to os_log, but requires Mac 10.12
#define iupAppleLog(...) asl_log(NULL, NULL, ASL_LEVEL_NOTICE, __VA_ARGS__)
#define iupAppleLogDebug(...) asl_log(NULL, NULL, ASL_LEVEL_DEBUG, __VA_ARGS__)
#define iupAppleLogInfo(...) asl_log(NULL, NULL, ASL_LEVEL_INFO, __VA_ARGS__)
#define iupAppleLogNotice(...) asl_log(NULL, NULL, ASL_LEVEL_NOTICE, __VA_ARGS__)
#define iupAppleLogWarning(...) asl_log(NULL, NULL, ASL_LEVEL_WARNING, __VA_ARGS__)
#define iupAppleLogError(...) asl_log(NULL, NULL, ASL_LEVEL_ERR, __VA_ARGS__)
#define iupAppleLogCritical(...) asl_log(NULL, NULL, ASL_LEVEL_CRIT, __VA_ARGS__)

#if __OBJC__
#define iupAppleNSLog(...) asl_log(NULL, NULL, ASL_LEVEL_INFO, "%s", [[NSString stringWithFormat:__VA_ARGS__] UTF8String])
#endif
	
	
IUP_EXPORTI void iupCocoaAddToParent(Ihandle* ih);
IUP_EXPORTI void iupCocoaRemoveFromParent(Ihandle* ih);
	
NSView* iupCocoaCommonBaseLayoutGetParentView(Ihandle* ih);
NSView* iupCocoaCommonBaseLayoutGetChildView(Ihandle* ih);
NSRect iupCocoaCommonBaseLayoutComputeChildFrameRectFromParentRect(Ihandle* ih, NSRect parent_rect);
	
void iupCocoaCommonLoopCallExitCb(void);

	// Cocoa is in Cartesian (a.k.a. math book, aka OpenGL coordinates, aka y increases upwards), but Iup is y increases downwards.
int iupCocoaComputeCartesianScreenHeightFromIup(int iup_height);
int iupCocoaComputeIupScreenHeightFromCartesian(int cartesian_height);


int iupCocoaMenuIsApplicationBar(Ihandle* ih);
void iupCocoaMenuSetApplicationMenu(Ihandle* ih);
// Note: This only gets the user's Ihandle to the application menu. If the user doesn't set it, the default application will not be returned in its place. NULL will be returned instead.
Ihandle* iupCocoaMenuGetApplicationMenu(void);
// My current understanding is that IUP will not clean up our application menu Ihandles. So we need to do it ourselves.
void iupCocoaMenuCleanupApplicationMenu(void);


// This is for NSResponder context menus. (I expect new modules will want to call these.)
IUP_EXPORTI void iupCocoaCommonBaseSetContextMenuForWidget(Ihandle* ih, id ih_widget_to_attach_menu_to, Ihandle* menu_ih);
IUP_EXPORTI int iupCocoaCommonBaseSetContextMenuAttrib(Ihandle* ih, const char* value);
IUP_EXPORTI char* iupCocoaCommonBaseGetContextMenuAttrib(Ihandle* ih);


#if 0
/* global variables, declared in iupmac_globalattrib.c */
extern int iupmac_utf8autoconvert;         


/* common */
gboolean iupmacEnterLeaveEvent(GtkWidget *widget, GdkEventCrossing *evt, Ihandle* ih);
gboolean iupmacShowHelp(GtkWidget *widget, GtkWidgetHelpType *arg1, Ihandle* ih);
GtkFixed* iupmacBaseGetFixed(Ihandle* ih);
void iupmacBaseAddToParent(Ihandle* ih);
void iupgdkColorSet(GdkColor* color, unsigned char r, unsigned char g, unsigned char b);
int iupmacSetDragDropAttrib(Ihandle* ih, const char* value);
int iupmacSetMnemonicTitle(Ihandle* ih, GtkLabel* label, const char* value);
char* iupmacStrConvertToUTF8(const char* str);
char* iupmacStrConvertFromUTF8(const char* str);
void iupmacReleaseConvertUTF8(void);
char* iupmacStrConvertFromFilename(const char* str);
char* iupmacStrConvertToFilename(const char* str);
void iupmacUpdateMnemonic(Ihandle* ih);
gboolean iupmacMotionNotifyEvent(GtkWidget *widget, GdkEventMotion *evt, Ihandle *ih);
gboolean iupmacButtonEvent(GtkWidget *widget, GdkEventButton *evt, Ihandle *ih);
void iupmacBaseSetBgColor(InativeHandle* handle, unsigned char r, unsigned char g, unsigned char b);
void iupmacBaseSetFgColor(InativeHandle* handle, unsigned char r, unsigned char g, unsigned char b);
void iupmacBaseSetFgGdkColor(InativeHandle* handle, GdkColor *color);


/* focus */
gboolean iupmacFocusInOutEvent(GtkWidget *widget, GdkEventFocus *evt, Ihandle* ih);


/* key */
gboolean iupmacKeyPressEvent(GtkWidget *widget, GdkEventKey *evt, Ihandle* ih);
gboolean iupmacKeyReleaseEvent(GtkWidget *widget, GdkEventKey *evt, Ihandle* ih);
void iupmacButtonKeySetStatus(guint state, unsigned int but, char* status, int doubleclick);
// original code used guint. Not sure what those are; changing to NSUInteger
// FIXME: file does not compile
//void iupmacKeyEncode(int key, NSUInteger *keyval, NSUInteger *state);


/* font */
char* iupmacGetPangoFontDescAttrib(Ihandle *ih);
char* iupmacGetFontIdAttrib(Ihandle *ih);
PangoFontDescription* iupmacGetPangoFontDesc(const char* value);
char* iupmacFindPangoFontDesc(PangoFontDescription* fontdesc);
void iupmacFontUpdatePangoLayout(Ihandle* ih, PangoLayout* layout);
void iupmacFontUpdateObjectPangoLayout(Ihandle* ih, gpointer object);


/* open */
char* iupmacGetNativeWindowHandle(Ihandle* ih);
void iupmacPushVisualAndColormap(void* visual, void* colormap);
void* iupmacGetNativeGraphicsContext(GtkWidget* widget);
void iupmacReleaseNativeGraphicsContext(GtkWidget* widget, void* gc);
void iupmacUpdateGlobalColors(GtkStyle* style);


/* dialog */
gboolean iupmacDialogDeleteEvent(GtkWidget *widget, GdkEvent *evt, Ihandle *ih);

#endif

#ifdef __cplusplus
}
#endif

#endif
