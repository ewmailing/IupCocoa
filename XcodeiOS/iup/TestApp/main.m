//
//  main.m
//  TestApp
//
//  Created by Eric Wing on 12/15/16.
//  Copyright © 2016 Tecgraf, PUC-Rio, Brazil. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AppDelegate.h"
#include "iup.h"


#include "iup.h"
#include <stddef.h>
#include <stdio.h>

#if __ANDROID__
#include <android/log.h>
void MyPrintf(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	__android_log_vprint(ANDROID_LOG_INFO, "IupTest", fmt, ap);
	va_end(ap);
}
#else
void MyPrintf(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	fprintf(stderr, fmt, ap);
	va_end(ap);
}
#endif
int OnButtonCallback()
{
	MyPrintf("OnButtonCallback()\n");
	
	char temp_string[1024];
	static int button_count = 0;
	snprintf(temp_string, 1024, "Iup Button %d", button_count);
	Ihandle* button = IupButton(temp_string, "");
	
	IupSetCallback(button, "ACTION", (Icallback)OnButtonCallback);
	Ihandle* dialog = IupDialog(button);
	snprintf(temp_string, 1024, "Iup Activity Title %d", button_count);
	
	IupSetStrAttribute(dialog, "TITLE", temp_string);
	
	
	
	IupShow(dialog);
	
	button_count++;
	
	return IUP_DEFAULT;
}

void IupEntry()
{

	Ihandle* button = IupButton("Iup Button", "");
	IupSetCallback(button, "ACTION", (Icallback)OnButtonCallback);
/*
	Ihandle* vb=IupVbox(button, NULL);
	IupSetAttribute(vb, "GAP", "10");
	IupSetAttribute(vb, "MARGIN", "10x10");
	IupSetAttribute(vb, "ALIGNMENT", "ACENTER");
*/
//	Ihandle* dialog = IupDialog(vb);
//	Ihandle* dialog = IupDialog(NULL);
	Ihandle* dialog = IupDialog(button);

	//	IupMap(dialog);
	IupSetAttribute(dialog, "TITLE", "Iup Activity Title");
	
	
	IupShow(dialog);
	
	
//	OnButtonCallback();
}




int main(int argc, char * argv[])
{
	IupOpen(0, NULL);
	IupSetFunction("ENTRY_POINT", (Icallback)IupEntry);
/*
	@autoreleasepool {
	    return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
	}
*/
//	IupEntry();
	IupMainLoop();
}
