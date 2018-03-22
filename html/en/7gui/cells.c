#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <iup.h>
#include <iupcontrols.h>

enum { OP_SUM };

typedef struct _Cell 
{
  int lin;
  int col;
} Cell;

typedef struct _Operation
{
  int type;
  int startLin;
  int endLin;
  int startCol;
  int endCol;
} Operation;

enum 
{
  FORM_READY, /* number is ready to use */
  FORM_CALC,  /* calculating */
  FORM_DIRTY  /* need calc */
};

enum {
  ERR_NONE,
  ERR_PARSE,
  ERR_RECURSE,
  ERR_DEPEND
};

/* each cell can contain:
  - no value, an empty cell
  - a text value
  - a numeric value
  - a sum of values (a formula starts with '=')
*/
typedef struct _CellData
{
  int parseError;
  double number;
  int isNumber;
  int calcFormula;
  char value[80];
  Operation operation;
  Cell dependencies[30]; /* cells that are depend on this cell */
  int dependenciesCount;
} CellData;


#define NUM_COL 26   /* A-Z=26 */
#define NUM_LIN 100  /* 0-99=100 */

static CellData data[NUM_LIN][NUM_COL];

static void cellDataInit(void)
{
  int i, j;
  for (i = 0; i < NUM_COL; i++)
  {
    for (j = 0; j < NUM_LIN; j++)
    {
      memset(&(data[i][j]), 0, sizeof(CellData));
    }
  }

  /* memset(data, 0, sizeof(data));  will also work because it is a constant array */
}

static char* cellDataGetValue(int lin, int col)
{
  return data[lin][col].value;
}

static int cellDataIsFormula(int lin, int col)
{
  if (data[lin][col].isNumber || data[lin][col].value[0] != '=')
    return 0;
  else
    return 1;
}

static void cellDataSetDependenciesDirty(CellData* c)
{
  int i;
  for (i = 0; i < c->dependenciesCount; i++)
  {
    int dep_lin = c->dependencies[i].lin;
    int dep_col = c->dependencies[i].col;
    data[dep_lin][dep_col].calcFormula = FORM_DIRTY;
  }
}

static double cellDataIsNumber(int lin, int col)
{
  return data[lin][col].isNumber || (data[lin][col].value[0] == '=' && data[lin][col].parseError == ERR_NONE);
}

static double cellDataGetNumber(int lin, int col);

static double cellDataSumCells(CellData* c)
{
  int i, j;
  double total = 0;

  for (i = c->operation.startLin; i <= c->operation.endLin; i++)
  {
    for (j = c->operation.startCol; j <= c->operation.endCol; j++)
    {
      if (cellDataIsNumber(i, j))
        total += cellDataGetNumber(i, j);
      else if (data[i][j].value[0] != 0)
      {
        c->parseError = ERR_DEPEND;
        return 0;
      }
    }
  }

  return total;
}

static void cellDataCalcFormula(CellData* c)
{
  if (c->calcFormula == FORM_CALC)
  {
    c->parseError = ERR_RECURSE;
    return;
  }

  cellDataSetDependenciesDirty(c);

  if (c->parseError == ERR_NONE)
  {
    switch (c->operation.type)
    {
    case OP_SUM:
      c->calcFormula = FORM_CALC;

      c->number = cellDataSumCells(c);

      if (c->parseError == ERR_NONE)
        c->calcFormula = FORM_READY;
      else
        c->calcFormula = FORM_DIRTY;
      break;
    }
  }
}

static double cellDataGetNumber(int lin, int col)
{
  CellData* c = &(data[lin][col]);
  if (c->value[0] == '=' && c->calcFormula != FORM_READY)
    cellDataCalcFormula(c);

  return c->number;
}

static char* cellDataGetString(int lin, int col)
{
  /* called when not a number */
  if (data[lin][col].value[0] == '=')
  {
    int error = data[lin][col].parseError;
    if (error != ERR_NONE)
    {
      if (error == ERR_PARSE)
        return "Error Invalid Formula!";
      else if (error == ERR_RECURSE)
        return "Error Recursive Formula!";
      else /* ERR_DEPEND */
        return "Error Dependency in Formula!";
    }
  }

  return data[lin][col].value;
}

static void cellDataSetNumber(int lin, int col, double value)
{
  CellData* c = &(data[lin][col]);

  c->isNumber = 1;
  c->number = value;
  c->calcFormula = FORM_READY;
  c->parseError = ERR_NONE;
  c->value[0] = 0;

  cellDataSetDependenciesDirty(c);
}

static void cellDataParseFormula(CellData* c)
{
  int l1, l2, n;
  char c1, c2, func[80];
  char *formula = c->value;

  c->parseError = ERR_PARSE;

  n = sscanf(formula+1, "%3s(%c%d:%c%d)", func, &c1, &l1, &c2, &l2);
  if (n != 5)
    return;

  if (strcmp(func, "sum") == 0)
    c->operation.type = OP_SUM;
  else
    return;

  if (c1 < 'A' || c1 > 'Z')
    return;
  if (c2 < 'A' || c2 > 'Z')
    return;
  if (l1 < 0 || l1 > 99)
    return;
  if (l2 < 0 || l2 > 99)
    return;

  c->parseError = ERR_NONE;

  c->operation.startCol = c1 - 'A';
  c->operation.startLin = l1;
  c->operation.endCol = c2 - 'A';
  c->operation.endLin = l2;
}

