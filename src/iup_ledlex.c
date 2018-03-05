/** \file
 * \brief lexical analysis manager for LED
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdarg.h>  
#include <stdio.h>   
#include <stdlib.h>  
#include <ctype.h>
#include <string.h>  
#include <errno.h>   

#include "iup.h"

#include "iup_class.h"
#include "iup_ledlex.h"
#include "iup_str.h"
#include "iup_table.h"
#include "iup_register.h"


static struct          /* lexical variables */
{
  const char* filename;   /* file name */
  const char* f;
  FILE* file;             /* file handle */
  int token;              /* lookahead iLexToken */
  char name[40960];       /* lexical identifier value */
  float number;           /* lexical number value */
  int line;               /* line number */
  Iclass *ic;             /* control class when func is CONTROL_ */
} ilex = {NULL, NULL, NULL, 0, "", (float) 0.0, 0, NULL};

static int iLexGetChar (void);
static int iLexToken(int *erro);
static int iLexCapture (char* dlm);
static void iLexSkipComment (void);
static int iLexCaptureAttr (void);

int iupLexStart(const char* filename, int is_file)      /* initialize lexical analysis */
{
  ilex.filename = filename;
  if (is_file)
  {
    ilex.file = fopen (ilex.filename,"r");
    if (!ilex.file)
      return iupLexError (IUPLEX_FILENOTOPENED, filename);
  }
  else
  {
    ilex.f = ilex.filename;
    ilex.file = NULL;
  }
  ilex.line = 0;
  ilex.line = 1;
  return iupLexAdvance();
}

void iupLexClose(void)
{
  if (!ilex.file)
    return;
  fclose (ilex.file);
  ilex.file = NULL;
}

static void iLexUngetc(int c)
{
  if (ilex.file)
    ungetc(c, ilex.file);
  else
  {
    if (c != EOF && ilex.filename < ilex.f)
      ilex.f--;
  }
}

static int iLexGetc(void)
{
  if (ilex.file)
    return getc(ilex.file);
  else
  {
    if (*(ilex.f) == 0)
      return EOF;
    ilex.f++;
    return *(ilex.f - 1);
  }
}

int iupLexLookAhead(void)
{
  return ilex.token;
}

int iupLexAdvance(void)
{
  int erro = 0;
  ilex.token = iLexToken(&erro);
  return erro;
}

int iupLexFollowedBy(int t)
{
  return (ilex.token==t);
}

int iupLexMatch(int t)
{
  if (ilex.token==t)
    return iupLexAdvance();
  else
    return iupLexError (IUPLEX_NOTMATCH, ilex.token, t);
}


int iupLexSeenMatch(int t, int *erro)
{
  if (ilex.token==t)
  {
    *erro = iupLexAdvance();
    return 1;
  }
  else
    return 0;
}

unsigned char iupLexByte(void)
{
  unsigned int b;
  sscanf(ilex.name,"%u", &b);  /* read as integer to avoid reading number as characters */
  if (b>255) b = 255;
  return (unsigned char)b;
}

int iupLexInt(void)
{
  int i;
  sscanf(ilex.name,"%d", &i);
  return i;
}

float iupLexFloat(void)
{
  float f;
  sscanf(ilex.name,"%f", &f);
  return f;
}

char* iupLexGetName(void)
{
  return iupStrDup(ilex.name);
}

float iupLexGetNumber(void)
{
  return ilex.number;
}

Iclass *iupLexGetClass(void)
{
  return ilex.ic;
}

