/***************************************************************************
 * pixel.cpp is part of Math Graphic Library
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
#include <algorithm>
#include "mgl2/canvas.h"
#include "mgl2/thread.h"
#if MGL_HAVE_OMP
#include <omp.h>
#endif

inline mreal get_persp(float pf, float z, float Depth)
//{	return (1-pf)/(1-pf*z/Depth);	}
{	return (1-pf/1.37)/(1-pf*z/Depth);	}
inline mreal get_pfact(float pf, float Depth)
//{	return pf/(1-pf)/Depth;	}
{	return pf/(1-pf/1.37)/Depth;	}
//-----------------------------------------------------------------------------
void mglCanvas::SetSize(int w,int h,bool clf)
{
	if(w<=0 || h<=0)	{	SetWarn(mglWarnSize,"SetSize");	return;	}
	if(Width==w && Height==h)
	{
		InPlot(0,1,0,1,false);
		if(clf || (Quality&4))	Clf();
		return;
	}

	const double dx = double(w)/Width;
	const double dy = double(h)/Height;
	const double dz = sqrt(double(w*h))/Depth;
	Width = w;	Height = h;	Depth = long(sqrt(double(w*h)));
	const long s = long(w)*long(h);
#if MGL_HAVE_PTHREAD
	pthread_mutex_lock(&mutexClf);
#elif MGL_HAVE_OMP
	omp_set_lock((omp_lock_t*)lockClf);
#endif
	if(G)	{	delete []G;	delete []C;	delete []Z;	delete []G4;delete []GB;delete []OI;	G=0;	}
	G = new unsigned char[s*3];
	G4= new unsigned char[s*4];
	GB= new unsigned char[s*4];
	C = new unsigned char[s*12];
	Z = new float[s*3];	// only 3 planes
	OI= new int[s];
#pragma omp parallel for
	for(long i=0;i<s;i++)	memcpy(GB+4*i,BDef,4);
#if MGL_HAVE_PTHREAD
	pthread_mutex_unlock(&mutexClf);
#elif MGL_HAVE_OMP
	omp_unset_lock((omp_lock_t*)lockClf);
#endif

	InPlot(0,1,0,1,false);
	if(clf || (Quality&4))	Clf();
	else	// No clearing. So, need to scale
	{
#if MGL_HAVE_PTHREAD
		pthread_mutex_lock(&mutexPnt);
		pthread_mutex_lock(&mutexClf);
#elif MGL_HAVE_OMP
		omp_set_lock((omp_lock_t*)lockClf);
#endif
		const long m = long(Prm.size());
		double dd = dx>dy?dy:dx;
#pragma omp parallel for	// Scale text
		for(long i=0;i<m;i++)	if(Prm[i].type==4)
		{
			mglPnt &q = Pnt[Prm[i].n1];
			Prm[i].p *=dd;
			q.u *= dd;	q.v *= dd;
		}
		const long n = long(Pnt.size());
#pragma omp parallel for	// Scale coordinates
		for(long i=0;i<n;i++)
		{
			mglPnt &q = Pnt[i];
			q.x*=dx;	q.y*=dy;	q.z*=dz;
			q.xx*=dx;	q.yy*=dy;	q.zz*=dz;
			if(mgl_isnum(q.w))
			{	q.u*=dx;	q.v*=dy;	q.w*=dz;	}
		}
		for(size_t i=0;i<Sub.size();i++)
		{	mglBlock &q = Sub[i];	q.n1*=dx;	q.n2*=dx;	q.n3*=dy;	q.n4*=dy;	}
		for(size_t k=0;k<DrwDat.size();k++)	// scale frames too
		{
			mglStack<mglPnt>  &pnt = DrwDat[k].Pnt;
			const long n = long(pnt.size());
#pragma omp parallel for
			for(long i=0;i<n;i++)
			{
				mglPnt &q = pnt[i];
				q.x*=dx;	q.y*=dy;	q.z*=dz;
				q.xx*=dx;	q.yy*=dy;	q.zz*=dz;
				if(mgl_isnum(q.w))
				{	q.u*=dx;	q.v*=dy;	q.w*=dz;	}
			}
			std::vector<mglBlock>  &sub = DrwDat[k].Sub;
			for(size_t i=0;i<sub.size();i++)
			{	mglBlock &q = sub[i];	q.n1*=dx;	q.n2*=dx;	q.n3*=dy;	q.n4*=dy;	}
		}
#if MGL_HAVE_PTHREAD
		pthread_mutex_unlock(&mutexClf);
		pthread_mutex_unlock(&mutexPnt);
#elif MGL_HAVE_OMP
		omp_unset_lock((omp_lock_t*)lockClf);
#endif
		ClfZB();	Finish();
	}
}
//-----------------------------------------------------------------------------
void mglDrawReg::set(mglCanvas *gr, int nx, int ny, int m)
{
	int mx = m%nx, my = m/nx;	ObjId = gr->ObjId;
	PDef = gr->mask;	angle = gr->MaskAn;
	x1 = gr->GetWidth()*mx/nx;		y1 = gr->GetHeight()-gr->GetHeight()*(my+1)/ny;
	x2 = gr->GetWidth()*(mx+1)/nx-1;	y2 = gr->GetHeight()-gr->GetHeight()*my/ny-1;
}
//-----------------------------------------------------------------------------
void mglCanvas::PutDrawReg(mglDrawReg *d, const mglCanvas *gr)
{
	if(gr)
	{
		const int dd = d->x2 - d->x1;
		for(long j=d->y1;j<d->y2;j++)
		{
			register long i = d->x1+Width*(Height-1-j);
			memcpy(OI+i,gr->OI+i,dd*sizeof(int));
			memcpy(Z+3*i,gr->Z+3*i,3*dd*sizeof(float));
			memcpy(C+12*i,gr->C+12*i,12*dd);
		}
	}
}
//-----------------------------------------------------------------------------
void mglCanvas::PostScale(const mglMatrix *M, mglPoint &p) const
{
	register mreal f = 1./(2*M->pf),x=p.x,y=p.y,z=p.z;
	const mreal *b=M->b;
	p.x = M->x + f*(x*b[0] + y*b[1] + z*b[2]);
	p.y = M->y + f*(x*b[3] + y*b[4] + z*b[5]);
	p.z = M->z + f*(x*b[6] + y*b[7] + z*b[8]);
}
//-----------------------------------------------------------------------------
bool mglCanvas::ScalePoint(const mglMatrix *M, mglPoint &p, mglPoint &n, bool use_nan) const
{
	bool res = get(MGL_DISABLE_SCALE) || mglBase::ScalePoint(M,p,n,use_nan);
	PostScale(M,p);

	register mreal nx=n.x, ny=n.y, nz=n.z;
	const mreal *b=M->b;
	n.x = nx*b[0] + ny*b[1] + nz*b[2];	// simpler for rotation only
	n.y = nx*b[3] + ny*b[4] + nz*b[5];
	n.z = nx*b[6] + ny*b[7] + nz*b[8];
	n.Normalize();
	return res;
}
//-----------------------------------------------------------------------------
long mglCanvas::ProjScale(int nf, long id, bool text)
{
	const mglPnt &pi = Pnt[id];
	mglPoint pp(pi.x,pi.y,pi.z), nn(pi.u,pi.v,pi.w), p, n;
	if(mgl_isnan(pp.x))	return -1;
	const mreal w=B1.b[0]/2, h=B1.b[4]/2, d=B1.b[8]/2, xx=B1.x-w/2, yy=B1.y-h/2;
	if(pi.sub>=0)
	{
		mglPoint q(RestorePnt(pp)/(2*B.pf));
		mglPoint u(RestorePnt(nn,true));	u.Normalize();
		if(nf==0)
		{	p.x = xx + q.x*w;	p.y = yy + q.y*h;	p.z = B1.z + q.z*d;	n = u;	}
		else if(nf==1)
		{	p.x = xx + q.x*w;	p.y = yy+h + q.z*h;	p.z = B1.z - q.y*d;	n.Set(u.x,u.z,-u.y);	}
		else if(nf==2)
		{	p.x = xx+w + q.z*w;	p.y = yy + q.y*h;	p.z = B1.z - q.x*d;	n.Set(u.z,u.y,-u.x);	}
		else
		{
			const mreal *b=B.b;	n = nn;
			p.x = xx+w + q.x*b[0]/2 + q.y*b[1]/2 + q.z*b[2]/2;
			p.y = yy+h + q.x*b[3]/2 + q.y*b[4]/2 + q.z*b[5]/2;
			p.z = B.z + q.x*b[6]/2 + q.y*b[7]/2 + q.z*b[8]/2;
		}
	}
	else
	{
		p.x = pi.x/2 + w*(nf/2);
		p.y = pi.y/2 + h*(nf%2);
		p.z = pi.z;	n=nn;
	}
	return CopyProj(id,p,text?n:nn,pi.sub);
}
//-----------------------------------------------------------------------------
void mglCanvas::LightScale(const mglMatrix *M, mglLight &ls)
{
	ls.p=ls.d;	ls.q=ls.r;
	ScalePoint(M,ls.q,ls.p,false);
	ls.p /= ls.p.norm();
}
//-----------------------------------------------------------------------------
void mglCanvas::LightScale(const mglMatrix *M)
{
	for(long i=0;i<10;i++)	if(light[i].n)	LightScale(M,light[i]);
	for(size_t j=0;j<Sub.size();j++)
		for(long i=0;i<10;i++)	if(light[i].n)	LightScale(&(Sub[j].B),Sub[j].light[i]);
}
//-----------------------------------------------------------------------------
// NOTE: Perspective is not fully supported now !!! Also it use LAST InPlot parameters!!!
mglPoint mglCanvas::RestorePnt(mglPoint ps, bool norm) const
{
	const mreal s3 = 2*B.pf;
	mglPoint p;

	const mreal W=Width/2, H=Height/2, D=Depth/2;
	const mreal *b=B.b,*d=Bp.b;
	mreal cx = B.z*d[2]+B.y*d[1]+B.x*d[0]-Bp.x*W-d[0]*W+W-d[1]*H-d[2]*D;
	const mreal c0 = b[6]*d[2]+b[3]*d[1]+b[0]*d[0];
	const mreal c1 = b[7]*d[2]+b[4]*d[1]+b[1]*d[0];
	const mreal c2 = b[8]*d[2]+b[5]*d[1]+b[2]*d[0];
	mreal cy = B.z*d[5]+B.y*d[4]+B.x*d[3]-d[3]*W-Bp.y*H-d[4]*H+H-d[5]*D;
	const mreal c3 = b[6]*d[5]+b[3]*d[4]+b[0]*d[3];
	const mreal c4 = b[7]*d[5]+b[4]*d[4]+b[1]*d[3];
	const mreal c5 = b[8]*d[5]+b[5]*d[4]+b[2]*d[3];
	mreal cz = B.z*d[8]+B.y*d[7]+B.x*d[6]-d[6]*W-d[7]*H-Bp.z*D-d[8]*D+D;
	const mreal c6 = b[6]*d[8]+b[3]*d[7]+b[0]*d[6];
	const mreal c7 = b[7]*d[8]+b[4]*d[7]+b[1]*d[6];
	const mreal c8 = b[8]*d[8]+b[5]*d[7]+b[2]*d[6];
	if(norm)	cx=cy=cz=0;

	if(mgl_isnum(ps.z))	// try to take into account perspective if z-value is provided
	{
		register float dd = get_persp(Bp.pf,ps.z,Depth);
		ps.x = Width/2 + (ps.x-Width/2)/dd;
		ps.y = Height/2+ (ps.y-Height/2)/dd;
	}
	const mreal xx = ps.x-cx, yy = ps.y-cy, zz = ps.z-cz;
	const mreal d1=c0*c4-c1*c3, d2=c1*c5-c2*c4, d3=c0*c5-c2*c3;

	if(mgl_isnum(zz))	// try to use z-values
	{
		// put inverse matrix here: [x,y,z]=B^(-1)[xx,yy,zz]
		mreal det = (-c0*c4*c8+c1*c3*c8+c0*c5*c7-c2*c3*c7-c1*c5*c6+c2*c4*c6)/s3;
		p.x = (c2*c4-c1*c5)*zz+(c1*c8-c2*c7)*yy+(c5*c7-c4*c8)*xx;	p.x /= det;
		p.y = (c0*c5-c2*c3)*zz+(c2*c6-c0*c8)*yy+(c3*c8-c5*c6)*xx;	p.y /= det;
		p.z = (c1*c3-c0*c4)*zz+(c0*c7-c1*c6)*yy+(c4*c6-c3*c7)*xx;	p.z /= det;
	}
	else if(fabs(d1) > fabs(d2) && fabs(d1) > fabs(d3))	// x-y plane
	{
		p.z = 0;
		p.x = s3*(c4*xx-c1*yy)/d1;
		p.y = s3*(c0*yy-c3*xx)/d1;
	}
	else if(fabs(d2) > fabs(d3))	// y-z
	{
		p.x = 0;
		p.y = s3*(c5*xx-c2*yy)/d2;
		p.z = s3*(c1*yy-c4*xx)/d2;
	}
	else	// x-z
	{
		p.y = 0;
		p.x = s3*(c5*xx-c2*yy)/d3;
		p.z = s3*(c0*yy-c3*xx)/d3;
	}
	return p;
}
//-----------------------------------------------------------------------------
mglPoint mglCanvas::CalcXYZ(int xs, int ys, bool real) const
{
	if(xs<0 || ys<0 || xs>=Width || ys>=Height)	return mglPoint(NAN,NAN,NAN);
	mglPoint p, ps(xs,Height-ys,NAN);
	float zz = Z[3*(xs+Width*(Height-1-ys))];
	if(zz>-1e20f)	{	ps.z = zz;	real=false;	}
	p = RestorePnt(ps);
	return real ? mglPoint(NAN,NAN,NAN) : mglPoint(Min.x + (Max.x-Min.x)*(p.x+1)/2,
				Min.y + (Max.y-Min.y)*(p.y+1)/2, Min.z + (Max.z-Min.z)*(p.z+1)/2);
}
//-----------------------------------------------------------------------------
void mglCanvas::CalcScr(mglPoint p, int *xs, int *ys) const
{
	mglPoint n;
	ScalePoint(GetB(),p,n);
	if(xs)	*xs=int(p.x);
	if(ys)	*ys=int(p.y);
}
//-----------------------------------------------------------------------------
mglPoint mglCanvas::CalcScr(mglPoint p) const
{	int x,y;	CalcScr(p,&x,&y);	return mglPoint(x,y);	}
//-----------------------------------------------------------------------------
void MGL_NO_EXPORT mgl_prm_swap(mglPrim &s1,mglPrim &s2,mglPrim *buf)
{
	memcpy(buf, &s1, sizeof(mglPrim));
	memcpy(&s1, &s2, sizeof(mglPrim));
	memcpy(&s2, buf, sizeof(mglPrim));
}
void MGL_NO_EXPORT sort_prm_c(const size_t l0, const size_t r0, mglStack<mglPrim> &s, mglPrim *buf)
{
	if(l0==r0)	return;
	if(l0+1==r0)
	{
		if(s[r0].n1<s[l0].n1)	mgl_prm_swap(s[r0],s[l0],buf);
		return;
	}
	bool del= (buf==0);
	if(del)	buf = (mglPrim*)malloc(sizeof(mglPrim));

	size_t l=l0, r=r0;
	const long v = s[(l+r)/2].n1;

	for(size_t i=l0;i<=r0;i++)	// first collect <0
		if(s[i].n1<v)
		{
			if(i>l)	mgl_prm_swap(s[i],s[l],buf);
			l++;
		}
	r=l;
	for(size_t i=l;i<=r0;i++)	// now collect =0
		if(s[i].n1==v)
		{
			if(i>r)	mgl_prm_swap(s[i],s[r],buf);
			r++;
		}

	if(l>l0+1)	sort_prm_c(l0,l-1,s,buf);
	if(r<r0)	sort_prm_c(r,r0,s,buf);
	if(del)	free(buf);
}
//-----------------------------------------------------------------------------
MGL_NO_EXPORT int mgl_type_prior[8]={1,2,4,5, 0,3,0, 7};
MGL_NO_EXPORT void *mgl_canvas_thr(void *par)
{	mglThreadG *t=(mglThreadG *)par;	(t->gr->*(t->f))(t->id, t->n, t->p);	return NULL;	}
void mglStartThread(void (mglCanvas::*func)(long i, long n, const void *p), mglCanvas *gr, long n, const void *p=NULL)
{
	if(!func || !gr)	return;
#if MGL_HAVE_PTHREAD
	if(mglNumThr<1)	mgl_set_num_thr(0);
	if(mglNumThr>1)
	{
		pthread_t *tmp=new pthread_t[mglNumThr];
		mglThreadG *par=new mglThreadG[mglNumThr];
		for(long i=0;i<mglNumThr;i++)	// put parameters into the structure
		{	par[i].gr=gr;	par[i].f=func;	par[i].n=n;	par[i].p=p;	par[i].id=i;	}
		for(long i=0;i<mglNumThr;i++)	pthread_create(tmp+i, 0, mgl_canvas_thr, par+i);
		for(long i=0;i<mglNumThr;i++)	pthread_join(tmp[i], 0);
		delete []tmp;	delete []par;
	}
	else
#endif
	{	mglNumThr = 1;	(gr->*func)(0,n,p);	}
}
//-----------------------------------------------------------------------------
void mglCanvas::pxl_combine(long id, long n, const void *)
{
#if !MGL_HAVE_PTHREAD
#pragma omp parallel for
#endif
	for(long i=id;i<n;i+=mglNumThr)
	{
		unsigned char *cc = C+12*i, c[4];
		memcpy(c,GB+4*i,4);	// memcpy(c,BDef,4);
		combine(c,cc+8);	combine(c,cc+4);
		combine(c,cc);		memcpy(G4+4*i,c,4);
	}
}
//-----------------------------------------------------------------------------
void mglCanvas::pxl_memcpy(long id, long n, const void *)
{
#if !MGL_HAVE_PTHREAD
#pragma omp parallel for
#endif
	for(long i=id;i<n;i+=mglNumThr)	memcpy(G4+4*i,C+12*i,4);
}
//-----------------------------------------------------------------------------
void mglCanvas::pxl_backgr(long id, long n, const void *)
{
#if !MGL_HAVE_PTHREAD
#pragma omp parallel for
#endif
	for(long i=id;i<n;i+=mglNumThr)
	{	unsigned char c[4];	memcpy(c,GB+4*i,4);	combine(c,G4+4*i);	memcpy(G+3*i,c,3);	}
}
//-----------------------------------------------------------------------------
void mglCanvas::pxl_transform(long id, long n, const void *)
{
	const mreal *b = Bp.b;
	const mreal dx = -Bp.x*Width/2, dy = -Bp.y*Height/2, dz = Depth/2.;
#if !MGL_HAVE_PTHREAD
#pragma omp parallel for
#endif
	for(long i=id;i<n;i+=mglNumThr)
	{
		mglPnt &p=Pnt[i];
		if(p.sub>=0)
		{
			register float x = p.xx-Width/2., y = p.yy-Height/2., z = p.zz-Depth/2.;
			p.x = b[0]*x + b[1]*y + b[2]*z + dx;
			p.y = b[3]*x + b[4]*y + b[5]*z + dy;
			p.z = b[6]*x + b[7]*y + b[8]*z + dz;
			register float d = get_persp(Bp.pf,p.z,Depth);
			p.x = Width/2. + d*p.x;	p.y = Height/2. + d*p.y;
		}
	}
}
//-----------------------------------------------------------------------------
void mglCanvas::pxl_setz_adv(long id, long n, const void *)
{
#if !MGL_HAVE_PTHREAD
#pragma omp parallel for
#endif
	for(long i=id;i<n;i+=mglNumThr)
	{
		mglPrim &q=Prm[i];	q.z = Pnt[q.n1].z;
		if(q.type==3)	q.z = (q.z + Pnt[q.n2].z + Pnt[q.n3].z + Pnt[q.n4].z)/4;
		else if(q.type==2)	q.z = (q.z + Pnt[q.n2].z + Pnt[q.n3].z)/3;
		else if(q.type==1)	q.z = (q.z + Pnt[q.n2].z)/2;
	}
}
//-----------------------------------------------------------------------------
uint32_t mglCanvas::GetColor(const mglPrim &p) const
{
	mglRGBA res, c1,c2,c3,c4;
	c1.c=pnt_col[p.type==1?p.n2:p.n1];
	unsigned r=c1.r[0], g=c1.r[1], b=c1.r[2], a=c1.r[3];
	switch(p.type)
	{
	case 3:
		c2.c=pnt_col[p.n2];	c3.c=pnt_col[p.n3];	c4.c=pnt_col[p.n4];
		res.r[0]=(r+c2.r[0]+c3.r[0]+c4.r[0])/4;
		res.r[1]=(g+c2.r[1]+c3.r[1]+c4.r[1])/4;
		res.r[2]=(b+c2.r[2]+c3.r[2]+c4.r[2])/4;
		res.r[3]=(a+c2.r[3]+c3.r[3]+c4.r[3])/4;	break;
	case 2:
		c2.c=pnt_col[p.n2];	c3.c=pnt_col[p.n3];
		res.r[0]=(r+c2.r[0]+c3.r[0])/3;
		res.r[1]=(g+c2.r[1]+c3.r[1])/3;
		res.r[2]=(b+c2.r[2]+c3.r[2])/3;
		res.r[3]=(a+c2.r[3]+c3.r[3])/3;	break;
	case 6:
		res.r[0]=p.n2&0xff;	res.r[1]=(p.n2/256)&0xff;	res.r[2]=(p.n2/65536)&0xff;	res.r[3]=255;	break;
//		res.c=p.n2;	break;
	default:
		res.c = c1.c;	break;
	}
	// add fog into resulting color
	float zf = FogDist*(p.z/Depth-0.5-FogDz);
	if(zf<0)	// add fog
	{
		unsigned char d = (unsigned char)(255*(1.-exp(5*zf)));
		unsigned char cb[4] = {BDef[0], BDef[1], BDef[2], d};
		if(d<255)	combine(res.r,cb);
	}
	return res.c;
}
//-----------------------------------------------------------------------------
void mglCanvas::pxl_pntcol(long id, long n, const void *)
{
#if !MGL_HAVE_PTHREAD
#pragma omp parallel for
#endif
	for(long i=id;i<n;i+=mglNumThr)
	{	mglRGBA c;	col2int(Pnt[i],c.r,-1);	pnt_col[i]=c.c;	}
}
//-----------------------------------------------------------------------------
void mglCanvas::pxl_setz(long id, long n, const void *)
{
#if !MGL_HAVE_PTHREAD
#pragma omp parallel for
#endif
	for(long i=id;i<n;i+=mglNumThr)
	{	mglPrim &q=Prm[i];	q.z = Pnt[q.n1].z;	}
}
//-----------------------------------------------------------------------------
HMGL mgl_qsort_gr=0;
int mglBase::PrmCmp(long i, long j) const
{
	const mglPrim &a = Prm[i];
	const mglPrim &b = Prm[j];
	if(a.z!=b.z) 	return int(100*(a.z - b.z));
	register int t1 = mgl_type_prior[a.type], t2 = mgl_type_prior[b.type];
	if(t1!=t2)		return t2 - t1;
	if(a.w!=b.w) 	return int(100*(b.w - a.w));
	return a.n3 - b.n3;
}
int MGL_LOCAL_PURE mgl_prm_cmp(const void *i,const void *j)
{
	return mgl_qsort_gr->PrmCmp(*(const long *)i, *(const long *)j);
}
//-----------------------------------------------------------------------------
void mglCanvas::PreparePrim(int fast)
{
	if(fast!=2)
	{
		mglStartThread(&mglCanvas::pxl_transform,this,Pnt.size());
		if(fast==0)	mglStartThread(&mglCanvas::pxl_setz,this,Prm.size());
		else	mglStartThread(&mglCanvas::pxl_setz_adv,this,Prm.size());
#pragma omp critical
		{
			ClearPrmInd();	mgl_qsort_gr = this;
			register size_t n = Prm.size();
			PrmInd = new long[n];
			for(size_t i=0;i<n;i++)	PrmInd[i]=i;
			qsort(PrmInd,n,sizeof(long),mgl_prm_cmp);
			clr(MGL_FINISHED);
		}
	}
	if(fast>0)
	{
#pragma omp critical
		{	if(pnt_col)	delete []pnt_col;	pnt_col = new uint32_t[Pnt.size()];	}
		mglStartThread(&mglCanvas::pxl_pntcol,this,Pnt.size());
	}
}
//-----------------------------------------------------------------------------
void mglCanvas::pxl_primdr(long id, long , const void *)
{
#define Q	4	// should be >= sqrt(2*num_thr) ???
	const int nx=Q,ny=Q;	// TODO find dependence on Q for 1, 2, 4, 8 threads. Try to select optimal
	if(!(Quality&3))
#if !MGL_HAVE_PTHREAD
#pragma omp parallel for
#endif
		for(long i=id;i<nx*ny;i+=mglNumThr)
		{
			mglDrawReg d;	d.set(this,nx,ny,i);
			for(size_t k=0;k<Prm.size();k++)
			{
				if(Stop)	break;
				const mglPrim &p=GetPrm(k);
				d.PDef = p.n3;	d.pPos = p.s;
				d.ObjId = p.id;	d.PenWidth=p.w;
				d.angle = p.angl;
				if(p.type==2 || p.type==3) d.PDef = p.m;
				register long n1=p.n1, n2=p.n2, n3=p.n3, n4=p.n4;
				switch(p.type)
				{
				case 3:	fast_draw(Pnt[n1],Pnt[n4],&d);	fast_draw(Pnt[n2],Pnt[n3],&d);	break;
				case 1:	fast_draw(Pnt[n1],Pnt[n2],&d);	break;
				case 4:	glyph_draw(p,&d);	break;
				case 0:	mark_draw(Pnt[n1],n4,p.s,&d);	break;
				case 2:	fast_draw(Pnt[n1],Pnt[n2],&d);	fast_draw(Pnt[n1],Pnt[n3],&d);
						fast_draw(Pnt[n2],Pnt[n3],&d);	break;
				}
			}
		}
	else if(!(Quality&MGL_DRAW_NORM))
#if !MGL_HAVE_PTHREAD
#pragma omp parallel for
#endif
		for(long i=id;i<nx*ny;i+=mglNumThr)
		{
			mglDrawReg d;	d.set(this,nx,ny,i);
			for(size_t k=0;k<Prm.size();k++)
			{
				if(Stop)	break;
				const mglPrim &p=GetPrm(k);
				d.PDef = p.n3;	d.pPos = p.s;
				d.ObjId = p.id;	d.PenWidth=p.w;
				d.angle = p.angl;
				if(p.type==2 || p.type==3) d.PDef = p.m;
				register long n1=p.n1, n2=p.n2, n3=p.n3, n4=p.n4;
				switch(p.type)
				{
				case 3:	trig_draw(Pnt[n1],Pnt[n2],Pnt[n4],true,&d);
						trig_draw(Pnt[n1],Pnt[n3],Pnt[n4],true,&d);	break;
				case 1:	line_draw(Pnt[n1],Pnt[n2],&d);	break;
				case 4:	glyph_draw(p,&d);	break;
				case 0:	mark_draw(Pnt[n1],n4,p.s,&d);	break;
				case 2:	trig_draw(Pnt[n1],Pnt[n2],Pnt[n3],true,&d);	break;
				}
			}
		}
	else
#if !MGL_HAVE_PTHREAD
#pragma omp parallel for
#endif
		for(long i=id;i<nx*ny;i+=mglNumThr)
		{
			mglDrawReg d;	d.set(this,nx,ny,i);
			for(size_t k=0;k<Prm.size();k++)
			{
				if(Stop)	break;
				const mglPrim &p=GetPrm(k);
				d.PDef = p.n3;	d.pPos = p.s;
				d.ObjId = p.id;	d.PenWidth=p.w;
				d.angle = p.angl;
				if(p.type==2 || p.type==3) d.PDef = p.m;
				register long n1=p.n1, n2=p.n2, n3=p.n3, n4=p.n4;
				switch(p.type)
				{
				case 3:	quad_draw(Pnt[n1],Pnt[n2],Pnt[n3],Pnt[n4],&d);	break;
				case 1:	line_draw(Pnt[n1],Pnt[n2],&d);	break;
				case 4:	glyph_draw(p,&d);	break;
				case 0:	mark_draw(Pnt[n1],n4,p.s,&d);	break;
				case 2:	trig_draw(Pnt[n1],Pnt[n2],Pnt[n3],true,&d);	break;
				}
			}
		}
}
//-----------------------------------------------------------------------------
void mglCanvas::pxl_primpx(long id, long n, const void *)	// NOTE this variant is too slow ... may be later in CUDA???
{
	mglDrawReg d;	d.set(this,1,1,id);
#if !MGL_HAVE_PTHREAD
#pragma omp parallel for firstprivate(d)
#endif
	for(long ii=0;ii<n;ii+=mglNumThr)
	{
		register long i=ii%Width, j=ii/Width;
		for(size_t k=0;k<Prm.size();k++)
		{
			if(Stop)	break;
			const mglPrim &p=GetPrm(k);
			d.PDef = p.n3;	d.pPos = p.s;
			d.ObjId = p.id;	d.PenWidth=p.w;
			d.angle = p.angl;
			if(p.type==2 || p.type==3) d.PDef = p.m;
			switch(p.type)
			{
			case 0:	mark_pix(i,j,Pnt[p.n1],p.n4,p.s,&d);	break;
			case 1:	line_pix(i,j,Pnt[p.n1],Pnt[p.n2],&d);	break;
			case 2:	trig_pix(i,j,Pnt[p.n1],Pnt[p.n2],Pnt[p.n3],true,&d);	break;
			case 3:	quad_pix(i,j,Pnt[p.n1],Pnt[p.n2],Pnt[p.n3],Pnt[p.n4],&d);	break;
			case 4:	glyph_pix(i,j,p,&d);	break;
			}
		}
	}
}
//-----------------------------------------------------------------------------
void mglCanvas::pxl_dotsdr(long id, long n, const void *)
{
	const mreal *b = Bp.b;
	const mreal dx = -Bp.x*Width/2, dy = -Bp.y*Height/2, dz = Depth/2.;
#if !MGL_HAVE_PTHREAD
#pragma omp parallel for
#endif
	for(long i=id;i<n;i+=mglNumThr)
	{
		unsigned char r[4]={0,0,0,255};
		const mglPnt &p=Pnt[i];
		if(p.sub<0)	continue;
		register float x = p.xx-Width/2., y = p.yy-Height/2., z = p.zz-Depth/2.,xx,yy,zz;
		xx = b[0]*x + b[1]*y + b[2]*z + dx;
		yy = b[3]*x + b[4]*y + b[5]*z + dy;
		zz = b[6]*x + b[7]*y + b[8]*z + dz;
		register float d = get_persp(Bp.pf,zz,Depth);
		xx = Width/2. + d*xx;	yy = Height/2. + d*yy;

		r[0] = (unsigned char)(255*p.r);
		r[1] = (unsigned char)(255*p.g);
		r[2] = (unsigned char)(255*p.b);
		register long i0=long(xx)+Width*(Height-1-long(yy));
		if(i0>=0 && i0<Width*Height && zz>Z[3*i0])
		{	Z[3*i0]=z;	memcpy(C+12*i0,r,4);	OI[i0]=-1;	}
	}
}
//-----------------------------------------------------------------------------
void mglCanvas::Finish()
{
	static mglMatrix bp;
	if(Quality==MGL_DRAW_NONE)	return;
#if MGL_HAVE_PTHREAD
	pthread_mutex_lock(&mutexPrm);
	pthread_mutex_lock(&mutexPnt);
	pthread_mutex_lock(&mutexClf);
#elif MGL_HAVE_OMP
	omp_set_lock((omp_lock_t*)lockClf);
#endif
	if(Quality==MGL_DRAW_DOTS)
	{
		mglStartThread(&mglCanvas::pxl_dotsdr,this,Pnt.size());
		mglStartThread(&mglCanvas::pxl_memcpy,this,Width*Height);
		mglStartThread(&mglCanvas::pxl_backgr,this,Width*Height);
	}
	else
	{
		if((Quality&MGL_DRAW_LMEM) || (memcmp(&Bp,&bp,sizeof(mglMatrix)) && !(Quality&MGL_DRAW_LMEM) && Prm.size()>0))
			clr(MGL_FINISHED);
		if(!get(MGL_FINISHED))
		{
			if(!(Quality&MGL_DRAW_LMEM) && Prm.size()>0)
			{
				PreparePrim(0);	bp=Bp;
				clr(MGL_FINISHED);
				mglStartThread(&mglCanvas::pxl_primdr,this,Prm.size());
			}
			size_t n=Width*Height;
			BDef[3] = (Flag&3)!=2 ? 0:255;
			if(Quality&MGL_DRAW_NORM)	mglStartThread(&mglCanvas::pxl_combine,this,n);
			else 			mglStartThread(&mglCanvas::pxl_memcpy,this,n);
			BDef[3] = 255;
			mglStartThread(&mglCanvas::pxl_backgr,this,n);
			set(MGL_FINISHED);
		}
	}
#if MGL_HAVE_PTHREAD
	pthread_mutex_unlock(&mutexClf);
	pthread_mutex_unlock(&mutexPnt);
	pthread_mutex_unlock(&mutexPrm);
#elif MGL_HAVE_OMP
	omp_unset_lock((omp_lock_t*)lockClf);
#endif
}
//-----------------------------------------------------------------------------
void mglCanvas::ClfZB(bool force)
{
	if(!force && (Quality&MGL_DRAW_LMEM))	return;
	register long n=Width*Height;
	memset(C,0,12*n);	memset(OI,0,n*sizeof(int));
#pragma omp parallel for
	for(long i=0;i<3*n;i++)	Z[i] = -1e20f;
	clr(MGL_FINISHED);
}
//-----------------------------------------------------------------------------
void mglCanvas::Clf(mglColor Back)
{
	Fog(0);	PDef = 0xffff;	pPos = 0;
	ClearFrame();
	if((Flag&3)==2)	Back.Set(0,0,0,0);
	if(Back!=NC)	FillBackground(Back);
}
//-----------------------------------------------------------------------------
void mglCanvas::Clf(const char *col)
{
	Fog(0);	PDef = 0xffff;	pPos = 0;
	ClearFrame();
	mglTexture txt(col,0,0);
	FillBackground(txt.col[1]);
}
//-----------------------------------------------------------------------------
void mglCanvas::Rasterize()
{
	Finish();
	memcpy(GB,G4,4*Width*Height);
}
//-----------------------------------------------------------------------------
bool MGL_NO_EXPORT mgl_read_image(unsigned char **g, int &w, int &h, const char *fname);
void mglCanvas::LoadBackground(const char *fname, double alpha)
{
	mgl_read_image(&GB,Width,Height,fname);
	if(alpha<1 && alpha>0)
#pragma omp parallel for
		for(long i=0;i<Width*Height;i++)	GB[4*i+3] = (unsigned char)(GB[4*i+3]*alpha);
}
//-----------------------------------------------------------------------------
void mglCanvas::FillBackground(const mglColor &cc)
{
	BDef[0] = (unsigned char)(255*cc.r);	BDef[1] = (unsigned char)(255*cc.g);
	BDef[2] = (unsigned char)(255*cc.b);	BDef[3] = (unsigned char)(255*cc.a);
#pragma omp parallel for
	for(long i=0;i<Width*Height;i++)	memcpy(GB+4*i,BDef,4);
}
//-----------------------------------------------------------------------------
void mglCanvas::pxl_other(long id, long n, const void *p)
{
	const mglCanvas *gr = (const mglCanvas *)p;
	if(Quality&MGL_DRAW_NORM)
#if !MGL_HAVE_PTHREAD
#pragma omp parallel for
#endif
		for(long k=id;k<n;k+=mglNumThr)
		{
			register long i = k%Width, j = Height-1-(k/Width);
			pnt_plot(i,j,gr->Z[3*k+2],gr->C+12*k+8,gr->OI[k]);
			pnt_plot(i,j,gr->Z[3*k+1],gr->C+12*k+4,gr->OI[k]);
			pnt_plot(i,j,gr->Z[3*k],gr->C+12*k,gr->OI[k]);
		}
	else
#if !MGL_HAVE_PTHREAD
#pragma omp parallel for
#endif
		for(long k=id;k<n;k+=mglNumThr)
		{
			register long i = k%Width, j = Height-1-(k/Width);
			pnt_plot(i,j,gr->Z[3*k],gr->C+12*k,gr->OI[k]);
		}
}
//-----------------------------------------------------------------------------
void mglCanvas::Combine(const mglCanvas *gr)
{
	if(Width!=gr->Width || Height!=gr->Height || !gr)	return;	// wrong sizes
	mglStartThread(&mglCanvas::pxl_other,this,Width*Height,gr);
}
//-----------------------------------------------------------------------------
void mglCanvas::pnt_plot(long x,long y,mreal z,const unsigned char ci[4], int obj_id)
{
	if(ci[3])
	{
		long i0=x+Width*(Height-1-y);
		unsigned char *cc = C+12*i0, c[4];
		memcpy(c,ci,4);
		float *zz = Z+3*i0, zf = FogDist*(z/Depth-0.5-FogDz);
		if(zf<0)	// add fog
		{
			int d = int(255.f-255.f*exp(5.f*zf));
			unsigned char cb[4] = {BDef[0], BDef[1], BDef[2], (unsigned char)d};
			if(d==255)	return;
			combine(c,cb);
		}
		if(Quality&MGL_DRAW_NORM)
		{
			if(z>=zz[1])	// shift point on slice down and paste new point
			{
				zz[2] = zz[1];	combine(cc+8,cc+4);
				if(z>=zz[0])
				{	zz[1] = zz[0];	zz[0] = z;	OI[i0]=obj_id;
					memcpy(cc+4,cc,4);	memcpy(cc,c,4);		}
				else	{	zz[1] = z;	memcpy(cc+4,c,4);	}
			}
			else
			{
				if(z>=zz[2])	// shift point on slice down and paste new point
				{	zz[2] = z;	combine(cc+8,c);	}
				else		// point below the background
				{	combine(c,cc+8);	memcpy(cc+8,c,4);	}
			}
		}
		if(Quality&MGL_DRAW_FAST)
		{
			if(z>=zz[0])	// point upper the background
			{	zz[0]=z;	combine(cc,c);	OI[i0]=obj_id;	}
		}
		else
		{
			if(z>=zz[0])	// point upper the background
			{	zz[0]=z;	memcpy(cc,c,4);		OI[i0]=obj_id;	}
		}
	}
}
//-----------------------------------------------------------------------------
unsigned char* mglCanvas::col2int(const mglPnt &p,unsigned char *r, int obj_id) const
{
//	if(!r)	return r;	// NOTE r must be provided!
	if(p.a<=0)	{	memset(r,0,4);	return r;	}
	register float b0=0,b1=0,b2=0, ar,ag,ab,dif;
	const size_t nl = p.sub>=0?p.sub:1-p.sub;
	const bool glob = !get(MGL_LOCAL_LIGHT);
	ar = ag = ab = glob?AmbBr:Sub[nl].AmbBr;
	dif = glob?DifBr:Sub[nl].DifBr;

	if(mgl_isnum(p.u+p.v+p.w))
	{
		float d0,d1,d2,nn;
		for(long i=0;i<10;i++)
		{
			const mglLight &ll=glob?light[i]:Sub[nl].light[i];
			if(!ll.n)	continue;
			if(mgl_isnan(ll.q.x))		// source at infinity
			{
				nn = 2*(p.u*ll.p.x+p.v*ll.p.y+p.w*ll.p.z) / (p.u*p.u+p.v*p.v+p.w*p.w+1e-6);
				d0 = ll.p.x - p.u*nn;
				d1 = ll.p.y - p.v*nn;
				d2 = ll.p.z - p.w*nn;
				nn = 1 + d2/sqrt(d0*d0+d1*d1+d2*d2+1e-6);

				nn = exp(-ll.a*nn)*ll.b*2;
				b0 += nn*ll.c.r;
				b1 += nn*ll.c.g;
				b2 += nn*ll.c.b;
			}
			else		// diffuse and specular light
			{
				d0 = ll.q.x-p.x;	// direction to light source
				d1 = ll.q.y-p.y;
				d2 = ll.q.z-p.z;
				nn = 1+(d0*ll.p.x+d1*ll.p.y+d2*ll.p.z)/sqrt(d0*d0+d1*d1+d2*d2+1e-6);
				float bb = exp(-3*ll.a*nn);	nn = bb*dif*2;
				ar += nn*ll.c.r;
				ag += nn*ll.c.g;
				ab += nn*ll.c.b;

				nn = 2*(p.u*d0+p.v*d1+p.w*d2) / (p.u*p.u+p.v*p.v+p.w*p.w+1e-6);
				d0 -= p.u*nn;	d1 -= p.v*nn;	d2 -= p.w*nn;
				nn = 1 + d2/sqrt(d0*d0+d1*d1+d2*d2+1e-6);

				nn = exp(-ll.a*nn)*bb*ll.b*2;
				b0 += nn*ll.c.r;
				b1 += nn*ll.c.g;
				b2 += nn*ll.c.b;
			}
		}
		b0 += (ar>1 ? 1:ar)*p.r;	// diffuse light
		b1 += (ag>1 ? 1:ag)*p.g;
		b2 += (ab>1 ? 1:ab)*p.b;
		b0 = b0<1 ? b0 : 1;			// normalize components
		b1 = b1<1 ? b1 : 1;
		b2 = b2<1 ? b2 : 1;
	}
	else
	{	b0=p.r;	b1=p.g;	b2=p.b;	}
	// try to highlight faces
	if(obj_id==HighId)	{	b0*=0.7;	b1*=0.7;	b2*=0.7;	}
	r[0] = (unsigned char)(255*b0);
	r[1] = (unsigned char)(255*b1);
	r[2] = (unsigned char)(255*b2);
//	r[3] = get(MGL_ENABLE_ALPHA) ? (unsigned char)(255*p.a) : 255;
	r[3] = (unsigned char)((Quality&MGL_DRAW_NORM)?255*p.a:255);
	return r;
}
//-----------------------------------------------------------------------------
/// color mixing: color c1 is under color c2 !!!
void mglCanvas::combine(unsigned char *c1, const unsigned char *c2) const
{
	if(c2[3])
	{
		const register unsigned a1=c1[3], a2=c2[3];
		if((Flag&3)==0)
		{
			register unsigned b1=255-a2;
			c1[0] = (c1[0]*b1 + c2[0]*a2)/256;
			c1[1] = (c1[1]*b1 + c2[1]*a2)/256;
			c1[2] = (c1[2]*b1 + c2[2]*a2)/256;
			c1[3] = (unsigned char)(a2+a1*b1/255);
		}
		else if((Flag&3)==1)
		{
			c1[0] = (unsigned char)((255-a1*(255-c1[0])/256)*(255-a2*(255-c2[0])/256)/256);
			c1[1] = (unsigned char)((255-a1*(255-c1[1])/256)*(255-a2*(255-c2[1])/256)/256);
			c1[2] = (unsigned char)((255-a1*(255-c1[2])/256)*(255-a2*(255-c2[2])/256)/256);
			c1[3] = 255;
		}
		else if((Flag&3)==2)
		{
			register unsigned b1,b2,b3;
			b1 = (c1[0]*a1 + c2[0]*a2)/255;	c1[0] = b1<255 ? b1 : 255;
			b2 = (c1[1]*a1 + c2[1]*a2)/255;	c1[1] = b2<255 ? b2 : 255;
			b3 = (c1[2]*a1 + c2[2]*a2)/255;	c1[2] = b3<255 ? b3 : 255;
			c1[3] = 255;
		}
	}
}
//-----------------------------------------------------------------------------
unsigned char **mglCanvas::GetRGBLines(long &w, long &h, unsigned char *&f, bool alpha)
{
	unsigned char **p;
	Finish();
	p = (unsigned char **)malloc(Height * sizeof(unsigned char *));
	long d = (alpha ? 4:3)*Width;
	unsigned char *gg = (alpha?G4:G);
	for(long i=0;i<Height;i++)	p[i] = gg + d*i;
	w = Width;	h = Height;		f = 0;
	return p;
}
//-----------------------------------------------------------------------------
bool inline visible(long i, long j, const unsigned char m[8], mreal pw, int a)	// Check if pixel visible
{
	register float c = mgl_cos[(a+360)%360], s = mgl_cos[(a+450)%360];
//	register int ii = int(0.5+(i*c+j*s)/pw)%8, jj = int(0.5+(j*c-i*s)/pw)%8;
//	if(ii<0)	ii+=8;	if(jj<0)	jj+=8;
	register int ii = int(0.5+(i*c+j*s)/pw)&7, jj = int(0.5+(j*c-i*s)/pw)&7;
	return m[jj] & (1L<<ii);
}
//-----------------------------------------------------------------------------
/* Bilinear interpolation r(u,v) = r0 + (r1-r0)*u + (r2-r0)*v + (r3+r0-r1-r2)*u*v
	is used (where r is one of {x,y,z,R,G,B,A}. Variables u,v are determined
	for each point (x,y) and selected one pair which 0<u<1 and 0<v<1.*/
