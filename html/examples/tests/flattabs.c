#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "iup.h"
#include "iupkey.h"


static Ihandle* load_image_LogoTecgraf(void)
{
  unsigned char imgdata[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 108, 120, 143, 125, 132, 148, 178, 173, 133, 149, 178, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 110, 130, 48, 130, 147, 177, 254, 124, 139, 167, 254, 131, 147, 176, 137, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 115, 128, 153, 134, 142, 159, 191, 194, 47, 52, 61, 110, 114, 128, 154, 222, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 143, 172, 192, 140, 156, 188, 99, 65, 69, 76, 16, 97, 109, 131, 251, 129, 144, 172, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 131, 147, 175, 232, 140, 157, 188, 43, 0, 0, 0, 0, 100, 112, 134, 211, 126, 141, 169, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 72, 78, 88, 26, 48, 52, 57, 60, 135, 150, 178, 254, 108, 121, 145, 83, 105, 118, 142, 76, 106, 119, 143, 201, 118, 133, 159, 122, 117, 129, 152, 25, 168, 176, 190, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    118, 128, 145, 3, 104, 117, 140, 92, 114, 127, 152, 180, 131, 147, 177, 237, 133, 149, 178, 249, 38, 42, 50, 222, 137, 152, 180, 249, 126, 142, 170, 182, 114, 128, 154, 182, 104, 117, 140, 227, 95, 107, 128, 238, 83, 93, 112, 248, 84, 95, 113, 239, 104, 117, 141, 180, 115, 129, 155, 93, 127, 140, 165, 4,
    98, 109, 130, 153, 109, 123, 147, 254, 145, 163, 195, 153, 138, 154, 182, 56, 115, 123, 138, 5, 92, 99, 109, 35, 134, 149, 177, 230, 0, 0, 0, 0, 0, 0, 0, 0, 120, 133, 159, 143, 135, 151, 181, 115, 86, 89, 93, 5, 41, 45, 51, 54, 40, 45, 53, 150, 107, 120, 144, 254, 122, 137, 164, 154,
    51, 57, 66, 147, 83, 93, 112, 255, 108, 121, 145, 159, 113, 126, 151, 62, 123, 136, 159, 8, 87, 93, 103, 35, 125, 141, 169, 230, 0, 0, 0, 0, 0, 0, 0, 0, 129, 143, 169, 143, 140, 156, 184, 115, 134, 147, 172, 8, 124, 138, 165, 60, 124, 139, 167, 155, 131, 147, 177, 255, 131, 147, 176, 153,
    64, 68, 73, 2, 36, 39, 45, 86, 41, 46, 54, 173, 60, 67, 80, 232, 75, 84, 101, 251, 89, 100, 120, 228, 105, 118, 142, 250, 110, 123, 148, 187, 118, 132, 158, 187, 126, 141, 169, 229, 134, 149, 177, 239, 136, 152, 179, 250, 136, 152, 181, 234, 139, 156, 186, 175, 130, 145, 173, 90, 124, 134, 151, 3,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 71, 74, 79, 19, 60, 64, 73, 50, 92, 103, 124, 254, 86, 95, 111, 84, 90, 100, 117, 76, 126, 141, 168, 201, 113, 126, 150, 119, 99, 105, 117, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 93, 105, 125, 231, 135, 151, 181, 46, 0, 0, 0, 0, 137, 154, 184, 212, 123, 137, 164, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 74, 83, 98, 191, 133, 149, 179, 102, 111, 121, 139, 17, 134, 150, 180, 252, 126, 140, 166, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 43, 48, 57, 132, 121, 136, 164, 197, 121, 135, 161, 115, 130, 146, 175, 221, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 43, 47, 52, 46, 87, 98, 118, 254, 126, 142, 170, 254, 124, 139, 166, 135, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 51, 57, 67, 118, 115, 128, 152, 170, 127, 140, 164, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  Ihandle* image = IupImageRGBA(16, 16, imgdata);
  return image;
}

#define TEST_IMAGE_SIZE 16

static Ihandle* load_image_TestImage(void)
{
  unsigned char image_data_8 [TEST_IMAGE_SIZE*TEST_IMAGE_SIZE] = 
  {
    5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
    5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
    5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
    5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
    5,0,0,0,1,1,1,1,2,2,2,2,0,0,0,5, 
    5,0,0,0,1,1,1,1,2,2,2,2,0,0,0,5, 
    5,0,0,0,1,1,1,1,2,2,2,2,0,0,0,5, 
    5,0,0,0,1,1,1,1,2,2,2,2,0,0,0,5, 
    5,0,0,0,3,3,3,3,4,4,4,4,0,0,0,5, 
    5,0,0,0,3,3,3,3,4,4,4,4,0,0,0,5, 
    5,0,0,0,3,3,3,3,4,4,4,4,0,0,0,5, 
    5,0,0,0,3,3,3,3,4,4,4,4,0,0,0,5,
    5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
    5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
    5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,
    5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
  };

  Ihandle* image = IupImage(TEST_IMAGE_SIZE, TEST_IMAGE_SIZE, image_data_8);
  IupSetAttribute(image, "0", "BGCOLOR");
  IupSetAttribute(image, "1", "255 0 0"); 
  IupSetAttribute(image, "2", "0 255 0");
  IupSetAttribute(image, "3", "0 0 255"); 
  IupSetAttribute(image, "4", "255 255 255"); 
  IupSetAttribute(image, "5", "0 0 0"); 

  return image;
}

static int cbUnHideAllTabs(Ihandle* ih)
{
  Ihandle* tabs = (Ihandle*)IupGetAttribute(ih, "APP_TABS");
  int i, count = IupGetInt(tabs, "COUNT");

  for (i = 0; i < count; i++)
  {
    if (!IupGetIntId(tabs, "TABVISIBLE", i))
    {
      IupSetAttributeId(tabs, "TABVISIBLE", i, "Yes");
      printf("tab %d = was hidden\n", i);
    }
    else
      printf("tab %d = is visible\n", i);
  }

  return IUP_DEFAULT;
}

static int cbEnableAllTabs(Ihandle* ih)
{
  Ihandle* tabs = (Ihandle*)IupGetAttribute(ih, "APP_TABS");
  int i, count = IupGetInt(tabs, "COUNT");

  for (i = 0; i < count; i++)
  {
    if (!IupGetIntId(tabs, "TABACTIVE", i))
    {
      IupSetAttributeId(tabs, "TABACTIVE", i, "Yes");
      printf("tab %d = was inactive\n", i);
    }
    else
      printf("tab %d = is active\n", i);
  }

  return IUP_DEFAULT;
}

static int cbRemoveThisTab(Ihandle* ih)
{
  Ihandle* tabs = (Ihandle*)IupGetAttribute(ih, "APP_TABS");
  int pos = IupGetInt(ih, "APP_THISTAB");
  Ihandle* child = IupGetChild(tabs, pos);

  if (child)
  {
    IupDestroy(child);
    IupRefreshChildren(tabs);  /* update children layout */
  }

  return IUP_DEFAULT;
}

static int cbHideThisTab(Ihandle* ih)
{
  Ihandle* tabs = (Ihandle*)IupGetAttribute(ih, "APP_TABS");
  int pos = IupGetInt(ih, "APP_THISTAB");
  IupSetAttributeId(tabs, "TABVISIBLE", pos, "No");
  return IUP_DEFAULT;
}

static int cbDisableThisTab(Ihandle* ih)
{
  Ihandle* tabs = (Ihandle*)IupGetAttribute(ih, "APP_TABS");
  int pos = IupGetInt(ih, "APP_THISTAB");
  IupSetAttributeId(tabs, "TABINACTIVE", pos, "No");
  return IUP_DEFAULT;
}

static int cbChildButton(Ihandle* ih)
{
  printf("button(%s)\n", IupGetAttribute(ih, "TITLE"));
  return IUP_DEFAULT;
}

static int cbTest(Ihandle* ih)
{
  Ihandle* tabs = (Ihandle*)IupGetAttribute(ih, "APP_TABS");

#if 1
  int count = IupGetInt(tabs, "COUNT");
  IupSetInt(tabs, "VALUEPOS", count - 1);
#endif

#if 0
  char att[50];
  int m_handle_id = 1;
  char* title;
  sprintf(att, "TABTITLE%d", m_handle_id);
  {
    Ihandle* child = IupGetChild(tabs, 1);
    title = IupGetAttribute(child, "TABTITLE");
    printf("%s=%s\n", att, title);
  }
  title = IupGetAttribute(tabs, att);
  printf("%s=%s\n", att, title);
#endif

#if 0
  IupSetAttribute(tabs, "VALUEPOS", "0");
  IupSetAttribute(tabs, "TABTITLE0", "1asdasd");
  printf("VALUE=%s\n", IupGetAttribute(tabs, "VALUE"));
  if (IupGetInt(tabs, "TABVISIBLE2"))
    IupSetAttribute(tabs, "TABVISIBLE2", "No");
  else
    IupSetAttribute(tabs, "TABVISIBLE2", "Yes");
#endif

  return IUP_DEFAULT;
}

static int cbType(Ihandle* ih, int v)
{
  if (v)
  {
    char* title = IupGetAttribute(ih, "TITLE");
    Ihandle* tabs = (Ihandle*)IupGetAttribute(ih, "APP_TABS");

    switch(*title)
    {
    case 'L':
      IupSetAttribute(tabs, "TABTYPE", "LEFT");
      break;
    case 'R':
      IupSetAttribute(tabs, "TABTYPE", "RIGHT");
      break;
    case 'T':
      IupSetAttribute(tabs, "TABTYPE", "TOP");
      break;
    case 'B':
      IupSetAttribute(tabs, "TABTYPE", "BOTTOM");
      break;
    }

    IupRefresh(tabs); /* update children layout */
  }
  return IUP_DEFAULT;
}

static int cbOrientation(Ihandle* ih, int v)
{
  if (v)
  {
    char* title = IupGetAttribute(ih, "TITLE");
    Ihandle* tabs = (Ihandle*)IupGetAttribute(ih, "APP_TABS");

    switch(*title)
    {
    case 'H':
      IupSetAttribute(tabs, "TABORIENTATION", "HORIZONTAL");
      break;
    case 'V':
      IupSetAttribute(tabs, "TABORIENTATION", "VERTICAL");
      break;
    }

    IupRefresh(tabs); /* update children layout */
  }
  return IUP_DEFAULT;
}

static int cbAddTab(Ihandle* ih)
{
  Ihandle* tabs = (Ihandle*)IupGetAttribute(ih, "APP_TABS");
  Ihandle *vbox;

  vbox = IupBackgroundBox(IupVbox(IupLabel("Label XXX"), IupButton("Button XXX", "cbChildButton"), NULL));
  IupSetAttribute(vbox, "TABTITLE", "XXX");
  IupSetAttribute(vbox, "TITLE", "TABS XXX");

  IupAppend(tabs, vbox);
  IupMap(vbox);

  IupRefresh(tabs); /* update children layout */

  return IUP_DEFAULT;
}

static int cbInsertTab(Ihandle* ih)
{
  Ihandle* tabs = (Ihandle*)IupGetAttribute(ih, "APP_TABS");
  Ihandle* ref_vbox = IupGetHandle(IupGetAttribute(tabs, "VALUE"));
  Ihandle *vbox;

  vbox = IupBackgroundBox(IupVbox(IupLabel("Label YYY"), IupButton("Button YYY", "cbChildButton"), NULL));
  IupSetAttribute(vbox, "TABTITLE", "YYY");
  IupSetAttribute(vbox, "TITLE", "TABS YYY");

  IupInsert(tabs, ref_vbox, vbox);
  IupMap(vbox);

  IupRefresh(tabs); /* update children layout */

  return IUP_DEFAULT;
}

static int cbRemoveTab(Ihandle* ih)
{
  Ihandle* tabs = (Ihandle*)IupGetAttribute(ih, "APP_TABS");
  Ihandle* child = IupGetHandle(IupGetAttribute(tabs, "VALUE"));

  IupDestroy(child);

  IupRefresh(tabs); /* update children layout */

  return IUP_DEFAULT;
}

static void TestFocusInChild(Ihandle* ih, int pos)
{
  /* Works only in Windows, GTK will set the focus to the first child always */
  if(pos==0)
  {
    Ihandle* text = IupGetDialogChild(ih, "ATEXT");
    if (text)
      IupSetFocus(text);  
  }
  else if(pos==4)
  {
    Ihandle* button = IupGetDialogChild(ih, "EEEEEBUTTON");
    if (button)
      IupSetFocus(button);
  }
}

static int cbTabChange(Ihandle* ih, Ihandle* new_tab, Ihandle* old_tab)
{
  printf("new Tab: %s, old Tab: %s\n", IupGetAttribute(new_tab, "TABTITLE"), IupGetAttribute(old_tab, "TABTITLE"));
  return IUP_DEFAULT;
}

static int cbTabChangePos(Ihandle* ih, int new_tab, int old_tab)
{
  printf("new Tab: %d, old Tab: %d\n", new_tab, old_tab);
  TestFocusInChild(ih, new_tab);
  return IUP_DEFAULT;
}

static int cbTabRightButton(Ihandle* ih, int pos)
{
  Ihandle* menu = IupMenu(IupItem("Add Tab", "cbAddTab"),
                          IupItem("Insert Tab", "cbInsertTab"),
                          IupItem("Remove Current Tab", "cbRemoveTab"),
                          IupItem("Remove This", "cbRemoveThisTab"), 
                          IupItem("Hide This", "cbHideThisTab"),
                          IupItem("Disable This", "cbDisableThisTab"),
                          NULL);
  
  IupSetAttribute(menu, "APP_TABS", IupGetAttribute(ih, "APP_TABS"));
  IupSetInt(menu, "APP_THISTAB", pos);

  IupPopup(menu, IUP_MOUSEPOS, IUP_MOUSEPOS);
  IupDestroy(menu);

  return IUP_DEFAULT;
}

static int cbInactive(Ihandle *ih, int state)
{
  Ihandle* tabs = (Ihandle*)IupGetAttribute(ih, "APP_TABS");
  IupSetAttribute(tabs, "ACTIVE", state? "NO": "YES");
  return IUP_DEFAULT;
}

char *iupKeyCodeToName(int code);

static int k_any(Ihandle *ih, int c)
{
  if (iup_isprint(c))
    printf("K_ANY(%d = %s \'%c\')\n", c, iupKeyCodeToName(c), (char)c);
  else
    printf("K_ANY(%d = %s)\n", c, iupKeyCodeToName(c));
  printf("  MODKEYSTATE(%s)\n", IupGetGlobal("MODKEYSTATE"));
  return IUP_DEFAULT;
}

static int getfocus_cb(Ihandle *ih)
{
  printf("GETFOCUS_CB()\n");
  return IUP_DEFAULT;
}

static int help_cb(Ihandle* ih)
{
  printf("HELP_CB()\n");
  return IUP_DEFAULT;
}
     
static int killfocus_cb(Ihandle *ih)
{
  printf("KILLFOCUS_CB()\n");
  return IUP_DEFAULT;
}

static int leavewindow_cb(Ihandle *ih)
{
  printf("LEAVEWINDOW_CB()\n");
  return IUP_DEFAULT;
}

static int enterwindow_cb(Ihandle *ih)
{
  printf("ENTERWINDOW_CB()\n");
  return IUP_DEFAULT;
}

static int extrabutton_cb(Ihandle *ih, int button, int press)
{
  printf("EXTRABUTTON_CB(%d, %d)\n", button, press);
  return IUP_DEFAULT;
}

static Ihandle* CreateTabs(int tab)
{
  Ihandle *vboxA, *vboxB, *vboxG, *text, *button,
          *vboxC, *vboxD,*vboxE, *vboxF, *vboxH, *vboxI,
          *tabs;

  text = IupText(NULL);
  IupSetAttribute(text, "NAME", "ATEXT");
  button  = IupButton("Button EEEFOCUS", "cbChildButton");
  IupSetAttribute(button, "NAME", "EEEEEBUTTON");

//  if (tab)  // to test Tabs inside Tabs
  //  vboxA = IupVbox(CreateTabs(0), NULL);
//  else
  vboxA = IupBackgroundBox(IupVbox(IupFill(), IupSetAttributes(IupLabel("Label AAA"), "EXPAND=HORIZONTAL"), IupButton("Button AAA", "cbChildButton"), //NULL));
                             text, IupToggle("Button TTTT", "cbChildButton"), 
                             IupVal(NULL), IupSetAttributes(IupProgressBar(), "VALUE=0.5"), NULL));
  vboxB = IupBackgroundBox(IupVbox(IupLabel("Label BBB"), IupButton("Button BBB", "cbChildButton"), NULL));
  vboxC = IupBackgroundBox(IupVbox(IupLabel("Label CCC"), IupButton("Button CCC", "cbChildButton"), NULL));
  vboxD = IupBackgroundBox(IupVbox(IupLabel("Label DDD"), IupButton("Button DDD", "cbChildButton"), NULL));
  vboxE = IupBackgroundBox(IupVbox(IupFill(), IupLabel("Label EEE"), IupButton("Button EEE", "cbChildButton"), 
                                   button, IupButton("Button EEE", "cbChildButton"), NULL));
  vboxF = IupBackgroundBox(IupVbox(IupLabel("Label FFF"), IupButton("Button FFF", "cbChildButton"), NULL));
  vboxG = IupBackgroundBox(IupVbox(IupLabel("Label GGG"), IupButton("Button GGG", "cbChildButton"), NULL));
  vboxH = IupBackgroundBox(IupVbox(IupLabel("Label HHH"), IupButton("Button HHH", "cbChildButton"), NULL));
  vboxI = IupBackgroundBox(IupVbox(IupLabel("Canvas"), IupCanvas(NULL), NULL));

  IupSetAttribute(vboxA, "TABTITLE", "Aj");
  IupSetAttributeHandle(vboxA, "TABIMAGE", load_image_LogoTecgraf());
//  IupSetAttribute(vboxB, "TABTITLE", "BB");
  //  IupSetAttribute(vboxC, "TABTITLE", "CCC");
  IupStoreAttribute(vboxC, "TABIMAGE", IupGetAttribute(vboxA, "TABIMAGE"));
  IupSetAttribute(vboxD, "TABTITLE", "DDDD");
  IupSetAttribute(vboxE, "TABTITLE", "EEEjj");
  IupSetAttribute(vboxF, "TABTITLE", "FFFFff");
  IupSetAttribute(vboxG, "TABTITLE", "GGGG ggg");
  IupSetAttribute(vboxH, "TABTITLE", "HHHHHHHH");
  IupSetAttribute(vboxI, "TABTITLE", "Canvas (��)");
  IupSetAttribute(vboxA, "TITLE", "TABS A");
  IupSetAttribute(vboxB, "TITLE", "TABS BB");
  IupSetAttribute(vboxC, "TITLE", "TABS CCC");
//  IupSetAttribute(vboxC, "ACTIVE", "NO");
  IupSetAttribute(vboxF, "TABSIZE", "100");
  IupSetAttribute(vboxI, "BGCOLOR", "32 192 32");

  tabs = IupFlatTabs(vboxA, vboxB, vboxC, vboxD, vboxE, vboxF, vboxG, vboxH, vboxI, NULL);

  IupSetAttribute(tabs, "TABTITLE1", "BB");

  //IupSetCallback(tabs, "TABCHANGE_CB", (Icallback)cbTabChange);
  IupSetCallback(tabs, "TABCHANGEPOS_CB", (Icallback)cbTabChangePos);
  IupSetCallback(tabs, "RIGHTCLICK_CB", (Icallback)cbTabRightButton);

  //IupSetAttributeHandle(tabs, "TABIMAGE1", load_image_LogoTecgraf());
  IupSetAttributeHandle(tabs, "TABIMAGE1", load_image_TestImage());

  IupSetAttribute(tabs, "TABVISIBLE2", "NO");
  IupSetAttribute(tabs, "TABACTIVE3", "NO");
//  IupSetAttribute(tabs, "TABSHIGHCOLOR", "192 0 0");
  IupSetAttribute(tabs, "TABTIP4", "Tip Tab4");

  /* like Office 2016 */
  if (0)
  {
    IupSetAttribute(tabs, "FORECOLOR", "192 0 0");
    IupSetAttribute(tabs, "TABSBACKCOLOR", "192 0 0");
    IupSetAttribute(tabs, "HIGHCOLOR", "255 128 128");
    IupSetAttribute(tabs, "CLOSEHIGHCOLOR", "255 128 128");
    IupSetAttribute(tabs, "TABSFORECOLOR", "255 255 255");
    IupSetAttribute(tabs, "SHOWLINES", "NO");
  }
  
//  IupSetAttribute(tabs, "EXTRABUTTONS", "3");
//  IupSetAttribute(tabs, "EXTRATITLE1", "Button1");
//  IupSetAttribute(tabs, "EXTRATITLE2", "But2");
//  IupSetAttribute(tabs, "EXTRATIP2", "Tip But2");
  //  IupSetAttribute(tabs, "EXTRATITLE3", "3");
//  IupSetStrAttribute(tabs, "EXTRAIMAGE3", IupGetAttribute(tabs, "TABIMAGE1"));

//  IupSetAttribute(tabs, "EXPANDBUTTON", "Yes");

//  IupSetAttribute(tabs, "SHOWCLOSE", "yes");
//  IupSetAttribute(tabs, "TABSPADDING", "10x50");
//  IupSetAttribute(tabs, "TABSFONTSIZE", "36");

  IupSetAttribute(tabs, "TABFONTSTYLE4", "Bold");

//  IupSetAttribute(tabs, "BACKGROUND", "10 150 200");
//  IupSetAttribute(tabs, "BGCOLOR", "92 92 255");
//  IupSetAttribute(tabs, "FGCOLOR", "250 0 0");
//  IupSetAttribute(tabs, "FONT", "Helvetica, Italic 16");

  IupSetAttribute(tabs, "TIP", "IupFlatTabs Tip");
//  IupSetAttribute(tabs, "TIPFONT", "SYSTEM");
//  IupSetAttribute(tabs, "TIPBGCOLOR", "255 128 128");
//  IupSetAttribute(tabs, "TIPFGCOLOR", "0 92 255");

//  IupSetAttribute(tabs, "EXPAND", "YES");
  IupSetAttribute(tabs, "MARGIN", "0x0");  /* for children */
  IupSetAttribute(tabs, "RASTERSIZE", "500x200");  /* initial size */
  
  //IupSetCallback(tabs, "K_ANY",        (Icallback)k_any);
  //IupSetCallback(tabs, "HELP_CB",      (Icallback)help_cb);

  //IupSetCallback(tabs, "GETFOCUS_CB",  (Icallback)getfocus_cb); 
  //IupSetCallback(tabs, "KILLFOCUS_CB", (Icallback)killfocus_cb);
  //IupSetCallback(tabs, "ENTERWINDOW_CB", (Icallback)enterwindow_cb);
  //IupSetCallback(tabs, "LEAVEWINDOW_CB", (Icallback)leavewindow_cb);
  IupSetCallback(tabs, "EXTRABUTTON_CB", (Icallback)extrabutton_cb);

  return tabs;
}

void FlatTabsTest(void)
{
  Ihandle *box, *frm1, *frm2, *dlg, *tabs;

  tabs = CreateTabs(1);
  
  box = IupHbox(tabs, 
#if 0
                frm1 = IupFrame(IupRadio(IupVbox(IupToggle("TOP", "cbType"), 
                                                 IupToggle("LEFT", "cbType"), 
                                                 IupToggle("BOTTOM", "cbType"), 
                                                 IupToggle("RIGHT", "cbType"), 
                                                 NULL))), 
                frm2 = IupFrame(IupRadio(IupVbox(IupToggle("HORIZONTAL", "cbOrientation"), 
                                                 IupToggle("VERTICAL", "cbOrientation"), 
                                                 NULL))), 
#endif
                IupVbox(IupSetAttributes(IupButton("Add Tab", "cbAddTab"), "TIP=\"Button Tip\""),
                        IupButton("Insert Tab", "cbInsertTab"),
                        IupButton("Remove Tab", "cbRemoveTab"),
                        IupButton("UnHide All Tabs", "cbUnHideAllTabs"),
                        IupButton("Enable All Tabs", "cbEnableAllTabs"),
                        IupToggle("Inactive", "cbInactive"),
                        IupButton("Test", "cbTest"),
                        NULL), 
                NULL);

#if 0
  IupSetAttribute(frm1, "MARGIN", "5x5");
  IupSetAttribute(frm2, "MARGIN", "5x5");
  IupSetAttribute(frm1, "GAP", "0");
  IupSetAttribute(frm2, "GAP", "0");
  IupSetAttribute(frm1, "TITLE", "Type");
  IupSetAttribute(frm2, "TITLE", "Orientation");
#endif

  IupSetAttribute(box, "MARGIN", "10x10");
  IupSetAttribute(box, "GAP", "10");
  dlg = IupDialog(box);

  IupSetAttribute(dlg, "TITLE", "IupFlatTabs Test");
  IupSetAttribute(dlg, "APP_TABS", (char*)tabs);
//  IupSetAttribute(box, "BGCOLOR", "92 92 255");
//  IupSetAttribute(dlg, "BGCOLOR", "92 92 255");
//  IupSetAttribute(dlg, "BACKGROUND", "200 10 80");
//  IupSetAttributeHandle(dlg, "BACKGROUND", load_image_LogoTecgraf());
//  IupSetAttribute(dlg, "FGCOLOR", "10 200 80");

  IupMap(dlg);
  IupSetAttribute(dlg, "SIZE", NULL);
  IupShowXY(dlg, IUP_CENTER, IUP_CENTER);

  IupSetFunction("cbOrientation", (Icallback)cbOrientation);
  IupSetFunction("cbType", (Icallback)cbType);
  IupSetFunction("cbAddTab", (Icallback)cbAddTab);
  IupSetFunction("cbInsertTab", (Icallback)cbInsertTab);
  IupSetFunction("cbRemoveTab", (Icallback)cbRemoveTab);
  IupSetFunction("cbInactive", (Icallback)cbInactive);
  IupSetFunction("cbChildButton", (Icallback)cbChildButton);
  IupSetFunction("cbTest", (Icallback)cbTest);
  IupSetFunction("cbRemoveThisTab", (Icallback)cbRemoveThisTab);
  IupSetFunction("cbUnHideAllTabs", (Icallback)cbUnHideAllTabs);
  IupSetFunction("cbEnableAllTabs", (Icallback)cbEnableAllTabs);
  IupSetFunction("cbHideThisTab", (Icallback)cbHideThisTab);
  IupSetFunction("cbDisableThisTab", (Icallback)cbDisableThisTab);
}

#ifndef BIG_TEST
int main(int argc, char* argv[])
{
  IupOpen(&argc, &argv);

  FlatTabsTest();

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
#endif
