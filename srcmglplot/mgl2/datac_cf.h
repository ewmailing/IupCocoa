/***************************************************************************
 * data_cf.h is part of Math Graphic Library
 * Copyright (C) 2007-2016 Alexey Balakin <mathgl.abalakin@gmail.ru>       *
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
#ifndef _MGL_DATAC_CF_H_
#define _MGL_DATAC_CF_H_
//-----------------------------------------------------------------------------
#include "mgl2/abstract.h"
//-----------------------------------------------------------------------------
#if MGL_HAVE_GSL
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#else
struct gsl_vector;
struct gsl_matrix;
#endif
//-----------------------------------------------------------------------------
#ifdef __cplusplus
class mglDataC;
typedef mglDataC* HADT;
extern "C" {
#else
typedef void *HADT;
#endif
/// Get integer power of x
mdual MGL_EXPORT_CONST mgl_ipowc(dual x,int n);
mdual MGL_EXPORT mgl_ipowc_(dual *x,int *n);
/// Get exp(i*a)
mdual MGL_EXPORT_CONST mgl_expi(dual a);

/// Create HMDT object
HADT MGL_EXPORT mgl_create_datac();
uintptr_t MGL_EXPORT mgl_create_datac_();
/// Create HMDT object with specified sizes
HADT MGL_EXPORT mgl_create_datac_size(long nx, long ny, long nz);
uintptr_t MGL_EXPORT mgl_create_datac_size_(int *nx, int *ny, int *nz);
/// Create HMDT object with data from file
HADT MGL_EXPORT mgl_create_datac_file(const char *fname);
uintptr_t MGL_EXPORT mgl_create_datac_file_(const char *fname, int len);
/// Delete HMDT object
void MGL_EXPORT mgl_delete_datac(HADT dat);
void MGL_EXPORT mgl_delete_datac_(uintptr_t *dat);

/// Rearange data dimensions
void MGL_EXPORT mgl_datac_rearrange(HADT dat, long mx,long my,long mz);
void MGL_EXPORT mgl_datac_rearrange_(uintptr_t *dat, int *mx, int *my, int *mz);
/// Link external data array (don't delete it at exit)
void MGL_EXPORT mgl_datac_link(HADT dat, dual *A,long mx,long my,long mz);
void MGL_EXPORT mgl_datac_link_(uintptr_t *d, dual *A, int *nx,int *ny,int *nz);
/// Allocate memory and copy the data from the (float *) array
void MGL_EXPORT mgl_datac_set_float(HADT dat, const float *A,long mx,long my,long mz);
void MGL_EXPORT mgl_datac_set_float_(uintptr_t *dat, const float *A,int *NX,int *NY,int *NZ);
/// Allocate memory and copy the data from the (double *) array
void MGL_EXPORT mgl_datac_set_double(HADT dat, const double *A,long mx,long my,long mz);
void MGL_EXPORT mgl_datac_set_double_(uintptr_t *dat, const double *A,int *NX,int *NY,int *NZ);
/// Allocate memory and copy the data from the (dual *) array
void MGL_EXPORT mgl_datac_set_complex(HADT dat, const dual *A,long mx,long my,long mz);
void MGL_EXPORT mgl_datac_set_complex_(uintptr_t *d, const dual *A,int *NX,int *NY,int *NZ);
/// Import data from abstract type
void MGL_EXPORT mgl_datac_set(HADT dat, HCDT a);
void MGL_EXPORT mgl_datac_set_(uintptr_t *dat, uintptr_t *a);
/// Allocate memory and copy the data from the gsl_vector
void MGL_EXPORT mgl_datac_set_vector(HADT dat, gsl_vector *v);
/// Allocate memory and copy the data from the gsl_matrix
void MGL_EXPORT mgl_datac_set_matrix(HADT dat, gsl_matrix *m);
/// Set value of data element [i,j,k]
void MGL_EXPORT mgl_datac_set_value(HADT dat, dual v, long i, long j, long k);
void MGL_EXPORT mgl_datac_set_value_(uintptr_t *d, dual *v, int *i, int *j, int *k);
/// Get value of data element [i,j,k]
mdual MGL_EXPORT mgl_datac_get_value(HCDT dat, long i, long j, long k);
mdual MGL_EXPORT mgl_datac_get_value_(uintptr_t *d, int *i, int *j, int *k);
/// Allocate memory and scanf the data from the string
void MGL_EXPORT mgl_datac_set_values(HADT dat, const char *val, long nx, long ny, long nz);
void MGL_EXPORT mgl_datac_set_values_(uintptr_t *d, const char *val, int *nx, int *ny, int *nz, int l);

/// Get array as solution of tridiagonal matrix solution a[i]*x[i-1]+b[i]*x[i]+c[i]*x[i+1]=d[i]
/** String \a how may contain:
 * 'x', 'y', 'z' for solving along x-,y-,z-directions, or
 * 'h' for solving along hexagonal direction at x-y plain (need nx=ny),
 * 'c' for using periodical boundary conditions,
 * 'd' for diffraction/diffuse calculation.
 * NOTE: It work for flat data model only (i.e. for a[i,j]==a[i+nx*j]) */
