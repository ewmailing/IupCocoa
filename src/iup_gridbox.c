/** \file
 * \brief GridBox Control.
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdio.h>
#include <stdlib.h>

#include "iup.h"
#include "iupcbs.h"

#include "iup_object.h"
#include "iup_attrib.h"
#include "iup_str.h"
#include "iup_drv.h"
#include "iup_drvfont.h"
#include "iup_stdcontrols.h"
#include "iup_layout.h"
#include "iup_normalizer.h"


/* Orientation */
enum {IGBOX_HORIZONTAL, IGBOX_VERTICAL};

struct _IcontrolData 
{
  int alignment_lin, alignment_col,
      expand_children,
      is_homogeneous_lin, is_homogeneous_col,
      normalize_size,
      margin_x, margin_y,
      size_lin, size_col,
      gap_lin, gap_col,
      orientation, num_div, num_lin, num_col,
      total_natural_width, total_natural_height,   /* calculated in ComputeNaturalSize, used in SetChildrenCurrentSize */
      homogeneous_width, homogeneous_height;       /* calculated in SetChildrenCurrentSize, used in SetChildrenPosition */
};


static void iGridBoxCalcLinCol(Ihandle* ih, int i, int *lin, int *col)
{
  if (ih->data->orientation == IGBOX_HORIZONTAL)
  {
    *lin = i / ih->data->num_col;
    *col = i % ih->data->num_col;
  }
  else
  {
    *col = i / ih->data->num_lin;
    *lin = i % ih->data->num_lin;
  }
}


/**********************************************************************************************/


static char* iGridBoxGetClientSizeAttrib(Ihandle* ih)
{
  int width = ih->currentwidth;
  int height = ih->currentheight;
  width -= 2 * ih->data->margin_x;
  height -= 2*ih->data->margin_y;
  if (width < 0) width = 0;
  if (height < 0) height = 0;
  return iupStrReturnIntInt(width, height, 'x');
}

static int iGridBoxSetCGapColAttrib(Ihandle* ih, const char* value)
{
  int cgap, charheight;
  iupStrToInt(value, &cgap);
  iupdrvFontGetCharSize(ih, NULL, &charheight);
  ih->data->gap_col = iupHEIGHT2RASTER(cgap, charheight);
  return 0;
}

static int iGridBoxSetCGapLinAttrib(Ihandle* ih, const char* value)
{
  int cgap, charwidth;
  iupStrToInt(value, &cgap);
  iupdrvFontGetCharSize(ih, &charwidth, NULL);
  ih->data->gap_lin = iupWIDTH2RASTER(cgap, charwidth);
  return 0;
}

static char* iGridBoxGetCGapLinAttrib(Ihandle* ih)
{
  int charwidth;
  iupdrvFontGetCharSize(ih, &charwidth, NULL);
  return iupStrReturnInt(iupRASTER2WIDTH(ih->data->gap_lin, charwidth));
}

static char* iGridBoxGetCGapColAttrib(Ihandle* ih)
{
  int charheight;
  iupdrvFontGetCharSize(ih, NULL, &charheight);
  return iupStrReturnInt(iupRASTER2HEIGHT(ih->data->gap_col, charheight));
}

static int iGridBoxSetGapLinAttrib(Ihandle* ih, const char* value)
{
  iupStrToInt(value, &ih->data->gap_lin);
  return 0;
}

static char* iGridBoxGetGapLinAttrib(Ihandle* ih)
{
  return iupStrReturnInt(ih->data->gap_lin);
}

static int iGridBoxSetGapColAttrib(Ihandle* ih, const char* value)
{
  iupStrToInt(value, &ih->data->gap_col);
  return 0;
}

static char* iGridBoxGetGapColAttrib(Ihandle* ih)
{
  return iupStrReturnInt(ih->data->gap_col);
}

static int iGridBoxSetSizeLinAttrib(Ihandle* ih, const char* value)
{
  iupStrToInt(value, &ih->data->size_lin);
  return 0;
}

static char* iGridBoxGetSizeLinAttrib(Ihandle* ih)
{
  return iupStrReturnInt(ih->data->size_lin);
}

static int iGridBoxSetSizeColAttrib(Ihandle* ih, const char* value)
{
  iupStrToInt(value, &ih->data->size_col);
  return 0;
}

static char* iGridBoxGetSizeColAttrib(Ihandle* ih)
{
  return iupStrReturnInt(ih->data->size_col);
}

static char* iGridBoxGetHomogeneousLinAttrib(Ihandle* ih)
{
  return iupStrReturnBoolean (ih->data->is_homogeneous_lin); 
}

