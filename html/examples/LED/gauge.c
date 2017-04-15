#include <stdio.h>
#include "iup.h"
#include "iupcontrols.h"
#include "cd.h"
#include "cdiup.h"

/* global variables that store handles used by the idle function */
Ihandle *dlg=NULL;
Ihandle *gauge=NULL;
Ihandle *timer=NULL;

/* timer callback */
int time_cb(void)
{
  char newvalue[40];
  double value = IupGetFloat(gauge, "VALUE");
  value += 0.01;
  if(value > 1.0) value = 0;
  sprintf(newvalue, "%.7f",value);
  IupSetAttribute(gauge, "VALUE", newvalue);
  return IUP_DEFAULT;
}

/* pause button callback */
int pausa_cb(void)
{
  if (IupGetInt(timer, "RUN"))
    IupSetAttribute(timer, "RUN", "NO");
  else
    IupSetAttribute(timer, "RUN", "YES");
  return IUP_DEFAULT;
}

/* start button callback */
int inicio_cb(void)
{
  IupSetAttribute(gauge, "VALUE", "0");
  return IUP_DEFAULT;
}

/* accelerate button callback */
int acelera_cb(void)
{
  int time = IupGetInt(timer, "TIME");
  time /= 2;
  IupSetAttribute(timer, "RUN", "NO");
  IupSetInt(timer, "TIME", time);
  IupSetAttribute(timer, "RUN", "YES");
  return IUP_DEFAULT;
}

/* decelerate button callback */
int freia_cb(void)
{
  int time = IupGetInt(timer, "TIME");
  time *= 2;
  IupSetAttribute(timer, "RUN", "NO");
  IupSetInt(timer, "TIME", time);
  IupSetAttribute(timer, "RUN", "YES");
  return IUP_DEFAULT;
}

/* show button callback */
int exibe_cb(void)
{
  if (IupGetInt(gauge,"SHOWTEXT"))
  {
    /* shows percentage in gauge */
    IupSetAttribute(gauge,"SHOWTEXT","NO");
  }  
  else
  {
    /* does not show percentage in gauge */
    IupSetAttribute(gauge,"SHOWTEXT","YES");
  }	
  return IUP_DEFAULT;
}

/* main program */
int main(int argc, char **argv)
{
  char *error;
  
  /* IUP initialization */
  IupOpen(&argc, &argv);       
  IupControlsOpen ();

  /* loads LED */
  error = IupLoad("gauge.led");
  if (error)
  {
    IupMessage("LED error", error);
    return 1;
  }

  timer = IupTimer();
  IupSetCallback(timer, "ACTION_CB", time_cb);
  IupSetAttribute(timer, "TIME", "100");

  dlg = IupGetHandle("dialog_name");
  gauge = IupGetHandle("gauge_name");

  /* sets callbacks */
  IupSetFunction( "acao_pausa", (Icallback) pausa_cb );
  IupSetFunction( "acao_inicio", (Icallback) inicio_cb );
  IupSetFunction( "acao_acelera", (Icallback) acelera_cb );
  IupSetFunction( "acao_freia", (Icallback) freia_cb );
  IupSetFunction( "acao_exibe", (Icallback) exibe_cb );
  
  /* shows dialog */
  IupShowXY(dlg,IUP_CENTER,IUP_CENTER);

  /* main loop */
  IupMainLoop();

  IupDestroy(dlg);

  /* ends IUP */
  IupClose();

  return 0;
}