HADT MGL_EXPORT mgl_datac_tridmat(HCDT A, HCDT B, HCDT C, HCDT D, const char *how);
uintptr_t MGL_EXPORT mgl_datac_tridmat_(uintptr_t *A, uintptr_t *B, uintptr_t *C, uintptr_t *D, const char *how, int);

/// Returns pointer to internal data array
MGL_EXPORT dual *mgl_datac_data(HADT dat);
/// Returns pointer to data element [i,j,k]
MGL_EXPORT dual *mgl_datac_value(HADT dat, long i,long j,long k);

/// Set the data from HCDT objects for real and imaginary parts
void MGL_EXPORT mgl_datac_set_ri(HADT dat, HCDT re, HCDT im);
void MGL_EXPORT mgl_datac_set_ri_(uintptr_t *dat, uintptr_t *re, uintptr_t *im);
/// Set the data from HCDT objects as amplitude and phase of complex data
void MGL_EXPORT mgl_datac_set_ap(HADT dat, HCDT abs, HCDT phi);
void MGL_EXPORT mgl_datac_set_ap_(uintptr_t *dat, uintptr_t *abs, uintptr_t *phi);

/// Read data from tab-separated text file with auto determining size
int MGL_EXPORT mgl_datac_read(HADT dat, const char *fname);
int MGL_EXPORT mgl_datac_read_(uintptr_t *d, const char *fname,int l);
/// Read data from text file with size specified at beginning of the file
int MGL_EXPORT mgl_datac_read_mat(HADT dat, const char *fname, long dim);
int MGL_EXPORT mgl_datac_read_mat_(uintptr_t *dat, const char *fname, int *dim, int);
/// Read data from text file with specifeid size
int MGL_EXPORT mgl_datac_read_dim(HADT dat, const char *fname,long mx,long my,long mz);
int MGL_EXPORT mgl_datac_read_dim_(uintptr_t *dat, const char *fname,int *mx,int *my,int *mz,int);
/// Read data from tab-separated text files with auto determining size which filenames are result of sprintf(fname,templ,t) where t=from:step:to
int MGL_EXPORT mgl_datac_read_range(HADT d, const char *templ, double from, double to, double step, int as_slice);
int MGL_EXPORT mgl_datac_read_range_(uintptr_t *d, const char *fname, mreal *from, mreal *to, mreal *step, int *as_slice,int l);
/// Read data from tab-separated text files with auto determining size which filenames are satisfied to template (like "t_*.dat")
int MGL_EXPORT mgl_datac_read_all(HADT dat, const char *templ, int as_slice);
int MGL_EXPORT mgl_datac_read_all_(uintptr_t *d, const char *fname, int *as_slice,int l);
/// Save whole data array (for ns=-1) or only ns-th slice to text file
void MGL_EXPORT mgl_datac_save(HCDT dat, const char *fname,long ns);
void MGL_EXPORT mgl_datac_save_(uintptr_t *dat, const char *fname,int *ns,int);

