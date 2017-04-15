/***************************************************************************
 * surf.cpp is part of Math Graphic Library
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
#include "mgl2/define.h"
#include "mgl2/surf.h"
#include "mgl2/data.h"
#include "mgl2/eval.h"
#include "mgl2/base.h"
//-----------------------------------------------------------------------------
void MGL_NO_EXPORT mgl_mesh_plot(mglBase *gr, long *pos, long n, long m, int how)
{
	int d = gr->MeshNum>0 ? gr->MeshNum+1 : n*m, dx = n>d?n/d:1, dy = m>d?m/d:1;
	// NOTE: number of lines in each direction can be reduced too
	if(how&1)	for(long j=0;j<m;j+=dy)
	{
		register long s,i;
		for(s=i=0;i<n-1;i++)	if(pos[n*j+i]>=0 && pos[n*j+i+1]>=0)	s++;
		d = gr->FaceNum>0 ? gr->FaceNum+1 : n;	s = s>d?s/d:1;
		for(i=0;i<n-s;i+=s)
			gr->line_plot(pos[n*j+i],pos[n*j+i+s]);

	}
	if(how&2)	for(long i=0;i<n;i+=dx)
	{
		register long s,j;
		for(s=j=0;j<m-1;j++)	if(pos[n*j+i]>=0 && pos[n*j+i+n]>=0)	s++;
		d = gr->FaceNum>0 ? gr->FaceNum+1 : n;	s = s>d?s/d:1;
		for(j=0;j<m-s;j+=s)
			gr->line_plot(pos[n*j+i],pos[n*j+i+s*n]);
	}
}
//-----------------------------------------------------------------------------
void MGL_NO_EXPORT mgl_surf_plot(mglBase *gr, long *pos, long n, long m)
{
	long s=0;
	for(long j=0;j<m-1;j++)	for(long i=0;i<n-1;i++)
		if(pos[n*j+i]>=0 && pos[n*j+i+1]>=0 && pos[n*j+i+n]>=0 && pos[n*j+i+n+1]>=0)
			s++;
	long dx=1,dy=1;
	if(gr->FaceNum && s>gr->FaceNum*gr->FaceNum)
	{
		int d = gr->FaceNum+1,ns=n*s/((n-1)*(m-1)),ms=m*s/((n-1)*(m-1));
		dx = ns>d?ns/d:1;		dy = ms>d?ms/d:1;
	}
	for(long j=0;j<m-dy;j+=dy)	for(long i=0;i<n-dx;i+=dx)
		gr->quad_plot(pos[n*j+i],pos[n*j+i+dx],pos[n*j+i+n*dy],pos[n*j+i+n*dy+dx]);
}
//-----------------------------------------------------------------------------
//
//	Plot by formulas series
//
//-----------------------------------------------------------------------------
HMDT MGL_NO_EXPORT mglFormulaCalc(const char *str, const std::vector<mglDataA*> &head);
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_fsurf(HMGL gr, const char *eqZ, const char *sch, const char *opt)
{	// NOTE Strong function variation analysis can be added here
	if(eqZ==0 || eqZ[0]==0)	return;		// nothing to plot
	mreal r = gr->SaveState(opt);
	long n = (mgl_isnan(r) || r<=0) ? 100:long(r+0.5);
	mglData z(n,n);
	mglDataV x(n,n,1, gr->Min.x,gr->Max.x,'x');	x.s=L"x";
	mglDataV y(n,n,1, gr->Min.y,gr->Max.y,'y');	y.s=L"y";
	mglDataV t(n,n);	t.s=L"#$mgl";
	std::vector<mglDataA*> list;
	list.push_back(&x);	list.push_back(&y);	list.push_back(&t);
	z.Move(mglFormulaCalc(eqZ,list));
	mgl_surf(gr, &z, sch,0);
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_fsurf_xyz(HMGL gr, const char *eqX, const char *eqY, const char *eqZ, const char *sch, const char *opt)
{	// NOTE Strong function variation analysis can be added here
	if(eqZ==0 || eqZ[0]==0)	return;		// nothing to plot
	mreal r = gr->SaveState(opt);
	long n = (mgl_isnan(r) || r<=0) ? 100:long(r+0.5);
	mglData z(n,n), x(n,n), y(n,n);
	mglDataV u(n,n,1, 0,1,'x');	u.s=L"u";
	mglDataV v(n,n,1, 0,1,'y');	v.s=L"v";
	mglDataV t(n,n);	t.s=L"#$mgl";
	std::vector<mglDataA*> list;
	list.push_back(&u);	list.push_back(&v);	list.push_back(&t);
	x.Move(mglFormulaCalc(eqX,list));
	y.Move(mglFormulaCalc(eqY,list));
	z.Move(mglFormulaCalc(eqZ,list));
	mgl_surf_xy(gr,&x,&y,&z,sch,0);
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_fsurf_(uintptr_t *gr, const char *fy, const char *stl, const char *opt, int ly, int ls, int lo)
{	char *s=new char[ly+1];	memcpy(s,fy,ly);	s[ly]=0;
	char *p=new char[ls+1];	memcpy(p,stl,ls);	p[ls]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_fsurf(_GR_, s, p, o);	delete []o;	delete []s;	delete []p;	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_fsurf_xyz_(uintptr_t *gr, const char *fx, const char *fy, const char *fz, const char *stl, const char *opt, int lx, int ly, int lz, int ls, int lo)
{
	char *sx=new char[lx+1];	memcpy(sx,fx,lx);	sx[lx]=0;
	char *sy=new char[ly+1];	memcpy(sy,fy,ly);	sy[ly]=0;
	char *sz=new char[lz+1];	memcpy(sz,fz,lz);	sz[lz]=0;
	char *p=new char[ls+1];		memcpy(p,stl,ls);	p[ls]=0;
	char *o=new char[lo+1];		memcpy(o,opt,lo);	o[lo]=0;
	mgl_fsurf_xyz(_GR_, sx, sy, sz, p, o);	delete []o;
	delete []sx;	delete []sy;	delete []sz;	delete []p;
}
//-----------------------------------------------------------------------------
//
//	Mesh series
//
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_mesh_xy(HMGL gr, HCDT x, HCDT y, HCDT z, const char *sch, const char *opt)
{
	long n=z->GetNx(),m=z->GetNy();
	if(mgl_check_dim2(gr,x,y,z,0,"Mesh"))	return;

	gr->SaveState(opt);
	static int cgid=1;	gr->StartGroup("Mesh",cgid++);
	gr->SetPenPal(sch,0,false);
	long ss = gr->AddTexture(sch);
	long *pos = new long[n*m];	memset(pos,-1L,n*m*sizeof(long));
	gr->Reserve(n*m*z->GetNz());

	for(long k=0;k<z->GetNz();k++)
	{
		if(gr->NeedStop())	break;
		for(long j=0;j<m;j++)	for(long i=0;i<n;i++)
		{
			register mreal zz = z->v(i,j,k);
			pos[i+n*j] = gr->AddPnt(mglPoint(GetX(x,i,j,k).x, GetY(y,i,j,k).x, zz),gr->GetC(ss,zz));
		}
		mgl_mesh_plot(gr,pos,n,m,3);
	}
	delete []pos;	gr->EndGroup();
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_mesh(HMGL gr, HCDT z, const char *sch, const char *opt)
{
	gr->SaveState(opt);
	mglDataV x(z->GetNx()), y(z->GetNy());
	x.Fill(gr->Min.x,gr->Max.x);
	y.Fill(gr->Min.y,gr->Max.y);
	mgl_mesh_xy(gr,&x,&y,z,sch,0);
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_mesh_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *a, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];		memcpy(o,opt,lo);	o[lo]=0;
	mgl_mesh_xy(_GR_, _DA_(x), _DA_(y), _DA_(a), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_mesh_(uintptr_t *gr, uintptr_t *a, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];		memcpy(o,opt,lo);	o[lo]=0;
	mgl_mesh(_GR_, _DA_(a), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
//
//	Fall series
//
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_fall_xy(HMGL gr, HCDT x, HCDT y, HCDT z, const char *sch, const char *opt)
{
	long n=z->GetNx(),m=z->GetNy();
	if(mgl_check_dim2(gr,x,y,z,0,"Fall"))	return;

	gr->SaveState(opt);
	static int cgid=1;	gr->StartGroup("Fall",cgid++);
	gr->SetPenPal(sch,0,false);
	long ss = gr->AddTexture(sch);
	long *pos = new long[n*m];
	gr->Reserve(n*m*z->GetNz());

	for(long k=0;k<z->GetNz();k++)
	{
		if(gr->NeedStop())	break;
		for(long j=0;j<m;j++)	for(long i=0;i<n;i++)
		{
			register mreal zz = z->v(i,j,k);
			pos[i+n*j] = gr->AddPnt(mglPoint(GetX(x,i,j,k).x, GetY(y,i,j,k).x, zz),gr->GetC(ss,zz));
		}
		mgl_mesh_plot(gr,pos,n,m, (mglchr(sch,'x')) ? 2:1);
	}
	delete []pos;	gr->EndGroup();
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_fall(HMGL gr, HCDT z, const char *sch, const char *opt)
{
	gr->SaveState(opt);
	mglDataV x(z->GetNx()), y(z->GetNy());
	x.Fill(gr->Min.x,gr->Max.x);
	y.Fill(gr->Min.y,gr->Max.y);
	mgl_fall_xy(gr,&x,&y,z,sch,0);
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_fall_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *a, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];		memcpy(o,opt,lo);	o[lo]=0;
	mgl_fall_xy(_GR_, _DA_(x), _DA_(y), _DA_(a), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_fall_(uintptr_t *gr, uintptr_t *a, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];		memcpy(o,opt,lo);	o[lo]=0;
	mgl_fall(_GR_, _DA_(a), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
//
//	Grid series
//
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_grid_xy(HMGL gr, HCDT x, HCDT y, HCDT z, const char *sch, const char *opt)
{
	long n=z->GetNx(),m=z->GetNy();
	if(mgl_check_dim2(gr,x,y,z,0,"Grid"))	return;

	gr->SaveState(opt);
	static int cgid=1;	gr->StartGroup("Grid",cgid++);
	mreal	zVal = gr->Min.z;
	gr->SetPenPal(sch?sch:"k-");
	long *pos = new long[n*m];
	gr->Reserve(n*m*z->GetNz());

	for(long k=0;k<z->GetNz();k++)
	{
		if(gr->NeedStop())	break;
		if(z->GetNz()>1)	zVal = gr->Min.z+(gr->Max.z-gr->Min.z)*mreal(k)/(z->GetNz()-1);
		for(long j=0;j<m;j++)	for(long i=0;i<n;i++)
			pos[i+n*j] = gr->AddPnt(mglPoint(GetX(x,i,j,k).x, GetY(y,i,j,k).x, zVal),gr->CDef);
		mgl_mesh_plot(gr,pos,n,m,3);
	}
	delete []pos;	gr->EndGroup();
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_grid(HMGL gr, HCDT z,const char *sch, const char *opt)
{
	gr->SaveState(opt);
	mglDataV x(z->GetNx()), y(z->GetNy());
	x.Fill(gr->Min.x,gr->Max.x);
	y.Fill(gr->Min.y,gr->Max.y);
	mgl_grid_xy(gr,&x,&y,z,sch,0);
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_grid_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *a, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];		memcpy(o,opt,lo);	o[lo]=0;
	mgl_grid_xy(_GR_,_DA_(x), _DA_(y), _DA_(a), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_grid_(uintptr_t *gr, uintptr_t *a,const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];		memcpy(o,opt,lo);	o[lo]=0;
	mgl_grid(_GR_, _DA_(a), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
//
//	Surf series
//
//-----------------------------------------------------------------------------
void MGL_NO_EXPORT mgl_surf_gen(HMGL gr, HCDT x, HCDT y, HCDT z, HCDT c, HCDT a, const char *sch)
{
	long n=z->GetNx(),m=z->GetNy();
	long ss = gr->AddTexture(sch);
	long *pos = new long[n*m];
	bool wire = (mglchr(sch,'#'));
	gr->Reserve((n+1)*(m+1)*z->GetNz()*(wire?2:1));

	mglPoint q,s,xx,yy;
	for(long k=0;k<z->GetNz();k++)
	{
		if(gr->NeedStop())	break;
		if(a)	for(long j=0;j<m;j++)	for(long i=0;i<n;i++)
		{
			xx = GetX(x,i,j,k);		yy = GetY(y,i,j,k);
			q.Set(xx.y, yy.y, z->dvx(i,j,k));
			s.Set(xx.z, yy.z, z->dvy(i,j,k));
			pos[i+n*j] = gr->AddPnt(mglPoint(xx.x, yy.x, z->v(i,j,k)), gr->GetC(ss,c->v(i,j,k)), q^s, gr->GetA(a->v(i,j,k)));
		}
		else	for(long j=0;j<m;j++)	for(long i=0;i<n;i++)
		{
			xx = GetX(x,i,j,k);		yy = GetY(y,i,j,k);
			q.Set(xx.y, yy.y, z->dvx(i,j,k));
			s.Set(xx.z, yy.z, z->dvy(i,j,k));
			pos[i+n*j] = gr->AddPnt(mglPoint(xx.x, yy.x, z->v(i,j,k)), gr->GetC(ss,c->v(i,j,k)), q^s);
		}
		if(sch && mglchr(sch,'.'))
			for(long i=0;i<n*m;i++)	gr->mark_plot(pos[i],'.');
		else	mgl_surf_plot(gr,pos,n,m);
		if(wire)
		{
			gr->SetPenPal("k-");
			for(long i=0;i<n*m;i++)	pos[i] = gr->CopyNtoC(pos[i],gr->CDef);
			mgl_mesh_plot(gr,pos,n,m,3);
		}
	}
	delete []pos;	gr->EndGroup();
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_surf_xy(HMGL gr, HCDT x, HCDT y, HCDT z, const char *sch, const char *opt)
{
	if(mgl_check_dim2(gr,x,y,z,0,"Surf"))	return;
	gr->SaveState(opt);
	static int cgid=1;	gr->StartGroup("Surf",cgid++);
	mgl_surf_gen(gr, x, y, z, z, 0, sch);
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_surf(HMGL gr, HCDT z, const char *sch, const char *opt)
{
	gr->SaveState(opt);
	mglDataV x(z->GetNx()), y(z->GetNy());
	x.Fill(gr->Min.x,gr->Max.x);
	y.Fill(gr->Min.y,gr->Max.y);
	mgl_surf_xy(gr,&x,&y,z,sch,0);
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_surf_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *a, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];		memcpy(o,opt,lo);	o[lo]=0;
	mgl_surf_xy(_GR_, _DA_(x), _DA_(y), _DA_(a), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_surf_(uintptr_t *gr, uintptr_t *a, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];		memcpy(o,opt,lo);	o[lo]=0;
	mgl_surf(_GR_, _DA_(a), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
//
//	SurfCA series
//
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_surfca_xy(HMGL gr, HCDT x, HCDT y, HCDT z, HCDT c, HCDT a, const char *sch, const char *opt)
{
	if(mgl_check_dim2(gr,x,y,z,c,"SurfCA"))	return;
	if(mgl_check_dim2(gr,x,y,z,a,"SurfCA"))	return;
	gr->SaveState(opt);
	static int cgid=1;	gr->StartGroup("SurfCA",cgid++);
	mgl_surf_gen(gr, x, y, z, c, a, sch);
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_surfca(HMGL gr, HCDT z, HCDT c, HCDT a, const char *sch, const char *opt)
{
	gr->SaveState(opt);
	mglDataV x(z->GetNx()), y(z->GetNy());
	x.Fill(gr->Min.x,gr->Max.x);
	y.Fill(gr->Min.y,gr->Max.y);
	mgl_surfca_xy(gr,&x,&y,z,c,a,sch,0);
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_surfca_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *c, uintptr_t *a, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];		memcpy(o,opt,lo);	o[lo]=0;
	mgl_surfca_xy(_GR_, _DA_(x), _DA_(y), _DA_(z), _DA_(c), _DA_(a), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_surfca_(uintptr_t *gr, uintptr_t *z, uintptr_t *c, uintptr_t *a, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];		memcpy(o,opt,lo);	o[lo]=0;
	mgl_surfca(_GR_, _DA_(z), _DA_(c), _DA_(a), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
//
//	SurfC series
//
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_surfc_xy(HMGL gr, HCDT x, HCDT y, HCDT z, HCDT c, const char *sch, const char *opt)
{
	if(mgl_check_dim2(gr,x,y,z,c,"SurfC"))	return;
	gr->SaveState(opt);
	static int cgid=1;	gr->StartGroup("SurfC",cgid++);
	mgl_surf_gen(gr, x, y, z, c, 0, sch);
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_surfc(HMGL gr, HCDT z, HCDT c, const char *sch, const char *opt)
{
	gr->SaveState(opt);
	mglDataV x(z->GetNx()), y(z->GetNy());
	x.Fill(gr->Min.x,gr->Max.x);
	y.Fill(gr->Min.y,gr->Max.y);
	mgl_surfc_xy(gr,&x,&y,z,c,sch,0);
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_surfc_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];		memcpy(o,opt,lo);	o[lo]=0;
	mgl_surfc_xy(_GR_, _DA_(x), _DA_(y), _DA_(z), _DA_(a), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_surfc_(uintptr_t *gr, uintptr_t *z, uintptr_t *a, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];		memcpy(o,opt,lo);	o[lo]=0;
	mgl_surfc(_GR_, _DA_(z), _DA_(a), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
//
//	SurfA series
//
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_surfa_xy(HMGL gr, HCDT x, HCDT y, HCDT z, HCDT c, const char *sch, const char *opt)
{
	if(mgl_check_dim2(gr,x,y,z,c,"SurfA"))	return;
	gr->SaveState(opt);
	static int cgid=1;	gr->StartGroup("SurfA",cgid++);
	mgl_surf_gen(gr, x, y, z, z, c, sch);
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_surfa(HMGL gr, HCDT z, HCDT c, const char *sch, const char *opt)
{
	gr->SaveState(opt);
	mglDataV x(z->GetNx()), y(z->GetNy());
	x.Fill(gr->Min.x,gr->Max.x);
	y.Fill(gr->Min.y,gr->Max.y);
	mgl_surfa_xy(gr,&x,&y,z,c,sch,0);
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_surfa_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *z, uintptr_t *a, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];		memcpy(o,opt,lo);	o[lo]=0;
	mgl_surfa_xy(_GR_, _DA_(x), _DA_(y), _DA_(z), _DA_(a), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_surfa_(uintptr_t *gr, uintptr_t *z, uintptr_t *a, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];		memcpy(o,opt,lo);	o[lo]=0;
	mgl_surfa(_GR_, _DA_(z), _DA_(a), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
//
//	Belt series
//
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_belt_xy(HMGL gr, HCDT x, HCDT y, HCDT z, const char *sch, const char *opt)
{
	long n=z->GetNx(),m=z->GetNy();
	if(mgl_check_dim2(gr,x,y,z,0,"Belt"))	return;

	gr->SaveState(opt);
	static int cgid=1;	gr->StartGroup("Belt",cgid++);
	int d = gr->MeshNum>0 ? gr->MeshNum+1 : n*m, dx = n>d?n/d:1, dy = m>d?m/d:1;
	long ss = gr->AddTexture(sch);
	long *pos = new long[2*(n>m?n:m)];
	gr->Reserve(2*n*m*z->GetNz());
	bool how = !mglchr(sch,'x');

	mglPoint p1,p2,q,s,xx,yy;
	for(long k=0;k<z->GetNz();k++)
	{
		if(gr->NeedStop())	break;
		if(how)	for(long i=0;i<n-dx;i+=dx)
		{
			for(long j=0;j<m;j++)
			{
				xx = GetX(x,i,j,k);		yy = GetY(y,i,j,k);
				p1.Set(xx.x, yy.x, z->v(i,j,k));
				s.Set(xx.z, yy.z, z->dvy(i,j,k));
				q.Set(xx.y, yy.y, 0);	s = q^s;
				register mreal c = gr->GetC(ss,p1.z);
				p2.Set(GetX(x,i+dx,j,k).x,GetY(y,i+dx,j,k).x,p1.z);
				pos[2*j] = gr->AddPnt(p1,c,s);
				pos[2*j+1]=gr->AddPnt(p2,c,s);
			}
			mgl_surf_plot(gr,pos,2,m);
		}
		else	for(long j=0;j<m-dy;j+=dy)
		{
			for(long i=0;i<n;i++)	// ñîçäàåì ìàññèâ òî÷åê
			{
				xx = GetX(x,i,j,k);		yy = GetY(y,i,j,k);
				p1.Set(xx.x, yy.x, z->v(i,j,k));
				q.Set(xx.y, yy.y, z->dvx(i,j,k));
				s.Set(xx.z, yy.z, 0);	s = q^s;
				register mreal c = gr->GetC(ss,p1.z);
				p2.Set(GetX(x,i,j+dy,k).x,GetY(y,i,j+dy,k).x,p1.z);
				pos[2*i] = gr->AddPnt(p1,c,s);
				pos[2*i+1]=gr->AddPnt(p2,c,s);
			}
			mgl_surf_plot(gr,pos,2,n);
		}
	}
	delete []pos; gr->EndGroup();
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_belt(HMGL gr, HCDT z, const char *sch, const char *opt)
{
	gr->SaveState(opt);
	mglDataV x(z->GetNx()), y(z->GetNy());
	x.Fill(gr->Min.x,gr->Max.x);
	y.Fill(gr->Min.y,gr->Max.y);
	mgl_belt_xy(gr,&x,&y,z,sch,0);
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_belt_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *a, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];		memcpy(o,opt,lo);	o[lo]=0;
	mgl_belt_xy(_GR_, _DA_(x), _DA_(y), _DA_(a), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_belt_(uintptr_t *gr, uintptr_t *a, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	mgl_belt(_GR_, _DA_(a), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
//
//	Dens series
//
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_dens_xy(HMGL gr, HCDT x, HCDT y, HCDT c, const char *sch, const char *opt)
{
	if(mgl_check_dim2(gr,x,y,c,0,"Dens"))	return;
	gr->SaveState(opt);
	static int cgid=1;	gr->StartGroup("Dens",cgid++);
	mreal	zVal = gr->Min.z;

	mglDataV z(c->GetNx(),c->GetNy(),c->GetNz());
	if(z.GetNz()>1)	z.Fill(gr->Min.z,gr->Max.z,'z');
	else	z.Fill(zVal);
	mgl_surf_gen(gr, x, y, &z, c, 0, sch);
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_dens(HMGL gr, HCDT c, const char *sch, const char *opt)
{
	gr->SaveState(opt);
	mglDataV x(c->GetNx()), y(c->GetNy());
	x.Fill(gr->Min.x, gr->Max.x);
	y.Fill(gr->Min.y, gr->Max.y);
	mgl_dens_xy(gr,&x,&y,c,sch,0);
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_dens_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *a, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];		memcpy(o,opt,lo);	o[lo]=0;
	mgl_dens_xy(_GR_,_DA_(x), _DA_(y), _DA_(a), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_dens_(uintptr_t *gr, uintptr_t *a, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];		memcpy(o,opt,lo);	o[lo]=0;
	mgl_dens(_GR_,_DA_(a), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
//
//	STFA series
//
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_stfa_xy(HMGL gr, HCDT x, HCDT y, HCDT re, HCDT im, int dn, const char *sch, const char *opt)
{	mglData tmp(mglSTFA(*re,*im,dn,'x'));	mgl_dens_xy(gr,x,y,&tmp,sch,opt);	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_stfa(HMGL gr, HCDT re, HCDT im, int dn, const char *sch, const char *opt)
{	mglData tmp(mglSTFA(*re,*im,dn,'x'));	mgl_dens(gr,&tmp,sch,opt);	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_stfa_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *re, uintptr_t *im, int *dn, const char *sch, const char *opt, int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];		memcpy(o,opt,lo);	o[lo]=0;
	mgl_stfa_xy(_GR_,_DA_(x), _DA_(y), _DA_(re), _DA_(im), *dn, s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_stfa_(uintptr_t *gr, uintptr_t *re, uintptr_t *im, int *dn, const char *sch, const char *opt, int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];		memcpy(o,opt,lo);	o[lo]=0;
	mgl_stfa(_GR_,_DA_(re), _DA_(im), *dn, s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
//
//	Boxs series
//
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_boxs_xy(HMGL gr, HCDT x, HCDT y, HCDT z, const char *sch, const char *opt)
{
	register long i,j,k,n=z->GetNx(),m=z->GetNy();
	if(mgl_check_dim2(gr,x,y,z,0,"Boxs",true))	return;

	gr->SaveState(opt);
	static int cgid=1;	gr->StartGroup("Boxs",cgid++);
	long ly = y->GetNy()>=m ? y->GetNy() : y->GetNx(), lx = x->GetNx();
	int d = gr->MeshNum>0 ? gr->MeshNum+1 : n*m, dx = n>d?n/d:1, dy = m>d?m/d:1;

	long ss = gr->AddTexture(sch);
	bool wire = mglchr(sch,'#');
	bool full = mglchr(sch,'@');
	gr->Reserve(8*n*m*z->GetNz());

	mglPoint p1,p2,p3,p4,q,s,t(wire||full?NAN:0,0,1),xx,yy;
	mreal zz,z1,z2,x1,y1,x2,y2,x3,y3,c,z0=gr->GetOrgZ('x');
	long k1,k2,k3,k4,k5,k6,k7,k8;
	for(k=0;k<z->GetNz();k++)
	{
		if(gr->NeedStop())	break;
		for(i=0;i<n;i+=dx)	for(j=0;j<m;j+=dy)
		{
			zz = z->v(i,j,k);		c  = gr->GetC(ss,zz);
			xx = GetX(x,i,j,k);		yy = GetY(y,i,j,k);
			x1 = i<lx-dx ? GetX(x,i+dx,j,k).x:NAN;
			y1 = i<lx-dx ? GetY(y,i+dx,j,k).x:NAN;
			x2 = j<ly-dy ? GetX(x,i,j+dy,k).x:NAN;
			y2 = j<ly-dy ? GetY(y,i,j+dy,k).x:NAN;
			x3 = i<lx-dx && j<ly-dy ? GetX(x,i+dx,j+dy,k).x:NAN;
			y3 = i<lx-dx && j<ly-dy ? GetY(y,i+dx,j+dy,k).x:NAN;
			z1 = i<n-dx?z->v(i+dx,j,k):NAN;
			z2 = j<m-dy?z->v(i,j+dy,k):NAN;
			q.Set(xx.y,yy.y,0);
			s.Set(xx.z,yy.z,0);
			p1.Set(xx.x,yy.x,zz);	k1 = gr->AddPnt(p1,c,t);
			p2.Set(x1,y1,zz);		k2 = gr->AddPnt(p2,c,t);
			p3.Set(x2,y2,zz);		k3 = gr->AddPnt(p3,c,t);
			p4.Set(x3,y3,zz);		k4 = gr->AddPnt(p4,c,t);
			if(wire)
			{
				gr->line_plot(k1,k2);	gr->line_plot(k1,k3);
				gr->line_plot(k4,k2);	gr->line_plot(k4,k3);
			}
			else	gr->quad_plot(k1,k2,k3,k4);

			if(full)
			{
				p1.Set(xx.x,yy.x,z0);	k5 = gr->AddPnt(p1,c,t);
				p2.Set(x1,y1,z0);		k6 = gr->AddPnt(p2,c,t);
				p3.Set(x2,y2,z0);		k7 = gr->AddPnt(p3,c,t);
				p4.Set(x3,y3,z0);		k8 = gr->AddPnt(p4,c,t);
				if(wire)
				{
					gr->line_plot(k5,k6);	gr->line_plot(k5,k7);
					gr->line_plot(k8,k6);	gr->line_plot(k8,k7);
					gr->line_plot(k1,k5);	gr->line_plot(k3,k7);
					gr->line_plot(k2,k6);	gr->line_plot(k4,k8);
				}
				else
				{
					gr->quad_plot(k1,k2,k5,k6);	gr->quad_plot(k1,k3,k5,k7);
					gr->quad_plot(k4,k2,k8,k6);	gr->quad_plot(k4,k3,k8,k7);
					gr->quad_plot(k5,k6,k7,k8);
				}
			}
			else
			{
				p3.Set(x1,y1,z1);		k5 = gr->AddPnt(p3,c,wire?t:q);
				p4.Set(x3,y3,z1);		k6 = gr->AddPnt(p4,c,wire?t:q);
				if(wire)
				{	gr->line_plot(k2,k5);	gr->line_plot(k6,k5);	gr->line_plot(k6,k4);	}
				else	gr->quad_plot(k2,k4,k5,k6);
				p3.Set(x2,y2,z2);		k7 = gr->AddPnt(p3,c,wire?t:s);
				p4.Set(x3,y3,z2);		k8 = gr->AddPnt(p4,c,wire?t:s);
				if(wire)
				{	gr->line_plot(k3,k7);	gr->line_plot(k4,k8);	gr->line_plot(k7,k8);	}
				else	gr->quad_plot(k3,k4,k7,k8);
			}
		}
	}
	gr->EndGroup();
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_boxs(HMGL gr, HCDT z, const char *sch, const char *opt)
{
	gr->SaveState(opt);
	mglDataV x(z->GetNx()+1), y(z->GetNy()+1);
	x.Fill(gr->Min.x,gr->Max.x);
	y.Fill(gr->Min.y,gr->Max.y);
	mgl_boxs_xy(gr,&x,&y,z,sch,0);
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_boxs_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *a, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];		memcpy(o,opt,lo);	o[lo]=0;
	mgl_boxs_xy(_GR_, _DA_(x), _DA_(y), _DA_(a), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_boxs_(uintptr_t *gr, uintptr_t *a, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];		memcpy(o,opt,lo);	o[lo]=0;
	mgl_boxs(_GR_, _DA_(a), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
//
//	Tile series
//
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_tile_xy(HMGL gr, HCDT x, HCDT y, HCDT z, const char *sch, const char *opt)
{
	long n=z->GetNx(),m=z->GetNy();
	if(mgl_check_dim2(gr,x,y,z,0,"Tile",true))	return;

	gr->SaveState(opt);
	static int cgid=1;	gr->StartGroup("Tile",cgid++);
	long ly = x->GetNy()>=z->GetNy() ? y->GetNy() : y->GetNx(), lx = x->GetNx();
	int d = gr->MeshNum>0 ? gr->MeshNum+1 : n*m, dx = n>d?n/d:1, dy = m>d?m/d:1;

	long ss = gr->AddTexture(sch);
	gr->Reserve(4*n*m*z->GetNz());

	mglPoint s(0,0,1);
	for(long k=0;k<z->GetNz();k++)
	{
		if(gr->NeedStop())	break;
		for(long j=0;j<m;j+=dx)	for(long i=0;i<n;i+=dy)
		{
			register mreal zz = z->v(i,j,k), c = gr->GetC(ss,zz);
			register mreal xx = GetX(x,i,j,k).x, yy = GetY(y,i,j,k).x;
			register long k1 = gr->AddPnt(mglPoint(xx,yy,zz),c,s);
			xx = i<lx-dx ? GetX(x,i+dx,j,k).x:NAN;
			yy = i<lx-dx ? GetY(y,i+dx,j,k).x:NAN;
			register long k2 = gr->AddPnt(mglPoint(xx,yy,zz),c,s);
			xx = j<ly-dy ? GetX(x,i,j+dy,k).x:NAN;
			yy = j<ly-dy ? GetY(y,i,j+dy,k).x:NAN;
			register long k3 = gr->AddPnt(mglPoint(xx,yy,zz),c,s);
			xx = i<lx-dx && j<ly-dy ? GetX(x,i+dx,j+dy,k).x:NAN;
			yy = i<lx-dx && j<ly-dy ? GetY(y,i+dx,j+dy,k).x:NAN;
			register long k4 = gr->AddPnt(mglPoint(xx,yy,zz),c,s);
			gr->quad_plot(k1,k2,k3,k4);
		}
	}
	gr->EndGroup();
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_tile(HMGL gr, HCDT z, const char *sch, const char *opt)
{
	gr->SaveState(opt);
	mglDataV x(z->GetNx()+1), y(z->GetNy()+1);
	x.Fill(gr->Min.x,gr->Max.x);
	y.Fill(gr->Min.y,gr->Max.y);
	mgl_tile_xy(gr,&x,&y,z,sch,0);
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_tile_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *a, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];		memcpy(o,opt,lo);	o[lo]=0;
	mgl_tile_xy(_GR_, _DA_(x), _DA_(y), _DA_(a), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_tile_(uintptr_t *gr, uintptr_t *a, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];		memcpy(o,opt,lo);	o[lo]=0;
	mgl_tile(_GR_, _DA_(a), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
//
//	TileS series
//
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_tiles_xy(HMGL gr, HCDT x, HCDT y, HCDT z, HCDT s, const char *sch, const char *opt)
{
	long n=z->GetNx(),m=z->GetNy();
	if(mgl_check_dim2(gr,x,y,z,s,"TileS",true))	return;

	gr->SaveState(opt);
	static int cgid=1;	gr->StartGroup("TileS",cgid++);
	long ly = x->GetNy()>=z->GetNy() ? y->GetNy() : y->GetNx(), lx = x->GetNx();
	int d = gr->MeshNum>0 ? gr->MeshNum+1 : n*m, dx = n>d?n/d:1, dy = m>d?m/d:1;

	long cc = gr->AddTexture(sch);
	gr->Reserve(4*n*m*z->GetNz());

	mglPoint t(0,0,1);
	mreal x1,x2,x3,x4,y1,y2,y3,y4;
	for(long k=0;k<z->GetNz();k++)
	{
		if(gr->NeedStop())	break;
		for(long j=0;j<m;j+=dx)	for(long i=0;i<n;i+=dy)
		{
			register mreal zz = z->v(i,j,k), c = gr->GetC(cc,zz);
			register mreal ss = (1-gr->GetA(s->v(i,j,k)))/2, sm = 1-ss;

			x1 = GetX(x,i,j,k).x;	y1 = GetY(y,i,j,k).x;
			x2 = x3 = x4 = y2 = y3 = y4 = NAN;
			if(i<lx-dx)
			{	x2 = GetX(x,i+dx,j,k).x-x1;	y2 = GetY(y,i+dx,j,k).x-y1;	}
			if(j<ly-dy)
			{	x4 = GetX(x,i,j+dy,k).x-x1;	y4 = GetY(y,i,j+dy,k).x-y1;	}
			if(i<lx-dx && j<ly-dy)
			{	x3 = GetX(x,i+dx,j+dy,k).x-x2-x4-x1;
				y3 = GetY(y,i+dx,j+dy,k).x-y2-y4-y1;	}

			register long k1 = gr->AddPnt(mglPoint(x1+x2*ss+x4*ss+x3*ss*ss, y1+y2*ss+y4*ss+y3*ss*ss, zz),c,t);
			register long k2 = gr->AddPnt(mglPoint(x1+x2*sm+x4*ss+x3*ss*sm, y1+y2*sm+y4*ss+y3*ss*sm, zz),c,t);
			register long k3 = gr->AddPnt(mglPoint(x1+x2*ss+x4*sm+x3*ss*sm, y1+y2*ss+y4*sm+y3*ss*sm, zz),c,t);
			register long k4 = gr->AddPnt(mglPoint(x1+x2*sm+x4*sm+x3*sm*sm, y1+y2*sm+y4*sm+y3*sm*sm, zz),c,t);
			gr->quad_plot(k1,k2,k3,k4);
		}
	}
	gr->EndGroup();
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_tiles(HMGL gr, HCDT z, HCDT s, const char *sch, const char *opt)
{
	gr->SaveState(opt);
	mglDataV x(z->GetNx()+1), y(z->GetNy()+1);
	x.Fill(gr->Min.x,gr->Max.x);
	y.Fill(gr->Min.y,gr->Max.y);
	mgl_tiles_xy(gr,&x,&y,z,s,sch,0);
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_tiles_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *a, uintptr_t *r, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];		memcpy(o,opt,lo);	o[lo]=0;
	mgl_tiles_xy(_GR_, _DA_(x), _DA_(y), _DA_(a), _DA_(r), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_tiles_(uintptr_t *gr, uintptr_t *a, uintptr_t *r, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];		memcpy(o,opt,lo);	o[lo]=0;
	mgl_tiles(_GR_, _DA_(a), _DA_(r), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
//
//	Map series
//
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_map_xy(HMGL gr, HCDT x, HCDT y, HCDT ax, HCDT ay, const char *sch, const char *opt)
{
	long n=ax->GetNx(),m=ax->GetNy();
	if(mgl_check_dim2(gr,x,y,ax,ay,"Map"))	return;

	bool nboth = !(x->GetNx()==n && y->GetNx()==n && x->GetNy()==m && y->GetNy()==m);
	gr->SaveState(opt);
	static int cgid=1;	gr->StartGroup("Map",cgid++);

	long ss = gr->AddTexture(mgl_have_color(sch)?sch:"rgb",2);
	long s = nboth ?1:n;

	mglPoint t(NAN);
	long *pos = new long[n*m];
	gr->Reserve(n*m);

#pragma omp parallel for collapse(2)
	for(long j=0;j<m;j++)	for(long i=0;i<n;i++)
	{
		register long s1 = i>0 ? 1:0, s2 = i<n-1 ? 1:0;
		register mreal xdx = (ax->v(i+s2,j)-ax->v(i-s1,j))/(GetX(x,i+s2,j).x-GetX(x,i-s1,j).x);
		register mreal ydx = (ay->v(i+s2,j)-ay->v(i-s1,j))/(GetX(x,i+s2,j).x-GetX(x,i-s1,j).x);
		s1 = j>0 ? s:0;		s2 = j<m-1 ? s:0;
		register mreal xdy = (ax->v(i,j+s2)-ax->v(i,j-s1))/(GetY(y,i,j+s2).x-GetY(y,i,j-s1).x);
		register mreal ydy = (ay->v(i,j+s2)-ay->v(i,j-s1))/(GetY(y,i,j+s2).x-GetY(y,i,j-s1).x);
		xdx = xdx*ydy - xdy*ydx;	// Jacobian
		register mreal xx,yy;

		if(nboth)
		{
			xx = (x->v(i) - gr->Min.x)/(gr->Max.x - gr->Min.x);
			yy = (y->v(j) - gr->Min.y)/(gr->Max.y - gr->Min.y);
		}
		else
		{
			xx = (x->v(i,j) - gr->Min.x)/(gr->Max.x - gr->Min.x);
			yy = (y->v(i,j) - gr->Min.y)/(gr->Max.y - gr->Min.y);
		}
		if(xx<0)	xx=0;	if(xx>=1)	xx=1/MGL_FEPSILON;
		if(yy<0)	yy=0;	if(yy>=1)	yy=1/MGL_FEPSILON;
		pos[i+n*j] = gr->AddPnt(mglPoint(ax->v(i,j), ay->v(i,j), xdx),gr->GetC(ss,xx,false),t,yy);
	}
	if(sch && mglchr(sch,'.'))
		for(long i=0;i<n*m;i++)	gr->mark_plot(pos[i],'.');
	else	mgl_surf_plot(gr,pos,n,m);
	delete []pos;	gr->EndGroup();
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_map(HMGL gr, HCDT ax, HCDT ay, const char *sch, const char *opt)
{
	gr->SaveState(opt);
	mglDataV x(ax->GetNx()), y(ax->GetNy());
	x.Fill(gr->Min.x,gr->Max.x);
	y.Fill(gr->Min.y,gr->Max.y);
	mgl_map_xy(gr,&x,&y,ax,ay,sch,0);
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_map_xy_(uintptr_t *gr, uintptr_t *x, uintptr_t *y, uintptr_t *a, uintptr_t *b, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];		memcpy(o,opt,lo);	o[lo]=0;
	mgl_map_xy(_GR_, _DA_(x), _DA_(y), _DA_(a), _DA_(b), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_map_(uintptr_t *gr, uintptr_t *a, uintptr_t *b, const char *sch, const char *opt,int l,int lo)
{	char *s=new char[l+1];	memcpy(s,sch,l);	s[l]=0;
	char *o=new char[lo+1];		memcpy(o,opt,lo);	o[lo]=0;
	mgl_map(_GR_, _DA_(a), _DA_(b), s, o);	delete []o;	delete []s;	}
//-----------------------------------------------------------------------------
