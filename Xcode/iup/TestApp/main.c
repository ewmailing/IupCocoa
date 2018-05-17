#include <stdlib.h>
#include <stdio.h>
#include "iup.h"
#include "iupkey.h"
#include "iup_config.h"

extern void DoLocalizedMessageBox(void);

Ihandle* g_someLabel1 = NULL;

static int DoSomething(Ihandle* button_object)
{
	fprintf(stderr, "DoSomething\n");
	
	//	IupSetAttribute(g_someLabel1, "ALIGNMENT", "ALEFT:ACENTER");
	
	//	IupSetAttribute(g_someLabel1, "WORDWRAP", "YES");
	
	//	IupSetAttribute(g_someLabel1, "ELLIPSIS", "YES");
	IupSetAttribute(g_someLabel1, "PADDING", "40x40");
	
	return IUP_NOERROR;
	
}

static int OnOpenFileSelect(Ihandle* button_object)
{
	int ret_status;
	const char* selected_path = NULL;
	Ihandle* file_dialog = IupFileDlg();
	
	
	
	//		printf("OnOpenFileSelect\n");
	IupSetAttribute(file_dialog, "DIALOGTYPE", "DIR");
	IupSetStrAttribute(file_dialog, "TITLE", "Select Project Directory");
	
	//	IupSetStrAttribute(file_dialog, "DIRECTORY",ApplicationState_GetLastOpenFileDialogPath());
	
	
	IupPopup(file_dialog, IUP_CURRENT, IUP_CURRENT);
	
	ret_status = IupGetInt(file_dialog, "STATUS");
	
	if(-1 != ret_status)
	{
#define MY_MAX_PATH_LEN 2048
		
		
		selected_path = IupGetAttribute(file_dialog, "VALUE");
		//		SDL_Log("Selected path: %s", selected_path);
		fprintf(stderr, "Selected path: %s\n", selected_path);
		
	}
	else
	{
		/* user cancelled */
		fprintf(stderr, "cancelled\n");
		
	}
	
	//  printf("OnNewProjectCallback(button=%d, press=%d)\n", button, press);
	
	
	
	return IUP_DEFAULT;
}



static void text2multiline (Ihandle* ih, char* attribute)
{
	Ihandle *mltline = IupGetDialogChild(ih, "mltline");
	Ihandle *text = IupGetDialogChild(ih, "text");
	IupSetAttribute (mltline, attribute, IupGetAttribute (text, "VALUE"));
}

static void multiline2text (Ihandle* ih, char* attribute)
{
	Ihandle *mltline = IupGetDialogChild(ih, "mltline");
	Ihandle *text = IupGetDialogChild(ih, "text");
	IupSetAttribute (text, "VALUE", IupGetAttribute(mltline, attribute));
}

static int btn_append_cb (Ihandle* ih)
{
	text2multiline(ih, "APPEND");
	return IUP_DEFAULT;
}

static int btn_insert_cb (Ihandle* ih)
{
	text2multiline(ih, "INSERT");
	return IUP_DEFAULT;
}

static int btn_clip_cb (Ihandle* ih)
{
	text2multiline(ih, "CLIPBOARD");
	return IUP_DEFAULT;
}

static int btn_key_cb (Ihandle* ih)
{
	Ihandle *mltline = IupGetDialogChild(ih, "mltline");
	Ihandle *text = IupGetDialogChild(ih, "text");
	IupSetFocus(mltline);
	IupFlush();
	IupSetfAttribute(NULL, "KEY", "%d", IupGetInt(text, "VALUE"));
	return IUP_DEFAULT;
}

static int btn_caret_cb (Ihandle* ih)
{
	Ihandle *opt = IupGetHandle("text2multi");
	if (IupGetInt(opt, "VALUE"))
		text2multiline(ih, "CARET");
	else
		multiline2text(ih, "CARET");
	return IUP_DEFAULT;
}

static int btn_readonly_cb (Ihandle* ih)
{
	Ihandle *opt = IupGetHandle("text2multi");
	if (IupGetInt(opt, "VALUE"))
	  text2multiline(ih, "READONLY");
	else
	  multiline2text(ih, "READONLY");
	if (IupGetInt(opt, "VALUE"))
		text2multiline(ih, "COUNT");
	else
		multiline2text(ih, "COUNT");
	return IUP_DEFAULT;
}

static int btn_selection_cb (Ihandle* ih)
{
	Ihandle *opt = IupGetHandle ("text2multi");
	if (IupGetInt (opt, "VALUE"))
		text2multiline (ih, "SELECTIONPOS");
	else
		multiline2text (ih, "SELECTIONPOS");
	return IUP_DEFAULT;
}

static int btn_selectedtext_cb (Ihandle* ih)
{
	Ihandle *opt = IupGetHandle ("text2multi");
	if (IupGetInt (opt, "VALUE"))
		text2multiline (ih, "SELECTEDTEXT");
	else
		multiline2text (ih, "SELECTEDTEXT");
	return IUP_DEFAULT;
}

static int btn_overwrite_cb (Ihandle* ih)
{
	Ihandle *opt = IupGetHandle ("text2multi");
	if (IupGetInt (opt, "VALUE"))
		text2multiline (ih, "OVERWRITE");
	else
		multiline2text (ih, "OVERWRITE");
	return IUP_DEFAULT;
}

static int btn_active_cb(Ihandle *ih)
{
	Ihandle *opt = IupGetHandle ("text2multi");
	if (IupGetInt (opt, "VALUE"))
		text2multiline (ih, "ACTIVE");
	else
		multiline2text (ih, "ACTIVE");
	return IUP_DEFAULT;
}

static int btn_remformat_cb(Ihandle *ih)
{
	text2multiline (ih, "REMOVEFORMATTING");
	return IUP_DEFAULT;
}

static int btn_nc_cb (Ihandle* ih)
{
	Ihandle *opt = IupGetHandle ("text2multi");
	if (IupGetInt (opt, "VALUE"))
		text2multiline (ih, "NC");
	else
		multiline2text (ih, "NC");
	return IUP_DEFAULT;
}

static int btn_value_cb (Ihandle* ih)
{
	Ihandle *opt = IupGetHandle ("text2multi");
	if (IupGetInt (opt, "VALUE"))
		text2multiline (ih, "VALUE");
	else
		multiline2text (ih, "VALUE");
	return IUP_DEFAULT;
}

static int btn_tabsize_cb (Ihandle* ih)
{
	Ihandle *opt = IupGetHandle ("text2multi");
	if (IupGetInt (opt, "VALUE"))
		text2multiline (ih, "TABSIZE");
	else
		multiline2text (ih, "TABSIZE");
	return IUP_DEFAULT;
}

static int k_f2(void)
{
	printf("K_F2\n");
	return IUP_DEFAULT;
}

static int file_open(void)
{
	char filename[100] = "";
	IupGetFile(filename);  // test key after dlg in multiline
	printf(filename);
	return IUP_DEFAULT;
}

/*
 char *iupKeyCodeToName(int code);
 
 static int k_any(Ihandle *ih, int c)
 {
	if (iup_isprint(c))
 printf("K_ANY(%d = %s \'%c\')\n", c, iupKeyCodeToName(c), (char)c);
	else
 printf("K_ANY(%d = %s)\n", c, iupKeyCodeToName(c));
	printf("  CARET(%s)\n", IupGetAttribute(ih, "CARET"));
	if (c == K_cA)
 return IUP_IGNORE;   // Sound a beep in Windows
	if (c == K_cP)
	{
 file_open();
 return IUP_IGNORE;   // Sound a beep in Windows
	}
	return IUP_CONTINUE;
 }
 */
static int valuechanged_cb(Ihandle *ih)
{
	printf("VALUECHANGED_CB()=%s\n", IupGetAttribute(ih, "VALUE"));
	return IUP_DEFAULT;
}

/*
 static int action(Ihandle *ih, int c, char* after)
 {
	if (iup_isprint(c))
 printf("ACTION(%d = %s \'%c\', %s)\n", c, iupKeyCodeToName(c), (char)c, after);
	else
 printf("ACTION(%d = %s, %s)\n", c, iupKeyCodeToName(c), after);
	if (c == K_i)
 return IUP_IGNORE;   // OK
	if (c == K_cD)
 return IUP_IGNORE;   // Sound a beep in Windows
	if (c == K_h)
 return K_j;
	return IUP_DEFAULT;
 }
 */

static int caret_cb(Ihandle *ih, int lin, int col, int pos)
{
	printf("CARET_CB(%d, %d - %d)\n", lin, col, pos);
	printf("  CARET(%s - %s)\n", IupGetAttribute(ih, "CARET"), IupGetAttribute(ih, "CARETPOS"));
	return IUP_DEFAULT;
}

static int getfocus_cb(void)
{
	printf("GETFOCUS_CB()\n");
	return IUP_DEFAULT;
}

static int help_cb(void)
{
	printf("HELP_CB()\n");
	return IUP_DEFAULT;
}

static int killfocus_cb(void)
{
	printf("KILLFOCUS_CB()\n");
	return IUP_DEFAULT;
}

static int leavewindow_cb(void)
{
	printf("LEAVEWINDOW_CB()\n");
	return IUP_DEFAULT;
}

static int enterwindow_cb(void)
{
	printf("ENTERWINDOW_CB()\n");
	return IUP_DEFAULT;
}

static int btn_def_esc_cb(void)
{
	printf("DEFAULTESC\n");
	return IUP_DEFAULT;
}

static int btn_def_enter_cb(void)
{
	printf("DEFAULTENTER\n");
	return IUP_DEFAULT;
}

static int dropfiles_cb(Ihandle *ih, const char* filename, int num, int x, int y)
{
	printf("DROPFILES_CB(%s, %d, x=%d, y=%d)\n", filename, num, x, y);
	return IUP_DEFAULT;
}

static int button_cb(Ihandle *ih,int but,int pressed,int x,int y,char* status)
{
	int lin, col, pos;
	printf("BUTTON_CB(but=%c (%d), x=%d, y=%d [%s])\n",(char)but,pressed,x,y, status);
	pos = IupConvertXYToPos(ih, x, y);
	IupTextConvertPosToLinCol(ih, pos, &lin, &col);
	printf("         (lin=%d, col=%d, pos=%d)\n", lin, col, pos);
	return IUP_DEFAULT;
}

static int motion_cb(Ihandle *ih,int x,int y,char* status)
{
	int lin, col, pos;
	printf("MOTION_CB(x=%d, y=%d [%s])\n",x,y, status);
	pos = IupConvertXYToPos(ih, x, y);
	IupTextConvertPosToLinCol(ih, pos, &lin, &col);
	printf("         (lin=%d, col=%d, pos=%d)\n", lin, col, pos);
	return IUP_DEFAULT;
}

static int spin_cb(Ihandle* ih, int pos)
{
	printf("spin_cb %d\n", pos);
	return IUP_DEFAULT;
	
}

