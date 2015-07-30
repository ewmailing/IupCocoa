//
//  main.m
//  TestApp
//
//  Created by Eric Wing on 7/22/15.
//  Copyright (c) 2015 Tecgraf, PUC-Rio, Brazil. All rights reserved.
//

#include "iup.h"
#include <stdlib.h>
#include <stdio.h>

static Ihandle* timer1 = NULL;

static int timer_cb(Ihandle* ih)
{
	if(ih == timer1)
	{
		int elapsed_time = IupGetInt(timer1, "ELAPSEDTIME");
		printf("timer 1 elapsed: %d\n", elapsed_time);
	}
	return IUP_DEFAULT;
}




void TimerTest()
{
	timer1 = IupTimer();
	IupSetAttribute(timer1, "TIME",  "1000");
	IupSetAttribute(timer1, "RUN",   "YES");
	IupSetCallback(timer1, "ACTION_CB", (Icallback)timer_cb);
	
}


void FileDialog()
{
	Ihandle *dlg = IupFileDlg();
	
	IupSetAttribute(dlg, "DIALOGTYPE", "OPEN");
	IupSetAttribute(dlg, "TITLE", "IupFileDlg Test");
	IupSetAttributes(dlg, "FILTER = \"*.bmp\", FILTERINFO = \"Bitmap Files\"");
//	IupSetCallback(dlg, "HELP_CB", (Icallback)help_cb);
	
	IupPopup(dlg, IUP_CURRENT, IUP_CURRENT);
	
	if (IupGetInt(dlg, "STATUS") != -1)
	{
  printf("OK\n");
  printf("  VALUE(%s)\n", IupGetAttribute(dlg, "VALUE"));
	}
	else
	{
  printf("CANCEL\n");
	}
	IupDestroy(dlg);
}

static int button_cb(Ihandle* ih)
{
	FileDialog();
	
	return IUP_DEFAULT;
	
}


int main(int argc, const char* argv[])
{

	IupOpen(&argc, &argv);
	Ihandle* label = IupLabel("Hello");
	Ihandle* button = IupButton("Hello", NULL);
	IupSetCallback(button, "ACTION", (Icallback)button_cb);

//	Ihandle* dialog = IupDialog(NULL);
//	Ihandle* dialog = IupDialog(label);
	Ihandle* dialog = IupDialog(button);
	IupSetAttribute(dialog, "TITLE", "Test Window");

/*
	if (IupGetInt(NULL, "UTF8MODE"))
		IupSetAttribute(label, "TITLE", "Text Labels (√ß√£√µ√°√≥√©)");
	else
		IupSetAttribute(label, "TITLE", "Text Labels (Á„ı·ÛÈ)");
*/
	
//	TimerTest();
	IupShowXY(dialog, IUP_CENTER, IUP_CENTER);
	
	IupMainLoop();
	IupDestroy(dialog);

//	IupDestroy(timer1);

	IupClose();
	
	
	return 0;
}
