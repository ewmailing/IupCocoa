//
//  main.m
//  TestApp
//
//  Created by Eric Wing on 12/15/16.
//  Copyright © 2016 Tecgraf, PUC-Rio, Brazil. All rights reserved.
//




#include "iup.h"
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>

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
	snprintf(temp_string, 1024, "Iup ViewController Title %d", button_count);
	
	IupSetStrAttribute(dialog, "TITLE", temp_string);
	
	
	
	IupShow(dialog);
	
	button_count++;
	
	return IUP_DEFAULT;
}

static void IupEntryPoint()
{

	Ihandle* button = IupButton("Iup Button", "");
	IupSetCallback(button, "ACTION", (Icallback)OnButtonCallback);
	
    Ihandle* label = IupLabel("Iup Label");
	IupSetAttribute(label,"FGCOLOR","255 255 255");
	IupSetAttribute(label,"BGCOLOR","120 0 128");

    Ihandle* label2 = IupLabel("Iup Label line one\nline two");

    Ihandle* progressBar0 = IupProgressBar() ;
    IupSetAttribute(progressBar0,"MIN","500.0");
    IupSetAttribute(progressBar0,"MAX","1000.0");
    IupSetAttribute(progressBar0,"VALUE","600.0");
    char* answer = IupGetAttribute(progressBar0, "VALUE");
    printf("Got back value: %s\n", answer);
    
    Ihandle* progressBar1 = IupProgressBar() ;
    IupSetAttribute(progressBar1,"FGCOLOR","255 0 0");
    IupSetAttribute(progressBar1,"BGCOLOR","0 255 255");
    IupSetAttribute(progressBar1,"MARQUEE","YES");
    
    Ihandle* progressBar2 = IupProgressBar() ;
    IupSetAttribute(progressBar2,"MIN","500.0");
    IupSetAttribute(progressBar2,"MAX","1000.0");
    IupSetAttribute(progressBar2,"VALUE","900.0");
    IupSetAttribute(progressBar2,"SIZE","47x28");

    Ihandle* progressBar3 = IupProgressBar() ;
    IupSetAttribute(progressBar3,"MARQUEE","YES");
//    IupSetAttribute(progressBar3,"ORIENTATION","VERTICAL");

    
	Ihandle* vb=IupVbox(button, label, label2, progressBar0, progressBar1, progressBar2, progressBar3, NULL);
	IupSetAttribute(vb, "GAP", "10");
	IupSetAttribute(vb, "MARGIN", "10x10");
	IupSetAttribute(vb, "ALIGNMENT", "ACENTER");

    Ihandle* dialog = IupDialog(vb);
	
	//	IupMap(dialog);
	IupSetAttribute(dialog, "TITLE", "Iup ViewController Title");
	
	IupShow(dialog);
    printf("size = %s\n", IupGetAttribute(progressBar1,"SIZE"));

}




int main(int argc, char * argv[])
{
	IupOpen(0, NULL);
	IupSetFunction("ENTRY_POINT", (Icallback)IupEntryPoint);
	IupMainLoop();
}