void TextTest(void)
{
	int formatting = 0;
	Ihandle *dlg, *mltline, *text, *opt, *btn_def_enter, *btn_def_esc, *btn_active, *btn_overwrite,
	*btn_append, *btn_insert, *btn_caret, *btn_clip, *btn_key, *btn_readonly, *btn_tabsize,
	*btn_selection, *btn_selectedtext, *btn_nc, *btn_value, *lbl, *formattag, *btn_remformat;
	
	//  IupSetGlobal("UTF8AUTOCONVERT", "NO");
	
	text = IupText (NULL);
	IupSetAttribute(text, "EXPAND", "HORIZONTAL");
	//  IupSetAttribute(text, "VALUE", "Single Line Text");
	IupSetAttribute(text, "CUEBANNER", "Enter Attribute Value Here");
	IupSetAttribute(text, "NAME", "text");
	IupSetAttribute(text, "TIP", "Attribute Value");
	//  IupSetAttribute(text, "FORMATTING", "YES");
	
	opt = IupToggle("Set/Get", NULL);
	IupSetAttribute (opt, "VALUE", "ON");
	IupSetHandle ("text2multi", opt);
	
	mltline = IupMultiLine(NULL);
	//  mltline = IupText(NULL);
	//  IupSetAttribute(mltline, "MULTILINE", "YES");
	IupSetAttribute(mltline, "NAME", "mltline");
	
	IupSetCallback(mltline, "DROPFILES_CB", (Icallback)dropfiles_cb);
	IupSetCallback(mltline, "BUTTON_CB",    (Icallback)button_cb);
	//  IupSetCallback(mltline, "MOTION_CB",    (Icallback)motion_cb);
	IupSetCallback(mltline, "HELP_CB",      (Icallback)help_cb);
	IupSetCallback(mltline, "GETFOCUS_CB",  (Icallback)getfocus_cb);
	IupSetCallback(mltline, "KILLFOCUS_CB", (Icallback)killfocus_cb);
	IupSetCallback(mltline, "ENTERWINDOW_CB", (Icallback)enterwindow_cb);
	IupSetCallback(mltline, "LEAVEWINDOW_CB", (Icallback)leavewindow_cb);
	//	IupSetCallback(mltline, "ACTION", (Icallback)action);
	//  IupSetCallback(mltline, "VALUECHANGED_CB", (Icallback)valuechanged_cb);
	//  IupSetCallback(mltline, "K_ANY", (Icallback)k_any);
	IupSetCallback(mltline, "K_F2", (Icallback)k_f2);
	//  IupSetCallback(mltline, "CARET_CB", (Icallback)caret_cb);
	//  IupSetAttribute(mltline, "WORDWRAP", "YES");
	//  IupSetAttribute(mltline, "BORDER", "NO");
	//  IupSetAttribute(mltline, "AUTOHIDE", "YES");
	//  IupSetAttribute(mltline, "BGCOLOR", "255 0 128");
	//  IupSetAttribute(mltline, "FGCOLOR", "0 128 192");
	//  IupSetAttribute(mltline, "PADDING", "15x15");
	if (IupGetInt(NULL, "UTF8MODE"))
		IupSetAttribute(mltline, "VALUE", "First Line\nSecond Line Big Big Big\nThird Line\nmore\nmore\n(Ã§Ã£ÃµÃ¡Ã³Ã©)"); // UTF-8
	else
	{
		char* txt = "First Line\nSecond Line Big Big Big\nThird Line\nmore\nmore\n(çãõáóé)";
		//int len = strlen(txt);
		IupSetAttribute(mltline, "VALUE", txt); // Windows-1252
	}
	IupSetAttribute(mltline, "TIP", "First Line\nSecond Line\nThird Line");
	//  IupSetAttribute(mltline, "FONT", "Helvetica, 14");
	//  IupSetAttribute(mltline, "MASK", IUP_MASK_FLOAT);
	//  IupSetAttribute(mltline, "FILTER", "UPPERCASE");
	//  IupSetAttribute(mltline, "ALIGNMENT", "ACENTER");
	//  IupSetAttribute(mltline, "CANFOCUS", "NO");
	//  IupSetAttribute(mltline, "SCROLLBAR", "NO");
	
	/* Turns on multiline expand and text horizontal expand */
	IupSetAttribute(mltline, "SIZE", "80x40");
	IupSetAttribute(mltline, "EXPAND", "YES");
	
	//  IupSetAttribute(mltline, "FONT", "Courier, 16");
	//  IupSetAttribute(mltline, "FONT", "Arial, 12");
	//    IupSetAttribute(mltline, "FORMATTING", "YES");
	
	formatting = 1;
	if (formatting)          /* just to make easier to comment this section */
	{
		/* formatting before Map */
		IupSetAttribute(mltline, "FORMATTING", "YES");
		
		formattag = IupUser();
		IupSetAttribute(formattag, "ALIGNMENT", "CENTER");
		IupSetAttribute(formattag, "SPACEAFTER", "10");
		//    IupSetAttribute(formattag, "FONTSIZE", "24");
		IupSetAttribute(formattag, "SELECTION", "3,1:3,50");
		IupSetAttribute(mltline, "ADDFORMATTAG_HANDLE", (char*)formattag);
		
		formattag = IupUser();
		IupSetAttribute(formattag, "BGCOLOR", "255 128 64");
		IupSetAttribute(formattag, "UNDERLINE", "SINGLE");
		IupSetAttribute(formattag, "WEIGHT", "BOLD");
		IupSetAttribute(formattag, "SELECTION", "3,7:3,11");
		IupSetAttribute(mltline, "ADDFORMATTAG_HANDLE", (char*)formattag);
	}
	
	/* Creates buttons */
	btn_append = IupButton ("&APPEND", NULL);
	btn_insert = IupButton ("INSERT", NULL);
	btn_caret = IupButton ("CARET", NULL);
	btn_readonly = IupButton ("READONLY", NULL);
	btn_selection = IupButton ("SELECTION", NULL);
	btn_selectedtext = IupButton ("SELECTEDTEXT", NULL);
	btn_nc = IupButton ("NC", NULL);
	btn_value = IupButton ("VALUE", NULL);
	btn_tabsize = IupButton ("TABSIZE", NULL);
	btn_clip = IupButton ("CLIPBOARD", NULL);
	btn_key = IupButton ("KEY", NULL);
	btn_def_enter = IupButton ("Default Enter", NULL);
	btn_def_esc = IupButton ("Default Esc", NULL);
	btn_active = IupButton("ACTIVE", NULL);
	btn_remformat = IupButton("REMOVEFORMATTING", NULL);
	btn_overwrite = IupButton("OVERWRITE", NULL);
	
	IupSetAttribute(btn_append, "TIP", "First Line\nSecond Line\nThird Line");
	
	/* Registers callbacks */
	IupSetCallback(btn_append, "ACTION", (Icallback) btn_append_cb);
	IupSetCallback(btn_insert, "ACTION", (Icallback) btn_insert_cb);
	IupSetCallback(btn_caret, "ACTION", (Icallback) btn_caret_cb);
	IupSetCallback(btn_readonly, "ACTION", (Icallback) btn_readonly_cb);
	IupSetCallback(btn_selection, "ACTION", (Icallback) btn_selection_cb);
	IupSetCallback(btn_selectedtext, "ACTION", (Icallback) btn_selectedtext_cb);
	IupSetCallback(btn_nc, "ACTION", (Icallback) btn_nc_cb);
	IupSetCallback(btn_value, "ACTION", (Icallback) btn_value_cb);
	IupSetCallback(btn_tabsize, "ACTION", (Icallback) btn_tabsize_cb);
	IupSetCallback(btn_clip, "ACTION", (Icallback) btn_clip_cb);
	IupSetCallback(btn_key, "ACTION", (Icallback) btn_key_cb);
	IupSetCallback(btn_def_enter, "ACTION", (Icallback) btn_def_enter_cb);
	IupSetCallback(btn_def_esc, "ACTION", (Icallback) btn_def_esc_cb);
	IupSetCallback(btn_active, "ACTION", (Icallback) btn_active_cb);
	IupSetCallback(btn_remformat, "ACTION", (Icallback) btn_remformat_cb);
	IupSetCallback(btn_overwrite, "ACTION", (Icallback) btn_overwrite_cb);
	
	lbl = IupLabel("&Multiline:");
	IupSetAttribute(lbl, "PADDING", "2x2");
	
	
	
	Ihandle* spinner = IupText(NULL);
	  IupSetAttribute(spinner, "EXPAND", "HORIZONTAL");
	IupSetAttribute(spinner, "SPIN", "YES");
	Ihandle* spinner2 = IupText(NULL);
	
//	IupSetAttribute(spinner2, "EXPAND", "HORIZONTAL");
	IupSetAttribute(spinner2, "SPIN", "YES");
	IupSetCallback(spinner2, "SPIN_CB", (Icallback) spin_cb);

	/* Creates dlg */
	dlg = IupDialog(IupVbox(lbl,
							mltline,
							IupHbox (text, opt, NULL),
							spinner,
							spinner2,

							IupHbox (btn_append, btn_insert, btn_caret, btn_readonly, btn_selection, NULL),
							IupHbox (btn_selectedtext, btn_nc, btn_value, btn_tabsize, btn_clip, btn_key, NULL),
							IupHbox (btn_def_enter, btn_def_esc, btn_active, btn_remformat, btn_overwrite, NULL),
							NULL));
	IupSetCallback(dlg, "K_cO", (Icallback)file_open);
	IupSetAttribute(dlg, "TITLE", "IupText Test");
	IupSetAttribute(dlg, "MARGIN", "10x10");
	IupSetAttribute(dlg, "GAP", "5");
	IupSetAttributeHandle(dlg, "DEFAULTENTER", btn_def_enter);
	IupSetAttributeHandle(dlg, "DEFAULTESC", btn_def_esc);
	IupSetAttribute(dlg, "SHRINK", "YES");

	if (formatting)          /* just to make easier to comment this section */
	{
		IupMap(dlg);
		/* formatting after Map */
		
		formattag = IupUser();
		IupSetAttribute(formattag, "ITALIC", "YES");
		IupSetAttribute(formattag, "STRIKEOUT", "YES");
		IupSetAttribute(formattag, "SELECTION", "2,1:2,12");
		IupSetAttribute(mltline, "ADDFORMATTAG_HANDLE", (char*)formattag);
		
		if (1)
		{
			int count = IupGetInt(mltline, "COUNT");
			IupSetAttribute(mltline, "APPEND", "Append Test");
			
			formattag = IupUser();
			IupSetAttribute(formattag, "FGCOLOR", "0 128 64");
			IupSetfAttribute(formattag, "SELECTIONPOS", "%d:%d", count+1, IupGetInt(mltline, "COUNT"));
			IupSetAttribute(mltline, "ADDFORMATTAG_HANDLE", (char*)formattag);
		}
	}
	
	/* Shows dlg in the center of the screen */
	IupShowXY(dlg, IUP_CENTER, IUP_CENTER);
	IupSetFocus(mltline);
}