static int iGridBoxSetHomogeneousLinAttrib(Ihandle* ih, const char* value)
{
  if (iupStrBoolean(value))
    ih->data->is_homogeneous_lin = 1;
  else
    ih->data->is_homogeneous_lin = 0;
  return 0;
}

static char* iGridBoxGetHomogeneousColAttrib(Ihandle* ih)
{
  return iupStrReturnBoolean (ih->data->is_homogeneous_col); 
}

static int iGridBoxSetHomogeneousColAttrib(Ihandle* ih, const char* value)
{
  if (iupStrBoolean(value))
    ih->data->is_homogeneous_col = 1;
  else
    ih->data->is_homogeneous_col = 0;
  return 0;
}

static int iGridBoxSetExpandChildrenAttrib(Ihandle* ih, const char* value)
{
  if (iupStrEqualNoCase(value, "YES"))
    ih->data->expand_children = IUP_EXPAND_BOTH;
  else if (iupStrEqualNoCase(value, "HORIZONTAL"))
    ih->data->expand_children = IUP_EXPAND_WIDTH;
  else if (iupStrEqualNoCase(value, "VERTICAL"))
    ih->data->expand_children = IUP_EXPAND_HEIGHT;
  else
    ih->data->expand_children = IUP_EXPAND_NONE;
  return 0;
}

static char* iGridBoxGetExpandChildrenAttrib(Ihandle* ih)
{
  if ((ih->data->expand_children & IUP_EXPAND_WIDTH) && (ih->data->expand_children & IUP_EXPAND_HEIGHT))
    return "YES";
  else if (ih->data->expand_children & IUP_EXPAND_WIDTH)
    return "HORIZONTAL";
  else if (ih->data->expand_children & IUP_EXPAND_HEIGHT)
    return "VERTICAL";
  else
    return "NO";
}

static int iGridBoxSetNormalizeSizeAttrib(Ihandle* ih, const char* value)
{
  ih->data->normalize_size = iupNormalizeGetNormalizeSize(value);
  return 0;
}

static char* iGridBoxGetNormalizeSizeAttrib(Ihandle* ih)
{
  return iupNormalizeGetNormalizeSizeStr(ih->data->normalize_size);
}

static void iGridBoxFitColChildren(Ihandle* ih, int fit_col)
{
  /* find the largest cel in the col */
  int max_width = 0, max, i;
  Ihandle* child, *ref_child = NULL;

  if (ih->data->num_col == 0 || ih->data->num_lin == 0)
    return;

  i = 0;
  for (child = ih->firstchild; child; child = child->brother)
  {
    if (!(child->flags & IUP_FLOATING))
    {
      int lin, col;
      iGridBoxCalcLinCol(ih, i, &lin, &col);

      if (col == fit_col)
      {
        if (child->naturalwidth > max_width)
          max_width = child->naturalwidth;

        if (lin == ih->data->size_lin)
          ref_child = child;
      }

      i++;
    }
  }

  max = iupAttribGetIntId(ih, "FITMAXWIDTH", fit_col);
  if (max && max > max_width)
    max_width = max;

  if (ref_child)
    ref_child->userwidth = max_width;
}

static void iGridBoxFitLineChildren(Ihandle* ih, int fit_lin)
{
  /* find the highest cel in the line */
  int max_height = 0, max, i;
  Ihandle* child, *ref_child = NULL;

  if (ih->data->num_col == 0 || ih->data->num_lin == 0)
    return;

  i = 0;
  for (child = ih->firstchild; child; child = child->brother)
  {
    if (!(child->flags & IUP_FLOATING))
    {
      int lin, col;
      iGridBoxCalcLinCol(ih, i, &lin, &col);

      if (lin == fit_lin)
      {
        if (child->naturalheight > max_height)
          max_height = child->naturalheight;

        if (col == ih->data->size_col)
          ref_child = child;
      }

      i++;
    }
  }

  max = iupAttribGetIntId(ih, "FITMAXHEIGHT", fit_lin);
  if (max && max > max_height)
    max_height = max;

  if (ref_child)
    ref_child->userheight = max_height;
}

static int iGridBoxSetFitToChildrenAttrib(Ihandle* ih, const char* value)
{
  if (!value || value[0] == 0)
    return 0;

  if (value[0] == 'C')
  {
    int col;
    if (iupStrToInt(value + 1, &col))
    {
      iGridBoxFitColChildren(ih, col);
      IupRefreshChildren(ih);
    }
  }
  else if (value[0] == 'L')
  {
    int line;
    if (iupStrToInt(value + 1, &line))
    {
      iGridBoxFitLineChildren(ih, line);
      IupRefreshChildren(ih);
    }
  }

  return 0;
}

