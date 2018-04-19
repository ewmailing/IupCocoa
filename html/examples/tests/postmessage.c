#include <stdlib.h>
#include <stdio.h>
#include "iup.h"
#include "iupcbs.h"

static Ihandle *timer1;
static Ihandle *button;

/* TODO: This callback signature will change when we decide on the final API. */
static int postmessage_cb(Ihandle *ih, char* not_used_char, void *message_data, int not_used_int)
{
	IupLog("DEBUG", "In postmessage_cb\n");
	int is_active = IupGetInt(ih, "ACTIVE");
	IupSetInt(ih, "ACTIVE", !is_active);
	
	/* return value is unused internally */
	return IUP_DEFAULT;
}

static int timer_cb(Ihandle *ih)
{
  void *message_data = NULL;
  IupPostMessage(button, NULL, message_data, 0);
  
  return IUP_DEFAULT;
}

void PostMessageTest(void)
{
  Ihandle *dlg;
  button = IupButton("Button", NULL);
  IupSetCallback(button, "POSTMESSAGE_CB", (Icallback)postmessage_cb);
  dlg = IupDialog(button);
  IupSetAttribute(dlg, "TITLE", "IupPostMessage Test");
  IupSetAttribute(dlg, "SIZE", "200x100");

  IupShow(dlg);

  timer1 = IupTimer();
  IupSetAttribute(timer1, "TIME",  "4000");
  IupSetAttribute(timer1, "RUN",   "YES");
  IupSetCallback(timer1, "ACTION_CB", (Icallback)timer_cb);
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);

  PostMessageTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
