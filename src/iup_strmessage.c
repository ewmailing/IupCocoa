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


#define ISRTMSG_NUM_LNG 6    /* 5+1 for expansion */
                             /* ENGLISH, PORTUGUESE, PORTUGUESE(UTF-8), SPANISH, SPANISH(UTF-8), NULL */

typedef struct _IstdMessage
{
  const char* name;
  const char* lng_str[ISRTMSG_NUM_LNG];
} IstdMessage;


/* When seeing this file assuming ISO8859-1 encoding, lng=1 will appear correct.
   When seeing this file assuming UTF-8 encoding, lng=2 will appear correct. */

static IstdMessage iStdMessages[] =
{
  {"IUP_ERROR", {"Error!", "Erro!", NULL, "Error!", NULL, NULL}},
  {"IUP_ATTENTION", {"Attention!", "Atenção!", "AtenÃ§Ã£o!", "¡Advertencia!", "Â¡Advertencia!", NULL}},
  {"IUP_YES", {"Yes", "Sim", NULL, "Si", NULL, NULL}},
  {"IUP_NO", {"No", "Não", "NÃ£o", "No", NULL, NULL}},
  {"IUP_INVALIDDIR", {"Invalid directory.", "Diretório inválido.", "DiretÃ³rio invÃ¡lido.", "Directorio inválido.", "Directorio invÃ¡lido.", NULL}},
  {"IUP_FILEISDIR", {"The selected name is a directory.", "O nome selecionado é um diretório.", "O nome selecionado Ã© um diretÃ³rio.", "El nombre seleccionado es un directorio.", NULL, NULL}},
  {"IUP_FILENOTEXIST", {"File does not exist.", "Arquivo inexistente.", NULL, "Archivo inexistente.", NULL, NULL}},
  {"IUP_FILEOVERWRITE", {"Overwrite existing file?", "Sobrescrever arquivo?", NULL, "¿Sobrescribir archivo?", "Â¿Sobrescribir archivo?", NULL}},
  {"IUP_CREATEFOLDER", {"Create Folder", "Criar Diretório", "Criar DiretÃ³rio", "Crear Directorio", NULL, NULL}},
  {"IUP_NAMENEWFOLDER", {"Name of the new folder:", "Nome do novo diretório:", "Nome do novo diretÃ³rio:", "Nombre del nuevo directorio:", NULL, NULL}},
  {"IUP_SAVEAS", {"Save As", "Salvar Como", NULL, "Guardar Como", NULL, NULL}},
  {"IUP_OPEN", {"Open", "Abrir", NULL, "Abrir", NULL, NULL}},
  {"IUP_SELECTDIR", {"Select Directory", "Selecionar Diretório", "Selecionar DiretÃ³rio", "Seleccionar Directorio", NULL, NULL}},
  {"IUP_OK", {"OK", "OK", NULL, "Aceptar", NULL, NULL}},
  {"IUP_CANCEL", {"Cancel", "Cancelar", NULL, "Cancelar", NULL, NULL}},
  {"IUP_RETRY", {"Retry", "Tentar Novamente", NULL, "Reintentar", NULL, NULL}},
  {"IUP_APPLY", {"Apply", "Aplicar", NULL, "Aplicar", NULL, NULL}},
  {"IUP_RESET", {"Reset", "Reinicializar", NULL, "Reiniciar", NULL, NULL}},
  {"IUP_GETCOLOR", {"Color Selection", "Seleção de Cor", "SeleÃ§Ã£o de Cor", "Selección de Color", "SelecciÃ³n de Color", NULL}},
  {"IUP_HELP", {"Help", "Ajuda", NULL, "Ayuda", NULL, NULL}},
  {"IUP_RED", {"&Red:", "&Vermelho:", NULL, "&Rojo:", NULL, NULL}},
  {"IUP_GREEN", {"&Green:", "V&erde:", NULL, "&Verde:", NULL, NULL}},
  {"IUP_BLUE", {"&Blue:", "&Azul:", NULL, "&Azul:", NULL, NULL}},
  {"IUP_HUE", {"&Hue:", "&Matiz:", NULL, "&Matiz:", NULL, NULL}},
  {"IUP_SATURATION", {"&Saturation:", "&Saturação:", "&SaturaÃ§Ã£o:", "&Saturación:", "&SaturaciÃ³n:", NULL}},
  {"IUP_INTENSITY", {"&Intensity:", NULL, "&Intensidade:", "&Intensidad:", NULL, NULL}},
  {"IUP_OPACITY", {"&Opacity:", "&Opacidade:", NULL, "&Opacidad:", NULL, NULL}},
  {"IUP_PALETTE", {"&Palette:", "&Paleta:", NULL, "&Paleta:", NULL, NULL}},
  {"IUP_TRUE", {"True", "Verdadeiro", NULL, "Verdadero", NULL, NULL}},
  {"IUP_FALSE", {"False", "Falso", NULL, "Falso", NULL, NULL}},
  {"IUP_FAMILY", {"Family:", "Família:", "FamÃ­lia:", "Familia:", NULL, NULL}},
  {"IUP_STYLE", {"Style:", "Estilo:", NULL, "Estilo:", NULL, NULL}},
  {"IUP_SIZE", {"Size:", "Tamanho:", NULL, "Tamaño:", "TamaÃ±o:", NULL}},
  {"IUP_SAMPLE", {"Sample:", "Exemplo:", NULL, "Ejemplo:", NULL, NULL}},
  {"IUP_ERRORFILEOPEN", { "Failed to open file.", "Falha ao abrir o arquivo.", NULL, "Error al abrir el archivo.", NULL, NULL } },
  {"IUP_ERRORFILESAVE", { "Failed to save file.", "Falha ao salvar o arquivo.", NULL, "Error al guardar el archivo.", NULL, NULL } },
  { "IUP_LUAERROR", { "Lua Error", "Erro de Lua", NULL, "Error de Lua", NULL, NULL } },
  { "IUP_CONTINUE", { "Continue", "Continuar", NULL, "Continúe", "ContinÃºe", NULL } },
  { "IUP_COPYTOCLIPBOARD", { "Copy text to clipboard.", "Copiar texto para a área de transferência.", "Copiar texto para a Ã¡rea de transferÃªncia.", "Copiar texto para un área de transferencia.", "Copiar texto para un Ã¡rea de transferencia.", NULL } },
  { "IUP_COPY", { "Copy", "Copiar", NULL, "Copiar", NULL, NULL } },
  { "IUP_EXIT", { "Exit", "Sair", NULL, "Salir", NULL, NULL } },
  { NULL, { NULL, NULL, NULL, NULL, NULL, NULL } }
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
      lng = 2;  /* PORTUGUESE(UTF - 8) */
    else
      lng = 1;  /* PORTUGUESE */
  }
  else if (iupStrEqualNoCase(language, "SPANISH"))
  {
    if (utf8mode)
      lng = 4; /* SPANISH(UTF - 8) */
    else
      lng = 3; /* SPANISH */
  }

  iStrMessageRegisterInternal(lng, utf8mode);
  iupRegisterUpdateClasses();
}