/// Read data array from HDF file (parse HDF4 and HDF5 files)
int MGL_EXPORT mgl_datac_read_hdf(HADT d,const char *fname,const char *data);
int MGL_EXPORT mgl_datac_read_hdf_(uintptr_t *d, const char *fname, const char *data,int l,int n);
/// Save data to HDF file
void MGL_EXPORT mgl_datac_save_hdf(HCDT d,const char *fname,const char *data,int rewrite);
void MGL_EXPORT mgl_datac_save_hdf_(uintptr_t *d, const char *fname, const char *data, int *rewrite,int l,int n);

/// Create or recreate the array with specified size and fill it by zero
void MGL_EXPORT mgl_datac_create(HADT dat, long nx,long ny,long nz);
void MGL_EXPORT mgl_datac_create_(uintptr_t *dat, int *nx,int *ny,int *nz);
/// Transpose dimensions of the data (generalization of Transpose)
void MGL_EXPORT mgl_datac_transpose(HADT dat, const char *dim);
void MGL_EXPORT mgl_datac_transpose_(uintptr_t *dat, const char *dim,int);

/// Get sub-array of the data with given fixed indexes
HADT MGL_EXPORT mgl_datac_subdata(HCDT dat, long xx,long yy,long zz);
uintptr_t MGL_EXPORT mgl_datac_subdata_(uintptr_t *dat, int *xx,int *yy,int *zz);
/// Get sub-array of the data with given fixed indexes (like indirect access)
HADT MGL_EXPORT mgl_datac_subdata_ext(HCDT dat, HCDT xx, HCDT yy, HCDT zz);
uintptr_t MGL_EXPORT mgl_datac_subdata_ext_(uintptr_t *dat, uintptr_t *xx,uintptr_t *yy,uintptr_t *zz);
/// Get column (or slice) of the data filled by formulas of named columns
HADT MGL_EXPORT mgl_datac_column(HCDT dat, const char *eq);
uintptr_t MGL_EXPORT mgl_datac_column_(uintptr_t *dat, const char *eq,int l);
/// Get trace of the data array
HADT MGL_EXPORT mgl_datac_trace(HCDT d);
uintptr_t MGL_EXPORT mgl_datac_trace_(uintptr_t *d);
/// Resize the data to new sizes
HADT MGL_EXPORT mgl_datac_resize(HCDT dat, long mx,long my,long mz);
uintptr_t MGL_EXPORT mgl_datac_resize_(uintptr_t *dat, int *mx,int *my,int *mz);
/// Resize the data to new sizes of box [x1,x2]*[y1,y2]*[z1,z2]
HADT MGL_EXPORT mgl_datac_resize_box(HCDT dat, long mx,long my,long mz,mreal x1,mreal x2,mreal y1,mreal y2,mreal z1,mreal z2);
uintptr_t MGL_EXPORT mgl_datac_resize_box_(uintptr_t *dat, int *mx,int *my,int *mz,mreal *x1,mreal *x2,mreal *y1,mreal *y2,mreal *z1,mreal *z2);
/// Get momentum (1D-array) of data along direction 'dir'. String looks like "x1" for median in x-direction, "x2" for width in x-dir and so on.
HADT MGL_EXPORT mgl_datac_momentum(HCDT dat, char dir, const char *how);
uintptr_t MGL_EXPORT mgl_datac_momentum_(uintptr_t *dat, char *dir, const char *how, int,int);
/// Get array which values is result of interpolation this for coordinates from other arrays
HADT MGL_EXPORT mgl_datac_evaluate(HCDT dat, HCDT idat, HCDT jdat, HCDT kdat, int norm);
uintptr_t MGL_EXPORT mgl_datac_evaluate_(uintptr_t *dat, uintptr_t *idat, uintptr_t *jdat, uintptr_t *kdat, int *norm);
/// Get array which is result of summation in given direction or directions
HADT MGL_EXPORT mgl_datac_sum(HCDT dat, const char *dir);
uintptr_t MGL_EXPORT mgl_datac_sum_(uintptr_t *dat, const char *dir,int);
/// Get the data which is direct multiplication (like, d[i,j] = this[i]*a[j] and so on)
HADT MGL_EXPORT mgl_datac_combine(HCDT dat1, HCDT dat2);
uintptr_t MGL_EXPORT mgl_datac_combine_(uintptr_t *dat1, uintptr_t *dat2);

