/** \file
 * \brief Canvas Control
 *
 * See Copyright Notice in "iup.h"
 */

#include <Cocoa/Cocoa.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdarg.h>
#include <limits.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_layout.h"
#include "iup_attrib.h"
#include "iup_dialog.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_drvinfo.h"
#include "iup_drvfont.h"
#include "iup_canvas.h"
#include "iup_key.h"
#include "iup_class.h" // needed for iup_classbase.h
#include "iup_classbase.h" // iupROUND

#include "iupcocoa_drv.h"



@interface IupCocoaCanvasView : NSView
{
	Ihandle* _ih;
}
@property(nonatomic, assign) Ihandle* ih;
@end

@implementation IupCocoaCanvasView
@synthesize ih = _ih;

- (instancetype) initWithFrame:(NSRect)frame_rect ih:(Ihandle*)ih
{
	self = [super initWithFrame:frame_rect];
	if(self)
	{
		_ih = ih;
	}
	return self;
}

- (void)drawRect:(NSRect)the_rect
{
	Ihandle* ih = _ih;
	
	// Obtain the Quartz context from the current NSGraphicsContext at the time the view's
	// drawRect method is called. This context is only appropriate for drawing in this invocation
	// of the drawRect method.
	// Interesting: graphicsPort is deprecated in 10.10
	// CGContextRef cg_context = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
	// Use [[NSGraphicsContext currentContext] CGContext] in 10.10+
	CGContextRef cg_context = [[NSGraphicsContext currentContext] CGContext];
	
	CGContextSaveGState(cg_context);

	// IUP has inverted y-coordinates compared to Cocoa (which uses Cartesian like OpenGL and your math books)
	// If we are clever, we can invert the transform matrix so that the rest of the drawing code doesn't need to care.
	{
		// just inverting the y-scale causes the rendered area to flip off-screen. So we need to shift the canvas so it is centered before inverting it.
		CGFloat translate_y = the_rect.size.height * 0.5;
		CGContextTranslateCTM(cg_context, 0.0, +translate_y);
		CGContextScaleCTM(cg_context, 1.0,  -1.0);
		CGContextTranslateCTM(cg_context, 0.0, -translate_y);
	}
	
	IFnff call_back = (IFnff)IupGetCallback(ih, "ACTION");
	if(call_back)
	{
		call_back(ih, ih->data->posx, ih->data->posy);
	}
	CGContextRestoreGState(cg_context);
}


//////// Keyboard stuff

- (BOOL) acceptsFirstResponder
{
	return YES;
}

- (void) flagsChanged:(NSEvent*)the_event
{
//	NSLog(@"flagsChanged: %@", the_event);
//	NSLog(@"modifierFlags: 0x%X", [the_event modifierFlags]);
/*
    NSEventModifierFlagCapsLock           = 1 << 16, // Set if Caps Lock key is pressed.
    NSEventModifierFlagShift              = 1 << 17, // Set if Shift key is pressed.
    NSEventModifierFlagControl            = 1 << 18, // Set if Control key is pressed.
    NSEventModifierFlagOption             = 1 << 19, // Set if Option or Alternate key is pressed.
    NSEventModifierFlagCommand            = 1 << 20, // Set if Command key is pressed.
    NSEventModifierFlagNumericPad         = 1 << 21, // Set if any key in the numeric keypad is pressed.
    NSEventModifierFlagHelp               = 1 << 22, // Set if the Help key is pressed.
    NSEventModifierFlagFunction           = 1 << 23, // Set if any function key is pressed.
*/
	Ihandle* ih = [self ih];
    unsigned short mac_key_code = [the_event keyCode];
//    NSLog(@"mac_key_code : %d", mac_key_code);
	bool should_not_propagate = iupCocoaModifierEvent(ih, the_event, (int)mac_key_code);
	if(!should_not_propagate)
	{
		[super flagsChanged:the_event];
	}
}

- (void) keyDown:(NSEvent*)the_event
{
    // gets ihandle
    Ihandle* ih = [self ih];
//	NSLog(@"keyDown: %@", the_event);
    unsigned short mac_key_code = [the_event keyCode];
//    NSLog(@"keydown string: %d", mac_key_code);

	bool should_not_propagate = iupCocoaKeyEvent(ih, the_event, (int)mac_key_code, true);
	if(!should_not_propagate)
	{
		[super keyDown:the_event];
	}
}

- (void) keyUp:(NSEvent*)the_event
{
	Ihandle* ih = [self ih];
    unsigned short mac_key_code = [the_event keyCode];
	bool should_not_propagate = iupCocoaKeyEvent(ih, the_event, (int)mac_key_code, false);
	if(!should_not_propagate)
	{
		[super keyUp:the_event];
	}
}

//////// Mouse stuff

- (void) mouseDown:(NSEvent*)the_event
{
	Ihandle* ih = _ih;
	bool should_not_propagate = iupCocoaCommonBaseHandleMouseButtonCallback(ih, the_event, self, true);
	if(!should_not_propagate)
	{
		[super mouseDown:the_event];
	}
}

