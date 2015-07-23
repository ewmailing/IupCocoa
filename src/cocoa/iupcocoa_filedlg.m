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

#define MAX_FILENAME_SIZE 65000
#define IUP_PREVIEWCANVAS 3000

enum {IUP_DIALOGOPEN, IUP_DIALOGSAVE, IUP_DIALOGDIR};
                           

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

/*
- (BOOL)panel:(id)sender shouldShowFilename:(NSString*)filename
{    
	return YES;
}
 */


static int macFileDlgPopup(Ihandle *ih, int x, int y)
{
#if 0
  InativeHandle* parent = iupDialogGetNativeParent(ih);
  char *value;    
  int dialogtype;       
  NSPanel *panel;    
  NSMutableArray *extArr = nil;

  iupAttribSetInt(ih, "_IUPDLG_X", x);   /* used in iupDialogUpdatePosition */
  iupAttribSetInt(ih, "_IUPDLG_Y", y);

  value = iupAttribGetStr(ih, "DIALOGTYPE");
  if (iupStrEqualNoCase(value, "SAVE"))
    dialogtype = IUP_DIALOGSAVE;
  else if (iupStrEqualNoCase(value, "DIR"))
    dialogtype = IUP_DIALOGDIR;
  else
    dialogtype = IUP_DIALOGOPEN;

  if (dialogtype == IUP_DIALOGDIR)
  {
    macFileDlgGetFolder(ih);
    return IUP_NOERROR;
  }

  if (dialogtype == IUP_DIALOGOPEN)
	panel = [NSOpenPanel openPanel];
  else
	panel = [NSSavePanel savePanel];

  value = iupAttribGet(ih, "EXTFILTER");
  if (value)
  {
	NSArray *arr = [[NSString stringWithUTF8String:value] componentsSeparatedByCharactersInSet:
		[NSCharacterSet characterSetWithCharactersInString:@"|;"]];  
	extArr = [NSMutableArray arrayWithCapacity:[arr count]];
	for(NSString *str in arr)
	{
	  if([str hasPrefix:@"*."]){
		[extArr addObject:[str substringFromIndex:2]];
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
		extArr = [NSMutableArray arrayWithCapacity:[arr count]];
		for(NSString *str in arr)
		{
		  if([str hasPrefix:@"*."]){
			[extArr addObject:[str substringFromIndex:2]];
		  }
	    }
    }
  }

  openfilename.lpstrFile = (char*)malloc(MAX_FILENAME_SIZE+1);
  value = iupAttribGet(ih, "FILE");
  if (value)
  {
	[panel setNameFieldStringValue:[NSString value]];
  }

  [panel setDirectory:[NSString iupAttribGet(ih, "DIRECTORY")]];  

  [panel setTitle:[NSString stringWithUTF8String:iupAttribGet(ih, "TITLE")]];
  if (iupAttribGetBoolean(ih, "SHOWHIDDEN"))
    [panel setShowsHiddenFiles:YES];

  value = iupAttribGet(ih, "ALLOWNEW");
  if (!value)
  {
    if (dialogtype == IUP_DIALOGSAVE)
      value = "YES";
    else
      value = "NO";
  }
  if (iupStrBoolean(value))
	[panel setCanCreateDirectories:YES];

  if (iupAttribGetBoolean(ih, "NOCHANGEDIR"))
    [panel setCanChooseDirectories:NO]

  if (iupAttribGetBoolean(ih, "MULTIPLEFILES"))
    [panel setAllowsMultipleSelection:YES]

  if (dialogtype == IUP_DIALOGOPEN)
    result = [panel runModalForTypes:extArr];
  else
    result = [panel runModal];

  if (result)
  {
    if (iupAttribGetBoolean(ih, "MULTIPLEFILES"))
    {            
	  NSMutableArray *muArray = [NSMutableArray arrayWithCapacity:[[panel URLs] count]];
	  for(NSURL *url in [panel URLs])
	  {
		[muArray addObject:[url path]];  
	  }              
      NSString *path = [muArray componentsJoinedByString:@"|"];  
  	  path = [path stringByAppendingString:@"|"];
      iupAttribStoreStr(ih, "VALUE", [path UTF8String]);      

      iupAttribSetStr(ih, "STATUS", "0");
      iupAttribSetStr(ih, "FILEEXIST", NULL);
    }
    else
    {
      if (iupdrvIsFile([[panel filename] UTF8String]))  /* check if file exists */
      {
        iupAttribSetStr(ih, "FILEEXIST", "YES");
        iupAttribSetStr(ih, "STATUS", "0");
      }
      else
      {
        iupAttribSetStr(ih, "FILEEXIST", "NO");
        iupAttribSetStr(ih, "STATUS", "1");
      }
      iupAttribStoreStr(ih, "VALUE", [[panel filename] UTF8String]);
    }

    iupAttribSetInt(ih, "FILTERUSED", NULL);
  }
  else
  {
    iupAttribSetStr(ih, "FILTERUSED", NULL);
    iupAttribSetStr(ih, "VALUE", NULL);
    iupAttribSetStr(ih, "FILEEXIST", NULL);
    iupAttribSetStr(ih, "STATUS", "-1");
  }
#endif
	
  return IUP_NOERROR;

}

void iupdrvFileDlgInitClass(Iclass* ic)
{
  ic->DlgPopup = macFileDlgPopup;

  iupClassRegisterAttribute(ic, "EXTFILTER", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FILTERINFO", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "FILTERUSED", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "MULTIPLEFILES", NULL, NULL, NULL, NULL, IUPAF_NO_INHERIT);
}
