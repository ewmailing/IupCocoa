/***************************************************************************
 * complex.cpp is part of Math Graphic Library
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
#include "mgl2/datac.h"
#include "mgl2/evalc.h"
#include "mgl2/thread.h"

#include "interp.hpp"
#define mgl2 	mreal(2)
#define mgl3 	mreal(3)
#define mgl4 	mreal(4)

//-----------------------------------------------------------------------------
void MGL_EXPORT mglStartThreadC(void *(*func)(void *), void (*post)(mglThreadC *,dual *), long n,
					dual *a, const dual *b, const dual *c, const long *p,
					const void *v, const dual *d, const dual *e, const char *s)
{
	if(!func)	return;
#if MGL_HAVE_PTHREAD
	if(mglNumThr<1)	mgl_set_num_thr(0);
	if(mglNumThr>1)
	{
		pthread_t *tmp=new pthread_t[mglNumThr];
		mglThreadC *par=new mglThreadC[mglNumThr];
		register long i;
		for(i=0;i<mglNumThr;i++)	// put parameters into the structure
		{	par[i].n=n;	par[i].a=a;	par[i].b=b;	par[i].c=c;	par[i].d=d;
			par[i].p=p;	par[i].v=v;	par[i].s=s;	par[i].e=e;	par[i].id=i;	}
		for(i=0;i<mglNumThr;i++)	pthread_create(tmp+i, 0, func, par+i);
		for(i=0;i<mglNumThr;i++)	pthread_join(tmp[i], 0);
		if(post)	post(par,a);
		delete []tmp;	delete []par;
	}
	else
#endif
	{
		mglNumThr = 1;
		mglThreadC par;
		par.n=n;	par.a=a;	par.b=b;	par.c=c;	par.d=d;
		par.p=p;	par.v=v;	par.s=s;	par.e=e;	par.id=0;
		func(&par);
		if(post)	post(&par,a);
	}
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mglStartThreadV(void *(*func)(void *), long n, dual *a, const void *b,
					const void *c, const long *p, const void *v, const mreal *d)
{
	if(!func)	return;
#if MGL_HAVE_PTHREAD
	if(mglNumThr<1)	mgl_set_num_thr(0);
	if(mglNumThr>1)
	{
		pthread_t *tmp=new pthread_t[mglNumThr];
		mglThreadV *par=new mglThreadV[mglNumThr];
		register long i;
		for(i=0;i<mglNumThr;i++)	// put parameters into the structure
		{	par[i].n=n;	par[i].a=0;	par[i].b=b;	par[i].c=c;	par[i].d=d;
			par[i].p=p;	par[i].v=v;	par[i].id=i;par[i].aa=a;	}
		for(i=0;i<mglNumThr;i++)	pthread_create(tmp+i, 0, func, par+i);
		for(i=0;i<mglNumThr;i++)	pthread_join(tmp[i], 0);
		delete []tmp;	delete []par;
	}
	else
#endif
	{
		mglNumThr = 1;
		mglThreadV par;
		par.n=n;	par.a=0;	par.b=b;	par.c=c;	par.d=d;
		par.p=p;	par.v=v;	par.id=0;	par.aa=a;
		func(&par);
	}
}
//-----------------------------------------------------------------------------
mdual MGL_EXPORT_CONST mgl_expi(dual a)
{
	dual r = exp(dual(0,1)*dual(a));
	return r.real()+r.imag()*mgl_I;
}
//-----------------------------------------------------------------------------
MGL_NO_EXPORT void *mgl_csmth_x(void *par)
{
	mglThreadC *t=(mglThreadC *)par;
	long nx=t->p[0], kind=t->p[2];
	dual *b=t->a;
	const dual *a=t->b;
	if(kind>0)
#if !MGL_HAVE_PTHREAD
#pragma omp parallel for
#endif
		for(long i=t->id;i<t->n;i+=mglNumThr)
		{
			register long j = i%nx;
			if(j-kind<0)	j = i+kind-j;
			else if(j+kind>nx-1)	j = i+nx-1-j-kind;
			else	j=i;
			for(long k=-kind;k<=kind;k++)	b[i] += a[j+k]/mreal(2*kind+1);
		}
	else
#if !MGL_HAVE_PTHREAD
#pragma omp parallel for
#endif
		for(long i=t->id;i<t->n;i+=mglNumThr)
		{
			register long j = i%nx;
			if(j>1 && j<nx-2)	b[i] = (mreal(12)*a[i-2] - mreal(3)*a[i-1] + mreal(17)*a[i] - mreal(3)*a[i+1] + mreal(12)*a[i+2])/mreal(35);
			else if(j==1 || j==nx-2)	b[i] = (a[i-1] + a[i] + a[i+1])/mreal(3);
			else	b[i] = a[i];
		}
	return 0;
}
MGL_NO_EXPORT void *mgl_csmth_y(void *par)
{
	mglThreadC *t=(mglThreadC *)par;
	long nx=t->p[0],ny=t->p[1], kind=t->p[2];
	dual *b=t->a;
	const dual *a=t->b;
	if(kind>0)
#if !MGL_HAVE_PTHREAD
#pragma omp parallel for
#endif
		for(long i=t->id;i<t->n;i+=mglNumThr)
		{
			register long j = (i/nx)%ny;
			if(j-kind<0)	j = i+(kind-j)*nx;
			else if(j+kind>ny-1)	j = i+(ny-1-j-kind)*nx;
			else	j=i;
			for(long k=-kind;k<=kind;k++)	b[i] += a[j+k*nx]/mreal(2*kind+1);
		}
	else
#if !MGL_HAVE_PTHREAD
#pragma omp parallel for
#endif
		for(long i=t->id;i<t->n;i+=mglNumThr)
		{
			register long j = (i/nx)%ny;
			if(j>1 && j<ny-2)	b[i] = (mreal(12)*a[i-2*nx] - mreal(3)*a[i-nx] + mreal(17)*a[i] - mreal(3)*a[i+nx] + mreal(12)*a[i+2*nx])/mreal(35);
			else if(j==1 || j==ny-2)	b[i] = (a[i-nx] + a[i] + a[i+nx])/mreal(3);
			else	b[i] = a[i];
		}
	return 0;
}
MGL_NO_EXPORT void *mgl_csmth_z(void *par)
{
	mglThreadC *t=(mglThreadC *)par;
	register long nn=t->p[0]*t->p[1], nz=t->n/nn, kind=t->p[2];
	dual *b=t->a;
	const dual *a=t->b;
	if(kind>0)
#if !MGL_HAVE_PTHREAD
#pragma omp parallel for
#endif
		for(long i=t->id;i<t->n;i+=mglNumThr)
		{
			register long j = i/nn;
			if(j-kind<0)	j = i+(kind-j)*nn;
			else if(j+kind>nz-1)	j = i+(nz-1-j-kind)*nn;
			else	j=i;
			for(long k=-kind;k<=kind;k++)	b[i] += a[j+k*nn]/mreal(2*kind+1);
		}
	else
#if !MGL_HAVE_PTHREAD
#pragma omp parallel for
#endif
		for(long i=t->id;i<t->n;i+=mglNumThr)
		{
			register long j = i/nn;
			if(j>1 && j<nz-2)	b[i] = (mreal(12)*a[i-2*nn] - mreal(3)*a[i-nn] + mreal(17)*a[i] - mreal(3)*a[i+nn] + mreal(12)*a[i+2*nn])/mreal(35);
			else if(j==1 || j==nz-2)	b[i] = (a[i-nn] + a[i] + a[i+nn])/mreal(3);
			else	b[i] = a[i];
		}
	return 0;
}
void MGL_EXPORT mgl_datac_smooth(HADT d, const char *dirs, mreal )
{
	long Type = -1;
	if(!dirs || *dirs==0)	dirs = "xyz";
	if(strchr(dirs,'0'))	return;
	if(strchr(dirs,'d'))
	{
		if(strchr(dirs,'1'))	Type = 1;
		if(strchr(dirs,'2'))	Type = 2;
		if(strchr(dirs,'3'))	Type = 3;
		if(strchr(dirs,'4'))	Type = 4;
		if(strchr(dirs,'5'))	Type = 5;
		if(strchr(dirs,'6'))	Type = 6;
		if(strchr(dirs,'7'))	Type = 7;
		if(strchr(dirs,'8'))	Type = 8;
		if(strchr(dirs,'9'))	Type = 9;
	}
	else
	{
		if(strchr(dirs,'1'))	return;
		if(strchr(dirs,'3'))	Type = 1;
		if(strchr(dirs,'5'))	Type = 2;
	}
	long nx=d->nx,ny=d->ny,nz=d->nz;
//	if(Type == SMOOTH_NONE)	return;
	long p[3]={nx,ny,Type};
	dual *b = new dual[nx*ny*nz];
	// ����������� �� x
	memset(b,0,nx*ny*nz*sizeof(dual));
	if(nx>4 && strchr(dirs,'x'))
	{
		mglStartThreadC(mgl_csmth_x,0,nx*ny*nz,b,d->a,0,p);
		memcpy(d->a,b,nx*ny*nz*sizeof(dual));
		memset(b,0,nx*ny*nz*sizeof(dual));
	}
	if(ny>4 && strchr(dirs,'y'))
	{
		mglStartThreadC(mgl_csmth_y,0,nx*ny*nz,b,d->a,0,p);
		memcpy(d->a,b,nx*ny*nz*sizeof(dual));
		memset(b,0,nx*ny*nz*sizeof(dual));
	}
	if(nz>4 && strchr(dirs,'z'))
	{
		mglStartThreadC(mgl_csmth_z,0,nx*ny*nz,b,d->a,0,p);
		memcpy(d->a,b,nx*ny*nz*sizeof(dual));
	}
	delete []b;
}
void MGL_EXPORT mgl_datac_smooth_(uintptr_t *d, const char *dir, mreal *delta,int l)
{	char *s=new char[l+1];	memcpy(s,dir,l);	s[l]=0;
	mgl_datac_smooth(_DC_,s,*delta);		delete []s;	}
//-----------------------------------------------------------------------------
MGL_NO_EXPORT void *mgl_ccsum_z(void *par)
{
	mglThreadC *t=(mglThreadC *)par;
	long nz=t->p[2], nn=t->n;
	dual *b=t->a;
	const dual *a=t->b;
#if !MGL_HAVE_PTHREAD
#pragma omp parallel for
#endif
	for(long i=t->id;i<nn;i+=mglNumThr)
	{
		b[i] = a[i];
		for(long j=1;j<nz;j++)	b[i+j*nn] = b[i+j*nn-nn] + a[i+j*nn];
	}
	return 0;
}
MGL_NO_EXPORT void *mgl_ccsum_y(void *par)
{
	mglThreadC *t=(mglThreadC *)par;
	long nx=t->p[0], ny=t->p[1], nn=t->n;
	dual *b=t->a;
	const dual *a=t->b;
#if !MGL_HAVE_PTHREAD
#pragma omp parallel for
#endif
	for(long i=t->id;i<nn;i+=mglNumThr)
	{
		register long k = (i%nx)+nx*ny*(i/nx);		b[k] = a[k];
		for(long j=1;j<ny;j++)	b[k+j*nx] = b[k+j*nx-nx] + a[k+nx*j];
	}
	return 0;
}
MGL_NO_EXPORT void *mgl_ccsum_x(void *par)
{
	mglThreadC *t=(mglThreadC *)par;
	long nx=t->p[0], nn=t->n;
	dual *b=t->a;
	const dual *a=t->b;
#if !MGL_HAVE_PTHREAD
#pragma omp parallel for
#endif
	for(long i=t->id;i<nn;i+=mglNumThr)
	{
		register long k = i*nx;			b[k] = a[k];
		for(long j=1;j<nx;j++)	b[j+k] = b[j+k-1] + a[j+k];
	}
	return 0;
}
void MGL_EXPORT mgl_datac_cumsum(HADT d, const char *dir)
{
	if(!dir || *dir==0)	return;
	long nx=d->nx,ny=d->ny,nz=d->nz,nn=nx*ny*nz;
	long p[3]={nx,ny,nz};
	dual *b = new dual[nn];
	memcpy(b,d->a,nn*sizeof(dual));
	if(strchr(dir,'z') && nz>1)
	{
		mglStartThreadC(mgl_ccsum_z,0,nx*ny,b,d->a,0,p);
		memcpy(d->a,b,nn*sizeof(dual));
	}
	if(strchr(dir,'y') && ny>1)
	{
		mglStartThreadC(mgl_ccsum_y,0,nx*nz,b,d->a,0,p);
		memcpy(d->a,b,nn*sizeof(dual));
	}
	if(strchr(dir,'x') && nx>1)
	{
		mglStartThreadC(mgl_ccsum_x,0,nz*ny,b,d->a,0,p);
		memcpy(d->a,b,nn*sizeof(dual));
	}
	delete []b;
}
void MGL_EXPORT mgl_datac_cumsum_(uintptr_t *d, const char *dir,int l)
{	char *s=new char[l+1];	memcpy(s,dir,l);	s[l]=0;
	mgl_datac_cumsum(_DC_,s);	delete []s;	}
//-----------------------------------------------------------------------------
MGL_NO_EXPORT void *mgl_cint_z(void *par)
{
	mglThreadC *t=(mglThreadC *)par;
	long nz=t->p[2], nn=t->n;
	dual *b=t->a, dd=0.5/nz;
	const dual *a=t->b;
#if !MGL_HAVE_PTHREAD
#pragma omp parallel for
#endif
	for(long i=t->id;i<nn;i+=mglNumThr)
	{
		b[i] = 0;
		for(long j=1;j<nz;j++)	b[i+j*nn] = b[i+j*nn-nn] + (a[i+nn*j]+a[i+j*nn-nn])*dd;
	}
	return 0;
}
MGL_NO_EXPORT void *mgl_cint_y(void *par)
{
	mglThreadC *t=(mglThreadC *)par;
	long nx=t->p[0], ny=t->p[1], nn=t->n;
	dual *b=t->a, dd=0.5/ny;
	const dual *a=t->b;
#if !MGL_HAVE_PTHREAD
#pragma omp parallel for
#endif
	for(long i=t->id;i<nn;i+=mglNumThr)
	{
		register long k = (i%nx)+nx*ny*(i/nx);	b[k] = 0;
		for(long j=1;j<ny;j++)	b[k+j*nx] = b[k+j*nx-nx] + (a[k+j*nx]+a[k+j*nx-nx])*dd;
	}
	return 0;
}
MGL_NO_EXPORT void *mgl_cint_x(void *par)
{
	mglThreadC *t=(mglThreadC *)par;
	long nx=t->p[0], nn=t->n;
	dual *b=t->a, dd=0.5/nx;
	const dual *a=t->b;
#if !MGL_HAVE_PTHREAD
#pragma omp parallel for
#endif
	for(long i=t->id;i<nn;i+=mglNumThr)
	{
		register long k = i*nx;			b[k] = 0;
		for(long j=1;j<nx;j++)	b[j+k] = b[j+k-1] + (a[j+k]+a[j+k-1])*dd;
	}
	return 0;
}
void MGL_EXPORT mgl_datac_integral(HADT d, const char *dir)
{
	if(!dir || *dir==0)	return;
	long nx=d->nx,ny=d->ny,nz=d->nz,nn=nx*ny*nz;
	long p[3]={nx,ny,nz};
	dual *b = new dual[nn];
	memcpy(b,d->a,nn*sizeof(dual));
	if(strchr(dir,'z') && nz>1)
	{
		mglStartThreadC(mgl_cint_z,0,nx*ny,b,d->a,0,p);
		memcpy(d->a,b,nn*sizeof(dual));
	}
	if(strchr(dir,'y') && ny>1)
	{
		mglStartThreadC(mgl_cint_y,0,nx*nz,b,d->a,0,p);
		memcpy(d->a,b,nn*sizeof(dual));
	}
	if(strchr(dir,'x') && nx>1)
	{
		mglStartThreadC(mgl_cint_x,0,nz*ny,b,d->a,0,p);
		memcpy(d->a,b,nn*sizeof(dual));
	}
	delete []b;
}
void MGL_EXPORT mgl_datac_integral_(uintptr_t *d, const char *dir,int l)
{	char *s=new char[l+1];	memcpy(s,dir,l);	s[l]=0;
	mgl_datac_integral(_DC_,s);	delete []s;	}
//-----------------------------------------------------------------------------
MGL_NO_EXPORT void *mgl_cdif_z(void *par)
{
	mglThreadC *t=(mglThreadC *)par;
	long nz=t->p[2], nn=t->n;
	dual *b=t->a, dd=0.5*nz;
	const dual *a=t->b;
#if !MGL_HAVE_PTHREAD
#pragma omp parallel for
#endif
	for(long i=t->id;i<nn;i+=mglNumThr)
	{
		b[i] = -(mgl3*a[i]-mgl4*a[i+nn]+a[i+2*nn])*dd;
		b[i+(nz-1)*nn] = (mgl3*a[i+(nz-1)*nn]-mgl4*a[i+(nz-2)*nn]+a[i+(nz-3)*nn])*dd;
		for(long j=1;j<nz-1;j++)		b[i+j*nn] = (a[i+j*nn+nn]-a[i+j*nn-nn])*dd;
	}
	return 0;
}
MGL_NO_EXPORT void *mgl_cdif_y(void *par)
{
	mglThreadC *t=(mglThreadC *)par;
	long nx=t->p[0], ny=t->p[1], nn=t->n;
	dual *b=t->a, dd=0.5*ny;
	const dual *a=t->b;
#if !MGL_HAVE_PTHREAD
#pragma omp parallel for
#endif
	for(long i=t->id;i<nn;i+=mglNumThr)
	{
		register long k = (i%nx)+nx*ny*(i/nx);
		b[k] = -(mgl3*a[k]-mgl4*a[k+nx]+a[k+2*nx])*dd;
		b[k+(ny-1)*nx] = (mgl3*a[k+(ny-1)*nx]-mgl4*a[k+(ny-2)*nx]+a[k+(ny-3)*nx])*dd;
		for(long j=1;j<ny-1;j++)	b[k+j*nx] = (a[k+j*nx+nx]-a[k+j*nx-nx])*dd;
	}
	return 0;
}
MGL_NO_EXPORT void *mgl_cdif_x(void *par)
{
	mglThreadC *t=(mglThreadC *)par;
	long nx=t->p[0], nn=t->n;
	dual *b=t->a, dd=0.5*nx;
	const dual *a=t->b;
#if !MGL_HAVE_PTHREAD
#pragma omp parallel for
#endif
	for(long i=t->id;i<nn;i+=mglNumThr)
	{
		register long k = i*nx;
		b[k] = -(mgl3*a[k]-mgl4*a[k+1]+a[k+2])*dd;
		b[k+nx-1] = (mgl3*a[k+nx-1]-mgl4*a[k+nx-2]+a[k+nx-3])*dd;
		for(long j=1;j<nx-1;j++)	b[j+k] = (a[j+k+1]-a[j+k-1])*dd;
	}
	return 0;
}
void MGL_EXPORT mgl_datac_diff(HADT d, const char *dir)
{
	if(!dir || *dir==0)	return;
	long nx=d->nx,ny=d->ny,nz=d->nz,nn=nx*ny*nz;
	long p[3]={nx,ny,nz};
	dual *b = new dual[nn];
	if(strchr(dir,'z') && nz>1)
	{
		mglStartThreadC(mgl_cdif_z,0,nx*ny,b,d->a,0,p);
		memcpy(d->a,b,nn*sizeof(dual));
	}
	if(strchr(dir,'y') && ny>1)
	{
		mglStartThreadC(mgl_cdif_y,0,nx*nz,b,d->a,0,p);
		memcpy(d->a,b,nn*sizeof(dual));
	}
	if(strchr(dir,'x') && nx>1)
	{
		mglStartThreadC(mgl_cdif_x,0,nz*ny,b,d->a,0,p);
		memcpy(d->a,b,nn*sizeof(dual));
	}
	delete []b;
}
void MGL_EXPORT mgl_datac_diff_(uintptr_t *d, const char *dir,int l)
{	char *s=new char[l+1];	memcpy(s,dir,l);	s[l]=0;
	mgl_datac_diff(_DC_,s);	delete []s;	}
//-----------------------------------------------------------------------------
MGL_NO_EXPORT void *mgl_cdif2_z(void *par)
{
	mglThreadC *t=(mglThreadC *)par;
	long nz=t->p[2], nn=t->n;
	dual *b=t->a, dd=0.5*nz*nz;
	const dual *a=t->b;
#if !MGL_HAVE_PTHREAD
#pragma omp parallel for
#endif
	for(long i=t->id;i<nn;i+=mglNumThr)
	{
		b[i] = b[i+(nz-1)*nn] = 0;
		for(long j=1;j<nz-1;j++)		b[i+j*nn] = (a[i+j*nn+nn]+a[i+j*nn-nn]-mgl2*a[i+j*nn])*dd;
	}
	return 0;
}
MGL_NO_EXPORT void *mgl_cdif2_y(void *par)
{
	mglThreadC *t=(mglThreadC *)par;
	long nx=t->p[0], ny=t->p[1], nn=t->n;
	dual *b=t->a, dd=0.5*ny*ny;
	const dual *a=t->b;
#if !MGL_HAVE_PTHREAD
#pragma omp parallel for
#endif
	for(long i=t->id;i<nn;i+=mglNumThr)
	{
		register long k = (i%nx)+nx*ny*(i/nx);	b[k] = b[k+(ny-1)*nx] = 0;
		for(long j=1;j<ny-1;j++)	b[k+j*nx] = (a[k+j*nx+nx]+a[k+j*nx-nx]-mgl2*a[k+j*nx])*dd;
	}
	return 0;
}
MGL_NO_EXPORT void *mgl_cdif2_x(void *par)
{
	mglThreadC *t=(mglThreadC *)par;
	long nx=t->p[0], nn=t->n;
	dual *b=t->a, dd=0.5*nx*nx;
	const dual *a=t->b;
#if !MGL_HAVE_PTHREAD
#pragma omp parallel for
#endif
	for(long i=t->id;i<nn;i+=mglNumThr)
	{
		register long k = i*nx;			b[k] = b[k+nx-1] = 0;
		for(long j=1;j<nx-1;j++)	b[j+k] = (a[j+k+1]+a[j+k-1]-mgl2*a[j+k])*dd;
	}
	return 0;
}
void MGL_EXPORT mgl_datac_diff2(HADT d, const char *dir)
{
	if(!dir || *dir==0)	return;
	long nx=d->nx,ny=d->ny,nz=d->nz,nn=nx*ny*nz;
	long p[3]={nx,ny,nz};
	dual *b = new dual[nn];
	if(strchr(dir,'z') && nz>1)
	{
		mglStartThreadC(mgl_cdif2_z,0,nx*ny,b,d->a,0,p);
		memcpy(d->a,b,nn*sizeof(dual));
	}
	if(strchr(dir,'y') && ny>1)
	{
		mglStartThreadC(mgl_cdif2_y,0,nx*nz,b,d->a,0,p);
		memcpy(d->a,b,nn*sizeof(dual));
	}
	if(strchr(dir,'x') && nx>1)
	{
		mglStartThreadC(mgl_cdif2_x,0,nz*ny,b,d->a,0,p);
		memcpy(d->a,b,nn*sizeof(dual));
	}
	delete []b;
}
void MGL_EXPORT mgl_datac_diff2_(uintptr_t *d, const char *dir,int l)
{	char *s=new char[l+1];	memcpy(s,dir,l);	s[l]=0;
	mgl_datac_diff2(_DC_,s);	delete []s;	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_datac_swap(HADT d, const char *dir)
{
	if(!dir || *dir==0)	return;
	if(strchr(dir,'z') && d->nz>1)	mgl_datac_roll(d,'z',d->nz/2);
	if(strchr(dir,'y') && d->ny>1)	mgl_datac_roll(d,'y',d->ny/2);
	if(strchr(dir,'x') && d->nx>1)	mgl_datac_roll(d,'x',d->nx/2);
}
void MGL_EXPORT mgl_datac_swap_(uintptr_t *d, const char *dir,int l)
{	char *s=new char[l+1];	memcpy(s,dir,l);	s[l]=0;
	mgl_datac_swap(_DC_,s);	delete []s;	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_datac_roll(HADT dd, char dir, long num)
{
	long nx=dd->nx,ny=dd->ny,nz=dd->nz, d;
	dual *b,*a=dd->a;
	if(dir=='z' && nz>1)
	{
		d = num>0 ? num%nz : (num+nz*(1-num/nz))%nz;
		if(d==0)	return;		// nothing to do
		b = new dual[nx*ny*nz];
		memcpy(b,a+nx*ny*d,nx*ny*(nz-d)*sizeof(dual));
		memcpy(b+nx*ny*(nz-d),a,nx*ny*d*sizeof(dual));
		memcpy(a,b,nx*ny*nz*sizeof(dual));	delete []b;
	}
	if(dir=='y' && ny>1)
	{
		d = num>0 ? num%ny : (num+ny*(1-num/ny))%ny;
		if(d==0)	return;		// nothing to do
		b = new dual[nx*ny*nz];
		memcpy(b,a+nx*d,(nx*ny*nz-nx*d)*sizeof(dual));
#pragma omp parallel for
		for(long i=0;i<nz;i++)
			memcpy(b+nx*(ny-d)+nx*ny*i,a+nx*ny*i,nx*d*sizeof(dual));
		memcpy(a,b,nx*ny*nz*sizeof(dual));	delete []b;
	}
	if(dir=='x' && nx>1)
	{
		d = num>0 ? num%nx : (num+nx*(1-num/nx))%nx;
		if(d==0)	return;		// nothing to do
		b = new dual[nx*ny*nz];
		memcpy(b,a+d,(nx*ny*nz-d)*sizeof(dual));
#pragma omp parallel for
		for(long i=0;i<nz*ny;i++)
			memcpy(b+nx-d+nx*i,a+nx*i,d*sizeof(dual));
		memcpy(a,b,nx*ny*nz*sizeof(dual));	delete []b;
	}
}
void MGL_EXPORT mgl_datac_roll_(uintptr_t *d, const char *dir, int *num, int)
{	mgl_datac_roll(_DC_,*dir,*num);	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_datac_mirror(HADT d, const char *dir)
{
	if(!dir || *dir==0)	return;
	long nx=d->nx,ny=d->ny,nz=d->nz;
	dual *a=d->a;
	if(strchr(dir,'z') && nz>1)
	{
#pragma omp parallel for collapse(2)
		for(long i=0;i<nx*ny;i++)	for(long j=0;j<nz/2;j++)
		{
			register long i0 = i+j*nx*ny, j0 = i+(nz-1-j)*nx*ny;
			dual b = a[i0];	a[i0] = a[j0];	a[j0] = b;
		}
	}
	if(strchr(dir,'y') && ny>1)
	{
#pragma omp parallel for collapse(2)
		for(long i=0;i<nx*nz;i++)	for(long j=0;j<ny/2;j++)
		{
			register long j0 = (i%nx)+nx*(ny*(i/nx)+j), i0 = j0+(ny-1-2*j)*nx;
			dual b = a[j0];	a[j0] = a[i0];	a[i0] = b;
		}
	}
	if(strchr(dir,'x') && nx>1)
	{
#pragma omp parallel for collapse(2)
		for(long j=0;j<ny*nz;j++)	for(long i=0;i<nx/2;i++)
		{
			register long i0 = nx-1-i+j*nx, j0 = i+j*nx;
			dual b = a[j0];	a[j0] = a[i0];	a[i0] = b;
		}
	}
}
void MGL_EXPORT mgl_datac_mirror_(uintptr_t *d, const char *dir,int l)
{	char *s=new char[l+1];	memcpy(s,dir,l);	s[l]=0;
	mgl_datac_mirror(_DC_,s);	delete []s;	}
//-----------------------------------------------------------------------------
dual MGL_EXPORT mglSpline3Cs(const dual *a, long nx, long ny, long nz, mreal x, mreal y, mreal z)
{	return mglSpline3st<dual>(a,nx,ny,nz,x,y,z);	}
//-----------------------------------------------------------------------------
dual MGL_EXPORT mglSpline3C(const dual *a, long nx, long ny, long nz, mreal x, mreal y, mreal z,dual *dx, dual *dy, dual *dz)
{	return mglSpline3t<dual>(a,nx,ny,nz,x,y,z,dx,dy,dz);	}
//-----------------------------------------------------------------------------
dual MGL_EXPORT mglLinearC(const dual *a, long nx, long ny, long nz, mreal x, mreal y, mreal z)
{	return mglLineart<dual>(a,nx,ny,nz,x,y,z);	}
//-----------------------------------------------------------------------------
mdual MGL_EXPORT mgl_datac_spline(HCDT d, mreal x,mreal y,mreal z)
{
	const mglDataC *dd=dynamic_cast<const mglDataC *>(d);
	dual r = dd ? mglSpline3st<dual>(dd->a,dd->nx,dd->ny,dd->nz,x,y,z) : d->value(x,y,z);
	return r.real()+r.imag()*mgl_I;
}
//-----------------------------------------------------------------------------
mdual MGL_EXPORT mgl_datac_spline_ext(HCDT d, mreal x,mreal y,mreal z, dual *dx,dual *dy,dual *dz)
{
	const mglDataC *dd=dynamic_cast<const mglDataC *>(d);
	if(!dd)
	{
		mreal rx=0,ry=0,rz=0,res;
		res=d->valueD(x,y,z,&rx,&ry,&rz);
		if(dx)	*dx=rx;	if(dy)	*dy=ry;	if(dz)	*dz=rz;
		return res;
	}
	dual r = mglSpline3t<dual>(dd->a,dd->nx,dd->ny,dd->nz,x,y,z,dx,dy,dz);
	return r.real()+r.imag()*mgl_I;
}
//-----------------------------------------------------------------------------
mdual MGL_EXPORT mgl_datac_spline_(uintptr_t *d, mreal *x,mreal *y,mreal *z)
{	return mgl_datac_spline(_DA_(d),*x,*y,*z);	}
mdual MGL_EXPORT mgl_datac_spline_ext_(uintptr_t *d, mreal *x,mreal *y,mreal *z, dual *dx,dual *dy,dual *dz)
{	return mgl_datac_spline_ext(_DA_(d),*x,*y,*z,dx,dy,dz);	}
//-----------------------------------------------------------------------------
mdual MGL_EXPORT mgl_datac_linear_ext(HCDT d, mreal x,mreal y,mreal z, dual *dx,dual *dy,dual *dz)
{
	long kx=long(x), ky=long(y), kz=long(z);
	dual b0,b1;
	const mglDataC *dd=dynamic_cast<const mglDataC *>(d);
	if(!dd)
	{
		mreal rx=0,ry=0,rz=0,res;
		res=mgl_data_linear_ext(d,x,y,z,&rx,&ry,&rz);
		if(dx)	*dx=rx;	if(dy)	*dy=ry;	if(dz)	*dz=rz;
		return res;
	}

	long nx=dd->nx, ny=dd->ny, nz=dd->nz, dn=ny>1?nx:0;
	kx = kx>=0 ? kx:0;	kx = kx<nx-1 ? kx:nx-2;
	ky = ky>=0 ? ky:0;	ky = ky<ny-1 ? ky:ny-2;
	kz = kz>=0 ? kz:0;	kz = kz<nz-1 ? kz:nz-2;
	x -= kx;	y -= ky;	z -= kz;
	const dual *aa = dd->a, *bb;
	if(kz>=0)
	{
		aa=dd->a+kx+nx*(ky+ny*kz);	bb = aa+nx*ny;
		b0 = aa[0]*(1-x-y+x*y) + x*(1-y)*aa[1] + y*(1-x)*aa[dn] + x*y*aa[1+dn];
		b1 = bb[0]*(1-x-y+x*y) + x*(1-y)*bb[1] + y*(1-x)*bb[dn] + x*y*bb[1+dn];
	}
	else
	{
		z=0;
		if(ky>=0)
		{
			aa=dd->a+kx+nx*ky;
			b0 = b1 = aa[0]*(1-x-y+x*y) + x*(1-y)*aa[1] + y*(1-x)*aa[dn] + x*y*aa[1+dn];
		}
		else if(kx>=0)
		{
			aa=dd->a+kx;	b0 = b1 = aa[0]*(1-x) + x*aa[1];
		}
		else	b0 = b1 = dd->a[0];
	}
	if(dx)	*dx = kx>=0?aa[1]-aa[0]:0;
	if(dy)	*dy = ky>=0?aa[dn]-aa[0]:0;
	if(dz)	*dz = b1-b0;
	dual r = b0 + z*(b1-b0);
	return r.real()+r.imag()*mgl_I;
}
mdual MGL_EXPORT mgl_datac_linear(HCDT d, mreal x,mreal y,mreal z)
{	return mgl_datac_linear_ext(d, x,y,z, 0,0,0);	}
//-----------------------------------------------------------------------------
mdual MGL_EXPORT mgl_datac_linear_(uintptr_t *d, mreal *x,mreal *y,mreal *z)
{	return mgl_datac_linear(_DA_(d),*x,*y,*z);	}
mdual MGL_EXPORT mgl_datac_linear_ext_(uintptr_t *d, mreal *x,mreal *y,mreal *z, dual *dx,dual *dy,dual *dz)
{	return mgl_datac_linear_ext(_DA_(d),*x,*y,*z,dx,dy,dz);	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_datac_crop(HADT d, long n1, long n2, char dir)
{
	long nx=d->nx,ny=d->ny,nz=d->nz, nn;
	dual *b;
	if(n1<0)	n1=0;
	switch(dir)
	{
	case 'x':
		if(n1>=nx)	break;
		n2 = n2>0 ? n2 : nx+n2;
		if(n2<0 || n2>=nx || n2<n1)	n2 = nx;
		nn = n2-n1;	b = new dual[nn*ny*nz];
#pragma omp parallel for
		for(long i=0;i<ny*nz;i++)
			memcpy(b+nn*i,d->a+nx*i+n1,nn*sizeof(dual));
		d->nx = nn;	if(!d->link)	delete []d->a;
		d->a = b;	d->link=false;	d->NewId();
		break;
	case 'y':
		if(n1>=ny)	break;
		n2 = n2>0 ? n2 : ny+n2;
		if(n2<0 || n2>=ny || n2<n1)	n2 = ny;
		nn = n2-n1;	b = new dual[nn*nx*nz];
#pragma omp parallel for
		for(long i=0;i<nn;i++)	for(long j=0;j<nz;j++)
			memcpy(b+nx*(i+nn*j),d->a+nx*(n1+i+ny*j),nx*sizeof(dual));
		d->ny = nn;	if(!d->link)	delete []d->a;
		d->a = b;	d->link=false;
		break;
	case 'z':
		if(n1>=nz)	break;
		n2 = n2>0 ? n2 : nz+n2;
		if(n2<0 || n2>=nz || n2<n1)	n2 = nz;
		nn = n2-n1;	b = new dual[nn*nx*ny];
		memcpy(b,d->a+nx*ny*n1,nn*nx*ny*sizeof(dual));
		d->nz = nn;	if(!d->link)	delete []d->a;
		d->a = b;	d->link=false;
		break;
	}
}
void MGL_EXPORT mgl_datac_crop_(uintptr_t *d, int *n1, int *n2, const char *dir,int)
{	mgl_datac_crop(_DC_,*n1,*n2,*dir);	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_datac_insert(HADT d, char dir, long at, long num)
{
	if(num<1)	return;
	at = at<0 ? 0:at;
	long nx=d->nx, ny=d->ny, nz=d->nz, nn;
	mglDataC b;
	if(dir=='x')
	{
		if(at>nx)	at=nx;
		nn=nx+num;	b.Create(nn,ny,nz);
#pragma omp parallel for
		for(long k=0;k<ny*nz;k++)
		{
			if(at>0)	memcpy(b.a+nn*k, d->a+nx*k,at*sizeof(dual));
			if(at<nx)	memcpy(b.a+at+num+nn*k, d->a+at+nx*k,(nx-at)*sizeof(dual));
			for(long i=0;i<num;i++)	b.a[nn*k+at+i]=d->a[nx*k+at];	// copy values
		}
		d->Set(b);	nx+=num;
	}
	if(dir=='y')
	{
		if(at>ny)	at=ny;
		nn=num+ny;	b.Create(nx,nn,nz);
#pragma omp parallel for
		for(long k=0;k<nz;k++)
		{
			if(at>0)	memcpy(b.a+nx*nn*k, d->a+nx*ny*k,at*nx*sizeof(dual));
			if(at<ny)	memcpy(b.a+nx*(at+num+nn*k), d->a+nx*(at+ny*k),(ny-at)*nx*sizeof(dual));
			for(long i=0;i<num;i++)	memcpy(b.a+nx*(nn*k+at+i),d->a+nx*(ny*k+at),nx*sizeof(dual));
		}
		d->Set(b);	ny+=num;
	}
	if(dir=='z')
	{
		if(at>nz)	at=nz;
		b.Create(nx,ny,nz+num);
		if(at>0)	memcpy(b.a, d->a,at*nx*ny*sizeof(dual));
		if(at<nz)	memcpy(b.a+nx*ny*(at+num), d->a+nx*ny*at,(nz-at)*nx*ny*sizeof(dual));
#pragma omp parallel for
		for(long i=0;i<num;i++)	memcpy(b.a+nx*ny*(at+i),d->a+nx*ny*at,nx*ny*sizeof(dual));
		d->Set(b);
	}
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_datac_delete(HADT d, char dir, long at, long num)
{
	if(num<1 || at<0)	return;
	mglDataC b;
	long nx=d->nx, ny=d->ny, nz=d->nz, nn;
	if(dir=='x')
	{
		if(at+num>=nx)	return;
		nn=nx-num;	b.Create(nn,ny,nz);
#pragma omp parallel for
		for(long k=0;k<ny*nz;k++)
		{
			if(at>0)	memcpy(b.a+nn*k, d->a+nx*k,at*sizeof(dual));
			memcpy(b.a+at+nn*k, d->a+at+num+nx*k,(nx-at-num)*sizeof(dual));
		}
		d->Set(b);	nx-=num;
	}
	if(dir=='y')
	{
		if(at+num>=ny)	return;
		nn=ny-num;	b.Create(nx,nn,nz);
#pragma omp parallel for
		for(long k=0;k<nz;k++)
		{
			if(at>0)	memcpy(b.a+nx*nn*k, d->a+nx*ny*k,at*nx*sizeof(dual));
			memcpy(b.a+nx*(at+nn*k), d->a+nx*(at+num+ny*k),(ny-at-num)*nx*sizeof(dual));
		}
		d->Set(b);	ny-=num;
	}
	if(dir=='z')
	{
		if(at+num>=nz)	return;
		b.Create(nx,ny,nz-num);
		if(at>0)	memcpy(b.a, d->a,at*nx*ny*sizeof(dual));
		memcpy(b.a+nx*ny*at, d->a+nx*ny*(at+num),(nz-at-num)*nx*ny*sizeof(dual));
		d->Set(b);
	}
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_datac_insert_(uintptr_t *d, const char *dir, int *at, int *num, int)
{	mgl_datac_insert(_DC_,*dir,*at,*num);	}
void MGL_EXPORT mgl_datac_delete_(uintptr_t *d, const char *dir, int *at, int *num, int)
{	mgl_datac_delete(_DC_,*dir,*at,*num);	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_datac_set_value(HADT dat, dual v, long i, long j, long k)
{
	if(i>=0 && i<dat->nx && j>=0 && j<dat->ny && k>=0 && k<dat->nz)
		dat->a[i+dat->nx*(j+dat->ny*k)]=v;
}
void MGL_EXPORT mgl_datac_set_value_(uintptr_t *d, dual *v, int *i, int *j, int *k)
{	mgl_datac_set_value(_DC_,*v,*i,*j,*k);	}
//-----------------------------------------------------------------------------
mdual MGL_EXPORT mgl_datac_get_value(HCDT dat, long i, long j, long k)
{
	register long nx=dat->GetNx(), ny=dat->GetNy(), i0=i+nx*(j+ny*k);
	if(i<0 || i>=nx || j<0 || j>=ny || k<0 || k>=dat->GetNz())
		return NAN;
	const mglDataC *d = dynamic_cast<const mglDataC*>(dat);
	dual r = d ? d->a[i0] : dual(dat->vthr(i0),0);
	return r.real()+r.imag()*mgl_I;
}
mdual MGL_EXPORT mgl_datac_get_value_(uintptr_t *d, int *i, int *j, int *k)
{	return mgl_datac_get_value(_DA_(d),*i,*j,*k);	}
//-----------------------------------------------------------------------------
MGL_EXPORT dual *mgl_datac_data(HADT dat)	{	return dat->a;	}
//-----------------------------------------------------------------------------
MGL_EXPORT dual *mgl_datac_value(HADT dat, long i,long j,long k)
{	register long ii=i*dat->nx*(j+dat->ny*k);
	return	ii>=0 && ii<dat->GetNN() ? dat->a+ii : 0;	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_datac_join(HADT d, HCDT v)
{
	long nx=d->nx, ny=d->ny, nz=d->nz, k=nx*ny*nz;
	const mglDataC *mv = dynamic_cast<const mglDataC *>(v);
	long vx=v->GetNx(), vy=v->GetNy(), vz=v->GetNz(), m = vx*vy*vz;

	if(nx==vx && ny==vy && ny>1)	d->nz += vz;
	else
	{
		ny *= nz;	vy *= vz;
		if(nx==vx && nx>1)
		{	d->nz = 1;	d->ny = ny+vy;	}
		else
		{	d->ny = d->nz = 1;	d->nx = k+m;	}
	}
	dual *b = new dual[k+m];
	memcpy(b,d->a,k*sizeof(dual));
	if(mv)	memcpy(b+k,mv->a,m*sizeof(dual));
	else
#pragma omp parallel for
		for(long i=0;i<m;i++)	b[k+i] = v->vthr(i);
	if(!d->link)	delete []d->a;
	d->a = b;	d->link=false;	d->NewId();
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_datac_join_(uintptr_t *d, uintptr_t *val)
{	mgl_datac_join(_DC_,_DA_(val));	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_datac_put_val(HADT d, dual val, long xx, long yy, long zz)
{
	long nx=d->nx, ny=d->ny, nz=d->nz;
	if(xx>=nx || yy>=ny || zz>=nz)	return;
	dual *a=d->a;
	if(xx>=0 && yy>=0 && zz>=0)	a[xx+nx*(yy+zz*ny)] = val;
	else if(xx<0 && yy<0 && zz<0)
#pragma omp parallel for
		for(long i=0;i<nx*ny*nz;i++)	a[i] = val;
	else if(xx<0 && yy<0)
#pragma omp parallel for
		for(long i=0;i<nx*ny;i++)	a[i+zz*nx*ny] = val;
	else if(yy<0 && zz<0)
#pragma omp parallel for
		for(long i=0;i<nz*ny;i++)	a[xx+i*nx] = val;
	else if(xx<0 && zz<0)
#pragma omp parallel for collapse(2)
		for(long i=0;i<nx;i++)	for(long j=0;j<nz;j++)	a[i+nx*(yy+j*ny)] = val;
	else if(xx<0)
#pragma omp parallel for
		for(long i=0;i<nx;i++)	a[i+nx*(yy+zz*ny)] = val;
	else if(yy<0)
#pragma omp parallel for
		for(long i=0;i<ny;i++)	a[xx+nx*(i+zz*ny)] = val;
	else //if(zz<0)
#pragma omp parallel for
		for(long i=0;i<nz;i++)	a[xx+nx*(yy+i*ny)] = val;
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_datac_put_dat(HADT d, HCDT v, long xx, long yy, long zz)
{
	long nx=d->nx, ny=d->ny, nz=d->nz;
	if(xx>=nx || yy>=ny || zz>=nz)	return;
	const mglDataC *mv = dynamic_cast<const mglDataC *>(v);
	dual *a=d->a, vv=v->v(0);
	const dual *b = mv?mv->a:0;
	long vx=v->GetNx(), vy=v->GetNy(), vz=v->GetNz();
	if(xx<0 && yy<0 && zz<0)	// whole array
	{
		if(vx>=nx && vy>=ny && vz>=nz)
#pragma omp parallel for
			for(long ii=0;ii<nx*ny*nz;ii++)
			{	register long i=ii%nx, j=(ii/nx)%ny, k=ii/(nx*ny);
				a[ii] = b?b[i+vx*(j+k*vy)]:v->v(i,j,k);	}
		else if(vx>=nx && vy>=ny)
#pragma omp parallel for
			for(long ii=0;ii<nx*ny*nz;ii++)
			{	register long i=ii%nx, j=(ii/nx)%ny;
				a[ii] = b?b[i+vx*j]:v->v(i,j);	}
		else if(vx>=nx)
#pragma omp parallel for
			for(long ii=0;ii<nx*ny*nz;ii++)
			{	register long i=ii%nx;	a[ii] = b?b[i]:v->v(i);	}
		else
#pragma omp parallel for
			for(long ii=0;ii<nx*ny*nz;ii++)	a[ii] = vv;
	}
	else if(xx<0 && yy<0)	// 2d
	{
		zz*=nx*ny;
		if(vx>=nx && vy>=ny)
#pragma omp parallel for
			for(long ii=0;ii<nx*ny;ii++)
			{	register long i=ii%nx, j=ii/nx;
				a[ii+zz] = b?b[i+vx*j]:v->v(i,j);	}
		else if(vx>=nx)
#pragma omp parallel for
			for(long ii=0;ii<nx*ny;ii++)
			{	register long i=ii%nx;	a[ii+zz] = b?b[i]:v->v(i);	}
		else
#pragma omp parallel for
			for(long ii=0;ii<nx*ny;ii++) 	a[ii+zz] = vv;
	}
	else if(yy<0 && zz<0)	// 2d
	{
		if(vx>=ny && vy>=nz)
#pragma omp parallel for
			for(long ii=0;ii<ny*nz;ii++)
			{	register long i=ii%ny, j=ii/ny;
				a[ii*nx+xx] = b?b[i+vx*j]:v->v(i,j);	}
		else if(vx>=ny)
#pragma omp parallel for
			for(long ii=0;ii<ny*nz;ii++)
			{	register long i=ii%ny;	a[ii*nx+xx] = b?b[i]:v->v(i);	}
		else
#pragma omp parallel for
			for(long ii=0;ii<ny*nz;ii++) 	a[ii*nx+xx] = vv;
	}
	else if(xx<0 && zz<0)	// 2d
	{
		yy *= nx;	zz = nx*ny;
		if(vx>=nx && vy>=nz)
#pragma omp parallel for
			for(long ii=0;ii<nx*nz;ii++)
			{	register long i=ii%nx, j=ii/nx;
				a[i+yy+j*zz] = b?b[i+vx*j]:v->v(i,j);	}
		else if(vx>=nx)
#pragma omp parallel for
			for(long ii=0;ii<nx*nz;ii++)
			{	register long i=ii%nx, j=ii/nx;
				a[i+yy+j*zz] = b?b[i]:v->v(i);	}
		else
#pragma omp parallel for
			for(long ii=0;ii<nx*nz;ii++)
			{	register long i=ii%nx, j=ii/nx;
				a[i+yy+j*zz] = vv;	}
	}
	else if(xx<0)
	{
		xx = nx*(yy+zz*ny);
		if(vx>=nx)
#pragma omp parallel for
			for(long i=0;i<nx;i++)	a[i+xx] = b?b[i]:v->v(i);
		else
#pragma omp parallel for
			for(long i=0;i<nx;i++)	a[i+xx] = vv;
	}
	else if(yy<0)
	{
		xx += zz*nx*ny;
		if(vx>=ny)
#pragma omp parallel for
			for(long i=0;i<ny;i++)	a[xx+nx*i] = b?b[i]:v->v(i);
		else
#pragma omp parallel for
			for(long i=0;i<ny;i++)	a[xx+nx*i] = vv;
	}
	else if(zz<0)
	{
		xx += nx*yy;	yy = nx*ny;
		if(vx>=nz)
#pragma omp parallel for
			for(long i=0;i<nz;i++)	a[xx+yy*i] = b?b[i]:v->v(i);
		else
#pragma omp parallel for
			for(long i=0;i<nz;i++)	a[xx+yy*i] = vv;
	}
	else	a[xx+nx*(yy+ny*zz)] = vv;
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_datac_put_val_(uintptr_t *d, dual *val, int *i, int *j, int *k)
{	mgl_datac_put_val(_DC_,*val, *i,*j,*k);	}
void MGL_EXPORT mgl_datac_put_dat_(uintptr_t *d, uintptr_t *val, int *i, int *j, int *k)
{	mgl_datac_put_dat(_DC_,_DA_(val), *i,*j,*k);	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_difr_grid(dual *a,int n,int step,dual q,int Border,dual *tmp,int kk);
void MGL_EXPORT mgl_difr_axial(dual *a,int n,int step,dual q,int Border,dual *tmp,int kk, double di);
//-----------------------------------------------------------------------------
MGL_NO_EXPORT void *mgl_difr(void *par)
{
#if !defined(_MSC_VER)	// MSVC produce internal compiler error on this code
	mglThreadC *t=(mglThreadC *)par;
	long n=t->p[0], st=t->p[1], bord=t->p[3], nn=t->n;
	dual *b=t->a, q = *(t->b);
#if !MGL_HAVE_PTHREAD
#pragma omp parallel
#endif
	{
		dual *tmp = new dual[2*n];
		if(t->p[2])
#if !MGL_HAVE_PTHREAD
#pragma omp for
#endif
			for(long i=t->id;i<nn;i+=mglNumThr)
				mgl_difr_axial(b + ((i%st)+n*(i/st)), n,st, q, bord,tmp,3,0);
		else
#if !MGL_HAVE_PTHREAD
#pragma omp for
#endif
			for(long i=t->id;i<nn;i+=mglNumThr)
				mgl_difr_grid(b + ((i%st)+n*(i/st)), n,st, q, bord,tmp,3);
		delete []tmp;
	}
#endif
	return 0;
}
void MGL_EXPORT mgl_datac_diffr(HADT d, const char *how, mreal q)
{
	if(!how || *how==0)	return;
	long nx=d->nx,ny=d->ny,nz=d->nz,ll=strlen(how);
	long p[4]={0,0,0,0};
	dual qq=q;
	for(long i=0;i<ll;i++)	if(how[i]>='0' && how[i]<='9')	p[3] = how[i]-'0';
	bool axial = mglchr(how,'r')||mglchr(how,'r');
	if(mglchr(how,'z') && nz>1)
	{
		p[0]=nz;	p[1]=nx*ny;	p[2]=0;
		mglStartThreadC(mgl_difr,0,nx*ny,0,&qq,0,p);
	}
	if(mglchr(how,'y') && ny>1 && !axial)
	{
		p[0]=ny;	p[1]=nx;	p[2]=0;
		mglStartThreadC(mgl_difr,0,nx*nz,0,&qq,0,p);
	}
	if(mglchr(how,'x') && nx>1 && !axial)
	{
		p[0]=nx;	p[1]=1;	p[2]=0;
		mglStartThreadC(mgl_difr,0,ny*nz,0,&qq,0,p);
	}
	if(axial && nx>1)
	{
		p[0]=nx;	p[1]=1;	p[2]=1;
		mglStartThreadC(mgl_difr,0,ny*nz,0,&qq,0,p);
	}
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_datac_diffr_(uintptr_t *d, const char *how, double q,int l)
{	char *s=new char[l+1];	memcpy(s,how,l);	s[l]=0;
	mgl_datac_diffr(_DC_,s,q);	delete []s;	}
//-----------------------------------------------------------------------------
HADT MGL_EXPORT mgl_gsplinec_init(HCDT x, HCDT v)
{
	long n = v->GetNx();
	if(!x || x->GetNx()!=n)	return 0;
	mglDataC *res = new mglDataC(5*(n-1));
	mreal *xx=0;
	dual *vv=0;
	const mglData *dx = dynamic_cast<const mglData *>(x);
	if(!dx)
	{
		xx = new mreal[n];
		for(long i=0;i<n;i++)	xx[i] = x->v(i);
	}
	const mglDataC *dv = dynamic_cast<const mglDataC *>(v);
	if(!dv)
	{
		vv = new dual[n];
		for(long i=0;i<n;i++)	vv[i] = v->v(i);
	}
	mgl_gspline_init(n,dx?dx->a:xx,dv?dv->a:vv,res->a);
	if(xx)	delete []xx;
	if(vv)	delete []vv;
	return res;
}
uintptr_t MGL_EXPORT mgl_gsplinec_init_(uintptr_t *x, uintptr_t *v)
{	return uintptr_t(mgl_gspline_init(_DA_(x),_DA_(v)));	}
//-----------------------------------------------------------------------------
mdual MGL_EXPORT mgl_gsplinec(HCDT c, mreal dx, dual *d1, dual *d2)
{
	long i=0, n = c->GetNx();
	if(n%5)	return NAN;	// not the table of coefficients
	while(dx>c->v(5*i) && i<n-1)	{	dx-=c->v(5*i);	i++;	}
	dual res;
	const mglDataC *d = dynamic_cast<const mglDataC *>(c);
	if(c)
	{
		const dual *a = d->a+5*i;
		if(d1)	*d1 = a[2]+dx*(mreal(2)*a[3]+(3*dx)*a[4]);
		if(d2)	*d2 = mreal(2)*a[3]+(6*dx)*a[4];
		res = a[1]+dx*(a[2]+dx*(a[3]+dx*a[4]));
	}
	else
	{
		if(d1)	*d1 = c->v(5*i+2)+dx*(2*c->v(5*i+3)+3*dx*c->v(5*i+4));
		if(d2)	*d2 = 2*c->v(5*i+3)+6*dx*c->v(5*i+4);
		res = c->v(5*i+1)+dx*(c->v(5*i+2)+dx*(c->v(5*i+3)+dx*c->v(5*i+4)));
	}
	return res.real()+res.imag()*mgl_I;
}
mdual MGL_EXPORT mgl_gsplinec_(uintptr_t *c, mreal *dx, dual *d1, dual *d2)
{	return mgl_gsplinec(_DA_(c),*dx,d1,d2);	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_datac_refill_gs(HADT dat, HCDT xdat, HCDT vdat, mreal x1, mreal x2, long sl)
{
	HADT coef = mgl_gsplinec_init(xdat, vdat);
	if(!coef)	return;	// incompatible dimensions
	const long nx = dat->nx, nn=dat->ny*dat->nz;
	mreal x0 = x1-xdat->v(0), dx = (x2-x1)/(nx-1);
#pragma omp parallel for
	for(long i=0;i<nx;i++)
	{
		dual d = mgl_gsplinec(coef,x0+dx*i,0,0);
		if(sl<0)	for(long j=0;j<nn;j++)	dat->a[i+j*nx] = d;
		else	dat->a[i+sl*nx] = d;
	}
	mgl_delete_datac(coef);
}
//-----------------------------------------------------------------------------
mreal MGL_NO_EXPORT mgl_index_1(mreal v, HCDT dat);
void MGL_EXPORT mgl_datac_refill_x(HADT dat, HCDT xdat, HCDT vdat, mreal x1, mreal x2, long sl)
{
	long nx=dat->nx,mx=vdat->GetNx(),nn=dat->ny*dat->nz;
	if(mx!=xdat->GetNx())	return;	// incompatible dimensions
	mreal dx = (x2-x1)/(nx-1);
#pragma omp parallel for
	for(long i=0;i<nx;i++)
	{
		register mreal u = mgl_index_1(x1+dx*i,xdat);
		dual d = mgl_datac_spline(vdat,u,0,0);
		if(sl<0)	for(long j=0;j<nn;j++)	dat->a[i+j*nx] = d;
		else	dat->a[i+sl*nx] = d;
	}
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_datac_refill_xy(HADT dat, HCDT xdat, HCDT ydat, HCDT vdat, mreal x1, mreal x2, mreal y1, mreal y2, long sl)
{
	long nx=dat->nx,ny=dat->ny,nz=dat->nz,mx=vdat->GetNx(),my=vdat->GetNy(),nn=nx*ny;
	bool both=(xdat->GetNN()==vdat->GetNN() && ydat->GetNN()==vdat->GetNN());
	if(!both && (xdat->GetNx()!=mx || ydat->GetNx()!=my))	return;	// incompatible dimensions
	mreal dx = (x2-x1)/(nx-1), dy = (y2-y1)/(ny-1);
	if(both)
	{
#pragma omp parallel for
		for(long i=0;i<nn*nz;i++)	dat->a[i]=NAN;
#pragma omp parallel for collapse(2)
		for(long j=0;j<my-1;j++)	for(long i=0;i<mx-1;i++)
		{
			long i0 = i+mx*j;
			mreal vx0 = (xdat->vthr(i0)-x1)/dx, vy0 = (ydat->vthr(i0)-y1)/dy;
			mreal vx1 = (xdat->vthr(i0+1)-x1)/dx, vy1 = (ydat->vthr(i0+1)-y1)/dy;
			mreal vx2 = (xdat->vthr(i0+mx)-x1)/dx, vy2 = (ydat->vthr(i0+mx)-y1)/dy;
			mreal vx3 = (xdat->vthr(i0+mx+1)-x1)/dx, vy3 = (ydat->vthr(i0+mx+1)-y1)/dy;
			long xx1 = long(mgl_min( mgl_min(vx0,vx1), mgl_min(vx2,vx3) ));	// bounding box
			long yy1 = long(mgl_min( mgl_min(vy0,vy1), mgl_min(vy2,vy3) ));
			long xx2 = long(mgl_max( mgl_max(vx0,vx1), mgl_max(vx2,vx3) ));
			long yy2 = long(mgl_max( mgl_max(vy0,vy1), mgl_max(vy2,vy3) ));
			xx1=mgl_max(xx1,0);	xx2=mgl_min(xx2,nx-1);
			yy1=mgl_max(yy1,0);	yy2=mgl_min(yy2,ny-1);
			if(xx1>xx2 || yy1>yy2)	continue;

			mreal d1x = vx1-vx0, d1y = vy1-vy0;
			mreal d2x = vx2-vx0, d2y = vy2-vy0;
			mreal d3x = vx3+vx0-vx1-vx2, d3y = vy3+vy0-vy1-vy2;
			mreal dd = d1x*d2y-d1y*d2x;
			mreal dsx =-4*(d2y*d3x - d2x*d3y)*d1y;
			mreal dsy = 4*(d2y*d3x - d2x*d3y)*d1x;

			for(long jj=yy1;jj<=yy2;jj++)	for(long ii=xx1;ii<=xx2;ii++)
			{
				mreal xx = (ii-vx0), yy = (jj-vy0);
				mreal s = dsx*xx + dsy*yy + (dd+d3y*xx-d3x*yy)*(dd+d3y*xx-d3x*yy);
				if(s>=0)
				{
					s = sqrt(s);
					mreal qu = d3x*yy - d3y*xx + dd + s;
					mreal qv = d3y*xx - d3x*yy + dd + s;
					mreal u = 2.f*(d2y*xx - d2x*yy)/qu;
					mreal v = 2.f*(d1x*yy - d1y*xx)/qv;
					if(u*(1.f-u)<0.f || v*(1.f-v)<0.f)	// first root bad
					{
						qu = d3x*yy - d3y*xx + dd - s;
						qv = d3y*xx - d3x*yy + dd - s;
						u = 2.f*(d2y*xx - d2x*yy)/qu;
						v = 2.f*(d1x*yy - d1y*xx)/qv;
						if(u*(1.f-u)<0.f || v*(1.f-v)<0.f)	continue;	// second root bad
					}
					i0 = ii+nx*jj;	s = vdat->value(i+u,j+v,0);
					if(sl<0)	for(long k=0;k<nz;k++)	dat->a[i0+k*nn] = s;
					else	dat->a[i0+sl*nn] = s;
				}
			}
		}
	}
	else
	{
		mglData u(nx), v(ny);
#pragma omp parallel for
		for(long i=0;i<nx;i++)	u.a[i] = mgl_index_1(x1+dx*i,xdat);
#pragma omp parallel for
		for(long i=0;i<ny;i++)	v.a[i] = mgl_index_1(y1+dy*i,ydat);
#pragma omp parallel for collapse(2)
		for(long j=0;j<ny;j++)	for(long i=0;i<nx;i++)
		{
			dual d = mgl_datac_spline(vdat,u.a[i],v.a[j],0);
			register long i0=i+nx*j;
			if(sl<0)	for(long k=0;k<nz;k++)	dat->a[i0+k*nn] = d;
			else	dat->a[i0+sl*nn] = d;
		}
	}
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_datac_refill_xyz(HADT dat, HCDT xdat, HCDT ydat, HCDT zdat, HCDT vdat, mreal x1, mreal x2, mreal y1, mreal y2, mreal z1, mreal z2)
{
	long nx=dat->nx,ny=dat->ny,nz=dat->nz,mx=vdat->GetNx(),my=vdat->GetNy(),mz=vdat->GetNz();
	bool both=(xdat->GetNN()==vdat->GetNN() && ydat->GetNN()==vdat->GetNN() && zdat->GetNN()==vdat->GetNN());
	if(!both && (xdat->GetNx()!=mx || ydat->GetNx()!=my || zdat->GetNx()!=mz))	return;	// incompatible dimensions
	const mreal acx=1e-6*fabs(x2-x1), acy=1e-6*fabs(y2-y1), acz=1e-6*fabs(z2-z1);
	if(both)
#pragma omp parallel for collapse(3)
		for(long k=0;k<nz;k++)	for(long j=0;j<ny;j++)	for(long i=0;i<nx;i++)
		{
			mreal xx = x1+(x2-x1)*i/(nx-1.),dxx,dxy,dxz,vx,dx=0,dd;
			mreal yy = y1+(y2-y1)*j/(ny-1.),dyx,dyy,dyz,vy,dy=0;
			mreal zz = z1+(z2-z1)*k/(nz-1.),dzx,dzy,dzz,vz,dz=0;
			vx = xdat->valueD(dx,dy,dz,&dxx,&dxy,&dxz);
			vy = ydat->valueD(dx,dy,dz,&dyx,&dyy,&dyz);
			vz = zdat->valueD(dx,dy,dz,&dzx,&dzy,&dzz);
			long count=0;
			do	// use Newton method to find root
			{
				if(count>50)	{	dx=NAN;	break;	}	count++;
				dd = -dxx*dyy*dzz+dxy*dyx*dzz+dxx*dyz*dzy-dxz*dyx*dzy-dxy*dyz*dzx+dxz*dyy*dzx;
				dx += ((dyz*dzy-dyy*dzz)*(xx-vx)+(dxy*dzz-dxz*dzy)*(yy-vy)+(dxz*dyy-dxy*dyz)*(zz-vz))/dd;
				dy += ((dyx*dzz-dyz*dzx)*(xx-vx)+(dxz*dzx-dxx*dzz)*(yy-vy)+(dxx*dyz-dxz*dyx)*(zz-vz))/dd;
				dz += ((dyy*dzx-dyx*dzy)*(xx-vx)+(dxx*dzy-dxy*dzx)*(yy-vy)+(dxy*dyx-dxx*dyy)*(zz-vz))/dd;
				vx = xdat->valueD(dx,dy,dz,&dxx,&dxy,&dxz);
				vy = ydat->valueD(dx,dy,dz,&dyx,&dyy,&dyz);
				vz = zdat->valueD(dx,dy,dz,&dzx,&dzy,&dzz);
			}	while(fabs(xx-vx)>acx && fabs(yy-vy)>acy && fabs(zz-vz)>acz);	// this valid for linear interpolation
			dat->a[i+nx*(j+ny*k)] = mgl_isnan(dx)?NAN:vdat->value(dx,dy,dz);
		}
	else
	{
		mglData u(nx), v(ny), w(nz);
		mreal dx = (x2-x1)/(nx-1), dy = (y2-y1)/(ny-1), dz = (z2-z1)/(nz-1);
#pragma omp parallel for
		for(long i=0;i<nx;i++)	u.a[i] = mgl_index_1(x1+dx*i,xdat);
#pragma omp parallel for
		for(long i=0;i<ny;i++)	v.a[i] = mgl_index_1(y1+dy*i,ydat);
#pragma omp parallel for
		for(long i=0;i<nz;i++)	w.a[i] = mgl_index_1(z1+dz*i,zdat);
#pragma omp parallel for collapse(3)
		for(long k=0;k<nz;k++)	for(long j=0;j<ny;j++)	for(long i=0;i<nx;i++)
			dat->a[i+nx*(j+ny*k)] = mgl_datac_spline(vdat,u.a[i],v.a[j],w.a[k]);
	}
}
//-----------------------------------------------------------------------------
