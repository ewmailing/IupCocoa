#include <stdlib.h>
#include <math.h>
#include <iup.h>
#include <iupdraw.h>


/*********************************** Circle List ****************************************/

#define DEF_RADIUS 30
#define MAX_RADIUS 100
#define MAX_CIRCLES 80

typedef struct _Circle
{
  int x;
  int y;
  int r;
  int id;
} Circle;

static Circle circlesList[MAX_CIRCLES];
static int circlesCount = 0;

static int circles_add(int x, int y)
{
  static int lastId = 0;

  if (circlesCount == MAX_CIRCLES)
    return -1;

  circlesList[circlesCount].x = x;
  circlesList[circlesCount].y = y;
  circlesList[circlesCount].r = DEF_RADIUS;
  circlesList[circlesCount].id = lastId;  /* each circle has an unique id */

  circlesCount++;
  lastId++;
  return lastId - 1;
}

static int circles_insert(Circle circle)
{
  if (circlesCount == MAX_CIRCLES)
    return -1;

  circlesList[circlesCount] = circle;
  circlesCount++;
  return 1;
}

static void circles_remove(Circle circle)
{
  int i, j;
  for (i = 0; i < circlesCount; i++)
  {
    if (circlesList[i].id == circle.id)
      break;
  }

  for (j = i; j < circlesCount - 1; j++)
    circlesList[j] = circlesList[j + 1];

  circlesCount--;
}

static void circles_update(Circle circle)
{
  int i;
  for (i = 0; i < circlesCount; i++)
  {
    if (circlesList[i].id == circle.id)
      break;
  }

  circlesList[i].r = circle.r;
}

static int circles_pick(int x, int y)
{
  int i;
  int min_r = MAX_RADIUS;
  int id = -1;

  for (i = 0; i < circlesCount; i++)
  {
    int dx = circlesList[i].x - x;
    int dy = circlesList[i].y - y;
    int r = dx*dx + dy*dy;

    r = (int)sqrt((double)r);

    if (r < circlesList[i].r)
    {
      if (id < 0)
      {
        id = circlesList[i].id;
        min_r = r;
      }
      else if (r < min_r)
        id = circlesList[i].id;
    }
  }

  return id;
}

static Circle circles_find(int id)
{
  int i;
  for (i = 0; i < circlesCount; i++)
  {
    if (circlesList[i].id == id)
      break;
  }
  return circlesList[i];
}


/************************************* Operation List ************************************************/

#define MAX_OPERATIONS 200

enum { OP_CREATE, OP_REMOVE, OP_UPDATE };

typedef struct _Operation
{
  int type;
  Circle circle;
} Operation;

static Operation operationsList[MAX_OPERATIONS];
static int operationsCount = 0;
static int currentOperation = -1;

static void op_add(int type, Circle circle)
{
  if (operationsCount == MAX_OPERATIONS)
    return;

  if (currentOperation < operationsCount - 1)
    operationsCount = currentOperation + 1;

  operationsList[operationsCount].type = type;
  operationsList[operationsCount].circle = circle;
  currentOperation = operationsCount;

  operationsCount++;
}

static void op_undo(void)
{
  Operation op;

  if (currentOperation < 0)
    return;

  op = operationsList[currentOperation];
  currentOperation--;

  switch (op.type)
  {
    case OP_CREATE:
      circles_remove(op.circle);
      break;
    case OP_REMOVE:
      circles_insert(op.circle);
      break;
    case OP_UPDATE:
      op = operationsList[currentOperation];
      currentOperation--; /* return 2 to get the old value */

      circles_update(op.circle);
      break;
  }
}

static void op_redo(void)
{
  Operation op;

  if (currentOperation + 1 == operationsCount)
    return;

  currentOperation++;
  op = operationsList[currentOperation];

  switch (op.type)
  {
    case OP_CREATE:
      circles_insert(op.circle);
      break;
    case OP_REMOVE:
      circles_remove(op.circle);
      break;
    case OP_UPDATE:
      currentOperation++; /* increment 2 to get the new value */
      op = operationsList[currentOperation];

      circles_update(op.circle);
      break;
  }
}

static int op_has_undo(void)
{
  if (currentOperation < 0)
    return 0;

  return 1;
}

