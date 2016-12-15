/** \file
 * \brief GTK Driver keyboard mapping
 *
 * See Copyright Notice in "iup.h"
 */

#import <UIKit/UIKit.h>
//#import <Carbon/HIToolbox/Events.h>

#include <stdlib.h>      
#include <stdio.h>      

#include "iup.h"
#include "iupcbs.h"
#include "iupkey.h"

#include "iup_object.h"
#include "iup_key.h"

#include "iupcocoatouch_drv.h"

#if 0
typedef struct _Imac2iupkey
{
  int maccode;
  int iupcode;
  int s_iupcode;
  int c_iupcode;
  int m_iupcode;
  int y_iupcode;
} Imac2iupkey;

static Imac2iupkey mackey_map[] = {

{ kVK_Escape,    K_ESC,   K_sESC,    K_cESC,   K_mESC   ,K_yESC   },
//{ kVK_Pause,     K_PAUSE, K_sPAUSE,  K_cPAUSE, K_mPAUSE ,K_yPAUSE },
//{ kVK_Print,     K_Print, K_sPrint,  K_cPrint, K_mPrint ,K_yPrint },
//{ kVK_Menu,      K_Menu,  K_sMenu,   K_cMenu,  K_mMenu  ,K_yMenu  },
                                                       
{ kVK_Home,      K_HOME,  K_sHOME,   K_cHOME,  K_mHOME  ,K_yHOME  },
{ kVK_UpArrow,        K_UP,    K_sUP,     K_cUP,    K_mUP    ,K_yUP    },
{ kVK_PageUp,     K_PGUP,  K_sPGUP,   K_cPGUP,  K_mPGUP  ,K_yPGUP  },
{ kVK_LeftArrow,      K_LEFT,  K_sLEFT,   K_cLEFT,  K_mLEFT  ,K_yLEFT  },
//{ kVK_Begin,     K_MIDDLE,K_sMIDDLE, K_cMIDDLE,K_mMIDDLE,K_yMIDDLE},
{ kVK_RightArrow,     K_RIGHT, K_sRIGHT,  K_cRIGHT, K_mRIGHT ,K_yRIGHT },
{ kVK_End,       K_END,   K_sEND,    K_cEND,   K_mEND   ,K_yEND   },
{ kVK_DownArrow,      K_DOWN,  K_sDOWN,   K_cDOWN,  K_mDOWN  ,K_yDOWN  },
{ kVK_PageDown,      K_PGDN,  K_sPGDN,   K_cPGDN,  K_mPGDN  ,K_yPGDN  },
//{ kVK_Insert,    K_INS,   K_sINS,    K_cINS,   K_mINS   ,K_yINS   },
{ kVK_ForwardDelete,    K_DEL,   K_sDEL,    K_cDEL,   K_mDEL   ,K_yDEL   },
{ kVK_Space,     K_SP,    K_sSP,     K_cSP,    K_mSP    ,K_ySP    },
{ kVK_Tab,       K_TAB,   K_sTAB,    K_cTAB,   K_mTAB   ,K_yTAB   },
{ kVK_Return,    K_CR,    K_sCR,     K_cCR,    K_mCR    ,K_yCR    },
{ kVK_Delete, K_BS,    K_sBS,     K_cBS,    K_mBS    ,K_yBS    },

{ '1',   K_1, K_exclam,      K_c1, K_m1, K_y1 },
{ '2',   K_2, K_at,          K_c2, K_m2, K_y2 },
{ '3',   K_3, K_numbersign,  K_c3, K_m3, K_y3 },
{ '4',   K_4, K_dollar,      K_c4, K_m4, K_y4 },
{ '5',   K_5, K_percent,     K_c5, K_m5, K_y5 },
{ '6',   K_6, K_circum,      K_c6, K_m6, K_y6 },
{ '7',   K_7, K_ampersand,   K_c7, K_m7, K_y7 },
{ '8',   K_8, K_asterisk,    K_c8, K_m8, K_y8 },
{ '9',   K_9, K_parentleft,  K_c9, K_m9, K_y9 },
{ '0',   K_0, K_parentright, K_c0, K_m0, K_y0 },


{ 'A',   K_a, K_A, K_cA, K_mA, K_yA },
{ 'B',   K_b, K_B, K_cB, K_mB, K_yB },
{ 'C',   K_c, K_C, K_cC, K_mC, K_yC },
{ 'D',   K_d, K_D, K_cD, K_mD, K_yD },
{ 'E',   K_e, K_E, K_cE, K_mE, K_yE },
{ 'F',   K_f, K_F, K_cF, K_mF, K_yF },
{ 'G',   K_g, K_G, K_cG, K_mG, K_yG },
{ 'H',   K_h, K_H, K_cH, K_mH, K_yH },
{ 'I',   K_i, K_I, K_cI, K_mI, K_yI },
{ 'J',   K_j, K_J, K_cJ, K_mJ, K_yJ },
{ 'K',   K_k, K_K, K_cK, K_mK, K_yK },
{ 'L',   K_l, K_L, K_cL, K_mL, K_yL },
{ 'M',   K_m, K_M, K_cM, K_mM, K_yM },
{ 'N',   K_n, K_N, K_cN, K_mN, K_yN },
{ 'O',   K_o, K_O, K_cO, K_mO, K_yO },
{ 'P',   K_p, K_P, K_cP, K_mP, K_yP },
{ 'Q',   K_q, K_Q, K_cQ, K_mQ, K_yQ },
{ 'R',   K_r, K_R, K_cR, K_mR, K_yR },
{ 'S',   K_s, K_S, K_cS, K_mS, K_yS },
{ 'T',   K_t, K_T, K_cT, K_mT, K_yT },
{ 'U',   K_u, K_U, K_cU, K_mU, K_yU },
{ 'V',   K_v, K_V, K_cV, K_mV, K_yV },
{ 'W',   K_w, K_W, K_cW, K_mW, K_yW },
{ 'X',   K_x, K_X, K_cX, K_mX, K_yX },
{ 'Y',   K_y, K_Y, K_cY, K_mY, K_yY },
{ 'Z',   K_z, K_Z, K_cZ, K_mZ, K_yZ },


{ kVK_F1,  K_F1,  K_sF1,  K_cF1,  K_mF1,  K_yF1 },
{ kVK_F2,  K_F2,  K_sF2,  K_cF2,  K_mF2,  K_yF2 },
{ kVK_F3,  K_F3,  K_sF3,  K_cF3,  K_mF3,  K_yF3 },
{ kVK_F4,  K_F4,  K_sF4,  K_cF4,  K_mF4,  K_yF4 },
{ kVK_F5,  K_F5,  K_sF5,  K_cF5,  K_mF5,  K_yF5 },
{ kVK_F6,  K_F6,  K_sF6,  K_cF6,  K_mF6,  K_yF6 },
{ kVK_F7,  K_F7,  K_sF7,  K_cF7,  K_mF7,  K_yF7 },
{ kVK_F8,  K_F8,  K_sF8,  K_cF8,  K_mF8,  K_yF8 },
{ kVK_F9,  K_F9,  K_sF9,  K_cF9,  K_mF9,  K_yF9 },
{ kVK_F10, K_F10, K_sF10, K_cF10, K_mF10, K_yF10 },
{ kVK_F11, K_F11, K_sF11, K_cF11, K_mF11, K_yF11 },
{ kVK_F12, K_F12, K_sF12, K_cF12, K_mF12, K_yF12 },

{ ';',   K_semicolon,    K_colon,      K_cSemicolon, K_mSemicolon, K_ySemicolon },
{ '=',       K_equal,        K_plus,       K_cEqual,     K_mEqual,  K_yEqual },
{ ',',       K_comma,        K_less,       K_cComma,     K_mComma,  K_yComma },
{ '-',       K_minus,        K_underscore, K_cMinus,     K_mMinus,  K_yMinus },
{ '.',      K_period,       K_greater,    K_cPeriod,    K_mPeriod, K_yPeriod },
{ '/',       K_slash,        K_question,   K_cSlash,     K_mSlash,  K_ySlash },
{ '`',       K_grave,        K_tilde,      0, 0, 0 },
{ '[', K_bracketleft,  K_braceleft,  K_cBracketleft, K_mBracketleft, K_yBracketleft },
{ '\\',   K_backslash,    K_bar,        K_cBackslash,   K_mBackslash,   K_yBackslash },
{ ']',K_bracketright, K_braceright, K_cBracketright,K_mBracketright,K_yBracketright },
{ '\'',  K_apostrophe,   K_quotedbl,   0, 0, 0 },
/*
{ '0',   K_0, K_0,  K_c0, K_m0, K_y0 },
{ '1',   K_1, K_1,  K_c1, K_m1, K_y1 },
{ '2',   K_2, K_2,  K_c2, K_m2, K_y2 },
{ '3',   K_3, K_3,  K_c3, K_m3, K_y3 },
{ '4',   K_4, K_4,  K_c4, K_m4, K_y4 },
{ '5',   K_5, K_5,  K_c5, K_m5, K_y5 },
{ '6',   K_6, K_6,  K_c6, K_m6, K_y6 },
{ '7',   K_7, K_7,  K_c7, K_m7, K_y7 },
{ '8',   K_8, K_8,  K_c8, K_m8, K_y8 },
{ '9',   K_9, K_9,  K_c9, K_m9, K_y9 },
{ '*',  K_asterisk, K_sAsterisk, K_cAsterisk, K_mAsterisk, K_yAsterisk },
{ '+',       K_plus,     K_sPlus,     K_cPlus,     K_mPlus,     K_yPlus },
{ '-',  K_minus,    K_sMinus,    K_cMinus,    K_mMinus,    K_yMinus },
{ '.',   K_period,   K_sPeriod,   K_cPeriod,   K_mPeriod,   K_yPeriod },
{ '/',    K_slash,    K_sSlash,    K_cSlash,    K_mSlash,    K_ySlash },
{ ',', K_comma,    K_sComma,    K_cComma,    K_mComma,    K_yComma },

{ kVK_ccedilla,     K_ccedilla, K_Ccedilla, K_cCcedilla, K_mCcedilla, K_yCcedilla },
{ kVK_Ccedilla,     K_ccedilla, K_Ccedilla, K_cCcedilla, K_mCcedilla, K_yCcedilla },

{ kVK_dead_tilde,      K_tilde, K_circum, 0, 0, 0 },
{ kVK_dead_acute,      K_acute, K_grave,  0, 0, 0 },
{ kVK_dead_grave,      K_grave, K_tilde,  0, 0, 0 },
{ kVK_dead_circumflex, K_tilde, K_circum, 0, 0, 0 },

{ kVK_KP_F1,        K_F1,    K_sF1,    K_cF1,    K_mF1,    K_yF1 },
{ kVK_KP_F2,        K_F2,    K_sF2,    K_cF2,    K_mF2,    K_yF2 },
{ kVK_KP_F3,        K_F3,    K_sF3,    K_cF3,    K_mF3,    K_yF3 },
{ kVK_KP_F4,        K_F4,    K_sF4,    K_cF4,    K_mF4,    K_yF4 },
{ kVK_KP_Space,     K_SP,    K_sSP,    K_cSP,    K_mSP    ,K_ySP    },
{ kVK_KP_Tab,       K_TAB,   K_sTAB,   K_cTAB,   K_mTAB   ,K_yTAB   },
{ kVK_KP_Equal,     K_equal, 0,        K_cEqual, K_mEqual, K_yEqual },

{ kVK_KP_Enter,     K_CR,    K_sCR,     K_cCR,    K_mCR,    K_yCR    },
{ kVK_KP_Home,      K_HOME,  K_sHOME,   K_cHOME,  K_mHOME,  K_yHOME  },
{ kVK_KP_Up,        K_UP,    K_sUP,     K_cUP,    K_mUP,    K_yUP    },
{ kVK_KP_Page_Up,   K_PGUP,  K_sPGUP,   K_cPGUP,  K_mPGUP,  K_yPGUP  },
{ kVK_KP_Left,      K_LEFT,  K_sLEFT,   K_cLEFT,  K_mLEFT,  K_yLEFT  },
{ kVK_KP_Begin,     K_MIDDLE,K_sMIDDLE, K_cMIDDLE,K_mMIDDLE,K_yMIDDLE},
{ kVK_KP_Right,     K_RIGHT, K_sRIGHT,  K_cRIGHT, K_mRIGHT, K_yRIGHT },
{ kVK_KP_End,       K_END,   K_sEND,    K_cEND,   K_mEND,   K_yEND   },
{ kVK_KP_Down,      K_DOWN,  K_sDOWN,   K_cDOWN,  K_mDOWN,  K_yDOWN  },
{ kVK_KP_Page_Down, K_PGDN,  K_sPGDN,   K_cPGDN,  K_mPGDN,  K_yPGDN  },
{ kVK_KP_Insert,    K_INS,   K_sINS,    K_cINS,   K_mINS,   K_yINS   },
{ kVK_KP_Delete,    K_DEL,   K_sDEL,    K_cDEL,   K_mDEL,   K_yDEL   }
*/
};

