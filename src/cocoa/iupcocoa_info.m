/** \file
 * \brief MAC OS System Information
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* This module should depend only on IUP core headers and
 Mac OS Carbon system headers. */

#import <Cocoa/Cocoa.h>
//#include <Carbon/Carbon.h>
// For computer name
#include <SystemConfiguration/SystemConfiguration.h>

#include <sys/utsname.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <sys/stat.h>
#include <langinfo.h>

//#include <gtk/gtk.h>

#include "iup.h"

#include "iup_str.h"
#include "iup_drv.h"
#include "iup_drvinfo.h"

#define IUP_MAC_ERROR -1

#if 0
static void iupMacStrToUniChar(const char* buffer, UniChar* outBuf, long length, long* outLen)
{
	CFStringRef stringRef = CFStringCreateWithCString(NULL, buffer, kCFStringEncodingUTF8);
	
	CFStringGetCharacters (stringRef, CFRangeMake(0, CFStringGetLength(stringRef)), outBuf);
	*outLen = (long) CFStringGetLength (stringRef);
	
	CFRelease(stringRef);
}
#endif

static int iMacIsFolder(const char* name)
{
	FSRef refName;
	Boolean isFolder;
	
	if(FSPathMakeRef((const UInt8*)name, &refName, &isFolder) != noErr)
		return IUP_MAC_ERROR;
	
	return isFolder;
}