static int iGridBoxSetCMarginAttrib(Ihandle* ih, const char* value)
{
  int cmargin_x=-1, cmargin_y=-1;
  int charwidth, charheight;
  iupdrvFontGetCharSize(ih, &charwidth, &charheight);
  iupStrToIntInt(value, &cmargin_x, &cmargin_y, 'x');
  if (cmargin_x!=-1)
    ih->data->margin_x = iupWIDTH2RASTER(cmargin_x, charwidth);
  if (cmargin_y!=-1)
    ih->data->margin_y = iupHEIGHT2RASTER(cmargin_y, charheight);
  return 1;
}

static char* iGridBoxGetCMarginAttrib(Ihandle* ih)
{
  int charwidth, charheight;
  iupdrvFontGetCharSize(ih, &charwidth, &charheight);
  return iupStrReturnIntInt(iupRASTER2WIDTH(ih->data->margin_x, charwidth), iupRASTER2HEIGHT(ih->data->margin_y, charheight), 'x');
}

static int iGridBoxSetMarginAttrib(Ihandle* ih, const char* value)
{
  iupStrToIntInt(value, &ih->data->margin_x, &ih->data->margin_y, 'x');
  return 0;
}

static char* iGridBoxGetMarginAttrib(Ihandle* ih)
{
  return iupStrReturnIntInt(ih->data->margin_x, ih->data->margin_y, 'x');
}

static int iGridBoxSetOrientationAttrib(Ihandle* ih, const char* value)
{
  if (iupStrEqualNoCase(value, "VERTICAL"))
    ih->data->orientation = IGBOX_VERTICAL;
  else if (iupStrEqualNoCase(value, "HORIZONTAL"))
    ih->data->orientation = IGBOX_HORIZONTAL;
  return 0;
}

static char* iGridBoxGetOrientationAttrib(Ihandle* ih)
{
  if (ih->data->orientation==IGBOX_HORIZONTAL)
    return "HORIZONTAL";
  else
    return "VERTICAL";
}

static int iGridBoxSetNumDivAttrib(Ihandle* ih, const char* value)
{
  if (iupStrEqualNoCase(value, "AUTO"))
    ih->data->num_div = -1;
  else
  {
    iupStrToInt(value, &ih->data->num_div);
    if (ih->data->num_div <= 0)
      ih->data->num_div = 1;
  }
  return 0;
}

static char* iGridBoxGetNumDivAttrib(Ihandle* ih)
{
  return iupStrReturnInt(ih->data->num_div);
}

static char* iGridBoxGetNumColAttrib(Ihandle* ih)
{
  if (!ih->handle)
    iupBaseComputeNaturalSize(ih);

  return iupStrReturnInt(ih->data->num_col);
}

static char* iGridBoxGetNumLinAttrib(Ihandle* ih)
{
  if (!ih->handle)
    iupBaseComputeNaturalSize(ih);

  return iupStrReturnInt(ih->data->num_lin);
}

static int iGridBoxGetAlignmentLin(Ihandle* ih, int lin)
{
  int alignment_lin = ih->data->alignment_lin;
  char *value;
  value = iupAttribGetId(ih, "ALIGNMENTLIN", lin);
  if (value)
  {
    if (iupStrEqualNoCase(value, "ABOTTOM"))
      alignment_lin = IUP_ALIGN_ABOTTOM;
    else if (iupStrEqualNoCase(value, "ACENTER"))
      alignment_lin = IUP_ALIGN_ACENTER;
    else
      alignment_lin = IUP_ALIGN_ATOP;
  }
  return alignment_lin;
}

static int iGridBoxGetAlignmentCol(Ihandle* ih, int col)
{
  int alignment_col = ih->data->alignment_col;
  char *value;
  value = iupAttribGetId(ih, "ALIGNMENTCOL", col);
  if (value)
  {
    if (iupStrEqualNoCase(value, "ARIGHT"))
      alignment_col = IUP_ALIGN_ARIGHT;
    else if (iupStrEqualNoCase(value, "ACENTER"))
      alignment_col = IUP_ALIGN_ACENTER;
    else if (iupStrEqualNoCase(value, "ALEFT"))
      alignment_col = IUP_ALIGN_ALEFT;
  }
  return alignment_col;
}

static int iGridBoxSetAlignmentLinAttrib(Ihandle* ih, const char* value)
{
  if (iupStrEqualNoCase(value, "ABOTTOM"))
    ih->data->alignment_lin = IUP_ALIGN_ABOTTOM;
  else if (iupStrEqualNoCase(value, "ACENTER"))
    ih->data->alignment_lin = IUP_ALIGN_ACENTER;
  else if (iupStrEqualNoCase(value, "ATOP"))
    ih->data->alignment_lin = IUP_ALIGN_ATOP;
 return 0;
}