void iupmacKeyEncode(int key, guint *keyval, guint *state)
{
  int i, iupcode = key & 0xFF; /* 0-255 interval */
  int count = sizeof(mackey_map)/sizeof(mackey_map[0]);
  for (i = 0; i < count; i++)
  {
    Imac2iupkey* key_map = &(mackey_map[i]);
    if (key_map->iupcode == iupcode)
    {
      *keyval = key_map->maccode;
      *state = 0;

      if (iupcode != key)
      {
        if (key_map->c_iupcode == key)
          *state = kVK_Control;
        else if (key_map->m_iupcode == key)
          *state = kVK_Option;
        else if (key_map->y_iupcode == key)
          *state = kVK_Command;
        else if (key_map->s_iupcode == key)
          *state = kVK_Shift;
      }
      return;
    }
    else if (key_map->s_iupcode == key)   /* There are Shift keys bellow 256 */
    {
      *keyval = key_map->maccode;
      *state = kVK_Shift;
      return;
    }
  }
}

static int macKeyMap2Iup(int state, int i)
{
  int code = 0;
  if (state & kVK_Control)   /* Ctrl */
    code = mackey_map[i].c_iupcode;
  else if (state & kVK_Option ||
           state & kVK_RightOption) /* Alt */
    code = mackey_map[i].m_iupcode;
  else if (state & kVK_Command) /* Apple/Win */
    code = mackey_map[i].y_iupcode;
  else if (state & kVK_CapsLock) /* CapsLock */
  {
    if ((state & kVK_Shift) || !iupKeyCanCaps(mackey_map[i].iupcode))
      return mackey_map[i].iupcode;
    else
      code = mackey_map[i].s_iupcode;
  }
  else if (state & kVK_Shift) /* Shift */
    code = mackey_map[i].s_iupcode;
  else
    return mackey_map[i].iupcode;

  if (!code)
    code = mackey_map[i].iupcode;

  return code;
}