void SizeTestLabel(void)
{
	Ihandle* the_label = IupLabel("The quick brown fox jumped over the lazy dog.");
//	IupSetAttribute(the_label, "ELLIPSIS", "YES");
//	IupSetAttribute(the_label, "EXPAND", "YES");
//	IupSetAttribute(the_label, "SHRINK", "YES");
//	IupSetAttribute(the_label, "RASTERSIZE", "60x17");
//	IupSetAttribute(the_label, "PADDING", "8x8");

	/* Creates dlg */
	Ihandle* the_dialog = IupDialog(the_label);
	/* Shows dlg in the center of the screen */
	IupShowXY(the_dialog, IUP_CENTER, IUP_CENTER);
	
}


void SizeTestTextField(void)
{
	Ihandle* text_field = IupText(NULL);
		//  IupSetAttribute(text, "VALUE", "Single Line Text");
	IupSetAttribute(text_field, "CUEBANNER", "Enter text here");
	
	/* Creates dlg */
	Ihandle* the_dialog = IupDialog(text_field);
	/* Shows dlg in the center of the screen */
	IupShowXY(the_dialog, IUP_CENTER, IUP_CENTER);
	
}

void SizeTestButton(void)
{
	Ihandle* the_button = IupButton("The Button", NULL);
	
	/* Creates dlg */
	Ihandle* the_dialog = IupDialog(the_button);
	/* Shows dlg in the center of the screen */
	IupShowXY(the_dialog, IUP_CENTER, IUP_CENTER);
	
}


void SizeTestToggle(void)
{
	Ihandle* the_button = IupToggle("The Toggle", NULL);
	
	/* Creates dlg */
	Ihandle* the_dialog = IupDialog(the_button);
	/* Shows dlg in the center of the screen */
	IupShowXY(the_dialog, IUP_CENTER, IUP_CENTER);
	
}

void LayoutTestGrid2Label(void)
{
	Ihandle* the_label = IupLabel("The Label:");
	Ihandle* the_label2 = IupLabel("The quick brown fox jumped over the lazy dog.");

	
	IupSetAttribute(the_label2, "EXPAND", "HORIZONTAL");
//	IupSetAttribute(the_label2, "SHRINK", "HORIZONTAL");

	IupSetAttribute(the_label2, "ELLIPSIS", "YES");
//	IupSetAttribute(the_label2, "MINSIZE", "80x17");

	//  IupSetAttribute(the_label2, "VALUE", "Single Line Text");
//	IupSetAttribute(the_label2, "CUEBANNER", "Enter text here");

	IupSetAttribute(the_label, "ALIGNMENT", "ARIGHT:ACENTER");
	IupSetAttribute(the_label2, "ALIGNMENT", "ALEFT:ACENTER");

	
	Ihandle* grid_box = IupGridBox(NULL);
	
	IupSetAttribute(grid_box, "ORIENTATION", "HORIZONTAL");
	IupSetAttribute(grid_box, "ALIGNMENTLIN", "ACENTER");
	IupSetInt(grid_box, "NUMDIV", 2);
//	IupSetInt(grid_box, "NUMDIV", 3);
	IupSetInt(grid_box, "SIZECOL", 1);
//	IupSetInt(grid_box, "SIZECOL", 2);
	
	IupSetAttribute(grid_box, "HOMOGENEOUSLIN", "YES");
	
	IupAppend(grid_box, the_label);
	IupAppend(grid_box, the_label2);

//	IupAppend(grid_box, button_select);
	
	

	/* Creates dlg */
	Ihandle* the_dialog = IupDialog(grid_box);
	/* Shows dlg in the center of the screen */
	IupShowXY(the_dialog, IUP_CENTER, IUP_CENTER);
	
}


void LayoutTestGrid2LabelText(void)
{
	Ihandle* the_label = IupLabel("The Label:");
	Ihandle* text_field = IupText(NULL);
	
	IupSetAttribute(text_field, "EXPAND", "HORIZONTAL");
	
	//  IupSetAttribute(text, "VALUE", "Single Line Text");
	IupSetAttribute(text_field, "CUEBANNER", "Enter text here");
	
	IupSetAttribute(the_label, "ALIGNMENT", "ARIGHT:ACENTER");
	IupSetAttribute(text_field, "ALIGNMENT", "ALEFT:ACENTER");
	
	
	Ihandle* grid_box = IupGridBox(NULL);
	
	IupSetAttribute(grid_box, "ORIENTATION", "HORIZONTAL");
	IupSetAttribute(grid_box, "ALIGNMENTLIN", "ACENTER");
	IupSetInt(grid_box, "NUMDIV", 2);
	//	IupSetInt(grid_box, "NUMDIV", 3);
	IupSetInt(grid_box, "SIZECOL", 1);
	//	IupSetInt(grid_box, "SIZECOL", 2);
	
	IupSetAttribute(grid_box, "HOMOGENEOUSLIN", "YES");
	
	IupAppend(grid_box, the_label);
	IupAppend(grid_box, text_field);
	
	//	IupAppend(grid_box, button_select);
	
	
	
	/* Creates dlg */
	Ihandle* the_dialog = IupDialog(grid_box);
	/* Shows dlg in the center of the screen */
	IupShowXY(the_dialog, IUP_CENTER, IUP_CENTER);
	
}



void LayoutTestGrid3LabelTextButton(void)
{
	Ihandle* the_label = IupLabel("The Label:");
	Ihandle* text_field = IupText(NULL);
	Ihandle* the_button = IupButton("The Button", NULL);
	
	IupSetAttribute(text_field, "EXPAND", "HORIZONTAL");
	
	IupSetAttribute(text_field, "VALUE", "The quick brown fox jumped over the lazy dog.");
	IupSetAttribute(text_field, "CUEBANNER", "Enter text here");
	IupSetInt(text_field, "VISIBLECOLUMNS", 10);

	
	IupSetAttribute(the_label, "ALIGNMENT", "ARIGHT:ACENTER");
	IupSetAttribute(text_field, "ALIGNMENT", "ALEFT:ACENTER");
	IupSetAttribute(the_button, "ALIGNMENT", "ARIGHT:ACENTER");

	
	Ihandle* grid_box = IupGridBox(NULL);
	
	IupSetAttribute(grid_box, "ORIENTATION", "HORIZONTAL");
	IupSetAttribute(grid_box, "ALIGNMENTLIN", "ACENTER");
	IupSetInt(grid_box, "NUMDIV", 3);
	IupSetInt(grid_box, "SIZECOL", 2);
	
	IupSetAttribute(grid_box, "HOMOGENEOUSLIN", "YES");
	
	IupAppend(grid_box, the_label);
	IupAppend(grid_box, text_field);
	
	IupAppend(grid_box, the_button);
	
	
	
	/* Creates dlg */
	Ihandle* the_dialog = IupDialog(grid_box);
	/* Shows dlg in the center of the screen */
	IupShowXY(the_dialog, IUP_CENTER, IUP_CENTER);
	
}


void LayoutTestGrid3LabelTextButtonInFrame(void)
{
	Ihandle* the_label = IupLabel("The Label:");
	Ihandle* text_field = IupText(NULL);
	Ihandle* the_button = IupButton("The Button", NULL);
	
	IupSetAttribute(text_field, "EXPAND", "HORIZONTAL");
	
	IupSetAttribute(text_field, "VALUE", "The quick brown fox jumped over the lazy dog.");
	IupSetAttribute(text_field, "CUEBANNER", "Enter text here");
	IupSetInt(text_field, "VISIBLECOLUMNS", 10);
	
	
	IupSetAttribute(the_label, "ALIGNMENT", "ARIGHT:ACENTER");
	IupSetAttribute(text_field, "ALIGNMENT", "ALEFT:ACENTER");
	IupSetAttribute(the_button, "ALIGNMENT", "ARIGHT:ACENTER");
	
	
	Ihandle* grid_box = IupGridBox(NULL);
	
	IupSetAttribute(grid_box, "ORIENTATION", "HORIZONTAL");
	IupSetAttribute(grid_box, "ALIGNMENTLIN", "ACENTER");
	IupSetInt(grid_box, "NUMDIV", 3);
	IupSetInt(grid_box, "SIZECOL", 2);
	
	IupSetAttribute(grid_box, "HOMOGENEOUSLIN", "YES");
	
	IupAppend(grid_box, the_label);
	IupAppend(grid_box, text_field);
	
	IupAppend(grid_box, the_button);
	
	
	Ihandle* container_frame = IupFrame(grid_box);

	
	
	/* Creates dlg */
	Ihandle* the_dialog = IupDialog(container_frame);
	/* Shows dlg in the center of the screen */
	IupShowXY(the_dialog, IUP_CENTER, IUP_CENTER);
	
}

static int change_string_callback(Ihandle* ih)
{
	Ihandle* the_label2 = (Ihandle*)IupGetAttribute(ih, "MY_USER_DATA");
	IupSetAttribute(the_label2, "TITLE", "The quick brown fox jumped over the lazy dog.");

	return IUP_DEFAULT;
}

void LayoutTestGrid3LabelLabelButton(void)
{
	Ihandle* the_label = IupLabel("The Label:");
	Ihandle* the_label2 = IupLabel("Hello World");
	Ihandle* the_button = IupButton("The Button", "change_string_callback");

	IupSetCallback(the_button, "ACTION", (Icallback)change_string_callback);
	IupSetAttribute(the_button, "MY_USER_DATA", (const char*)the_label2);
	
	IupSetAttribute(the_label2, "EXPAND", "HORIZONTAL");
	IupSetAttribute(the_label2, "ELLIPSIS", "YES");

//	IupSetAttribute(the_label2, "PADDING", "8x8");
//	IupSetAttribute(the_label2, "MINSIZE", "80x17");

	
	IupSetAttribute(the_label, "ALIGNMENT", "ARIGHT:ACENTER");
	IupSetAttribute(the_label2, "ALIGNMENT", "ALEFT:ACENTER");
	IupSetAttribute(the_button, "ALIGNMENT", "ARIGHT:ACENTER");
	
	
	Ihandle* grid_box = IupGridBox(NULL);
	
	IupSetAttribute(grid_box, "ORIENTATION", "HORIZONTAL");
	IupSetAttribute(grid_box, "ALIGNMENTLIN", "ACENTER");
	IupSetInt(grid_box, "NUMDIV", 3);
	IupSetInt(grid_box, "SIZECOL", 2);
	
	IupSetAttribute(grid_box, "HOMOGENEOUSLIN", "YES");
	
	IupAppend(grid_box, the_label);
	IupAppend(grid_box, the_label2);
	
	IupAppend(grid_box, the_button);
	
	
	
	/* Creates dlg */
	Ihandle* the_dialog = IupDialog(grid_box);
	/* Shows dlg in the center of the screen */
	IupShowXY(the_dialog, IUP_CENTER, IUP_CENTER);
	
}