static char* iGridBoxGetAlignmentLinAttrib(Ihandle* ih)
{
  static char* align2str[3] = {"ATOP", "ACENTER", "ABOTTOM"};
  return align2str[ih->data->alignment_lin];
}

static int iGridBoxSetAlignmentColAttrib(Ihandle* ih, const char* value)
{
  if (iupStrEqualNoCase(value, "ARIGHT"))
    ih->data->alignment_col = IUP_ALIGN_ARIGHT;
  else if (iupStrEqualNoCase(value, "ACENTER"))
    ih->data->alignment_col = IUP_ALIGN_ACENTER;
  else if (iupStrEqualNoCase(value, "ALEFT"))
    ih->data->alignment_col = IUP_ALIGN_ALEFT;
  return 0;
}

static char* iGridBoxGetAlignmentColAttrib(Ihandle* ih)
{
  char* align2str[3] = {"ALEFT", "ACENTER", "ARIGHT"};
  return iupStrReturnStr(align2str[ih->data->alignment_col]);
}

static int iGridBoxUpdateAttribFromFont(Ihandle* ih)
{
  char* value = iupAttribGet(ih, "CMARGIN");
  if (!value)
    value = iupAttribGet(ih, "NCMARGIN");
  if (value)
    iGridBoxSetCMarginAttrib(ih, value);

  value = iupAttribGet(ih, "CGAPLIN");
  if (!value)
    value = iupAttribGet(ih, "NCGAPLIN");
  if (value)
    iGridBoxSetCGapLinAttrib(ih, value);

  value = iupAttribGet(ih, "CGAPCOL");
  if (!value)
    value = iupAttribGet(ih, "NCGAPCOL");
  if (value)
    iGridBoxSetCGapColAttrib(ih, value);

  return IUP_DEFAULT;
}


/**********************************************************************************/


static int iGridBoxCalcNumDiv(Ihandle* ih)
{
  int num_div;
  Ihandle* child;

  if (ih->data->orientation==IGBOX_HORIZONTAL)
  {
    int children_w = 0;

    num_div = 1;
    for (child = ih->firstchild; child; child = child->brother)
    {
      if (!(child->flags & IUP_FLOATING))
      {
        children_w += child->naturalwidth;
        if (children_w > ih->naturalwidth)
          break;

        num_div++;
      }
    }
  }
  else
  {
    int children_h = 0;

    num_div = 1;
    for (child = ih->firstchild; child; child = child->brother)
    {
      if (!(child->flags & IUP_FLOATING))
      {
        children_h += child->naturalheight;
        if (children_h > ih->naturalheight)
          break;

        num_div++;
      }
    }
  }

  return num_div;
}

static void iGridBoxComputeNaturalSizeMethod(Ihandle* ih, int *w, int *h, int *children_expand)
{
  Ihandle* child;
  int num_lin, num_col, num_div, i;
  int children_natural_width = 0, children_natural_height = 0;

  /* calculate total children natural size */
  int children_natural_maxwidth = 0;
  int children_natural_maxheight = 0;
  int child_count = 0;

  for (child = ih->firstchild; child; child = child->brother)
  {
    if (ih->data->expand_children)
      child->expand = ih->data->expand_children;

    /* update child natural size first */
    if (!(child->flags & IUP_FLOATING_IGNORE))
      iupBaseComputeNaturalSize(child);

    if (!(child->flags & IUP_FLOATING))
    {
      *children_expand |= child->expand;
      child_count++;
    }
  }

  if (ih->data->num_div == -1)
    num_div = iGridBoxCalcNumDiv(ih);
  else
    num_div = ih->data->num_div;

  /* Notice that num_lin*num_col can be > child_count */
  if (ih->data->orientation==IGBOX_HORIZONTAL)
  {
    num_lin = child_count / num_div;
    if (child_count % num_div)
      num_lin++;
    num_col = num_div;
  }
  else
  {
    num_lin = num_div;
    num_col = child_count / num_div;
    if (child_count % num_div)
      num_col++;
  }

  /* read only, computed only here */
  ih->data->num_lin = num_lin;
  ih->data->num_col = num_col;

  /* IMPORTANT: the reference line defines the width of each column,
     and the reference column defines the height of each line */

  i = 0;
  for (child = ih->firstchild; child; child = child->brother)
  {
    if (!(child->flags & IUP_FLOATING))
    {
      int lin, col;
      iGridBoxCalcLinCol(ih, i, &lin, &col);

      if (col == ih->data->size_col)
      {
        children_natural_height += child->naturalheight;
        children_natural_maxheight = iupMAX(children_natural_maxheight, child->naturalheight);
      }
      if (lin == ih->data->size_lin)
      {
        children_natural_width += child->naturalwidth;
        children_natural_maxwidth = iupMAX(children_natural_maxwidth, child->naturalwidth);
      }

      i++;
    }
  }

  /* reset to max natural width and/or height if NORMALIZESIZE is defined */
  if (ih->data->normalize_size)  /* this directly affects the child natural size */
  {
    iupNormalizeSizeBoxChild(ih, ih->data->normalize_size, children_natural_maxwidth, children_natural_maxheight);

    /* must update the children natural size because
       normalize was done after its calculation */
    if (ih->data->normalize_size & 1) 
      children_natural_width = children_natural_maxwidth*num_col;
    if (ih->data->normalize_size & 2)
      children_natural_height = children_natural_maxheight*num_lin;
  }

  /* leave room at the element for the maximum natural size of the children when is_homogeneous */
  /* this affects only the space left for the child to expand */
  if (ih->data->is_homogeneous_col)  /* all columns with the same width */
    children_natural_width = children_natural_maxwidth*num_col;
  if (ih->data->is_homogeneous_lin)  /* all lines with the same height */
    children_natural_height = children_natural_maxheight*num_lin;

  /* compute the GridBox contents natural size */
  *w = children_natural_width  + (num_col-1)*ih->data->gap_col + 2*ih->data->margin_x;
  *h = children_natural_height + (num_lin-1)*ih->data->gap_lin + 2*ih->data->margin_y;

  /* Store to be used in iGridCalcEmptyHeight */
  ih->data->total_natural_width  = *w;
  ih->data->total_natural_height = *h;
}

