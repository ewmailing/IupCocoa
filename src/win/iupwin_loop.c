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
#include "iup_loop.h"

#include "iupwin_drv.h"
#include "iupwin_handle.h"


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
  PostQuitMessage(0);
}

static int winLoopProcessMessage(MSG* msg)
{
  if (msg->message == WM_QUIT)  /* IUP_CLOSE returned in a callback or IupHide in a popup dialog or all dialogs closed */
    return IUP_CLOSE;
  else
  {
    TranslateMessage(msg);
    DispatchMessage(msg);
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
  int return_code = IUP_NOERROR;
  static int has_done_entry = 0;
  static int has_done_exit = 0;

  if (0 == has_done_entry)
  {
	  has_done_entry = 1;
	  iupLoopCallEntryCb();
  }

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
          // win_main_loop will be decremented at the end of this function;
          return_code = IUP_CLOSE;
          break;
        }
      }
      else
      {
        if (winLoopCallIdle() == IUP_CLOSE)
        {
          // win_main_loop will be decremented at the end of this function;
          return_code = IUP_CLOSE;
          break;
        }
      }
    }
    else
    {
      ret = GetMessage(&msg, NULL, 0, 0);
      if (ret == -1) /* error */
      {
        // win_main_loop will be decremented at the end of this function;
        return_code = IUP_ERROR;
        break;
      }
      if (ret == 0 || /* WM_QUIT */
          winLoopProcessMessage(&msg) == IUP_CLOSE)  /* ret != 0 */
      {
        // win_main_loop will be decremented at the end of this function;
        return_code = IUP_NOERROR;
        break;
      }
    }
  } while (ret);

  win_main_loop--;


  if ((0 == win_main_loop) && (0 == has_done_exit))
  {
    has_done_exit = 1;
    iupLoopCallExitCb();
  }

  return return_code;
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