void LayoutTestGrid3LabelLabelButtonInFrame(void)
{
	Ihandle* the_label = IupLabel("The Label:");
	Ihandle* the_label2 = IupLabel("The quick brown fox jumped over the lazy dog. The quick brown fox jumped over the lazy dog.");
	Ihandle* the_button = IupButton("The Button", "change_string_callback");
	
	IupSetCallback(the_button, "ACTION", (Icallback)change_string_callback);
	IupSetAttribute(the_button, "MY_USER_DATA", (const char*)the_label2);
	
	IupSetAttribute(the_label2, "EXPAND", "HORIZONTAL");
	IupSetAttribute(the_label2, "ELLIPSIS", "YES");
	
	//	IupSetAttribute(the_label2, "PADDING", "8x8");
	//	IupSetAttribute(the_label2, "MINSIZE", "80x17");
	
	
	IupSetAttribute(the_label, "ALIGNMENT", "ARIGHT:ACENTER");
	IupSetAttribute(the_label2, "ALIGNMENT", "ALEFT:ACENTER");
	IupSetAttribute(the_button, "ALIGNMENT", "ARIGHT:ACENTER");
	
	
	Ihandle* grid_box = IupGridBox(NULL);
	
	IupSetAttribute(grid_box, "ORIENTATION", "HORIZONTAL");
	IupSetAttribute(grid_box, "ALIGNMENTLIN", "ACENTER");
	IupSetInt(grid_box, "NUMDIV", 3);
	IupSetInt(grid_box, "SIZECOL", 2);
	
	IupSetAttribute(grid_box, "HOMOGENEOUSLIN", "YES");
	
	IupAppend(grid_box, the_label);
	IupAppend(grid_box, the_label2);
	
	IupAppend(grid_box, the_button);
	
	Ihandle* container_frame = IupFrame(grid_box);

	
	/* Creates dlg */
	Ihandle* the_dialog = IupDialog(container_frame);
	/* Shows dlg in the center of the screen */
	IupShowXY(the_dialog, IUP_CENTER, IUP_CENTER);
	
}




/*
 #ifndef BIG_TEST
 int main(int argc, char* argv[])
 {
	IupOpen(&argc, &argv);
	
	TextTest();
	
	IupMainLoop();
	
	IupClose();
	
	return EXIT_SUCCESS;
 }
 #endif
 */

void IupExitCallback()
{
	IupClose();
	
}


Ihandle* progressbar1;
Ihandle* progressbar2;
static float increment_amt = 1.0f;
const float MAX_BAR = 360.0f;
//static float increment_percent = 0.01f;
static Ihandle *btn_pause;
static Ihandle *timer = NULL;


static int time_cb(void)
{
	float value = IupGetFloat(progressbar2, "VALUE");
	//	value += increment*MAX_BAR;
	value += increment_amt;
	if (value > MAX_BAR) value = 0; /* start over */
	IupSetfAttribute(progressbar2, "VALUE", "%g", (double)value);
	return IUP_DEFAULT;
}

void ProgressbarTest2(void)
{
	Ihandle* pb1 = IupProgressBar();
	IupSetAttribute(pb1, "ORIENTATION", "VERTICAL");
	IupSetAttribute(pb1, "EXPAND", "YES");

	Ihandle* dlg = IupDialog(pb1);
	IupShowXY(dlg, IUP_CENTER, IUP_CENTER);

	
}
void ProgressbarTest(void)
{
	Ihandle *dlg, *vbox, *hbox;
	Ihandle *btn_restart, *btn_accelerate, *btn_decelerate, *btn_show1, *btn_show2;
	
	/* timer to update progressbar2 */
	
	if (timer)
		IupDestroy(timer);
	timer = IupTimer();
	IupSetCallback(timer, "ACTION_CB", (Icallback)time_cb);
	IupSetAttribute(timer, "TIME", "2000");
	
	
	progressbar1 = IupProgressBar();
	progressbar2 = IupProgressBar();
 
	IupSetAttribute(progressbar1, "EXPAND", "YES");
	IupSetAttribute(progressbar2, "EXPAND", "YES");
	IupSetAttribute(progressbar1, "MARQUEE", "YES");
	IupSetAttribute(progressbar2, "ORIENTATION", "VERTICAL");
	/*
	 IupSetAttribute(progressbar2, "BGCOLOR", "255 0 128");
	 IupSetAttribute(progressbar2, "FGCOLOR", "0 128 0");
	 IupSetAttribute(progressbar2, "RASTERSIZE", "30x100");
	 */
	//	IupSetAttribute(progressbar2, "MAX", "360");
	IupSetFloat(progressbar2, "MAX", MAX_BAR);
	IupSetAttribute(progressbar2, "VALUE", "25");
	//IupSetAttribute(progressbar2, "DASHED", "YES");
	
	btn_restart = IupButton(NULL, NULL);
	Ihandle* btn_pause = IupButton(NULL, NULL);
	btn_accelerate = IupButton(NULL, NULL);
	btn_decelerate = IupButton(NULL, NULL);
	btn_show1 = IupButton("Dashed", NULL);
	btn_show2 = IupButton("Marquee", NULL);
	
	
	IupSetAttribute(btn_restart, "IMAGE", "img_restart");
	IupSetAttribute(btn_restart, "TIP", "Restart" );
	IupSetAttribute(btn_pause, "IMAGE", "img_pause");
	IupSetAttribute(btn_pause, "TIP", "Play/Pause");
	IupSetAttribute(btn_accelerate, "IMAGE", "img_forward");
	IupSetAttribute(btn_accelerate, "TIP", "Accelerate");
	IupSetAttribute(btn_decelerate, "IMAGE", "img_rewind");
	IupSetAttribute(btn_decelerate, "TIP", "Decelerate");
	IupSetAttribute(btn_show1, "TIP", "Dashed or Continuous");
	IupSetAttribute(btn_show2, "TIP", "Marquee or Defined");
	
	hbox = IupHbox
	(
	 IupFill(),
	 btn_pause,
	 btn_restart,
	 btn_decelerate,
	 btn_accelerate,
	 btn_show1,
	 btn_show2,
	 IupFill(),
	 NULL
	 );
	
	//	vbox = IupHbox(IupVbox(progressbar1, hbox, NULL), progressbar2, NULL);
	vbox = IupHbox(IupVbox(progressbar2, hbox, NULL), progressbar1, NULL);
	IupSetAttribute(vbox, "MARGIN", "10x10");
	IupSetAttribute(vbox, "GAP", "5");
	
	dlg = IupDialog(vbox);
	/*
	 IupSetAttribute(dlg, "TITLE", "IupProgressBar Test");
	 IupSetCallback(dlg, "UNMAP_CB", (Icallback) unmap_cb);
	 
	 IupSetCallback(btn_pause, "ACTION", (Icallback) btn_pause_cb);
	 IupSetCallback(btn_restart, "ACTION", (Icallback) btn_restart_cb);
	 IupSetCallback(btn_accelerate, "ACTION", (Icallback) btn_accelerate_cb);
	 IupSetCallback(btn_decelerate, "ACTION", (Icallback) btn_decelerate_cb);
	 IupSetCallback(btn_show1, "ACTION", (Icallback) btn_show1_cb);
	 IupSetCallback(btn_show2, "ACTION", (Icallback) btn_show2_cb);
	 */
	IupShowXY(dlg, IUP_CENTER, IUP_CENTER);
	
	IupSetAttribute(timer, "RUN", "YES");
}



static int button_cb2(Ihandle *ih,int but,int pressed,int x,int y,char* status)
{
	printf("BUTTON_CB(but=%c (%d), x=%d, y=%d [%s]) - [pos=%d]\n",(char)but,pressed,x,y, status, IupConvertXYToPos(ih, x, y));
	return IUP_DEFAULT;
}

static int motion_cb2(Ihandle *ih,int x,int y,char* status)
{
	printf("MOTION_CB(x=%d, y=%d [%s]) - [pos=%d]\n",x,y, status,IupConvertXYToPos(ih, x, y));
	return IUP_DEFAULT;
}

static int multilist_cb2 (Ihandle *ih, char *s)
{
	printf("MULTISELECT_CB(%s)\n", s);
	return IUP_DEFAULT;
}

static int list_cb2 (Ihandle *ih, char *t, int i, int v)
{
	printf("ACTION[%p](%s - %d %d)\n", ih, t, i, v);
	return IUP_DEFAULT;
}

static int dropdown_cb2(Ihandle *ih, int state)
{
	printf("DROPDOWN_CB(%d)\n", state);
	return IUP_DEFAULT;
}

static int dblclick_cb2(Ihandle *ih, int pos, char *text)
{
	printf("DBLCLICK_CB(%d - %s)\n", pos, text);
	return IUP_DEFAULT;
}

static int edit_cb2 (Ihandle *ih, int c, char *after)
{
	printf("EDIT_CB(%d - %s)\n", c, after);
	return IUP_DEFAULT;
}

static int caret_cb2(Ihandle *ih, int lin, int col)
{
	printf("CARET_CB(%d, %d)\n", lin, col);
	return IUP_DEFAULT;
}

static int btok_cb2(Ihandle *ih)
{
	printf("Default Enter\n");
	return IUP_DEFAULT;
}

static int btcancel_cb2(Ihandle *ih)
{
	printf("Default Esc\n");
	return IUP_DEFAULT;
}

static int selectedtext_cb2(Ihandle *ih)
{
	Ihandle *list = (Ihandle*)IupGetAttribute(IupGetDialog(ih), "_ACTIVE_LIST");
	Ihandle *text = IupGetDialogChild(ih, "text");
	IupSetAttribute(text, "VALUE", IupGetAttribute(list, "SELECTEDTEXT"));
	return IUP_DEFAULT;
}

static int selection_cb2(Ihandle *ih)
{
	Ihandle *list = (Ihandle*)IupGetAttribute(IupGetDialog(ih), "_ACTIVE_LIST");
	Ihandle *text = IupGetDialogChild(ih, "text");
	IupSetAttribute(text, "VALUE", IupGetAttribute(list, "SELECTION"));
	return IUP_DEFAULT;
}

static int getcaret_cb2(Ihandle *ih)
{
	Ihandle *list = (Ihandle*)IupGetAttribute(IupGetDialog(ih), "_ACTIVE_LIST");
	Ihandle *text = IupGetDialogChild(ih, "text");
	IupSetAttribute(text, "VALUE", IupGetAttribute(list, "CARET"));
	return IUP_DEFAULT;
}

static int getvalue_cb2(Ihandle *ih)
{
	Ihandle *list = (Ihandle*)IupGetAttribute(IupGetDialog(ih), "_ACTIVE_LIST");
	Ihandle *text = IupGetDialogChild(ih, "text");
	IupSetAttribute(text, "VALUE", IupGetAttribute(list, "VALUE"));
	//  IupSetAttribute(list, "ACTIVE", "No");
	return IUP_DEFAULT;
}

static int setvalue_cb2(Ihandle *ih)
{
	Ihandle *list = (Ihandle*)IupGetAttribute(IupGetDialog(ih), "_ACTIVE_LIST");
	Ihandle *text = IupGetDialogChild(ih, "text");
	IupSetAttribute(list, "VALUE", IupGetAttribute(text, "VALUE"));
	//  IupSetAttribute(list, "ACTIVE", "Yes");
	return IUP_DEFAULT;
}

static int getcount_cb2(Ihandle *ih)
{
	Ihandle *list = (Ihandle*)IupGetAttribute(IupGetDialog(ih), "_ACTIVE_LIST");
	printf("COUNT=%s\n", IupGetAttribute(list, "COUNT"));
	return IUP_DEFAULT;
}