int iupdrvGetWindowDecor(void* wnd, int *border, int *caption)
{
#if 0
	Rect rect;
	CGRect cg;
	int minX, minY;
	
	CGDirectDisplayID mainDisplayID = CGMainDisplayID();
#ifdef OLD_MAC_INFO
	GDHandle hGDev;
	DMGetGDeviceByDisplayID((DisplayIDType)mainDisplayID, &hGDev, false);
	GetAvailableWindowPositioningBounds(hGDev, &rect);
#else
	HIWindowGetAvailablePositioningBounds(mainDisplayID, kHICoordSpaceScreenPixel, &rect);
#endif
	
	cg = CGRectMake(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	
	minX = (int)CGRectGetMinX(cg);
	minY = (int)CGRectGetMinY(cg);
	
	if (minX >= 0 && minY >= 0 && (minY >= minX))
	{
		*border  = minX;
		*caption = minY - *border;
  
		return 1;
	}
	
	*border  = 0;
	*caption = 0;
	
	return 0;
#endif
	return 0;

}

void iupdrvAddScreenOffset(int *x, int *y, int add)
{
	/* ?????? */
}

// How is this different than iupdrvGetFullSize? Is this supposed to subtract the menu and dock?
void iupdrvGetScreenSize(int *width, int *height)
{
	NSRect screen_rect = [[NSScreen mainScreen] visibleFrame];
	
	// Points vs. Pixels in Cocoa
	//  int w_size = CGDisplayPixelsWide(kCGDirectMainDisplay);
	//  int h_size = CGDisplayPixelsHigh(kCGDirectMainDisplay);
	if (width) *width = screen_rect.size.width;
	if (height) *height = screen_rect.size.height;
	
}

void iupdrvGetFullSize(int *width, int *height)
{
	/*
  CGRect rect;
	 
  rect = CGDisplayBounds(kCGDirectMainDisplay);
	 
  *width  = (int)CGRectGetWidth(rect);
  *height = (int)CGRectGetHeight(rect);
	 */
	NSRect screen_rect = [[NSScreen mainScreen] frame];
	
	// Points vs. Pixels in Cocoa
	//  int w_size = CGDisplayPixelsWide(kCGDirectMainDisplay);
	//  int h_size = CGDisplayPixelsHigh(kCGDirectMainDisplay);
	if (width) *width = screen_rect.size.width;
	if (height) *height = screen_rect.size.height;
}

int iupdrvGetScreenDepth(void)
{
	return CGDisplayBitsPerPixel(kCGDirectMainDisplay);  /* Deprecated in Mac OS X v10.6 */
}

// I think this is not going to work on Cocoa. Apple does everything in their power to hide this for retina.
double iupdrvGetScreenDpi(void)
{
	CGRect rect = CGDisplayBounds(kCGDirectMainDisplay);
	int height = (int)CGRectGetHeight(rect);   /* pixels */
	CGSize size = CGDisplayScreenSize(kCGDirectMainDisplay);  /* millimeters */
	return ((float)height / size.height) * 25.4f;  /* mm to inch */
}

void iupdrvGetCursorPos(int *x, int *y)
{
#if 0
	CGPoint point;
#ifdef OLD_MAC_INFO
	Point pnt;
	GetMouse(&pnt);
	point = CGPointMake(pnt.h, pnt.v);
#else
	HIGetMousePosition(kHICoordSpaceScreenPixel, NULL, &point);
#endif
	
	*x = (int)point.x;
	*y = (int)point.y;
#endif
}

void iupdrvGetKeyState(char* key)
{
#if 0
	if (GetCurrentEventKeyModifiers() & shiftKey)
		key[0] = 'S';
	else
		key[0] = ' ';
	if (GetCurrentEventKeyModifiers() & controlKey)
		key[1] = 'C';
	else
		key[1] = ' ';
	if (GetCurrentEventKeyModifiers() & optionKey)
		key[2] = 'A';
	else
		key[2] = ' ';
	if (GetCurrentEventKeyModifiers() & cmdKey)
		key[3] = 'Y';
	else
		key[3] = ' ';
	
	key[4] = 0;
#endif
	
}

char *iupdrvGetSystemName(void)
{

	// Ugh. This is too much work to maintain because the name changes every year and there still isn't a Mac port of iup after a decade.
	// Note: Gestalt is deprecated.
	// NSProcessInfo only returns numbers.
	// uname returns kernel info
	// Reading the plist won't work for sandboxing (Mac App Store requirement)
	/*
	 NSDictionary *version = [NSDictionary dictionaryWithContentsOfFile:@"/System/Library/CoreServices/SystemVersion.plist"];
	 NSString *productVersion = [version objectForKey:@"ProductVersion"];
	 NSLog (@"productVersion =========== %@", productVersion);
	 */
	// Just give up and return "OS X", or should it be "OSX"?
	
	return "OS X";
	
	
#if 0
	SInt32 systemVersion;
	
	if (Gestalt(gestaltSystemVersion, &systemVersion) == noErr)
	{
		if (systemVersion >= 0x1060)
			return "Snow Leopard";
		else if (systemVersion >= 0x1050)
			return "Leopard";
		else if (systemVersion >= 0x1040)
			return "Tiger";
		else if (systemVersion >= 0x1030)
			return "Panther";
		else if (systemVersion >= 0x1020)
			return "Jaguar";
		else if (systemVersion >= 0x1010)
			return "Puma";
		else if (systemVersion >= 0x1010)
			return "Cheetah";
	}
	
	return "MacOS";
#endif
	
}

char *iupdrvGetSystemVersion(void)
{
	
#if 0
	char* str = iupStrGetMemory(100);
	SInt32 systemVersion, versionMajor, versionMinor, versionBugFix, systemArchitecture;
	
	if (Gestalt(gestaltSystemVersion, &systemVersion) != noErr)
		return NULL;
	
	if (systemVersion < 0x1040)
	{
		/* Major, Minor, Bug fix */
		sprintf(str, "%ld.%ld.%ld", (((long)systemVersion & 0xF000) >> 12) * 10 + (((long)systemVersion & 0x0F00) >> 8),
				(((long)systemVersion & 0x00F0) >> 4), ((long)systemVersion & 0x000F));
	}
	else  /* MAC_OS_X_VERSION_10_4 or later */
	{
		Gestalt(gestaltSystemVersionMajor,  &versionMajor);
		Gestalt(gestaltSystemVersionMinor,  &versionMinor);
		Gestalt(gestaltSystemVersionBugFix, &versionBugFix);
		
		sprintf(str, "%ld.%ld.%ld", (long)versionMajor, (long)versionMinor, (long)versionBugFix);
	}
	
	if (Gestalt(gestaltSysArchitecture, &systemArchitecture) == noErr)
	{
		if (systemArchitecture == gestalt68k)
			strcat(str, " (Motorola 68k)");
		else if (systemArchitecture == gestaltPowerPC)
			strcat(str, " (Power PC)");
		else /* gestaltIntel */
			strcat(str, " (Intel)");
	}
	
	return str;
#else
	
	NSString* version_string = nil;
	version_string = [[NSProcessInfo processInfo] operatingSystemVersionString];
	
	const char* c_str = [version_string UTF8String];
	// don't use [version_string length]...counts characters, not bytes
	size_t str_len = strlen(c_str);
	
	char* iup_str = iupStrGetMemory((int)str_len);
	strlcpy(iup_str, c_str, str_len+1);
	
	return iup_str;
#endif

}

/*
Drat. hostName blocks so if the network is down, this function will hang.

Claims are CSCopeMachineName() does not block, but it is deprecated.
 
SCDynamicStoreCopyLocalHostName sounds like it might work.
// https://lists.apple.com/archives/cocoa-dev/2009/Sep/msg00680.html
  If you're just looking for the mDNS Local Hostname, it's MUCH more efficient to use SCDynamicStoreCopyLocalHostName, which is just a Mach message over to configd running on the local machine. No network delays.
 It's peer function, SCDynamicStoreCopyComputerName might be even better.
Both require the SystemConfiguration framework


gethostname may also work
// https://lists.apple.com/archives/cocoa-dev/2009/Sep/msg00616.html
 char hostname[_POSIX_HOST_NAME_MAX + 1];
 gethostname(hostname, _POSIX_HOST_NAME_MAX);
 name = [NSString stringWithCString:hostname encoding:NSUTF8StringEncoding];
 */
char *iupdrvGetComputerName(void)
{
/*
	char* str = iupStrGetMemory(50);
	CFStringRef computer_name = CSCopyMachineName(); // suspect: should CFRelease?
	CFStringGetCString(computer_name, str, 50, kCFStringEncodingUTF8);
	return str;
*/
	
#if 0

	// hostName is considered good enough for Bonjour names so it is good enough for this
	NSString* host_name = nil;
	host_name = [[NSProcessInfo processInfo] hostName];
	
	const char* c_str = [host_name UTF8String];
	// don't use [version_string length]...counts characters, not bytes
	size_t str_len = strlen(c_str);
	
	char* iup_str = iupStrGetMemory((int)str_len);
	strlcpy(iup_str, c_str, str_len+1);
	
	return iup_str;
#else
	
	// Returns NULL/nil if no computer name set, or error occurred. OSX 10.1+
	NSString* computer_name = [(NSString *)SCDynamicStoreCopyComputerName(NULL, NULL) autorelease];
	const char* c_str = [computer_name UTF8String];
	// don't use [version_string length]...counts characters, not bytes
	size_t str_len = strlen(c_str);
	
	char* iup_str = iupStrGetMemory((int)str_len);
	strlcpy(iup_str, c_str, str_len+1);
	
	return iup_str;
	
#endif
	
}


char *iupdrvGetUserName(void)
{
#if 0
	char* str = iupStrGetMemory(50);
	CFStringRef userName = CSCopyUserName(TRUE);  /* TRUE = login name   FALSE = user name */
	CFStringGetCString(userName, str, 50, kCFStringEncodingUTF8);
	return str;
#else
	
	NSString* user_name = nil;
	// Which one should we use?
//	user_name = NSUserName();
	user_name = NSFullUserName();

	const char* c_str = [user_name UTF8String];
	// don't use [version_string length]...counts characters, not bytes
	size_t str_len = strlen(c_str);
	
	char* iup_str = iupStrGetMemory((int)str_len);
	strlcpy(iup_str, c_str, str_len+1);
	
	return iup_str;
#endif


	
}

int iupdrvGetPreferencePath(char *filename, int str_len, const char* app_name)
{
	NSArray* support_paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
	if([support_paths count] > 0)
	{
		BOOL is_dir = NO;
		NSError* the_error = nil;
		
		NSBundle* main_bundle = [NSBundle mainBundle];
		// Use the Info.plist to get: com.organization.appname so we can create/use a unique subdirectory in ~/Library/Application Support
		// Note Mac App Sandboxing will give a per-app sandbox with its own private ~/Library/Application Support so everything still works.
		NSString* bundle_name = [[main_bundle infoDictionary] objectForKey:@"CFBundleIdentifier"];
		NSString* ns_path = [[support_paths objectAtIndex:0] stringByAppendingPathComponent:bundle_name];

		if(![[NSFileManager defaultManager] fileExistsAtPath:ns_path isDirectory:&is_dir]
		   && is_dir == NO
		   )
		{
			BOOL did_succeed = [[NSFileManager defaultManager] createDirectoryAtPath:ns_path withIntermediateDirectories:YES attributes:nil error:&the_error];
			if(NO == did_succeed)
			{
				NSLog(@"Create preference directory error: %@", the_error);
				filename[0] = '\0';
				return 0;
			}
		}
		
		const char* c_path = [ns_path fileSystemRepresentation];
		
		if(NULL != c_path)
		{
			size_t num = strlcpy(filename, c_path, str_len);
			if(num >= str_len)
			{
				filename[0] = '\0';
				return 0;
			}
			num = strlcat(filename, "/", str_len);
			if(num >= str_len)
			{
				filename[0] = '\0';
				return 0;
			}
			return 1;
		}
		else
		{
			filename[0] = '\0';
			return 0;
		}
	}

	filename[0] = '\0';
	return 0;
}


char* iupdrvLocaleInfo(void)
{
	return iupStrReturnStr(nl_langinfo(CODESET));
}
