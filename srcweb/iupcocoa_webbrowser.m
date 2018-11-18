/** \file
 * \brief Web Browser Control
 *
 * See Copyright Notice in "iup.h"
 */

#import <Cocoa/Cocoa.h>
#import <WebKit/WebKit.h>
#import <objc/runtime.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_str.h"
#include "iup_webbrowser.h"
#include "iup_drv.h"
#include "iup_drvfont.h"

#include "iupcocoa_drv.h"

// the point of this is we have a unique memory address for an identifier
static const void* IUP_COCOA_WEBVIEW_DELEGATE_OBJ_KEY = "IUP_COCOA_WEBVIEW_DELEGATE_OBJ_KEY";

typedef NS_ENUM(NSInteger, IupCocoaWebViewLoadStatus)
{
	IupCocoaWebViewLoadStatusFinished,
	IupCocoaWebViewLoadStatusFailed,
	IupCocoaWebViewLoadStatusLoading
};

@interface IupCocoaWebViewDelegate : NSObject <WebPolicyDelegate, WebFrameLoadDelegate, WebUIDelegate>

@property(nonatomic, assign) Ihandle* ihandle;
@property(nonatomic, assign) IupCocoaWebViewLoadStatus currentLoadStatus;
@end

@implementation IupCocoaWebViewDelegate

// WebView isn't the same as UIWebview:
//- (BOOL)webView:(WebView *)webView shouldStartLoadWithRequest:(NSURLRequest *)request navigationType:(WebViewNavigationType)navigationType
// http://forums.macrumors.com/showthread.php?t=1077692
// WebPolicyDelegate
- (void) webView:(WebView*)the_sender decidePolicyForNavigationAction:(NSDictionary*)action_information request:(NSURLRequest*)url_request frame:(WebFrame*)web_frame decisionListener:(id<WebPolicyDecisionListener>)policy_listener
{
	
	Ihandle* ih = [self ihandle];
	
	IFns cb = (IFns)IupGetCallback(ih, "NAVIGATE_CB");
	if (cb)
	{
		
		const char* c_url = [[[url_request URL] absoluteString] UTF8String];
		
		if(cb(ih, (char*)c_url) == IUP_IGNORE)
		{
			[policy_listener ignore];
		}
		else
		{
			[policy_listener use];

		}
	}
	
	[policy_listener use];

}

