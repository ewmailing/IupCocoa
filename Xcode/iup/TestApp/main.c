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

int main(int argc, const char* argv[])
{

	IupOpen(&argc, &argv);
	Ihandle* label = IupLabel("Hello");

//	Ihandle* dialog = IupDialog(NULL);
	Ihandle* dialog = IupDialog(label);
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
