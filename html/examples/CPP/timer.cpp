/* IupTimer Example in C */

#include <stdio.h>
#include <iup_plus.h>

static int timer1_cb(Ihandle*)
{
  printf("timer 1 called\n");
  return IUP_DEFAULT;
}

static int timer2_cb(Ihandle*)
{
  printf("timer 2 called - close\n");
  return IUP_CLOSE;
}

int main(int argc, char **argv)
{
  Iup::Open(argc, argv);

  /* Creating main dialog */
  Iup::Dialog dialog(Iup::Vbox(Iup::Label("Timer example")));
  dialog.SetAttribute("TITLE", "timer example");
  dialog.SetAttribute("SIZE", "200x200");
  dialog.ShowXY(IUP_CENTER, IUP_CENTER);

  Iup::Timer timer1;
  Iup::Timer timer2;

  timer1.SetAttribute("TIME", "1000");
  timer1.SetAttribute("RUN", "YES");
  timer1.SetCallback("ACTION_CB", timer1_cb);

  timer2.SetAttribute("TIME", "4000");
  timer2.SetAttribute("RUN", "YES");
  timer2.SetCallback("ACTION_CB", timer2_cb);

  Iup::MainLoop();

  /* Timers are NOT automatically destroyed, must be manually done */
  timer1.Destroy();
  timer2.Destroy();

  Iup::Close();

  return 0;
}
