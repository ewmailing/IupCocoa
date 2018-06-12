/** \file
 * \brief GTK Message Loop
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>    
#include <string.h>    
#include <stdlib.h>

#include <gtk/gtk.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_str.h"

/* local variables */
static IFidle gtk_idle_cb = NULL;
static guint gtk_idle_id;

static gboolean gtkIdleFunc(gpointer data)
{
  (void)data;
  if (gtk_idle_cb)
  {
    int ret = gtk_idle_cb();
    if (ret == IUP_CLOSE)
    {
      gtk_idle_cb = NULL;
      IupExitLoop();
      return FALSE; /* removes the idle */
    }
    if (ret == IUP_IGNORE)
    {
      gtk_idle_cb = NULL;
      return FALSE; /* removes the idle */
    }

    return TRUE; /* keeps the idle */
  }

  return FALSE; /* removes the idle */
}

void iupdrvSetIdleFunction(Icallback f)
{
  if (gtk_idle_cb)
    g_source_remove(gtk_idle_id);

  gtk_idle_cb = (IFidle)f;

  if (gtk_idle_cb)
    gtk_idle_id = g_idle_add(gtkIdleFunc, NULL);
}

void IupExitLoop(void)
{
  char* exit_loop = IupGetGlobal("EXITLOOP");
  if (gtk_main_level() > 1 || !exit_loop || iupStrBoolean(exit_loop))
  {
    if (gtk_main_iteration_do(FALSE) == FALSE)
      gtk_main_quit();
  }
}

int IupMainLoopLevel(void)
{
  return gtk_main_level();
}

int IupMainLoop(void)
{
  gtk_main();
  return IUP_NOERROR;
}

int IupLoopStepWait(void)
{
  if (gtk_main_iteration_do(TRUE))
    return IUP_CLOSE;
  return IUP_DEFAULT;
}

int IupLoopStep(void)
{
  if (gtk_main_iteration_do(FALSE))
    return IUP_CLOSE;
  return IUP_DEFAULT;
}

void IupFlush(void)
{
  int count = 0;

  IFidle old_gtk_idle_cb = NULL;
  if (gtk_idle_cb)
  {
    old_gtk_idle_cb = gtk_idle_cb;
    iupdrvSetIdleFunction(NULL);
  }

  while (count<100 && gtk_events_pending())
  {
    gtk_main_iteration();

    /* we detected that after destroying a popup dialog
       just after clicking in a button of the same dialog,
       sometimes a message gets lost and gtk_events_pending
       keeps returning TRUE */
    count++;
  }

  if (old_gtk_idle_cb)
    iupdrvSetIdleFunction((Icallback)old_gtk_idle_cb);
}


typedef struct {
  Ihandle* ih;
  void* messageData;
  char* ptrParam;
  int intParam;
} gtkPostMessageUserData;

static gint gtkPostMessageCallback(void *user_data)
{
  gtkPostMessageUserData* message_user_data = (gtkPostMessageUserData*)user_data;
  Ihandle* ih = message_user_data->ih;
  /* TODO: Figure out callback type. For now, I'm reusing an existing type so I don't have to add one until we decide. */
  IFnsVi post_message_callback = (IFnsVi)IupGetCallback(ih, "POSTMESSAGE_CB");
  if (post_message_callback)
  {
    void* message_data = message_user_data->messageData;
    char* ptr_param = message_user_data->ptrParam;
    void* int_param = message_user_data->intParam;
    post_message_callback(ih, ptr_param, message_data, int_param);
  }
  free(user_data);
  return FALSE; // call only once
}

/* TODO: Make decision on final API. For now, this API is just to get a usable demo. */
void IupPostMessage(Ihandle* ih, char* unusedchar, void* message_data, int unusedint)
{
  gtkPostMessageUserData* user_data = (gtkPostMessageUserData*)malloc(sizeof(gtkPostMessageUserData));
  user_data->ih = ih;
  user_data->messageData = message_data;
  user_data->ptrParam = unusedchar;
  user_data->intParam = unusedint;
  g_idle_add(gtkPostMessageCallback, user_data);  
}