/// Set names for columns (slices)
void MGL_EXPORT mgl_datac_set_id(HADT d, const char *id);
void MGL_EXPORT mgl_datac_set_id_(uintptr_t *dat, const char *id,int l);
/// Equidistantly fill the data to range [x1,x2] in direction dir
void MGL_EXPORT mgl_datac_fill(HADT dat, dual x1,dual x2,char dir);
void MGL_EXPORT mgl_datac_fill_(uintptr_t *dat, dual *x1,dual *x2,const char *dir,int);
/// Modify the data by specified formula assuming x,y,z in range [r1,r2]
void MGL_EXPORT mgl_datac_fill_eq(HMGL gr, HADT dat, const char *eq, HCDT vdat, HCDT wdat,const char *opt);
void MGL_EXPORT mgl_datac_fill_eq_(uintptr_t *gr, uintptr_t *dat, const char *eq, uintptr_t *vdat, uintptr_t *wdat,const char *opt, int, int);
/// Fill dat by interpolated values of vdat parametrically depended on xdat for x in range [x1,x2] using global spline
void MGL_EXPORT mgl_datac_refill_gs(HADT dat, HCDT xdat, HCDT vdat, mreal x1, mreal x2, long sl);
void MGL_EXPORT mgl_datac_refill_gs_(uintptr_t *dat, uintptr_t *xdat, uintptr_t *vdat, mreal *x1, mreal *x2, long *sl);
/// Fill dat by interpolated values of vdat parametrically depended on xdat for x in range [x1,x2]
void MGL_EXPORT mgl_datac_refill_x(HADT dat, HCDT xdat, HCDT vdat, mreal x1, mreal x2, long sl);
void MGL_EXPORT mgl_datac_refill_x_(uintptr_t *dat, uintptr_t *xdat, uintptr_t *vdat, mreal *x1, mreal *x2, long *sl);
/// Fill dat by interpolated values of vdat parametrically depended on xdat,ydat for x,y in range [x1,x2]*[y1,y2]
void MGL_EXPORT mgl_datac_refill_xy(HADT dat, HCDT xdat, HCDT ydat, HCDT vdat, mreal x1, mreal x2, mreal y1, mreal y2, long sl);
void MGL_EXPORT mgl_datac_refill_xy_(uintptr_t *dat, uintptr_t *xdat, uintptr_t *ydat, uintptr_t *vdat, mreal *x1, mreal *x2, mreal *y1, mreal *y2, long *sl);
/// Fill dat by interpolated values of vdat parametrically depended on xdat,ydat,zdat for x,y,z in range [x1,x2]*[y1,y2]*[z1,z2]
void MGL_EXPORT mgl_datac_refill_xyz(HADT dat, HCDT xdat, HCDT ydat, HCDT zdat, HCDT vdat, mreal x1, mreal x2, mreal y1, mreal y2, mreal z1, mreal z2);
void MGL_EXPORT mgl_datac_refill_xyz_(uintptr_t *dat, uintptr_t *xdat, uintptr_t *ydat, uintptr_t *zdat, uintptr_t *vdat, mreal *x1, mreal *x2, mreal *y1, mreal *y2, mreal *z1, mreal *z2);
/// Fill dat by interpolated values of vdat parametrically depended on xdat,ydat,zdat for x,y,z in axis range
void MGL_EXPORT mgl_datac_refill_gr(HMGL gr, HADT dat, HCDT xdat, HCDT ydat, HCDT zdat, HCDT vdat, long sl, const char *opt);
void MGL_EXPORT mgl_datac_refill_gr_(uintptr_t *gr, uintptr_t *dat, uintptr_t *xdat, uintptr_t *ydat, uintptr_t *zdat, uintptr_t *vdat, long *sl, const char *opt,int);

