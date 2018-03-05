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

static void ButtonLabelProgressBar()
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

static void ButtonTextView()
{
	
	//	Ihandle* label = IupLabel("Iup Label");
	
#if 1
	Ihandle* text_view_result = IupMultiLine(NULL);
	//	Ihandle* text_view_result = IupText(NULL);
	//  IupSetAttribute(text_view_result, "SIZE", "500x100");
	IupSetAttribute(text_view_result, "VISIBLECOLUMNS", "40");
	IupSetAttribute(text_view_result, "VISIBLELINES", "10");
	IupSetAttribute(text_view_result, "EXPAND", "HORIZONTAL");
	//	IupSetAttribute(text_view_result, "READONLY", "YES");
	IupSetAttribute(text_view_result, "VALUE", "Welcome to the wonderful world");
	
#endif
	
	Ihandle* button = IupButton("Iup Button", "");
	IupSetCallback(button, "ACTION", (Icallback)OnButtonCallback);
	Ihandle* button2 = IupButton("Iup Button 2", "");
	Ihandle* button3 = IupButton("Iup Button 3", "");
	Ihandle* button4 = IupButton("Iup Button 4", "");
	
	//	Ihandle* vb=IupVbox(text_view_result, button, NULL);
	//	Ihandle* vb=IupVbox(button, text_view_result, NULL);
	//	Ihandle* vb=IupVbox(button, button2, NULL);
	Ihandle* hb1=IupHbox(button, button2, NULL);
	Ihandle* hb2=IupHbox(button3, button4, NULL);
	//	Ihandle* vb=IupVbox(hb1, hb2, NULL);
	Ihandle* vb=IupVbox(hb1, text_view_result, hb2, NULL);
	
	//	IupSetAttribute(vb, "GAP", "10");
	//	IupSetAttribute(vb, "MARGIN", "10x10");
	//	IupSetAttribute(vb, "ALIGNMENT", "ACENTER");
	
	Ihandle* dialog = IupDialog(vb);
	//	IupMap(dialog);
	IupSetAttribute(dialog, "TITLE", "Iup Activity Title");
	//	IupSetAttribute(dialog, "RASTERSIZE", "1024x1920");
	
	
	IupShow(dialog);

}
void IupExitPoint()
{
	IupClose();
}

static void IupEntryPoint()
{
	IupSetFunction("EXIT_CB", (Icallback)IupExitPoint);

//	ButtonLabelProgressBar();
	ButtonTextView();
}


int main(int argc, char * argv[])
{
	IupOpen(0, NULL);
	IupSetFunction("ENTRY_POINT", (Icallback)IupEntryPoint);
	IupMainLoop();
}
