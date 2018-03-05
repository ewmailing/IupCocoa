/** \file
 * \brief Web Browser Control
 *
 * See Copyright Notice in "iup.h"
 */
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <WebKit/WebKit.h>
#import <objc/runtime.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"

#include "iup_str.h"
#include "iup_webbrowser.h"
#include "iup_drv.h"
#include "iup_drvfont.h"

#include "iupcocoatouch_drv.h"

// the point of this is we have a unique memory address for an identifier
static const void* IUP_COCOATOUCH_WEBVIEW_DELEGATE_OBJ_KEY = "IUP_COCOATOUCH_WEBVIEW_DELEGATE_OBJ_KEY";

typedef NS_ENUM(NSInteger, IupCocoaTouchWebViewLoadStatus)
{
	IupCocoaTouchWebViewLoadStatusFinished,
	IupCocoaTouchWebViewLoadStatusFailed,
	IupCocoaTouchWebViewLoadStatusLoading
};

@interface IupCocoaTouchWebViewDelegate : NSObject <UIWebViewDelegate>

@property(nonatomic, assign) Ihandle* ihandle;
@property(nonatomic, assign) IupCocoaTouchWebViewLoadStatus currentLoadStatus;
@end

@implementation IupCocoaTouchWebViewDelegate

// NOTE: I don't see a clear place to implement the NEWWINDOW_CB. Desktop Cocoa WebView has createWebViewWithRequest, but there doesn't seem to be a UIWebView counterpart.

// WebView isn't the same as UIWebview:
- (BOOL) webView:(UIWebView*)web_view shouldStartLoadWithRequest:(NSURLRequest*)url_request navigationType:(UIWebViewNavigationType)navigation_type
// http://forums.macrumors.com/showthread.php?t=1077692
// WebPolicyDelegate
//- (void) webView:(WebView*)the_sender decidePolicyForNavigationAction:(NSDictionary*)action_information request:(NSURLRequest*)url_request frame:(WebFrame*)web_frame decisionListener:(id<WebPolicyDecisionListener>)policy_listener
{
	
	Ihandle* ih = [self ihandle];
	
	// Only trigger the callback for clicked links. Other platforms don't trigger for things like back button.
	if(UIWebViewNavigationTypeLinkClicked == navigation_type)
	{
		IFns cb = (IFns)IupGetCallback(ih, "NAVIGATE_CB");
		if (cb)
		{
			const char* c_url = [[[url_request URL] absoluteString] UTF8String];
			
			if(cb(ih, (char*)c_url) == IUP_IGNORE)
			{
				return NO;
			}
			else
			{
				return YES;

			}
		}
	}
	return YES;

}


- (void) webView:(UIWebView*)web_view didFailLoadWithError:(NSError*)the_error
{
	Ihandle* ih = [self ihandle];

	[self setCurrentLoadStatus:IupCocoaTouchWebViewLoadStatusFailed];
	
	IFns cb = (IFns)IupGetCallback(ih, "ERROR_CB");
	if (cb)
	{
		// Might be useful
		// [[the_error localizedDescription] UTF8String]
		// [the_error code]
	
//		NSLog(@"webView:didFailLoadWithError:forFrame: %@", the_error);

		// In testing, I get error 999 a lot.
		// "It actually means that another request is made before the previous request is completed."
		// https://stackoverflow.com/questions/30024244/webview-didfailloadwitherror-error-code-999
		// Maybe we should suppresss.
		// TODO: Introduce API to pass error code and error string.
		
		const char* failed_url = [[[the_error userInfo] valueForKey:NSURLErrorFailingURLStringErrorKey] UTF8String];
		cb(ih, (char*)failed_url);
	}

}

- (void) webViewDidFinishLoad:(UIWebView*)web_view
{
	Ihandle* ih = [self ihandle];

	[self setCurrentLoadStatus:IupCocoaTouchWebViewLoadStatusFinished];

	IFns cb = (IFns)IupGetCallback(ih, "COMPLETED_CB");
	if (cb)
	{
//		const char* c_url = [[[[[web_frame provisionalDataSource] request] URL] absoluteString] UTF8String];
		const char* c_url = [[[[web_view request] URL] absoluteString] UTF8String];
		cb(ih, (char*)c_url);
	}
	
}