/// Modify the data by specified formula
void MGL_EXPORT mgl_datac_modify(HADT dat, const char *eq,long dim);
void MGL_EXPORT mgl_datac_modify_(uintptr_t *dat, const char *eq,int *dim,int);
/// Modify the data by specified formula
void MGL_EXPORT mgl_datac_modify_vw(HADT dat, const char *eq,HCDT vdat,HCDT wdat);
void MGL_EXPORT mgl_datac_modify_vw_(uintptr_t *dat, const char *eq, uintptr_t *vdat, uintptr_t *wdat,int);

/// Limit the data to be inside [-v,v], keeping the original sign
void MGL_EXPORT mgl_datac_limit(HADT dat, mreal v);
void MGL_EXPORT mgl_datac_limit_(uintptr_t *dat, mreal *v);

/// Put value to data element(s)
void MGL_EXPORT mgl_datac_put_val(HADT dat, dual val, long i, long j, long k);
void MGL_EXPORT mgl_datac_put_val_(uintptr_t *dat, dual *val, int *i, int *j, int *k);
/// Put array to data element(s)
void MGL_EXPORT mgl_datac_put_dat(HADT dat, HCDT val, long i, long j, long k);
void MGL_EXPORT mgl_datac_put_dat_(uintptr_t *dat, uintptr_t *val, int *i, int *j, int *k);

/// Reduce size of the data
void MGL_EXPORT mgl_datac_squeeze(HADT dat, long rx,long ry,long rz,long smooth);
void MGL_EXPORT mgl_datac_squeeze_(uintptr_t *dat, int *rx,int *ry,int *rz,int *smooth);
/// Extend data dimensions
void MGL_EXPORT mgl_datac_extend(HADT dat, long n1, long n2);
void MGL_EXPORT mgl_datac_extend_(uintptr_t *dat, int *n1, int *n2);
/// Insert data rows/columns/slices
void MGL_EXPORT mgl_datac_insert(HADT dat, char dir, long at, long num);
void MGL_EXPORT mgl_datac_insert_(uintptr_t *dat, const char *dir, int *at, int *num, int);
/// Delete data rows/columns/slices
void MGL_EXPORT mgl_datac_delete(HADT dat, char dir, long at, long num);
void MGL_EXPORT mgl_datac_delete_(uintptr_t *dat, const char *dir, int *at, int *num, int);
/// Joind another data array
void MGL_EXPORT mgl_datac_join(HADT dat, HCDT d);
void MGL_EXPORT mgl_datac_join_(uintptr_t *dat, uintptr_t *d);

/// Smooth the data on specified direction or directions
/** String \a dir may contain:
 *  ‘x’, ‘y’, ‘z’ for 1st, 2nd or 3d dimension;
 *  ‘dN’ for linear averaging over N points;
 *  ‘3’ for linear averaging over 3 points;
 *  ‘5’ for linear averaging over 5 points.
 *  By default quadratic averaging over 5 points is used. */
void MGL_EXPORT mgl_datac_smooth(HADT d, const char *dirs, mreal delta);
void MGL_EXPORT mgl_datac_smooth_(uintptr_t *dat, const char *dirs, mreal *delta,int);
/// Cumulative summation the data in given direction or directions
void MGL_EXPORT mgl_datac_cumsum(HADT dat, const char *dir);
void MGL_EXPORT mgl_datac_cumsum_(uintptr_t *dat, const char *dir,int);
/// Integrate (cumulative summation) the data in given direction or directions
void MGL_EXPORT mgl_datac_integral(HADT dat, const char *dir);
void MGL_EXPORT mgl_datac_integral_(uintptr_t *dat, const char *dir,int);
/// Differentiate the data in given direction or directions
void MGL_EXPORT mgl_datac_diff(HADT dat, const char *dir);
void MGL_EXPORT mgl_datac_diff_(uintptr_t *dat, const char *dir,int);
/// Double-differentiate (like Laplace operator) the data in given direction
void MGL_EXPORT mgl_datac_diff2(HADT dat, const char *dir);
void MGL_EXPORT mgl_datac_diff2_(uintptr_t *dat, const char *dir,int);
/// Swap left and right part of the data in given direction (useful for Fourier spectrum)
void MGL_EXPORT mgl_datac_swap(HADT dat, const char *dir);
void MGL_EXPORT mgl_datac_swap_(uintptr_t *dat, const char *dir,int);
/// Roll data along direction dir by num slices
void MGL_EXPORT mgl_datac_roll(HADT dat, char dir, long num);
void MGL_EXPORT mgl_datac_roll_(uintptr_t *dat, const char *dir, int *num, int);
/// Mirror the data in given direction (useful for Fourier spectrum)
void MGL_EXPORT mgl_datac_mirror(HADT dat, const char *dir);
void MGL_EXPORT mgl_datac_mirror_(uintptr_t *dat, const char *dir,int);
/// Crop the data
void MGL_EXPORT mgl_datac_crop(HADT dat, long n1, long n2, char dir);
void MGL_EXPORT mgl_datac_crop_(uintptr_t *dat, int *n1, int *n2, const char *dir,int);

