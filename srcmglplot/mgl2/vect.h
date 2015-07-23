/***************************************************************************
 * vect.h is part of Math Graphic Library
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
#ifndef _MGL_VECT_H_
#define _MGL_VECT_H_
#include "mgl2/abstract.h"
//-----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
//-----------------------------------------------------------------------------
/// Plot vectors at position {x,y} along {ax,ay} with length/color proportional to |a|
void MGL_EXPORT mgl_traj_xy(HMGL gr, HCDT x, HCDT y, HCDT ax, HCDT ay, const char *sch, const char *opt);
void MGL_EXPORT mgl_traj_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *ax, uintptr_t *ay, const char *sch, const char *opt,int,int);
/// Plot vectors at position {x,y,z} along {ax,ay,az} with length/color proportional to |a|
void MGL_EXPORT mgl_traj_xyz(HMGL gr, HCDT x, HCDT y, HCDT z, HCDT ax, HCDT ay, HCDT az, const char *sch, const char *opt);
void MGL_EXPORT mgl_traj_xyz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch, const char *opt,int,int);

/// Plot vector field {ax,ay} parametrically depended on coordinate {x,y} with length/color proportional to |a|
void MGL_EXPORT mgl_vect_xy(HMGL gr, HCDT x, HCDT y, HCDT ax, HCDT ay, const char *sch, const char *opt);
void MGL_EXPORT mgl_vect_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *ax, uintptr_t *ay, const char *sch, const char *opt,int,int);
/// Plot vector field {ax,ay} with length/color proportional to |a|
void MGL_EXPORT mgl_vect_2d(HMGL gr, HCDT ax, HCDT ay, const char *sch, const char *opt);
void MGL_EXPORT mgl_vect_2d_(uintptr_t *gr, uintptr_t *ax, uintptr_t *ay, const char *sch, const char *opt,int,int);
/// Plot vector field {ax,ay,az} parametrically depended on coordinate {x,y,z} with length/color proportional to |a|
void MGL_EXPORT mgl_vect_xyz(HMGL gr, HCDT x, HCDT y, HCDT z, HCDT ax, HCDT ay, HCDT az, const char *sch, const char *opt);
void MGL_EXPORT mgl_vect_xyz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch, const char *opt,int,int);
/// Plot vector field {ax,ay,az} with length/color proportional to |a|
void MGL_EXPORT mgl_vect_3d(HMGL gr, HCDT ax, HCDT ay, HCDT az, const char *sch, const char *opt);
void MGL_EXPORT mgl_vect_3d_(uintptr_t *gr, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch, const char *opt,int,int);

/// Plot flows for vector field {ax,ay} parametrically depended on coordinate {x,y} with color proportional to |a|
void MGL_EXPORT mgl_flow_xy(HMGL gr, HCDT x, HCDT y, HCDT ax, HCDT ay, const char *sch, const char *opt);
void MGL_EXPORT mgl_flow_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *ax, uintptr_t *ay, const char *sch, const char *opt,int,int);
/// Plot flows for vector field {ax,ay} with color proportional to |a|
void MGL_EXPORT mgl_flow_2d(HMGL gr, HCDT ax, HCDT ay, const char *sch, const char *opt);
void MGL_EXPORT mgl_flow_2d_(uintptr_t *gr, uintptr_t *ax, uintptr_t *ay, const char *sch, const char *opt,int,int);
/// Plot flows for vector field {ax,ay,az} parametrically depended on coordinate {x,y,z} with color proportional to |a|
void MGL_EXPORT mgl_flow_xyz(HMGL gr, HCDT x, HCDT y, HCDT z, HCDT ax, HCDT ay, HCDT az, const char *sch, const char *opt);
void MGL_EXPORT mgl_flow_xyz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch, const char *opt,int,int);
/// Plot flows for vector field {ax,ay,az} with color proportional to |a|
void MGL_EXPORT mgl_flow_3d(HMGL gr, HCDT ax, HCDT ay, HCDT az, const char *sch, const char *opt);
void MGL_EXPORT mgl_flow_3d_(uintptr_t *gr, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch, const char *opt,int,int);

/// Plot flow from point p for vector field {ax,ay} parametrically depended on coordinate {x,y} with color proportional to |a|
void MGL_EXPORT mgl_flowp_xy(HMGL gr, double x0, double y0, double z0, HCDT x, HCDT y, HCDT ax, HCDT ay, const char *sch, const char *opt);
void MGL_EXPORT mgl_flowp_xy_(uintptr_t *gr, mreal *x0, mreal *y0, mreal *z0, uintptr_t *x, uintptr_t *y, uintptr_t *ax, uintptr_t *ay, const char *sch, const char *opt,int, int);
/// Plot flow from point p for vector field {ax,ay} with color proportional to |a|
void MGL_EXPORT mgl_flowp_2d(HMGL gr, double x0, double y0, double z0, HCDT ax, HCDT ay, const char *sch, const char *opt);
void MGL_EXPORT mgl_flowp_2d_(uintptr_t *gr, mreal *x0, mreal *y0, mreal *z0, uintptr_t *ax, uintptr_t *ay, const char *sch, const char *opt,int, int);
/// Plot flow from point p for vector field {ax,ay,az} parametrically depended on coordinate {x,y,z} with color proportional to |a|
void MGL_EXPORT mgl_flowp_xyz(HMGL gr, double x0, double y0, double z0, HCDT x, HCDT y, HCDT z, HCDT ax, HCDT ay, HCDT az, const char *sch, const char *opt);
void MGL_EXPORT mgl_flowp_xyz_(uintptr_t *gr, mreal *x0, mreal *y0, mreal *z0, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch, const char *opt,int, int);
/// Plot flow from point p for vector field {ax,ay,az} with color proportional to |a|
void MGL_EXPORT mgl_flowp_3d(HMGL gr, double x0, double y0, double z0, HCDT ax, HCDT ay, HCDT az, const char *sch, const char *opt);
void MGL_EXPORT mgl_flowp_3d_(uintptr_t *gr, mreal *x0, mreal *y0, mreal *z0, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch, const char *opt,int,int);

/// Plot flow pipes for vector field {ax,ay} parametrically depended on coordinate {x,y} with color and radius proportional to |a|
void MGL_EXPORT mgl_pipe_xy(HMGL gr, HCDT x, HCDT y, HCDT ax, HCDT ay, const char *sch, double r0, const char *opt);
void MGL_EXPORT mgl_pipe_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *ax, uintptr_t *ay, const char *sch, mreal *r0, const char *opt,int,int);
/// Plot flow pipes for vector field {ax,ay} with color and radius proportional to |a|
void MGL_EXPORT mgl_pipe_2d(HMGL gr, HCDT ax, HCDT ay, const char *sch, double r0, const char *opt);
void MGL_EXPORT mgl_pipe_2d_(uintptr_t *gr, uintptr_t *ax, uintptr_t *ay, const char *sch, mreal *r0, const char *opt,int,int);
/// Plot flow pipes for vector field {ax,ay,az} parametrically depended on coordinate {x,y,z} with color and radius proportional to |a|
void MGL_EXPORT mgl_pipe_xyz(HMGL gr, HCDT x, HCDT y, HCDT z, HCDT ax, HCDT ay, HCDT az, const char *sch, double r0, const char *opt);
void MGL_EXPORT mgl_pipe_xyz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch, mreal *r0, const char *opt,int,int);
/// Plot flow pipes for vector field {ax,ay,az} with color and radius proportional to |a|
void MGL_EXPORT mgl_pipe_3d(HMGL gr, HCDT ax, HCDT ay, HCDT az, const char *sch, double r0, const char *opt);
void MGL_EXPORT mgl_pipe_3d_(uintptr_t *gr, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch, mreal *r0, const char *opt,int,int);

/// Plot flows for gradient of scalar field phi parametrically depended on coordinate {x,y,z}
void MGL_EXPORT mgl_grad_xyz(HMGL gr, HCDT x, HCDT y, HCDT z, HCDT ph, const char *sch, const char *opt);
void MGL_EXPORT mgl_grad_xyz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *ph, const char *sch, const char *opt,int, int);
/// Plot flows for gradient of scalar field phi parametrically depended on coordinate {x,y}
void MGL_EXPORT mgl_grad_xy(HMGL gr, HCDT x, HCDT y, HCDT ph, const char *sch, const char *opt);
void MGL_EXPORT mgl_grad_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *ph, const char *sch, const char *opt,int,int);
/// Plot flows for gradient of scalar field phi
void MGL_EXPORT mgl_grad(HMGL gr, HCDT ph, const char *sch, const char *opt);
void MGL_EXPORT mgl_grad_(uintptr_t *gr, uintptr_t *ph, const char *sch, const char *opt,int,int);

/// Draw vector plot at slice for 3d data specified parametrically
void MGL_EXPORT mgl_vect3_xyz(HMGL gr, HCDT x, HCDT y, HCDT z, HCDT ax, HCDT ay, HCDT az, const char *sch, double sVal, const char *opt);
void MGL_EXPORT mgl_vect3_xyz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch, mreal *sVal, const char *opt,int,int);
/// Draw vector plot at slice for 3d data
void MGL_EXPORT mgl_vect3(HMGL gr, HCDT ax, HCDT ay, HCDT az, const char *sch, double sVal, const char *opt);
void MGL_EXPORT mgl_vect3_(uintptr_t *gr, uintptr_t *ax, uintptr_t *ay, uintptr_t *az, const char *sch, mreal *sVal, const char *opt,int,int);
//-----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif
//-----------------------------------------------------------------------------
#endif