static int iGridBoxCalcHomogeneousWidth(Ihandle *ih)
{
  /* all columns with the same width */
  int homogeneous_width = (ih->currentwidth - (ih->data->num_col-1)*ih->data->gap_col - 2*ih->data->margin_x)/ih->data->num_col;
  if (homogeneous_width<0) homogeneous_width = 0;
  return homogeneous_width;
}

static int iGridBoxCalcHomogeneousHeight(Ihandle *ih)
{
  /* all lines with the same height */
  int homogeneous_height = (ih->currentheight - (ih->data->num_lin-1)*ih->data->gap_lin - 2*ih->data->margin_y)/ih->data->num_lin;
  if (homogeneous_height < 0) homogeneous_height = 0;
  return homogeneous_height;
}

static int iGridBoxCalcEmptyWidth(Ihandle *ih, int expand)
{
  /* This is the space that the child can be expanded. */
  Ihandle* child;
  int empty_width;

  int expand_count=0, i=0;
  for (child = ih->firstchild; child; child = child->brother)
  {
    if (!(child->flags & IUP_FLOATING))
    {
      int lin, col;
      iGridBoxCalcLinCol(ih, i, &lin, &col);

      /* only for the reference line,
         the reference line defines the width of each column */
      if (lin==ih->data->size_lin &&
          child->expand & expand)
        expand_count++;

      i++;
    }
  }
  if (expand_count == 0)
    return 0;

  /* equal spaces for all expandable elements */
  empty_width = (ih->currentwidth - ih->data->total_natural_width)/expand_count;
  if (empty_width < 0) empty_width = 0;
  return empty_width;
}

static int iGridBoxCalcEmptyHeight(Ihandle *ih, int expand)
{
  /* This is the space that the child can be expanded. */
  Ihandle* child;
  int empty_height;

  int expand_count=0, i=0;
  for (child = ih->firstchild; child; child = child->brother)
  {
    if (!(child->flags & IUP_FLOATING))
    {
      int lin, col;
      iGridBoxCalcLinCol(ih, i, &lin, &col);

      /* only for the reference column,
         the reference column defines the height of each line */
      if (col==ih->data->size_col &&
         child->expand & expand)
        expand_count++;

      i++;
    }
  }
  if (expand_count == 0)
    return 0;

  /* equal spaces for all expandable elements */
  empty_height = (ih->currentheight - ih->data->total_natural_height)/expand_count;  
  if (empty_height < 0) empty_height = 0;
  return empty_height;
}

static void iGridBoxCalcColWidth(Ihandle* ih, int *col_width, int empty_w0, int empty_w1)
{
  Ihandle* child;

  int i=0;
  for (child = ih->firstchild; child; child = child->brother)
  {
    if (!(child->flags & IUP_FLOATING))
    {
      int lin, col;
      iGridBoxCalcLinCol(ih, i, &lin, &col);

      /* only for the reference line,
         the reference line defines the width of each column */
      if (lin==ih->data->size_lin)
      {
        int empty_width = (child->expand & IUP_EXPAND_W1)? empty_w1: ((child->expand & IUP_EXPAND_W0)? empty_w0: 0);
        col_width[col] = child->naturalwidth+empty_width;
      }

      i++;
    }
  }
}