static void cellDataAddDependencies(CellData* c, int dep_lin, int dep_col)
{
  int i;
  Cell cell;
  for (i = 0; i < c->dependenciesCount; i++)
  {
    if (dep_lin == c->dependencies[i].lin && 
        dep_col == c->dependencies[i].col)
      return; /* already in the list */
  }

  /* not found, add to the list */
  cell.col = dep_col;
  cell.lin = dep_lin;
  c->dependencies[c->dependenciesCount] = cell;
  c->dependenciesCount++;
}

static void cellDataRemoveDependencies(CellData* c, int dep_lin, int dep_col)
{
  int i;
  for (i = 0; i < c->dependenciesCount; i++)
  {
    if (dep_lin == c->dependencies[i].lin &&
        dep_col == c->dependencies[i].col)
        break; /* found in the list */
  }

  if (i == c->dependenciesCount)
    return;

  /* remove from the list */
  for (; i < c->dependenciesCount - 1; i++)
  {
    c->dependencies[i] = c->dependencies[i+1];
  }

  c->dependenciesCount--;
}

static void cellDataUpdateDependencies(CellData* c, int lin, int col)
{
  int i, j;

  for (i = 0; i < NUM_LIN; i++)
  {
    for (j = 0; j < NUM_COL; j++)
    {
      if (i >= c->operation.startLin && i <= c->operation.endLin &&
          j >= c->operation.startCol && j <= c->operation.endCol)
        cellDataAddDependencies(&(data[i][j]), lin, col);
      else
        cellDataRemoveDependencies(&(data[i][j]), lin, col);
    }
  }
}

static void cellDataSetString(int lin, int col, const char *value)
{
  CellData* c = &(data[lin][col]);

  cellDataSetDependenciesDirty(c);

  c->isNumber = 0;
  c->number = 0;
  c->calcFormula = FORM_READY;
  c->parseError = ERR_NONE;
  strcpy(c->value, value);

  if (c->value[0] == '=')
  {
    c->calcFormula = FORM_DIRTY;

    cellDataParseFormula(c);

    if (c->parseError == ERR_NONE)
      cellDataUpdateDependencies(c, lin, col);
  }
}


/*************************************** Interface **************************************************/


static char* matrix_value_cb(Ihandle *self, int lin, int col)
{
  static char text[80];
  char *editcell = IupGetAttribute(self, "EDITCELL");
  if (editcell != NULL)
  {
    int elin, ecol;
    sscanf(editcell, "%d:%d", &elin, &ecol);
    if (elin == lin && ecol == col && cellDataIsFormula(lin - 1, col - 1))
      return cellDataGetValue(lin - 1, col - 1);
  }

  if (lin == 0 && col == 0)
    return "";
  else if (lin == 0)
  {
    sprintf(text, "%c", 'A' + col - 1);
    return text;
  }
  else if (col == 0)
  {
    sprintf(text, "%d", lin - 1);
    return text;
  }

  if (cellDataIsNumber(lin - 1, col - 1))
  {
    double number = cellDataGetNumber(lin - 1, col - 1);
    if (cellDataIsNumber(lin - 1, col - 1))
      sprintf(text, "%.2lf", number);
    else
      return cellDataGetString(lin - 1, col - 1);
  }
  else
    return cellDataGetString(lin - 1, col - 1);

  return text;
}

static int matrix_value_edit_cb(Ihandle *self, int lin, int col, char* newvalue)
{
  char str[80];
  double d;

  int i = sscanf(newvalue, "%lf%s", &d, &str);
  if (i == 1) /* check if there is more text after the number (text may start with a number) */
    cellDataSetNumber(lin - 1, col - 1, d);
  else
    cellDataSetString(lin - 1, col - 1, newvalue);

  (void)self;
  return IUP_DEFAULT;
}

int main(int argc, char **argv)
{
  Ihandle *dlg, *matrix;

  IupOpen(&argc, &argv);
  IupControlsOpen();

  matrix = IupMatrix(NULL);

  IupSetAttribute(matrix, "NAME", "MATRIX");

  IupSetInt(matrix, "NUMCOL", NUM_COL);
  IupSetInt(matrix, "NUMLIN", NUM_LIN);

  IupSetAttribute(matrix, "NUMCOL_VISIBLE", "4");
  IupSetAttribute(matrix, "NUMLIN_VISIBLE", "7");

  IupSetAttribute(matrix, "WIDTH0", "15");
  IupSetAttribute(matrix, "WIDTHDEF", "40");
  IupSetAttribute(matrix, "HEIGHT0", "8");
  IupSetAttribute(matrix, "RESIZEMATRIX", "Yes");

  cellDataInit();

  IupSetCallback(matrix, "VALUE_CB", (Icallback)matrix_value_cb);
  IupSetCallback(matrix, "VALUE_EDIT_CB", (Icallback)matrix_value_edit_cb);

  dlg = IupDialog(matrix);
  IupSetAttribute(dlg, "TITLE", "Cells");
  IupSetAttribute(dlg, "SIZE", "300x150");

  IupShowXY(dlg, IUP_CENTER, IUP_CENTER);

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
