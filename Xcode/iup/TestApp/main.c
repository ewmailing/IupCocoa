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
	Ihandle* dialog = IupDialog(NULL);
	TimerTest();
	IupShowXY(dialog, IUP_CENTER, IUP_CENTER);
	
	IupMainLoop();
	IupClose();
	
	IupDestroy(timer1);
	
	return 0;
}