- (void) webViewDidStartLoad:(UIWebView*)web_view;
{
	[self setCurrentLoadStatus:IupCocoaTouchWebViewLoadStatusLoading];
}

@end





/*********************************************************************************************/

/*
#if 0
static char* cocoaTouchWebBrowserGetItemHistoryAttrib(Ihandle* ih, int index)
{
	// Negative values represent steps backward while positive values represent steps forward.
	UIWebView* web_view = (UIWebView*)ih->handle;

    return NULL;
}

static char* cocoaTouchWebBrowserGetForwardCountAttrib(Ihandle* ih)
{
	UIWebView* web_view = (UIWebView*)ih->handle;

	return iupStrReturnInt(0);
}

static char* cocoaTouchWebBrowserGetBackCountAttrib(Ihandle* ih)
{
	UIWebView* web_view = (UIWebView*)ih->handle;

	return iupStrReturnInt(0);
}
#endif
*/
static int cocoaTouchWebBrowserSetHTMLAttrib(Ihandle* ih, const char* value)
{
	if(value)
	{
		UIWebView* web_view = (UIWebView*)ih->handle;
		NSString* html_string = [NSString stringWithUTF8String:value];
		[web_view loadHTMLString:html_string baseURL:nil];
	}
	return 0; /* do not store value in hash table */
}


static int cocoaTouchWebBrowserSetCopyAttrib(Ihandle* ih, const char* value)
{
	(void)value;

	UIWebView* web_view = (UIWebView*)ih->handle;
	// use the responder chain. WebView provides a custom implementation. Does this work with UIWebView?
	[web_view copy:nil];

/*
	DOMRange* dom_range = [web_view selectedDOMRange];
	NSPasteboard* paste_board = [NSPasteboard generalPasteboard];

	[paste_board setData:[[dom_range webArchive] data] forType:WebArchivePboardType];
*/
	
	return 0;
}

static int cocoaTouchWebBrowserSetSelectAllAttrib(Ihandle* ih, const char* value)
{
	(void)value;
	UIWebView* web_view = (UIWebView*)ih->handle;
	// use the responder chain. WebView provides a custom implementation. Does this work with UIWebView?
	[web_view selectAll:nil];
	return 0;
}

static int cocoaTouchWebBrowserSetPrintAttrib(Ihandle* ih, const char* value)
{
	(void)value;
	UIWebView* web_view = (UIWebView*)ih->handle;
	
	
	
	UIPrintInfo* print_info = [UIPrintInfo printInfo];
	[print_info setOutputType:UIPrintInfoOutputGeneral];
	[print_info setJobName:[[[web_view request] URL] absoluteString]];

	
	UIPrintInteractionController* print_controller = [UIPrintInteractionController sharedPrintController];
	[print_controller setPrintInfo:print_info];
	// Internet suggests YES causes some problems with scaling to fit. Say no?
	[print_controller setShowsPageRange:NO];
	[print_controller setPrintFormatter:[web_view viewPrintFormatter]];
	[print_controller presentAnimated:YES completionHandler:
		^(UIPrintInteractionController* print_contoller_completion, BOOL is_completed, NSError* the_error)
		{
			if(nil != the_error)
			{
				NSLog(@"Print error: %@", the_error);
			}
		}
	];
	
	
	
	return 0;
}

#if 0
static int cocoaTouchWebBrowserSetZoomAttrib(Ihandle* ih, const char* value)
{
  int zoom;
  if (iupStrToInt(value, &zoom))
    webkit_web_view_set_zoom_level((WebKitWebView*)ih->handle, (float)zoom/100.0f);
  return 0;
}

static char* cocoaTouchWebBrowserGetZoomAttrib(Ihandle* ih)
{
  int zoom = (int)(webkit_web_view_get_zoom_level((WebKitWebView*)ih->handle) * 100);
  return iupStrReturnInt(zoom);
}
#endif