static int append_cb2(Ihandle *ih)
{
	Ihandle *list = (Ihandle*)IupGetAttribute(IupGetDialog(ih), "_ACTIVE_LIST");
	Ihandle *text = IupGetDialogChild(ih, "text");
	IupSetAttribute(list, "APPEND", IupGetAttribute(text, "VALUE"));
	return IUP_DEFAULT;
}

static int readonly_cb2(Ihandle *ih)
{
	Ihandle *list = (Ihandle*)IupGetAttribute(IupGetDialog(ih), "_ACTIVE_LIST");
	Ihandle *text = IupGetDialogChild(ih, "text");
	IupSetAttribute(list, "READONLY", IupGetAttribute(text, "VALUE"));
	return IUP_DEFAULT;
}

static int insertitem_cb2(Ihandle *ih)
{
	Ihandle *list = (Ihandle*)IupGetAttribute(IupGetDialog(ih), "_ACTIVE_LIST");
	Ihandle *text = IupGetDialogChild(ih, "text");
	IupSetAttribute(list, "INSERTITEM3", IupGetAttribute(text, "VALUE"));
	return IUP_DEFAULT;
}

static int setitem_cb2(Ihandle *ih)
{
	Ihandle *list = (Ihandle*)IupGetAttribute(IupGetDialog(ih), "_ACTIVE_LIST");
	Ihandle *text = IupGetDialogChild(ih, "text");
	IupSetAttribute(list, "3", IupGetAttribute(text, "VALUE"));
	return IUP_DEFAULT;
}

static int appenditem_cb2(Ihandle *ih)
{
	Ihandle *list = (Ihandle*)IupGetAttribute(IupGetDialog(ih), "_ACTIVE_LIST");
	Ihandle *text = IupGetDialogChild(ih, "text");
	IupSetAttribute(list, "APPENDITEM", IupGetAttribute(text, "VALUE"));
	return IUP_DEFAULT;
}

static int removeitem_cb2(Ihandle *ih)
{
	Ihandle *list = (Ihandle*)IupGetAttribute(IupGetDialog(ih), "_ACTIVE_LIST");
	Ihandle *text = IupGetDialogChild(ih, "text");
	IupSetAttribute(list, "REMOVEITEM", IupGetAttribute(text, "VALUE"));
	return IUP_DEFAULT;
}

static int topitem_cb2(Ihandle *ih)
{
	Ihandle *list = (Ihandle*)IupGetAttribute(IupGetDialog(ih), "_ACTIVE_LIST");
	Ihandle *text = IupGetDialogChild(ih, "text");
	IupSetAttribute(list, "TOPITEM", IupGetAttribute(text, "VALUE"));
	return IUP_DEFAULT;
}

static int showdropdown_cb2(Ihandle *ih)
{
	Ihandle *list = (Ihandle*)IupGetAttribute(IupGetDialog(ih), "_ACTIVE_LIST");
	IupSetAttribute(list, "SHOWDROPDOWN", "YES");
	return IUP_DEFAULT;
}

static void setactivelist(Ihandle* ih)
{
	Ihandle* dialog = IupGetDialog(ih);
	Ihandle* label = (Ihandle*)IupGetAttribute(dialog, "_LABEL");
	IupSetAttribute(dialog, "_ACTIVE_LIST", (char*)ih);
	IupSetAttribute(label, "TITLE", IupGetAttribute(IupGetParent(IupGetParent(ih)), "TITLE"));
}

static int getfocus_cb2(Ihandle *ih)
{
	setactivelist(ih);
	printf("GETFOCUS_CB(%s)\n", IupGetAttribute(IupGetParent(IupGetParent(ih)), "TITLE"));
	return IUP_DEFAULT;
}

static int killfocus_cb2(Ihandle *ih)
{
	printf("KILLFOCUS_CB(%s)\n", IupGetAttribute(IupGetParent(IupGetParent(ih)), "TITLE"));
	return IUP_DEFAULT;
}

static int leavewindow_cb2(Ihandle *ih)
{
	printf("LEAVEWINDOW_CB(%s)\n", IupGetAttribute(IupGetParent(IupGetParent(ih)), "TITLE"));
	return IUP_DEFAULT;
}

static int enterwindow_cb2(Ihandle *ih)
{
	setactivelist(ih);
	printf("ENTERWINDOW_CB(%s)\n", IupGetAttribute(IupGetParent(IupGetParent(ih)), "TITLE"));
	return IUP_DEFAULT;
}

char *iupKeyCodeToName(int code);

static int k_any2(Ihandle *ih, int c)
{
	if (iup_isprint(c))
		printf("K_ANY(%s, %d = %s \'%c\')\n", IupGetAttribute(IupGetParent(IupGetParent(ih)), "TITLE"), c, iupKeyCodeToName(c), (char)c);
	else
		printf("K_ANY(%s, %d = %s)\n", IupGetAttribute(IupGetParent(IupGetParent(ih)), "TITLE"), c, iupKeyCodeToName(c));
	return IUP_CONTINUE;
}

static int help_cb2(Ihandle* ih)
{
	printf("HELP_CB(%s)\n", IupGetAttribute(IupGetParent(IupGetParent(ih)), "TITLE"));
	return IUP_DEFAULT;
}

static int valuechanged_cb2(Ihandle *ih)
{
	printf("VALUECHANGED_CB(%p)=%s\n", ih, IupGetAttribute(ih, "VALUE"));
	return IUP_DEFAULT;
}

static void set_callbacks(Ihandle* list)
{
	IupSetCallback(list, "ACTION", (Icallback)list_cb2);
	IupSetCallback(list, "VALUECHANGED_CB", (Icallback)valuechanged_cb2);
	IupSetCallback(list, "DBLCLICK_CB", (Icallback)dblclick_cb2);
	IupSetCallback(list, "DROPDOWN_CB", (Icallback)dropdown_cb2);
	IupSetCallback(list, "EDIT_CB", (Icallback)edit_cb2);
	IupSetCallback(list, "CARET_CB", (Icallback)caret_cb2);
	//  IupSetCallback(list, "BUTTON_CB",    (Icallback)button_cb);
	//  IupSetCallback(list, "MOTION_CB",    (Icallback)motion_cb);
	
	IupSetCallback(list, "GETFOCUS_CB", getfocus_cb2);
	IupSetCallback(list, "KILLFOCUS_CB", killfocus_cb2);
	
	IupSetCallback(list, "ENTERWINDOW_CB", (Icallback)enterwindow_cb2);
	IupSetCallback(list, "LEAVEWINDOW_CB", (Icallback)leavewindow_cb2);
	
	IupSetCallback(list, "K_ANY", (Icallback)k_any2);
	IupSetCallback(list, "HELP_CB", (Icallback)help_cb2);
	
	//  IupSetAttribute(list, "DROPEXPAND", "NO");
	//  IupSetAttribute(list, "AUTOHIDE", "NO");
	//  IupSetAttribute(list, "SCROLLBAR", "NO");
	
	//	IupSetAttribute(list, "VISIBLEITEMS", "20");
	//  IupSetAttribute(list, "VISIBLECOLUMNS", "7");
	IupSetAttribute(list, "VISIBLELINES", "4");
}

//#define IupFrame IupRadio

