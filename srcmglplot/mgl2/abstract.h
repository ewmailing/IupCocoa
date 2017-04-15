/***************************************************************************
 * abstract.h is part of Math Graphic Library
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
#ifndef _MGL_ABSTRACT_H_
#define _MGL_ABSTRACT_H_

#include "mgl2/define.h"
//-----------------------------------------------------------------------------
#ifdef __cplusplus
#include "mgl2/type.h"
#define MGL_TO_WCS(str,code)	if(str && *str){size_t s=mbstowcs(0,str,0); wchar_t *wcs=new wchar_t[s+1]; mbstowcs(wcs,str,s); wcs[s]=0; code; delete []wcs;}else{const wchar_t *wcs=L""; code;}
//-----------------------------------------------------------------------------
class mglBase;
class mglData;
class mglDataA;
class mglDataC;
class mglParser;
class mglFormula;
class mglFormulaC;
class mglFont;
typedef mglBase* HMGL;
typedef mglData* HMDT;
typedef mglDataC* HADT;
typedef mglParser* HMPR;
typedef mglFormula* HMEX;
typedef mglFormulaC* HAEX;
typedef const mglDataA* HCDT;

std::string MGL_EXPORT mgl_data_to_string(HCDT d, long ns);
std::string MGL_EXPORT mgl_datac_to_string(HCDT d, long ns);
extern "C" {

#else
#define mglDataA void
typedef void *HMGL;
typedef void *HMDT;
typedef void *HADT;
typedef void *HMEX;
typedef void *HAEX;
typedef void *HMPR;
typedef const void *HCDT;
#endif

/// Set seed for random numbers
void MGL_EXPORT mgl_srnd(long seed);
void MGL_EXPORT mgl_srnd_(int *seed);
/// Get random number
double MGL_EXPORT mgl_rnd();
double MGL_EXPORT mgl_rnd_();

/// Set name for data variable (can be used in mgl_formula_calc() or in MGL scripts)
void MGL_EXPORT mgl_data_set_name(mglDataA *dat, const char *name);
void MGL_EXPORT mgl_data_set_name_(uintptr_t *dat, const char *name,int);
void MGL_EXPORT mgl_data_set_name_w(mglDataA *dat, const wchar_t *name);
/// Set callback function which is called at deleting variable
void MGL_EXPORT mgl_data_set_func(mglDataA *dat, void (*func)(void *), void *par);

/// Save whole data array (for ns=-1) or only ns-th slice to text file
void MGL_EXPORT mgl_data_save(HCDT dat, const char *fname,long ns);
void MGL_EXPORT mgl_data_save_(uintptr_t *dat, const char *fname,int *ns,int);
/// Export data array (for ns=-1) or only ns-th slice to PNG file according color scheme
void MGL_EXPORT mgl_data_export(HCDT dat, const char *fname, const char *scheme,mreal v1,mreal v2,long ns);
void MGL_EXPORT mgl_data_export_(uintptr_t *dat, const char *fname, const char *scheme,mreal *v1,mreal *v2,int *ns,int,int);
/// Save data to HDF file
void MGL_EXPORT mgl_data_save_hdf(HCDT d,const char *fname,const char *data,int rewrite);
void MGL_EXPORT mgl_data_save_hdf_(uintptr_t *d, const char *fname, const char *data, int *rewrite,int l,int n);
/// Get information about the data (sizes and momentum) to string
MGL_EXPORT const char *mgl_data_info(HCDT dat);
int MGL_EXPORT mgl_data_info_(uintptr_t *dat, char *out, int len);
/// Put HDF data names into buf as '\t' separated.
int MGL_EXPORT mgl_datas_hdf(const char *fname, char *buf, long size);
int MGL_EXPORT mgl_datas_hdf_(const char *fname, char *buf, int l, int size);

/// Get maximal value of the data
mreal MGL_EXPORT mgl_data_max(HCDT dat);
mreal MGL_EXPORT mgl_data_max_(uintptr_t *dat);
/// Get maximal value of the data which is less than 0
mreal MGL_EXPORT mgl_data_neg_max(HCDT dat);
mreal MGL_EXPORT mgl_data_neg_max_(uintptr_t *dat);
/// Get minimal value of the data
mreal MGL_EXPORT mgl_data_min(HCDT dat);
mreal MGL_EXPORT mgl_data_min_(uintptr_t *dat);
/// Get minimal value of the data which is larger than 0
mreal MGL_EXPORT mgl_data_pos_min(HCDT dat);
mreal MGL_EXPORT mgl_data_pos_min_(uintptr_t *dat);
/// Find position (after specified in i,j,k) of first nonzero value of formula
mreal MGL_EXPORT mgl_data_first(HCDT dat, const char *cond, long *i, long *j, long *k);
mreal MGL_EXPORT mgl_data_first_(uintptr_t *dat, const char *cond, int *i, int *j, int *k, int);
/// Find position (before specified in i,j,k) of last nonzero value of formula
mreal MGL_EXPORT mgl_data_last(HCDT dat, const char *cond, long *i, long *j, long *k);
mreal MGL_EXPORT mgl_data_last_(uintptr_t *dat, const char *cond, int *i, int *j, int *k, int);
/// Find position of first in direction 'dir' nonzero value of formula
long MGL_EXPORT mgl_data_find(HCDT dat, const char *cond, char dir, long i, long j, long k);
int MGL_EXPORT mgl_data_find_(uintptr_t *dat, const char *cond, char *dir, int *i, int *j, int *k, int,int);
/// Find if any nonzero value of formula
int MGL_EXPORT mgl_data_find_any(HCDT dat, const char *cond);
int MGL_EXPORT mgl_data_find_any_(uintptr_t *dat, const char *cond, int);
/// Get maximal value of the data and its position
mreal MGL_EXPORT mgl_data_max_int(HCDT dat, long *i, long *j, long *k);
mreal MGL_EXPORT mgl_data_max_int_(uintptr_t *dat, int *i, int *j, int *k);
/// Get maximal value of the data and its approximated position
mreal MGL_EXPORT mgl_data_max_real(HCDT dat, mreal *x, mreal *y, mreal *z);
mreal MGL_EXPORT mgl_data_max_real_(uintptr_t *dat, mreal *x, mreal *y, mreal *z);
/// Get minimal value of the data and its position
mreal MGL_EXPORT mgl_data_min_int(HCDT dat, long *i, long *j, long *k);
mreal MGL_EXPORT mgl_data_min_int_(uintptr_t *dat, int *i, int *j, int *k);
/// Get minimal value of the data and its approximated position
mreal MGL_EXPORT mgl_data_min_real(HCDT dat, mreal *x, mreal *y, mreal *z);
mreal MGL_EXPORT mgl_data_min_real_(uintptr_t *dat, mreal *x, mreal *y, mreal *z);
/// Get "energy and find 4 momenta of data: median, width, skewness, kurtosis
mreal MGL_EXPORT mgl_data_momentum_val(HCDT d, char dir, mreal *m, mreal *w, mreal *s, mreal *k);
mreal MGL_EXPORT mgl_data_momentum_val_(uintptr_t *dat, char *dir, mreal *m, mreal *w, mreal *s, mreal *k,int);

/// Interpolate by linear function the data to given point x=[0...nx-1], y=[0...ny-1], z=[0...nz-1]
mreal MGL_EXPORT mgl_data_linear(HCDT dat, mreal x,mreal y,mreal z);
mreal MGL_EXPORT mgl_data_linear_(uintptr_t *dat, mreal *x,mreal *y,mreal *z);
/// Interpolate by linear function the data and return its derivatives at given point x=[0...nx-1], y=[0...ny-1], z=[0...nz-1]
mreal MGL_EXPORT mgl_data_linear_ext(HCDT dat, mreal x,mreal y,mreal z, mreal *dx,mreal *dy,mreal *dz);
mreal MGL_EXPORT mgl_data_linear_ext_(uintptr_t *dat, mreal *x,mreal *y,mreal *z, mreal *dx,mreal *dy,mreal *dz);

/// Internal function for (un-)locking mutex in mglStack
void MGL_EXPORT mgl_mutex_lock(void *);
void MGL_EXPORT mgl_mutex_unlock(void *);

//-----------------------------------------------------------------------------
/// Callback function for asking user a question. Result shouldn't exceed 1024.
extern MGL_EXPORT void (*mgl_ask_func)(const wchar_t *quest, wchar_t *res);
//-----------------------------------------------------------------------------
#ifdef __cplusplus
}
/// Abstract class for data array
class MGL_EXPORT mglDataA
{
public:
	std::wstring s;	///< Data name
	bool temp;		///< This is temporary variable
	void (*func)(void *);	///< Callback function for destroying
	void *o; 		///< Pointer to external object

	mglDataA()	{	temp=false;	func=0;	o=0;	}
	virtual ~mglDataA()	{	if(func)	func(o);	}
	virtual void set_v(mreal /*val*/, long /*i*/,long /*j*/=0,long /*k*/=0)	{}
	/// Get the interpolated value and its derivatives in given data cell without border checking
	virtual mreal valueD(mreal x,mreal y=0,mreal z=0,mreal *dx=0,mreal *dy=0,mreal *dz=0) const =0;
	/// Get the interpolated value in given data cell without border checking
	virtual mreal value(mreal x,mreal y=0,mreal z=0) const =0;
	/// Interpolate by linear function the data to given point
	inline mreal linear(mreal x,mreal y=0,mreal z=0)	const
	{	return mgl_data_linear_ext(this,x,y,z,0,0,0);	}
	/// Interpolate by linear function the data to given point and get the gradient
	inline mreal linearD(mreal x,mreal y=0,mreal z=0,mreal *dx=0,mreal *dy=0,mreal *dz=0)	const
	{	return mgl_data_linear_ext(this,x,y,z,dx,dy,dz);	}
	virtual mreal v(long i,long j=0,long k=0) const = 0;
	virtual mreal vthr(long i) const
	{	return v(i%GetNx(), (i/GetNx())%GetNy(), i/(GetNx()*GetNy()));	}
	virtual dual vc(long i,long j=0,long k=0) const	{	return v(i,j,k);	}
	virtual dual vcthr(long i) const	{	return vthr(i);	}
	virtual long GetNx() const = 0;
	virtual long GetNy() const = 0;
	virtual long GetNz() const = 0;
	inline long GetNN() const {	return GetNx()*GetNy()*GetNz();	}
	virtual mreal dvx(long i,long j=0,long k=0) const = 0;
