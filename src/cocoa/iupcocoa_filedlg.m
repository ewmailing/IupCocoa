/** \file
 * \brief IupFileDlg pre-defined dialog
 *
 * See Copyright Notice in "iup.h"
 */

#include <Cocoa/Cocoa.h>

#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <stdio.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_drvinfo.h"
#include "iup_dialog.h"
#include "iup_strmessage.h"
#include "iup_array.h"
#include "iup_drvinfo.h"

#include "iupcocoa_drv.h"

#define MAX_FILENAME_SIZE PATH_MAX
#define IUP_PREVIEWCANVAS 3000

enum {IUP_DIALOGOPEN, IUP_DIALOGSAVE, IUP_DIALOGDIR};
                           
/*
static void macFileDlgGetFolder(Ihandle *ih)
{
  InativeHandle* parent = iupDialogGetNativeParent(ih);
//  BROWSEINFO browseinfo;
  char buffer[PATH_MAX];

  NSOpenPanel *op = [NSOpenPanel openPanel];
  [op setCanChooseFiles:NO];
  [op setCanChooseDirectories:YES];
  [op setPrompt:@"Choose folder"];
  [op setTitle: [NSString stringWithUTF8String:iupAttribGet(ih, "TITLE")]];

  if([op runModal] == NSOKButton) 
  {           
    strcpy(buffer,[[op filename] UTF8String]);                       
    iupAttribStoreStr(ih, "VALUE", buffer);
    iupAttribSetStr(ih, "STATUS", "0");	
  }  
  else 
  {
    iupAttribSetStr(ih, "VALUE", NULL);
    iupAttribSetStr(ih, "STATUS", "-1");	
  }  

  iupAttribSetStr(ih, "FILEEXIST", NULL);
  iupAttribSetStr(ih, "FILTERUSED", NULL);
}
*/
/*
- (BOOL)panel:(id)sender shouldShowFilename:(NSString*)filename
{    
	return YES;
}
 */


