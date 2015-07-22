/***************************************************************************
 * cont.h is part of Math Graphic Library
 * Copyright (C) 2007-2014 Alexey Balakin <mathgl.abalakin@gmail.ru>       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef _MGL_CONT_H_
#define _MGL_CONT_H_
#include "mgl2/abstract.h"
//-----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

/// Print text along the curve in parametric form {x,y,z}
void MGL_EXPORT mgl_text_xyz(HMGL gr, HCDT x, HCDT y, HCDT z, const char *text, const char *font, const char *opt);
void MGL_EXPORT mgl_text_xyz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z,const char *text,const char *font, const char *opt,int,int l,int n);
void MGL_EXPORT mgl_textw_xyz(HMGL gr, HCDT x, HCDT y, HCDT z, const wchar_t *text, const char *font, const char *opt);
/// Print text along the curve in parametric form {x,y}
void MGL_EXPORT mgl_text_xy(HMGL gr, HCDT x, HCDT y, const char *text, const char *font, const char *opt);
void MGL_EXPORT mgl_text_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, const char *text, const char *font, const char *opt,int, int l,int n);
void MGL_EXPORT mgl_textw_xy(HMGL gr, HCDT x, HCDT y, const wchar_t *text, const char *font, const char *opt);
/// Print text along the curve
void MGL_EXPORT mgl_text_y(HMGL gr, HCDT y, const char *text, const char *font, const char *opt);
void MGL_EXPORT mgl_text_y_(uintptr_t *gr, uintptr_t *y, const char *text, const char *font, const char *opt,int, int l,int n);
void MGL_EXPORT mgl_textw_y(HMGL gr, HCDT y, const wchar_t *text, const char *font, const char *opt);

void MGL_EXPORT mgl_cont_gen(HMGL gr, double val, HCDT a, HCDT x, HCDT y, HCDT z, const char *stl);
void MGL_EXPORT mgl_contf_gen(HMGL gr, double v1, double v2, HCDT a, HCDT x, HCDT y, HCDT z, const char *stl);
//void MGL_EXPORT mgl_contv_gen(HMGL gr, double v1, double v2, HCDT a, HCDT x, HCDT y, HCDT z, const char *stl);
//void MGL_EXPORT mgl_axial_gen(HMGL gr, double v1, double v2, HCDT a, HCDT x, HCDT y, HCDT z, const char *stl);

/// Draw manual contour lines for 2d data specified parametrically
void MGL_EXPORT mgl_cont_xy_val(HMGL gr, HCDT v, HCDT x, HCDT y, HCDT z, const char *sch, const char *opt);
void MGL_EXPORT mgl_cont_xy_val_(uintptr_t *gr, uintptr_t *v, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *sch, const char *opt,int,int);
/// Draw manual contour lines for 2d data
void MGL_EXPORT mgl_cont_val(HMGL gr, HCDT v, HCDT z, const char *sch, const char *opt);
void MGL_EXPORT mgl_cont_val_(uintptr_t *gr, uintptr_t *v, uintptr_t *z, const char *sch, const char *opt,int,int);
/// Draw contour lines for 2d data specified parametrically
void MGL_EXPORT mgl_cont_xy(HMGL gr, HCDT x, HCDT y, HCDT z, const char *sch, const char *opt);
void MGL_EXPORT mgl_cont_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *sch, const char *opt,int,int);
/// Draw contour lines for 2d data
void MGL_EXPORT mgl_cont(HMGL gr, HCDT z, const char *sch, const char *opt);
void MGL_EXPORT mgl_cont_(uintptr_t *gr, uintptr_t *z, const char *sch, const char *opt,int,int);

/// Draw manual solid contours for 2d data specified parametrically
void MGL_EXPORT mgl_contf_xy_val(HMGL gr, HCDT v, HCDT x, HCDT y, HCDT z, const char *sch, const char *opt);
void MGL_EXPORT mgl_contf_xy_val_(uintptr_t *gr, uintptr_t *v, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *sch, const char *opt,int,int);
/// Draw manual solid contours for 2d data
void MGL_EXPORT mgl_contf_val(HMGL gr, HCDT v, HCDT z, const char *sch, const char *opt);
void MGL_EXPORT mgl_contf_val_(uintptr_t *gr, uintptr_t *v, uintptr_t *z, const char *sch, const char *opt,int,int);
/// Draw solid contours for 2d data specified parametrically
void MGL_EXPORT mgl_contf_xy(HMGL gr, HCDT x, HCDT y, HCDT z, const char *sch, const char *opt);
void MGL_EXPORT mgl_contf_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *sch, const char *opt,int,int);
/// Draw solid contours for 2d data
void MGL_EXPORT mgl_contf(HMGL gr, HCDT z, const char *sch, const char *opt);
void MGL_EXPORT mgl_contf_(uintptr_t *gr, uintptr_t *z, const char *sch, const char *opt,int,int);

/// Draw manual solid contours for 2d data specified parametrically with manual colors
void MGL_EXPORT mgl_contd_xy_val(HMGL gr, HCDT v, HCDT x, HCDT y, HCDT z, const char *sch, const char *opt);
void MGL_EXPORT mgl_contd_xy_val_(uintptr_t *gr, uintptr_t *v, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *sch, const char *opt,int,int);
/// Draw manual solid contours for 2d data with manual colors
void MGL_EXPORT mgl_contd_val(HMGL gr, HCDT v, HCDT z, const char *sch, const char *opt);
void MGL_EXPORT mgl_contd_val_(uintptr_t *gr, uintptr_t *v, uintptr_t *z, const char *sch, const char *opt,int,int);
/// Draw solid contours for 2d data specified parametrically with manual colors
void MGL_EXPORT mgl_contd_xy(HMGL gr, HCDT x, HCDT y, HCDT z, const char *sch, const char *opt);
void MGL_EXPORT mgl_contd_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *sch, const char *opt,int,int);
/// Draw solid contours for 2d data with manual colors
void MGL_EXPORT mgl_contd(HMGL gr, HCDT z, const char *sch, const char *opt);
void MGL_EXPORT mgl_contd_(uintptr_t *gr, uintptr_t *z, const char *sch, const char *opt,int,int);

/// Draw manual contour tubes for 2d data specified parametrically
void MGL_EXPORT mgl_contv_xy_val(HMGL gr, HCDT v, HCDT x, HCDT y, HCDT z, const char *sch, const char *opt);
void MGL_EXPORT mgl_contv_xy_val_(uintptr_t *gr, uintptr_t *v, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *sch, const char *opt,int,int);
/// Draw manual contour tubes for 2d data
void MGL_EXPORT mgl_contv_val(HMGL gr, HCDT v, HCDT z, const char *sch, const char *opt);
void MGL_EXPORT mgl_contv_val_(uintptr_t *gr, uintptr_t *v, uintptr_t *z, const char *sch, const char *opt,int,int);
/// Draw contour tubes for 2d data specified parametrically
void MGL_EXPORT mgl_contv_xy(HMGL gr, HCDT x, HCDT y, HCDT z, const char *sch, const char *opt);
void MGL_EXPORT mgl_contv_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *sch, const char *opt,int,int);
/// Draw contour tubes for 2d data
void MGL_EXPORT mgl_contv(HMGL gr, HCDT z, const char *sch, const char *opt);
void MGL_EXPORT mgl_contv_(uintptr_t *gr, uintptr_t *z, const char *sch, const char *opt,int,int);

/// Draw manual axial-symmetric isosurfaces for 2d data specified parametrically
void MGL_EXPORT mgl_axial_xy_val(HMGL gr, HCDT v, HCDT x, HCDT y, HCDT a, const char *sch, const char *opt);
void MGL_EXPORT mgl_axial_xy_val_(uintptr_t *gr, uintptr_t *v, uintptr_t *x, uintptr_t *y, uintptr_t *a, const char *sch, const char *opt,int,int);
/// Draw manual axial-symmetric isosurfaces for 2d data
void MGL_EXPORT mgl_axial_val(HMGL gr, HCDT v, HCDT a, const char *sch, const char *opt);
void MGL_EXPORT mgl_axial_val_(uintptr_t *gr, uintptr_t *v, uintptr_t *a, const char *sch, const char *opt,int,int);
/// Draw axial-symmetric isosurfaces for 2d data specified parametrically
void MGL_EXPORT mgl_axial_xy(HMGL gr, HCDT x, HCDT y, HCDT a, const char *sch, const char *opt);
void MGL_EXPORT mgl_axial_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *a, const char *sch, const char *opt,int,int);
/// Draw axial-symmetric isosurfaces for 2d data
void MGL_EXPORT mgl_axial(HMGL gr, HCDT a, const char *sch, const char *opt);
void MGL_EXPORT mgl_axial_(uintptr_t *gr, uintptr_t *a, const char *sch, const char *opt,int,int);

/// Draw surface of curve {r,z} rotatation around axis
void MGL_EXPORT mgl_torus(HMGL gr, HCDT r, HCDT z, const char *col, const char *opt);
void MGL_EXPORT mgl_torus_(uintptr_t *gr, uintptr_t *r, uintptr_t *z, const char *pen, const char *opt,int,int);

/// Draw grid lines for density plot at slice for 3d data specified parametrically
void MGL_EXPORT mgl_grid3_xyz(HMGL gr, HCDT x, HCDT y, HCDT z, HCDT a, const char *stl, double sVal, const char *opt);
void MGL_EXPORT mgl_grid3_xyz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a, const char *sch, mreal *sVal, const char *opt,int,int);
/// Draw grid lines for density plot at slice for 3d data
void MGL_EXPORT mgl_grid3(HMGL gr, HCDT a, const char *stl, double sVal, const char *opt);
void MGL_EXPORT mgl_grid3_(uintptr_t *gr, uintptr_t *a, const char *sch, mreal *sVal, const char *opt,int,int);

/// Draw density plot at slice for 3d data specified parametrically
void MGL_EXPORT mgl_dens3_xyz(HMGL gr, HCDT x, HCDT y, HCDT z, HCDT a, const char *stl, double sVal, const char *opt);
void MGL_EXPORT mgl_dens3_xyz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a, const char *sch, mreal *sVal, const char *opt,int,int);
/// Draw density plot at slice for 3d data
void MGL_EXPORT mgl_dens3(HMGL gr, HCDT a, const char *stl, double sVal, const char *opt);
void MGL_EXPORT mgl_dens3_(uintptr_t *gr, uintptr_t *a, const char *sch, mreal *sVal, const char *opt,int,int);

/// Draw manual contour lines at slice for 3d data specified parametrically
void MGL_EXPORT mgl_cont3_xyz_val(HMGL gr, HCDT v, HCDT x, HCDT y, HCDT z, HCDT a, const char *sch, double sVal, const char *opt);
void MGL_EXPORT mgl_cont3_xyz_val_(uintptr_t *gr, uintptr_t *v, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a, const char *sch, mreal *sVal, const char *opt,int,int);
/// Draw manual contour lines at slice for 3d data
void MGL_EXPORT mgl_cont3_val(HMGL gr, HCDT v, HCDT a, const char *sch, double sVal, const char *opt);
void MGL_EXPORT mgl_cont3_val_(uintptr_t *gr, uintptr_t *v, uintptr_t *a, const char *sch, mreal *sVal, const char *opt,int,int);
/// Draw contour lines at slice for 3d data specified parametrically
void MGL_EXPORT mgl_cont3_xyz(HMGL gr, HCDT x, HCDT y, HCDT z, HCDT a, const char *sch, double sVal, const char *opt);
void MGL_EXPORT mgl_cont3_xyz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a, const char *sch, mreal *sVal, const char *opt,int,int);
/// Draw contour lines at slice for 3d data
void MGL_EXPORT mgl_cont3(HMGL gr, HCDT a, const char *sch, double sVal, const char *opt);
void MGL_EXPORT mgl_cont3_(uintptr_t *gr, uintptr_t *a, const char *sch, mreal *sVal, const char *opt,int,int);

/// Draw manual solid contours at slice for 3d data specified parametrically
void MGL_EXPORT mgl_contf3_xyz_val(HMGL gr, HCDT v, HCDT x, HCDT y, HCDT z, HCDT a, const char *sch, double sVal, const char *opt);
void MGL_EXPORT mgl_contf3_xyz_val_(uintptr_t *gr, uintptr_t *v, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a, const char *sch, mreal *sVal, const char *opt,int,int);
/// Draw manual solid contours at slice for 3d data
void MGL_EXPORT mgl_contf3_val(HMGL gr, HCDT v, HCDT a, const char *sch, double sVal, const char *opt);
void MGL_EXPORT mgl_contf3_val_(uintptr_t *gr, uintptr_t *v, uintptr_t *a, const char *sch, mreal *sVal, const char *opt,int,int);
/// Draw solid contours at slice for 3d data specified parametrically
void MGL_EXPORT mgl_contf3_xyz(HMGL gr, HCDT x, HCDT y, HCDT z, HCDT a, const char *sch, double sVal, const char *opt);
void MGL_EXPORT mgl_contf3_xyz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a, const char *sch, mreal *sVal, const char *opt,int,int);
/// Draw solid contours at slice for 3d data
void MGL_EXPORT mgl_contf3(HMGL gr, HCDT a, const char *sch, double sVal, const char *opt);
void MGL_EXPORT mgl_contf3_(uintptr_t *gr, uintptr_t *a, const char *sch, mreal *sVal, const char *opt,int,int);

//-----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif
//-----------------------------------------------------------------------------
#endif