//	{	return i>0 ? (i<GetNx()-1 ? (v(i+1,j,k)-v(i-1,j,k))/2 : v(i,j,k)-v(i-1,j,k)) : v(1,j,k)-v(0,j,k);	}
	virtual mreal dvy(long i,long j=0,long k=0) const = 0;
//	{	return j>0 ? (j<GetNy()-1 ? (v(i,j+1,k)-v(i,j-1,k))/2 : v(i,j,k)-v(i,j-1,k)) : v(i,1,k)-v(i,0,k);	}
	virtual mreal dvz(long i,long j=0,long k=0) const = 0;
//	{	return k>0 ? (k<GetNz()-1 ? (v(i,j,k+1)-v(i,j,k-1))/2 : v(i,j,k)-v(i,j,k-1)) : v(i,j,1)-v(i,j,0);	}

	// Now some common function which applicable for most of data types
	/// Save whole data array (for ns=-1) or only ns-th slice to text file
	virtual void Save(const char *fname,long ns=-1) const
	{	mgl_data_save(this,fname,ns);	}
	/// Get whole data array (for ns=-1) or only ns-th slice to string
	virtual std::string Get(long ns=-1) const
	{	return mgl_data_to_string(this,ns);	}
	/// Export data array (for ns=-1) or only ns-th slice to PNG file according color scheme
	inline void Export(const char *fname,const char *scheme,mreal v1=0,mreal v2=0,long ns=-1) const
	{	mgl_data_export(this,fname,scheme,v1,v2,ns);	}
	/// Save data to HDF file
	virtual void SaveHDF(const char *fname,const char *data,bool rewrite=false) const
	{	mgl_data_save_hdf(this,fname,data,rewrite);	}
	/// Put HDF data names into buf as '\t' separated.
	inline static int DatasHDF(const char *fname, char *buf, long size)
	{	return mgl_datas_hdf(fname,buf,size);	}

	/// Get information about the data (sizes and momentum) to string
	inline const char *PrintInfo() const	{	return mgl_data_info(this);	}
	/// Print information about the data (sizes and momentum) to FILE (for example, stdout)
	inline void PrintInfo(FILE *fp) const
	{	if(fp)	{	fprintf(fp,"%s",mgl_data_info(this));	fflush(fp);	}	}
	/// Get maximal value of the data
	virtual mreal Maximal() const	{	return mgl_data_max(this);	}
	/// Get minimal value of the data
	virtual mreal Minimal() const	{	return mgl_data_min(this);	}
	/// Get maximal value of the data which is less than 0
	inline mreal MaximalNeg() const	{	return mgl_data_neg_max(this);	}
	/// Get minimal value of the data which is larger than 0
	inline mreal MinimalPos() const	{	return mgl_data_pos_min(this);	}
	/// Get maximal value of the data and its position
	inline mreal Maximal(long &i,long &j,long &k) const
	{	return mgl_data_max_int(this,&i,&j,&k);	}
	/// Get minimal value of the data and its position
	inline mreal Minimal(long &i,long &j,long &k) const
	{	return mgl_data_min_int(this,&i,&j,&k);	}
	/// Get maximal value of the data and its approximated position
	inline mreal Maximal(mreal &x,mreal &y,mreal &z) const
	{	return mgl_data_max_real(this,&x,&y,&z);	}
	/// Get minimal value of the data and its approximated position
	inline mreal Minimal(mreal &x,mreal &y,mreal &z) const
	{	return mgl_data_min_real(this,&x,&y,&z);	}
	/// Get "energy" and find first (median) and second (width) momenta of data
	inline mreal Momentum(char dir,mreal &m,mreal &w) const
	{	return mgl_data_momentum_val(this,dir,&m,&w,0,0);	}
	/// Get "energy and find 4 momenta of data: median, width, skewness, kurtosis
	inline mreal Momentum(char dir,mreal &m,mreal &w,mreal &s,mreal &k) const
	{	return mgl_data_momentum_val(this,dir,&m,&w,&s,&k);	}
	/// Find position (after specified in i,j,k) of first nonzero value of formula
	inline mreal Find(const char *cond, long &i, long &j, long &k) const
	{	return mgl_data_first(this,cond,&i,&j,&k);	}
	/// Find position (before specified in i,j,k) of last nonzero value of formula
	inline mreal Last(const char *cond, long &i, long &j, long &k) const
	{	return mgl_data_last(this,cond,&i,&j,&k);	}
	/// Find position of first in direction 'dir' nonzero value of formula
	inline long Find(const char *cond, char dir, long i=0, long j=0, long k=0) const
	{	return mgl_data_find(this,cond,dir,i,j,k);	}
	/// Find if any nonzero value of formula
	inline bool FindAny(const char *cond) const
	{	return mgl_data_find_any(this,cond);	}

	/// Interpolate by cubic spline the data to given point x=[0...nx-1], y=[0...ny-1], z=[0...nz-1]
	inline mreal Spline(mreal x,mreal y=0,mreal z=0) const
	{	return value(x,y,z);	}
	/// Interpolate by cubic spline the data to given point x,\a y,\a z which normalized in range [0, 1]
	inline mreal Spline1(mreal x,mreal y=0,mreal z=0) const
	{	return value(x*(GetNx()-1),y*(GetNy()-1),z*(GetNz()-1));	}
	/// Interpolate by cubic spline the data and return its derivatives at given point x=[0...nx-1], y=[0...ny-1], z=[0...nz-1]
	inline mreal Spline(mglPoint &dif, mreal x,mreal y=0,mreal z=0) const
	{	return valueD(x,y,z, &(dif.x),&(dif.y), &(dif.z));	}
	/// Interpolate by cubic spline the data and return its derivatives at given point x,\a y,\a z which normalized in range [0, 1]
	inline mreal Spline1(mglPoint &dif, mreal x,mreal y=0,mreal z=0) const
	{	mreal res=valueD(x*(GetNx()-1),y*(GetNy()-1),z*(GetNz()-1), &(dif.x),&(dif.y), &(dif.z));
		dif.x*=GetNx()>1?GetNx()-1:1;	dif.y*=GetNy()>1?GetNy()-1:1;	dif.z*=GetNz()>1?GetNz()-1:1;	return res;	}

	/// Interpolate by linear function the data to given point x=[0...nx-1], y=[0...ny-1], z=[0...nz-1]
	inline mreal Linear(mreal x,mreal y=0,mreal z=0)	const
	{	return mgl_data_linear_ext(this,x,y,z,0,0,0);	}
	/// Interpolate by line the data to given point x,\a y,\a z which normalized in range [0, 1]
	inline mreal Linear1(mreal x,mreal y=0,mreal z=0) const
	{	return mgl_data_linear_ext(this,x*(GetNx()-1),y*(GetNy()-1),z*(GetNz()-1),0,0,0);	}
	/// Interpolate by linear function the data and return its derivatives at given point x=[0...nx-1], y=[0...ny-1], z=[0...nz-1]
	inline mreal Linear(mglPoint &dif, mreal x,mreal y=0,mreal z=0)	const
	{	return mgl_data_linear_ext(this,x,y,z, &(dif.x),&(dif.y), &(dif.z));	}
	/// Interpolate by line the data and return its derivatives at given point x,\a y,\a z which normalized in range [0, 1]
	inline mreal Linear1(mglPoint &dif, mreal x,mreal y=0,mreal z=0) const
	{	mreal res=mgl_data_linear_ext(this,x*(GetNx()-1),y*(GetNy()-1),z*(GetNz()-1), &(dif.x),&(dif.y), &(dif.z));
		dif.x*=GetNx()>1?GetNx()-1:1;	dif.y*=GetNy()>1?GetNy()-1:1;	dif.z*=GetNz()>1?GetNz()-1:1;	return res;	}
};
//-----------------------------------------------------------------------------
/// Structure for color ID
struct MGL_EXPORT mglColorID
{
	char id;
	mglColor col;
};
MGL_EXPORT extern mglColorID mglColorIds[31];
MGL_EXPORT extern std::string mglGlobalMess;	///< Buffer for receiving global messages
//-----------------------------------------------------------------------------
#endif

#ifdef MGL_SRC
#define _Da_(d)	(*((const mglDataA *)(d)))
#define _DA_(a)	((const mglDataA *)*(a))
#define _GR_	((mglBase *)(*gr))
//#define _D_(d)	*((mglData *)*(d))
#define _DM_(a)	((mglData *)*(a))
#define _DT_	((mglData *)*d)
#endif

#endif