/// Multiply the data by other one for each element
void MGL_EXPORT mgl_datac_mul_dat(HADT dat, HCDT d);
void MGL_EXPORT mgl_datac_mul_dat_(uintptr_t *dat, uintptr_t *d);
/// Divide the data by other one for each element
void MGL_EXPORT mgl_datac_div_dat(HADT dat, HCDT d);
void MGL_EXPORT mgl_datac_div_dat_(uintptr_t *dat, uintptr_t *d);
/// Add the other data
void MGL_EXPORT mgl_datac_add_dat(HADT dat, HCDT d);
void MGL_EXPORT mgl_datac_add_dat_(uintptr_t *dat, uintptr_t *d);
/// Subtract the other data
void MGL_EXPORT mgl_datac_sub_dat(HADT dat, HCDT d);
void MGL_EXPORT mgl_datac_sub_dat_(uintptr_t *dat, uintptr_t *d);
/// Multiply each element by the number
void MGL_EXPORT mgl_datac_mul_num(HADT dat, dual d);
void MGL_EXPORT mgl_datac_mul_num_(uintptr_t *dat, dual *d);
/// Divide each element by the number
void MGL_EXPORT mgl_datac_div_num(HADT dat, dual d);
void MGL_EXPORT mgl_datac_div_num_(uintptr_t *dat, dual *d);
/// Add the number
void MGL_EXPORT mgl_datac_add_num(HADT dat, dual d);
void MGL_EXPORT mgl_datac_add_num_(uintptr_t *dat, dual *d);
/// Subtract the number
void MGL_EXPORT mgl_datac_sub_num(HADT dat, dual d);
void MGL_EXPORT mgl_datac_sub_num_(uintptr_t *dat, dual *d);

/// Apply Hankel transform
void MGL_EXPORT mgl_datac_hankel(HADT dat, const char *dir);
void MGL_EXPORT mgl_datac_hankel_(uintptr_t *dat, const char *dir,int);
/// Apply Fourier transform
void MGL_EXPORT mgl_datac_fft(HADT dat, const char *dir);
void MGL_EXPORT mgl_datac_fft_(uintptr_t *dat, const char *dir,int);
/// Find correlation between 2 data arrays
HADT MGL_EXPORT mgl_datac_correl(HCDT dat1, HCDT dat2, const char *dir);
uintptr_t MGL_EXPORT mgl_datac_correl_(uintptr_t *dat1, uintptr_t *dat2, const char *dir,int);
/// Calculate one step of diffraction by finite-difference method with parameter q
void MGL_EXPORT mgl_datac_diffr(HADT dat, const char *how, mreal q);
void MGL_EXPORT mgl_datac_diffr_(uintptr_t *d, const char *how, double q,int l);

