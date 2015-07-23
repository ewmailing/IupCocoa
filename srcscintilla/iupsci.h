/** \file
 * \brief Scintilla control.
 *
 * See Copyright Notice in "iup.h"
 */
 
#ifndef __IUPSCI_H 
#define __IUPSCI_H

#ifdef __cplusplus
extern "C" {
#endif

long iupScintillaEncodeColor(unsigned char r, unsigned char g, unsigned char b);
void iupScintillaDecodeColor(long color, unsigned char *r, unsigned char *g, unsigned char *b);
void iupScintillaConvertLinColToPos(Ihandle* ih, int lin, int col, int *pos);
void iupScintillaConvertPosToLinCol(Ihandle* ih, int pos, int *lin, int *col);

void iupScintillaRegisterText(Iclass* ic);
void iupScintillaRegisterSelection(Iclass* ic);
void iupScintillaRegisterClipboard(Iclass* ic);
void iupScintillaRegisterOvertype(Iclass* ic);
void iupScintillaRegisterTab(Iclass* ic);
void iupScintillaRegisterWordWrap(Iclass* ic);
void iupScintillaRegisterStyle(Iclass* ic);
void iupScintillaRegisterLexer(Iclass* ic);
void iupScintillaRegisterFolding(Iclass* ic);
void iupScintillaRegisterMargin(Iclass* ic);
void iupScintillaRegisterMarker(Iclass* ic);
void iupScintillaRegisterWhiteSpace(Iclass* ic);
void iupScintillaRegisterBraceLight(Iclass* ic);
void iupScintillaRegisterCursor(Iclass* ic);
void iupScintillaRegisterAnnotation(Iclass* ic);
void iupScintillaRegisterScrolling(Iclass* ic);
void iupScintillaRegisterAutocompletion(Iclass* ic);
void iupScintillaRegisterSearching(Iclass* ic);

/* this function is also exported in the DLL */
sptr_t IupScintillaSendMessage(Ihandle* ih, unsigned int iMessage, uptr_t wParam, sptr_t lParam);

enum {IUP_SCINTILLA_PLAIN, IUP_SCINTILLA_SQUIGGLE, IUP_SCINTILLA_TT, IUP_SCINTILLA_DIAGONAL, IUP_SCINTILLA_STRIKE, IUP_SCINTILLA_HIDDEN, IUP_SCINTILLA_BOX, IUP_SCINTILLA_ROUNDBOX, IUP_SCINTILLA_STRAIGHTBOX, IUP_SCINTILLA_DASH, IUP_SCINTILLA_DOTS, IUP_SCINTILLA_SQUIGGLELOW, IUP_SCINTILLA_DOTBOX, IUP_SCINTILLA_SQUIGGLEPIXMAP};

struct _IcontrolData
{
  int sb;
  int append_newline;
  int last_caret_pos;
  int ignore_change;

  /* unused for now */
  int useBraceHLIndicator;
  int useBraceBLIndicator;
};


#ifdef __cplusplus
}
#endif

#endif
