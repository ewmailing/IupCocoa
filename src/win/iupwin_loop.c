/** \file
 * \brief Windows Message Loop
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h> 
#include <stdlib.h>
#include <string.h> 

#include <windows.h>

#include "iup.h"
#include "iupcbs.h"  

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"

#include "iupwin_drv.h"
#include "iupwin_handle.h"

/* This just needs to be a random unique number not used by the OS */
#define IWIN_POSTMESSAGE_ID 0x4456

static IFidle win_idle_cb = NULL;
static int win_main_loop = 0;


void iupdrvSetIdleFunction(Icallback f)
{
  win_idle_cb = (IFidle)f;
}

static int winLoopCallIdle(void)
{
  int ret = win_idle_cb();
  if (ret == IUP_CLOSE)
  {
    win_idle_cb = NULL;
    return IUP_CLOSE;
  }
  if (ret == IUP_IGNORE) 
    win_idle_cb = NULL;
  return ret;
}

void IupExitLoop(void)
{
  char* exit_loop = IupGetGlobal("EXITLOOP");
  if (win_main_loop > 1 || !exit_loop || iupStrBoolean(exit_loop))
    PostQuitMessage(0);
}

static int winLoopProcessMessage(MSG* msg)
{
  if (msg->message == WM_QUIT)  /* IUP_CLOSE returned in a callback or IupHide in a popup dialog or all dialogs closed */
    return IUP_CLOSE;
  else
  {
    if (!CallMsgFilter(msg, IWIN_POSTMESSAGE_ID))
    {
      TranslateMessage(msg);
      DispatchMessage(msg);
    }
    return IUP_DEFAULT;
  }
}

int IupMainLoopLevel(void)
{
  return win_main_loop;
}

int IupMainLoop(void)
{
  MSG msg;
  int ret;

  win_main_loop++;

  do 
  {
    if (win_idle_cb)
    {
      ret = 1;
      if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
      {
        if (winLoopProcessMessage(&msg) == IUP_CLOSE)
        {
          win_main_loop--;
          return IUP_CLOSE;
        }
      }
      else
      {
        if (winLoopCallIdle() == IUP_CLOSE)
        {
          win_main_loop--;
          return IUP_CLOSE;
        }
      }
    }
    else
    {
      ret = GetMessage(&msg, NULL, 0, 0);
      if (ret == -1) /* error */
      {
        win_main_loop--;
        return IUP_ERROR;
      }
      if (ret == 0 || /* WM_QUIT */
          winLoopProcessMessage(&msg) == IUP_CLOSE)  /* ret != 0 */
      {
        win_main_loop--;
        return IUP_NOERROR;
      }
    }
  } while (ret);

  win_main_loop--;   /* just for the record, should never pass here */
  return IUP_NOERROR;
}

int IupLoopStepWait(void)
{
  MSG msg;
  int ret = GetMessage(&msg, NULL, 0, 0);
  if (ret == -1) /* error */
    return IUP_ERROR;
  if (ret == 0 || /* WM_QUIT */
      winLoopProcessMessage(&msg) == IUP_CLOSE)  /* ret != 0 */
    return IUP_CLOSE;
  return IUP_DEFAULT;
}

int IupLoopStep(void)
{
  MSG msg;
  if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
    return winLoopProcessMessage(&msg);
  else if (win_idle_cb)
    return winLoopCallIdle();

  return IUP_DEFAULT;
}

void IupFlush(void)
{
  int post_quit = 0;
  MSG msg;

  while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
  {
    if (winLoopProcessMessage(&msg) == IUP_CLOSE)
    {
      post_quit = 1;
      break;
    }
  }

  /* re post the quit message if still inside MainLoop */
  if (post_quit && win_main_loop>0)
    IupExitLoop();
}

/* Based on Raymond Chen's discussion of PostThreadMessage
https://blogs.msdn.microsoft.com/oldnewthing/20050428-00/?p=35753
TODO: Make decision on final API. For now, this API is just to get a usable demo.
*/
void IupPostMessage(Ihandle* ih, char* unusedchar, void* message_data, int unusedint)
{
  /* REVIEW: I am passing the Ihandle* ih into the WPARAM field because I'm using the LPARAM field for message_data. */
  /* I think this is okay because the size of WPARAM should be the same as LPARAM. */
  PostThreadMessage(iupwin_mainthreadid, WM_APP, (WPARAM)ih, (LPARAM)message_data);
}

LRESULT CALLBACK iupwinPostMessageFilterProc(int code, WPARAM wParam, LPARAM lParam)
{
  MSG* pmsg = (MSG*)lParam;
  /* Interesting: Chen uses code >= 0 for a purpose.
	Usually, we get back IWIN_POSTMESSAGE_ID, but in the case where we could lose messages (right-click on the title bar)
	I get a different number. (2 for the right-click title bar)
	If we check explicitly only for code == IWIN_POSTMESSAGE_ID, then we skip this block in this case and lose messages.
	Unfortunately, we lose yet another identifier to recognize this is our message.
	However, Chen looks for WM_APP in psg->message which implies that is unique enough.
	If somehow we start getting other WM_APP messages that aren't ours, we may need
	to change things to set pmsg->wParam to another unique identifier so we can recognize this is our data.
	But for now, this seems to work and hopefully there can't be any other messages since we are the only
	ones that should be posting messages to our thread in an IUP app.
  */
  if (code >= 0)
  {
    switch (pmsg->message)
    {
      case WM_APP: 
      {
        Ihandle* ih = (Ihandle*)pmsg->wParam;
        /* TODO: Figure out callback type. For now, I'm reusing an existing type so I don't have to add one until we decide. */
        IFnsVi post_message_callback = (IFnsVi)IupGetCallback(ih, "POSTMESSAGE_CB");
        if (post_message_callback)
        {
          void* message_data = (Ihandle*)pmsg->lParam;
          post_message_callback(ih, NULL, message_data, 0);
        }
        return TRUE;
      }
    }
  }

  return CallNextHookEx(iupwin_threadmsghook, code, wParam, lParam);
}