static int op_has_redo(void)
{
  if (currentOperation + 1 == operationsCount)
    return 0;

  return 1;
}


/*********************************************** Interface ****************************************/

void update_buttons(Ihandle* ih)
{
  Ihandle *undoButton = IupGetDialogChild(ih, "UNDO");
  Ihandle *redoButton = IupGetDialogChild(ih, "REDO");

  if (op_has_undo())
    IupSetAttribute(undoButton, "ACTIVE", "YES");
  else
    IupSetAttribute(undoButton, "ACTIVE", "NO");

  if (op_has_redo())
    IupSetAttribute(redoButton, "ACTIVE", "YES");
  else
    IupSetAttribute(redoButton, "ACTIVE", "NO");
}

int bt_undo_cb(Ihandle *self)
{
  Ihandle *canvas = IupGetDialogChild(self, "CANVAS");

  op_undo();

  update_buttons(self);

  IupUpdate(canvas);

  return IUP_DEFAULT;
}

int bt_redo_cb(Ihandle *self)
{
  Ihandle *canvas = IupGetDialogChild(self, "CANVAS");

  op_redo();

  update_buttons(self);

  IupUpdate(canvas);

  return IUP_DEFAULT;
}

static int canvas_button_cb(Ihandle *ih, int but, int pressed, int x, int y, char* status)
{
  Ihandle *dlg = IupGetDialog(ih);

  if (but == IUP_BUTTON1 && pressed)
  {
    int id = circles_pick(x, y);
    if (id >= 0)
      IupSetInt(ih, "HIGHLIGHTEDCIRCLE", id);
    else
    {
      id = circles_add(x, y);
      op_add(OP_CREATE, circles_find(id));

      update_buttons(ih);
    }

    IupUpdate(ih);
  }
  else if (but == IUP_BUTTON3 && pressed)
  {
    int highlightedCircle = IupGetInt(ih, "HIGHLIGHTEDCIRCLE");
    int id = circles_pick(x, y);
    if (id == highlightedCircle)
    {
      Circle circle = circles_find(id);
      int new_r, old_r = circle.r;
      Ihandle *configDialog = (Ihandle *)IupGetAttribute(dlg, "CONFIGDIALOG");
      Ihandle *val = IupGetDialogChild(configDialog, "VAL");
      Ihandle *lbl = IupGetDialogChild(configDialog, "LBL");

      IupSetStrf(lbl, "TITLE", "Adjust the diameter of the circle at (%d, %d)", circle.x, circle.y);
      IupSetInt(val, "VALUE", old_r);
      IupSetInt(configDialog, "CIRCLEID", id);

      IupPopup(configDialog, IUP_CENTERPARENT, IUP_CENTERPARENT);

      new_r = IupGetInt(val, "VALUE");
      if (new_r != old_r)
      {
        op_add(OP_UPDATE, circle); /* add both old and new */
        circle.r = new_r;
        op_add(OP_UPDATE, circle);

        update_buttons(ih);
      }
    }
  }

  (void)status;
  return IUP_DEFAULT;
}

static int canvas_action(Ihandle *ih)
{
  int i, w, h, highlightedCircle;

  IupDrawBegin(ih);

  IupDrawGetSize(ih, &w, &h);

  IupSetAttribute(ih, "DRAWCOLOR", "255 255 255");
  IupSetAttribute(ih, "DRAWSTYLE", "FILL");
  IupDrawRectangle(ih, 0, 0, w - 1, h - 1);

  IupSetAttribute(ih, "DRAWCOLOR", "0 0 0");
  IupSetAttribute(ih, "DRAWSTYLE", "STROKE");

  highlightedCircle = IupGetInt(ih, "HIGHLIGHTEDCIRCLE");

  for (i = 0; i < circlesCount; i++)
  {
    Circle circle = circlesList[i];

    if (highlightedCircle == circle.id)
    {
      IupSetAttribute(ih, "DRAWCOLOR", "128 128 128");
      IupSetAttribute(ih, "DRAWSTYLE", "FILL");
    }
    else
    {
      IupSetAttribute(ih, "DRAWCOLOR", "0 0 0");
      IupSetAttribute(ih, "DRAWSTYLE", "STROKE");
    }

    IupDrawArc(ih, circle.x - circle.r, circle.y - circle.r,
                   circle.x + circle.r, circle.y + circle.r, 0., 360.);
  }

  IupDrawEnd(ih);
  return IUP_DEFAULT;
}