/*
 Besides implementing the webView:didStartProvisionalLoadForFrame: method to display the page title, you can also use it to display the status, for example, “Loading.” Remember that at this point the content has only been requested, not loaded; therefore, the data source is provisional.
 
 Similarly, implement the webView:didFinishLoadForFrame:, webView:didFailProvisionalLoadWithError:forFrame: and webView:didFailLoadWithError:forFrame: delegate methods to receive notification when a page has been loaded successfully or unsuccessfully. You might want to display a message if an error occurred.
*/
static char* cocoaTouchWebBrowserGetStatusAttrib(Ihandle* ih)
{
	UIWebView* web_view = (UIWebView*)ih->handle;

	// The state must be tracked through delegate callbacks. We save the current state directly in the delegate for convenience.
	IupCocoaTouchWebViewDelegate* webview_delegate = (IupCocoaTouchWebViewDelegate*)objc_getAssociatedObject(web_view, IUP_COCOATOUCH_WEBVIEW_DELEGATE_OBJ_KEY);
	
	IupCocoaTouchWebViewLoadStatus current_status = [webview_delegate currentLoadStatus];
	
	switch(current_status)
	{
		case IupCocoaTouchWebViewLoadStatusFailed:
		{
			return "FAILED";
			break;
		}
		case IupCocoaTouchWebViewLoadStatusLoading:
		{
			return "LOADING";
			break;
		}
		case IupCocoaTouchWebViewLoadStatusFinished:
		{
			return "COMPLETED";
			break;
		}
		default:
		{
			NSLog(@"Unexpected case in CocoaTouchWebBrowserGetStatusAttrib");
			return "FAILED";
		}
	}
	return "FAILED";
}

static int cocoaTouchWebBrowserSetReloadAttrib(Ihandle* ih, const char* value)
{
	UIWebView* web_view = (UIWebView*)ih->handle;
	[web_view reload];
	return 0; /* do not store value in hash table */
}

static int cocoaTouchWebBrowserSetStopAttrib(Ihandle* ih, const char* value)
{
	UIWebView* web_view = (UIWebView*)ih->handle;
	[web_view stopLoading];
	return 0; /* do not store value in hash table */
}

// iOS doesn't have the WebBackForwardList as on Mac.
// This is the brute force way of doing it, but I'm not convinced this is reliable.
// The API doesn't specifiy what happens if you try to keep going back while loading.
// From a user-perspective, I've seen sites with aggressive redirects fight the back button and events get eaten.
// This is implemented for completeness/convenience, but officially I don't want to say this is supported.
// Please use the new CANGOBACK/FORWARD, and GOBACK/FORWARD APIs instead.
static int cocoaTouchWebBrowserSetBackForwardAttrib(Ihandle* ih, const char* value)
{
	int val;
	if(iupStrToInt(value, &val))
	{
		UIWebView* web_view = (UIWebView*)ih->handle;
		
		/* Negative values represent steps backward while positive values represent steps forward. */
		if(val > 0)
		{
			for(int i = 0; i < val; i++)
			{
				[web_view goForward];
			}
		}
		else if(val < 0)
		{
			for(int i = 0; i < -(val); i++)
			{
				[web_view goBack];
			}
		}
	}
		
	return 0; // do not store value in hash table
}


static int cocoaTouchWebBrowserSetValueAttrib(Ihandle* ih, const char* value)
{
	if(value)
	{
		UIWebView* web_view = (UIWebView*)ih->handle;
		NSString* ns_string = [NSString stringWithUTF8String:value];
		NSURL* ns_url = [NSURL URLWithString:ns_string];
		NSURLRequest* url_request = [NSURLRequest requestWithURL:ns_url];
		[web_view loadRequest:url_request];
	}
	return 0; /* do not store value in hash table */
}

static char* cocoaTouchWebBrowserGetValueAttrib(Ihandle* ih)
{
	UIWebView* web_view = (UIWebView*)ih->handle;
	const char* c_url = [[[[web_view request] URL] absoluteString] UTF8String];
	return iupStrReturnStr(c_url);
}


