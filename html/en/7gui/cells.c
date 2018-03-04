#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "iup.h"
#include "iupcontrols.h"

enum { SUM };

typedef struct _Cell {
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

typedef struct _celldata{
  int isFormula;
  int parseError;
  double value;
  char formula[80];
  Operation operation;
  Cell dependencies[80];
  int dependenciesSize;
} Celldata;

static Celldata data[101][27];
static char text[80];

static void initData()
{
  int i, j;
  for (i = 1; i < 27; i++)
  {
    for (j = 1; j < 101; j++)
    {
      data[i][j].isFormula = 0;
      data[i][j].value = 0.0;
      data[i][j].dependenciesSize = 0;
    }
  }
}
static char *getFormula(int lin, int col)
{
  return data[lin][col].formula;
}

static int isFormula(int lin, int col)
{
  return data[lin][col].isFormula;
}

static int getParseError(lin, col)
{
  return data[lin][col].parseError;
}

static double getValue(lin, col)
{
  return data[lin][col].value;
}

static void parseFormula(int lin, int col)
{
  char *formula = data[lin][col].formula;
  int startLin, endLin, startCol, endCol, n;
  char c1, c2, func[80];
  double total;

  data[lin][col].parseError = 1;

  if (formula[0] == '\0')
    return;

  n = sscanf(formula, "%3s(%c%d:%c%d)", func, &c1, &startLin, &c2, &endLin);

  if (n != 5)
    return;

  data[lin][col].parseError = 0;

  if (strcmp(func, "SUM") == 0)
    data[lin][col].operation.type = SUM;

  data[lin][col].operation.startCol = c1 - 'A' + 1;
  data[lin][col].operation.startLin = startLin+1;
  data[lin][col].operation.endCol = c2 - 'A' + 1;
  data[lin][col].operation.endLin = endLin+1;

  return;
}

static void addDependency(int lin, int col, int depLin, int depCol)
{
  Cell cell = { depLin, depCol };
  data[lin][col].dependencies[data[lin][col].dependenciesSize] = cell;
  ++data[lin][col].dependenciesSize;
}

static void removeDependency(int lin, int col, int depLin, int depCol)
{
  int i;

  for (i = 0; i < data[lin][col].dependenciesSize; i++)
  {
    if (data[lin][col].dependencies[i].lin == depLin && data[lin][col].dependencies[i].col == depCol)
      break;
  }
  for (; i < data[lin][col].dependenciesSize - 1; i++)
  {
    data[lin][col].dependencies[i] = data[lin][col].dependencies[i];
  }
  --data[lin][col].dependenciesSize;
}

static void setFormula(int lin, int col, char *formula)
{
  if (data[lin][col].isFormula && !data[lin][col].parseError)
  {
    int i, j;

    for (i = data[lin][col].operation.startLin; i <= data[lin][col].operation.endLin; i++)
    {
      for (j = data[lin][col].operation.startCol; j <= data[lin][col].operation.endCol; j++)
        removeDependency(i+1, j+1, lin, col);
    }
  }
  data[lin][col].isFormula = 1;
  strcpy(data[lin][col].formula, formula);
  parseFormula(lin, col);
  if (data[lin][col].isFormula && !data[lin][col].parseError)
  {
    int i, j;

    for (i = data[lin][col].operation.startLin; i <= data[lin][col].operation.endLin; i++)
    {
      for (j = data[lin][col].operation.startCol; j <= data[lin][col].operation.endCol; j++)
        addDependency(i, j, lin, col);
    }
  }
}

static void setValue(int lin, int col, double value)
{
  data[lin][col].isFormula = 0;
  data[lin][col].value = value;
}

static double sumCells(int lin, int col)
{
  int i, j;
  double total = 0;

  for (i = data[lin][col].operation.startLin; i <= data[lin][col].operation.endLin; i++)
  {
    for (j = data[lin][col].operation.startCol; j <= data[lin][col].operation.endCol; j++)
    {
      total += data[i][j].value;
    }
  }

  return total;
}

static void runFormula(int lin, int col)
{
  if (!data[lin][col].parseError)
  {
    switch (data[lin][col].operation.type)
    {
      case SUM:
        data[lin][col].value = sumCells(lin, col);
        break;
    }
  }

}

static char* value_cb(Ihandle *self, int lin, int col)
{
  char *editcell = IupGetAttribute(self, "EDITCELL");
  if (editcell != NULL)
  {
    int elin, ecol;
    sscanf(editcell, "%d:%d", &elin, &ecol);
    if (isFormula(lin, col) && elin == lin && ecol == col)
      return getFormula(lin, col);
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
    sprintf(text, "%d", lin-1);
    return text;
  }

  if (isFormula(lin, col) && getParseError(lin, col))
    strcpy(text, "ERROR!");
  else
    sprintf(text, "%.2lf", getValue(lin, col));

  return text;
}

static int value_edit_cb(Ihandle *self, int lin, int col, char* newvalue)
{
  int s;
  double d;

  int i = sscanf(newvalue, "%lf%s", &d, &s);
  if (i == 1)
  {
    int j;
    setValue(lin, col, d);
    for (j = 0; j < data[lin][col].dependenciesSize; j++)
      runFormula(data[lin][col].dependencies[j].lin, data[lin][col].dependencies[j].col);
  }
  else
  {
    setFormula(lin, col, newvalue);
    runFormula(lin, col);
  }

  return IUP_DEFAULT;
}

int main(int argc, char **argv)
{
  Ihandle *dlg, *matrix;
  int i, j;

  IupOpen(&argc, &argv);
  IupControlsOpen();

  matrix = IupMatrix(NULL);

  IupSetAttribute(matrix, "NAME", "MATRIX");

  IupSetAttribute(matrix, "NUMCOL", "26");
  IupSetAttribute(matrix, "NUMLIN", "100");

  IupSetAttribute(matrix, "NUMCOL_VISIBLE", "4");
  IupSetAttribute(matrix, "NUMLIN_VISIBLE", "7");

  IupSetAttribute(matrix, "WIDTH0", "20");
  IupSetAttribute(matrix, "HEIGHT0", "8");
  IupSetAttribute(matrix, "ALIGNMENT0", "ACENTER");

  IupSetAttribute(matrix, "SCROLLBAR", "YES");

  initData();

  IupSetCallback(matrix, "VALUE_CB", (Icallback)value_cb);
  IupSetCallback(matrix, "VALUE_EDIT_CB", (Icallback)value_edit_cb);
  //IupSetCallback(matrix, "EDITION_CB", (Icallback)edition_cb);
  //IupSetCallback(matrix, "TRANSLATEVALUE_CB", (Icallback)translatevalue_cb);
  //IupSetCallback(matrix, "VALUECHANGED_CB", (Icallback)valuechanged_cb);

  dlg = IupDialog(matrix);
  IupSetAttribute(dlg, "TITLE", "Cells");
  IupSetAttribute(dlg, "SIZE", "300x150");

  IupShowXY(dlg, IUP_CENTER, IUP_CENTER);

  IupMainLoop();

  IupClose();

  return EXIT_SUCCESS;
}
