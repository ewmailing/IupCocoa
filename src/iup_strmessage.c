/** \file
 * \brief String Utilities
 *
 * See Copyright Notice in "iup.h"
 */

 
#include <string.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <limits.h>  
#include <stdarg.h>

#include "iup.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_strmessage.h"
#include "iup_table.h"
#include "iup_register.h"


static void iStrMessageRegisterInternal(int lng, int utf8mode);

static Itable *istrmessage_table = NULL;   /* the message hash table indexed by the name string */

void iupStrMessageInit(void)
{
  istrmessage_table = iupTableCreate(IUPTABLE_STRINGINDEXED);

  iStrMessageRegisterInternal(0, 0);
}

void iupStrMessageFinish(void)
{
  iupTableDestroy(istrmessage_table);
  istrmessage_table = NULL;
}

char* IupGetLanguageString(const char* name)
{
  char* value;
  if (!name) return NULL;
  value = (char*)iupTableGet(istrmessage_table, name);
  if (!value)
    return (char*)name;
  return value;
}

void IupSetLanguageString(const char* name, const char* str)
{
  iupTableSet(istrmessage_table, name, (char*)str, IUPTABLE_POINTER);
}

void IupStoreLanguageString(const char* name, const char* str)
{
  iupTableSet(istrmessage_table, name, (char*)str, IUPTABLE_STRING);
}

void IupSetLanguagePack(Ihandle* ih)
{
  if (!ih)
    iupTableClear(istrmessage_table);
  else
  {
    char *name, *value;

    name = iupTableFirst(ih->attrib);
    while (name)
    {
      value = (char*)iupTableGetCurr(ih->attrib);

      if (iupTableGetCurrType(ih->attrib)==IUPTABLE_STRING)
        iupTableSet(istrmessage_table, name, value, IUPTABLE_STRING);
      else
        iupTableSet(istrmessage_table, name, value, IUPTABLE_POINTER);

      name = iupTableNext(ih->attrib);
    }
  }
}

void IupSetLanguage(const char *language)
{
  IupStoreGlobal("LANGUAGE", language);
}

char *IupGetLanguage(void)
{
  return IupGetGlobal("LANGUAGE");
}


/**********************************************************************************/


#define ISRTMSG_NUM_LNG 4    /* 3+1 for expansion */
                             /* ENGLISH, PORTUGUESE, PORTUGUESE(UTF-8), NULL */

typedef struct _IstdMessage
{
  const char* name;
  const char* lng_str[ISRTMSG_NUM_LNG];
} IstdMessage;


/* When seeing this file assuming ISO8859-1 encoding, lng=1 will appear correct.
   When seeing this file assuming UTF-8 encoding, lng=2 will appear correct. */

static IstdMessage iStdMessages[] =
{
  {"IUP_ERROR", {"Error", "Erro", NULL, NULL}},
  {"IUP_YES", {"Yes", "Sim", NULL, NULL}},
  {"IUP_NO", {"No", "Não", "NÃ£o", NULL}},
  {"IUP_INVALIDDIR", {"Invalid directory.", "Diretório inválido.", "DiretÃ³rio invÃ¡lido.", NULL}},
  {"IUP_FILEISDIR", {"The selected name is a directory.", "O nome selecionado é um diretório.", "O nome selecionado Ã© um diretÃ³rio.", NULL}},
  {"IUP_FILENOTEXIST", {"File does not exist.", "Arquivo inexistente.", NULL, NULL}},
  {"IUP_FILEOVERWRITE", {"Overwrite existing file?", "Sobrescrever arquivo?", NULL, NULL}},
  {"IUP_CREATEFOLDER", {"Create Folder", "Criar Diretório", "Criar DiretÃ³rio", NULL}},
  {"IUP_NAMENEWFOLDER", {"Name of the new folder:", "Nome do novo diretório:", "Nome do novo diretÃ³rio:", NULL}},
  {"IUP_SAVEAS", {"Save As", "Salvar Como", NULL, NULL}},
  {"IUP_OPEN", {"Open", "Abrir", NULL, NULL}},
  {"IUP_SELECTDIR", {"Select Directory", "Selecionar Diretório", "Selecionar DiretÃ³rio", NULL}},
  {"IUP_OK", {"OK", "OK", NULL, NULL}},
  {"IUP_CANCEL", {"Cancel", "Cancelar", NULL, NULL}},
  {"IUP_RETRY", {"Retry", "Tentar Novamente", NULL, NULL}},
  {"IUP_APPLY", {"Apply", "Aplicar", NULL, NULL}},
  {"IUP_RESET", {"Reset", "Reinicializar", NULL, NULL}},
  {"IUP_GETCOLOR", {"Color Selection", "Seleção de Cor", "SeleÃ§Ã£o de Cor", NULL}},
  {"IUP_HELP", {"Help", "Ajuda", NULL, NULL}},
  {"IUP_RED", {"&Red:", "&Vermelho:", NULL, NULL}},
  {"IUP_GREEN", {"&Green:", "V&erde:", NULL, NULL}},
  {"IUP_BLUE", {"&Blue:", "&Azul:", NULL, NULL}},
  {"IUP_HUE", {"&Hue:", "&Matiz:", NULL, NULL}},
  {"IUP_SATURATION", {"&Saturation:", "&Saturação:", "&SaturaÃ§Ã£o:", NULL}},
  {"IUP_INTENSITY", {"&Intensity:", NULL, "&Intensidade:", NULL}},
  {"IUP_OPACITY", {"&Opacity:", "&Opacidade:", NULL, NULL}},
  {"IUP_PALETTE", {"&Palette:", "&Paleta:", NULL, NULL}},
  {"IUP_TRUE", {"True", "Verdadeiro", NULL, NULL}},
  {"IUP_FALSE", {"False", "Falso", NULL, NULL}},
  {"IUP_FAMILY", {"Family:", "Família:", "FamÃ­lia:", NULL}},
  {"IUP_STYLE", {"Style:", "Estilo:", NULL, NULL}},
  {"IUP_SIZE", {"Size:", "Tamanho:", NULL, NULL}},
  {"IUP_SAMPLE", {"Sample:", "Exemplo:", NULL, NULL}},
  {NULL, {NULL, NULL, NULL}}
};

static void iStrMessageRegisterInternal(int lng, int utf8mode)
{
  IstdMessage* messages = iStdMessages;
  while (messages->name)
  {
    if (utf8mode && !(messages->lng_str[lng]))
      IupSetLanguageString(messages->name, messages->lng_str[lng-1]);
    else
      IupSetLanguageString(messages->name, messages->lng_str[lng]);
    messages++;
  }
}


void iupStrMessageUpdateLanguage(const char* language)
{
  /* called after the global attribute is changed */

  int lng = 0;  /* ENGLISH */
  int utf8mode = IupGetInt(NULL, "UTF8MODE");
  if (iupStrEqualNoCase(language, "PORTUGUESE"))
  {
    if (utf8mode)
      lng = 2;
    else
      lng = 1;
  }

  iStrMessageRegisterInternal(lng, utf8mode);
  iupRegisterUpdateClasses();
}