static char* cocoaTouchWebBrowserCanGoBackAttrib(Ihandle* ih)
{
	UIWebView* web_view = (UIWebView*)ih->handle;
	BOOL is_true = [web_view canGoBack];
	return iupStrReturnBoolean((int)is_true);
}

static char* cocoaTouchWebBrowserCanGoForwardAttrib(Ihandle* ih)
{
	UIWebView* web_view = (UIWebView*)ih->handle;
	BOOL is_true = [web_view canGoForward];
	return iupStrReturnBoolean((int)is_true);
}

static int cocoaTouchWebBrowserSetBackAttrib(Ihandle* ih, const char* value)
{
	(void)value;
	
	UIWebView* web_view = (UIWebView*)ih->handle;
	[web_view goBack];
	
	return 0; /* do not store value in hash table */
}

static int cocoaTouchWebBrowserSetForwardAttrib(Ihandle* ih, const char* value)
{
	(void)value;
	
	UIWebView* web_view = (UIWebView*)ih->handle;
	[web_view goForward];
	
	return 0; /* do not store value in hash table */
}



/*********************************************************************************************/


static int cocoaTouchWebBrowserMapMethod(Ihandle* ih)
{
//	char* value;


	UIWebView* web_view = [[UIWebView alloc] initWithFrame:CGRectZero];
//	UIWebView* web_view = [[UIWebView alloc] initWithFrame:CGRectMake(0,0,480, 640)];
	[web_view setScalesPageToFit:YES];
	
	
	
	
	
	ih->handle = web_view;
	
	
	
	
	IupCocoaTouchWebViewDelegate* webview_delegate = [[IupCocoaTouchWebViewDelegate alloc] init];
	[webview_delegate setIhandle:ih];
	// I'm using objc_setAssociatedObject/objc_getAssociatedObject because it allows me to avoid making subclasses just to hold ivars.
	// We're going to use OBJC_ASSOCIATION_RETAIN because I do believe it will do the right thing for us.
	objc_setAssociatedObject(web_view, IUP_COCOATOUCH_WEBVIEW_DELEGATE_OBJ_KEY, (id)webview_delegate, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
	[webview_delegate release];

	
	[web_view setDelegate:webview_delegate];

	
	
	// All CocoaTouch views shoud call this to add the new view to the parent view.
	iupCocoaTouchAddToParent(ih);

	
	return IUP_NOERROR;
}

static void cocoaTouchWebBrowserUnMapMethod(Ihandle* ih)
{
	UIWebView* web_view = (UIWebView*)ih->handle;

/*
	id web_view = objc_getAssociatedObject(the_button, IUP_COCOATOUCH_WEBVIEW_DELEGATE_OBJ_KEY);
	objc_setAssociatedObject(the_button, IUP_COCOATOUCH_WEBVIEW_DELEGATE_OBJ_KEY, nil, OBJC_ASSOCIATION_ASSIGN);
	[web_view release];
*/
	iupCocoaTouchRemoveFromParent(ih);

	[web_view release];
	ih->handle = NULL;
	
}






static void cocoaTouchWebBrowserComputeNaturalSizeMethod(Ihandle* ih, int *w, int *h, int *children_expand)
{
  int natural_w = 0, natural_h = 0;
  (void)children_expand; /* unset if not a container */

  /* natural size is 1 character */
  iupdrvFontGetCharSize(ih, &natural_w, &natural_h);

  *w = natural_w;
  *h = natural_h;
	
	/*
	*w = 480;
	*h = 640;
*/
	// FIXME: Iup is not expanding and making the visible height 1 row. Screen size routines never seem to be hit.
	*w = 480;
	*h = 480;
}

static int cocoaTouchWebBrowserCreateMethod(Ihandle* ih, void **params)
{
	ih->expand = IUP_EXPAND_BOTH;

#if 0
  (void)params;

  ih->data = iupALLOCCTRLDATA();

  /* default EXPAND is YES */
  ih->expand = IUP_EXPAND_BOTH;
  ih->data->sb = IUP_SB_HORIZ | IUP_SB_VERT;  /* default is YES */

#endif

  return IUP_NOERROR;
}

Iclass* iupWebBrowserNewClass(void)
{
  Iclass* ic = iupClassNew(NULL);

  ic->name = "webbrowser";
  ic->format = NULL; /* no parameters */
  ic->nativetype  = IUP_TYPECONTROL;
  ic->childtype   = IUP_CHILDNONE;
  ic->is_interactive = 1;
  ic->has_attrib_id = 1;   /* has attributes with IDs that must be parsed */

  /* Class functions */
  ic->New = iupWebBrowserNewClass;
  ic->Create = cocoaTouchWebBrowserCreateMethod;
  ic->Map = cocoaTouchWebBrowserMapMethod;
  ic->UnMap = cocoaTouchWebBrowserUnMapMethod;
  ic->ComputeNaturalSize = cocoaTouchWebBrowserComputeNaturalSizeMethod;
  ic->LayoutUpdate = iupdrvBaseLayoutUpdateMethod;

  /* Callbacks */
  iupClassRegisterCallback(ic, "NEWWINDOW_CB", "s");
  iupClassRegisterCallback(ic, "NAVIGATE_CB", "s");
  iupClassRegisterCallback(ic, "ERROR_CB", "s");

  /* Common */
  iupBaseRegisterCommonAttrib(ic);

  /* Visual */
  iupBaseRegisterVisualAttrib(ic);

  /* Overwrite Visual */
  iupClassRegisterAttribute(ic, "BGCOLOR", NULL, iupdrvBaseSetBgColorAttrib, IUPAF_SAMEASSYSTEM, "DLGBGCOLOR", IUPAF_DEFAULT); 

  // TODO: UIWebView doesn't have arbitrary back/forward history list APIs, but does support 1 step goBack/goForward and canGoBack/Forward query APIs. Consider adding to official API "GOBACK", "GOFORWARD", "CANGOBACK", "CANGOFORWARD"

  /* IupWebBrowser only */
  iupClassRegisterAttribute(ic, "VALUE", cocoaTouchWebBrowserGetValueAttrib, cocoaTouchWebBrowserSetValueAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "BACKFORWARD", NULL, cocoaTouchWebBrowserSetBackForwardAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "STOP", NULL, cocoaTouchWebBrowserSetStopAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "RELOAD", NULL, cocoaTouchWebBrowserSetReloadAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "HTML", NULL, cocoaTouchWebBrowserSetHTMLAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "STATUS", cocoaTouchWebBrowserGetStatusAttrib, NULL, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "COPY", NULL, cocoaTouchWebBrowserSetCopyAttrib, NULL, NULL, IUPAF_WRITEONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SELECTALL", NULL, cocoaTouchWebBrowserSetSelectAllAttrib, NULL, NULL, IUPAF_WRITEONLY | IUPAF_NO_INHERIT);
#if 0
  iupClassRegisterAttribute(ic, "ZOOM", cocoaTouchWebBrowserGetZoomAttrib, cocoaTouchWebBrowserSetZoomAttrib, NULL, NULL, IUPAF_NO_INHERIT);
#endif
  iupClassRegisterAttribute(ic, "PRINT", NULL, cocoaTouchWebBrowserSetPrintAttrib, NULL, NULL, IUPAF_WRITEONLY | IUPAF_NO_INHERIT);

#if 0
  iupClassRegisterAttribute(ic, "BACKCOUNT", cocoaTouchWebBrowserGetBackCountAttrib, NULL, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FORWARDCOUNT", cocoaTouchWebBrowserGetForwardCountAttrib, NULL, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "ITEMHISTORY",  cocoaTouchWebBrowserGetItemHistoryAttrib,  NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);
#endif

  iupClassRegisterAttribute(ic, "CANGOBACK", cocoaTouchWebBrowserCanGoBackAttrib, NULL, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CANGOFORWARD", cocoaTouchWebBrowserCanGoForwardAttrib, NULL, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_READONLY|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "GOBACK", NULL, cocoaTouchWebBrowserSetBackAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "GOFORWARD", NULL, cocoaTouchWebBrowserSetForwardAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);

  return ic;
}
