/* IupMultiline: Simple Example in C 
   Shows a multiline that ignores the treatment of the DEL key, canceling its effect.
*/

#include <iup_plus.h>

static int mlaction(Ihandle*, int c, char*)
{
  if (c == K_g)
    return IUP_IGNORE;
  else
    return IUP_DEFAULT;
}

int main(int argc, char **argv)
{
  Iup::Open(argc, argv);

  Iup::Text ml;
  ml.SetAttribute("MULTILINE", "YES");
  ml.SetCallback("ACTION", (Icallback)mlaction);
  ml.SetAttribute("EXPAND", "YES");
  ml.SetAttribute("VALUE", "I ignore the \"g\" key!");
  ml.SetAttribute("BORDER", "YES");

  Iup::Dialog dlg(ml);
  dlg.SetAttribute("TITLE", "IupMultiline");
  dlg.SetAttribute("SIZE", "QUARTERxQUARTER");

  dlg.Show();
  Iup::MainLoop();
  Iup::Close();
  return 0;
}
