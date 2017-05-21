/*IupZbox Example in C 
An application of a zbox could be a program requesting several entries 
from the user according to a previous selection. 
In this example, a list of possible layouts, each one consisting of an element, 
is presented, and according to the selected option the dialog below the list is changed. */

#include <iup_plus.h>

static int list_cb (Ihandle* ih, char *t, int, int selected)
{
  if (selected == 1)
  {
    Iup::List list(ih);
    Iup::Control zbox = list.GetDialogChild("zbox");
    zbox.SetAttribute("VALUE", t);
  }
  
  return IUP_DEFAULT;
}

int main(int argc, char **argv)
{
  Iup::Open(argc, argv);

  Iup::Frame frame(Iup::List().SetAttributes("DROPDOWN=YES, 1=Test, 2=XXX, VALUE=1"));
  frame.SetAttribute("TITLE", "List");

  Iup::Text text;

  text.SetAttributes("EXPAND = YES, VALUE = \"Enter your text here\"");
  
  /* Creates a label */
  Iup::Label lbl("This element is a label");

  /* Creatas a button */
  Iup::Button btn("This button does nothing");

  /* Creates handles for manipulating the zbox VALUE */
  Iup::SetHandle("frame", frame);
  Iup::SetHandle("text", text);
  Iup::SetHandle("lbl", lbl);
  Iup::SetHandle("btn", btn);
	
  /* Creates zbox with four elements */
  Iup::Zbox zbox(frame, text, lbl, btn);

  /* Sets zbox alignment */
  zbox.SetAttribute("ALIGNMENT", "ACENTER");
  zbox.SetAttribute("VALUE", "text");
  zbox.SetAttribute("NAME", "ZBOX");

  Iup::List list;
  Iup::Hbox hbox(list);

    /* Creates frame */
  Iup::Frame frm(hbox);

  /* Creates dialog */
  Iup::Dialog dlg
  (
    Iup::Vbox
    (
      frm,
      zbox
    )
  );

  list.SetAttributes("1 = frame, 2 = text, 3 = lbl, 4 = btn, VALUE=2");
  frm.SetAttribute("TITLE", "Select an element");
  dlg.SetAttributes("MARGIN=10x10, GAP=10, TITLE = \"IupZbox Example\"");
  list.SetCallback("ACTION", (Icallback)list_cb);

  dlg.ShowXY(IUP_CENTER, IUP_CENTER);

  Iup::MainLoop();

  Iup::Close();

  return 0;
}