static int macKeyDecode(int maccode)
{
  int i;
  int count = sizeof(mackey_map)/sizeof(mackey_map[0]);

  for (i = 0; i < count; i++)
  {
    if (mackey_map[i].maccode == maccode)
      return macKeyMap2Iup(evt->state, i);
  }

  return 0;
}

int iupmacKeyEvent(Ihandle *ih, int maccode, int press)
{
  int result,code;
	
  if (!ih->iclass->is_interactive)
	return FALSE;
	
  code = macKeyDecode(maccode);
  if (code == 0) 
    return FALSE;

  result = iupKeyCallKeyCb(ih, code);
  if (result == IUP_CLOSE)
  {
    IupExitLoop();
    return FALSE;
  }
  if (result == IUP_IGNORE)
    return TRUE;

  /* in the previous callback the dialog could be destroyed */
  if (iupObjectCheck(ih))
  {
    /* this is called only for canvas */
    if (ih->iclass->nativetype == IUP_TYPECANVAS) 
    {
      result = iupKeyCallKeyPressCb(ih, code, 1);
      if (result == IUP_CLOSE)
      {
        IupExitLoop();
        return FALSE;
      }
      if (result == IUP_IGNORE)
        return TRUE;
    }

    if (!iupKeyProcessNavigation(ih, code, evt->state & kVK_SHIFT_MASK))
      return TRUE;

    /* compensate the show-help limitation. 
     * It is not called on F1, only on Shift+F1 and Ctrl+F1. */
    if (code == K_F1)
    {
      Icallback cb = IupGetCallback(ih, "HELP_CB");
      if (cb)
      {
        if (cb(ih) == IUP_CLOSE) 
          IupExitLoop();
      }
    }
  }

  (void)widget;
  return FALSE;
}
#endif

