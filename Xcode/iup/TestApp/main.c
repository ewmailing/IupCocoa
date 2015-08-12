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
	IupSetAttribute(dlg, "PARENTDIALOG", "_MAIN_DIALOG_TEST_");

	IupPopup(dlg, IUP_CURRENT, IUP_CURRENT);
	
	char* selected_file = NULL;
	int is_selected = 0;
	if (IupGetInt(dlg, "STATUS") != -1)
	{
  printf("OK\n");
  printf("  VALUE(%s)\n", IupGetAttribute(dlg, "VALUE"));
		selected_file = IupGetAttribute(dlg, "VALUE");
		is_selected = 1;
	}
	else
	{
  printf("CANCEL\n");
	}
	
	
	// must copy VALUE before dialog destroy
	char message[2048];
	if(NULL == selected_file)
	{
		sprintf(message, "File selection was cancelled");
	}
	else
	{
		sprintf(message, "Selected file:\n%s", selected_file);
		printf("  VALUE(%s)\n", selected_file);
		
	}
	
	IupDestroy(dlg);
	
	
	dlg = IupMessageDlg();
	IupSetAttribute(dlg, "DIALOGTYPE", "INFORMATION");
	IupSetAttribute(dlg, "TITLE", "IupMessageDlg Test");
	if(is_selected)
	{
//		IupSetAttribute(dlg, "BUTTONS", "OKCANCEL");
		IupSetAttribute(dlg, "BUTTONS", "YESNO");
	}
	else
	{
		IupSetAttribute(dlg, "BUTTONS", "OK");

	}

	IupSetStrAttribute(dlg, "VALUE", message);
//	IupSetCallback(dlg, "HELP_CB", (Icallback)help_cb);
	
	IupPopup(dlg, IUP_CURRENT, IUP_CURRENT);
	
	printf("BUTTONRESPONSE(%s)\n", IupGetAttribute(dlg, "BUTTONRESPONSE"));
	IupDestroy(dlg);

}

static int button_cb(Ihandle* ih)
{
	FileDialog();
	
	return IUP_DEFAULT;
	
}


void TextTest()
{
	Ihandle* text_obj = IupText(NULL);

	
	
}



int main(int argc, const char* argv[])
{

	IupOpen(&argc, &argv);
//	Ihandle* label = IupLabel("Hello");
	Ihandle* button = IupButton("Hello", NULL);
	IupSetCallback(button, "ACTION", (Icallback)button_cb);

//	Ihandle* text_obj = IupText(NULL);
//	Ihandle* list_obj = IupList(NULL);

//	Ihandle* dialog = IupDialog(NULL);
//	Ihandle* dialog = IupDialog(label);
	Ihandle* dialog = IupDialog(button);
//	Ihandle* dialog = IupDialog(text_obj);
//	Ihandle* dialog = IupDialog(list_obj);
	IupSetHandle("_MAIN_DIALOG_TEST_", dialog);

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