int val_valuechanged_cb(Ihandle *self)
{
  Ihandle* dlg = IupGetDialog(self);
  Ihandle* canvas = (Ihandle *)IupGetAttribute(self, "CANVAS");
  int circleId = IupGetInt(dlg, "CIRCLEID");
  int r = IupGetInt(self, "VALUE");
  Circle circle = circles_find(circleId);

  circle.r = r;
  circles_update(circle);

  IupUpdate(canvas);

  return IUP_DEFAULT;
}

Ihandle *createCircleConfigDialog(Ihandle* parent_dlg)
{
  Ihandle *dlg, *val, *lbl, *box;

  lbl = IupLabel(NULL);
  IupSetAttribute(lbl, "EXPAND", "HORIZONTAL");
  IupSetAttribute(lbl, "TITLE", "Adjust the diameter of the circle at (100, 100)");
  IupSetAttribute(lbl, "NAME", "LBL");

  val = IupVal("HORIZONTAL");
  IupSetAttribute(val, "EXPAND", "HORIZONTAL");
  IupSetAttribute(val, "MIN", "0");
  IupSetInt(val, "MAX", MAX_RADIUS);
  IupSetAttribute(val, "NAME", "VAL");

  box = IupVbox(IupFill(), lbl, val, IupFill(), NULL);
  IupSetAttribute(box, "ALIGMENT", "ACENTER");

  IupSetAttribute(box, "NMARGIN", "10x10");
  dlg = IupDialog(box);
  IupSetAttribute(dlg, "TITLE", "Circle Config");
  IupSetAttributeHandle(dlg, "PARENTDIALOG", parent_dlg);

  IupSetCallback(val, "VALUECHANGED_CB", (Icallback)val_valuechanged_cb);

  return dlg;
}

int main(int argc, char **argv)
{
  Ihandle *dlg, *hbox, *vbox, *configDialog;
  Ihandle *undoButton, *redoButton, *canvas;

  IupOpen(&argc, &argv);

  undoButton = IupButton("Undo", NULL);
  IupSetAttribute(undoButton, "NAME", "UNDO");
  IupSetAttribute(undoButton, "SIZE", "60");
  redoButton = IupButton("Redo", NULL);
  IupSetAttribute(redoButton, "NAME", "REDO");
  IupSetAttribute(redoButton, "SIZE", "60");

  IupSetAttribute(redoButton, "ACTIVE", "NO");
  IupSetAttribute(undoButton, "ACTIVE", "NO");

  canvas = IupCanvas(NULL);
  IupSetAttribute(canvas, "NAME", "CANVAS");
  IupSetAttribute(canvas, "EXPAND", "YES");
  IupSetInt(canvas, "HIGHLIGHTEDCIRCLE", -1);

  hbox = IupHbox(IupFill(), undoButton, redoButton, IupFill(), NULL);

  vbox = IupVbox(hbox, canvas, NULL);

  IupSetAttribute(vbox, "NMARGIN", "10x10");
  IupSetAttribute(vbox, "GAP", "10");
  dlg = IupDialog(vbox);
  IupSetAttribute(dlg, "TITLE", "Circle Drawer");
  IupSetAttribute(dlg, "SIZE", "300x150");

  /* Registers callbacks */
  IupSetCallback(undoButton, "ACTION", (Icallback)bt_undo_cb);
  IupSetCallback(redoButton, "ACTION", (Icallback)bt_redo_cb);
  IupSetCallback(canvas, "ACTION", (Icallback)canvas_action);
  IupSetCallback(canvas, "BUTTON_CB", (Icallback)canvas_button_cb);

  configDialog = createCircleConfigDialog(dlg);

  IupSetAttribute(configDialog, "CANVAS", (char *)canvas);

  IupSetAttribute(dlg, "CONFIGDIALOG", (char *)configDialog);

  IupShowXY(dlg, IUP_CENTER, IUP_CENTER);

  IupMainLoop();

  IupClose();
  return EXIT_SUCCESS;
}
