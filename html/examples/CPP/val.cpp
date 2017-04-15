/*
  Program to exemplify the IupVal element.
*/
#include <stdlib.h>
#include <stdio.h>

#include <iup_plus.h>


static int mousemove(Ihandle *ih,double a)
{
  Iup::Val val(ih);

  Iup::Control label = (Ihandle*)val.GetUserData("LABEL");

  char buffer[40];
  sprintf(buffer, "VALUE=%.2g", a);
  label.SetString("TITLE", buffer);

  return IUP_DEFAULT;
}

static int button_press(Ihandle *ih,double a)
{
  Iup::Val val(ih);

  Iup::Control label = (Ihandle*)val.GetUserData("LABEL");
  label.SetAttribute("FGCOLOR", "255 0 0");

  mousemove(ih, a);

  return IUP_DEFAULT;
}

static int button_release(Ihandle *ih,double a)
{
  Iup::Val val(ih);

  Iup::Control label = (Ihandle*)val.GetUserData("LABEL");
  label.SetAttribute("FGCOLOR", "0 0 0");

  mousemove(ih, a);

  return IUP_DEFAULT;
}


int main(int argc, char* argv[])
{
  Iup::Open(argc, argv);

  Iup::Val val_v("VERTICAL");
  Iup::Val val_h("HORIZONTAL");
  Iup::Label lbl_v("VALUE=");
  Iup::Label lbl_h("VALUE=");
  lbl_v.SetAttribute("SIZE", "50x");
  lbl_h.SetAttribute("SIZE", "50x");
  val_v.SetAttribute("SHOWTICKS", "5");
  val_v.SetUserData("LABEL", lbl_v.GetHandle());
  val_h.SetUserData("LABEL", lbl_h.GetHandle());

  Iup::Dialog dlg
  (
    Iup::Hbox
    (
      Iup::Hbox
      (
        val_v,
        lbl_v
      ).SetAttributes("ALIGNMENT=ACENTER"),
      Iup::Vbox
      (
        val_h,
        lbl_h
      ).SetAttributes("ALIGNMENT=ACENTER")
    )
  );

  val_v.SetCallback("BUTTON_PRESS_CB", (Icallback)button_press);
  val_v.SetCallback("BUTTON_RELEASE_CB", (Icallback)button_release);
  val_v.SetCallback("MOUSEMOVE_CB", (Icallback)mousemove);

  val_h.SetCallback("BUTTON_PRESS_CB", (Icallback)button_press);
  val_h.SetCallback("BUTTON_RELEASE_CB", (Icallback)button_release);
  val_h.SetCallback("MOUSEMOVE_CB", (Icallback)mousemove);

  dlg.SetAttribute("TITLE", "IupVal");
  dlg.SetAttribute("MARGIN", "10x10");
  dlg.ShowXY(IUP_CENTER, IUP_CENTER);

  Iup::MainLoop();
  Iup::Close();
  return 0;
}