// WebFrameLoadDelegate
- (void) webView:(WebView*)the_sender didFailLoadWithError:(NSError*)the_error forFrame:(WebFrame*)web_frame
{
	Ihandle* ih = [self ihandle];

	[self setCurrentLoadStatus:IupCocoaWebViewLoadStatusFailed];
	
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

// WebFrameLoadDelegate
- (void) webView:(WebView*)the_sender didFinishLoadForFrame:(WebFrame*)web_frame
{
	Ihandle* ih = [self ihandle];

	[self setCurrentLoadStatus:IupCocoaWebViewLoadStatusFinished];

	IFns cb = (IFns)IupGetCallback(ih, "COMPLETED_CB");
	if (cb)
	{
//		const char* c_url = [[[[[web_frame provisionalDataSource] request] URL] absoluteString] UTF8String];
		const char* c_url = [[[[[web_frame dataSource] request] URL] absoluteString] UTF8String];
		cb(ih, (char*)c_url);
	}
	
}

// WebUIDelegate
- (WebView*) webView:(WebView*)the_sender createWebViewWithRequest:(NSURLRequest*)url_request
{
	// FIXME: I think the GTK version is returning the original webview and not creating a new window.
	// I'm doing the same, but I don't know if that is right.
	// Maybe the intention is that the user must explicitly create a new window themselves?
	Ihandle* ih = [self ihandle];

	IFns cb = (IFns)IupGetCallback(ih, "NEWWINDOW_CB");
	if (cb)
	{
		const char* c_url = [[[url_request URL] absoluteString] UTF8String];

		cb(ih, (char*)c_url);
	}
	
	return the_sender;
	
/*
	id myDocument = [[NSDocumentController sharedDocumentController] openUntitledDocumentAndDisplay:YES error:nil];
	[[[myDocument webView] mainFrame] loadRequest:request];
	return [myDocument webView];
*/
	
}

// WebFrameLoadDelegate
- (void) webView:(WebView*)the_sender didStartProvisionalLoadForFrame:(WebFrame*)web_frame
{
	[self setCurrentLoadStatus:IupCocoaWebViewLoadStatusLoading];
}

// WebFrameLoadDelegate
- (void) webView:(WebView*)the_sender didFailProvisionalLoadWithError:(NSError*)the_error forFrame:(WebFrame*)web_frame;
{
	[self setCurrentLoadStatus:IupCocoaWebViewLoadStatusFailed];
}


@end





/*********************************************************************************************/



static char* cocoaWebBrowserGetItemHistoryAttrib(Ihandle* ih, int index)
{
	/* Negative values represent steps backward while positive values represent steps forward. */
	WebView* web_view = (WebView*)ih->handle;
	WebBackForwardList* back_forward_list = [web_view backForwardList];
	WebHistoryItem* history_item = [back_forward_list itemAtIndex:index];
	if(nil != history_item)
	{
		const char* c_url = [[history_item URLString] UTF8String];
		return iupStrReturnStr(c_url);
	}

    return NULL;
}

static char* cocoaWebBrowserGetForwardCountAttrib(Ihandle* ih)
{
	WebView* web_view = (WebView*)ih->handle;
	WebBackForwardList* back_forward_list = [web_view backForwardList];
	int item_count = [back_forward_list forwardListCount];
	return iupStrReturnInt(item_count);
}

static char* cocoaWebBrowserGetBackCountAttrib(Ihandle* ih)
{
	WebView* web_view = (WebView*)ih->handle;
	WebBackForwardList* back_forward_list = [web_view backForwardList];
	int item_count = [back_forward_list backListCount];
	return iupStrReturnInt(item_count);
}

static int cocoaWebBrowserSetHTMLAttrib(Ihandle* ih, const char* value)
{
	if(value)
	{
		WebView* web_view = (WebView*)ih->handle;
		NSString* html_string = [NSString stringWithUTF8String:value];
		[[web_view mainFrame] loadHTMLString:html_string baseURL:nil];
	}
	return 0; /* do not store value in hash table */
}


static int cocoaWebBrowserSetCopyAttrib(Ihandle* ih, const char* value)
{
	(void)value;

	WebView* web_view = (WebView*)ih->handle;
	// use the responder chain. WebView provides a custom implementation.
	[web_view copy:nil];

/*
	DOMRange* dom_range = [web_view selectedDOMRange];
	NSPasteboard* paste_board = [NSPasteboard generalPasteboard];

	[paste_board setData:[[dom_range webArchive] data] forType:WebArchivePboardType];
*/
	
	return 0;
}

static int cocoaWebBrowserSetSelectAllAttrib(Ihandle* ih, const char* value)
{
	(void)value;
	WebView* web_view = (WebView*)ih->handle;
	// use the responder chain. WebView provides a custom implementation.
	[web_view selectAll:nil];
	return 0;
}

static int cocoaWebBrowserSetPrintAttrib(Ihandle* ih, const char* value)
{
	(void)value;
	WebView* web_view = (WebView*)ih->handle;
	WebFrameView* frame_view = [[web_view mainFrame] frameView];
	// Use documentView to print whole document instead of visible
	NSPrintOperation* print_operation = [NSPrintOperation printOperationWithView:[frame_view documentView]];
	NSWindow* parent_window = [web_view window];
	[print_operation runOperationModalForWindow:parent_window delegate:nil didRunSelector:nil contextInfo:nil];
	
	return 0;
}

#if 0
static int cocoaWebBrowserSetZoomAttrib(Ihandle* ih, const char* value)
{
  int zoom;
  if (iupStrToInt(value, &zoom))
    webkit_web_view_set_zoom_level((WebKitWebView*)ih->handle, (float)zoom/100.0f);
  return 0;
}

static char* cocoaWebBrowserGetZoomAttrib(Ihandle* ih)
{
  int zoom = (int)(webkit_web_view_get_zoom_level((WebKitWebView*)ih->handle) * 100);
  return iupStrReturnInt(zoom);
}
#endif

/*
 Besides implementing the webView:didStartProvisionalLoadForFrame: method to display the page title, you can also use it to display the status, for example, “Loading.” Remember that at this point the content has only been requested, not loaded; therefore, the data source is provisional.
 
 Similarly, implement the webView:didFinishLoadForFrame:, webView:didFailProvisionalLoadWithError:forFrame: and webView:didFailLoadWithError:forFrame: delegate methods to receive notification when a page has been loaded successfully or unsuccessfully. You might want to display a message if an error occurred.
*/
static char* cocoaWebBrowserGetStatusAttrib(Ihandle* ih)
{
	WebView* web_view = ih->handle;

	// The state must be tracked through delegate callbacks. We save the current state directly in the delegate for convenience.
	IupCocoaWebViewDelegate* webview_delegate = (IupCocoaWebViewDelegate*)objc_getAssociatedObject(web_view, IUP_COCOA_WEBVIEW_DELEGATE_OBJ_KEY);
	
	IupCocoaWebViewLoadStatus current_status = [webview_delegate currentLoadStatus];
	
	switch(current_status)
	{
		case IupCocoaWebViewLoadStatusFailed:
		{
			return "FAILED";
			break;
		}
		case IupCocoaWebViewLoadStatusLoading:
		{
			return "LOADING";
			break;
		}
		case IupCocoaWebViewLoadStatusFinished:
		{
			return "COMPLETED";
			break;
		}
		default:
		{
			NSLog(@"Unexpected case in cocoaWebBrowserGetStatusAttrib");
			return "FAILED";
		}
	}
	return "FAILED";
}

static int cocoaWebBrowserSetReloadAttrib(Ihandle* ih, const char* value)
{
	WebView* web_view = (WebView*)ih->handle;
	WebFrame* web_frame = [web_view mainFrame];
	[web_frame reload];
	return 0; /* do not store value in hash table */
}

static int cocoaWebBrowserSetStopAttrib(Ihandle* ih, const char* value)
{
	WebView* web_view = (WebView*)ih->handle;
	WebFrame* web_frame = [web_view mainFrame];
	[web_frame stopLoading];
	return 0; /* do not store value in hash table */
}

static int cocoaWebBrowserSetBackForwardAttrib(Ihandle* ih, const char* value)
{
	int val;
	if(iupStrToInt(value, &val))
	{
		/* Negative values represent steps backward while positive values represent steps forward. */
		WebView* web_view = (WebView*)ih->handle;
		WebBackForwardList* back_forward_list = [web_view backForwardList];
		WebHistoryItem* history_item = [back_forward_list itemAtIndex:val];
		if(nil != history_item)
		{
			[web_view goToBackForwardItem:history_item];
		}
	}
	return 0; /* do not store value in hash table */
}

static int cocoaWebBrowserSetValueAttrib(Ihandle* ih, const char* value)
{
	if(value)
	{
		WebView* web_view = (WebView*)ih->handle;
		NSString* ns_string = [NSString stringWithUTF8String:value];
		NSURL* ns_url = [NSURL URLWithString:ns_string];
		NSURLRequest* url_request = [NSURLRequest requestWithURL:ns_url];
		[[web_view mainFrame] loadRequest:url_request];
	}
	return 0; /* do not store value in hash table */
}

static char* cocoaWebBrowserGetValueAttrib(Ihandle* ih)
{
	WebView* web_view = (WebView*)ih->handle;
	WebFrame* web_frame = [web_view mainFrame];
	const char* c_url = [[[[[web_frame dataSource] request] URL] absoluteString] UTF8String];
	return iupStrReturnStr(c_url);
}


static char* cocoaWebBrowserCanGoBackAttrib(Ihandle* ih)
{
	WebView* web_view = (WebView*)ih->handle;
	BOOL is_true = [web_view canGoBack];
	return iupStrReturnBoolean((int)is_true);
}

static char* cocoaWebBrowserCanGoForwardAttrib(Ihandle* ih)
{
	WebView* web_view = (WebView*)ih->handle;
	BOOL is_true = [web_view canGoForward];
	return iupStrReturnBoolean((int)is_true);
}

static int cocoaWebBrowserSetBackAttrib(Ihandle* ih, const char* value)
{
	(void)value;

	WebView* web_view = (WebView*)ih->handle;
	[web_view goBack];

	return 0; /* do not store value in hash table */
}

static int cocoaWebBrowserSetForwardAttrib(Ihandle* ih, const char* value)
{
	(void)value;

	WebView* web_view = (WebView*)ih->handle;
	[web_view goForward];

	return 0; /* do not store value in hash table */
}


/*********************************************************************************************/


static int cocoaWebBrowserMapMethod(Ihandle* ih)
{
//	char* value;


	WebView* web_view = [[WebView alloc] initWithFrame:NSZeroRect frameName:nil groupName:nil];
	[web_view setShouldCloseWithWindow:YES];
	
	
	
	
	
	ih->handle = web_view;
	iupCocoaSetAssociatedViews(ih, web_view, web_view);

	
	
	
	IupCocoaWebViewDelegate* webview_delegate = [[IupCocoaWebViewDelegate alloc] init];
	[webview_delegate setIhandle:ih];
	// I'm using objc_setAssociatedObject/objc_getAssociatedObject because it allows me to avoid making subclasses just to hold ivars.
	// We're going to use OBJC_ASSOCIATION_RETAIN because I do believe it will do the right thing for us.
	objc_setAssociatedObject(web_view, IUP_COCOA_WEBVIEW_DELEGATE_OBJ_KEY, (id)webview_delegate, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
	[webview_delegate release];
	
	[web_view setPolicyDelegate:webview_delegate];
	[web_view setFrameLoadDelegate:webview_delegate];
	[web_view setUIDelegate:webview_delegate];

	
	
	// All Cocoa views shoud call this to add the new view to the parent view.
	iupCocoaAddToParent(ih);

	
	return IUP_NOERROR;
}

static void cocoaWebBrowserUnMapMethod(Ihandle* ih)
{
	WebView* web_view = ih->handle;

/*
	id web_view = objc_getAssociatedObject(the_button, IUP_COCOA_WEBVIEW_DELEGATE_OBJ_KEY);
	objc_setAssociatedObject(the_button, IUP_COCOA_WEBVIEW_DELEGATE_OBJ_KEY, nil, OBJC_ASSOCIATION_ASSIGN);
	[web_view release];
*/
	iupCocoaRemoveFromParent(ih);
	iupCocoaSetAssociatedViews(ih, nil, nil);

	[web_view release];
	ih->handle = NULL;
	
}






static void cocoaWebBrowserComputeNaturalSizeMethod(Ihandle* ih, int *w, int *h, int *children_expand)
{
  int natural_w = 0, natural_h = 0;
  (void)children_expand; /* unset if not a container */

  /* natural size is 1 character */
  iupdrvFontGetCharSize(ih, &natural_w, &natural_h);

  *w = natural_w;
  *h = natural_h;
}

static int cocoaWebBrowserCreateMethod(Ihandle* ih, void **params)
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
  ic->Create = cocoaWebBrowserCreateMethod;
  ic->Map = cocoaWebBrowserMapMethod;
  ic->UnMap = cocoaWebBrowserUnMapMethod;
  ic->ComputeNaturalSize = cocoaWebBrowserComputeNaturalSizeMethod;
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

  /* IupWebBrowser only */
  iupClassRegisterAttribute(ic, "VALUE", cocoaWebBrowserGetValueAttrib, cocoaWebBrowserSetValueAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "BACKFORWARD", NULL, cocoaWebBrowserSetBackForwardAttrib, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "STOP", NULL, cocoaWebBrowserSetStopAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "RELOAD", NULL, cocoaWebBrowserSetReloadAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "HTML", NULL, cocoaWebBrowserSetHTMLAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "STATUS", cocoaWebBrowserGetStatusAttrib, NULL, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "COPY", NULL, cocoaWebBrowserSetCopyAttrib, NULL, NULL, IUPAF_WRITEONLY | IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SELECTALL", NULL, cocoaWebBrowserSetSelectAllAttrib, NULL, NULL, IUPAF_WRITEONLY | IUPAF_NO_INHERIT);
#if 0
  iupClassRegisterAttribute(ic, "ZOOM", cocoaWebBrowserGetZoomAttrib, cocoaWebBrowserSetZoomAttrib, NULL, NULL, IUPAF_NO_INHERIT);
#endif
  iupClassRegisterAttribute(ic, "PRINT", NULL, cocoaWebBrowserSetPrintAttrib, NULL, NULL, IUPAF_WRITEONLY | IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "BACKCOUNT", cocoaWebBrowserGetBackCountAttrib, NULL, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FORWARDCOUNT", cocoaWebBrowserGetForwardCountAttrib, NULL, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttributeId(ic, "ITEMHISTORY",  cocoaWebBrowserGetItemHistoryAttrib,  NULL, IUPAF_READONLY|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "CANGOBACK", cocoaWebBrowserCanGoBackAttrib, NULL, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_READONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CANGOFORWARD", cocoaWebBrowserCanGoForwardAttrib, NULL, NULL, NULL, IUPAF_NO_DEFAULTVALUE|IUPAF_READONLY|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "GOBACK", NULL, cocoaWebBrowserSetBackAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "GOFORWARD", NULL, cocoaWebBrowserSetForwardAttrib, NULL, NULL, IUPAF_WRITEONLY|IUPAF_NO_INHERIT);

  return ic;
}