void mglCanvas::quad_draw(const mglPnt &p1, const mglPnt &p2, const mglPnt &p3, const mglPnt &p4, const mglDrawReg *d)
{
	if(Quality&MGL_DRAW_LMEM)
	{
		if(!(Quality&3))
		{	fast_draw(p1,p4,d);	fast_draw(p2,p3,d);	return;	}
		if(!(Quality&MGL_DRAW_NORM))
		{	trig_draw(p1,p2,p4,true,d);	trig_draw(p1,p3,p4,true,d);	return;	}
	}
	unsigned char r[4];
	long y1,x1,y2,x2;
	mglPnt d1(p2-p1), d2(p3-p1), d3(p4+p1-p2-p3);

	if(d1.x==0 && d1.y==0)	{	trig_draw(p1,p3,p4,true,d);	return;	}
	if(d2.x==0 && d2.y==0)	{	trig_draw(p1,p2,p4,true,d);	return;	}

	x1 = long(mgl_min(mgl_min(p1.x,p2.x), mgl_min(p3.x,p4.x)));	// bounding box
	y1 = long(mgl_min(mgl_min(p1.y,p2.y), mgl_min(p3.y,p4.y)));
	x2 = long(mgl_max(mgl_max(p1.x,p2.x), mgl_max(p3.x,p4.x)));
	y2 = long(mgl_max(mgl_max(p1.y,p2.y), mgl_max(p3.y,p4.y)));
	x1=mgl_max(x1,d->x1);	x2=mgl_min(x2,d->x2);
	y1=mgl_max(y1,d->y1);	y2=mgl_min(y2,d->y2);
//	if(x1>x2 || y1>y2)	return;

	const float dd = d1.x*d2.y-d1.y*d2.x;
	const float dsx =-4*(d2.y*d3.x - d2.x*d3.y)*d1.y;
	const float dsy = 4*(d2.y*d3.x - d2.x*d3.y)*d1.x;

	mglPoint n1(mglPoint(p2.x-p1.x,p2.y-p1.y,p2.z-p1.z)^mglPoint(p3.x-p1.x,p3.y-p1.y,p3.z-p1.z));
	mglPoint n2(mglPoint(p2.x-p4.x,p2.y-p4.y,p2.z-p4.z)^mglPoint(p3.x-p4.x,p3.y-p4.y,p3.z-p4.z));
	mglPoint nr((n1.x+n2.x)*0.5,(n1.y+n2.y)*0.5,(n1.z+n2.z)*0.5);

	const float x0 = p1.x, y0 = p1.y;
	const int oi = d->ObjId, ang=d->angle;
	const mreal pw = d->PenWidth;
	const uint64_t pd = d->PDef;
	for(long j=y1;j<=y2;j++)	for(long i=x1;i<=x2;i++)
	{
		if(pd==MGL_SOLID_MASK || visible(i,j,d->m, pw,ang))
		{
			register float xx = (i-x0), yy = (j-y0), s;
			s = dsx*xx + dsy*yy + (dd+d3.y*xx-d3.x*yy)*(dd+d3.y*xx-d3.x*yy);
			if(s>=0)
			{
				s = sqrt(s);
				register float qu = d3.x*yy - d3.y*xx + dd + s;
				register float qv = d3.y*xx - d3.x*yy + dd + s;
				register float u = 2.f*(d2.y*xx - d2.x*yy)/qu;
				register float v = 2.f*(d1.x*yy - d1.y*xx)/qv;
				if(u*(1.f-u)<0.f || v*(1.f-v)<0.f)	// first root bad
				{
					qu = d3.x*yy - d3.y*xx + dd - s;
					qv = d3.y*xx - d3.x*yy + dd - s;
//					u = v = -1.f;
					u = 2.f*(d2.y*xx - d2.x*yy)/qu;	v = 2.f*(d1.x*yy - d1.y*xx)/qv;
					if(u*(1.f-u)<0.f || v*(1.f-v)<0.f)	continue;	// second root bad
				}
				mglPnt p(p1+d1*u+d2*v+d3*(u*v));
				if(mgl_isnan(p.u) && mgl_isnum(p.v))
				{	p.u = nr.x;	p.v = nr.y;	p.w = nr.z;	}
				pnt_plot(i,j,p.z,col2int(p,r,oi),oi);
			}
		}
	}
}
//-----------------------------------------------------------------------------
void mglCanvas::quad_pix(long i, long j, const mglPnt &p1, const mglPnt &p2, const mglPnt &p3, const mglPnt &p4, const mglDrawReg *d)
{
	if(!visible(i,j,d->m, d->PenWidth,d->angle))	return;
	mglPnt d1(p2-p1), d2(p3-p1), d3(p4+p1-p2-p3);
	register float dd = d1.x*d2.y-d1.y*d2.x;
	register float dsx =-4*(d2.y*d3.x - d2.x*d3.y)*d1.y;
	register float dsy = 4*(d2.y*d3.x - d2.x*d3.y)*d1.x;
	register float xx = (i-p1.x), yy = (j-p1.y), s;
	s = dsx*xx + dsy*yy + (dd+d3.y*xx-d3.x*yy)*(dd+d3.y*xx-d3.x*yy);
	if(s<0)	return;	// no solution
	s = sqrt(s);
	register float qu = d3.x*yy - d3.y*xx + dd + s, u=-1;
	register float qv = d3.y*xx - d3.x*yy + dd + s, v=-1;
	if(qu && qv)
	{
		u = 2.f*(d2.y*xx - d2.x*yy)/qu;
		v = 2.f*(d1.x*yy - d1.y*xx)/qv;
	}
	if(u*(1.f-u)<0.f || v*(1.f-v)<0.f)	// first root bad
	{
		qu = d3.x*yy - d3.y*xx + dd - s;
		qv = d3.y*xx - d3.x*yy + dd - s;
		u = v = -1.f;
		if(qu && qv)
		{
			u = 2.f*(d2.y*xx - d2.x*yy)/qu;
			v = 2.f*(d1.x*yy - d1.y*xx)/qv;
		}
		if(u*(1.f-u)<0.f || v*(1.f-v)<0.f)	return;	// second root bad
	}
	mglPnt p(p1+d1*u+d2*v+d3*(u*v));
	if(mgl_isnan(p.u) && mgl_isnum(p.v))
	{
		mglPoint n1(mglPoint(p2.x-p1.x,p2.y-p1.y,p2.z-p1.z)^mglPoint(p3.x-p1.x,p3.y-p1.y,p3.z-p1.z));
		mglPoint n2(mglPoint(p2.x-p4.x,p2.y-p4.y,p2.z-p4.z)^mglPoint(p3.x-p4.x,p3.y-p4.y,p3.z-p4.z));
		p.u = (n1.x+n2.x)*0.5;
		p.v = (n1.y+n2.y)*0.5;
		p.w = (n1.z+n2.z)*0.5;
	}
	unsigned char r[4];
	pnt_plot(i,j,p.z,col2int(p,r,d->ObjId),d->ObjId);
}
//-----------------------------------------------------------------------------
/* Linear interpolation r(u,v) = r0 + (r1-r0)*u + (r2-r0)*v is used, where r is
	one of {x,y,z,R,G,B,A}. Variables u,v are determined for each point (x,y).
	Point plotted is u>0 and v>0 and u+v<1.*/