static int iLexToken(int *erro)
{
  for (;;)
  {
    int c = iLexGetChar();
    switch (c)
    {
    case 26:
    case EOF:
      return IUPLEX_TK_END;

    case ']':
      return IUPLEX_TK_ENDATTR;

    case '#':          /* Skip comment */
    case '%':          /* Skip comment */
      iLexSkipComment();
      continue;

    case ' ':          /* ignore whitespace */
    case '\t':
    case '\n':
    case '\r':
    case '\f':
    case '\v':
      continue;

    case '=':          /* attribuicao */
      return IUPLEX_TK_SET;

    case ',':
      return IUPLEX_TK_COMMA;

    case '(':          /* begin parameters */
      return IUPLEX_TK_BEGP;

    case ')':          /* end parameters */
      return IUPLEX_TK_ENDP;

    case '[':          /* attributes */
      if (iLexCaptureAttr() == IUPLEX_TK_END)
      {
        *erro=iupLexError (IUPLEX_NOTENDATTR);
        return 0;
      }
      return IUPLEX_TK_ATTR;

    case '\"':          /* string */
      iLexCapture ("\"");
      return IUPLEX_TK_STR;

    case '\'':          /* string */
      iLexCapture ("\'");
      return IUPLEX_TK_STR;

    default:
      if (c > 32)          /* identifier */
      {
        char class_name[50];
        iLexUngetc(c);
        iLexUngetc(iLexCapture ("=[](), \t\n\r\f\v"));
        iupStrLower(class_name, ilex.name);
        ilex.ic = iupRegisterFindClass(class_name);
        if (ilex.ic)
          return IUPLEX_TK_FUNC;
        else
          return IUPLEX_TK_NAME;
      }
    }
    return c;
  }
}

static int iLexCapture (char* dlm)
{
  int i=0;
  int c;
  do
  {
    c = iLexGetChar ();
    if (i < sizeof(ilex.name))
      ilex.name[i++] = (char) c;
  } while ((c > 0) && !strchr (dlm,c));
  ilex.name[i-1]='\0';                            /* discard delimiter */
  return c;                                      /* return delimiter */
}

static int iLexCaptureAttr (void)
{
  int i=0;
  int c;
  int aspas=0;
  do
  {
    c = iLexGetChar ();
    if (i < sizeof(ilex.name))
      ilex.name[i++] = (char) c;
    if (c == '"')
      ++aspas;
  } while ((c > 0) && ((aspas & 1) || c != ']'));
  ilex.name[i-1]='\0';                            /* discard delimiter */
  return c;                                      /* return delimiter */
}

static void iLexSkipComment (void)
{
  int c;
  do
  {
    c = iLexGetChar();
  } while ((c > 0) && (c != '\n'));
}

static int iLexGetChar (void)
{
  int c = iLexGetc(); if (c == '\n') ++ilex.line;
  if (c == '\\')
  {
    c = iLexGetc();
    if (c == 'n')
      return '\n';
    else if (c == '\\')
      return '\\';
  }
  return c;
}

static char* iupTokenStr(int t)
{
  switch (t)
  {
  case IUPLEX_TK_END  : return "end of file";
  case IUPLEX_TK_BEGP : return "(";
  case IUPLEX_TK_ENDP : return ")";
  case IUPLEX_TK_ATTR : return "[";
  case IUPLEX_TK_STR  : return "string";
  case IUPLEX_TK_NAME : return "identifier";
  case IUPLEX_TK_NUMB : return "number";
  case IUPLEX_TK_SET  : return "=";
  case IUPLEX_TK_COMMA: return ",";
  case IUPLEX_TK_FUNC : return "function";
  case IUPLEX_TK_ENDATTR : return "]";
  }
  return "";
}

static char ilex_erromsg[10240];

char *iupLexGetError(void)
{
  return ilex_erromsg;
}

int iupLexError (int n, ...)
{
  char msg[10240];
  va_list va;
  va_start(va,n);
  switch (n)
  {
  case IUPLEX_FILENOTOPENED:
    {
      char *fn=va_arg(va,char *);
      sprintf (msg, "cannot open file %s", fn);
    }
    break;
  case IUPLEX_NOTMATCH:
    {
      int tr=va_arg(va,int);        /* iLexToken read */
      int te=va_arg(va,int);        /* iLexToken expected */
      char *str=iupTokenStr(tr);
      char *ste=iupTokenStr(te);
      sprintf (msg, "expected %s but found %s", ste, str);
    }
    break;
  case IUPLEX_NOTENDATTR:
    {
      sprintf (msg, "missing ']'");
    }
    break;
  case IUPLEX_PARSEERROR:
    {
      char* s=va_arg(va,char*);        /* iLexToken expected */
      sprintf(msg,"%.*s",(int)(sizeof(msg)-1),s);
    }
    break;
  }
  va_end(va);
  sprintf(ilex_erromsg, "led(%s): bad input at line %d - %s\n", ilex.filename, ilex.line, msg);
  return n;
}