static void iGridBoxCalcLineHeight(Ihandle* ih, int *line_height, int empty_h0, int empty_h1)
{
  Ihandle* child;

  int i=0;
  for (child = ih->firstchild; child; child = child->brother)
  {
    if (!(child->flags & IUP_FLOATING))
    {
      int lin, col;
      iGridBoxCalcLinCol(ih, i, &lin, &col);

      /* only for the reference column,
         the reference column defines the height of each line */
      if (col==ih->data->size_col)
      {
        int empty_height = (child->expand & IUP_EXPAND_H1)? empty_h1: ((child->expand & IUP_EXPAND_H0)? empty_h0: 0);
        line_height[lin] = child->naturalheight+empty_height;
      }

      i++;
    }
  }
}

static void iGridBoxSetChildrenCurrentSizeMethod(Ihandle* ih, int shrink)
{
  Ihandle* child;
  int *line_height=NULL, *col_width=NULL, i;

  if (ih->data->is_homogeneous_lin)
    ih->data->homogeneous_height = iGridBoxCalcHomogeneousHeight(ih);
  else
  {
    int empty_h0 = 0, empty_h1 = 0;

    ih->data->homogeneous_height = 0;

    /* must calculate the space left for each control to grow inside the container */
    /* H1 means there is an EXPAND enabled inside */
    if (ih->expand & IUP_EXPAND_H1)
      empty_h1 = iGridBoxCalcEmptyHeight(ih, IUP_EXPAND_H1);
    /* Not H1 and H0 means that EXPAND is not enabled, but there are some IupFill inside */
    else if (ih->expand & IUP_EXPAND_H0)
      empty_h0 = iGridBoxCalcEmptyHeight(ih, IUP_EXPAND_H0);

    line_height = (int*)malloc(ih->data->num_lin * sizeof(int));
    iGridBoxCalcLineHeight(ih, line_height, empty_h0, empty_h1);
  }

  if (ih->data->is_homogeneous_col)
    ih->data->homogeneous_width = iGridBoxCalcHomogeneousWidth(ih);
  else
  {
    int empty_w0 = 0, empty_w1 = 0;

    ih->data->homogeneous_width = 0;

    /* must calculate the space left for each control to grow inside the container */
    /* W1 means there is an EXPAND enabled inside */
    if (ih->expand & IUP_EXPAND_W1)
      empty_w1 = iGridBoxCalcEmptyWidth(ih, IUP_EXPAND_W1);
    /* Not W1 and W0 means that EXPAND is not enabled but there are some IupFill inside */
    else if (ih->expand & IUP_EXPAND_W0)
      empty_w0 = iGridBoxCalcEmptyWidth(ih, IUP_EXPAND_W0);

    col_width = (int*)malloc(ih->data->num_col * sizeof(int));
    iGridBoxCalcColWidth(ih, col_width, empty_w0, empty_w1);
  }

  i = 0;
  for (child = ih->firstchild; child; child = child->brother)
  {
    if (!(child->flags & IUP_FLOATING))
    {
      int lin, col;
      iGridBoxCalcLinCol(ih, i, &lin, &col);

      if (ih->data->homogeneous_width && ih->data->homogeneous_height)
        iupBaseSetCurrentSize(child, ih->data->homogeneous_width, ih->data->homogeneous_height, shrink);
      else if (ih->data->homogeneous_width)
        iupBaseSetCurrentSize(child, ih->data->homogeneous_width, line_height[lin], shrink);
      else if (ih->data->homogeneous_height)
        iupBaseSetCurrentSize(child, col_width[col], ih->data->homogeneous_height, shrink);
      else
        iupBaseSetCurrentSize(child, col_width[col], line_height[lin], shrink);

      i++;
    }
    else if (!(child->flags & IUP_FLOATING_IGNORE))
    {
      /* update children to their own natural size */
      iupBaseSetCurrentSize(child, child->naturalwidth, child->naturalheight, shrink);
    }
  }

  if (line_height)
    free(line_height);
  if (col_width)
    free(col_width);
}