- (void) mouseDragged:(NSEvent*)the_event
{
	Ihandle* ih = _ih;
	bool should_not_propagate = iupCocoaCommonBaseHandleMouseMotionCallback(ih, the_event, self);
	if(!should_not_propagate)
	{
		[super mouseDragged:the_event];
	}
}

- (void) mouseUp:(NSEvent*)the_event
{
	Ihandle* ih = _ih;
	bool should_not_propagate = iupCocoaCommonBaseHandleMouseButtonCallback(ih, the_event, self, false);
	if(!should_not_propagate)
	{
		[super mouseUp:the_event];
	}
}

// I learned that if I don't call super, the context menu doesn't activate.
- (void) rightMouseDown:(NSEvent*)the_event
{
	Ihandle* ih = _ih;
	bool should_not_propagate = iupCocoaCommonBaseHandleMouseButtonCallback(ih, the_event, self, true);
	if(!should_not_propagate)
	{
		[super rightMouseDown:the_event];
	}
}

- (void) rightMouseDragged:(NSEvent*)the_event
{
	Ihandle* ih = _ih;
	bool should_not_propagate = iupCocoaCommonBaseHandleMouseMotionCallback(ih, the_event, self);
	if(!should_not_propagate)
	{
		[super rightMouseDragged:the_event];
	}
}

- (void) rightMouseUp:(NSEvent*)the_event
{
	Ihandle* ih = _ih;
	bool should_not_propagate = iupCocoaCommonBaseHandleMouseButtonCallback(ih, the_event, self, false);
	if(!should_not_propagate)
	{
		[super rightMouseUp:the_event];
	}
}

- (void) otherMouseDown:(NSEvent*)the_event
{
	Ihandle* ih = _ih;
	bool should_not_propagate = iupCocoaCommonBaseHandleMouseButtonCallback(ih, the_event, self, true);
	if(!should_not_propagate)
	{
		[super otherMouseDown:the_event];
	}
}

- (void) otherMouseDragged:(NSEvent*)the_event
{
	Ihandle* ih = _ih;
	bool should_not_propagate = iupCocoaCommonBaseHandleMouseMotionCallback(ih, the_event, self);
	if(!should_not_propagate)
	{
		[super otherMouseDragged:the_event];
	}
}

- (void) otherMouseUp:(NSEvent*)the_event
{
	Ihandle* ih = _ih;
	bool should_not_propagate = iupCocoaCommonBaseHandleMouseButtonCallback(ih, the_event, self, false);
	if(!should_not_propagate)
	{
		[super otherMouseUp:the_event];
	}
}


@end


static NSView* cocoaCanvasGetRootView(Ihandle* ih)
{
	NSView* root_container_view = (NSView*)ih->handle;
	NSCAssert([root_container_view isKindOfClass:[NSView class]], @"Expected NSView");
	return root_container_view;
}

static NSScrollView* cocoaCanvasGetScrollView(Ihandle* ih)
{
	if(iupAttribGetBoolean(ih, "_IUPCOCOA_CANVAS_HAS_SCROLLBAR"))
	{
		NSScrollView* scroll_view = (NSScrollView*)ih->handle;
		NSCAssert([scroll_view isKindOfClass:[NSScrollView class]], @"Expected NSScrollView");
		return scroll_view;
	}
	else
	{
		return nil;
	}
}

static IupCocoaCanvasView* cocoaCanvasGetCanvasView(Ihandle* ih)
{
	if(iupAttribGetBoolean(ih, "_IUPCOCOA_CANVAS_HAS_SCROLLBAR"))
	{
		NSScrollView* scroll_view = cocoaCanvasGetScrollView(ih);
		IupCocoaCanvasView* canvas_view = (IupCocoaCanvasView*)[scroll_view documentView];
		NSCAssert([canvas_view isKindOfClass:[IupCocoaCanvasView class]], @"Expected IupCocoaCanvasView");
		return canvas_view;
	}
	else
	{
		IupCocoaCanvasView* canvas_view = (IupCocoaCanvasView*)ih->handle;
		return canvas_view;
	}
}




static char* cocoaCanvasGetDrawableAttrib(Ihandle* ih)
{
	IupCocoaCanvasView* canvas_view = cocoaCanvasGetCanvasView(ih);
	CGContextRef cg_context = NULL;
	[canvas_view lockFocus];
	// Interesting: graphicsPort is deprecated in 10.10
	// cg_context = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
	// Use [[NSGraphicsContext currentContext] CGContext] in 10.10+
	cg_context = [[NSGraphicsContext currentContext] CGContext];
	[canvas_view unlockFocus];
	
	return (char*)cg_context;
}

static char* cocoaCanvasGetDrawSizeAttrib(Ihandle *ih)
{
	int w, h;

	// scrollview or canvas view?
	IupCocoaCanvasView* canvas_view = cocoaCanvasGetCanvasView(ih);

	NSRect the_frame = [canvas_view frame];
	w = iupROUND(the_frame.size.width);
	h = iupROUND(the_frame.size.height);
	
	return iupStrReturnIntInt(w, h, 'x');
}



