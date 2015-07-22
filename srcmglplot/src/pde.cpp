/***************************************************************************
 * pde.cpp is part of Math Graphic Library
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
#include "mgl2/data.h"
#include "mgl2/datac.h"
#include "mgl2/eval.h"
#include "mgl2/thread.h"
#include "mgl2/base.h"
const double GAMMA=0.1;	///< value for damping
mglData MGL_NO_EXPORT mglFormulaCalc(const char *str, const std::vector<mglDataA*> &head);
mglDataC MGL_NO_EXPORT mglFormulaCalcC(const char *str, const std::vector<mglDataA*> &head);
//-----------------------------------------------------------------------------
struct mgl_pde_ham
{
	ddual *a,*hxy,*hxv,*huv,*huy;
	const char *eqs;
	long nx,ny;
	double xx,yy,xs,ys,dx,dy,dq,dp,zz;
	double dd;
};
void MGL_NO_EXPORT mgl_pde_hprep(const mgl_pde_ham *f)
{
	long nx = f->nx, ny = f->ny;
	mglDataV x(nx,ny), y(nx,ny), z, r(nx,ny);
	mglDataW p(nx,ny), q(nx,ny);
	x.s = L"x";	y.s = L"y";	p.s = L"p";	q.s = L"q";	r.s=L"#$mgl";
	z.s = L"z";	z.Fill(f->zz);
	dual dd(0,f->dd);
	mglData u(nx,ny);	u.s = L"u";
#pragma omp parallel for
	for(long i=0;i<nx*ny;i++)	u.a[i] = abs(f->a[i]);
	std::vector<mglDataA*> list;
	list.push_back(&x);	list.push_back(&y);	list.push_back(&z);
	list.push_back(&p);	list.push_back(&q);	list.push_back(&u);

	x.Fill(f->xx,f->xx+f->dx*(nx-1),'x');	p.Freq(0,'x');
	y.Fill(f->yy,f->yy+f->dy*(ny-1),'y');	q.Freq(0,'y');
	mglDataC res = mglFormulaCalcC(f->eqs, list);
#pragma omp parallel for
	for(long i=0;i<nx*ny;i++)	f->hxy[i] = res.a[i]*dd;
	if(ny>2)
	{
		x.Fill(f->xs);	p.Freq(f->dp,'x');
		res = mglFormulaCalcC(f->eqs, list);
#pragma omp parallel for
		for(long i=0;i<nx*ny;i++)	f->huy[i] = res.a[i]*dd;
	}
	x.Fill(f->xs);	p.Freq(f->dp,'x');
	y.Fill(f->ys);	q.Freq(f->dq,'y');
	res = mglFormulaCalcC(f->eqs, list);
#pragma omp parallel for
	for(long i=0;i<nx*ny;i++)	f->huv[i] = res.a[i]*dd;
	if(ny>2)
	{
		x.Fill(f->xx,f->xx+f->dx*(nx-1),'x');	p.Freq(0,'x');
		res = mglFormulaCalcC(f->eqs, list);
#pragma omp parallel for
		for(long i=0;i<nx*ny;i++)	f->hxv[i] = res.a[i]*dd;
	}
}
//-----------------------------------------------------------------------------
// Solve equation dx/dz = func(p,q,x,y,z,|u|)[u] where p=d/dx, q=d/dy. At this moment simplified form of ham is supported: ham = f(p,q,z) + g(x,y,z,'u'), where variable 'u'=|u| (for allowing solve nonlinear problems). You may specify imaginary part like ham = p^2 + i*x*(x>0) but only if dependence on variable 'i' is linear (i.e. ham = hre+i*him).
HADT MGL_EXPORT mgl_pde_solve_c(HMGL gr, const char *ham, HCDT ini_re, HCDT ini_im, mreal dz, mreal k0, const char *opt)
{
	gr->SaveState(opt);
	mglPoint Min=gr->Min, Max=gr->Max;
	long nx=ini_re->GetNx(), ny=ini_re->GetNy(), nz = long((Max.z-Min.z)/dz)+1;
	if(nx<2 || nz<2 || Max.x==Min.x)			// Too small data
	{	gr->SetWarn(mglWarnLow,"PDE");	return 0;	}
	if(ini_im->GetNx()*ini_im->GetNy() != nx*ny)// Wrong dimensions
	{	gr->SetWarn(mglWarnDim,"PDE");	return 0;	}
	mglDataC *res=new mglDataC(nz, nx, ny);

	ddual *a = new ddual[4*nx*ny], hh0;	// Add "damping" area
	ddual *hxy = new ddual[4*nx*ny], *hxv = new ddual[4*nx*ny];
	ddual *huy = new ddual[4*nx*ny], *huv = new ddual[4*nx*ny];
	ddual *hx = new ddual[2*nx], *hv = new ddual[2*ny];
	ddual *hy = new ddual[2*ny], *hu = new ddual[2*nx];
	double *dmp = new double[4*nx*ny];
	memset(a,0,4*nx*ny*sizeof(ddual));
	memset(dmp,0,4*nx*ny*sizeof(double));
#pragma omp parallel for collapse(2)
	for(long j=0;j<ny;j++)	for(long i=0;i<nx;i++)	// Initial conditions
	{
		register long i0 = i+nx/2+2*nx*(j+ny/2);
		a[i0] = dual(ini_re->v(i,j), ini_im->v(i,j));
		res->a[nz*(i+nx*j)] = a[i0];
	}
#pragma omp parallel for collapse(2)
	for(long j=0;j<2*ny;j++)	for(long i=0;i<2*nx;i++)	// step 1
	{
		register long i0 = i+2*nx*j;
		if(i<nx/2)		dmp[i0] += GAMMA*mgl_ipow((nx/2-i)/(nx/2.),2);
		if(i>3*nx/2)	dmp[i0] += GAMMA*mgl_ipow((i-3*nx/2-1)/(nx/2.),2);
		if(j<ny/2)		dmp[i0] += GAMMA*mgl_ipow((ny/2-j)/(ny/2.),2);
		if(j>3*ny/2)	dmp[i0] += GAMMA*mgl_ipow((j-3*ny/2-1)/(ny/2.),2);
	}
	mreal dx = (Max.x-Min.x)/(nx-1), dy = ny>1?(Max.y-Min.y)/(ny-1):0;
	mreal dp = M_PI/(Max.x-Min.x)/k0, dq = M_PI/(Max.y-Min.y)/k0;
	mreal xs=(Min.x+Max.x)/2, ys=(Min.y+Max.y)/2;
	double dd = k0*dz;

	mgl_pde_ham tmp;tmp.eqs = ham;
	tmp.nx = 2*nx;	tmp.ny = 2*ny;	tmp.dd = dd;	tmp.a=a;
	tmp.hxy=hxy;	tmp.hxv=hxv;	tmp.huy=huy;	tmp.huv=huv;
	tmp.xx = Min.x-dx*(nx/2);	tmp.xs = xs;	tmp.dx = dx;	tmp.dp = dp;
	tmp.yy = Min.y-dy*(ny/2);	tmp.ys = ys;	tmp.dy = dy;	tmp.dq = dq;

	// prepare fft. NOTE: slow procedures due to unknown nx, ny.
	void *wtx = mgl_fft_alloc(2*nx,0,0);
	void *wty = mgl_fft_alloc(2*ny,0,0);
	for(long k=1;k<nz;k++)
	{
		if(gr->NeedStop())	break;
		tmp.zz = Min.z+dz*k;
		memset(hxy,0,4*nx*ny*sizeof(dual));	memset(hxv,0,4*nx*ny*sizeof(dual));
		memset(huv,0,4*nx*ny*sizeof(dual));	memset(huy,0,4*nx*ny*sizeof(dual));
		mgl_pde_hprep(&tmp);
		for(long i=0;i<2*nx;i++)	{	hx[i] = hxv[i];			hu[i] = huv[i];		}
		for(long j=0;j<2*ny;j++)	{	hy[j] = huy[2*nx*j];	hv[j] = huv[2*nx*j];}
		// rearrange arrays
		hh0=hu[0];
		if(ny>1)
#pragma omp parallel for collapse(2)
			for(long i=0;i<2*nx;i++) for(long j=0;j<2*ny;j++)
			{
				register long i0 = i+2*nx*j;	huv[i0] -= hh0;
				hxv[i0] -= hx[i]+hv[j]-hh0;
				huy[i0] -= hu[i]+hy[j]-hh0;
			}
		else
#pragma omp parallel for
			for(long i=0;i<4*nx*ny;i++)	huv[i] -= hh0;
			// solve equation
		if(ny>1)
#pragma omp parallel
		{
			void *wsx = mgl_fft_alloc_thr(2*nx), *wsy = mgl_fft_alloc_thr(2*ny);
#pragma omp for
			for(long i=0;i<4*nx*ny;i++)	a[i] *= exp(hxy[i])*exp(-double(dmp[i]*dz));
#pragma omp for
			for(long i=0;i<2*ny;i++)	mgl_fft((double *)(a+i*2*nx), 1, 2*nx, wtx, wsx, false);
#pragma omp for
			for(long i=0;i<4*nx*ny;i++)	a[i] *= exp(huy[i]);
#pragma omp for
			for(long i=0;i<2*nx;i++)	mgl_fft((double *)(a+i), 2*nx, 2*ny, wty, wsy, false);
#pragma omp for
			for(long i=0;i<4*nx*ny;i++)	a[i] *= exp(huv[i]);
#pragma omp for
			for(long i=0;i<2*ny;i++)	mgl_fft((double *)(a+2*i*nx), 1, 2*nx, wtx, wsx, true);
#pragma omp for
			for(long i=0;i<4*nx*ny;i++)	a[i] *= exp(hxv[i]);
#pragma omp for
			for(long i=0;i<2*nx;i++)	mgl_fft((double *)(a+i), 2*nx, 2*ny, wty, wsy, true);
			mgl_fft_free_thr(wsx);	mgl_fft_free_thr(wsy);
		}
		else
#pragma omp parallel
		{
			void *wsx = mgl_fft_alloc_thr(2*nx);
#pragma omp for
			for(long i=0;i<4*nx*ny;i++)	a[i] *= exp(hxy[i])*exp(-double(dmp[i]*dz));
#pragma omp for
			for(long i=0;i<2*ny;i++)	mgl_fft((double *)(a+i*2*nx), 1, 2*nx, wtx, wsx, false);
#pragma omp for
			for(long i=0;i<4*nx*ny;i++)	a[i] *= exp(huv[i]);
#pragma omp for
			for(long i=0;i<2*ny;i++)	mgl_fft((double *)(a+2*i*nx), 1, 2*nx, wtx, wsx, true);
			mgl_fft_free_thr(wsx);
		}
#pragma omp parallel for collapse(2)
		for(long i=0;i<nx;i++)	for(long j=0;j<ny;j++)	// save result
			res->a[k+nz*(i+nx*j)] = a[i+nx/2+2*nx*(j+ny/2)];
	}
	mgl_fft_free(wtx,0,0);	mgl_fft_free(wty,0,0);
	delete []a;		delete []dmp;
	delete []hxy;	delete []hxv;	delete []huy;	delete []huv;
	delete []hx;	delete []hy;	delete []hu;	delete []hv;
	gr->LoadState();
	return res;
}
//-----------------------------------------------------------------------------
HMDT MGL_EXPORT mgl_pde_solve(HMGL gr, const char *ham, HCDT ini_re, HCDT ini_im, mreal dz, mreal k0, const char *opt)
{
	HADT res = mgl_pde_solve_c(gr,ham,ini_re,ini_im,dz,k0,opt);
	HMDT out = mgl_datac_abs(res);	delete res;	return out;
}
//-----------------------------------------------------------------------------
struct mglOdeTxt	{	long n;	HMEX *eq;	const char *var;	};
void MGL_NO_EXPORT mgl_txt_func(const mreal *x, mreal *dx, void *par)
{
	mglOdeTxt *p=(mglOdeTxt *)par;
	mreal vars['z'-'a'+1];
	for(long i=0;i<p->n;i++)
	{	char ch = p->var[i];	if(ch>='a' && ch<='z')	vars[ch-'a']=x[i];	}
#pragma omp parallel for
	for(long i=0;i<p->n;i++)
		dx[i] = mgl_expr_eval_v(p->eq[i], vars);
}
HMDT MGL_EXPORT mgl_ode_solve_str(const char *func, const char *var, HCDT x0, mreal dt, mreal tmax)
{
	if(!var || !(*var) || !func)	return 0;
	long len = strlen(func);
	mglOdeTxt par;	par.var=var;
	par.n = strlen(var);
	par.eq = new HMEX[par.n];
	char *buf = new char[len+1], *f=buf, *g=f;	memcpy(buf,func,len+1);
	mreal *xx = new mreal[par.n];
	for(long i=0;i<par.n;i++)
	{
		xx[i] = x0?x0->vthr(i):0;
		for(long k=0;f[k];k++)	if(f[k]==';')
		{ g = f+k+1;	f[k]=0;	break;	}
		if(f==g)	g = f+strlen(f);
		par.eq[i] = mgl_create_expr(f);
		f = g;
	}
	HMDT res = mgl_ode_solve_ex(mgl_txt_func,par.n,xx,dt,tmax,&par,NULL);
	for(long i=0;i<par.n;i++)	mgl_delete_expr(par.eq[i]);
	delete []par.eq;	delete []buf;	delete []xx;
	return res;
}
HMDT MGL_EXPORT mgl_ode_solve(void (*func)(const mreal *x, mreal *dx, void *par), int n, const mreal *x0, mreal dt, mreal tmax, void *par)
{	return mgl_ode_solve_ex(func,n,x0,dt,tmax,par,0);	}
HMDT MGL_EXPORT mgl_ode_solve_ex(void (*func)(const mreal *x, mreal *dx, void *par), int n, const mreal *x0, mreal dt, mreal tmax, void *par, void (*bord)(mreal *x, const mreal *xp, void *par))
{
	if(tmax<dt)	return 0;	// nothing to do
	int nt = int(tmax/dt+0.5)+1;
	mglData *res=new mglData(n,nt);
	mreal *x=new mreal[n], *k1=new mreal[n], *k2=new mreal[n], *k3=new mreal[n], *v=new mreal[n], hh=dt/2;
	register long i,k;
	// initial conditions
	for(i=0;i<n;i++)	x[i] = res->a[i] = x0[i];
	// Runge Kutta scheme of 4th order
	for(k=1;k<nt;k++)
	{
		func(x,k1,par);
		for(i=0;i<n;i++)	v[i] = x[i]+k1[i]*hh;
		func(v,k2,par);
		for(i=0;i<n;i++)	v[i] = x[i]+k2[i]*hh;
		func(v,k3,par);
		for(i=0;i<n;i++)	{	v[i] = x[i]+k3[i]*dt;	k3[i] += k2[i];	}
		func(v,k2,par);
		for(i=0;i<n;i++)	x[i] += (k1[i]+k2[i]+2*k3[i])*dt/6;
		if(bord)	bord(x,res->a+n*(k-1),par);
		for(i=0;i<n;i++)	res->a[i+n*k] = x[i];
	}
	delete []x;	delete []k1;	delete []k2;	delete []k3;	delete []v;
	return res;
}
//-----------------------------------------------------------------------------
void MGL_NO_EXPORT mgl_ray3d(const mreal *in, mreal *out, void *par)
{
	mglFormula *eqs = (mglFormula *)par;
	const char *v="xyzpqvt";
	mreal var[MGL_VS];	memset(var,0,MGL_VS*sizeof(mreal));
	for(int i=0;i<7;i++)	var[v[i]-'a'] = in[i];
	out[0] = eqs->CalcD(var,'p');	out[3] = -eqs->CalcD(var,'x');
	out[1] = eqs->CalcD(var,'q');	out[4] = -eqs->CalcD(var,'y');
	out[2] = eqs->CalcD(var,'v');	out[5] = -eqs->CalcD(var,'z');
	out[7] = eqs->CalcD(var,'i');	out[6] = 1;
}
// Solve GO ray equation like dr/dt = d ham/dp, dp/dt = -d ham/dr where ham = ham(x,y,z,p,q,v,t) and px=p, py=q, pz=v. The starting point (at t=0) is r0, p0. Result is array of {x,y,z,p,q,v,t}
HMDT MGL_EXPORT mgl_ray_trace(const char *ham, mreal x0, mreal y0, mreal z0, mreal px, mreal py, mreal pz, mreal dt, mreal tmax)
{
	mglFormula eqs(ham);
	mreal in[8]={x0,y0,z0,px,py,pz,0,0};
	HMDT res = mgl_ode_solve(mgl_ray3d,8,in,dt,tmax,&eqs);
	mgl_data_set_id(res,"xyzpqvti");
	return res;
}
//-----------------------------------------------------------------------------
struct mgl_ap
{
	double x0,y0,z0,x1,y1,z1,x2,y2,z2;	// vectors {l, g1, g2}
	double t1,t2,ch,q1,q2,pt,dt,d1,d2;	// theta_{1,2}, chi, q_{1,2}, p_t, dtau, dq_{1,2}
	mgl_ap()	{	memset(this,0,sizeof(mgl_ap));	}
};
//-----------------------------------------------------------------------------
void MGL_NO_EXPORT mgl_init_ra(int n, int n7, const mreal *r, mgl_ap *ra)	// prepare some intermediate data for QO (3d case)
{
	register double tt;
	tt = hypot(r[n7]-r[0], r[n7+1]-r[1]);
	if(tt)
	{
		ra[0].x1 = (r[n7+1]-r[1])/tt;
		ra[0].y1 = (r[0]-r[n7])/tt;
		ra[0].z1 = 0;
	}
	else	{	ra[0].x1 = ra[0].y1 = 0;	ra[0].z1 = 1;	}
	ra[0].x0 = r[n7] - r[0];	ra[0].y0 = r[n7+1] - r[1];	ra[0].z0 = r[n7+2] - r[2];
	tt = sqrt(ra[0].x0*ra[0].x0 + ra[0].y0*ra[0].y0 + ra[0].z0*ra[0].z0);
	ra[0].x0 /= tt;	ra[0].y0 /= tt;	ra[0].z0 /= tt;
	ra[0].x2 = ra[0].y1*ra[0].z0 - ra[0].y0*ra[0].z1;	// vector g_2
	ra[0].y2 = ra[0].z1*ra[0].x0 - ra[0].z0*ra[0].x1;
	ra[0].z2 = ra[0].x1*ra[0].y0 - ra[0].x0*ra[0].y1;
	register long i;
	for(i=1;i<n;i++)	// NOTE: no parallel due to dependence on prev point!
	{
		ra[i].dt = r[6+n7*i] - r[6+n7*(i-1)];
		ra[i].x0 = r[n7*i]   - r[n7*(i-1)];		// NOTE: very rough formulas
		ra[i].y0 = r[n7*i+1] - r[n7*(i-1)+1];	// for corresponding with dt one
		ra[i].z0 = r[n7*i+2] - r[n7*(i-1)+2];	// for corresponding with dt one
		tt = sqrt(ra[i].x0*ra[i].x0 + ra[i].y0*ra[i].y0 + ra[i].z0*ra[i].z0);
		ra[i].x0 /= tt;	ra[i].y0 /= tt;	ra[i].z0 /= tt;
		ra[i].ch = tt/ra[i].dt;
		tt = ra[i].x0*ra[i-1].x1 + ra[i].y0*ra[i-1].y1 + ra[i].z0*ra[i-1].z1;
		ra[i].x1 = ra[i-1].x1 - tt*ra[i].x0;	// vector g_1
		ra[i].y1 = ra[i-1].y1 - tt*ra[i].y0;
		ra[i].z1 = ra[i-1].z1 - tt*ra[i].z0;
		ra[i].t1 = tt/(ra[i].dt*ra[i].ch);
		tt = sqrt(ra[i].x1*ra[i].x1 + ra[i].y1*ra[i].y1 + ra[i].z1*ra[i].z1);
		ra[i].x1 /= tt;	ra[i].y1 /= tt;	ra[i].z1 /= tt;	// norm for reducing numeric error
		ra[i].x2 = ra[i].y1*ra[i].z0 - ra[i].y0*ra[i].z1;	// vector g_2
		ra[i].y2 = ra[i].z1*ra[i].x0 - ra[i].z0*ra[i].x1;
		ra[i].z2 = ra[i].x1*ra[i].y0 - ra[i].x0*ra[i].y1;
		tt = ra[i].x0*ra[i-1].x2 + ra[i].y0*ra[i-1].y2 + ra[i].z0*ra[i-1].z2;
		ra[i].t2 = tt/(ra[i].dt*ra[i].ch);
		// other parameters
		ra[i].pt = r[n7*i+3]*ra[i].x0 + r[n7*i+4]*ra[i].y0 + r[n7*i+5]*ra[i].z0;
		ra[i].q1 = r[n7*i+3]*ra[i].x1 + r[n7*i+4]*ra[i].y1 + r[n7*i+5]*ra[i].z1;
		ra[i].q2 = r[n7*i+3]*ra[i].x2 + r[n7*i+4]*ra[i].y2 + r[n7*i+5]*ra[i].z2;
		ra[i].d1 = (ra[i].q1-ra[i-1].q1)/(ra[i].dt*ra[i].ch);
		ra[i].d2 = (ra[i].q2-ra[i-1].q2)/(ra[i].dt*ra[i].ch);
	}
	memcpy(ra,ra+1,sizeof(mgl_ap));	// setup zero point
	ra[0].pt = r[3]*ra[0].x0 + r[4]*ra[0].y0 + r[5]*ra[0].z0;
	ra[0].q1 = r[3]*ra[0].x1 + r[4]*ra[0].y1 + r[5]*ra[0].z1;
	ra[0].q2 = r[3]*ra[0].x2 + r[4]*ra[0].y2 + r[5]*ra[0].z2;
}
//-----------------------------------------------------------------------------
//
//		QO2d series
//
//-----------------------------------------------------------------------------
struct mgl_qo2d_ham
{
	ddual *hx, *hu, *a, h0;
	double *dmp, dr, dk;
	mreal *r;
	mgl_ap *ra;
	ddual (*ham)(mreal u, mreal x, mreal y, mreal px, mreal py, void *par);
	void *par;
};
//-----------------------------------------------------------------------------
MGL_NO_EXPORT void *mgl_qo2d_hprep(void *par)
{
	mglThreadD *t=(mglThreadD *)par;
	mgl_qo2d_ham *f = (mgl_qo2d_ham *)t->v;
	mgl_ap *ra = f->ra;

	const mreal *r = f->r;
	long nx=t->n;
#if !MGL_HAVE_PTHREAD
#pragma omp parallel for
#endif
	for(long i=t->id;i<nx;i+=mglNumThr)
	{
		// x terms
		register mreal x1 = (2*i-nx+1)*f->dr, hh = 1 - ra->t1*x1;
		hh = sqrt(sqrt(0.041+hh*hh*hh*hh));
		register mreal tt = (ra->pt + ra->d1*x1)/hh - ra->pt;
		f->hx[i] = f->ham(abs(f->a[i]), r[0]+ra->x1*x1, r[1]+ra->y1*x1, r[3]+ra->x0*tt, r[4]+ra->y0*tt, f->par) - f->h0/2.;
		// u-y terms
		x1 = f->dk/2*(i<nx/2 ? i:i-nx);
		f->hu[i] = f->ham(0, r[0], r[1], r[3]+ra->x1*x1, r[4]+ra->y1*x1, f->par) - f->h0/2.;

		if(imag(f->hx[i])>0)	f->hx[i] = f->hx[i].real();
		if(imag(f->hu[i])>0)	f->hu[i] = f->hu[i].real();
		// add boundary conditions for x-direction
		f->hx[i] -= dual(0,f->dmp[i]);
	}
	return 0;
}
//-----------------------------------------------------------------------------
HADT MGL_EXPORT mgl_qo2d_func_c(ddual (*ham)(mreal u, mreal x, mreal y, mreal px, mreal py, void *par), void *par, HCDT ini_re, HCDT ini_im, HCDT ray_dat, mreal r, mreal k0, HMDT xx, HMDT yy)
{
	const mglData *ray=dynamic_cast<const mglData *>(ray_dat);	// NOTE: Ray must be mglData!
	if(!ray)	return 0;
	long nx=ini_re->GetNx(), nt=ray->ny, n7=ray->nx;
	if(nx<2 || ini_im->GetNx()!=nx || nt<2)	return 0;
	mglDataC *res=new mglDataC(nx,nt,1);

	ddual *a=new ddual[2*nx], *hu=new ddual[2*nx],  *hx=new ddual[2*nx];
	double *dmp=new double[2*nx];
	mgl_ap *ra = new mgl_ap[nt];	mgl_init_ra(nt, n7, ray->a, ra);	// ray

	mreal dr = r/(nx-1), dk = M_PI*(nx-1)/(k0*r*nx);
	memset(dmp,0,2*nx*sizeof(double));
	for(long i=0;i<nx/2;i++)	// prepare damping
	{
		register mreal x1 = (nx/2-i)/(nx/2.);
		dmp[2*nx-1-i] = dmp[i] = 30*GAMMA*x1*x1/k0;
	}
	for(long i=0;i<nx;i++)	a[i+nx/2] = dual(ini_re->v(i),ini_im->v(i));	// init
	void *wsx, *wtx = mgl_fft_alloc(2*nx,&wsx,1);
	if(xx && yy)	{	xx->Create(nx,nt);	yy->Create(nx,nt);	}

	mgl_qo2d_ham tmp;	// parameters for Hamiltonian calculation
	tmp.hx=hx;	tmp.hu=hu;	tmp.dmp=dmp;	tmp.par=par;
	tmp.dr=dr;	tmp.dk=dk;	tmp.ham=ham;	tmp.a=a;
	// start calculation
	for(long k=0;k<nt;k++)
	{
		for(long i=0;i<nx;i++)	// "save"
			res->a[i+k*nx]=a[i+nx/2]*sqrt(ra[0].ch/ra[k].ch);
		if(xx && yy)	for(long i=0;i<nx;i++)	// prepare xx, yy
		{
			register mreal x1 = (2*i-nx+1)*dr;
			xx->a[i+k*nx] = ray->a[n7*k] + ra[k].x1*x1;	// new coordinates
			yy->a[i+k*nx] = ray->a[n7*k+1] + ra[k].y1*x1;
		}
		tmp.r=ray->a+n7*k;	tmp.ra=ra+k;
		mreal hh = ra[k].pt*(1/sqrt(sqrt(1.041))-1);	// 0.041=0.45^4 -- minimal value of h
		tmp.h0 = ham(0, tmp.r[0], tmp.r[1], tmp.r[3]+ra[k].x0*hh, tmp.r[4]+ra[k].x0*hh, par);
		mglStartThread(mgl_qo2d_hprep,0,2*nx,0,0,0,0,&tmp);
		// Step for field
		ddual dt = ddual(0, -ra[k].dt*k0);
		for(long i=0;i<2*nx;i++)	a[i] *= exp(hx[i]*dt);
		mgl_fft((double *)a, 1, 2*nx, wtx, wsx, false);
		for(long i=0;i<2*nx;i++)	a[i] *= exp(hu[i]*dt);
		mgl_fft((double *)a, 1, 2*nx, wtx, wsx, true);

/*		// Calculate B1			// TODO make more general scheme later!!!
		hh = ra[k].pt*(1/sqrt(sqrt(1.041))-1);
		var['x'-'a'] = ray->a[n7*k];	// new coordiantes
		var['y'-'a'] = ray->a[n7*k+1];
		var['p'-'a'] = ray->a[n7*k+3] + ra[k].x0*hh;	// new momentums
		var['q'-'a'] = ray->a[n7*k+4] + ra[k].y0*hh;
		tt = h.CalcD(var,'p')*ra[k].x1 + h.CalcD(var,'q')*ra[k].y1;
		var['x'-'a'] = ray->a[n7*k] + ra[k].x1*dr;	// new coordiantes
		var['y'-'a'] = ray->a[n7*k+1] + ra[k].y1*dr;
		hh = 1 - ra[k].t1*dr;	hh = sqrt(sqrt(0.041+hh*hh*hh*hh));
		hh = (ra[k].ch*ra[k].pt + ra[k].d1*dr)/(hh*ra[k].ch) - ra[k].pt;
		var['p'-'a'] = ray->a[n7*k+3] + ra[k].x0*hh;	// new momentums
		var['q'-'a'] = ray->a[n7*k+4] + ra[k].y0*hh;
		B1 = h.CalcD(var,'p')*ra[k].x1 + h.CalcD(var,'q')*ra[k].y1;
		B1 = (B1-tt)/dr;
		double a1=0, a2=0;
		for(i=0;i<2*nx;i++)	a1 += norm(a[i]);
		hx[0] = hx[2*nx-1] = 0.;
		for(i=1;i<2*nx-1;i++)	hx[i] = (B1*ra[k].dt*(i-nx))*(a[i+1]-a[i-1]);
		for(i=0;i<2*nx;i++)	{	a[i] += hx[i];	a2 += norm(a[i]);	}
		a1 = sqrt(a1/a2);
		for(i=0;i<2*nx;i++)	a[i] *= a1;*/
	}
	mgl_fft_free(wtx,&wsx,1);
	delete []a;		delete []hu;	delete []hx;	delete []ra;	delete []dmp;
	return res;
}
//-----------------------------------------------------------------------------
HMDT MGL_EXPORT mgl_qo2d_func(ddual (*ham)(mreal u, mreal x, mreal y, mreal px, mreal py, void *par), void *par, HCDT ini_re, HCDT ini_im, HCDT ray_dat, mreal r, mreal k0, HMDT xx, HMDT yy)
{
	HADT res = mgl_qo2d_func_c(ham,par,ini_re,ini_im,ray_dat,r,k0,xx,yy);
	HMDT out = mgl_datac_abs(res);	delete res;	return out;
}
//-----------------------------------------------------------------------------
ddual MGL_NO_EXPORT mgl_ham2d(mreal u, mreal x, mreal y, mreal px, mreal py, void *par)
{
	mglFormula *h = (mglFormula *)par;
	mreal var[MGL_VS];	memset(var,0,MGL_VS*sizeof(mreal));
	var['x'-'a'] = x;	var['y'-'a'] = y;	var['u'-'a'] = u;
	var['p'-'a'] = px;	var['q'-'a'] = py;
	return ddual(h->Calc(var), -h->CalcD(var,'i'));
}
//-----------------------------------------------------------------------------
HADT MGL_EXPORT mgl_qo2d_solve_c(const char *ham, HCDT ini_re, HCDT ini_im, HCDT ray_dat, mreal r, mreal k0, HMDT xx, HMDT yy)
{
	mglFormula h(ham);
	return mgl_qo2d_func_c(mgl_ham2d, &h, ini_re, ini_im, ray_dat, r, k0, xx, yy);
}
//-----------------------------------------------------------------------------
HMDT MGL_EXPORT mgl_qo2d_solve(const char *ham, HCDT ini_re, HCDT ini_im, HCDT ray_dat, mreal r, mreal k0, HMDT xx, HMDT yy)
{
	HADT res = mgl_qo2d_solve_c(ham,ini_re,ini_im,ray_dat,r,k0,xx,yy);
	HMDT out = mgl_datac_abs(res);	delete res;	return out;
}
//-----------------------------------------------------------------------------
//
//		QO2d series
//
//-----------------------------------------------------------------------------
struct mgl_qo3d_ham
{
	ddual *hxy, *huv, *hxv, *huy, *a;
	ddual *hx, *hy, *hu, *hv, h0;
	double *dmp, dr, dk;
	mreal *r;
	mgl_ap *ra;
	ddual (*ham)(mreal u, mreal x, mreal y, mreal z, mreal px, mreal py, mreal pz, void *par);
	void *par;
};
//-----------------------------------------------------------------------------
MGL_NO_EXPORT void *mgl_qo3d_hprep(void *par)
{
	mglThreadD *t=(mglThreadD *)par;
	mgl_qo3d_ham *f = (mgl_qo3d_ham *)t->v;
	mgl_ap *ra = f->ra;
	const mreal *r = f->r;
	long nx=t->n;
#if !MGL_HAVE_PTHREAD
#pragma omp parallel for
#endif
	for(long ii=t->id;ii<nx*nx;ii+=mglNumThr)
	{
		register long i = ii%nx, j = ii/nx;
		// x-y terms
		register mreal x1 = (2*i-nx+1)*f->dr, x2 = (2*j-nx+1)*f->dr, hh = 1-ra->t1*x1-ra->t2*x2;
		hh = sqrt(sqrt(0.041+hh*hh*hh*hh));
		register mreal tt = (ra->pt + ra->d1*x1 + ra->d2*x2)/hh - ra->pt;
		f->hxy[ii] = f->ham(abs(f->a[i]), r[0]+ra->x1*x1+ra->x2*x2, r[1]+ra->y1*x1+ra->y2*x2, r[2]+ra->z1*x1+ra->z2*x2, r[3]+ra->x0*tt, r[4]+ra->y0*tt, r[5]+ra->z0*tt, f->par);
		// x-v terms
		x1 = (2*i-nx+1)*f->dr;	x2 = f->dk/2*(j<nx/2 ? j:j-nx);	hh = 1-ra->t1*x1;
		hh = sqrt(sqrt(0.041+hh*hh*hh*hh));
		tt = (ra->pt + ra->d1*x1)/hh - ra->pt;
		f->hxv[ii] = f->ham(0, r[0]+ra->x1*x1, r[1]+ra->y1*x1, r[2]+ra->z1*x1, r[3]+ra->x0*tt+ra->x2*x2, r[4]+ra->y0*tt+ra->y2*x2, r[5]+ra->z0*tt+ra->z2*x2, f->par);
		// u-y terms
		x1 = f->dk/2*(i<nx/2 ? i:i-nx);	x2 = (2*j-nx+1)*f->dr;	hh = 1-ra->t2*x2;
		hh = sqrt(sqrt(0.041+hh*hh*hh*hh));
		tt = (ra->pt + ra->d2*x2)/hh - ra->pt;
		f->huy[ii] = f->ham(0, r[0]+ra->x2*x2, r[1]+ra->y2*x2, r[2]+ra->z2*x2, r[3]+ra->x1*x1+ra->x0*tt, r[4]+ra->y1*x1+ra->y0*tt, r[5]+ra->z1*x1+ra->z0*tt, f->par);
		// u-y terms
		x1 = f->dk/2*(i<nx/2 ? i:i-nx);	x2 = f->dk/2*(j<nx/2 ? j:j-nx);
		f->huv[ii] = f->ham(0, r[0], r[1], r[2], r[3]+ra->x1*x1+ra->x2*x2, r[4]+ra->y1*x1+ra->y2*x2, r[5]+ra->z1*x1+ra->z2*x2, f->par);
	}
	return 0;
}
//-----------------------------------------------------------------------------
MGL_NO_EXPORT void *mgl_qo3d_post(void *par)
{
	mglThreadD *t=(mglThreadD *)par;
	mgl_qo3d_ham *f = (mgl_qo3d_ham *)t->v;
	long nx=t->n;
#if !MGL_HAVE_PTHREAD
#pragma omp parallel for
#endif
	for(long ii=t->id;ii<nx*nx;ii+=mglNumThr)
	{
		register long i = ii%nx, j = ii/nx;
		f->hxy[ii] -= (f->hx[i]+f->hy[j]-f->h0/2.)/2.;
		if(imag(f->hxy[ii])>0)	f->hxy[ii] = f->hxy[ii].real();
		f->hxv[ii] -= (f->hx[i]+f->hv[j]-f->h0/2.)/2.;
		if(imag(f->hxv[ii])>0)	f->hxv[ii] = f->hxv[ii].real();
		f->huy[ii] -= (f->hu[i]+f->hy[j]-f->h0/2.)/2.;
		if(imag(f->huy[ii])>0)	f->huy[ii] = f->huy[ii].real();
		f->huv[ii] -= (f->hu[i]+f->hv[j]-f->h0/2.)/2.;
		if(imag(f->huv[ii])>0)	f->huv[ii] = f->huv[ii].real();
		// add boundary conditions for x-direction
		f->hxy[ii] -= dual(0,f->dmp[ii]);
	}
	return 0;
}
//-----------------------------------------------------------------------------
HADT MGL_EXPORT mgl_qo3d_func_c(ddual (*ham)(mreal u, mreal x, mreal y, mreal z, mreal px, mreal py, mreal pz, void *par), void *par, HCDT ini_re, HCDT ini_im, HCDT ray_dat, mreal r, mreal k0, HMDT xx, HMDT yy, HMDT zz)
{
	const mglData *ray=dynamic_cast<const mglData *>(ray_dat);	// NOTE: Ray must be mglData!
	if(!ray)	return 0;
	long nx=ini_re->GetNx(), nt=ray->ny, n7=ray->nx;	// NOTE: only square grids are supported now (for simplicity)
	if(nx<2 || ini_re->GetNx()!=nx || ini_im->GetNx()*ini_im->GetNy()!=nx*nx || nt<2)	return 0;
	mglDataC *res=new mglDataC(nx,nx,nt);

	ddual *a=new ddual[4*nx*nx], *huv=new ddual[4*nx*nx],  *hxy=new ddual[4*nx*nx], *huy=new ddual[4*nx*nx],  *hxv=new ddual[4*nx*nx];
	ddual *hu=new ddual[2*nx],  *hx=new ddual[2*nx], *hy=new ddual[2*nx],  *hv=new ddual[2*nx];
	double *dmp=new double[4*nx*nx];
	mgl_ap *ra = new mgl_ap[nt];
	mgl_init_ra(nt, n7, ray->a, ra);	// prepare ray

	double dr = r/(nx-1), dk = M_PI*(nx-1)/(k0*r*nx);
	memset(dmp,0,4*nx*nx*sizeof(double));
#pragma omp parallel for collapse(2)
	for(long i=0;i<nx/2;i++)	for(long j=0;j<nx/2;j++)	// prepare damping
	{
		register double x1 = (nx/2-i)/(nx/2.), x2 = (nx/2-j)/(nx/2.);
		dmp[2*nx-1-i] = dmp[i] = 30*GAMMA*x1*x1/k0;
		dmp[(2*nx-1-j)*2*nx] += 30*GAMMA*x2*x2/k0;
		dmp[j*2*nx] += 30*GAMMA*x2*x2/k0;
	}
#pragma omp parallel for collapse(2)
	for(long i=0;i<nx;i++)	for(long j=0;j<nx;j++)	// init
		a[i+nx/2+2*nx*(j+nx/2)] = dual(ini_re->v(i,j),ini_im->v(i,j));
	void *wtx = mgl_fft_alloc(2*nx,0,0);
	if(xx && yy && zz)	{	xx->Create(nx,nx,nt);	yy->Create(nx,nx,nt);	zz->Create(nx,nx,nt);	}

	mgl_qo3d_ham tmp;	// parameters for Hamiltonian calculation
	tmp.hxy=hxy;	tmp.hx=hx;	tmp.huv=huv;	tmp.hu=hu;
	tmp.huy=huy;	tmp.hy=hy;	tmp.hxv=hxv;	tmp.hv=hv;
	tmp.dmp=dmp;	tmp.par=par;
	tmp.dr=dr;	tmp.dk=dk;	tmp.ham=ham;	tmp.a=a;
	// start calculation
	for(long k=0;k<nt;k++)
	{
#pragma omp parallel for collapse(2)
		for(long i=0;i<nx;i++)	for(long j=0;j<nx;j++)	// "save"
			res->a[i+nx*(j+k*nx)]=a[i+nx/2+2*nx*(j+nx/2)]*sqrt(ra[0].ch/ra[k].ch);
		if(xx && yy && zz)
#pragma omp parallel for collapse(2)
			for(long i=0;i<nx;i++)	for(long j=0;j<nx;j++)	// prepare xx, yy, zz
			{
				register mreal x1 = (2*i-nx+1)*dr, x2 = (2*j-nx+1)*dr;
				xx->a[i+nx*(j+k*nx)] = ray->a[n7*k] + ra[k].x1*x1 + ra[k].x2*x2;	// new coordinates
				yy->a[i+nx*(j+k*nx)] = ray->a[n7*k+1] + ra[k].y1*x1 + ra[k].y2*x2;
				zz->a[i+nx*(j+k*nx)] = ray->a[n7*k+2] + ra[k].z1*x1 + ra[k].z2*x2;
			}
		tmp.r=ray->a+n7*k;	tmp.ra=ra+k;
		mglStartThread(mgl_qo3d_hprep,0,2*nx,0,0,0,0,&tmp);	tmp.h0 = huv[0];
		for(long i=0;i<2*nx;i++)	// fill intermediate arrays
		{
			tmp.hx[i] = hxv[i];	tmp.hy[i] = huy[i*2*nx];
			tmp.hv[i] = huv[i];	tmp.hu[i] = huv[i*2*nx];
		}
		mglStartThread(mgl_qo3d_post,0,2*nx,0,0,0,0,&tmp);
		// Step for field
		ddual dt = ddual(0, -ra[k].dt*k0);
#pragma omp parallel
		{
			void *wsx = mgl_fft_alloc_thr(2*nx);
#pragma omp for
			for(long i=0;i<4*nx*nx;i++)	a[i] *= exp(hxy[i]*dt);		// x-y
#pragma omp for
			for(long i=0;i<2*nx;i++)	// x->u
				mgl_fft((double *)(a+i*2*nx), 1, 2*nx, wtx, wsx, false);
#pragma omp for
			for(long i=0;i<4*nx*nx;i++)	a[i] *= exp(huy[i]*dt);		// u-y
#pragma omp for
			for(long i=0;i<2*nx;i++)	// y->v
				mgl_fft((double *)(a+i), 2*nx, 2*nx, wtx, wsx, false);
#pragma omp for
			for(long i=0;i<4*nx*nx;i++)	a[i] *= exp(huv[i]*dt);		// u-v
#pragma omp for
			for(long i=0;i<2*nx;i++)	// u->x
				mgl_fft((double *)(a+i*2*nx), 1, 2*nx, wtx, wsx, true);
#pragma omp for
			for(long i=0;i<4*nx*nx;i++)	a[i] *= exp(hxv[i]*dt);		// x-v
#pragma omp for
			for(long i=0;i<2*nx;i++)	// v->y
				mgl_fft((double *)(a+i), 2*nx, 2*nx, wtx, wsx, true);
			mgl_fft_free_thr(wsx);
		}

/*		// Calculate B1			// TODO make more general scheme later!!!
		hh = ra[k].pt*(1/sqrt(sqrt(1.041))-1);
		var['x'-'a'] = ray->a[n7*k];	// new coordiantes
		var['y'-'a'] = ray->a[n7*k+1];
		var['p'-'a'] = ray->a[n7*k+3] + ra[k].x0*hh;	// new momentums
		var['q'-'a'] = ray->a[n7*k+4] + ra[k].y0*hh;
		tt = h.CalcD(var,'p')*ra[k].x1 + h.CalcD(var,'q')*ra[k].y1;
		var['x'-'a'] = ray->a[n7*k] + ra[k].x1*dr;	// new coordiantes
		var['y'-'a'] = ray->a[n7*k+1] + ra[k].y1*dr;
		hh = 1 - ra[k].t1*dr;	hh = sqrt(sqrt(0.041+hh*hh*hh*hh));
		hh = (ra[k].ch*ra[k].pt + ra[k].d1*dr)/(hh*ra[k].ch) - ra[k].pt;
		var['p'-'a'] = ray->a[n7*k+3] + ra[k].x0*hh;	// new momentums
		var['q'-'a'] = ray->a[n7*k+4] + ra[k].y0*hh;
		B1 = h.CalcD(var,'p')*ra[k].x1 + h.CalcD(var,'q')*ra[k].y1;
		B1 = (B1-tt)/dr;
		double a1=0, a2=0;
		for(i=0;i<2*nx;i++)	a1 += norm(a[i]);
		hx[0] = hx[2*nx-1] = 0.;
		for(i=1;i<2*nx-1;i++)	hx[i] = (B1*ra[k].dt*(i-nx))*(a[i+1]-a[i-1]);
		for(i=0;i<2*nx;i++)	{	a[i] += hx[i];	a2 += norm(a[i]);	}
		a1 = sqrt(a1/a2);
		for(i=0;i<2*nx;i++)	a[i] *= a1;*/
	}
	mgl_fft_free(wtx,0,0);
	delete []a;		delete []ra;	delete []dmp;
	delete []huv;	delete []hxy;	delete []hxv;	delete []huy;
	delete []hu;	delete []hx;	delete []hv;	delete []hy;
	return res;
}
//-----------------------------------------------------------------------------
HMDT MGL_EXPORT mgl_qo3d_func(ddual (*ham)(mreal u, mreal x, mreal y, mreal z, mreal px, mreal py, mreal pz, void *par), void *par, HCDT ini_re, HCDT ini_im, HCDT ray_dat, mreal r, mreal k0, HMDT xx, HMDT yy, HMDT zz)
{
	HADT res = mgl_qo3d_func_c(ham,par,ini_re,ini_im,ray_dat,r,k0,xx,yy,zz);
	HMDT out = mgl_datac_abs(res);	delete res;	return out;
}
//-----------------------------------------------------------------------------
ddual MGL_NO_EXPORT mgl_ham3d(mreal u, mreal x, mreal y, mreal z, mreal px, mreal py, mreal pz, void *par)
{
	mglFormula *h = (mglFormula *)par;
	mreal var[MGL_VS];	memset(var,0,MGL_VS*sizeof(mreal));
	var['x'-'a'] = x;	var['y'-'a'] = y;	var['z'-'a'] = z;	var['u'-'a'] = u;
	var['p'-'a'] = px;	var['q'-'a'] = py;	var['v'-'a'] = pz;
	return ddual(h->Calc(var), -h->CalcD(var,'i'));
}
//-----------------------------------------------------------------------------
HADT MGL_EXPORT mgl_qo3d_solve_c(const char *ham, HCDT ini_re, HCDT ini_im, HCDT ray_dat, mreal r, mreal k0, HMDT xx, HMDT yy, HMDT zz)
{
	mglFormula h(ham);
	return mgl_qo3d_func_c(mgl_ham3d, &h, ini_re, ini_im, ray_dat, r, k0, xx, yy, zz);
}
//-----------------------------------------------------------------------------
HMDT MGL_EXPORT mgl_qo3d_solve(const char *ham, HCDT ini_re, HCDT ini_im, HCDT ray_dat, mreal r, mreal k0, HMDT xx, HMDT yy, HMDT zz)
{
	HADT res = mgl_qo3d_solve_c(ham,ini_re,ini_im,ray_dat,r,k0,xx,yy,zz);
	HMDT out = mgl_datac_abs(res);	delete res;	return out;
}
//-----------------------------------------------------------------------------
MGL_NO_EXPORT void *mgl_jacob2(void *par)
{
	mglThreadD *t=(mglThreadD *)par;
	long nx=t->p[0], ny=t->p[1];
	mreal *r=t->a;
	const mreal *x=t->b, *y=t->c;
#if !MGL_HAVE_PTHREAD
#pragma omp parallel for
#endif
	for(long i0=t->id;i0<t->n;i0+=mglNumThr)
	{
		register long i=i0%nx, j=i0/nx;
		register long ip = i<nx-1 ? 1:0, jp = j<ny-1 ? nx:0;
		register long im = i>0 ? -1:0, jm = j>0 ? -nx:0;
		r[i0] = (x[i0+ip]-x[i0+im])*(y[i0+jp]-y[i0+jm]) -
				(y[i0+ip]-y[i0+im])*(x[i0+jp]-x[i0+jm]);
		r[i0] *= mreal((nx-1)*(ny-1)) / ((ip-im)*(jp-jm));
	}
	return 0;
}
HMDT MGL_EXPORT mgl_jacobian_2d(HCDT x, HCDT y)
{
	int nx = x->GetNx(), ny=x->GetNy();
	if(nx!=y->GetNx() || ny!=y->GetNy() || nx<2 || ny<2)	return	0;
	mglData *r=new mglData(nx,ny,1);
	const mglData *xx=dynamic_cast<const mglData *>(x);
	const mglData *yy=dynamic_cast<const mglData *>(y);
	if(xx && yy)
	{
		long p[2]={nx,ny};
		mglStartThread(mgl_jacob2,0,nx*ny,r->a,xx->a,yy->a,p);
	}
	else	// slow variant
	{
#pragma omp parallel for collapse(2)
		for(long j=0;j<ny;j++)	for(long i=0;i<nx;i++)
		{
			register long im = i>0 ? i-1:i, ip = i<nx-1 ? i+1:i;
			register long jm = j>0 ? j-1:j, jp = j<ny-1 ? j+1:j;
			r->a[i+nx*j] = (x->v(ip,j)-x->v(im,j))*(y->v(i,jp)-y->v(i,jm)) -
						(y->v(ip,j)-y->v(im,j))*(x->v(i,jp)-x->v(i,jm));
			r->a[i+nx*j] *= mreal((nx-1)*(ny-1)) / ((ip-im)*(jp-jm));
		}
	}
	return r;
}
//-----------------------------------------------------------------------------
MGL_NO_EXPORT void *mgl_jacob3(void *par)
{
	mglThreadD *t=(mglThreadD *)par;
	long nx=t->p[0], ny=t->p[1], nz=t->p[2];
	mreal *r=t->a;
	const mreal *x=t->b, *y=t->c, *z=t->d;
#if !MGL_HAVE_PTHREAD
#pragma omp parallel for
#endif
	for(long i0=t->id;i0<t->n;i0+=mglNumThr)
	{
		register long i=i0%nx, j=(i0/nx)%ny, k=i0/(nx*ny);
		register long ip = i<nx-1 ? 1:0, jp = j<ny-1 ? nx:0, kp = k<nz-1 ? nx*ny:0;
		register long im = i>0 ? -1:0, jm = j>0 ? -nx:0, km = k>0 ? -nx*ny:0;
		r[i0] = (x[i0+ip]-x[i0+im])*(y[i0+jp]-y[i0+jm])*(z[i0+kp]-z[i0+km]) -
				(x[i0+ip]-x[i0+im])*(y[i0+kp]-y[i0+km])*(z[i0+jp]-z[i0+jm]) -
				(x[i0+jp]-x[i0+jm])*(y[i0+ip]-y[i0+im])*(z[i0+kp]-z[i0+km]) +
				(x[i0+jp]-x[i0+jm])*(y[i0+kp]-y[i0+km])*(z[i0+ip]-z[i0+im]) +
				(x[i0+kp]-x[i0+km])*(y[i0+ip]-y[i0+im])*(z[i0+jp]-z[i0+jm]) -
				(x[i0+kp]-x[i0+km])*(y[i0+jp]-y[i0+jm])*(z[i0+ip]-z[i0+im]);
		r[i0] *= mreal((nx-1)*(ny-1)*(nz-1)) / ((ip-im)*(jp-jm)*(kp-km));
	}
	return 0;
}
HMDT MGL_EXPORT mgl_jacobian_3d(HCDT x, HCDT y, HCDT z)
{
	int nx = x->GetNx(), ny=x->GetNy(), nz=x->GetNz(), nn = nx*ny*nz;
	if(nx<2 || ny<2 || nz<2)	return 0;
	if(nn!=y->GetNN() || nn!=z->GetNN())	return 0;
	mglData *r=new mglData(nx,ny,nz);
	const mglData *xx=dynamic_cast<const mglData *>(x);
	const mglData *yy=dynamic_cast<const mglData *>(y);
	const mglData *zz=dynamic_cast<const mglData *>(z);
	if(xx && yy && zz)
	{
		long p[3]={nx,ny,nz};
		mglStartThread(mgl_jacob3,0,nx*ny*nz,r->a,xx->a,yy->a,p,0,zz->a);
	}
	else	// slow variant
	{
#pragma omp parallel for collapse(3)
		for(long k=0;k<nz;k++)	for(long j=0;j<ny;j++)	for(long i=0;i<nx;i++)
		{
			register long im = i>0 ? i-1:i, ip = i<nx-1 ? i+1:i;
			register long jm = j>0 ? j-1:j, jp = j<ny-1 ? j+1:j;
			register long km = k>0 ? k-1:k, kp = k<nz-1 ? k+1:k;
			register long i0 = i+nx*(j+ny*k);
			r->a[i0] = (x->v(ip,j,k)-x->v(im,j,k))*(y->v(i,jp,k)-y->v(i,jm,k))*(z->v(i,j,kp)-z->v(i,j,km)) -
					(x->v(ip,j,k)-x->v(im,j,k))*(y->v(i,j,kp)-y->v(i,j,km))*(z->v(i,jp,k)-z->v(i,jm,k)) -
					(x->v(i,jp,k)-x->v(i,jm,k))*(y->v(ip,j,k)-y->v(im,j,k))*(z->v(i,j,kp)-z->v(i,j,km)) +
					(x->v(i,jp,k)-x->v(i,jm,k))*(y->v(i,j,kp)-y->v(i,j,km))*(z->v(ip,j,k)-z->v(im,j,k)) +
					(x->v(i,j,kp)-x->v(i,j,km))*(y->v(ip,j,k)-y->v(im,j,k))*(z->v(i,jp,k)-z->v(i,jm,k)) -
					(x->v(i,j,kp)-x->v(i,j,km))*(y->v(i,jp,k)-y->v(i,jm,k))*(z->v(ip,j,k)-z->v(im,j,k));
			r->a[i0] *= mreal((nx-1)*(ny-1)*(nz-1)) / ((ip-im)*(jp-jm)*(kp-km));
		}

	}
	return r;
}
//-----------------------------------------------------------------------------
uintptr_t MGL_EXPORT mgl_pde_solve_c_(uintptr_t* gr, const char *ham, uintptr_t* ini_re, uintptr_t* ini_im, mreal *dz, mreal *k0, const char *opt, int l, int lo)
{	char *s=new char[l+1];	memcpy(s,ham,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	uintptr_t res = uintptr_t(mgl_pde_solve_c(_GR_, s, _DA_(ini_re), _DA_(ini_im), *dz, *k0, o));
	delete []o;	delete []s;	return res;	}
uintptr_t MGL_EXPORT mgl_pde_solve_(uintptr_t* gr, const char *ham, uintptr_t* ini_re, uintptr_t* ini_im, mreal *dz, mreal *k0, const char *opt, int l, int lo)
{	char *s=new char[l+1];	memcpy(s,ham,l);	s[l]=0;
	char *o=new char[lo+1];	memcpy(o,opt,lo);	o[lo]=0;
	uintptr_t res = uintptr_t(mgl_pde_solve(_GR_, s, _DA_(ini_re), _DA_(ini_im), *dz, *k0, o));
	delete []o;	delete []s;	return res;	}
uintptr_t MGL_EXPORT mgl_ray_trace_(const char *ham, mreal *x0, mreal *y0, mreal *z0, mreal *px, mreal *py, mreal *pz, mreal *dt, mreal *tmax,int l)
{	char *s=new char[l+1];	memcpy(s,ham,l);	s[l]=0;
	uintptr_t res = uintptr_t(mgl_ray_trace(s, *x0,*y0,*z0, *px,*py,*pz, *dt,*tmax));
	delete []s;	return res;	}
uintptr_t MGL_EXPORT mgl_qo2d_solve_(const char *ham, uintptr_t* ini_re, uintptr_t* ini_im, uintptr_t* ray, mreal *r, mreal *k0, uintptr_t* xx, uintptr_t* yy, int l)
{	char *s=new char[l+1];	memcpy(s,ham,l);	s[l]=0;
	uintptr_t res = uintptr_t(mgl_qo2d_solve(s, _DA_(ini_re), _DA_(ini_im), _DA_(ray), *r, *k0, _DM_(xx), _DM_(yy)));
	delete []s;	return res;	}
uintptr_t MGL_EXPORT mgl_qo3d_solve_(const char *ham, uintptr_t* ini_re, uintptr_t* ini_im, uintptr_t* ray, mreal *r, mreal *k0, uintptr_t* xx, uintptr_t* yy, uintptr_t* zz, int l)
{	char *s=new char[l+1];	memcpy(s,ham,l);	s[l]=0;
	uintptr_t res = uintptr_t(mgl_qo3d_solve(s, _DA_(ini_re), _DA_(ini_im), _DA_(ray), *r, *k0, _DM_(xx), _DM_(yy), _DM_(zz)));
	delete []s;	return res;	}
uintptr_t MGL_EXPORT mgl_jacobian_2d_(uintptr_t* x, uintptr_t* y)
{	return uintptr_t(mgl_jacobian_2d(_DA_(x), _DA_(y)));	}
uintptr_t MGL_EXPORT mgl_jacobian_3d_(uintptr_t* x, uintptr_t* y, uintptr_t* z)
{	return uintptr_t(mgl_jacobian_3d(_DA_(x), _DA_(y), _DA_(z)));	}
//-----------------------------------------------------------------------------