/// Get real part of data values
HMDT MGL_EXPORT mgl_datac_real(HCDT dat);
uintptr_t MGL_EXPORT mgl_datac_real_(uintptr_t *dat);
/// Get imaginary part of data values
HMDT MGL_EXPORT mgl_datac_imag(HCDT dat);
uintptr_t MGL_EXPORT mgl_datac_imag_(uintptr_t *dat);
/// Get absolute value of data values, i.e. |u|
HMDT MGL_EXPORT mgl_datac_abs(HCDT dat);
uintptr_t MGL_EXPORT mgl_datac_abs_(uintptr_t *dat);
/// Get argument of data values
HMDT MGL_EXPORT mgl_datac_arg(HCDT dat);
uintptr_t MGL_EXPORT mgl_datac_arg_(uintptr_t *dat);
/// Get square of absolute value of data values, i.e. |u|^2
HMDT MGL_EXPORT mgl_datac_norm(HCDT dat);
uintptr_t MGL_EXPORT mgl_datac_norm_(uintptr_t *dat);

/// Interpolate by linear function the data to given point x=[0...nx-1], y=[0...ny-1], z=[0...nz-1]
mdual MGL_EXPORT mgl_datac_linear(HCDT d, mreal x,mreal y,mreal z);
mdual MGL_EXPORT mgl_datac_linear_(uintptr_t *d, mreal *x,mreal *y,mreal *z);
/// Interpolate by linear function the data and return its derivatives at given point x=[0...nx-1], y=[0...ny-1], z=[0...nz-1]
mdual MGL_EXPORT mgl_datac_linear_ext(HCDT d, mreal x,mreal y,mreal z, dual *dx,dual *dy,dual *dz);
mdual MGL_EXPORT mgl_datac_linear_ext_(uintptr_t *d, mreal *x,mreal *y,mreal *z, dual *dx,dual *dy,dual *dz);
/// Interpolate by cubic spline the data to given point x=[0...nx-1], y=[0...ny-1], z=[0...nz-1]
mdual MGL_EXPORT mgl_datac_spline(HCDT dat, mreal x,mreal y,mreal z);
mdual MGL_EXPORT mgl_datac_spline_(uintptr_t *dat, mreal *x,mreal *y,mreal *z);
/// Interpolate by cubic spline the data and return its derivatives at given point x=[0...nx-1], y=[0...ny-1], z=[0...nz-1]
mdual MGL_EXPORT mgl_datac_spline_ext(HCDT dat, mreal x,mreal y,mreal z, dual *dx,dual *dy,dual *dz);
mdual MGL_EXPORT mgl_datac_spline_ext_(uintptr_t *dat, mreal *x,mreal *y,mreal *z, dual *dx,dual *dy,dual *dz);
/// Prepare coefficients for global spline interpolation
HADT MGL_EXPORT mgl_gsplinec_init(HCDT x, HCDT v);
uintptr_t MGL_EXPORT mgl_gspline_init_(uintptr_t *x, uintptr_t *v);
/// Evaluate global spline (and its derivatives d1, d2 if not NULL) using prepared coefficients \a coef
mdual MGL_EXPORT mgl_gsplinec(HCDT coef, mreal dx, dual *d1, dual *d2);
mdual MGL_EXPORT mgl_gsplinec_(uintptr_t *c, mreal *dx, dual *d1, dual *d2);
//-----------------------------------------------------------------------------
/// Create HAEX object for expression evaluating
HAEX MGL_EXPORT mgl_create_cexpr(const char *expr);
uintptr_t MGL_EXPORT mgl_create_cexpr_(const char *expr, int);
/// Delete HAEX object
void MGL_EXPORT mgl_delete_cexpr(HAEX ex);
void MGL_EXPORT mgl_delete_cexpr_(uintptr_t *ex);
/// Return value of expression for given x,y,z variables
mdual MGL_EXPORT mgl_cexpr_eval(HAEX ex, dual x, dual y,dual z);
mdual MGL_EXPORT mgl_cexpr_eval_(uintptr_t *ex, dual *x, dual *y, dual *z);
/// Return value of expression for given variables
mdual MGL_EXPORT mgl_cexpr_eval_v(HAEX ex, dual *vars);

#ifdef __cplusplus
}
#endif
//-----------------------------------------------------------------------------
#endif
