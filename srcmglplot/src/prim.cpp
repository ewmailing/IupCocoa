/***************************************************************************
 * prim.cpp is part of Math Graphic Library
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
#include "mgl2/canvas.h"
#include "mgl2/prim.h"
#include "mgl2/data.h"
std::wstring MGL_EXPORT mgl_ftoa(double v, const char *fmt);
//-----------------------------------------------------------------------------
//
//	Mark & Curve series
//
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_mark(HMGL gr, double x, double y, double z,const char *mark)
{
	char mk = gr->SetPenPal(mark);
	if(!mk)	mk = '.';
	if(mgl_isnan(z))	z=2*gr->Max.z-gr->Min.z;
	static int cgid=1;	gr->StartGroup("MarkS",cgid++);
	long k = gr->AddPnt(mglPoint(x,y,z),gr->CDef,mglPoint(NAN),-1,3);
	gr->mark_plot(k,mk,gr->GetPenWidth()); 	gr->AddActive(k);
	gr->EndGroup();
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_mark_(uintptr_t *gr, mreal *x, mreal *y, mreal *z, const char *pen,int l)
{	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	mgl_mark(_GR_, *x,*y,*z,s);	delete []s;	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_ball(HMGL gr, double x, double y, double z)
{
	static int cgid=1;	gr->StartGroup("Ball",cgid++);
	if(mgl_isnan(z))	z=2*gr->Max.z-gr->Min.z;
	long k = gr->AddPnt(mglPoint(x,y,z),gr->AddTexture('r'),mglPoint(NAN),-1,3);
	gr->mark_plot(k,'.');	gr->AddActive(k);
	gr->EndGroup();
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_ball_(uintptr_t *gr, mreal *x, mreal *y, mreal *z)
{	mgl_ball(_GR_, *x,*y,*z);	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_line(HMGL gr, double x1, double y1, double z1, double x2, double y2, double z2, const char *pen,int n)
{
	static int cgid=1;	gr->StartGroup("Line",cgid++);
	if(mgl_isnan(z1) || mgl_isnan(z2))	z1=z2=2*gr->Max.z-gr->Min.z;
	mglPoint p1(x1,y1,z1), p2(x2,y2,z2), p=p1,nn(NAN);
	gr->SetPenPal(pen);
	n = (n<2) ? 2 : n;

	gr->Reserve(n);
	long k1 = gr->AddPnt(p,gr->CDef,nn,-1,3);	gr->AddActive(k1);
	for(long i=1;i<n;i++)
	{
		mreal s = i/mreal(n-1);	p = p1*(1-s)+p2*s;
		long k2 = k1;
		k1 = gr->AddPnt(p,gr->CDef,nn,-1,3);
		gr->line_plot(k2,k1);
		if(i==1)	gr->arrow_plot(k2,k1,gr->Arrow1);
		if(i==n-1)	gr->arrow_plot(k1,k2,gr->Arrow2);
	}
	gr->AddActive(k1,1);	gr->EndGroup();
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_line_(uintptr_t *gr, mreal *x1, mreal *y1, mreal *z1, mreal *x2, mreal *y2, mreal *z2, const char *pen,int *n,int l)
{	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	mgl_line(_GR_, *x1,*y1,*z1, *x2,*y2,*z2,s,*n);	delete []s;	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_curve(HMGL gr, double x1, double y1, double z1, double dx1, double dy1, double dz1, double x2, double y2, double z2, double dx2, double dy2, double dz2, const char *pen,int n)
{
	static int cgid=1;	gr->StartGroup("Curve",cgid++);
	if(mgl_isnan(z1) || mgl_isnan(z2))	z1=z2=2*gr->Max.z-gr->Min.z;
	const mglPoint p1(x1,y1,z1), p2(x2,y2,z2),nn(NAN);
	const mglPoint d1(3*dx1,3*dy1,3*dz1), d2(3*dx2,3*dy2,3*dz2);	// NOTE use d->3*d to be exact as Bezier curve
	mglPoint a,b,p=p1;
	a = 3*(p2-p1)-d2-2*d1;	b = d1+d2-2*(p2-p1);
	n = (n<2) ? 2 : n;
	gr->SetPenPal(pen);

	gr->Reserve(n);
	long k1=gr->AddPnt(p,gr->CDef,nn,-1,3);	gr->AddActive(k1);
	for(long i=1;i<n;i++)
	{
		mreal s = i/(n-1.);	p = p1+s*(d1+s*(a+s*b));
		long k2 = k1;
		k1 = gr->AddPnt(p,gr->CDef,nn,-1,3);
		gr->line_plot(k2,k1);
		if(i==1)	gr->arrow_plot(k2,k1,gr->Arrow1);
		if(i==n-1)	gr->arrow_plot(k1,k2,gr->Arrow2);
	}
	gr->AddActive(gr->AddPnt(p1+d1/3,gr->CDef,nn,-1,3),1);
	gr->AddActive(gr->AddPnt(p2-d2/3,gr->CDef,nn,-1,3),3);
	gr->AddActive(k1,2);	gr->EndGroup();
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_curve_(uintptr_t* gr, mreal *x1, mreal *y1, mreal *z1, mreal *dx1, mreal *dy1, mreal *dz1, mreal *x2, mreal *y2, mreal *z2, mreal *dx2, mreal *dy2, mreal *dz2, const char *pen,int *n, int l)
{	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	mgl_curve(_GR_, *x1,*y1,*z1, *dx1,*dy1,*dz1, *x2,*y2,*z2, *dx2,*dy2,*dz2, s, *n);	delete []s;}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_error_box(HMGL gr, double x, double y, double z, double ex, double ey, double ez, const char *pen)
{
	static int cgid=1;	gr->StartGroup("ErBox",cgid++);
	char mk=gr->SetPenPal(pen);
	mglPoint p(x,y,z), q,nn(NAN);
	gr->Reserve(7);
	long k1,k2;
	q = p;	q.x += ex;	k1 = gr->AddPnt(q,gr->CDef,nn,0,3);
	q = p;	q.x -= ex;	k2 = gr->AddPnt(q,gr->CDef,nn,0,3);
	gr->line_plot(k1,k2);	gr->arrow_plot(k1,k2,'I');	gr->arrow_plot(k2,k1,'I');
	q = p;	q.y += ey;	k1 = gr->AddPnt(q,gr->CDef,nn,0,3);
	q = p;	q.y -= ey;	k2 = gr->AddPnt(q,gr->CDef,nn,0,3);
	gr->line_plot(k1,k2);	gr->arrow_plot(k1,k2,'I');	gr->arrow_plot(k2,k1,'I');
	q = p;	q.z += ez;	k1 = gr->AddPnt(q,gr->CDef,nn,0,3);
	q = p;	q.z -= ez;	k2 = gr->AddPnt(q,gr->CDef,nn,0,3);
	gr->line_plot(k1,k2);	gr->arrow_plot(k1,k2,'I');	gr->arrow_plot(k2,k1,'I');
	if(mk)	gr->mark_plot(gr->AddPnt(p,gr->CDef,nn,0,3),mk);
	gr->AddActive(gr->AddPnt(p,gr->CDef,nn,-1,3),0);
	gr->AddActive(gr->AddPnt(p+mglPoint(ex,ey),gr->CDef,nn,-1,3),1);
	gr->EndGroup();
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_error_box_(uintptr_t *gr, mreal *x1, mreal *y1, mreal *z1, mreal *x2, mreal *y2, mreal *z2, const char *pen,int l)
{	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	mgl_error_box(_GR_, *x1,*y1,*z1, *x2,*y2,*z2,s);	delete []s;	}
//-----------------------------------------------------------------------------
//
//	Face series
//
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_face(HMGL gr, double x0, double y0, double z0, double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3, const char *stl)
{
	static int cgid=1;	gr->StartGroup("Face",cgid++);
	long pal;
	gr->SetPenPal(stl,&pal);
//	mreal c1,c2,c3,c4,zz=(gr->Min.z+gr->Max.z)/2;
	mreal c1,c2,c3,c4,zz=2*gr->Max.z-gr->Min.z;
	c1=c2=c3=c4=gr->CDef;
	if(mgl_isnan(z0))	z0 = zz;	if(mgl_isnan(z1))	z1 = zz;
	if(mgl_isnan(z2))	z2 = zz;	if(mgl_isnan(z3))	z3 = zz;
	mglPoint p1(x0,y0,z0), p2(x1,y1,z1), p3(x2,y2,z2), p4(x3,y3,z3);
	if(gr->GetNumPal(pal)>=4)
	{	c2=gr->NextColor(pal,1);	c3=gr->NextColor(pal,2);	c4=gr->NextColor(pal,3);	}
	mglPoint q1,q2,q3,q4;
	q1 = (p2-p1)^(p3-p1);	q4 = (p2-p4)^(p3-p4);
	q2 = (p1-p2)^(p4-p2);	q3 = (p1-p3)^(p4-p3);
	gr->Reserve(4);
	long k1,k2,k3,k4;
	double a = gr->get(MGL_ENABLE_ALPHA)?-1:1;
	k1 = gr->AddPnt(p1,c1,q1,a,11);	gr->AddActive(k1,0);
	k2 = gr->AddPnt(p2,c2,q2,a,11);	gr->AddActive(k2,1);
	k3 = gr->AddPnt(p3,c3,q3,a,11);	gr->AddActive(k3,2);
	k4 = gr->AddPnt(p4,c4,q4,a,11);	gr->AddActive(k4,3);
	gr->quad_plot(k1,k2,k3,k4);
	if(mglchr(stl,'#'))
	{
		gr->Reserve(4);
		pal = gr->AddTexture('k');
		k1=gr->CopyNtoC(k1,pal);	k2=gr->CopyNtoC(k2,pal);
		k3=gr->CopyNtoC(k3,pal);	k4=gr->CopyNtoC(k4,pal);
		gr->line_plot(k1,k2);		gr->line_plot(k1,k3);
		gr->line_plot(k3,k4);		gr->line_plot(k2,k4);
	}
	gr->EndGroup();
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_facex(HMGL gr, double x0, double y0, double z0, double wy, double wz, const char *stl, double d1, double d2)
{	mgl_face(gr, x0,y0,z0, x0,y0+wy,z0, x0,y0,z0+wz, x0,y0+wy+d1,z0+wz+d2, stl);	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_facey(HMGL gr, double x0, double y0, double z0, double wx, double wz, const char *stl, double d1, double d2)
{	mgl_face(gr, x0,y0,z0, x0+wx,y0,z0, x0,y0,z0+wz, x0+wx+d1,y0,z0+wz+d2, stl);	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_facez(HMGL gr, double x0, double y0, double z0, double wx, double wy, const char *stl, double d1, double d2)
{	mgl_face(gr, x0,y0,z0, x0,y0+wy,z0, x0+wx,y0,z0, x0+wx+d1,y0+wy+d2,z0, stl);	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_facex_(uintptr_t* gr, mreal *x0, mreal *y0, mreal *z0, mreal *wy, mreal *wz, const char *stl, mreal *dx, mreal *dy, int l)
{	char *s=new char[l+1];	memcpy(s,stl,l);	s[l]=0;
	mgl_facex(_GR_, *x0,*y0,*z0,*wy,*wz,s,*dx,*dy);	delete []s;
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_facey_(uintptr_t* gr, mreal *x0, mreal *y0, mreal *z0, mreal *wx, mreal *wz, const char *stl, mreal *dx, mreal *dy, int l)
{	char *s=new char[l+1];	memcpy(s,stl,l);	s[l]=0;
	mgl_facey(_GR_, *x0,*y0,*z0,*wx,*wz,s,*dx,*dy);	delete []s;	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_facez_(uintptr_t* gr, mreal *x0, mreal *y0, mreal *z0, mreal *wx, mreal *wy, const char *stl, mreal *dx, mreal *dy, int l)
{	char *s=new char[l+1];	memcpy(s,stl,l);	s[l]=0;
	mgl_facez(_GR_, *x0,*y0,*z0,*wx,*wy,s,*dx,*dy);	delete []s;	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_face_(uintptr_t* gr, mreal *x0, mreal *y0, mreal *z0, mreal *x1, mreal *y1, mreal *z1, mreal *x2, mreal *y2, mreal *z2, mreal *x3, mreal *y3, mreal *z3, const char *stl, int l)
{	char *s=new char[l+1];	memcpy(s,stl,l);	s[l]=0;
	mgl_face(_GR_, *x0,*y0,*z0, *x1,*y1,*z1, *x2,*y2,*z2, *x3,*y3,*z3, stl);	delete []s;	}
//-----------------------------------------------------------------------------
//
//	Cone
//
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_cone(HMGL gr, double x1, double y1, double z1, double x2, double y2, double z2, double r1, double r2, const char *stl)
{
	if(r2<0)	r2=r1;
	if(r1==0 && r2==0)	return;

	static int cgid=1;	gr->StartGroup("Cone",cgid++);
	mglPoint p1(x1,y1,z1), p2(x2,y2,z2), p,q(NAN,NAN),t(NAN,NAN), d=p2-p1,a,b;
	a=!d;	a.Normalize();		b=d^a;	b.Normalize();
	mreal c1,c2,dr=r2-r1;
	const char *s;
	if((s=strstr(stl,"{&"))!=0)	c1 = c2 = atof(s+2);
	else
	{
		register long ss=gr->AddTexture(stl);
		c1=gr->GetC(ss,p1.z);
		c2=gr->GetC(ss,p2.z);
	}
	long *kk=new long[164],k1=-1,k2=-1;
	bool edge = mglchr(stl,'@');
	bool wire = mglchr(stl,'#');
	gr->Reserve(edge?166:82);
	if(edge && !wire)
	{
		k1=gr->AddPnt(p1,c1,d,-1,3);
		k2=gr->AddPnt(p2,c2,d,-1,3);
	}
	long n=wire?6:18;	//wire?6:18;
	if(mglchr(stl,'4'))	n=2;
	else if(mglchr(stl,'6'))	n=3;
	else if(mglchr(stl,'8'))	n=4;
	bool refr = n>6;
	if(refr)	t=d;

	for(long i=0;i<2*n+1;i++)
	{
		register int f = n!=4?(2*i+1)*90/n:45*i;
		register mreal co = mgl_cos[f%360], si = mgl_cos[(f+270)%360];
		p = p1+(r1*co)*a+(r1*si)*b;
		if(refr)	q = (si*a-co*b)^(d + (dr*co)*a + (dr*si)*b);
		kk[i] = gr->AddPnt(p,c1,q,-1,3);
		if(edge && !wire)	kk[i+82] = gr->AddPnt(p,c1,t,-1,3);
		p = p2+(r2*co)*a+(r2*si)*b;
		kk[i+2*n+1] = gr->AddPnt(p,c2,q,-1,3);
		if(edge && !wire)	kk[i+123] = gr->AddPnt(p,c2,t,-1,3);
	}
	if(wire)	for(long i=0;i<2*n;i++)
	{
		gr->line_plot(kk[i],kk[i+1]);
		gr->line_plot(kk[i],kk[i+2*n+1]);
		gr->line_plot(kk[i+2*n+2],kk[i+1]);
		gr->line_plot(kk[i+2*n+2],kk[i+2*n+1]);
	}
	else	for(long i=0;i<2*n;i++)
	{
		gr->quad_plot(kk[i],kk[i+1],kk[i+2*n+1],kk[i+2*n+2]);
		if(edge)
		{
			gr->trig_plot(k1,kk[i+82],kk[i+83]);
			gr->trig_plot(k2,kk[i+123],kk[i+124]);
		}
	}
	gr->EndGroup();	delete []kk;
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_cone_(uintptr_t* gr, mreal *x1, mreal *y1, mreal *z1, mreal *x2, mreal *y2, mreal *z2, mreal *r1, mreal *r2, const char *stl, int l)
{	char *s=new char[l+1];	memcpy(s,stl,l);	s[l]=0;
	mgl_cone(_GR_, *x1,*y1,*z1, *x2,*y2,*z2,*r1,*r2,s);	delete []s;	}
//-----------------------------------------------------------------------------
//
//	Bars series
//
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_cones_xyz(HMGL gr, HCDT x, HCDT y, HCDT z, const char *pen, const char *opt)
{
	long m,n=z->GetNx(),nx=x->GetNx(), nz=z->GetNy(), pal;
	if(mgl_check_dim1(gr,x,z,y,0,"Cones",true))	return;

	gr->SaveState(opt);
	static int cgid=1;	gr->StartGroup("Cones",cgid++);
	m = x->GetNy() > y->GetNy() ? x->GetNy() : y->GetNy();	m = nz > m ? nz : m;

	bool above= mglchr(pen,'a');
	bool wire = mglchr(pen,'#');
	bool tube = mglchr(pen,'t');
	mreal *dd=new mreal[2*n], x1,z0,zz,d, vx,vy,vz,v0,v1,dv=nx>n?1:0;
	if(mglchr(pen,'<'))	dv = 1;
	if(mglchr(pen,'^'))	dv = 0;
	if(mglchr(pen,'>'))	dv = -1;

	gr->SetPenPal(pen,&pal);
	std::string cb="@";
	if(wire)	cb += '#';
	if(mglchr(pen,'4'))	cb+='4';
	else if(mglchr(pen,'6'))	cb+='6';
	else if(mglchr(pen,'8'))	cb+='8';

	memset(dd,0,2*n*sizeof(mreal));
	z0 = gr->GetOrgZ('x');
	for(long i=0;i<n;i++)	for(long j=0;j<m;j++)	dd[i] += z->v(i, j<nz ? j:0);

	char buf[64];
	for(long j=0;j<m;j++)
	{
		if(gr->NeedStop())	break;
		sprintf(buf,"{&%g}",gr->NextColor(pal));
		std::string c1=cb+buf, c2=c1;
		if(gr->GetNumPal(pal)==2*m)
		{	sprintf(buf,"{&%g}",gr->NextColor(pal));	c2 = cb+buf;	}
		long mx = j<x->GetNy() ? j:0, my = j<y->GetNy() ? j:0, mz = j<nz ? j:0;
		for(long i=0;i<n;i++)
		{
			vx=x->v(i,mx);	vy=y->v(i,my);	vz=z->v(i,mz);
			v0=y->v(i,0);	v1=i<nx-1 ? x->v(i+1,mx):x->v(i-1,mx);
			d = i<nx-1 ? v1-vx : vx-v1;
			x1 = vx + d/2*(dv-0.*gr->BarWidth);
			d *= 0.7*gr->BarWidth;
			if(above)
			{
				zz = j>0?dd[i+n]:z0;	dd[i+n] += vz;
				mgl_cone(gr, x1,v0,zz, x1,v0,dd[i+n],
						 tube?d:d*(dd[i]-zz)/(dd[i]-z0),
						 tube?d:d*(dd[i]-dd[i+n])/(dd[i]-z0), c1.c_str());
			}
			else	mgl_cone(gr, x1,vy,z0, x1,vy,vz, d,tube?d:0, vz<0?c1.c_str():c2.c_str());
		}
	}
	gr->EndGroup();	delete []dd;
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_cones_xz(HMGL gr, HCDT x, HCDT z, const char *pen, const char *opt)
{
	gr->SaveState(opt);
	mglData y(z);
	y.Fill(gr->Min.y,gr->Max.y,'y');
	mgl_cones_xyz(gr,x,&y,z,pen,0);
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_cones(HMGL gr, HCDT z, const char *pen, const char *opt)
{
	gr->SaveState(opt);
	mglData x(z->GetNx()+1);
	x.Fill(gr->Min.x,gr->Max.x);
	mgl_cones_xz(gr,&x,z,pen,0);
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_cones_xyz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *pen, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_cones_xyz(_GR_,_DA_(x),_DA_(y),_DA_(z),s,o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_cones_xz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, const char *pen, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_cones_xz(_GR_,_DA_(x),_DA_(y),s,o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_cones_(uintptr_t *gr, uintptr_t *y,	const char *pen, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,pen,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_cones(_GR_,_DA_(y),s,o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
//
//	Ellipse & Rhomb
//
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_polygon(HMGL gr, double x1, double y1, double z1, double x2, double y2, double z2, int n, const char *stl)
{
	if(n<3)	return;
	long pal=0, n0,n1,n2,np,k1=-1,kp=-1;
	static int cgid=1;	gr->StartGroup("Polygon",cgid++);
	gr->SetPenPal(stl,&pal);
	mreal c=gr->NextColor(pal);
	mreal k=(gr->GetNumPal(pal)>1)?gr->NextColor(pal):gr->AddTexture('k');
	bool fill = !mglchr(stl,'#'), box = mglchr(stl,'@') || !fill;
	if(!fill)	k=c;
	gr->Reserve(box?2*n+1:n+1);
	if(mgl_isnan(z1) || mgl_isnan(z2))	z1=z2=2*gr->Max.z-gr->Min.z;
	mglPoint p1(x1,y1,z1), p2(x2,y2,z2), d=p2-p1, u=!d, p,qq;
	n0 = gr->AddPnt(p1,c,qq,-1,11);
	u = (d.norm()/u.norm())*u;
	n1 = np = gr->AddPnt(p2,c,qq,-1,11);
	gr->AddActive(n0,0);	gr->AddActive(n1,1);
	if(box) k1 = kp = gr->CopyNtoC(n1,k);
	for(long i=1;i<n;i++)
	{
		p = p1+d*cos(2*M_PI*i/n)+u*sin(2*M_PI*i/n);
		n2 = gr->AddPnt(p,c,qq,-1,11);
		if(fill)	gr->trig_plot(n0,n1,n2);
		if(box)
		{
			long kk = gr->CopyNtoC(n2,k);
			gr->line_plot(k1, kk);	k1 = kk;
		}
		n1 = n2;
	}
	if(fill)	gr->trig_plot(n0,n2,np);
	if(box)		gr->line_plot(k1, kp);
	gr->EndGroup();
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_arc_ext(HMGL gr, double x0, double y0, double z0, double xr, double yr, double zr, double x1, double y1, double z1, double a, const char* stl)
{
	long pal=0, n = long(fabs(a)/9+1.5);	a *= M_PI/180;
	static int cgid=1;	gr->StartGroup("Arc",cgid++);
	gr->SetPenPal(stl,&pal);
	mreal c=gr->NextColor(pal);
	gr->Reserve(n+2);
	if(mgl_isnan(z0) || mgl_isnan(z1))	z0=z1=2*gr->Max.z-gr->Min.z;
	mglPoint p0(x0,y0,z0), p1(x1,y1,z1), d=p1-p0, u(mglPoint(xr,yr,zr)^d), p,qq;
	if(u.norm()==0)	return;	// wrong vector orientation
	u = (d.norm()/u.norm())*u;
	gr->AddActive(gr->AddPnt(p0,gr->CDef,qq,-1,3),0);
	long n1 = gr->AddPnt(p1,c,qq,-1,11);	gr->AddActive(n1,1);
	for(long i=1;i<n;i++)
	{
		p = p0+d*cos(a*i/(n-1))+u*sin(a*i/(n-1));
		long n2 = gr->AddPnt(p,c,qq,-1,11);
		if(i==1)	gr->arrow_plot(n1,n2,gr->Arrow1);
		if(i==n-1)	{	gr->arrow_plot(n2,n1,gr->Arrow2);	gr->AddActive(n2,2);	}
		gr->line_plot(n1,n2);	n1 = n2;
	}
	gr->EndGroup();
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_arc(HMGL gr, double x0, double y0, double x1, double y1, double a, const char* stl)
{	mgl_arc_ext(gr,x0,y0,NAN,0,0,1,x1,y1,NAN,a,stl);	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_polygon_(uintptr_t* gr, mreal *x1, mreal *y1, mreal *z1, mreal *x2, mreal *y2, mreal *z2, int *n, const char *stl,int l)
{	char *s=new char[l+1];	memcpy(s,stl,l);	s[l]=0;
	mgl_polygon(_GR_,*x1,*y1,*z1,*x2,*y2,*z2,*n,s);	delete []s;	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_arc_ext_(uintptr_t* gr, mreal *x0, mreal *y0, mreal *z0, mreal *x1, mreal *y1, mreal *z1, mreal *x2, mreal *y2, mreal *z2, mreal *r, const char *stl,int l)
{	char *s=new char[l+1];	memcpy(s,stl,l);	s[l]=0;
	mgl_arc_ext(_GR_,*x0,*y0,*z0,*x1,*y1,*z1,*x2,*y2,*z2,*r,s);	delete []s;	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_arc_(uintptr_t* gr, mreal *x0, mreal *y0, mreal *x1, mreal *y1, mreal *a, const char *stl,int l)
{	char *s=new char[l+1];	memcpy(s,stl,l);	s[l]=0;
	mgl_arc(_GR_,*x0,*y0,*x1,*y1,*a,s);	delete []s;	}
//-----------------------------------------------------------------------------
//
//	Ellipse & Rhomb
//
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_ellipse(HMGL gr, double x1, double y1, double z1, double x2, double y2, double z2, double r, const char *stl)
{
	const int n = 41;
	long pal=0,n0,n1=-1,m1=-1;
	static int cgid=1;	gr->StartGroup("Ellipse",cgid++);
	gr->SetPenPal(stl,&pal);
	mreal c=gr->NextColor(pal), d;
	mreal k=(gr->GetNumPal(pal)>1)?gr->NextColor(pal):gr->AddTexture('k');
	bool fill = !mglchr(stl,'#'), box = mglchr(stl,'@') || !fill;
	if(!fill)	k=c;

	gr->Reserve(2*n+1);
	if(mgl_isnan(z1) || mgl_isnan(z2))	z1=z2=2*gr->Max.z-gr->Min.z;
	mglPoint p1(x1,y1,z1), p2(x2,y2,z2), v=p2-p1;
	d = v.norm();
	if(d==0)	v.Set(1);	else	v /= d;
	mglPoint u(mglPoint(0,0,1)^v), q(u^v), p, s=(p1+p2)/2.;
	u *= r;		v *= sqrt(d*d/4+r*r);
	// central point first
	n0 = gr->AddPnt(p1,c,q,-1,11);	gr->AddActive(n0);
	gr->AddActive(gr->AddPnt(p2,c,q,-1,11),1);
	for(long i=0;i<n;i++)
	{
		register int t=i*360/(n-1);
		p = s+v*mgl_cos[t%360]+u*mgl_cos[(270+t)%360];
		long n2 = n1;	n1 = gr->AddPnt(p,c,q,-1,11);
		if(i==n/4)	gr->AddActive(n1,2);
		long m2 = m1;	m1 = gr->CopyNtoC(n1,k);
		if(i>0)
		{
			if(fill)	gr->trig_plot(n0,n1,n2);
			if(box)	gr->line_plot(m1,m2);
		}
	}
	gr->EndGroup();
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_rhomb(HMGL gr, double x1, double y1, double z1, double x2, double y2, double z2, double r, const char *stl)
{
	long pal=0, n1,n2,n3,n4;
	static int cgid=1;	gr->StartGroup("Rhomb",cgid++);
	gr->SetPenPal(stl,&pal);
	mreal c=gr->NextColor(pal);
	mreal k=(gr->GetNumPal(pal)>1)?gr->NextColor(pal):gr->AddTexture('k');
	mreal b=(gr->GetNumPal(pal)>2)?gr->NextColor(pal):c;
	bool fill = !mglchr(stl,'#'), box = mglchr(stl,'@') || !fill;
	if(!fill)	k=c;
	gr->Reserve(8);
	if(mgl_isnan(z1) || mgl_isnan(z2))	z1=z2=2*gr->Max.z-gr->Min.z;
	mglPoint p1(x1,y1,z1), p2(x2,y2,z2), u=!(p1-p2), p, s,qq;
	u = (r/u.norm())*u;	s = (p1+p2)/2.;
	p = p1;		n1 = gr->AddPnt(p,c,qq,-1,11);
	p = s+u;	n2 = gr->AddPnt(p,b==c?c:k,qq,-1,11);
	p = p2;		n3 = gr->AddPnt(p,b,qq,-1,11);
	p = s-u;	n4 = gr->AddPnt(p,b==c?c:k,qq,-1,11);
	gr->AddActive(n1,0);	gr->AddActive(n2,2);	gr->AddActive(n3,1);
	if(fill)	gr->quad_plot(n1,n2,n4,n3);
	n1 = gr->CopyNtoC(n1,k);	n2 = gr->CopyNtoC(n2,k);
	n3 = gr->CopyNtoC(n3,k);	n4 = gr->CopyNtoC(n4,k);
	if(box)
	{	gr->line_plot(n1,n2);	gr->line_plot(n2,n3);
		gr->line_plot(n3,n4);	gr->line_plot(n4,n1);	}
	gr->EndGroup();
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_ellipse_(uintptr_t* gr, mreal *x1, mreal *y1, mreal *z1, mreal *x2, mreal *y2, mreal *z2, mreal *r, const char *stl,int l)
{	char *s=new char[l+1];	memcpy(s,stl,l);	s[l]=0;
	mgl_ellipse(_GR_,*x1,*y1,*z1,*x2,*y2,*z2,*r,s);	delete []s;	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_rhomb_(uintptr_t* gr, mreal *x1, mreal *y1, mreal *z1, mreal *x2, mreal *y2, mreal *z2, mreal *r, const char *stl,int l)
{	char *s=new char[l+1];	memcpy(s,stl,l);	s[l]=0;
	mgl_rhomb(_GR_,*x1,*y1,*z1,*x2,*y2,*z2,*r,s);	delete []s;	}
//-----------------------------------------------------------------------------
//
//	Sphere & Drop
//
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_sphere(HMGL gr, double x, double y, double z, double r, const char *stl)
{	mgl_drop(gr,x,y,z,1,0,0,2*r,stl,0,1);	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_sphere_(uintptr_t* gr, mreal *x, mreal *y, mreal *z, mreal *r, const char *stl,int l)
{	char *s=new char[l+1];	memcpy(s,stl,l);	s[l]=0;
	mgl_sphere(_GR_, *x,*y,*z,*r,s);	delete []s;	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_drop(HMGL gr, mglPoint p, mglPoint q, double r, double c, double sh, double a)
{
	mglPoint p1,p2,pp,qq;
	if(q.norm()==0)	{	q.Set(1,0,0);	sh=0;	}
	q.Normalize();	p1 = !q;	p2 = q^p1;	r /= 2;

	static int cgid=1;	gr->StartGroup("Drop",cgid++);
	const int m=12, n=2*m+1;
	gr->Reserve(n*m);
	long *nn=new long[2*n],n1,n2;
	mreal x,y,z,rr,dr;

	z = r*(1+sh)*(1+sh);	n1 = gr->AddPnt(p + q*z,c,q,-1,3);
	z = r*(1+sh)*(sh-1);	n2 = gr->AddPnt(p + q*z,c,q,-1,3);

	for(long i=0;i<=m;i++)	for(long j=0;j<n;j++)	// NOTE use prev.points => not for omp
	{
		if(i>0 && i<m)
		{
			register int u=i*180/m, v=180*j/m+202;
			register float co=mgl_cos[u%360], si=mgl_cos[(u+270)%360];
			register float cv=mgl_cos[v%360], sv=mgl_cos[(v+270)%360];
			rr = r*a*si*(1.+sh*co)/(1+sh);
			dr = r*a/(1+sh)*(co*(1.+sh*co) - sh*si*si);
			x = rr*cv;	y = rr*sv;
			z = r*(1+sh)*(co+sh);
			pp = p + p1*x + p2*y + q*z;
			qq = (p1*sv-p2*cv)^(p1*(dr*cv) + p2*(dr*sv) - q*(r*(1+sh)*si));
			nn[j+n]=nn[j];	nn[j]=gr->AddPnt(pp,c,qq,-1,3);
		}
		else if(i==0)	nn[j] = n1;
		else if(i==m)	{	nn[j+n]=nn[j];	nn[j]=n2;	}
		if(i*j>0)	gr->quad_plot(nn[j-1], nn[j], nn[j+n-1], nn[j+n]);
	}
	delete []nn;	gr->EndGroup();
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_drop(HMGL gr, double x1, double y1, double z1, double x2, double y2, double z2, double r, const char *stl, double sh, double a)
{
	mreal c=gr->AddTexture((stl && stl[0]) ? stl[0]:'r');
	mgl_drop(gr,mglPoint(x1,y1,z1), mglPoint(x2,y2,z2), r, c, sh, a);
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_drop_(uintptr_t* gr, mreal *x1, mreal *y1, mreal *z1, mreal *x2, mreal *y2, mreal *z2, mreal *r, const char *stl, mreal *shift, mreal *ap, int l)
{	char *s=new char[l+1];	memcpy(s,stl,l);	s[l]=0;
	mgl_drop(_GR_, *x1,*y1,*z1, *x2,*y2,*z2, *r,s,*shift,*ap);	delete []s;	}
//-----------------------------------------------------------------------------
//
//	Dew series
//
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_dew_xy(HMGL gr, HCDT x, HCDT y, HCDT ax, HCDT ay, const char *sch, const char *opt)
{
	long n=ax->GetNx(),m=ax->GetNy();
	if(mgl_check_dim2(gr,x,y,ax,ay,"Dew"))	return;

	gr->SaveState(opt);
	static int cgid=1;	gr->StartGroup("DewXY",cgid++);

	long ss = gr->AddTexture(sch);
	bool inv = mglchr(sch,'i');
	mreal zVal = gr->Min.z, xm=0;
	long tx=1,ty=1;
	if(gr->MeshNum>1)	{	tx=(n-1)/(gr->MeshNum-1);	ty=(m-1)/(gr->MeshNum-1);	}
	if(tx<1)	tx=1;	if(ty<1)	ty=1;

	for(long k=0;k<ax->GetNz();k++)	for(long j=0;j<m;j++)	for(long i=0;i<n;i++)
	{
		register mreal ym = sqrt(ax->v(i,j,k)*ax->v(i,j,k)+ay->v(i,j,k)*ay->v(i,j,k));
		xm = xm>ym ? xm : ym;
	}
	xm = 1./MGL_FEPSILON/(xm==0 ? 1:xm);

	for(long k=0;k<ax->GetNz();k++)
	{
		if(ax->GetNz()>1)	zVal = gr->Min.z+(gr->Max.z-gr->Min.z)*mreal(k)/(ax->GetNz()-1);
		for(long i=0;i<n;i+=tx)	for(long j=0;j<m;j+=ty)
		{
			if(gr->NeedStop())	{	gr->EndGroup();	return;	}
			register mreal xx=GetX(x,i,j,k).x, yy=GetY(y,i,j,k).x, dd;
			register mreal dx = i<n-1 ? (GetX(x,i+1,j,k).x-xx) : (xx-GetX(x,i-1,j,k).x);
			register mreal dy = j<m-1 ? (GetY(y,i,j+1,k).x-yy) : (yy-GetY(y,i,j-1,k).x);
			dx *= tx;	dy *= ty;

			mglPoint q(ax->v(i,j,k),ay->v(i,j,k));	dd = q.norm();
			if(inv)	q = -q;
			mgl_drop(gr,mglPoint(xx, yy, zVal),q,(dx<dy?dx:dy)/2,gr->GetC(ss,dd*xm,false),dd*xm,1);
		}
	}
	gr->EndGroup();
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_dew_2d(HMGL gr, HCDT ax, HCDT ay, const char *sch, const char *opt)
{
	gr->SaveState(opt);
	mglData x(ax->GetNx()), y(ax->GetNy());
	x.Fill(gr->Min.x,gr->Max.x);
	y.Fill(gr->Min.y,gr->Max.y);
	mgl_dew_xy(gr,&x,&y,ax,ay,sch,0);
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_dew_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *ax, uintptr_t *ay, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_dew_xy(_GR_, _DA_(x), _DA_(y), _DA_(ax), _DA_(ay), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_dew_2d_(uintptr_t *gr, uintptr_t *ax, uintptr_t *ay, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_dew_2d(_GR_, _DA_(ax), _DA_(ay), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
//
//	Puts series
//
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_puts(HMGL gr, double x, double y, double z,const char *text, const char *font, double size)
{	mgl_puts_dir(gr, x, y, z, NAN, NAN, 0, text, font, size);	}
void MGL_EXPORT mgl_putsw(HMGL gr, double x, double y, double z,const wchar_t *text, const char *font, double size)
{	mgl_putsw_dir(gr, x, y, z, NAN, NAN, 0, text, font, size);	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_puts_dir(HMGL gr, double x, double y, double z, double dx, double dy, double dz, const char *text, const char *font, double size)
{
	MGL_TO_WCS(text,mgl_putsw_dir(gr, x, y, z, dx, dy, dz, wcs, font, size));
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_putsw_dir(HMGL gr, double x, double y, double z, double dx, double dy, double dz, const wchar_t *text, const char *font, double size)
{
	bool a=mglchr(font,'a'), A=mglchr(font,'A');
	static int cgid=1;	gr->StartGroup("Puts",cgid++);
	mglCanvas *g = dynamic_cast<mglCanvas *>(gr);
	if(g && (a||A))
	{
		g->Push();	g->Identity(a);
		gr->set(MGL_DISABLE_SCALE);
		register mreal s=a?1:g->GetPlotFactor();
		x = (2*x-1)*s;	y = (2*y-1)*s;
		dx= (2*dx-1)*s;	dy= (2*dy-1)*s;
	}
	if(mgl_isnan(z))	z=2*gr->Max.z-gr->Min.z;
	mglPoint p(x,y,z), d(dx-x,dy-y,dz-z);
	long k = gr->AddPnt(p,-1,d,-1,7);
	gr->AddActive(k,0);
	gr->AddActive(gr->AddPnt(mglPoint(dx,dy,dz),-1,d,-1,7),1);
	if(g && (a||A))	{	g->Pop();	gr->clr(MGL_DISABLE_SCALE);	}
	gr->text_plot(k,text,font,size);
	gr->EndGroup();
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_puts_(uintptr_t *gr, mreal *x, mreal *y, mreal *z,const char *text, const char *font, mreal *size, int l, int n)
{	wchar_t *s=new wchar_t[l+1];	mbstowcs(s,text,l);	s[l]=0;
	char *f=new char[n+1];	memcpy(f,font,n);	f[n]=0;
	mgl_putsw_dir(_GR_, *x, *y, *z, NAN, NAN, 0, s, f, *size);
	delete []s;	delete []f;	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_puts_dir_(uintptr_t *gr, mreal *x, mreal *y, mreal *z, mreal *dx, mreal *dy, mreal *dz, const char *text, const char *font, mreal *size, int l, int n)
{	wchar_t *s=new wchar_t[l+1];	mbstowcs(s,text,l);	s[l]=0;
	char *f=new char[n+1];	memcpy(f,font,n);	f[n]=0;
	mgl_putsw_dir(_GR_, *x, *y, *z, *dx, *dy, *dz, s, f, *size);
	delete []s;	delete []f;	}
//-----------------------------------------------------------------------------
//
//	TextMark series
//
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_textmarkw_xyzr(HMGL gr, HCDT x, HCDT y, HCDT z, HCDT r, const wchar_t *text, const char *fnt, const char *opt)
{
	long m,n=y->GetNx();
	if(mgl_check_dim0(gr,x,y,z,r,"TextMark"))	return;

	gr->SaveState(opt);
	static int cgid=1;	gr->StartGroup("TextMark",cgid++);
	m = x->GetNy() > y->GetNy() ? x->GetNy() : y->GetNy();
	m = z->GetNy() > m ? z->GetNy() : m;
	m = r->GetNy() > m ? r->GetNy() : m;
	gr->Reserve(n*m);

	mglPoint p,q(NAN);
	for(long j=0;j<m;j++)
	{
		if(gr->NeedStop())	break;
		long mx = j<x->GetNy() ? j:0, my = j<y->GetNy() ? j:0;
		long mz = j<z->GetNy() ? j:0, mr = j<r->GetNy() ? j:0;
		for(long i=0;i<n;i++)
		{
			p.Set(x->v(i,mx), y->v(i,my), z->v(i,mz));
			register long k = gr->AddPnt(p,-1,q);
			gr->text_plot(k, text, fnt, -0.5*fabs(r->v(i,mr)));
		}
	}
	gr->EndGroup();
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_textmarkw_xyr(HMGL gr, HCDT x, HCDT y, HCDT r, const wchar_t *text, const char *fnt, const char *opt)
{
	gr->SaveState(opt);
	mglData z(y->GetNx());	z.Fill(gr->Min.z,gr->Min.z);
	mgl_textmarkw_xyzr(gr,x,y,&z,r,text,fnt,0);
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_textmarkw_yr(HMGL gr, HCDT y, HCDT r, const wchar_t *text, const char *fnt, const char *opt)
{
	register long n=y->GetNx();
	gr->SaveState(opt);
	mglData x(n);	x.Fill(gr->Min.x,gr->Max.x);
	mglData z(n);	z.Fill(gr->Min.z,gr->Min.z);
	mgl_textmarkw_xyzr(gr,&x,y,&z,r,text,fnt,0);
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_textmarkw(HMGL gr, HCDT y, const wchar_t *text, const char *fnt, const char *opt)
{
	register long n=y->GetNx();
	gr->SaveState(opt);
	mglData r(n);	r.Fill(1,1);
	mglData x(n);	x.Fill(gr->Min.x,gr->Max.x);
	mglData z(n);	z.Fill(gr->Min.z,gr->Min.z);
	mgl_textmarkw_xyzr(gr,&x,y,&z,&r,text,fnt,0);
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_textmark_xyzr(HMGL gr, HCDT x, HCDT y, HCDT z, HCDT r, const char *str, const char *fnt, const char *opt)
{	MGL_TO_WCS(str,mgl_textmarkw_xyzr(gr, x, y, z, r, wcs, fnt, opt));	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_textmark_xyr(HMGL gr, HCDT x, HCDT y, HCDT r, const char *str, const char *fnt, const char *opt)
{	MGL_TO_WCS(str,mgl_textmarkw_xyr(gr, x, y, r, wcs, fnt, opt));	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_textmark_yr(HMGL gr, HCDT y, HCDT r, const char *str, const char *fnt, const char *opt)
{	MGL_TO_WCS(str,mgl_textmarkw_yr(gr, y, r, wcs, fnt, opt));	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_textmark(HMGL gr, HCDT y, const char *str, const char *fnt, const char *opt)
{	MGL_TO_WCS(str,mgl_textmarkw(gr, y, wcs, fnt, opt));	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_textmark_xyzr_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *r, const char *text, const char *fnt, const char *opt, int l,int n,int lo)
{	wchar_t *s=new wchar_t[l+1];	memcpy(s,text,l);	s[l]=0;
	char *f=new char[n+1];	memcpy(f,fnt,n);	f[n]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_textmarkw_xyzr(_GR_, _DA_(x),_DA_(y),_DA_(z),_DA_(r),s,f, o);
	delete []o;	delete []s;		delete []f;	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_textmark_xyr_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *r, const char *text, const char *fnt, const char *opt, int l,int n,int lo)
{	wchar_t *s=new wchar_t[l+1];	mbstowcs(s,text,l);	s[l]=0;
	char *f=new char[n+1];	memcpy(f,fnt,n);	f[n]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_textmarkw_xyr(_GR_, _DA_(x),_DA_(y),_DA_(r),s,f, o);
	delete []o;	delete []s;	delete []f;	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_textmark_yr_(uintptr_t *gr, uintptr_t *y, uintptr_t *r, const char *text, const char *fnt, const char *opt, int l,int n,int lo)
{	wchar_t *s=new wchar_t[l+1];	mbstowcs(s,text,l);	s[l]=0;
	char *f=new char[n+1];	memcpy(f,fnt,n);	f[n]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_textmarkw_yr(_GR_, _DA_(y),_DA_(r),s,f, o);	delete []o;	delete []s;	delete []f;	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_textmark_(uintptr_t *gr, uintptr_t *y, const char *text, const char *fnt, const char *opt, int l,int n,int lo)
{	wchar_t *s=new wchar_t[l+1];	mbstowcs(s,text,l);	s[l]=0;
	char *f=new char[n+1];	memcpy(f,fnt,n);	f[n]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_textmarkw(_GR_, _DA_(y),s,f, o);	delete []o;	delete []s;	delete []f;	}
//-----------------------------------------------------------------------------
//
//	Label series
//
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_labelw_xyz(HMGL gr, HCDT x, HCDT y, HCDT z, const wchar_t *text, const char *fnt, const char *opt)
{
	long m,n=y->GetNx();
	if(mgl_check_dim0(gr,x,y,z,0,"Label"))	return;

	gr->SaveState(opt);
	static int cgid=1;	gr->StartGroup("Label",cgid++);
	m = x->GetNy() > y->GetNy() ? x->GetNy() : y->GetNy();	m = z->GetNy() > m ? z->GetNy() : m;

	mglPoint q(NAN);

	char fmt[8]="2",ss[2]=" ";
	std::string Tstl;
	for(const char *s="0123456789";*s;s++)	if(mglchr(fnt,*s))	fmt[0] = *s;
	for(const char *s="f+E-F";*s;s++)	if(mglchr(fnt,*s))
	{	ss[0] = *s;	strcat(fmt,ss);	}
	for(long j=0;j<m;j++)
	{
		if(gr->NeedStop())	break;
		long mx = j<x->GetNy() ? j:0, my = j<y->GetNy() ? j:0, mz = j<z->GetNy() ? j:0;
		for(long i=0;i<n;i++)
		{
			mreal xx=x->v(i,mx), yy=y->v(i,my), zz=z->v(i,mz);
			register long kk = gr->AddPnt(mglPoint(xx,yy,zz),-1,q),k,l;
			std::wstring buf;
			for(k=l=0;text[k];k++)
			{
				if(text[k]!='%' || (k>0 && text[k-1]=='\\'))
				{	buf += text[k];	continue;	}
				else if(text[k+1]=='%')	buf+=L"%";
				else if(text[k+1]=='n')	{	wchar_t tmp[32];	mglprintf(tmp,32,L"%ld",i);	buf += tmp;	}
				else if(text[k+1]=='x')	buf += mgl_ftoa(xx,fmt);
				else if(text[k+1]=='y')	buf += mgl_ftoa(yy,fmt);
				else if(text[k+1]=='z')	buf += mgl_ftoa(zz,fmt);
				else {	buf+=L"%";	continue;	}
				k++;
			}
			gr->text_plot(kk, buf.c_str(), fnt, -0.7, 0.05);
		}
	}
	gr->EndGroup();
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_labelw_xy(HMGL gr, HCDT x, HCDT y, const wchar_t *text, const char *fnt, const char *opt)
{
	gr->SaveState(opt);
	mglData z(y->GetNx());	z.Fill(gr->Min.z,gr->Min.z);
	mgl_labelw_xyz(gr,x,y,&z,text,fnt,0);
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_labelw_y(HMGL gr, HCDT y, const wchar_t *text, const char *fnt, const char *opt)
{
	register long n=y->GetNx();
	if(n<2)	{	gr->SetWarn(mglWarnLow,"TextMark");	return;	}
	gr->SaveState(opt);
	mglData x(n);	x.Fill(gr->Min.x,gr->Max.x);
	mglData z(n);	z.Fill(gr->Min.z,gr->Min.z);
	mgl_labelw_xyz(gr,&x,y,&z,text,fnt,0);
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_label_xyz(HMGL gr, HCDT x, HCDT y, HCDT z, const char *str, const char *fnt, const char *opt)
{	MGL_TO_WCS(str,mgl_labelw_xyz(gr, x, y, z, wcs, fnt, opt));	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_label_xy(HMGL gr, HCDT x, HCDT y, const char *str, const char *fnt, const char *opt)
{	MGL_TO_WCS(str,mgl_labelw_xy(gr, x, y, wcs, fnt, opt));	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_label_y(HMGL gr, HCDT y, const char *str, const char *fnt, const char *opt)
{	MGL_TO_WCS(str,mgl_labelw_y(gr, y, wcs, fnt, opt));	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_label_xyz_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, const char *text, const char *fnt, const char *opt, int l,int n,int lo)
{	wchar_t *s=new wchar_t[l+1];	mbstowcs(s,text,l);	s[l]=0;
	char *f=new char[n+1];	memcpy(f,fnt,n);	f[n]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_labelw_xyz(_GR_, _DA_(x),_DA_(y),_DA_(z),s,f, o);
	delete []o;	delete []s;		delete []f;	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_label_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, const char *text, const char *fnt, const char *opt, int l,int n,int lo)
{	wchar_t *s=new wchar_t[l+1];	mbstowcs(s,text,l);	s[l]=0;
	char *f=new char[n+1];	memcpy(f,fnt,n);	f[n]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_labelw_xy(_GR_, _DA_(x),_DA_(y),s,f, o);
	delete []o;	delete []s;	delete []f;	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_label_y_(uintptr_t *gr, uintptr_t *y, const char *text, const char *fnt, const char *opt, int l,int n,int lo)
{	wchar_t *s=new wchar_t[l+1];	mbstowcs(s,text,l);	s[l]=0;
	char *f=new char[n+1];	memcpy(f,fnt,n);	f[n]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_labelw_y(_GR_, _DA_(y),s,f, o);	delete []o;	delete []s;	delete []f;	}
//-----------------------------------------------------------------------------
//
//	Table series
//
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_tablew(HMGL gr, double x, double y, HCDT val, const wchar_t *text, const char *fnt, const char *opt)
{
	mglCanvas *g = dynamic_cast<mglCanvas *>(gr);
	if(g)	g->Table(x,y,val,text,fnt,opt);
}
void MGL_EXPORT mgl_table(HMGL gr, double x, double y, HCDT val, const char *text, const char *fnt, const char *opt)
{
	if(!text)	mgl_tablew(gr,x,y,val,L"",fnt,opt);
	else	MGL_TO_WCS(text,mgl_tablew(gr, x, y, val, wcs, fnt, opt));
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_table_(uintptr_t *gr, mreal *x, mreal *y, uintptr_t *val, const char *text, const char *fnt, const char *opt,int l,int n,int lo)
{	wchar_t *s=new wchar_t[l+1];	mbstowcs(s,text,l);	s[l]=0;
	char *f=new char[n+1];	memcpy(f,fnt,n);	f[n]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_tablew(_GR_, *x, *y, _DA_(val),s,f, o);
	delete []o;	delete []s;	delete []f;	}
//-----------------------------------------------------------------------------
//
//	Logo series
//
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_logo(HMGL gr, long w, long h, const unsigned char *rgba, int smooth, const char *opt)
{
	if(w<1 || h<1 || !rgba)	{	gr->SetWarn(mglWarnLow,"Logo");	return;	}
	static int cgid=1;	gr->StartGroup("Logo",cgid++);
	mreal z = gr->SaveState(opt);
	if(mgl_isnan(z))	z = gr->Min.z;
	mreal x1 = gr->Min.x, y1 = gr->Min.y, dx, dy;
	if(!smooth || w<2 || h<2)
	{
		dx = (gr->Max.x-x1)/w;	dy = (gr->Max.y-y1)/h;
		gr->Reserve(4*(w+1)*(h+1));
		for(long j=0;j<h;j++)	for(long i=0;i<w;i++)
		{
			long i0 = 4*(i+w*(h-1-j)), k1,k2,k3,k4;
			mglColor c(rgba[i0]/255.,rgba[i0+1]/255.,rgba[i0+2]/255.);
			k1 = gr->AddPnt(mglPoint(x1+dx*i,y1+dy*j,z),0);	gr->SetRGBA(k1,c);
			k2 = gr->AddPnt(mglPoint(x1+dx*(i+1),y1+dy*j,z),0);	gr->SetRGBA(k2,c);
			k3 = gr->AddPnt(mglPoint(x1+dx*i,y1+dy*(j+1),z),0);	gr->SetRGBA(k3,c);
			k4 = gr->AddPnt(mglPoint(x1+dx*(i+1),y1+dy*(j+1),z),0);	gr->SetRGBA(k4,c);
			gr->quad_plot(k1,k2,k3,k4);
		}
	}
	else
	{
		dx = (gr->Max.x-x1)/(w-1);	dy = (gr->Max.y-y1)/(h-1);
		gr->Reserve(w*h);
		long *pos = new long[w*h];
		for(long j=0;j<h;j++)	for(long i=0;i<w;i++)
		{
			long i0 = 4*(i+w*(h-1-j)), i1 = i+w*j;
			pos[i1] = gr->AddPnt(mglPoint(x1+dx*i,y1+dy*j,z),0);
			gr->SetRGBA(pos[i1],mglColor(rgba[i0]/255.,rgba[i0+1]/255.,rgba[i0+2]/255.));
		}
		for(long j=0;j<h-1;j++)	for(long i=0;i<w-1;i++)
		{
			long i1 = i+w*j;
			gr->quad_plot(pos[i1],pos[i1+1],pos[i1+w],pos[i1+1+w]);
		}
		delete []pos;
	}
	gr->EndGroup();
}
//-----------------------------------------------------------------------------
bool MGL_NO_EXPORT mgl_read_image(unsigned char **g, int &w, int &h, const char *fname);
void MGL_EXPORT mgl_logo_file(HMGL gr, const char *fname, int smooth, const char *opt)
{
	unsigned char *g = 0;
	int w=0, h=0;
	if(!mgl_read_image(&g,w,h,fname))	return;
	mgl_logo(gr,w,h,g,smooth,opt);
	delete []g;
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_logo_file_(uintptr_t *gr, const char *fname, int *smooth, const char *opt,int l,int n)
{	char *s=new char[l+1];	memcpy(s,fname,l);	s[l]=0;
	char *f=new char[n+1];	memcpy(f,opt,n);	f[n]=0;
	mgl_logo_file(_GR_,s,*smooth,f);	delete []s;		delete []f;	}
//-----------------------------------------------------------------------------
//
//	Lamerey series
//
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_lamerey(HMGL gr, double x0, double (*f)(double,void *), void *par, const char *stl, const char *opt)
{
	static int cgid=1;	gr->StartGroup("Lamerey",cgid++);
	mreal r=gr->SaveState(opt);
	double x=x0, dx = 1e-5*fabs(gr->Max.x-gr->Min.x);
	long n = r>2 ? long(r+0.5):20, n1, n2;
	gr->SetPenPal(stl);	gr->Reserve(6*n+1);
	bool vect = mglchr(stl,'v');
	n2 = gr->AddPnt(mglPoint(x,x,gr->Max.z));
	if(!mglchr(stl,'~'))
	{
		n1 = gr->AddPnt(mglPoint(x,gr->GetOrgY('x'),gr->Max.z));
		gr->line_plot(n1,n2);	if(vect)	gr->vect_plot(n1,n2,0.3*gr->GetArrowSize());
	}
	for(long i=0;i<n;i++)
	{
		x0 = x;		x = f(x0,par);
		if(fabs(x-x0)<dx)	break;
		n1=n2;	n2 = gr->AddPnt(mglPoint(x0,x,gr->Max.z));
		gr->line_plot(n1,n2);	if(vect)	gr->vect_plot(n1,n2,0.3*gr->GetArrowSize());
		n1=n2;	n2 = gr->AddPnt(mglPoint(x,x,gr->Max.z));
		gr->line_plot(n1,n2);	if(vect)	gr->vect_plot(n1,n2,0.3*gr->GetArrowSize());
	}
	gr->EndGroup();
}
//-----------------------------------------------------------------------------
struct mglDatSpl	{	HCDT d;	double x0,dx;	double y0,dy;	};
double MGL_NO_EXPORT func_dat(double x, void *p)
{	mglDatSpl *s = (mglDatSpl *)p;	return s->d->value((x-s->x0)*s->dx);	}
void MGL_EXPORT mgl_lamerey_dat(HMGL gr, double x0, HCDT f, const char *stl, const char *opt)
{
	mreal r = gr->SaveState(opt);
	char buf[64]="";	if(r>2)	sprintf(buf,"value %g",r);
	mglDatSpl s;	s.d=f;	s.x0=gr->Min.x;	s.dx=f->GetNx()/(gr->Max.x-gr->Min.x);
	mgl_lamerey(gr,x0,func_dat,&s,stl,buf);
}
//-----------------------------------------------------------------------------
double MGL_NO_EXPORT func_str(double x, void *p)
{	HMEX s = (HMEX)p;	return mgl_expr_eval(s,x,0,0);	}
void MGL_EXPORT mgl_lamerey_str(HMGL gr, double x0, const char *f, const char *stl, const char *opt)
{
	HMEX eq = mgl_create_expr(f);
	mgl_lamerey(gr,x0,func_str,eq,stl,opt);
	mgl_delete_expr(eq);
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_lamerey_dat_(uintptr_t *gr, double *x0, uintptr_t *f, const char *stl, const char *opt, int l,int n)
{	char *s=new char[l+1];	memcpy(s,stl,l);	s[l]=0;
	char *o=new char[n+1];	memcpy(o,opt,n);	o[n]=0;
	mgl_lamerey_dat(_GR_,*x0,_DA_(f),s,o);	delete []s;	delete []o;	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_lamerey_str_(uintptr_t *gr, double *x0, const char *func, const char *stl, const char *opt, int m,int l,int n)
{	char *s=new char[l+1];	memcpy(s,stl,l);	s[l]=0;
	char *o=new char[n+1];	memcpy(o,opt,n);	o[n]=0;
	char *f=new char[m+1];	memcpy(f,func,m);	f[m]=0;
	mgl_lamerey_str(_GR_,*x0,f,s,o);	delete []f;	delete []s;	delete []o;	}
//-----------------------------------------------------------------------------
//
//	Bifurcation series
//
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_bifurcation(HMGL gr, double dx, double (*f)(double,double,void *), void *par, const char *stl, const char *opt)
{
	if((gr->Max.x-gr->Min.x)*dx<=0)	{	gr->SetWarn(mglWarnSlc,"Bifurcation");	return;	}
	static int cgid=1;	gr->StartGroup("Bifurcation",cgid++);
	mreal res=gr->SaveState(opt);
	long n = res>2 ? long(res+0.5):1024, m=(gr->Max.x-gr->Min.x)/dx, m1=0,m2=0;
	gr->SetPenPal(stl);	gr->Reserve(2*n*m);
	double *v1=new double[n], *v2=new double[n], dd=0.1*fabs(gr->Max.y-gr->Min.y)/n;
	double r = gr->Min.y+mgl_rnd()*(gr->Max.y-gr->Min.y), r0 = r;

	bool fin=false;
	for(long i=0;i<10*n;i++)	r = f(gr->Min.x,r,par);	// wait for loop stabilization
	for(m1=0;m1<n;m1++)	// collect period information
	{
		r = f(gr->Min.x,r,par);
		for(long j=0;j<m1;j++)	if(fabs(v1[j]-r)<dd)
		{	fin=true;	break;	}
		if(fin)	break;	v1[m1]=r;
	}
	for(mreal xx = gr->Min.x+dx;xx<=gr->Max.x;xx+=dx)
	{
		m2=m1;	memcpy(v2,v1,n*sizeof(double));	r=r0;
		for(long i=0;i<10*n;i++)	r = f(xx,r,par);	// wait for loop stabilization
		for(fin=false,m1=0;m1<n;m1++)	// collect period information
		{
			r = f(xx,r,par);
			for(long j=0;j<m1;j++)	if(fabs(v1[j]-r)<dd)
			{	fin=true;	break;	}
			if(fin)	break;	v1[m1]=r;
		}
		if(m1>=m2)	for(long i=0;i<m1;i++)
		{
			double vv=v2[0], vi=v1[i];
			for(long j=1;j<m2;j++)	if(fabs(v2[j]-vi)<fabs(vv-vi))	vv = v2[j];
			gr->line_plot(gr->AddPnt(mglPoint(xx-dx,vv,gr->Max.z)), gr->AddPnt(mglPoint(xx,v1[i],gr->Max.z)));
		}
		else	for(long i=0;i<m1;i++)
			gr->line_plot(gr->AddPnt(mglPoint(xx-dx,v1[i],gr->Max.z)), gr->AddPnt(mglPoint(xx,v1[i],gr->Max.z)));
	}
	gr->EndGroup();	delete []v1;	delete []v2;
}
//-----------------------------------------------------------------------------
double MGL_NO_EXPORT bif_dat(double x, double y, void *p)
{	mglDatSpl *s = (mglDatSpl *)p;	return s->d->value((x-s->x0)*s->dx, (y-s->y0)*s->dy);	}
void MGL_EXPORT mgl_bifurcation_dat(HMGL gr, double dx, HCDT f, const char *stl, const char *opt)
{
	if(dx==0 || (gr->Max.x-gr->Min.x)*dx<0)	{	gr->SetWarn(mglWarnSlc,"Bifurcation");	return;	}
	if(f->GetNy()<2)	{	gr->SetWarn(mglWarnLow,"Bifurcation");	return;	}
	mreal r = gr->SaveState(opt);
	char buf[64]="";	if(r>2)	sprintf(buf,"value %g",r);
	mglDatSpl s;	s.d=f;
	s.x0=gr->Min.x;	s.dx=f->GetNx()/(gr->Max.x-gr->Min.x);
	s.y0=gr->Min.y;	s.dy=f->GetNy()/(gr->Max.y-gr->Min.y);
	mgl_bifurcation(gr,dx,bif_dat,&s,stl,buf);
}
//-----------------------------------------------------------------------------
double MGL_NO_EXPORT bif_str(double x, double y, void *p)
{	HMEX s = (HMEX)p;	return mgl_expr_eval(s,x,y,0);	}
void MGL_EXPORT mgl_bifurcation_str(HMGL gr, double dx, const char *f, const char *stl, const char *opt)
{
	HMEX eq = mgl_create_expr(f);
	mgl_bifurcation(gr,dx,bif_str,eq,stl,opt);
	mgl_delete_expr(eq);
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_bifurcation_dat_(uintptr_t *gr, double *dx, uintptr_t *f, const char *stl, const char *opt, int l,int n)
{	char *s=new char[l+1];	memcpy(s,stl,l);	s[l]=0;
	char *o=new char[n+1];	memcpy(o,opt,n);	o[n]=0;
	mgl_bifurcation_dat(_GR_,*dx,_DA_(f),s,o);	delete []s;	delete []o;	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_bifurcation_str_(uintptr_t *gr, double *dx, const char *func, const char *stl, const char *opt, int m,int l,int n)
{	char *s=new char[l+1];	memcpy(s,stl,l);	s[l]=0;
	char *o=new char[n+1];	memcpy(o,opt,n);	o[n]=0;
	char *f=new char[m+1];	memcpy(f,func,m);	f[m]=0;
	mgl_bifurcation_str(_GR_,*dx,f,s,o);	delete []f;	delete []s;	delete []o;	}
//-----------------------------------------------------------------------------
