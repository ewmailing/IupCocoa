#include <stdlib.h>
#include <stdio.h>
#include <iup.h>

static void load_medal_images(void)
{
  Ihandle *image_gold, *image_silver, *image_bronze;  

  unsigned char img_gold[16*16] =
  {
    0,0,0,4,4,4,4,4,4,4,4,4,4,0,0,0,
    0,0,4,4,4,4,4,4,4,4,4,4,4,4,0,0,
    0,0,4,4,4,4,4,4,4,4,4,4,4,4,0,0,
    0,4,4,4,4,4,4,4,4,4,4,4,4,4,4,0,
    0,4,4,4,4,4,4,4,4,4,4,4,4,4,4,0,
    0,4,4,4,4,4,4,4,4,4,4,4,4,4,4,0,
    0,4,4,4,4,4,4,4,4,4,4,4,4,4,4,0,
    0,4,4,4,4,4,4,4,4,4,4,4,4,4,4,0,
    0,3,4,4,2,4,4,4,4,4,2,2,4,4,3,0,
    2,0,2,2,2,2,2,2,2,2,2,2,2,2,0,2,
    0,0,3,3,2,2,2,1,1,2,2,2,1,3,3,0,
    0,1,1,1,3,2,1,1,1,1,2,3,3,3,3,0,
    3,3,1,1,1,3,3,3,1,3,3,1,1,1,1,1,
    3,3,1,1,1,1,1,3,3,1,1,1,1,1,1,1,
    0,0,0,0,3,1,1,0,0,1,1,1,0,0,0,0,
    0,0,0,0,0,3,3,0,0,1,1,3,0,0,0,0
  };

  unsigned char img_silver[16*16] =
  {
    0,0,0,3,3,3,3,3,3,3,3,3,3,0,0,0,
    0,0,4,3,3,3,3,3,3,3,3,3,3,4,0,0,
    0,0,3,3,3,3,3,3,3,3,3,3,3,3,0,0,
    0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,0,
    0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,0,
    0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,0,
    0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,0,
    0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,0,
    0,3,3,3,3,3,3,3,3,3,3,2,3,3,3,0,
    3,0,2,2,2,2,2,3,3,2,2,2,2,2,0,3,
    0,0,1,1,2,2,1,1,1,2,2,2,1,1,3,0,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
    2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    2,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,
    0,0,0,0,2,1,1,0,0,0,1,1,0,0,0,0,
    0,0,0,0,0,1,1,0,0,1,1,2,0,0,0,0
  };

  unsigned char img_bronze[16*16] =
  {
    0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,
    0,0,4,1,1,1,1,1,1,1,1,1,1,4,0,0,
    0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
    0,1,1,1,3,3,1,1,1,1,3,3,1,1,1,0,
    4,0,3,3,3,3,3,3,3,3,3,3,3,2,0,4,
    0,0,3,3,3,3,2,2,2,2,3,2,2,3,4,0,
    0,2,2,2,2,3,2,2,2,2,2,2,2,2,3,0,
    4,3,2,2,2,2,2,2,2,2,2,2,2,2,2,3,
    4,3,2,2,2,2,2,2,4,2,2,2,2,2,2,3,
    0,0,0,0,3,2,2,0,0,2,2,2,0,0,0,0,
    0,0,0,0,0,3,3,0,0,2,2,4,0,0,0,0
  };

  image_gold = IupImage(16, 16, img_gold);
  IupSetAttribute(image_gold, "0", "BGCOLOR");
  IupSetAttribute(image_gold, "1", "128 0 0");
  IupSetAttribute(image_gold, "2", "128 128 0");
  IupSetAttribute(image_gold, "3", "255 0 0");
  IupSetAttribute(image_gold, "4", "255 255 0");
  IupSetHandle("IMGGOLD", image_gold);

  image_silver = IupImage(16, 16, img_silver);
  IupSetAttribute(image_silver, "0", "BGCOLOR");
  IupSetAttribute(image_silver, "1", "0 128 128");
  IupSetAttribute(image_silver, "2", "128 128 128");
  IupSetAttribute(image_silver, "3", "192 192 192");
  IupSetAttribute(image_silver, "4", "255 255 255");
  IupSetHandle("IMGSILVER", image_silver);

  image_bronze = IupImage(16, 16, img_bronze);
  IupSetAttribute(image_bronze, "0", "BGCOLOR");
  IupSetAttribute(image_bronze, "1", "128 0 0");
  IupSetAttribute(image_bronze, "2", "0 128 0");
  IupSetAttribute(image_bronze, "3", "128 128 0");
  IupSetAttribute(image_bronze, "4", "128 128 128");
  IupSetHandle("IMGBRONZE", image_bronze);
}

int main(int argc, char **argv) 
{
  Ihandle *dlg;
  Ihandle *list1, *list2;
  Ihandle *frm_medal1, *frm_medal2;

  IupOpen(&argc, &argv);

  list1 = IupList (NULL);
  IupSetAttributes(list1, "1=Gold, 2=Silver, 3=Bronze, 4=Latão, 5=None,"
                          "SHOWIMAGE=YES, DRAGDROPLIST=YES, XXX_SPACING=4, VALUE=4");
  load_medal_images();
  IupSetAttribute(list1, "IMAGE1", "IMGGOLD");
  IupSetAttribute(list1, "IMAGE2", "IMGSILVER");
  IupSetAttribute(list1, "IMAGE3", "IMGBRONZE");
  //IupSetAttribute(list1, "MULTIPLE", "YES");
  IupSetAttribute(list1, "DRAGSOURCE", "YES");
  //IupSetAttribute(list1, "DRAGSOURCEMOVE", "YES");
  IupSetAttribute(list1, "DRAGTYPES", "ITEMLIST");
  
  frm_medal1 = IupFrame (list1);
  IupSetAttribute (frm_medal1, "TITLE", "List 1");
  
  list2 = IupList (NULL);
  IupSetAttributes(list2, "1=Açaí, 2=Cajá, 3=Pêssego, 4=Limão, 5=Morango, 6=Coco,"
                          "SHOWIMAGE=YES, DRAGDROPLIST=YES, XXX_SPACING=4, VALUE=4");
  IupSetAttribute(list2, "DROPTARGET", "YES");
  IupSetAttribute(list2, "DROPTYPES", "ITEMLIST");
  frm_medal2 = IupFrame (list2);
  IupSetAttribute (frm_medal2, "TITLE", "List 2");
  
  dlg = IupDialog (IupHbox (frm_medal1, frm_medal2, NULL));
  IupSetAttribute (dlg, "TITLE", "IupList Example");
  IupShowXY (dlg, IUP_CENTER, IUP_CENTER);

  IupMainLoop ();
  IupClose ();
  return EXIT_SUCCESS;
}