Ihandle* ListTest(void)
{
	Ihandle *dlg, *list1, *list2, *list3, *list4, *text, *btok, *btcancel,
	*box, *lists, *buttons1, *buttons2, *buttons3, *label;
	
	list1 = IupList(NULL);
	
	
	IupSetAttributes(list1, "1=\"US$ 1000\", 2=\"US$ 2000\", 3=\"US$ 300.000.000\", 4=\"Strawberry Shortcake vs. B. Pudding\","
					 "EDITBOX=YES, DROPDOWN=YES, TIP=Edit+Drop, VALUE=\"Edit Here\", NAME=list1");
	//					 "EDITBOX=NO, DROPDOWN=YES, NAME=list1");
	set_callbacks(list1);
	printf("COUNT(list1)=%s\n", IupGetAttribute(list1, "COUNT"));
	
	//	return list1;
	
	//	dlg = IupDialog(list1);
	//	IupShowXY(dlg, IUP_CENTER, IUP_CENTER);
	//	IupShow(dlg);
	
	list2 = IupList(NULL);
	IupSetAttributes(list2, "1=\"Banana\", 2=\"Apple\", 3=\"Orange\", 4=\"Strawberry Shortcake vs. B. Pudding\", 5=\"Grape\","
					 "DROPDOWN=YES, NAME=list2, TIP=Drop, XXX_VALUE=2, XXX_SORT=YES, XXX_BGCOLOR=\"192 64 192\"");
	set_callbacks(list2);
	
	printf("COUNT(list2)=%s\n", IupGetAttribute(list2, "COUNT"));
	
	
	
	list4 = IupList(NULL);
	
	IupSetAttributes(list4, "1=\"Number 3\", 2=\"Number 4\", 3=\"Number 2\", 4=\"Number 1\", 5=\"Strawberry Shortcake vs. B. Pudding\", 6=\"Number 5\", 7=\"Number 7\","
					 "MULTIPLE=YES, NAME=list4, TIP=List, XXX_SORT=YES, VALUE=+--++--, XXX_CANFOCUS=NO");
	
	set_callbacks(list4);
	
	printf("COUNT(list4)=%s\n", IupGetAttribute(list4, "COUNT"));
	
	
	//	Ihandle* vb=IupVbox(list1, NULL);
	//	Ihandle* vb=IupVbox(list2, NULL);
	//	Ihandle* vb=IupVbox(list4, NULL);
	
	Ihandle* vb=IupVbox(list1, list2, list4, NULL);
	return vb;
	//	return list2;
	//	return list2;
	//	return list4;
	
	
#if 0
	
	list3 = IupList(NULL);
	IupSetAttributes(list3, "1=\"Char A\", 2=\"Char B\", 3=\"Char CCCCC\", 4=\"Char D\", 5=\"Char E\", 6=\"Char F\","
					 "EDITBOX=YES, NAME=list3, TIP=Edit+List, VALUE=\"Edit Here\"");
	set_callbacks(list3);
	printf("COUNT(list3)=%s\n", IupGetAttribute(list3, "COUNT"));
	
	//  IupSetAttributes(list4, "1=\"Number 1\", 2=\"Number 2\", 3=\"Number 3\", 4=\"Number 4\", 5=\"Number 5\", 6=\"Number 6\", 7=\"Number 7\","
	//                          "MULTIPLE=YES, NAME=list4, TIP=List, VALUE=+--++--");
	
	
	//IupSetAttributes(list1, "5=\"US$ 1000\", 6=\"US$ 2000\", 7=\"US$ 300.000.000\", 8=\"US$ 4000\"");
	//IupSetAttributes(list1, "9=\"US$ 1000\", 10=\"US$ 2000\", 11=\"US$ 300.000.000\", 12=\"US$ 4000\"");
	//IupSetAttributes(list1, "13=\"US$ 1000\", 14=\"US$ 2000\", 15=\"US$ 300.000.000\", 16=\"US$ 4000\"");
	//IupSetAttributes(list1, "17=\"US$ 1000\", 18=\"US$ 2000\", 19=\"US$ 300.000.000\", 20=\"US$ 4000\"");
	//IupSetAttributes(list1, "21=\"US$ 1000\", 22=\"US$ 2000\", 23=\"US$ 300.000.000\", 24=\"US$ 4000\"");
	
	
	//IupSetAttributes(list2, "6=\"R$ 1000\", 7=\"R$ 2000\", 8=\"R$ 123456789\", 9=\"R$ 4000\", 10=\"R$ 5000\",");
	//IupSetAttributes(list2, "11=\"R$ 1000\", 12=\"R$ 2000\", 13=\"R$ 123456789\", 14=\"R$ 4000\", 15=\"R$ 5000\",");
	//IupSetAttributes(list2, "16=\"R$ 1000\", 17=\"R$ 2000\", 18=\"R$ 123456789\", 19=\"R$ 4000\", 20=\"R$ 5000\",");
	
	//IupSetCallback(list4, "MULTISELECT_CB", (Icallback)multilist_cb);
	
	//  IupSetAttribute(list1, "READONLY", "YES");
	//  IupSetAttribute(list3, "READONLY", "YES");
	//  IupSetAttribute(list1, "PADDING", "10x10");
	//  IupSetAttribute(list3, "PADDING", "10x10");
	
	//  IupSetAttribute(list3, "SPACING", "10");
	//  IupSetAttribute(list4, "SPACING", "10");
	
	
	lists = IupHbox(
					IupVbox(
							IupSetAttributes(IupFrame(IupVbox(list1, NULL)), "TITLE=EDITBOX+DROPDOWN"),
							IupSetAttributes(IupFrame(IupVbox(list2, NULL)), "TITLE=DROPDOWN"),
							NULL),
					IupSetAttributes(IupFrame(IupVbox(list3, NULL)), "TITLE=EDITBOX"),
					IupSetAttributes(IupFrame(IupVbox(list4, NULL)), "TITLE=MULTIPLE"),
					IupVbox(
							btok = IupSetCallbacks(IupButton("Default Enter", NULL), "ACTION", btok_cb2, NULL),
							btcancel = IupSetCallbacks(IupButton("Default Esc", NULL), "ACTION", btcancel_cb2, NULL),
							NULL),
					NULL);
	
	text = IupText(NULL);
	IupSetAttribute(text, "EXPAND", "HORIZONTAL");
	IupSetAttribute(text, "NAME", "text");
	
	buttons1 = IupHbox(
					   IupSetCallbacks(IupButton("Get(VALUE)", NULL), "ACTION", getvalue_cb2, NULL),
					   IupSetCallbacks(IupButton("Set(VALUE)", NULL), "ACTION", setvalue_cb2, NULL),
					   IupSetCallbacks(IupButton("Get(COUNT)", NULL), "ACTION", getcount_cb2, NULL),
					   NULL);
	buttons2 = IupHbox(
					   IupSetCallbacks(IupButton("3", NULL), "ACTION", setitem_cb2, NULL),
					   IupSetCallbacks(IupButton("INSERTITEM3", NULL), "ACTION", insertitem_cb2, NULL),
					   IupSetCallbacks(IupButton("APPENDITEM", NULL), "ACTION", appenditem_cb2, NULL),
					   IupSetCallbacks(IupButton("REMOVEITEM", NULL), "ACTION", removeitem_cb2, NULL),
					   IupSetCallbacks(IupButton("SHOWDROPDOWN", NULL), "ACTION", showdropdown_cb2, NULL),
					   IupSetCallbacks(IupButton("TOPITEM", NULL), "ACTION", topitem_cb2, NULL),
					   NULL);
	buttons3 = IupHbox(
					   IupSetCallbacks(IupButton("APPEND", NULL), "ACTION", append_cb2, NULL),
					   IupSetCallbacks(IupButton("Get(CARET)", NULL), "ACTION", getcaret_cb2, NULL),
					   IupSetCallbacks(IupButton("Set(READONLY)", NULL), "ACTION", readonly_cb2, NULL),
					   IupSetCallbacks(IupButton("Get(SELECTEDTEXT)", NULL), "ACTION", selectedtext_cb2, NULL),
					   IupSetCallbacks(IupButton("Get(SELECTION)", NULL), "ACTION", selection_cb2, NULL),
					   NULL);
	
	box = IupVbox(lists, IupHbox(IupLabel("Attrib. Value:  "), text, NULL), buttons1, buttons2, buttons3, IupHbox(IupLabel("Current List:  "), label = IupLabel(NULL), NULL), NULL);
	IupSetAttributes(buttons1,"MARGIN=5x5, GAP=5");
	IupSetAttributes(buttons2,"MARGIN=5x5, GAP=5");
	IupSetAttributes(buttons3,"MARGIN=5x5, GAP=5");
	IupSetAttributes(lists,"MARGIN=10x10, GAP=10");
	IupSetAttribute(label,"EXPAND", "HORIZONTAL");
	
	
	dlg = IupDialog(box);
	IupSetAttribute(dlg, "TITLE", "IupList Test");
	IupSetAttributeHandle(dlg, "DEFAULTENTER", btok);
	IupSetAttributeHandle(dlg, "DEFAULTESC", btcancel);
	IupSetAttribute(dlg, "_LABEL", (char*)label);
	
	//  IupSetAttribute(box, "BGCOLOR", "92 92 255");
	//  IupSetAttribute(dlg, "BGCOLOR", "92 92 255");
	//  IupSetAttribute(dlg, "BACKGROUND", "200 10 80");
	//  IupSetAttribute(dlg, "BGCOLOR", "173 177 194");  // Motif BGCOLOR for documentation
	
	//  IupSetAttribute(dlg, "FONT", "Helvetica, 24");
	//  IupSetAttribute(box, "FGCOLOR", "255 0 0");
	
	IupShowXY(dlg, IUP_CENTER, IUP_CENTER);
#endif
	
}



Ihandle* DropDownListTest(void)
{
	Ihandle* list2;
	

	list2 = IupList(NULL);
//	IupSetAttributes(list2, "1=\"Banana\", 2=\"Apple\", 3=\"Orange\", 4=\"Strawberry Shortcake vs. B. Pudding\", 5=\"Grape\","
//					 "DROPDOWN=YES, NAME=list2, TIP=Drop, XXX_VALUE=2, XXX_SORT=YES, XXX_BGCOLOR=\"192 64 192\"");
	IupSetAttributes(list2, "1=\"Strawberry Shortcake vs. B. Pudding\","
					 "DROPDOWN=YES, NAME=list2");
//	set_callbacks(list2);
	
	
	Ihandle* dialog = IupDialog(list2);
	IupShow(dialog);
	
	return dialog;
	
	

}

Ihandle* EditboxDropDownListTest(void)
{
	Ihandle* list2;
	
	
	list2 = IupList(NULL);
	//IupSetAttributes(list2, "1=\"Banana\", 2=\"Apple\", 3=\"Orange\", 4=\"Strawberry Shortcake vs. B. Pudding\", 5=\"Grape\","
	// "DROPDOWN=YES, NAME=list2, TIP=Drop, XXX_VALUE=2, XXX_SORT=YES, XXX_BGCOLOR=\"192 64 192\"");
	IupSetAttributes(list2, "1=\"Strawberry Shortcake vs. B. Pudding\","
					                    "DROPDOWN=YES, EDITBOX=YES, TIP=Edit+Drop, NAME=list2");
	//set_callbacks(list2);
	
	
	Ihandle* dialog = IupDialog(list2);
	IupShow(dialog);
	
	return dialog;
	
	
	
}




static int OnButton(Ihandle* button_object)
{
	Ihandle* override_sdk_dialog = (Ihandle*)IupGetAttribute(button_object, "overrideSDKDialog");
	IupDestroy(override_sdk_dialog);
	return IUP_DEFAULT;
}
static int OnDialogClose(Ihandle* the_dialog)
{
	return IUP_DEFAULT;
	
}

static int CreateModalWindow(Ihandle* button_object)
{
#if 0
	Ihandle* button = IupButton("Strawberry Shortcake vs. B. Pudding", "");
	Ihandle* dialog2 = IupDialog(button);
	
	IupSetAttribute(button, "overrideSDKDialog", (const char*)dialog2);
	IupSetCallback(dialog2, "CLOSE_CB", (Icallback)OnDialogClose);
	IupSetCallback(button, "ACTION", (Icallback)OnButton);
	
	IupPopup(dialog2,  IUP_CENTER, IUP_CENTER);
#else
 
	Ihandle* button_makepopup = IupButton("Create Modal Window", "");
	IupSetAttribute(button_makepopup, "EXPAND", "YES");
	IupSetCallback(button_makepopup, "ACTION", (Icallback)CreateModalWindow);
	Ihandle* dialog2 = IupDialog(button_makepopup);
	
	IupPopup(dialog2,  IUP_CENTER, IUP_CENTER);
	
#endif
	return IUP_DEFAULT;
	
}

void GridTest()
{
	Ihandle* main_grid = IupGridBox(NULL);
	
	IupSetAttribute(main_grid, "ORIENTATION", "HORIZONTAL");
	IupSetAttribute(main_grid, "ALIGNMENTLIN", "ACENTER");
	IupSetInt(main_grid, "NUMDIV", 2);
	IupSetInt(main_grid, "SIZECOL", 1);
	//	IupSetInt(main_grid, "GAPCOL", 8);
	//	IupSetInt(main_grid, "GAPLIN", 8);
	IupSetAttribute(main_grid, "HOMOGENEOUSLIN", "YES");
	
	
	
	Ihandle* label1 = IupLabel("Hello World:");
	Ihandle* text_field1 = IupText(NULL);

	IupSetAttribute(label1, "EXPAND", "HORIZONTAL");
	IupSetAttribute(text_field1, "EXPAND", "HORIZONTAL");
	IupSetStrAttribute(text_field1, "CUEBANNER", "Enter here");
	
	IupAppend(main_grid, label1);
	IupAppend(main_grid, text_field1);

	
	
	Ihandle* label2 = IupLabel("Hello World:");
	Ihandle* button1 = IupButton("Some Button", "");

	IupSetAttribute(label2, "EXPAND", "HORIZONTAL");
	IupSetAttribute(button1, "EXPAND", "HORIZONTAL");
	IupAppend(main_grid, label2);
	IupAppend(main_grid, button1);
	
	
#if 0
	
	// make all the right-side fields the same width
	{
		Ihandle* field_normalizer = IupNormalizer(
												  IupGetChild(main_grid, 1),
												  IupGetChild(main_grid, 3),
//												  IupGetChild(main_grid, 0),
//												  IupGetChild(main_grid, 2),
												  NULL
												  );
		IupSetAttributes(field_normalizer, "NORMALIZE=VERTICAL");
		
		IupDestroy(field_normalizer);
		field_normalizer = NULL;
	}
	
#endif
	
	
	
	Ihandle* dialog_simple = IupDialog(main_grid);
	IupShow(dialog_simple);
	
	
	

}