static void iGridBoxSetChildrenPositionMethod(Ihandle* ih, int x, int y)
{
  int dx, dy, i, *line_pos, 
      lin, col, *col_pos,
      *alignment_col, *alignment_lin, *col_width, *line_height;
  Ihandle* child;

  col_pos = (int*)malloc(ih->data->num_col *sizeof(int));
  line_pos = (int*)malloc(ih->data->num_lin *sizeof(int));
  alignment_col = (int*)malloc(ih->data->num_col *sizeof(int));
  alignment_lin = (int*)malloc(ih->data->num_lin *sizeof(int));
  col_width = (int*)malloc(ih->data->num_col *sizeof(int));
  line_height = (int*)malloc(ih->data->num_lin *sizeof(int));

  i = 0;
  for (child = ih->firstchild; child; child = child->brother)
  {
    if (!(child->flags & IUP_FLOATING))
    {
      iGridBoxCalcLinCol(ih, i, &lin, &col);

      /* only for the reference line,
         the reference line defines the width of each column */
      if (lin==ih->data->size_lin)
      {
        if (ih->data->homogeneous_width)
          col_width[col] = ih->data->homogeneous_width + ih->data->gap_col;
        else
          col_width[col] = child->currentwidth + ih->data->gap_col;

        alignment_col[col] = iGridBoxGetAlignmentCol(ih, col);
      }

      /* only for the reference column,
         the reference column defines the height of each line */
      if (col==ih->data->size_col)
      {
        if (ih->data->homogeneous_height)
          line_height[lin] = ih->data->homogeneous_height + ih->data->gap_lin;
        else
          line_height[lin] = child->currentheight + ih->data->gap_lin;

        alignment_lin[lin] = iGridBoxGetAlignmentLin(ih, lin);
      }

      i++;
    }
  }

  for (col=0; col<ih->data->num_col; col++)
  {
    if (col==0)
      col_pos[col] = ih->data->margin_x;
    else
      col_pos[col] = col_pos[col-1] + col_width[col-1];
  }

  for (lin=0; lin<ih->data->num_lin; lin++)
  {
    if (lin==0)
      line_pos[lin] = ih->data->margin_y;
    else
      line_pos[lin] = line_pos[lin-1] + line_height[lin-1];
  }

  i = 0;
  for (child = ih->firstchild; child; child = child->brother)
  {
    if (!(child->flags & IUP_FLOATING))
    {
      iGridBoxCalcLinCol(ih, i, &lin, &col);

      if (alignment_lin[lin] == IUP_ALIGN_ACENTER)
        dy = (line_height[lin] - child->currentheight)/2;
      else if (alignment_lin[lin] == IUP_ALIGN_ABOTTOM)
        dy = line_height[lin] - child->currentheight;
      else  /* IUP_ALIGN_ATOP */
        dy = 0;
      if (dy<0) dy = 0;

      if (alignment_col[col] == IUP_ALIGN_ACENTER)
        dx = (col_width[col] - child->currentwidth)/2;
      else if (alignment_col[col] == IUP_ALIGN_ARIGHT)
        dx = col_width[col] - child->currentwidth;
      else  /* IUP_ALIGN_ALEFT */
        dx = 0;
      if (dx<0) dx = 0;

      /* update child */
      iupBaseSetPosition(child, x+col_pos[col]+dx, y+line_pos[lin]+dy);

      i++;
    }
  }

  free(alignment_col);
  free(alignment_lin);
  free(col_width);
  free(line_height);
  free(col_pos);
  free(line_pos);
}

static int iGridBoxCreateMethod(Ihandle* ih, void** params)
{
  ih->data = iupALLOCCTRLDATA();

  if (params)
  {
    Ihandle** iparams = (Ihandle**)params;
    while (*iparams) 
    {
      IupAppend(ih, *iparams);
      iparams++;
    }
  }

  ih->data->num_div = -1; /* default is auto */

  IupSetCallback(ih, "UPDATEATTRIBFROMFONT", iGridBoxUpdateAttribFromFont);

  return IUP_NOERROR;
}


/******************************************************************************/


Ihandle *IupGridBoxv(Ihandle **children)
{
  return IupCreatev("gridbox", (void**)children);
}

Ihandle *IupGridBox(Ihandle* child, ...)
{
  Ihandle **children;
  Ihandle *ih;

  va_list arglist;
  va_start(arglist, child);
  children = (Ihandle **)iupObjectGetParamList(child, arglist);
  va_end(arglist);

  ih = IupCreatev("gridbox", (void**)children);
  free(children);

  return ih;
}