void mglCanvas::trig_draw(const mglPnt &p1, const mglPnt &p2, const mglPnt &p3, bool anorm, const mglDrawReg *d)
{
	if(!(Quality&3) && anorm)
	{	fast_draw(p1,p2,d);	fast_draw(p1,p3,d);	fast_draw(p2,p3,d);	return;	}
	unsigned char r[4];
	long y1,x1,y2,x2;
	const mglPnt d1(p2-p1), d2(p3-p1);

	const float tmp = d2.x*d1.y - d1.x*d2.y;
	if(fabs(tmp)<1e-5)	return;		// points lies on the same line
	const float dyv =-d1.x/tmp,	dxv = d1.y/tmp;
	const float dyu = d2.x/tmp,	dxu =-d2.y/tmp;

	x1 = long(mgl_min(p1.x<p2.x?p1.x:p2.x, p3.x));	// bounding box
	y1 = long(mgl_min(p1.y<p2.y?p1.y:p2.y, p3.y));
	x2 = long(mgl_max(p1.x>p2.x?p1.x:p2.x, p3.x));
	y2 = long(mgl_max(p1.y>p2.y?p1.y:p2.y, p3.y));
	x1=x1>d->x1?x1:d->x1;	x2=x2<d->x2?x2:d->x2;
	y1=y1>d->y1?y1:d->y1;	y2=y2<d->y2?y2:d->y2;
//	if(x1>x2 || y1>y2)	return;
	// default normale
	const mglPoint nr(mglPoint(p2.x-p1.x,p2.y-p1.y,p2.z-p1.z)^mglPoint(p3.x-p1.x,p3.y-p1.y,p3.z-p1.z));
	const float x0 = p1.x, y0 = p1.y;
	// provide additional height to be well visible on the surfaces
	const float dz = anorm? 0 : (Width>2 ? 1 : 1e-5*Width);
	const int oi = d->ObjId, ang=d->angle;
	const mreal pw = d->PenWidth;
	const uint64_t pd = d->PDef;
	if(Quality&MGL_DRAW_NORM)	for(long j=y1;j<=y2;j++)	for(long i=x1;i<=x2;i++)
	{
		if(pd==MGL_SOLID_MASK || visible(i,j,d->m, pw,ang))
		{
			register float xx = (i-x0), yy = (j-y0);
			register float u = dxu*xx+dyu*yy, v = dxv*xx+dyv*yy;
			if(u<0 || v<0 || u+v>1)	continue;
			mglPnt p(p1+d1*u+d2*v);
			if(mgl_isnan(p.u) && mgl_isnum(p.v) && anorm)
			{	p.u = nr.x;	p.v = nr.y;	p.w = nr.z;	}
			pnt_plot(i,j,p.z+dz,col2int(p,r,oi),oi);
		}
	}
	else
	{
		col2int(p1,r,oi);
		float zz = p1.z+dz;
		for(long j=y1;j<=y2;j++)	for(long i=x1;i<=x2;i++)
		{
			if(pd==MGL_SOLID_MASK || visible(i,j,d->m, pw,ang))
			{
				register float xx = (i-x0), yy = (j-y0);
				register float u = dxu*xx+dyu*yy, v = dxv*xx+dyv*yy;
				if(u<0 || v<0 || u+v>1)	continue;
				pnt_plot(i,j,zz,r,oi);
			}
		}
	}
}
//-----------------------------------------------------------------------------
void mglCanvas::trig_pix(long i, long j, const mglPnt &p1, const mglPnt &p2, const mglPnt &p3, bool anorm, const mglDrawReg *d)
{
	if(!visible(i,j,d->m, d->PenWidth,d->angle))	return;
	mglPnt d1(p2-p1), d2(p3-p1);
	register float dd = d2.x*d1.y - d1.x*d2.y;
	if(fabs(dd)<1e-5)	return;		// points lies on the same line
	register float dyv =-d1.x/dd, dxv = d1.y/dd, dyu = d2.x/dd, dxu =-d2.y/dd;
	register float xx = (i-p1.x), yy = (j-p1.y);
	register float u = dxu*xx+dyu*yy, v = dxv*xx+dyv*yy;
	if(u<0 || v<0 || u+v>1)	return;
	mglPnt p(p1+d1*u+d2*v);
	if(mgl_isnan(p.u) && mgl_isnum(p.v) && anorm)
	{	mglPoint nr(mglPoint(p2.x-p1.x,p2.y-p1.y,p2.z-p1.z)^mglPoint(p3.x-p1.x,p3.y-p1.y,p3.z-p1.z));
		p.u = nr.x;	p.v = nr.y;	p.w = nr.z;	}
	unsigned char r[4];
	pnt_plot(i,j,p.z,col2int(p,r,d->ObjId),d->ObjId);
}
//-----------------------------------------------------------------------------
inline unsigned char mgl_sline(unsigned char c,float x)
{	x*=x/2;	return (unsigned char)((c)/(1+x+x*x/5));	}
void mglCanvas::line_draw(const mglPnt &p1, const mglPnt &p2, const mglDrawReg *dr)
{
	if((Quality&3)==MGL_DRAW_WIRE)	{	fast_draw(p1,p2,dr);	return;	}	// previously was <2. This may slightly slow down for Quality=1
	unsigned char r[4];
	long y1,x1,y2,x2;

	const float dz = Width>2 ? 1 : 1e-5*Width;		// provide additional height to be well visible on the surfaces
	const int oi = dr->ObjId;
	const float pw=dr->PenWidth*(oi==HighId?2:1), dpw=pen_delta*(oi==HighId?2:3);
	const mglPnt d(p2-p1);
	bool hor = fabs(d.x)>fabs(d.y);

	x1 = long(p1.x<p2.x?p1.x:p2.x);	y1 = long(p1.y<p2.y?p1.y:p2.y);	// bounding box
	x2 = long(p1.x>p2.x?p1.x:p2.x);	y2 = long(p1.y>p2.y?p1.y:p2.y);
	x1 -= pw+10/dpw;	x2 += pw+10/dpw;
	y1 -= pw+10/dpw;	y2 += pw+10/dpw;
	x1=x1>dr->x1?x1:dr->x1;	x2=x2<dr->x2?x2:dr->x2;
	y1=y1>dr->y1?y1:dr->y1;	y2=y2<dr->y2?y2:dr->y2;
	const float dd = hypot(d.x, d.y);
//	if(x1>x2 || y1>y2 || dd<1e-5)	return;
	if(dd<1e-5)	return;

	const float dxv = d.y/dd, dyv =-d.x/dd;
	const float dxu = d.x/dd, dyu = d.y/dd;

	const uint64_t pd = dr->PDef;
	const mreal pp = dr->pPos;
	if(hor)	for(long i=x1;i<=x2;i++)
	{
		y1 = int(p1.y+d.y*(i-p1.x)/d.x - pw - 10/dpw);
		y2 = int(p1.y+d.y*(i-p1.x)/d.x + pw + 10/dpw);
		y1=y1>dr->y1?y1:dr->y1;	y2=y2<dr->y2?y2:dr->y2;
		if(y1>y2)	continue;
		for(long j=y1;j<=y2;j++)
		{
			register float xx = (i-p1.x), yy = (j-p1.y);
			register float u = dxu*xx+dyu*yy, v = dxv*xx+dyv*yy;	v = v*v;
			if(u<0)			v += u*u;
			else if(u>dd)	v += (u-dd)*(u-dd);
//			if(v>pw*pw)		continue;
			if(!(pd & ((uint64_t)1<<long(fmod(pp+u/pw/1.5, 16)) ) ))	continue;
			mglPnt p(p1+d*(u/dd));	col2int(p,r,oi);
			r[3] = v<(pw-1)*(pw-1)/4 ? 255 : mgl_sline(255,dpw*(sqrt(v)+(1-pw)/2));
			pnt_plot(i,j,p.z+dz,r,oi);
		}
	}
	else	for(long j=y1;j<=y2;j++)
	{
		x1 = int(p1.x+d.x*(j-p1.y)/d.y - pw - 10/dpw);
		x2 = int(p1.x+d.x*(j-p1.y)/d.y + pw + 10/dpw);
		x1=x1>dr->x1?x1:dr->x1;	x2=x2<dr->x2?x2:dr->x2;
		if(x1>x2)	continue;

		for(long i=x1;i<=x2;i++)
		{
			register float xx = (i-p1.x), yy = (j-p1.y);
			register float u = dxu*xx+dyu*yy, v = dxv*xx+dyv*yy;	v = v*v;
			if(u<0)			v += u*u;
			else if(u>dd)	v += (u-dd)*(u-dd);
//			if(v>pw*pw)		continue;
			if(!(pd & ((uint64_t)1<<long(fmod(pp+u/pw/1.5, 16)))))		continue;
			mglPnt p(p1+d*(u/dd));	col2int(p,r,oi);
			r[3] = v<(pw-1)*(pw-1)/4 ? 255 : mgl_sline(255,dpw*(sqrt(v)+(1-pw)/2));
			pnt_plot(i,j,p.z+dz,r,oi);
		}
	}
}
//-----------------------------------------------------------------------------
void mglCanvas::pnt_fast(long x,long y,mreal z,const unsigned char ci[4], int obj_id)
{
	register long i0=x+Width*(Height-1-y);
	if(ci[3]!=0 && z>Z[3*i0])	// point upper the background
	{	Z[3*i0]=z;	memcpy(C+12*i0,ci,4);	OI[i0]=obj_id;	}
}
//-----------------------------------------------------------------------------
void mglCanvas::fast_draw(const mglPnt &p1, const mglPnt &p2, const mglDrawReg *dr)
{
	if(p1.x==p2.x && p1.y==p2.y) return;
	const mglPnt d(p2-p1);
	const int oi = dr->ObjId;
	unsigned char r[4];	col2int(p1,r,oi);
	long y1,x1,y2,x2;

	const bool hor = fabs(d.x)>fabs(d.y);

	x1 = long(p1.x<p2.x?p1.x:p2.x);	y1 = long(p1.y<p2.y?p1.y:p2.y);	// bounding box
	x2 = long(p1.x>p2.x?p1.x:p2.x);	y2 = long(p1.y>p2.y?p1.y:p2.y);
	x1=x1>dr->x1?x1:dr->x1;	x2=x2<dr->x2?x2:dr->x2;
	y1=y1>dr->y1?y1:dr->y1;	y2=y2<dr->y2?y2:dr->y2;
	if(x1>x2 || y1>y2)	return;
	const float dz = Width>2 ? 1 : 1e-5*Width;	// provide additional height to be well visible on the surfaces

	if(hor)	for(long i=x1;i<=x2;i++)
	{
		register long c = long(p1.y+d.y*(i-p1.x)/d.x);
		if(c>=y1 && c<=y2)
			pnt_fast(i, c, p1.z+d.z*(i-p1.x)/d.x+dz, r,oi);
	}
	else	for(long i=y1;i<=y2;i++)
	{
		register long c = long(p1.x+d.x*(i-p1.y)/d.y);
		if(c>=x1 && c<=x2)
			pnt_fast(c, i, p1.z+d.z*(i-p1.y)/d.y+dz, r,oi);
	}
}
//-----------------------------------------------------------------------------
void mglCanvas::line_pix(long i, long j, const mglPnt &p1, const mglPnt &p2, const mglDrawReg *dr)
{
	register float xx = (i-p1.x), yy = (j-p1.y);
	mglPnt d(p2-p1);
	register float dd = hypot(d.x, d.y);
	register float dxv = d.y/dd, dyv =-d.x/dd, dxu = d.x/dd, dyu = d.y/dd;
	register float u = dxu*xx+dyu*yy, v = dxv*xx+dyv*yy;	v = v*v;
	if(u<0)			v += u*u;
	else if(u>dd)	v += (u-dd)*(u-dd);
	register float pw=dr->PenWidth, dpw=3*pen_delta;
	if(dr->ObjId==HighId)	{	pw *= 2;	dpw=2*pen_delta;	}
	if(v>pw*pw || !(dr->PDef & ( (uint64_t)1<<long(fmod(dr->pPos+u/pw/1.5, 16)) ) ))	return;
	mglPnt p(p1+d*(u/dd));
	unsigned char r[4];
	col2int(p,r,dr->ObjId);
	r[3] = v<(pw-1)*(pw-1)/4 ? 255 : mgl_sline(255,dpw*(sqrt(v)+(1-pw)/2));
	register float dz = Width>2 ? 1 : 1e-5*Width;		// provide additional height to be well visible on the surfaces
	pnt_plot(i,j,p.z+dz,r,dr->ObjId);
}
//-----------------------------------------------------------------------------
void mglCanvas::pnt_draw(const mglPnt &p, const mglDrawReg *dr)
{
//	if(k<0 || !dr)	return;
	const int oi = dr->ObjId;
	const float pw=(oi==HighId?6:3)*dr->PenWidth,dpw=(oi==HighId?2:3)*pen_delta;
	unsigned char cs[4], cc;
	col2int(p,cs,oi);	cc = cs[3];
	if(cc==0)	return;
	const long s = long(pw+10/dpw+fabs(pw));
	const long i1=mgl_max(-s,dr->x1-p.x),i2=mgl_min(s,dr->x2-p.x);
	const long j1=mgl_max(-s,dr->y1-p.y),j2=mgl_min(s,dr->y2-p.y);
	if(!(Quality&3))	for(long j=j1;j<=j2;j++)	for(long i=i1;i<=i2;i++)	// fast draw
	{
		register float v = i*i+j*j;
		if(v>1+(pw-1)*(pw-1)/4)	continue;
		pnt_plot(p.x+i,p.y+j,p.z,cs,oi);
	}
	else	for(long j=j1;j<=j2;j++)	for(long i=i1;i<=i2;i++)
	{
		register float v = i*i+j*j;
		cs[3] = v<(pw-1)*(pw-1)/4 ? cc : mgl_sline(cc,dpw*(sqrt(v)+(1-pw)/2));
		pnt_plot(p.x+i,p.y+j,p.z,cs,oi);
	}
}
//-----------------------------------------------------------------------------
void mglCanvas::pnt_pix(long i, long j, const mglPnt &p, const mglDrawReg *dr)
{
	register float pw=3*dr->PenWidth,dpw=3*pen_delta;
	if(dr->ObjId==HighId)	{	pw *= 2;	dpw=2*pen_delta;	}
	unsigned char cs[4];
	col2int(p,cs,dr->ObjId);
	register float xx = (i-p.x), yy = (j-p.y), v = xx*xx+yy*yy;
	if(cs[3]==0 || v>(10/dpw+pw)*(10/dpw+pw))	return;
	if(v<(pw-1)*(pw-1)/4)	cs[3] = mgl_sline(cs[3],dpw*(sqrt(v)+(1-pw)/2));
	pnt_plot(i,j,p.z,cs,dr->ObjId);
}
//-----------------------------------------------------------------------------
void mglCanvas::mark_draw(const mglPnt &q, char type, mreal size, mglDrawReg *d)
{
	const int oi = d->ObjId;
	unsigned char cs[4];	col2int(q,cs,oi);
	const unsigned char ca = cs[3];// = size>0 ? 255 : 255*q.t;
	const mreal ss=(strchr("xsSoO",type)?1:1.1)*fabs(size), dpw=(oi==HighId?2:3)*pen_delta;
	mreal PW=1;

	if(type=='.' || ss==0)
	{
		if(d)	PW = 3*(ss?ss:sqrt(font_factor/400));
		if(oi==HighId)	PW *= 2;
		const mreal pw = PW;
		register mreal dd = pw+10/dpw;
		long x1 = long(q.x-dd), y1 = long(q.y-dd);	// bounding box
		long x2 = long(q.x+dd), y2 = long(q.y+dd);
		x1=x1>d->x1?x1:d->x1;	x2=x2<d->x2?x2:d->x2;
		y1=y1>d->y1?y1:d->y1;	y2=y2<d->y2?y2:d->y2;
		if(x1>x2 || y1>y2)	return;
		const float V=(pw-1)*(pw-1)/4,S=(1-pw)/2;
		
		for(long j=y1;j<=y2;j++)	for(long i=x1;i<=x2;i++)
		{
			register float dx=i-q.x, dy=j-q.y, v=dx*dx+dy*dy;
			register int sum = v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
			cs[3] = ca*sum/255;
			pnt_plot(i,j,q.z+1,cs,oi);
		}
	}
	else
	{
		if(d)
		{
			d->PDef = MGL_SOLID_MASK;	d->angle = 0;
			PW = d->PenWidth*sqrt(fabs(50*size));
			if(PW<1)	PW=1;
		}
		if(oi==HighId)	PW *= 2;
		const mreal pw = PW;

		register mreal dd = ss+pw+10/dpw;
		long x1 = long(q.x-dd), y1 = long(q.y-dd);	// bounding box
		long x2 = long(q.x+dd), y2 = long(q.y+dd);
		x1=x1>d->x1?x1:d->x1;	x2=x2<d->x2?x2:d->x2;
		y1=y1>d->y1?y1:d->y1;	y2=y2<d->y2?y2:d->y2;
		if(x1>x2 || y1>y2)	return;
		const float V=(pw-1)*(pw-1)/4,S=(1-pw)/2;

		switch(type)
		{
		case 'P':
			for(long j=y1;j<=y2;j++)	for(long i=x1;i<=x2;i++)
			{
				register float dx=i-q.x, dy=j-q.y, v,u;
				register int sum=0;
				u = fabs(dy)-ss;	v = (dx-ss)*(dx-ss)+(u<0?0:u*u);
				sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				u = fabs(dy)-ss;	v = (dx+ss)*(dx+ss)+(u<0?0:u*u);
				sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				u = fabs(dy)-ss;	v = dx*dx+(u<0?0:u*u);
				sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				u = fabs(dx)-ss;	v = (dy-ss)*(dy-ss)+(u<0?0:u*u);
				sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				u = fabs(dx)-ss;	v = (dy+ss)*(dy+ss)+(u<0?0:u*u);
				sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				u = fabs(dx)-ss;	v = dy*dy+(u<0?0:u*u);
				sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				sum = sum>255?255:sum;	cs[3] = ca*sum/255;
				pnt_plot(i,j,q.z+1,cs,oi);
			}
			break;
		case '+':
			for(long j=y1;j<=y2;j++)	for(long i=x1;i<=x2;i++)
			{
				register float dx=i-q.x, dy=j-q.y, v,u;
				register int sum=0;
				u = fabs(dy)-ss;	v = dx*dx+(u<0?0:u*u);
				sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				u = fabs(dx)-ss;	v = dy*dy+(u<0?0:u*u);
				sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				sum = sum>255?255:sum;	cs[3] = ca*sum/255;
				pnt_plot(i,j,q.z+1,cs,oi);
			}
			break;
		case 'X':
			for(long j=y1;j<=y2;j++)	for(long i=x1;i<=x2;i++)
			{
				register float dx=i-q.x, dy=j-q.y, v,u;
				register int sum=0;
				u = fabs(dy)-ss;	v = (dx-ss)*(dx-ss)+(u<0?0:u*u);
				sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				u = fabs(dy)-ss;	v = (dx+ss)*(dx+ss)+(u<0?0:u*u);
				sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				u = fabs(dx)-ss;	v = (dy-ss)*(dy-ss)+(u<0?0:u*u);
				sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				u = fabs(dx)-ss;	v = (dy+ss)*(dy+ss)+(u<0?0:u*u);
				sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));

				u = fabs(dx+dy)-2*ss;	v = dx-dy;	v = v*v+(u<0?0:u*u);
				sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				u = fabs(dx-dy)-2*ss;	v = dx+dy;	v = v*v+(u<0?0:u*u);
				sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));

				sum = sum>255?255:sum;	cs[3] = ca*sum/255;
				pnt_plot(i,j,q.z+1,cs,oi);
			}
			break;
		case 'x':
			for(long j=y1;j<=y2;j++)	for(long i=x1;i<=x2;i++)
			{
				register float dx=i-q.x, dy=j-q.y, v,u;
				register int sum=0;
				u = fabs(dx+dy)-2*ss;	v = dx-dy;	v = v*v+(u<0?0:u*u);
				sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				u = fabs(dx-dy)-2*ss;	v = dx+dy;	v = v*v+(u<0?0:u*u);
				sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				sum = sum>255?255:sum;	cs[3] = ca*sum/255;
				pnt_plot(i,j,q.z+1,cs,oi);
			}
			break;
		case 'S':
			for(long j=y1;j<=y2;j++)	for(long i=x1;i<=x2;i++)
			{
				register float dx=i-q.x, dy=j-q.y, v,u;
				u = fabs(dy)-ss;	if(u<0)	u=0;
				v = fabs(dx)-ss;	if(v<0)	v=0;	v = u*u+v*v;
				register int sum = v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				cs[3] = ca*sum/255;
				pnt_plot(i,j,q.z+1,cs,oi);
			}
			break;
		case 's':
			for(long j=y1;j<=y2;j++)	for(long i=x1;i<=x2;i++)
			{
				register float dx=i-q.x, dy=j-q.y, v,u;
				register int sum=0;
				u = fabs(dy)-ss;	v = (dx-ss)*(dx-ss)+(u<0?0:u*u);
				sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				u = fabs(dy)-ss;	v = (dx+ss)*(dx+ss)+(u<0?0:u*u);
				sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				u = fabs(dx)-ss;	v = (dy-ss)*(dy-ss)+(u<0?0:u*u);
				sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				u = fabs(dx)-ss;	v = (dy+ss)*(dy+ss)+(u<0?0:u*u);
				sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				sum = sum>255?255:sum;	cs[3] = ca*sum/255;
				pnt_plot(i,j,q.z+1,cs,oi);
			}
			break;
		case 'D':
			for(long j=y1;j<=y2;j++)	for(long i=x1;i<=x2;i++)
			{
				register float dx=i-q.x, dy=j-q.y, v,u;
				u = fabs(dx-dy)-ss;	if(u<0)	u=0;
				v = fabs(dx+dy)-ss;	if(v<0)	v=0;	v = u*u+v*v;
				register int sum = v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				cs[3] = ca*sum/255;
				pnt_plot(i,j,q.z+1,cs,oi);
			}
			break;
		case 'd':
			for(long j=y1;j<=y2;j++)	for(long i=x1;i<=x2;i++)
			{
				register float dx=i-q.x, dy=j-q.y, v,u;
				register int sum=0;
				u = fabs(dx+dy)-ss;	v = (dx-dy-ss)*(dx-dy-ss)+(u<0?0:u*u);
				sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				u = fabs(dx+dy)-ss;	v = (dx-dy+ss)*(dx-dy+ss)+(u<0?0:u*u);
				sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				u = fabs(dx-dy)-ss;	v = (dx+dy-ss)*(dx+dy-ss)+(u<0?0:u*u);
				sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				u = fabs(dx-dy)-ss;	v = (dx+dy+ss)*(dx+dy+ss)+(u<0?0:u*u);
				sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				sum = sum>255?255:sum;	cs[3] = ca*sum/255;
				pnt_plot(i,j,q.z+1,cs,oi);
			}
			break;
		case 'Y':
			for(long j=y1;j<=y2;j++)	for(long i=x1;i<=x2;i++)
			{
				register float dx=i-q.x, dy=j-q.y, v,u;
				register int sum=0;
				u = fabs(dy+ss/2)-ss/2;	v = dx*dx+(u<0?0:u*u);
				sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				u = fabs(0.87*dx+0.5*dy-ss/2)-ss/2;	v = (0.5*dx-0.87*dy)*(0.5*dx-0.87*dy)+(u<0?0:u*u);
				sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				u = fabs(-0.87*dx+0.5*dy-ss/2)-ss/2;	v = (0.5*dx+0.87*dy)*(0.5*dx+0.87*dy)+(u<0?0:u*u);
				sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				sum = sum>255?255:sum;	cs[3] = ca*sum/255;
				pnt_plot(i,j,q.z+1,cs,oi);
			}
			break;
		case '*':
			for(long j=y1;j<=y2;j++)	for(long i=x1;i<=x2;i++)
			{
				register float dx=i-q.x, dy=j-q.y, v,u;
				register int sum=0;
				u = fabs(dy)-ss;	v = dx*dx+(u<0?0:u*u);
				sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				u = fabs(0.87*dx+0.5*dy)-ss;	v = (0.5*dx-0.87*dy)*(0.5*dx-0.87*dy)+(u<0?0:u*u);
				sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				u = fabs(-0.87*dx+0.5*dy)-ss;	v = (0.5*dx+0.87*dy)*(0.5*dx+0.87*dy)+(u<0?0:u*u);
				sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				sum = sum>255?255:sum;	cs[3] = ca*sum/255;
				pnt_plot(i,j,q.z+1,cs,oi);
			}
			break;
		case 'T':
			for(long j=y1;j<=y2;j++)	for(long i=x1;i<=x2;i++)
			{
				register float dx=i-q.x, dy=j-q.y, v,u;
				u=dy/1.5+ss/3;	v=(dx+ss-u)/2;
				if(u>0 && v>0 && u+v<ss)	cs[3]=ca;
				else
				{
					register int sum=0;
					u = fabs(dx)-ss;	v = dy+ss/2;	v = v*v+(u<0?0:u*u);
					sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
					u = fabs(0.55*dx+0.83*dy)-0.9*ss;	v = 0.83*dx-0.55*dy+0.55*ss;	v = v*v+(u<0?0:u*u);
					sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
					u = fabs(0.55*dx-0.83*dy)-0.9*ss;	v = 0.83*dx+0.55*dy-0.55*ss;	v = v*v+(u<0?0:u*u);
					sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
					sum = sum>255?255:sum;	cs[3] = ca*sum/255;
				}
				pnt_plot(i,j,q.z+1,cs,oi);
			}
			break;
		case '^':
			for(long j=y1;j<=y2;j++)	for(long i=x1;i<=x2;i++)
			{
				register float dx=i-q.x, dy=j-q.y, v,u;
				register int sum=0;
				u = fabs(dx)-ss;	v = dy+ss/2;	v = v*v+(u<0?0:u*u);
				sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				u = fabs(0.55*dx+0.83*dy)-0.9*ss;	v = 0.83*dx-0.55*dy+0.55*ss;	v = v*v+(u<0?0:u*u);
				sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				u = fabs(0.55*dx-0.83*dy)-0.9*ss;	v = 0.83*dx+0.55*dy-0.55*ss;	v = v*v+(u<0?0:u*u);
				sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				sum = sum>255?255:sum;	cs[3] = ca*sum/255;
				pnt_plot(i,j,q.z+1,cs,oi);
			}
			break;
		case 'V':
			for(long j=y1;j<=y2;j++)	for(long i=x1;i<=x2;i++)
			{
				register float dx=i-q.x, dy=j-q.y, v,u;
				u=-dy/1.5+ss/3;	v=(dx+ss-u)/2;
				if(u>0 && v>0 && u+v<ss)	cs[3]=ca;
				else
				{
					register int sum=0;
					u = fabs(dx)-ss;	v = dy-ss/2;	v = v*v+(u<0?0:u*u);
					sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
					u = fabs(0.55*dx+0.83*dy)-0.9*ss;	v = 0.83*dx-0.55*dy-0.55*ss;	v = v*v+(u<0?0:u*u);
					sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
					u = fabs(0.55*dx-0.83*dy)-0.9*ss;	v = 0.83*dx+0.55*dy+0.55*ss;	v = v*v+(u<0?0:u*u);
					sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
					sum = sum>255?255:sum;	cs[3] = ca*sum/255;
				}
				pnt_plot(i,j,q.z+1,cs,oi);
			}
			break;
		case 'v':
			for(long j=y1;j<=y2;j++)	for(long i=x1;i<=x2;i++)
			{
				register float dx=i-q.x, dy=j-q.y, v,u;
				register int sum=0;
				u = fabs(dx)-ss;	v = dy-ss/2;	v = v*v+(u<0?0:u*u);
				sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				u = fabs(0.55*dx+0.83*dy)-0.9*ss;	v = 0.83*dx-0.55*dy-0.55*ss;	v = v*v+(u<0?0:u*u);
				sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				u = fabs(0.55*dx-0.83*dy)-0.9*ss;	v = 0.83*dx+0.55*dy+0.55*ss;	v = v*v+(u<0?0:u*u);
				sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				sum = sum>255?255:sum;	cs[3] = ca*sum/255;
				pnt_plot(i,j,q.z+1,cs,oi);
			}
			break;
		case 'L':
			for(long j=y1;j<=y2;j++)	for(long i=x1;i<=x2;i++)
			{
				register float dx=i-q.x, dy=j-q.y, v,u;
				u=-dx/1.5+ss/3;	v=(dy+ss-u)/2;
				if(u>0 && v>0 && u+v<ss)	cs[3]=ca;
				else
				{
					register int sum=0;
					u = fabs(dy)-ss;	v = dx-ss/2;	v = v*v+(u<0?0:u*u);
					sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
					u = fabs(0.55*dy+0.83*dx)-0.9*ss;	v = 0.83*dy-0.55*dx-0.55*ss;	v = v*v+(u<0?0:u*u);
					sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
					u = fabs(0.55*dy-0.83*dx)-0.9*ss;	v = 0.83*dy+0.55*dx+0.55*ss;	v = v*v+(u<0?0:u*u);
					sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
					sum = sum>255?255:sum;	cs[3] = ca*sum/255;
				}
				pnt_plot(i,j,q.z+1,cs,oi);
			}
			break;
		case '<':
			for(long j=y1;j<=y2;j++)	for(long i=x1;i<=x2;i++)
			{
				register float dx=i-q.x, dy=j-q.y, v,u;
				register int sum=0;
				u = fabs(dy)-ss;	v = dx-ss/2;	v = v*v+(u<0?0:u*u);
				sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				u = fabs(0.55*dy+0.83*dx)-0.9*ss;	v = 0.83*dy-0.55*dx-0.55*ss;	v = v*v+(u<0?0:u*u);
				sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				u = fabs(0.55*dy-0.83*dx)-0.9*ss;	v = 0.83*dy+0.55*dx+0.55*ss;	v = v*v+(u<0?0:u*u);
				sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				sum = sum>255?255:sum;	cs[3] = ca*sum/255;
				pnt_plot(i,j,q.z+1,cs,oi);
			}
			break;
		case 'R':
			for(long j=y1;j<=y2;j++)	for(long i=x1;i<=x2;i++)
			{
				register float dx=i-q.x, dy=j-q.y, v,u;
				u=dx/1.5+ss/3;	v=(dy+ss-u)/2;
				if(u>0 && v>0 && u+v<ss)	cs[3]=ca;
				else
				{
					register int sum=0;
					u = fabs(dy)-ss;	v = dx+ss/2;	v = v*v+(u<0?0:u*u);
					sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
					u = fabs(0.55*dy+0.83*dx)-0.9*ss;	v = 0.83*dy-0.55*dx+0.55*ss;	v = v*v+(u<0?0:u*u);
					sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
					u = fabs(0.55*dy-0.83*dx)-0.9*ss;	v = 0.83*dy+0.55*dx-0.55*ss;	v = v*v+(u<0?0:u*u);
					sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
					sum = sum>255?255:sum;	cs[3] = ca*sum/255;
				}
				pnt_plot(i,j,q.z+1,cs,oi);
			}
			break;
		case '>':
			for(long j=y1;j<=y2;j++)	for(long i=x1;i<=x2;i++)
			{
				register float dx=i-q.x, dy=j-q.y, v,u;
				register int sum=0;
				u = fabs(dy)-ss;	v = dx+ss/2;	v = v*v+(u<0?0:u*u);
				sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				u = fabs(0.55*dy+0.83*dx)-0.9*ss;	v = 0.83*dy-0.55*dx+0.55*ss;	v = v*v+(u<0?0:u*u);
				sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				u = fabs(0.55*dy-0.83*dx)-0.9*ss;	v = 0.83*dy+0.55*dx-0.55*ss;	v = v*v+(u<0?0:u*u);
				sum += v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				sum = sum>255?255:sum;	cs[3] = ca*sum/255;
				pnt_plot(i,j,q.z+1,cs,oi);
			}
			break;
		case 'O':
			for(long j=y1;j<=y2;j++)	for(long i=x1;i<=x2;i++)
			{
				register float dx=i-q.x, dy=j-q.y, v;
				v = hypot(dx,dy)-ss;	v=v<0?0:v*v;
				register int sum = v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				cs[3] = ca*sum/255;
				pnt_plot(i,j,q.z+1,cs,oi);
			}
			break;
		case 'o':
			for(long j=y1;j<=y2;j++)	for(long i=x1;i<=x2;i++)
			{
				register float dx=i-q.x, dy=j-q.y, v;
				v = hypot(dx,dy)-ss;	v=v*v;
				register int sum = v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				cs[3] = ca*sum/255;
				pnt_plot(i,j,q.z+1,cs,oi);
			}
			break;
		case 'C':
			for(long j=y1;j<=y2;j++)	for(long i=x1;i<=x2;i++)
			{
				register float dx=i-q.x, dy=j-q.y, v;
				v = hypot(dx,dy)-ss;	v=v*v;
				register int sum = v<V ? 255 : mgl_sline(255,dpw*(sqrt(v)+S));
				v = dx*dx+dy*dy;
				sum += v<(2*pw-1)*(2*pw-1)/4 ? 255 : mgl_sline(255,dpw*(sqrt(v)+(1-2*pw)/2));
				sum = sum>255?255:sum;	cs[3] = ca*sum/255;
				pnt_plot(i,j,q.z+1,cs,oi);
			}
			break;
		}
	}
}
//-----------------------------------------------------------------------------
void mglCanvas::mark_pix(long i, long j, const mglPnt &q, char type, mreal size, mglDrawReg *d)
{
	unsigned char cs[4];	col2int(q,cs,d->ObjId);	cs[3] = size>0 ? 255 : 255*q.t;
	mglPnt p0=q,p1=q,p2=q,p3=q;
	mreal ss=fabs(size);

	if(type=='.' || ss==0)
	{
		if(d)	d->PenWidth = ss?ss:sqrt(font_factor/400);
		pnt_pix(i,j,q,d);
	}
	else
	{
		if(d)
		{
			d->PDef = MGL_SOLID_MASK;	d->angle = 0;
			d->PenWidth*=fabs(50*size);
			if(d->PenWidth<1)	d->PenWidth=1;
		}
		if(!strchr("xsSoO",type))	ss *= 1.1;
		switch(type)
		{
		case 'P':
			p0.x = q.x-ss;	p0.y = q.y-ss;	p1.x = q.x+ss;	p1.y = q.y-ss;
			p2.x = q.x+ss;	p2.y = q.y+ss;	p3.x = q.x-ss;	p3.y = q.y+ss;
			line_pix(i,j,p0,p1,d);	line_pix(i,j,p1,p2,d);
			line_pix(i,j,p2,p3,d);	line_pix(i,j,p3,p0,d);
		case '+':
			p0.x = q.x-ss;	p0.y = q.y;	p1.x = q.x+ss;	p1.y = q.y;	line_pix(i,j,p0,p1,d);
			p2.x = q.x;	p2.y = q.y-ss;	p3.x = q.x;	p3.y = q.y+ss;	line_pix(i,j,p2,p3,d);
			break;
		case 'X':
			p0.x = q.x-ss;	p0.y = q.y-ss;	p1.x = q.x+ss;	p1.y = q.y-ss;
			p2.x = q.x+ss;	p2.y = q.y+ss;	p3.x = q.x-ss;	p3.y = q.y+ss;
			line_pix(i,j,p0,p1,d);	line_pix(i,j,p1,p2,d);
			line_pix(i,j,p2,p3,d);	line_pix(i,j,p3,p0,d);
		case 'x':
			p0.x = q.x-ss;	p0.y = q.y-ss;	p1.x = q.x+ss;	p1.y = q.y+ss;	line_pix(i,j,p0,p1,d);
			p2.x = q.x+ss;	p2.y = q.y-ss;	p3.x = q.x-ss;	p3.y = q.y+ss;	line_pix(i,j,p2,p3,d);
			break;
		case 'S':
			p0.x = q.x-ss;	p0.y = q.y-ss;	p1.x = q.x-ss;	p1.y = q.y+ss;
			p2.x= q.x+ss;	p2.y= q.y+ss;	p3.x = q.x+ss;	p3.y = q.y-ss;
			quad_pix(i,j,p0,p1,p3,p2,d);
		case 's':
			p0.x = q.x-ss;	p0.y = q.y-ss;	p1.x = q.x+ss;	p1.y = q.y-ss;
			p2.x = q.x+ss;	p2.y = q.y+ss;	p3.x = q.x-ss;	p3.y = q.y+ss;
			line_pix(i,j,p0,p1,d);	line_pix(i,j,p1,p2,d);
			line_pix(i,j,p2,p3,d);	line_pix(i,j,p3,p0,d);
			break;
		case 'D':
			p0.x = q.x;	p0.y = q.y-ss;	p1.x = q.x+ss;	p1.y = q.y;
			p2.x= q.x;	p2.y= q.y+ss;	p3.x = q.x-ss;	p3.y = q.y;
			quad_pix(i,j,p0,p1,p3,p2,d);
		case 'd':
			p0.x = q.x;	p0.y = q.y-ss;	p1.x = q.x+ss;	p1.y = q.y;
			p2.x = q.x;	p2.y = q.y+ss;	p3.x = q.x-ss;	p3.y = q.y;
			line_pix(i,j,p0,p1,d);	line_pix(i,j,p1,p2,d);
			line_pix(i,j,p2,p3,d);	line_pix(i,j,p3,p0,d);
			break;
		case 'Y':
			p1.x = q.x;	p1.y = q.y-ss;	line_pix(i,j,q,p1,d);
			p2.x = q.x-0.8*ss;	p2.y = q.y+0.6*ss;	line_pix(i,j,q,p2,d);
			p3.x = q.x+0.8*ss;	p3.y = q.y+0.6*ss;	line_pix(i,j,q,p3,d);
			break;
		case '*':
			p0.x = q.x-ss;		p0.y = q.y;
			p1.x = q.x+ss;		p1.y = q.y;	line_pix(i,j,p0,p1,d);
			p0.x = q.x-0.6*ss;	p0.y = q.y-0.8*ss;
			p1.x = q.x+0.6*ss;	p1.y = q.y+0.8*ss;	line_pix(i,j,p0,p1,d);
			p0.x = q.x-0.6*ss;	p0.y = q.y+0.8*ss;
			p1.x = q.x+0.6*ss;	p1.y = q.y-0.8*ss;	line_pix(i,j,p0,p1,d);
			break;
		case 'T':
			p0.x = q.x-ss;	p0.y = q.y-ss/2;
			p1.x = q.x+ss;	p1.y = q.y-ss/2;
			p2.x= q.x;		p2.y= q.y+ss;
			trig_pix(i,j,p0,p1,p2,false,d);
		case '^':
			p0.x = q.x-ss;	p0.y = q.y-ss/2;
			p1.x = q.x+ss;	p1.y = q.y-ss/2;
			p2.x= q.x;		p2.y= q.y+ss;
			line_pix(i,j,p0,p1,d);	line_pix(i,j,p1,p2,d);
			line_pix(i,j,p2,p0,d);	break;
		case 'V':
			p0.x = q.x-ss;	p0.y = q.y+ss/2;
			p1.x = q.x+ss;	p1.y = q.y+ss/2;
			p2.x= q.x;		p2.y= q.y-ss;
			trig_pix(i,j,p0,p1,p2,false,d);
		case 'v':
			p0.x = q.x-ss;	p0.y = q.y+ss/2;
			p1.x = q.x+ss;	p1.y = q.y+ss/2;
			p2.x= q.x;		p2.y= q.y-ss;
			line_pix(i,j,p0,p1,d);	line_pix(i,j,p1,p2,d);
			line_pix(i,j,p2,p0,d);	break;
		case 'L':
			p0.x = q.x+ss/2;	p0.y = q.y+ss;
			p1.x = q.x+ss/2;	p1.y = q.y-ss;
			p2.x= q.x-ss;		p2.y= q.y;
			trig_pix(i,j,p0,p1,p2,false,d);
		case '<':
			p0.x = q.x+ss/2;	p0.y = q.y+ss;
			p1.x = q.x+ss/2;	p1.y = q.y-ss;
			p2.x= q.x-ss;		p2.y= q.y;
			line_pix(i,j,p0,p1,d);	line_pix(i,j,p1,p2,d);
			line_pix(i,j,p2,p0,d);	break;
		case 'R':
			p0.x = q.x-ss/2;	p0.y = q.y+ss;
			p1.x = q.x-ss/2;	p1.y = q.y-ss;
			p2.x= q.x+ss;		p2.y= q.y;
			trig_pix(i,j,p0,p1,p2,false,d);
		case '>':
			p0.x = q.x-ss/2;	p0.y = q.y+ss;
			p1.x = q.x-ss/2;	p1.y = q.y-ss;
			p2.x= q.x+ss;		p2.y= q.y;
			line_pix(i,j,p0,p1,d);	line_pix(i,j,p1,p2,d);
			line_pix(i,j,p2,p0,d);	break;
		case 'O':
			{
				register float xx = (i-q.x), yy = (j-q.y);
				register float dz = Width>2 ? 1 : 1e-5*Width;		// provide additional height to be well visible on the surfaces
				if(xx*xx+yy*yy<ss*ss)	pnt_plot(i,j,q.z+dz,cs,d->ObjId);
			}
		case 'o':
			{
				register float pw=d->PenWidth;
				register float xx = (i-q.x), yy = (j-q.y), v = hypot(xx,yy);
				v = (v-ss)*(v-ss);
//				if(v>pw*pw)	return;
				if(v>(pw-1)*(pw-1)/4)	cs[3] = mgl_sline(cs[3],2*(sqrt(v)+(1-pw)/2));
				register float dz = Width>2 ? 1 : 1e-5*Width;		// provide additional height to be well visible on the surfaces
				pnt_plot(i,j,q.z+dz,cs,d->ObjId);
			}
			break;
		case 'C':
			pnt_pix(i,j,q,d);
			{
				register float pw=d->PenWidth;
				register float xx = (i-q.x), yy = (j-q.y), v = hypot(xx,yy);
				v = (v-ss)*(v-ss);
//				if(v>pw*pw)	return;
				if(v>(pw-1)*(pw-1)/4)	cs[3] = mgl_sline(cs[3],2*(sqrt(v)+(1-pw)/2));
				register float dz = Width>2 ? 1 : 1e-5*Width;		// provide additional height to be well visible on the surfaces
				pnt_plot(i,j,q.z+dz,cs,d->ObjId);
			}
			break;
		}
	}
}
//-----------------------------------------------------------------------------
// scale direction for new view/zoom
float mglCanvas::GetGlyphPhi(const mglPnt &q, float phi)
{
	float x,y,z,ll;
	if(q.sub<0)
	{	x = q.u;	y = q.v;	z = q.w;	}
	else
	{
		x = Bp.b[0]*q.u + Bp.b[1]*q.v + Bp.b[2]*q.w;
		y = Bp.b[3]*q.u + Bp.b[4]*q.v + Bp.b[5]*q.w;
		z = Bp.b[6]*q.u + Bp.b[7]*q.v + Bp.b[8]*q.w;

		register float dv= get_persp(Bp.pf,q.z,Depth);
		register float c = get_pfact(Bp.pf,Depth);
		x += (q.x-Width/2)*z*c*dv;
		y += (q.y-Height/2)*z*c*dv;
	}
	ll = x*x+y*y;
	if(ll < 1e-10)	return NAN;
	if(ll==ll && phi<1e4)
	{
		phi = -atan2(y,x)*180/M_PI;
//		if(fabs(phi)>90) 	phi+=180;	// NOTE this is 2nd part of rotation changes (see also text_plot())
	}
	else phi=0;
	return phi;
}
//-----------------------------------------------------------------------------
void mglCanvas::glyph_draw(const mglPrim &P, mglDrawReg *d)
{
	float phi = GetGlyphPhi(Pnt[P.n2],P.w);
	if(mgl_isnan(phi))	return;

	if(d)	{	d->PDef = MGL_SOLID_MASK;	d->angle = 0;	d->PenWidth=0.6;	}
	mglPnt p=Pnt[P.n1];
	// NOTE check this later for mglInPlot
	mreal fact = get_persp(Bp.pf,p.z,Depth);
	mreal pf=p.sub<0?1:sqrt((Bp.b[0]*Bp.b[0]+Bp.b[1]*Bp.b[1]+Bp.b[3]*Bp.b[3]+Bp.b[4]*Bp.b[4])/2)*fact, f = P.p*pf;

	mglMatrix M;
	M.b[0] = M.b[4] = M.b[8] = P.s;
	M.RotateN(phi,0,0,1);
	M.x=p.x;	M.y=p.y;	M.z=p.z;	M.pf = 1;
	p.u *= pf;	p.v *= pf;

	const mglGlyph &g = Glf[P.n4];
	if(P.n3&8)
	{
		if(!(P.n3&4))	glyph_line(&M,p,f,true, d);
		glyph_line(&M,p,f,false, d);
	}
	else
	{
		if(!(P.n3&4))	glyph_fill(&M,p,f,g, d);
		glyph_wire(&M,p,f,g, d);
	}
}
//-----------------------------------------------------------------------------
void mglCanvas::glyph_fill(const mglMatrix *M, const mglPnt &pp, mreal f, const mglGlyph &g, const mglDrawReg *d)
{
	if(!g.trig || g.nt<=0)	return;
	mglPnt q0=pp, q1=pp, q2=pp;
	q0.u=q0.v=q1.u=q1.v=q2.u=q2.v=NAN;
	for(long ik=0;ik<g.nt;ik++)
	{
		register long ii = 6*ik;	mglPoint p;
		p.Set(f*g.trig[ii]+pp.u,f*g.trig[ii+1]+pp.v,0);
		PostScale(M,p);	q0.x = p.x;	q0.y = p.y;	q0.z = p.z;
		ii+=2;	p.Set(f*g.trig[ii]+pp.u,f*g.trig[ii+1]+pp.v,0);
		PostScale(M,p);	q1.x = p.x;	q1.y = p.y;	q1.z = p.z;
		ii+=2;	p.Set(f*g.trig[ii]+pp.u,f*g.trig[ii+1]+pp.v,0);
		PostScale(M,p);	q2.x = p.x;	q2.y = p.y;	q2.z = p.z;
		trig_draw(q0,q1,q2,false,d);
	}
}
//-----------------------------------------------------------------------------
void mglCanvas::glyph_wire(const mglMatrix *M, const mglPnt &pp, mreal f, const mglGlyph &g, const mglDrawReg *d)
{
	if(!g.line || g.nl<=0)	return;
	long il=0;
	mglPnt q0=pp, q1=pp;	q0.u=q0.v=q1.u=q1.v=NAN;
	mglPoint p1,p2;
	for(long ik=0;ik<g.nl;ik++)
	{
		register long ii = 2*ik;
		if(g.line[ii]==0x3fff && g.line[ii+1]==0x3fff)	// line breakthrough
		{	il = ik+1;	continue;	}
		else if(ik==g.nl-1 || (g.line[ii+2]==0x3fff && g.line[ii+3]==0x3fff))
		{	// enclose the circle. May be in future this block should be commented
			p1.Set(f*g.line[ii]+pp.u,f*g.line[ii+1]+pp.v,0);	ii=2*il;
			p2.Set(f*g.line[ii]+pp.u,f*g.line[ii+1]+pp.v,0);
		}
		else
		{	// normal line
			p1.Set(f*g.line[ii]+pp.u,f*g.line[ii+1]+pp.v,0);	ii+=2;
			p2.Set(f*g.line[ii]+pp.u,f*g.line[ii+1]+pp.v,0);
		}
		PostScale(M,p1);	PostScale(M,p2);
		q0.x = p1.x;	q0.y = p1.y;	q0.z = p1.z;
		q1.x = p2.x;	q1.y = p2.y;	q1.z = p2.z;
		line_draw(q0,q1,d);
	}
}
//-----------------------------------------------------------------------------
void mglCanvas::glyph_line(const mglMatrix *M, const mglPnt &pp, mreal f, bool solid, const mglDrawReg *d)
{
	mglPnt q0=pp,q1=pp,q2=pp,q3=pp;
	q0.u=q0.v=q1.u=q1.v=q2.u=q2.v=q3.u=q3.v=NAN;
	mglPoint p1,p2,p3,p4;

	mreal dy = 0.004;
	p1.Set(pp.u,pp.v-dy,0);	PostScale(M,p1);
	p2.Set(pp.u,pp.v+dy,0);	PostScale(M,p2);
	p3.Set(fabs(f)+pp.u,pp.v+dy,0);	PostScale(M,p3);
	p4.Set(fabs(f)+pp.u,pp.v-dy,0);	PostScale(M,p4);

	q0.x = p1.x;	q0.y = p1.y;	q0.z = p1.z;
	q1.x = p2.x;	q1.y = p2.y;	q1.z = p2.z;
	q2.x = p3.x;	q2.y = p3.y;	q2.z = p3.z;
	q3.x = p4.x;	q3.y = p4.y;	q3.z = p4.z;

	if(solid)	quad_draw(q0,q1,q3,q2,d);
	else
	{
		line_draw(q0,q1,d);	line_draw(q2,q1,d);
		line_draw(q0,q3,d);	line_draw(q2,q3,d);
	}
}
//-----------------------------------------------------------------------------
void mglCanvas::glyph_pix(long i, long j, const mglPrim &P, mglDrawReg *d)
{
	float phi = GetGlyphPhi(Pnt[P.n2],P.w);
	if(mgl_isnan(phi))	return;

	if(d)	{	d->PDef = MGL_SOLID_MASK;	d->angle = 0;	d->PenWidth=1;	}
	mglPnt p=Pnt[P.n1];
	mreal pf=sqrt((Bp.b[0]*Bp.b[0]+Bp.b[1]*Bp.b[1]+Bp.b[3]*Bp.b[3]+Bp.b[4]*Bp.b[4])/2), f = P.p*pf;

	mglMatrix M;
	M.b[0] = M.b[4] = M.b[8] = P.s;
	M.RotateN(phi,0,0,1);
	M.x=p.x;	M.y=p.y;	M.z=p.z;	M.pf = 1;
	p.u *= pf;	p.v *= pf;

	const mglGlyph &g = Glf[P.n4];
	if(P.n3&8)
	{
		if(!(P.n3&4))	glyph_lpix(i,j,&M,p,f,true, d);
		glyph_lpix(i,j,&M,p,f,false, d);
	}
	else
	{
		if(!(P.n3&4))	glyph_fpix(i,j,&M,p,f,g, d);
		glyph_wpix(i,j,&M,p,f,g, d);
	}
}
//-----------------------------------------------------------------------------
void mglCanvas::glyph_fpix(long i, long j, const mglMatrix *M, const mglPnt &pp, mreal f, const mglGlyph &g, const mglDrawReg *d)
{
	if(!g.trig || g.nt<=0)	return;
	mglPnt q0=pp, q1=pp, q2=pp;
	q0.u=q0.v=q1.u=q1.v=q2.u=q2.v=NAN;
	for(long ik=0;ik<g.nt;ik++)
	{
		register long ii = 6*ik;	mglPoint p;
		p.Set(f*g.trig[ii]+pp.u,f*g.trig[ii+1]+pp.v,0);
		PostScale(M,p);	q0.x = p.x;	q0.y = p.y;	q0.z = p.z;
		ii+=2;	p.Set(f*g.trig[ii]+pp.u,f*g.trig[ii+1]+pp.v,0);
		PostScale(M,p);	q1.x = p.x;	q1.y = p.y;	q1.z = p.z;
		ii+=2;	p.Set(f*g.trig[ii]+pp.u,f*g.trig[ii+1]+pp.v,0);
		PostScale(M,p);	q2.x = p.x;	q2.y = p.y;	q2.z = p.z;
		trig_pix(i,j,q0,q1,q2,false,d);
	}
}
//-----------------------------------------------------------------------------
void mglCanvas::glyph_wpix(long i, long j, const mglMatrix *M, const mglPnt &pp, mreal f, const mglGlyph &g, const mglDrawReg *d)
{
	if(!g.line || g.nl<=0)	return;
	long il=0;
	mglPnt q0=pp, q1=pp;	q0.u=q0.v=q1.u=q1.v=NAN;
	mglPoint p1,p2;
	for(long ik=0;ik<g.nl;ik++)
	{
		register long ii = 2*ik;
		if(g.line[ii]==0x3fff && g.line[ii+1]==0x3fff)	// line breakthrough
		{	il = ik+1;	continue;	}
		else if(ik==g.nl-1 || (g.line[ii+2]==0x3fff && g.line[ii+3]==0x3fff))
		{	// enclose the circle. May be in future this block should be commented
			p1.Set(f*g.line[ii]+pp.u,f*g.line[ii+1]+pp.v,0);	ii=2*il;
			p2.Set(f*g.line[ii]+pp.u,f*g.line[ii+1]+pp.v,0);
		}
		else
		{	// normal line
			p1.Set(f*g.line[ii]+pp.u,f*g.line[ii+1]+pp.v,0);	ii+=2;
			p2.Set(f*g.line[ii]+pp.u,f*g.line[ii+1]+pp.v,0);
		}
		PostScale(M,p1);	PostScale(M,p2);
		q0.x = p1.x;	q0.y = p1.y;	q0.z = p1.z;
		q1.x = p2.x;	q1.y = p2.y;	q1.z = p2.z;
		line_pix(i,j,q0,q1,d);
	}
}
//-----------------------------------------------------------------------------
void mglCanvas::glyph_lpix(long i, long j, const mglMatrix *M, const mglPnt &pp, mreal f, bool solid, const mglDrawReg *d)
{
	mglPnt q0=pp,q1=pp,q2=pp,q3=pp;
	q0.u=q0.v=q1.u=q1.v=q2.u=q2.v=q3.u=q3.v=NAN;
	mglPoint p1,p2,p3,p4;

	mreal dy = 0.004;
	p1.Set(pp.u,pp.v-dy,0);	PostScale(M,p1);
	p2.Set(pp.u,pp.v+dy,0);	PostScale(M,p2);
	p3.Set(fabs(f)+pp.u,pp.v+dy,0);	PostScale(M,p3);
	p4.Set(fabs(f)+pp.u,pp.v-dy,0);	PostScale(M,p4);

	q0.x = p1.x;	q0.y = p1.y;	q0.z = p1.z;
	q1.x = p2.x;	q1.y = p2.y;	q1.z = p2.z;
	q2.x = p3.x;	q2.y = p3.y;	q2.z = p3.z;
	q3.x = p4.x;	q3.y = p4.y;	q3.z = p4.z;

	if(solid)	quad_pix(i,j,q0,q1,q3,q2,d);
	else
	{
		line_pix(i,j,q0,q1,d);	line_pix(i,j,q2,q1,d);
		line_pix(i,j,q0,q3,d);	line_pix(i,j,q2,q3,d);
	}
}
//-----------------------------------------------------------------------------
long mglCanvas::setPp(mglPnt &q, const mglPoint &p)
{
	q.xx=q.x=p.x;	q.yy=q.y=p.y;	q.zz=q.z=p.z;
	long k;
#pragma omp critical(pnt)
	{k=Pnt.size();	MGL_PUSH(Pnt,q,mutexPnt);}
	return k;
}
//-----------------------------------------------------------------------------
void mglCanvas::arrow_draw(long n1, long n2, char st, float ll)
{
	const mglPnt &p1=Pnt[n1], &p2=Pnt[n2];
	mglPnt q=p1; 	//q.u=q.v=q.w=0;

	mglPoint kl(p1.x-p2.x,p1.y-p2.y,p1.z-p2.z), kt, p0(p1.x,p1.y,p1.z), p;
	mreal d = hypot(kl.x,kl.y);
	if(d==0)	return;
	kl /= d;	kt = !kl;
	kl *= ll;	kt *= ll;

	Reserve(8);
	long k1,k2,k3,k4;

	switch(st)	// S,D -- cube, T -- sq.pyramid, I -- square, O -- sphere???, A,K,V -- cone???
	{
		case 'I':
			k1=setPp(q,p0+kt);	k2=setPp(q,p0-kt);	line_plot(k1,k2);	break;
		case 'D':
			k1=setPp(q,p0+kl);	k2=setPp(q,p0-kl);	k3=setPp(q,p0+kt);	k4=setPp(q,p0-kt);
			trig_plot(k1,k2,k3);	trig_plot(k1,k2,k4);	break;
		case 'S':
			k1=setPp(q,p0+kl+kt);	k2=setPp(q,p0+kl-kt);
			k3=setPp(q,p0-kl-kt);	k4=setPp(q,p0-kl+kt);
			quad_plot(k1,k2,k4,k3);	break;
		case 'X':
			k1=setPp(q,p0+kl+kt);	k2=setPp(q,p0+kl-kt);
			k3=setPp(q,p0-kl-kt);	k4=setPp(q,p0-kl+kt);
			line_plot(k1,k3);	line_plot(k2,k4);	break;
		case 'T':
			k1=setPp(q,p0-kl+kt);	k2=setPp(q,p0-kl-kt);	k3=setPp(q,p0+kl);
			trig_plot(k1,k2,k3);	break;
		case 'K':
			k1=setPp(q,p0+kt);	k2=setPp(q,p0-kt);	line_plot(k1,k2);
		case 'A':
			k1=setPp(q,p0-2.*kl+kt);	k2=setPp(q,p0-2.*kl-kt);	k3=setPp(q,p0-1.5*kl);
			trig_plot(n1,k3,k1);	trig_plot(n1,k3,k2);	break;
		case 'V':
			k1=setPp(q,p0+2.*kl+kt);	k2=setPp(q,p0+2.*kl-kt);	k3=setPp(q,p0+1.5*kl);
			trig_plot(n1,k3,k1);	trig_plot(n1,k3,k2);	break;
		case 'O':	// let draw icosahedron
		{
			const int n = 12;	k1=setPp(q,p0+kl);
			for(int i=1;i<=n;i++)
			{
				mreal u = 2*i*M_PI/n;
				k2 = k1;	k1 = setPp(q,p0+kl*cos(u)+kt*sin(u));
				trig_plot(n1,k1,k2);
			}
			break;
		}
	}
}
//-----------------------------------------------------------------------------
void mglCanvas::arrow_plot_3d(long n1, long n2, char st, float ll)
{
	const mglPnt &p1=Pnt[n1], &p2=Pnt[n2];
	mglPnt q=p1; 	//q.u=q.v=q.w=0;

	mglPoint kl(p1.x-p2.x,p1.y-p2.y,p1.z-p2.z), kt, kz, p0(p1.x,p1.y,p1.z), p;
	if(kl.norm()==0)	return;
	kl.Normalize();	kt = !kl;	kz = kl^kt;
	kl *= ll;	kt *= ll;	kz *= ll;

	Reserve(8);
	long k1,k2,k3,k4,k5, k6,k7,k8;

	switch(st)	// S,D -- cube, T -- sq.pyramid, I -- square, O -- sphere???, A,K,V -- cone???
	{
		case 'I':
			k1=setPp(q,p0+kt);	k2=setPp(q,p0+kz);
			k3=setPp(q,p0-kt);	k4=setPp(q,p0-kz);
			quad_plot(k1,k2,k4,k3);	break;
		case 'D':
			k1=setPp(q,p0+kl);	k2=setPp(q,p0-kl);	k5=k3=setPp(q,p0+kt);
			k4=setPp(q,p0+kz);	trig_plot(k1,k3,k4);	trig_plot(k2,k3,k4);	k3=k4;
			k4=setPp(q,p0-kt);	trig_plot(k1,k3,k4);	trig_plot(k2,k3,k4);	k3=k4;
			k4=setPp(q,p0-kz);	trig_plot(k1,k3,k4);	trig_plot(k2,k3,k4);	k3=k4;
			trig_plot(k1,k3,k5);	trig_plot(k2,k3,k5);	break;
		case 'S':
			k1=setPp(q,p0+kl+kt);	k2=setPp(q,p0+kl+kz);	k3=setPp(q,p0+kl-kt);	k4=setPp(q,p0+kl-kz);
			k5=setPp(q,p0-kl+kt);	k6=setPp(q,p0-kl+kz);	k7=setPp(q,p0-kl-kt);	k8=setPp(q,p0-kl-kz);
			quad_plot(k1,k2,k4,k3);	quad_plot(k1,k2,k5,k6);	quad_plot(k3,k2,k7,k6);
			quad_plot(k1,k4,k5,k8);	quad_plot(k3,k4,k7,k8);	quad_plot(k5,k6,k8,k7);	break;
		case 'X':
			k1=setPp(q,p0+kl+kt);	k2=setPp(q,p0+kl+kz);	k3=setPp(q,p0+kl-kt);	k4=setPp(q,p0+kl-kz);
			k5=setPp(q,p0-kl+kt);	k6=setPp(q,p0-kl+kz);	k7=setPp(q,p0-kl-kt);	k8=setPp(q,p0-kl-kz);
			line_plot(k1,k7);	line_plot(k2,k8);	line_plot(k3,k5);	line_plot(k4,k6);	break;
		case 'T':
			k1=setPp(q,p0-kl+kt);	k2=setPp(q,p0-kl+kz);	k3=setPp(q,p0-kl-kt);
			k4=setPp(q,p0-kl-kz);	k5=setPp(q,p0+kl);
			trig_plot(k1,k2,k5);	trig_plot(k2,k3,k5);
			trig_plot(k3,k4,k5);	trig_plot(k1,k4,k5);	break;
		case 'K':
			k1=setPp(q,p0+kt);	k2=setPp(q,p0+kz);
			k3=setPp(q,p0-kt);	k4=setPp(q,p0-kz);	quad_plot(k1,k2,k4,k3);	break;
		case 'A':
			k1=setPp(q,p0-2.*kl+kt);	k2=setPp(q,p0-2.*kl+kz);	k3=setPp(q,p0-2.*kl-kt);
			k4=setPp(q,p0-2.*kl-kz);	k5=setPp(q,p0-1.5*kl);
			trig_plot(n1,k5,k1);	trig_plot(n1,k5,k2);
			trig_plot(n1,k5,k3);	trig_plot(n1,k5,k4);	break;
		case 'V':
			k1=setPp(q,p0+2.*kl+kt);	k2=setPp(q,p0+2.*kl+kz);	k3=setPp(q,p0+2.*kl-kt);
			k4=setPp(q,p0+2.*kl-kz);	k5=setPp(q,p0+1.5*kl);
			trig_plot(n1,k5,k1);	trig_plot(n1,k5,k2);
			trig_plot(n1,k5,k3);	trig_plot(n1,k5,k4);	break;
		case 'O':	// let draw icosahedron
		{
			const int n = 12, m = n/2;	Reserve(n*m);
			register long i,j;
			long *nn=new long[2*n], n1=setPp(q,p0+kl), n2=setPp(q,p0-kl);
			mreal u,v,rr;
			for(i=0;i<m;i++)	for(j=0;j<n;j++)
			{
				if(i>0 && i<m-1)
				{
					u = i*M_PI/(m-1.);	v = 2*M_PI*j/(n-1.)-1;	rr = sin(u);
					nn[j+n]=nn[j];	nn[j]=setPp(q,p0+kl*cos(u)+kt*rr*cos(v)+kz*rr*sin(v));
				}
				else if(i==0)	nn[j] = n1;
				else if(i==m-1)	{	nn[j+n]=nn[j];	nn[j]=n2;	}
				if(i*j>0)	quad_plot(nn[j-1], nn[j], nn[j+n-1], nn[j+n]);
			}
			delete []nn;	break;
		}
	}
}
//-----------------------------------------------------------------------------