void TextFieldBasicLayoutTest()
{
	IupSetFunction("EXIT_CB", (Icallback)IupExitCallback);
	
	
	Ihandle* button_makepopup = IupButton("Create Modal Window", "");
	//	IupSetAttribute(button_makepopup, "EXPAND", "YES");
	IupSetCallback(button_makepopup, "ACTION", (Icallback)CreateModalWindow);
	
	
	Ihandle* button = IupButton("Strawberry Shortcake vs. B. Pudding", "");
	//	IupSetCallback(button, "ACTION", (Icallback)OnOpenFileSelect);
	IupSetCallback(button, "ACTION", (Icallback)DoSomething);
	
	//	Ihandle* long_label = IupLabel("Hello:");
	Ihandle* long_label = IupLabel("The quick brown fox jumped over the lazy dog.");
	Ihandle* label2 = IupLabel("Hello World.");
	
	//	Ihandle* long_label = IupLabel("The Blurrr SDK could not be found. Please do not move this application from its original location in the SDK. (This is how Blurrr can remain installer-free. If you must proceed, you may enter the path to the Blurrr SDK as an unsupported hack. The Blurrr SDK could not be found. Please do not move this application from its original location in the SDK. (This is how Blurrr can remain installer-free. If you must proceed, you may enter the path to the Blurrr SDK as an unsupported hack.");
	//	IupSetAttribute(long_label, "SIZE", "260x60");
	IupSetAttribute(long_label, "ALIGNMENT", "ACENTER:ACENTER");
	IupSetAttribute(label2, "ALIGNMENT", "ACENTER:ACENTER");
	
	//	IupSetAttribute(long_label, "PADDING", "20x20");
	
	g_someLabel1 = long_label;
	
	Ihandle* text_field = IupText(NULL);
	Ihandle* text_field2 = IupText(NULL);
	
	IupSetStrAttribute(text_field, "VALUE", "Strawberry Shortcake vs. B. Pudding");
	IupSetStrAttribute(text_field, "CUEBANNER", "Enter here");
	IupSetStrAttribute(text_field2, "CUEBANNER", "Enter here");
	IupSetAttribute(text_field, "EXPAND", "HORIZONTAL");
	IupSetAttribute(text_field2, "EXPAND", "HORIZONTAL");
	
	//	IupSetAttribute(text_field, "EXPAND", "YES");
	//	IupSetAttribute(text_field2, "EXPAND", "YES");
	
	Ihandle* label3 = IupLabel("Hello World.");
	IupSetAttribute(label3, "ALIGNMENT", "ACENTER:ACENTER");
	
	Ihandle* text_field3 = IupText(NULL);
	IupSetStrAttribute(text_field, "CUEBANNER", "Enter here");
	
	Ihandle* simple_hb=IupHbox(label3, text_field3, NULL);
	
	
	IupSetAttribute(long_label, "EXPAND", "YES");
	//	IupSetAttribute(button, "EXPAND", "YES");
	//	Ihandle* simple_vb=IupVbox(long_label, button, button_makepopup, NULL);
	//	Ihandle* simple_vb=IupVbox(button, button_makepopup, NULL);
	//	Ihandle* simple_vb=IupVbox(button, text_field, button_makepopup, NULL);
	//	Ihandle* simple_vb=IupVbox(text_field, button, button_makepopup, NULL);
	//	Ihandle* simple_vb=IupVbox(text_field, text_field2, button, button_makepopup, NULL);
	//	Ihandle* simple_vb=IupVbox(button, button_makepopup, text_field, text_field2, NULL);
	//	Ihandle* simple_vb=IupVbox(long_label, label2, button, button_makepopup, text_field, text_field2, simple_hb, NULL);
	Ihandle* simple_vb=IupVbox(long_label, text_field, label2, text_field2, simple_hb, NULL);
	
	//	Ihandle* simple_vb=IupVbox(long_label, NULL);
	//	Ihandle* simple_vb=IupVbox(button, long_label, NULL);
	
	Ihandle* dialog_simple = IupDialog(simple_vb);
	IupShow(dialog_simple);
	//	IupSetAttribute(dialog_simple, "FULLSCREEN", "YES");

	
}

void ListAndModalWindowTest()
{
	IupSetFunction("EXIT_CB", (Icallback)IupExitCallback);
	
	
	Ihandle* button_makepopup = IupButton("Create Modal Window", "");
	//	IupSetAttribute(button_makepopup, "EXPAND", "YES");
	IupSetCallback(button_makepopup, "ACTION", (Icallback)CreateModalWindow);
	
	
	Ihandle* button = IupButton("Strawberry Shortcake vs. B. Pudding", "");
	//	IupSetCallback(button, "ACTION", (Icallback)OnOpenFileSelect);
	IupSetCallback(button, "ACTION", (Icallback)DoSomething);
	
	//	Ihandle* long_label = IupLabel("Hello:");
	Ihandle* long_label = IupLabel("The quick brown fox jumped over the lazy dog.");
	Ihandle* label2 = IupLabel("Hello World.");

	//	Ihandle* long_label = IupLabel("The Blurrr SDK could not be found. Please do not move this application from its original location in the SDK. (This is how Blurrr can remain installer-free. If you must proceed, you may enter the path to the Blurrr SDK as an unsupported hack. The Blurrr SDK could not be found. Please do not move this application from its original location in the SDK. (This is how Blurrr can remain installer-free. If you must proceed, you may enter the path to the Blurrr SDK as an unsupported hack.");
	//	IupSetAttribute(long_label, "SIZE", "260x60");
	IupSetAttribute(long_label, "ALIGNMENT", "ACENTER:ACENTER");
	IupSetAttribute(label2, "ALIGNMENT", "ACENTER:ACENTER");

	//	IupSetAttribute(long_label, "PADDING", "20x20");
	
	g_someLabel1 = long_label;
	
	
	/*
	 Ihandle* about_menu_item = IupItem("About",
	 NULL
	 );
	 Ihandle* menu_file = IupMenu(
	 about_menu_item,
	 NULL
	 );
	 Ihandle* sub_menu_file = IupSubmenu("File", menu_file);
	 Ihandle* menu_bar = IupMenu(
	 sub_menu_file,
	 NULL
	 );
	 IupSetAttributeHandle(dialog_simple, "MENU", menu_bar);
	 */
	
	
	
	//ProgressbarTest();
	Ihandle* list = ListTest();
	
	
	//	Ihandle* vb=IupHbox(button, NULL);
	//IupSetAttribute(vb, "EXPAND", "YES");
	
	//	Ihandle* vb=IupVbox(button, NULL);
	//	Ihandle* vb=IupVbox(list, NULL);
	//	Ihandle* vb=IupVbox(button, list, NULL);
	Ihandle* vb=IupVbox(button_makepopup, list, NULL);
	//	IupSetAttribute(vb, "GAP", "10");
	//	IupSetAttribute(vb, "MARGIN", "10x10");
	IupSetAttribute(vb, "ALIGNMENT", "ACENTER");
	
	
	
	Ihandle* frame1 = IupFrame(vb);
	IupSetAttribute(frame1, "TITLE", "Frame Title");
	IupSetAttribute(frame1, "CLIENTOFFSET", "10x10");
	IupSetAttribute(frame1, "CLIENTSIZE", "10x10");
	
	
	//	Ihandle* dialog = IupDialog(vb);
	Ihandle* dialog = IupDialog(frame1);
	//	Ihandle* dialog = IupDialog(button);
	//	Ihandle* dialog = IupDialog(list);
	
	//IupMap(dialog);
	
	
	 Ihandle* about_menu_item = IupItem("About",
		NULL
	 );
	 Ihandle* menu_file = IupMenu(
		about_menu_item,
		NULL
	 );
	 Ihandle* sub_menu_file = IupSubmenu("File", menu_file);
	 Ihandle* menu_bar = IupMenu(
		sub_menu_file,
	 NULL
	 );
	 IupSetAttributeHandle(dialog, "MENU", menu_bar);
	 
	IupShow(dialog);
	
	IupSetCallback(button_makepopup, "ACTION", (Icallback)CreateModalWindow);
#if 1
	Ihandle* dialog2 = IupDialog(button);
	
	
	IupSetAttribute(button, "overrideSDKDialog", (const char*)dialog2);
	IupSetCallback(dialog2, "CLOSE_CB", (Icallback)OnDialogClose);
	IupSetCallback(button, "ACTION", (Icallback)OnButton);
	
	IupPopup(dialog2,  IUP_CENTER, IUP_CENTER);
	
#endif
}


void ListTextButtonLabelTest()
{
	IupSetFunction("EXIT_CB", (Icallback)IupExitCallback);
	
	
	Ihandle* button_makepopup = IupButton("Create Modal Window", "");
	//	IupSetAttribute(button_makepopup, "EXPAND", "YES");
	IupSetCallback(button_makepopup, "ACTION", (Icallback)CreateModalWindow);
	
	
	Ihandle* button = IupButton("Strawberry Shortcake vs. B. Pudding", "");
	//	IupSetCallback(button, "ACTION", (Icallback)OnOpenFileSelect);
	IupSetCallback(button, "ACTION", (Icallback)DoSomething);
	
	//	Ihandle* long_label = IupLabel("Hello:");
	Ihandle* long_label = IupLabel("The quick brown fox jumped over the lazy dog.");
	Ihandle* label2 = IupLabel("Hello World.");
	
	//	Ihandle* long_label = IupLabel("The Blurrr SDK could not be found. Please do not move this application from its original location in the SDK. (This is how Blurrr can remain installer-free. If you must proceed, you may enter the path to the Blurrr SDK as an unsupported hack. The Blurrr SDK could not be found. Please do not move this application from its original location in the SDK. (This is how Blurrr can remain installer-free. If you must proceed, you may enter the path to the Blurrr SDK as an unsupported hack.");
	//	IupSetAttribute(long_label, "SIZE", "260x60");
	IupSetAttribute(long_label, "ALIGNMENT", "ACENTER:ACENTER");
	IupSetAttribute(label2, "ALIGNMENT", "ACENTER:ACENTER");
	
	//	IupSetAttribute(long_label, "PADDING", "20x20");
	
	g_someLabel1 = long_label;
	
	
	
	Ihandle* text_field1 = IupText(NULL);
	Ihandle* text_field2 = IupText(NULL);
	
	IupSetStrAttribute(text_field1, "VALUE", "Strawberry Shortcake vs. B. Pudding");
	IupSetStrAttribute(text_field1, "CUEBANNER", "Enter here");
	IupSetStrAttribute(text_field2, "CUEBANNER", "Enter here");
	IupSetAttribute(text_field1, "EXPAND", "HORIZONTAL");
	IupSetAttribute(text_field2, "EXPAND", "HORIZONTAL");
	
	
	/*
	 Ihandle* about_menu_item = IupItem("About",
	 NULL
	 );
	 Ihandle* menu_file = IupMenu(
	 about_menu_item,
	 NULL
	 );
	 Ihandle* sub_menu_file = IupSubmenu("File", menu_file);
	 Ihandle* menu_bar = IupMenu(
	 sub_menu_file,
	 NULL
	 );
	 IupSetAttributeHandle(dialog_simple, "MENU", menu_bar);
	 */
	
	
	
	//ProgressbarTest();
	Ihandle* list = ListTest();
	
	
	//	Ihandle* vb=IupHbox(button, NULL);
	//IupSetAttribute(vb, "EXPAND", "YES");
	
	//	Ihandle* vb=IupVbox(button, NULL);
	//	Ihandle* vb=IupVbox(list, NULL);
	//	Ihandle* vb=IupVbox(button, list, NULL);
	Ihandle* vb=IupVbox(button_makepopup, list, text_field1, text_field2, NULL);
	//	IupSetAttribute(vb, "GAP", "10");
	//	IupSetAttribute(vb, "MARGIN", "10x10");
	IupSetAttribute(vb, "ALIGNMENT", "ACENTER");
	
	
	
	Ihandle* frame1 = IupFrame(vb);
	IupSetAttribute(frame1, "TITLE", "Frame Title");
	IupSetAttribute(frame1, "CLIENTOFFSET", "10x10");
	IupSetAttribute(frame1, "CLIENTSIZE", "10x10");
	
	
	//	Ihandle* dialog = IupDialog(vb);
	Ihandle* dialog = IupDialog(frame1);
	//	Ihandle* dialog = IupDialog(button);
	//	Ihandle* dialog = IupDialog(list);
	
	//IupMap(dialog);
	
	/*
	 Ihandle* about_menu_item = IupItem("About",
		NULL
	 );
	 Ihandle* menu_file = IupMenu(
		about_menu_item,
		NULL
	 );
	 Ihandle* sub_menu_file = IupSubmenu("File", menu_file);
	 Ihandle* menu_bar = IupMenu(
		sub_menu_file,
	 NULL
	 );
	 IupSetAttributeHandle(dialog, "MENU", menu_bar);
	 */
	IupShow(dialog);
	
	IupSetCallback(button_makepopup, "ACTION", (Icallback)CreateModalWindow);

	
}