void iupmacButtonKeySetStatus(int keys, int but, char* status, int doubleclick)
{
/*
	CGEventFlags flags =CGEventSourceFlagsState(kCGEventSourceStateCombinedSessionState);
	if (flags & kCGEventFlagMaskShift)
		iupKEYSETSHIFT(status);
	
	if (flags & kCGEventFlagMaskControl)
		iupKEYSETCONTROL(status); 
	
	if (CGEventSourceButtonState(kCGEventSourceStateCombinedSessionState,kCGMouseButtonLeft))
		iupKEYSETBUTTON1(status);
	
	if (CGEventSourceButtonState(kCGEventSourceStateCombinedSessionState,kCGMouseButtonLeft))
		iupKEYSETBUTTON2(status);
	
	if (CGEventSourceButtonState(kCGEventSourceStateCombinedSessionState,kCGMouseButtonRight))
		iupKEYSETBUTTON3(status);
	
	if (doubleclick)
		iupKEYSETDOUBLE(status);
	
	if (CGEventSourceKeyState(kCGEventSourceStateCombinedSessionState,kVK_Option))
		iupKEYSETALT(status);
	
	if (CGEventSourceKeyState(kCGEventSourceStateCombinedSessionState,kVK_Command))
		iupKEYSETSYS(status);

 */
}