Iclass* iupGridBoxNewClass(void)
{
  Iclass* ic = iupClassNew(NULL);

  ic->name = "gridbox";
  ic->format = "g"; /* array of Ihandle */
  ic->nativetype = IUP_TYPEVOID;
  ic->childtype = IUP_CHILDMANY;
  ic->is_interactive = 0;

  /* Class functions */
  ic->New = iupGridBoxNewClass;
  ic->Create = iGridBoxCreateMethod;
  ic->Map = iupBaseTypeVoidMapMethod;
  ic->ComputeNaturalSize = iGridBoxComputeNaturalSizeMethod;
  ic->SetChildrenCurrentSize = iGridBoxSetChildrenCurrentSizeMethod;
  ic->SetChildrenPosition = iGridBoxSetChildrenPositionMethod;

  /* Common */
  iupBaseRegisterCommonAttrib(ic);

  /* Base Container */
  iupClassRegisterAttribute(ic, "EXPAND", iupBaseContainerGetExpandAttrib, NULL, IUPAF_SAMEASSYSTEM, "YES", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CLIENTSIZE", iGridBoxGetClientSizeAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "CLIENTOFFSET", iupBaseGetClientOffsetAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  /* boxes */
  iupClassRegisterAttribute(ic, "MARGIN", iGridBoxGetMarginAttrib, iGridBoxSetMarginAttrib, IUPAF_SAMEASSYSTEM, "0x0", IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "CMARGIN", iGridBoxGetCMarginAttrib, iGridBoxSetCMarginAttrib, IUPAF_SAMEASSYSTEM, "0x0", IUPAF_NO_SAVE|IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "NMARGIN", iGridBoxGetMarginAttrib, iGridBoxSetMarginAttrib, IUPAF_SAMEASSYSTEM, "0x0", IUPAF_NO_SAVE|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "NCMARGIN", iGridBoxGetCMarginAttrib, iGridBoxSetCMarginAttrib, IUPAF_SAMEASSYSTEM, "0x0", IUPAF_NO_SAVE|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "EXPANDCHILDREN", iGridBoxGetExpandChildrenAttrib, iGridBoxSetExpandChildrenAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "HOMOGENEOUSLIN", iGridBoxGetHomogeneousLinAttrib, iGridBoxSetHomogeneousLinAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "HOMOGENEOUSCOL", iGridBoxGetHomogeneousColAttrib, iGridBoxSetHomogeneousColAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "NORMALIZESIZE", iGridBoxGetNormalizeSizeAttrib, iGridBoxSetNormalizeSizeAttrib, IUPAF_SAMEASSYSTEM, "NONE", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  /* GridBox only */
  iupClassRegisterAttribute(ic, "GAPLIN", iGridBoxGetGapLinAttrib, iGridBoxSetGapLinAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "CGAPLIN", iGridBoxGetCGapLinAttrib, iGridBoxSetCGapLinAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NO_SAVE|IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "GAPCOL", iGridBoxGetGapColAttrib, iGridBoxSetGapColAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "CGAPCOL", iGridBoxGetCGapColAttrib, iGridBoxSetCGapColAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NO_SAVE|IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "NGAPLIN", iGridBoxGetGapLinAttrib, iGridBoxSetGapLinAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NO_SAVE|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "NCGAPLIN", iGridBoxGetCGapLinAttrib, iGridBoxSetCGapLinAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NO_SAVE|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "NGAPCOL", iGridBoxGetGapColAttrib, iGridBoxSetGapColAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NO_SAVE|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "NCGAPCOL", iGridBoxGetCGapColAttrib, iGridBoxSetCGapColAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NO_SAVE|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);

  iupClassRegisterAttribute(ic, "ALIGNMENTLIN", iGridBoxGetAlignmentLinAttrib, iGridBoxSetAlignmentLinAttrib, IUPAF_SAMEASSYSTEM, "ATOP", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ALIGNMENTCOL", iGridBoxGetAlignmentColAttrib, iGridBoxSetAlignmentColAttrib, IUPAF_SAMEASSYSTEM, "ALEFT", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "ORIENTATION", iGridBoxGetOrientationAttrib, iGridBoxSetOrientationAttrib, NULL, NULL, IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "NUMDIV", iGridBoxGetNumDivAttrib, iGridBoxSetNumDivAttrib, IUPAF_SAMEASSYSTEM, "AUTO", IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "NUMCOL", iGridBoxGetNumColAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "NUMLIN", iGridBoxGetNumLinAttrib, NULL, NULL, NULL, IUPAF_READONLY|IUPAF_NOT_MAPPED|IUPAF_NO_INHERIT);
  iupClassRegisterAttribute(ic, "SIZELIN", iGridBoxGetSizeLinAttrib, iGridBoxSetSizeLinAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "SIZECOL", iGridBoxGetSizeColAttrib, iGridBoxSetSizeColAttrib, IUPAF_SAMEASSYSTEM, "0", IUPAF_NOT_MAPPED);
  iupClassRegisterAttribute(ic, "FITTOCHILDREN", NULL, iGridBoxSetFitToChildrenAttrib, NULL, NULL, IUPAF_NOT_MAPPED | IUPAF_WRITEONLY | IUPAF_NO_INHERIT);

  return ic;
}