void BasicListTest()
{

	Ihandle* list = ListTest();
	
	
	//	Ihandle* vb=IupHbox(button, NULL);
	//IupSetAttribute(vb, "EXPAND", "YES");
	
	//	Ihandle* vb=IupVbox(button, NULL);
	//	Ihandle* vb=IupVbox(list, NULL);
	//	Ihandle* vb=IupVbox(button, list, NULL);
	Ihandle* vb=IupVbox(list, NULL);
	//	IupSetAttribute(vb, "GAP", "10");
	//	IupSetAttribute(vb, "MARGIN", "10x10");
	//IupSetAttribute(vb, "ALIGNMENT", "ACENTER");
	
	
	
	Ihandle* frame1 = IupFrame(vb);
//	IupSetAttribute(frame1, "TITLE", "Frame Title");
//	IupSetAttribute(frame1, "CLIENTOFFSET", "10x10");
//	IupSetAttribute(frame1, "CLIENTSIZE", "10x10");
	
	
	Ihandle* dialog = IupDialog(frame1);
		IupShow(dialog);
	
	
	
}
static int OnPreferencesCallback(Ihandle* the_object)
{
	Ihandle* label1 = IupLabel("Prefernences placeholder");
	Ihandle* dialog = IupDialog(label1);
	IupShow(dialog);
	
	return IUP_DEFAULT;

}

static int OnQuitMenuCallback(Ihandle* the_object)
{
	return IUP_CLOSE;
}


void AppMenuTest()
{
	Ihandle* label1 = IupLabel("Main menu test");

	//	Ihandle* label1 = IupText(NULL);
//	IupSetStrAttribute(label1, "VALUE", "Main menu test");

//	Ihandle* frame = IupFrame(label1);
	
//	Ihandle* vb = IupVbox(label1, NULL);
	Ihandle* dialog = IupDialog(label1);
//	Ihandle* dialog = IupDialog(vb);
//	Ihandle* dialog = IupDialog(frame);

	

#if __APPLE__
	// NOTE: Apple uses the single ellipsis character, not 3 dots.
	Ihandle* preferences_menu_item = IupItem(
		"Preferences…",
		NULL
	);

#else
	// I have no idea...Windows is Preferences, Settings, Options.
	// On Ubuntu 12.04, the File manager and Firefox use Preferences with n as the shortcut under the Edit menu
	Ihandle* preferences_menu_item = IupItem(
		 "Prefere&nces",
		NULL
	);
	
	
	Ihandle* quit_menu_item = IupItem(
		"Quit",
		NULL
	); 

	IupSetCallback(quit_menu_item, "ACTION", (Icallback)OnQuitMenuCallback);
#endif
	
	IupSetCallback(preferences_menu_item, "ACTION", (Icallback)OnPreferencesCallback);

	Ihandle* dosomething_menu_item = IupItem(
		"Do Something",
		NULL
	);

	

	
#if __APPLE__
	Ihandle* menu_application = IupMenu(
		preferences_menu_item,
		NULL
	);
	Ihandle* menu_file = IupMenu(
		NULL
	);
	Ihandle* menu_edit = IupMenu(
		dosomething_menu_item,
		NULL
	);
#else
	Ihandle* menu_file = IupMenu(
		dosomething_menu_item,
		quit_menu_item,
		NULL
	);
	// Preferences is last on Ubuntu 12.04
	Ihandle* menu_edit = IupMenu(
		dosomething_menu_item,
		preferences_menu_item,
		NULL
	);
#endif

	

#if __APPLE__
	Ihandle* menu_bar = IupMenu(
		IupSubmenu("TestApp", menu_application),
		IupSubmenu("File", menu_file),
		IupSubmenu("Edit", menu_edit),
		NULL
	);
#else
	Ihandle* menu_bar = IupMenu(
		IupSubmenu("File", menu_file),
		IupSubmenu("Edit", menu_edit),
		NULL
	);
#endif
	
//	IupSetAttributeHandle(dialog, "MENU", menu_bar);
	IupSetGlobal("MENU", menu_bar);

	IupShow(dialog);

}

static int FixIt(Ihandle *ih,int but,int pressed,int x,int y,char* status)
{
	
	Ihandle* label_description = (Ihandle*)IupGetAttribute(ih, "labelDescription");
		IupSetAttribute(label_description, "ELLIPSIS", "YES");

	//IupRedraw(label_description, 0);
	return IUP_DEFAULT;
}

void LabelSizeTest()
{
	Ihandle* label_description = NULL;
label_description = IupLabel(
//	"The Blurrr SDK could not be found.\nPlease do not move this application from its original location in the SDK.\n(This is how Blurrr can remain installer-free.)\nIf you must proceed, you may enter the path to the Blurrr SDK as an unsupported hack."

	"The quick brown fox jumped over the lazy dog. The quick brown fox jumped over the lazy dog."
	);
	
	IupSetAttribute(label_description, "WORDWRAP", "YES");
	//	IupSetAttribute(label_description, "EXPAND", "YES");
	IupSetAttribute(label_description, "RASTERSIZE", "200x60");
	
	
//	Ihandle* button_fix = IupButton("Fix it", (const char*)FixIt);
	Ihandle* button_fix = IupButton("Fix it", NULL);
	IupSetCallback(button_fix, "ACTION", (Icallback)FixIt);

	IupSetAttribute(button_fix, "labelDescription", (const char*)label_description);

	
	Ihandle* vbox_for_all = IupVbox(
									label_description,
									button_fix,
									NULL
									);
	
	
//	Ihandle* dialog = IupDialog(label_description);
	Ihandle* dialog = IupDialog(vbox_for_all);
	IupShow(dialog);

	
}

void TextViewTest()
{
	Ihandle* text_display = IupText(NULL);
	IupSetAttribute(text_display, "MULTILINE", "YES");
	IupSetAttribute(text_display, "READONLY", "YES");
	IupSetAttribute(text_display, "EXPAND", "YES");
	IupSetAttribute(text_display, "VISIBLECOLUMNS", "40");
	IupSetAttribute(text_display, "VISIBLELINES", "4");
	//			IupSetAttribute(text_display, "PADDING", "10x10");
	IupSetAttribute(text_display, "SCROLLBAR", "YES");
	IupSetAttribute(text_display, "WORDWRAP", "YES");
	
//	IupSetAttribute(text_display, "SHRINK", "YES");

	//			IupSetAttribute(text_display, "RASTERSIZE", "640x480");
	IupSetStrAttribute(text_display, "VALUE", "The quick brown fox jumped over the lazy dog. The quick brown fox jumped over the lazy dog. The quick brown fox jumped over the lazy dog. The quick brown fox jumped over the lazy dog. The quick brown fox jumped over the lazy dog.");
	
	

	Ihandle* dialog_display = IupDialog(text_display);
//	IupPopup(dialog_display, IUP_CURRENT, IUP_CURRENT);
	IupSetAttribute(dialog_display, "SHRINK", "YES");

	IupShow(dialog_display);

}

void IupExitPoint()
{
	fprintf(stderr, "IupExitPoint\n");
	IupClose();
	
}

void IupEntryPoint()
{
	IupLog(NULL, "Entered IupEntryPoint");
	IupSetFunction("EXIT_CB", (Icallback)IupExitPoint);

	//DropDownListTest();
	EditboxDropDownListTest();
	
	BasicListTest();
//	GridTest();
//	ListAndModalWindowTest();
//	ListTextButtonLabelTest();
//	AppMenuTest();
//	ProgressbarTest2();
	
//	TextTest();

//	DoLocalizedMessageBox();

/*
	SizeTestLabel();
	SizeTestTextField();
	SizeTestButton();
	SizeTestToggle();
	LayoutTestGrid2Label();
	LayoutTestGrid2LabelText();
*/
/*
	LayoutTestGrid3LabelTextButton();
 LayoutTestGrid3LabelTextButtonInFrame();

 LayoutTestGrid3LabelLabelButton();
	LayoutTestGrid3LabelLabelButtonInFrame();
	LayoutTestGrid3LabelTextButton();

*/

//	LabelSizeTest();
	
//	TextViewTest();
	
}



int main(int argc, char* argv[])
{
	int ret_val;
	IupOpen(&argc, &argv);
	
	Ihandle* config_file = IupConfig();
	IupSetStrAttribute(config_file, "APP_NAME", "TestApp");
	ret_val = IupConfigLoad(config_file);
	
	if(ret_val == 0)
	{
		const char* config_value = IupConfigGetVariableStrDef(config_file, "Group1", "Key1", "");
		printf("config value is %s\n", config_value);
	}
	else
	{
		printf("config file not found\n");
	}
	IupConfigSetVariableStr(config_file, "Group1", "Key1", "Value1");
	IupConfigSave(config_file);
	IupDestroy(config_file);
	config_file = NULL;
	
#if 1 // new ENTRY_POINT callback
	IupSetFunction("ENTRY_POINT", (Icallback)IupEntryPoint);
	
	IupMainLoop();
	
#else // legacy
	IupEntryPoint();
	IupMainLoop();
	// legacy: New way should assume IupMainLoop may return immediately or this code is never reached.
	IupClose();
#endif
	
	return 0;
}