static int cocoaCanvasSetContextMenuAttrib(Ihandle* ih, const char* value)
{
	Ihandle* menu_ih = (Ihandle*)value;
 	IupCocoaCanvasView* canvas_view = cocoaCanvasGetCanvasView(ih);
	iupCocoaCommonBaseSetContextMenuForWidget(ih, canvas_view, menu_ih);

	return 1;
}

static int cocoaCanvasMapMethod(Ihandle* ih)
{
	NSView* root_view = nil;
	IupCocoaCanvasView* canvas_view = [[IupCocoaCanvasView alloc] initWithFrame:NSZeroRect ih:ih];
	
	if(iupAttribGetBoolean(ih, "SPIN"))
	{
		NSScrollView* scroll_view = [[NSScrollView alloc] initWithFrame:NSZeroRect];
		[scroll_view setDocumentView:canvas_view];
		[canvas_view release];
		[scroll_view setHasVerticalScroller:YES];
	
		[scroll_view setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
		[canvas_view setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
		
		root_view = scroll_view;
		iupAttribSet(ih, "_IUPCOCOA_CANVAS_HAS_SCROLLBAR", "1");
	}
	else
	{
		root_view = canvas_view;
	}
	
	
	ih->handle = root_view;

	
	
	// All Cocoa views shoud call this to add the new view to the parent view.
	iupCocoaAddToParent(ih);
	

	
	
	
	
	
	return IUP_NOERROR;
}

static void cocoaCanvasUnMapMethod(Ihandle* ih)
{
	id root_view = ih->handle;
	iupCocoaRemoveFromParent(ih);
	[root_view release];
	ih->handle = NULL;
}




void iupdrvCanvasInitClass(Iclass* ic)
{
	/* Driver Dependent Class functions */
	ic->Map = cocoaCanvasMapMethod;
	ic->UnMap = cocoaCanvasUnMapMethod;
#if 0
	ic->LayoutUpdate = gtkCanvasLayoutUpdateMethod;
	
	/* Driver Dependent Attribute functions */
	
	/* Visual */
	iupClassRegisterAttribute(ic, "BGCOLOR", NULL, gtkCanvasSetBgColorAttrib, "255 255 255", NULL, IUPAF_DEFAULT);  /* force new default value */
	
	/* IupCanvas only */
#endif
	iupClassRegisterAttribute(ic, "DRAWSIZE", cocoaCanvasGetDrawSizeAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
	
#if 0
	iupClassRegisterAttribute(ic, "DX", NULL, gtkCanvasSetDXAttrib, NULL, NULL, IUPAF_NO_INHERIT);  /* force new default value */
	iupClassRegisterAttribute(ic, "DY", NULL, gtkCanvasSetDYAttrib, NULL, NULL, IUPAF_NO_INHERIT);  /* force new default value */
	iupClassRegisterAttribute(ic, "POSX", iupCanvasGetPosXAttrib, gtkCanvasSetPosXAttrib, "0", NULL, IUPAF_NO_INHERIT);  /* force new default value */
	iupClassRegisterAttribute(ic, "POSY", iupCanvasGetPosYAttrib, gtkCanvasSetPosYAttrib, "0", NULL, IUPAF_NO_INHERIT);  /* force new default value */
#endif
	
	iupClassRegisterAttribute(ic, "DRAWABLE", cocoaCanvasGetDrawableAttrib, NULL, NULL, NULL, IUPAF_NO_STRING);

#if 0
	/* IupCanvas Windows or X only */
#ifndef GTK_MAC
#ifdef WIN32
	iupClassRegisterAttribute(ic, "HWND", iupgtkGetNativeWindowHandle, NULL, NULL, NULL, IUPAF_NO_STRING|IUPAF_NO_INHERIT);
#else
	iupClassRegisterAttribute(ic, "XWINDOW", iupgtkGetNativeWindowHandle, NULL, NULL, NULL, IUPAF_NO_INHERIT|IUPAF_NO_STRING);
	iupClassRegisterAttribute(ic, "XDISPLAY", (IattribGetFunc)iupdrvGetDisplay, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT|IUPAF_NO_STRING);
#endif
#endif
	
	/* Not Supported */
	iupClassRegisterAttribute(ic, "BACKINGSTORE", NULL, NULL, "YES", NULL, IUPAF_NOT_SUPPORTED|IUPAF_NO_INHERIT);
	iupClassRegisterAttribute(ic, "TOUCH", NULL, NULL, NULL, NULL, IUPAF_NOT_SUPPORTED|IUPAF_NO_INHERIT);
#endif

	/* New API for view specific contextual menus (Mac only) */
	iupClassRegisterAttribute(ic, "CONTEXTMENU", iupCocoaCommonBaseGetContextMenuAttrib, cocoaCanvasSetContextMenuAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
}
