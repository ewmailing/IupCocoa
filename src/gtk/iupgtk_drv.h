/** \file
 * \brief GTK Driver 
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUPGTK_DRV_H 
#define __IUPGTK_DRV_H

#ifdef __cplusplus
extern "C" {
#endif

#define iupCOLORDoubleTO8(_x) ((unsigned char)(_x*255))  /* 1.0*255 = 255 */
#define iupCOLOR8ToDouble(_x) ((double)_x/255.0)



/* common */
gboolean iupgtkEnterLeaveEvent(GtkWidget *widget, GdkEventCrossing *evt, Ihandle* ih);
gboolean iupgtkMotionNotifyEvent(GtkWidget *widget, GdkEventMotion *evt, Ihandle *ih);
gboolean iupgtkButtonEvent(GtkWidget *widget, GdkEventButton *evt, Ihandle *ih);
gboolean iupgtkShowHelp(GtkWidget *widget, GtkWidgetHelpType *arg1, Ihandle* ih);

int iupgtkSetMnemonicTitle(Ihandle* ih, GtkLabel* label, const char* value);
void iupgtkUpdateMnemonic(Ihandle* ih);

void iupgdkColorSet(GdkColor* color, unsigned char r, unsigned char g, unsigned char b);
void iupgtkSetBgColor(InativeHandle* handle, unsigned char r, unsigned char g, unsigned char b);
void iupgtkSetFgColor(InativeHandle* handle, unsigned char r, unsigned char g, unsigned char b);

void iupgtkAddToParent(Ihandle* ih);
const char* iupgtkGetWidgetClassName(GtkWidget* widget);
void iupgtkSetPosSize(GtkContainer* parent, GtkWidget* widget, int x, int y, int width, int height);
GdkWindow* iupgtkGetWindow(GtkWidget *widget);
void iupgtkWindowGetPointer(GdkWindow *window, int *x, int *y, GdkModifierType *mask);
int iupgtkIsVisible(GtkWidget* widget);
void iupgtkClearSizeStyleCSS(GtkWidget* widget);

GtkWidget* iupgtkNativeContainerNew(int has_window);
void iupgtkNativeContainerAdd(GtkWidget* container, GtkWidget* widget);
void iupgtkNativeContainerMove(GtkWidget* container, GtkWidget* widget, int x, int y);

/* str */
void  iupgtkStrRelease(void);
char* iupgtkStrConvertToSystem(const char* str);
char* iupgtkStrConvertToSystemLen(const char* str, int *len);
char* iupgtkStrConvertFromSystem(const char* str);
char* iupgtkStrConvertFromFilename(const char* str);
char* iupgtkStrConvertToFilename(const char* str);
void  iupgtkStrSetUTF8Mode(int utf8mode);
int   iupgtkStrGetUTF8Mode(void);


/* focus */
gboolean iupgtkFocusInOutEvent(GtkWidget *widget, GdkEventFocus *evt, Ihandle* ih);
void iupgtkSetCanFocus(GtkWidget *widget, int can);


/* key */
gboolean iupgtkKeyPressEvent(GtkWidget *widget, GdkEventKey *evt, Ihandle* ih);
gboolean iupgtkKeyReleaseEvent(GtkWidget *widget, GdkEventKey *evt, Ihandle* ih);
void iupgtkButtonKeySetStatus(guint state, unsigned int but, char* status, int doubleclick);
int iupgtkKeyDecode(GdkEventKey *evt);


/* font */
PangoFontDescription* iupgtkGetPangoFontDesc(const char* value);
char* iupgtkGetPangoFontDescAttrib(Ihandle *ih);
char* iupgtkGetPangoLayoutAttrib(Ihandle *ih);
char* iupgtkGetFontIdAttrib(Ihandle *ih);
void iupgtkUpdateObjectFont(Ihandle* ih, gpointer object);
void iupgtkUpdateWidgetFont(Ihandle *ih, GtkWidget* widget);
PangoLayout* iupgtkGetPangoLayout(const char* value);

/* There are PANGO_SCALE Pango units in one device unit. 
  For an output backend where a device unit is a pixel, 
  a size value of 10 * PANGO_SCALE gives 10 pixels. */
#define iupGTK_PANGOUNITS2PIXELS(_x) (((_x) + PANGO_SCALE/2) / PANGO_SCALE)
#define iupGTK_PIXELS2PANGOUNITS(_x) ((_x) * PANGO_SCALE)


/* open */
char* iupgtkGetNativeWindowHandle(Ihandle* ih);
void iupgtkPushVisualAndColormap(void* visual, void* colormap);
void* iupgtkGetNativeGraphicsContext(GtkWidget* widget);
void iupgtkReleaseNativeGraphicsContext(GtkWidget* widget, void* gc);


/* dialog */
gboolean iupgtkDialogDeleteEvent(GtkWidget *widget, GdkEvent *evt, Ihandle *ih);


#ifdef __cplusplus
}
#endif

#endif