// FIXME: NOOVERWRITEPROMPT This is going to require a delegate
// FIXME: NOCHANGEDIR I don't know to support this. Apple manages this themselves.
// TODO: FILTERUSED FILTERINFO
static int cocoaFileDlgPopup(Ihandle *ih, int x, int y)
{

//  InativeHandle* parent = iupDialogGetNativeParent(ih);
  char* value;
  int dialogtype;
	NSInteger ret_val;
	
	// NSSavePanel is the base class for both save and open
	NSSavePanel* file_panel = nil;
//	NSSavePanel* save_panel = nil;
//	NSOpenPanel* open_panel = nil;

	NSMutableArray* extention_array = nil;

  iupAttribSetInt(ih, "_IUPDLG_X", x);   /* used in iupDialogUpdatePosition */
  iupAttribSetInt(ih, "_IUPDLG_Y", y);

	// TODO: What does "DIR" mean in Cocoa???
	// ???  [open_panel setCanChooseDirectories:YES];

  value = iupAttribGetStr(ih, "DIALOGTYPE");
  if (iupStrEqualNoCase(value, "SAVE"))
  {
    dialogtype = IUP_DIALOGSAVE;
  }
  else if (iupStrEqualNoCase(value, "DIR"))
  {
	dialogtype = IUP_DIALOGDIR;
	NSOpenPanel* open_panel = [NSOpenPanel openPanel];
	  [open_panel setCanChooseDirectories:YES];
	  // ???
	  [open_panel setCanChooseFiles:NO];

	  file_panel = open_panel;
	  

  }
  else
  {
    dialogtype = IUP_DIALOGOPEN;
	  NSOpenPanel* open_panel = [NSOpenPanel openPanel];
	  file_panel = open_panel;

  }
	
	
  value = iupAttribGet(ih, "EXTFILTER");
  if (value)
  {
	NSArray *arr = [[NSString stringWithUTF8String:value] componentsSeparatedByCharactersInSet:
		[NSCharacterSet characterSetWithCharactersInString:@"|;"]];  
	extention_array = [NSMutableArray arrayWithCapacity:[arr count]];
	for(NSString *str in arr)
	{
	  if([str hasPrefix:@"*."]){
		[extention_array addObject:[str substringFromIndex:2]];
	  }
    }
  }
  else 
  {
    value = iupAttribGet(ih, "FILTER");
    if (value)
    {
		NSArray *arr = [[NSString stringWithUTF8String:value] componentsSeparatedByCharactersInSet:
			[NSCharacterSet characterSetWithCharactersInString:@"|;"]];  
		extention_array = [NSMutableArray arrayWithCapacity:[arr count]];
		for(NSString *str in arr)
		{
		  if([str hasPrefix:@"*."]){
			[extention_array addObject:[str substringFromIndex:2]];
		  }
	    }
    }
  }

//  openfilename.lpstrFile = (char*)malloc(MAX_FILENAME_SIZE+1);
  value = iupAttribGet(ih, "FILE");
  if(value && 0!=*value)
  {
	[file_panel setNameFieldStringValue:[NSString stringWithUTF8String:value]];
  }

	value = iupAttribGet(ih, "DIRECTORY");
	if(value && 0!=*value)
	{
		NSString* ns_string = [NSString stringWithUTF8String:value];
		NSURL* ns_url = [NSURL URLWithString:ns_string];
		[file_panel setDirectoryURL:ns_url];
	}

	value = iupAttribGet(ih, "TITLE");
	if(value && 0!=*value)
	{
		NSString* ns_string = [NSString stringWithUTF8String:value];
		[file_panel setTitle:ns_string];
	}

	

	if(iupAttribGetBoolean(ih, "SHOWHIDDEN"))
	{
		[file_panel setShowsHiddenFiles:YES];

	}

  value = iupAttribGet(ih, "ALLOWNEW");
  if(value)
  {
	  
	  // Should we prevent this for openPanel???
	  int allow_new = iupAttribGetBoolean(ih, "ALLOWNEW");
	  [file_panel setCanCreateDirectories:allow_new];

	  
  }

  if (iupAttribGetBoolean(ih, "MULTIPLEFILES"))
  {
	  // only valid for NSOpenPanel
	  if(iupStrEqualNoCase(value, "SAVE"))
	  {
		  
	  }
	  else
	  {
		  [(NSOpenPanel*)file_panel setAllowsMultipleSelection:YES];
		  
	  }

  }
	
	ret_val = [file_panel runModal];

	
	if(ret_val == NSModalResponseOK)
	{
		
		// Slightly different things for save vs open, so let's split them up
		if(iupStrEqualNoCase(value, "SAVE"))
		{
			NSURL* ns_url = [file_panel URL];

			// For STATUS, we must return 1 for a new file, 0 for an existing file.
			// A delegate callback might allow us to handle this more directly.
			if([[NSFileManager defaultManager] fileExistsAtPath:[ns_url path]])
			{
				iupAttribSetInt(ih, "STATUS", 0);
				// TODO: maybe not set DIALOGTYPE=DIR or MULTIPLEFILES=YES
				iupAttribSetInt(ih, "FILEEXIST", 1);

				
			}
			else
			{
				iupAttribSetInt(ih, "STATUS", 1);
				// TODO: maybe not set DIALOGTYPE=DIR or MULTIPLEFILES=YES
				iupAttribSetInt(ih, "FILEEXIST", 0);
			}
			
			iupAttribSetStr(ih, "VALUE", [[[file_panel URL] path] UTF8String]);
			
		}
		else
		{
			if(iupAttribGetBoolean(ih, "MULTIPLEFILES") && !iupStrEqualNoCase(value, "SAVE"))
			{
				NSArray* array_of_urls = [(NSOpenPanel*)file_panel URLs];
				
				 
				NSMutableArray* array_of_strings = [NSMutableArray arrayWithCapacity:[array_of_urls count]];
				
				// TODO: implement MULTIVALUEid
				for(NSURL* a_url in array_of_urls)
				{
					[array_of_strings addObject:[a_url path]];
				}
				NSString* joined_path = [array_of_strings componentsJoinedByString:@"|"];
				joined_path = [joined_path stringByAppendingString:@"|"];

				// Should this be fileSystemRepresentation? Not sure it will work with the | separators.
				iupAttribSetStr(ih, "VALUE", [joined_path UTF8String]);
				iupAttribSetInt(ih, "MULTIVALUECOUNT", (int)[array_of_urls count]);
				
			}
			else
			{
				// Not using fileSystemRepresentation to be consistent with above
				iupAttribSetStr(ih, "VALUE", [[[file_panel URL] path] UTF8String]);
				
			}
			
			
			// TODO: FILTERUSED
			iupAttribSetStr(ih, "FILTERUSED", NULL);

		}
	}
	else // user cancelled
	{
		iupAttribSetStr(ih, "VALUE", NULL);
		iupAttribSetInt(ih, "STATUS", -1);
	}
	

	
  return IUP_NOERROR;

}

void iupdrvFileDlgInitClass(Iclass* ic)
{
  ic->DlgPopup = cocoaFileDlgPopup;

  iupClassRegisterAttribute(ic, "EXTFILTER", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FILTERINFO", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FILTERUSED", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MULTIPLEFILES", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
}
