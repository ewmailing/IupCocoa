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
  {"IUP_ATTENTION", {"Attention!", "Aten��o!", "Atenção!", "�Advertencia!", "¡Advertencia!", NULL}},
  {"IUP_YES", {"Yes", "Sim", NULL, "Si", NULL, NULL}},
  {"IUP_NO", {"No", "N�o", "Não", "No", NULL, NULL}},
  {"IUP_INVALIDDIR", {"Invalid directory.", "Diret�rio inv�lido.", "Diretório inválido.", "Directorio inv�lido.", "Directorio inválido.", NULL}},
  {"IUP_FILEISDIR", {"The selected name is a directory.", "O nome selecionado � um diret�rio.", "O nome selecionado é um diretório.", "El nombre seleccionado es un directorio.", NULL, NULL}},
  {"IUP_FILENOTEXIST", {"File does not exist.", "Arquivo inexistente.", NULL, "Archivo inexistente.", NULL, NULL}},
  {"IUP_FILEOVERWRITE", {"Overwrite existing file?", "Sobrescrever arquivo?", NULL, "�Sobrescribir archivo?", "¿Sobrescribir archivo?", NULL}},
  {"IUP_CREATEFOLDER", {"Create Folder", "Criar Diret�rio", "Criar Diretório", "Crear Directorio", NULL, NULL}},
  {"IUP_NAMENEWFOLDER", {"Name of the new folder:", "Nome do novo diret�rio:", "Nome do novo diretório:", "Nombre del nuevo directorio:", NULL, NULL}},
  {"IUP_SAVEAS", {"Save As", "Salvar Como", NULL, "Guardar Como", NULL, NULL}},
  {"IUP_OPEN", {"Open", "Abrir", NULL, "Abrir", NULL, NULL}},
  {"IUP_SELECTDIR", {"Select Directory", "Selecionar Diret�rio", "Selecionar Diretório", "Seleccionar Directorio", NULL, NULL}},
  {"IUP_OK", {"OK", "OK", NULL, "Aceptar", NULL, NULL}},
  {"IUP_CANCEL", {"Cancel", "Cancelar", NULL, "Cancelar", NULL, NULL}},
  {"IUP_RETRY", {"Retry", "Tentar Novamente", NULL, "Reintentar", NULL, NULL}},
  {"IUP_APPLY", {"Apply", "Aplicar", NULL, "Aplicar", NULL, NULL}},
  {"IUP_RESET", {"Reset", "Reinicializar", NULL, "Reiniciar", NULL, NULL}},
  {"IUP_GETCOLOR", {"Color Selection", "Sele��o de Cor", "Seleção de Cor", "Selecci�n de Color", "Selección de Color", NULL}},
  {"IUP_HELP", {"Help", "Ajuda", NULL, "Ayuda", NULL, NULL}},
  {"IUP_RED", {"&Red:", "&Vermelho:", NULL, "&Rojo:", NULL, NULL}},
  {"IUP_GREEN", {"&Green:", "V&erde:", NULL, "&Verde:", NULL, NULL}},
  {"IUP_BLUE", {"&Blue:", "&Azul:", NULL, "&Azul:", NULL, NULL}},
  {"IUP_HUE", {"&Hue:", "&Matiz:", NULL, "&Matiz:", NULL, NULL}},
  {"IUP_SATURATION", {"&Saturation:", "&Satura��o:", "&Saturação:", "&Saturaci�n:", "&Saturación:", NULL}},
  {"IUP_INTENSITY", {"&Intensity:", NULL, "&Intensidade:", "&Intensidad:", NULL, NULL}},
  {"IUP_OPACITY", {"&Opacity:", "&Opacidade:", NULL, "&Opacidad:", NULL, NULL}},
  {"IUP_PALETTE", {"&Palette:", "&Paleta:", NULL, "&Paleta:", NULL, NULL}},
  {"IUP_TRUE", {"True", "Verdadeiro", NULL, "Verdadero", NULL, NULL}},
  {"IUP_FALSE", {"False", "Falso", NULL, "Falso", NULL, NULL}},
  {"IUP_FAMILY", {"Family:", "Fam�lia:", "Família:", "Familia:", NULL, NULL}},
  {"IUP_STYLE", {"Style:", "Estilo:", NULL, "Estilo:", NULL, NULL}},
  {"IUP_SIZE", {"Size:", "Tamanho:", NULL, "Tama�o:", "Tamaño:", NULL}},
  {"IUP_SAMPLE", {"Sample:", "Exemplo:", NULL, "Ejemplo:", NULL, NULL}},
  {"IUP_ERRORFILEOPEN", { "Failed to open file.", "Falha ao abrir o arquivo.", NULL, "Error al abrir el archivo.", NULL, NULL } },
  {"IUP_ERRORFILESAVE", { "Failed to save file.", "Falha ao salvar o arquivo.", NULL, "Error al guardar el archivo.", NULL, NULL } },
  { "IUP_LUAERROR", { "Lua Error", "Erro de Lua", NULL, "Error de Lua", NULL, NULL } },
  { "IUP_CONTINUE", { "Continue", "Continuar", NULL, "Contin�e", "Continúe", NULL } },
  { "IUP_COPYTOCLIPBOARD", { "Copy text to clipboard.", "Copiar texto para a �rea de transfer�ncia.", "Copiar texto para a área de transferência.", "Copiar texto para un �rea de transferencia.", "Copiar texto para un área de transferencia.", NULL } },
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
