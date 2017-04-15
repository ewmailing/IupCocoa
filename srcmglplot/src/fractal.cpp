/***************************************************************************
*pixel.cpp is part of Math Graphic Library
*Copyright (C) 2007-2016 Alexey Balakin <mathgl.abalakin@gmail.ru>       *
*                                                                        *
*  This program is free software; you can redistribute it and/or modify  *
*  it under the terms of the GNU Library General Public License as       *
*  published by the Free Software Foundation; either version 3 of the    *
*  License, or (at your option) any later version.                       *
*                                                                        *
*  This program is distributed in the hope that it will be useful,       *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*  GNU General Public License for more details.                          *
*                                                                        *
*  You should have received a copy of the GNU Library General Public     *
*  License along with this program; if not, write to the                 *
*  Free Software Foundation, Inc.,                                       *
*  59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "mgl2/other.h"
#include "mgl2/data.h"
MGL_NO_EXPORT char *mgl_read_gz(gzFile fp);
//-----------------------------------------------------------------------------
//
//	IFS series
//
//-----------------------------------------------------------------------------
void MGL_NO_EXPORT mgl_ifs_2d_point(HCDT A, mreal& x, mreal& y, mreal amax)
{
	long i, n=A->GetNy();
	mreal r = amax*mgl_rnd(), sum_prob = 0, x1;
	for(i=0;i<n;i++)
	{
		sum_prob += A->v(6,i);
		if(r<sum_prob)	break;
	}
	x1= A->v(0,i)*x + A->v(1,i)*y + A->v(4,i);
	y = A->v(2,i)*x + A->v(3,i)*y + A->v(5,i);	x = x1;
}
HMDT MGL_EXPORT mgl_data_ifs_2d(HCDT A, long n, long skip)
{
	if(!A || A->GetNx()<7 || n<1)	return 0;	// incompatible dimensions
	mreal amax=0;
	for(long i=0; i<A->GetNy(); i++)	amax += A->v(6,i);
	if(amax<=0)	return 0;
	
	mglData *f = new mglData(2,n);
	mreal x = 0, y = 0;
	for(long i=0; i<skip; i++)	mgl_ifs_2d_point(A, x, y,amax);
	for(long i=0; i<n; i++)
	{
		mgl_ifs_2d_point(A, x, y, amax);
		f->a[2*i] = x;	f->a[2*i+1] = y;
	}
	return f;
}
uintptr_t MGL_EXPORT mgl_data_ifs_2d_(uintptr_t *d, long *n, long *skip)
{	return uintptr_t(mgl_data_ifs_2d(_DT_,*n,*skip));	}
//-----------------------------------------------------------------------------
void MGL_NO_EXPORT mgl_ifs_3d_point(HCDT A, mreal& x, mreal& y, mreal& z, mreal amax)
{
	int i, n=A->GetNy();
	mreal r = amax*mgl_rnd(), sum_prob = 0, x1, y1;
	for (i=0; i<n; i++)
	{
		sum_prob += A->v(12,i);
		if(r < sum_prob)  break;
	}
	x1= A->v(0,i)*x + A->v(1,i)*y + A->v(2,i)*z + A->v(9,i);
	y1= A->v(3,i)*x + A->v(4,i)*y + A->v(5,i)*z + A->v(10,i);
	z = A->v(6,i)*x + A->v(7,i)*y + A->v(8,i)*z + A->v(11,i);
	x = x1;	y = y1;
}
HMDT MGL_EXPORT mgl_data_ifs_3d(HCDT A, long n, long skip)
{
	if(!A || A->GetNx()<13 || n<1)	return 0;   // incompatible dimensions
	mreal amax = 0;
	for(int i=0; i<A->GetNy(); i++)	amax += A->v(12,i);
	if(amax <= 0) return 0;
	
	mglData *f = new mglData(3,n);
	mreal x = 0, y = 0, z = 0;
	for(long i=0; i<skip; i++)	mgl_ifs_3d_point(A, x, y, z, amax);
	for(long i=0; i<n; i++)
	{
		mgl_ifs_3d_point(A, x, y, z, amax);
		f->a[3*i] = x;	f->a[3*i+1] = y;	f->a[3*i+2] = z;
	}
	return f;
}
uintptr_t MGL_EXPORT mgl_data_ifs_3d_(uintptr_t *d, long *n, long *skip)
{   return uintptr_t(mgl_data_ifs_3d(_DT_,*n,*skip));   }
//-----------------------------------------------------------------------------
HMDT MGL_EXPORT mgl_data_ifs_file(const char *fname, const char *name, long n, long skip)
{
	gzFile fp = gzopen(fname,"r");
	if(!fp)	return 0;		// Couldn't open file file
	char *buf = mgl_read_gz(fp);	gzclose(fp);
	char *s = (char*)strstr(buf,name);
	if(!s)	return 0;		// No data for fractal 'name' in the file

	char *p = (char*)strchr(s,'{'), *e;
	if(!p)	return 0;		// Wrong data format for fractal 'name' in the file
	bool ext3d = false;
	e = (char*)strstr(s,"(3D)");	if(e && e<p)	ext3d = true;
	e = (char*)strstr(s,"(3d)");	if(e && e<p)	ext3d = true;
	e = (char*)strchr(p,'}');
	
	std::vector<mreal> nums;
	for(size_t i=0;p[i] && p+i<e;i++)
	{
		while(p[i]<=' ')	i++;
		if(p[i]==';' || p[i]=='#')	while(p[i] && p[i]!='\n')	i++;
		if(strchr("0123456789.+-",p[i]))	// this is number
		{
			nums.push_back(atof(p+i));
			while(p[i]>' ')	i++;
		}
	}
	HMDT dat = new mglData, res;
	if(ext3d)
	{
		dat->Set(&(nums[0]), 13, nums.size()/13, 1);
		res = mgl_data_ifs_3d(dat, n, skip);
	}
	else
	{
		dat->Set(&(nums[0]), 7, nums.size()/7, 1);
		res = mgl_data_ifs_2d(dat, n, skip);
	}
	delete dat;	free(buf);	return res;
}
uintptr_t mgl_data_ifs_file_(const char *fname, const char *name, long *n, long *skip,int l,int m)
{	char *s=new char[l+1];		memcpy(s,fname,l);	s[l]=0;
	char *t=new char[m+1];		memcpy(t,name,m);	t[m]=0;
	uintptr_t r = uintptr_t(mgl_data_ifs_file(s,t,*n,*skip));
	delete []s;	delete []t;		return r;	}
//-----------------------------------------------------------------------------
//
//	Functions for flame fractal
//
//-----------------------------------------------------------------------------
void MGL_NO_EXPORT mgl_linear_var0(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{	xNew += par[0]*x;	yNew += par[0]*y;	}
void MGL_NO_EXPORT mgl_sinusoidal_var1(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{	xNew += par[0]*sin(x);	yNew += par[0]*sin(y);	}
void MGL_NO_EXPORT mgl_spherical_var2(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{	mreal c1 = par[0]/(x*x+y*y);	xNew += c1*x;	yNew += c1*y;	}
void MGL_NO_EXPORT mgl_swirl_var3(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{
	mreal r2=x*x+y*y, c1=sin(r2), c2=cos(r2);	
	xNew += par[0]*(x*c1 - y*c2);
	yNew += par[0]*(x*c2 + y*c1);
}
void MGL_NO_EXPORT mgl_horseshoe_var4(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{
	mreal c1 = par[0]/hypot(x,y);	
	xNew += c1*(x*x-y*y);
	yNew += 2*c1*x*y;
}
void MGL_NO_EXPORT mgl_polar_var5(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{
	xNew += par[0]*atan2(x,y)/M_PI;
	yNew += par[0]*(hypot(x,y)-1);
}
void MGL_NO_EXPORT mgl_handkerchief_var6(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{
	mreal r=hypot(x,y), t=atan2(x,y), c1=par[0]*r;
	xNew += c1*sin(t+r);	yNew += c1*cos(t-r);
}
void MGL_NO_EXPORT mgl_heart_var7(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{
	mreal r=hypot(x,y), c1=par[0]*r, c2=atan2(x,y)*r;
	xNew +=  c1*sin(c2);	yNew += -c1*cos(c2);
}
void MGL_NO_EXPORT mgl_disc_var8(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{
	mreal c1=par[0]*atan2(x,y)/M_PI, c2=M_PI*hypot(x,y);
	xNew += c1*sin(c2);		yNew += c1*cos(c2);
}
void MGL_NO_EXPORT mgl_spiral_var9(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{
	mreal r=hypot(x,y), t=atan2(x,y), c1=par[0]/r;
	xNew += c1*(cos(t)+sin(r));
	yNew += c1*(sin(t)-cos(r));
}
void MGL_NO_EXPORT mgl_hyperbolic_var10(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{
	mreal r=hypot(x,y), t=atan2(x,y);
	xNew += par[0]*sin(t)/r;
	yNew += par[0]*r*cos(t);
}
void MGL_NO_EXPORT mgl_diamond_var11(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{
	mreal r=hypot(x,y), t=atan2(x,y);
	xNew += par[0]*sin(t)*cos(r);
	yNew += par[0]*cos(t)*sin(r);
}
void MGL_NO_EXPORT mgl_ex_var12(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{
	mreal r=hypot(x,y), t=atan2(x,y), c1=par[0]*r;
	mreal c2=mgl_ipow(sin(t+r),3), c3 = mgl_ipow(cos(t-r), 3);	
	xNew += c1*(c2 + c3);	yNew += c1*(c2 - c3);
}
void MGL_NO_EXPORT mgl_julia_var13(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{
	mreal c1=par[0]*sqrt(hypot(x,y)), c2=atan2(x,y)/2, c3=(rand()%2)*M_PI;
	xNew += c1*cos(c2+c3);	yNew += c1*sin(c2+c3);
}
void MGL_NO_EXPORT mgl_bent_var14(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{
	if (x>=0 && y>=0)
	{	xNew += par[0]*x;	yNew += par[0]*y;	}
	else if (x<0 && y>=0)
	{	xNew += par[0]*2*x;	yNew += par[0]*y;	}
	else if (x>=0 && x<0)
	{	xNew += par[0]*x;	yNew += par[0]*y/2;	}
	else
	{	xNew += par[0]*2*x;	yNew += par[0]*y/2;	}
}
void MGL_NO_EXPORT mgl_waves_var15(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{	// NOTE: par[1]=b[i], par[2]=1/c[i]^2, par[3]=e[i], par[4]=1/f[i]^2
	xNew += par[0]*(x + par[1]*sin(y*par[2]));
	yNew += par[0]*(y + par[3]*sin(x*par[4]));
}
void MGL_NO_EXPORT mgl_fisheye_var16(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{
	mreal c1 = par[0]*2/(hypot(x,y) + 1);
	xNew += c1*y;	yNew += c1*x;
}
void MGL_NO_EXPORT mgl_popcorn_var17(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{	// NOTE: par[1]=c[i], par[2]=f[i]
	xNew += par[0]*(x + par[1]*sin(tan(3*y)));
	yNew += par[0]*(y + par[2]*sin(tan(3*x)));
}
void MGL_NO_EXPORT mgl_exponential_var18(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{
	mreal c1=par[0]*exp(x-1);
	xNew += c1*cos(M_PI*y);	yNew += c1*sin(M_PI*y);
}
void MGL_NO_EXPORT mgl_power_var19(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{
	mreal t=atan2(x,y), c1=par[0]*pow(hypot(x,y), sin(t));
	xNew += c1*cos(t);	yNew += c1*sin(t);
}
void MGL_NO_EXPORT mgl_cosine_var20(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{
	xNew +=  par[0]*cos(M_PI*x)*cosh(y);
	yNew += -par[0]*sin(M_PI*x)*sinh(y);
}
void MGL_NO_EXPORT mgl_rings_var21(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{	// NOTE: par[1]=c[i]^2
	mreal t=atan2(x,y), r=hypot(x,y), c1=par[0]*(fmod(r+par[1],2*par[1])-par[1]+r*(1-par[1])); // convert to int?
	xNew += c1*cos(t);	yNew += c1*sin(t);
}
void MGL_NO_EXPORT mgl_fan_var22(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{	// NOTE: par[1]=c[i]^2, par[2]=f[i]
	mreal t=atan2(x,y), c1=par[0]*hypot(x,y), c2;
	c2 = fmod(t+par[2], M_PI*par[1]); // convert to int?
	if (c2 > M_PI/2*par[1])    c2 = t - M_PI/2*par[1];
	else    c2 += t;
	xNew += c1*cos(c2);	yNew += c1*sin(c2);
}
void MGL_NO_EXPORT mgl_blob_var23(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{
	mreal t=atan2(x,y), c1=par[0]*hypot(x,y)*(par[2]+(par[1]-par[2])/2*(sin(par[3]*t)));
	xNew += c1*cos(t);	yNew += c1*sin(t);
}
void MGL_NO_EXPORT mgl_pdj_var24(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{
	xNew += par[0]*(sin(par[1]*y) - cos(par[2]*x));
	yNew += par[0]*(sin(par[3]*x) - cos(par[4]*y));
}
void MGL_NO_EXPORT mgl_fan2_var25(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{
	mreal t=atan2(x,y), c1, c2;
	c1 = M_PI*par[1]*par[1];
	c2 = t + par[2] - c1*int(2*t*par[2]/c1);
	c1 /= 2;	c2 = c2>c1?t-c1:t+c1;
	c1 = par[0]*hypot(x,y);
	xNew += c1*sin(c2);	yNew += c1*cos(c2);
}
void MGL_NO_EXPORT mgl_rings2_var26(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{
	mreal r=hypot(x,y), t=atan2(x,y), c1=par[1]*par[1];
	c1 = par[0]*(r - 2*c1*int((r+c1)/(2*c1)) + r*(1-c1));
	xNew += c1*cos(t);	yNew += c1*sin(t);
}
void MGL_NO_EXPORT mgl_eyefish_var27(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{
	mreal c1 = par[0]*2/(hypot(x,y)+1);
	xNew += c1*x;		yNew += c1*y;
}
void MGL_NO_EXPORT mgl_bubble_var28(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{
	mreal c1 = par[0]*4/(x*x+y*y+4);
	xNew += c1*x;		yNew += c1*y;
}
void MGL_NO_EXPORT mgl_cylinder_var29(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{	xNew += par[0]*sin(x);	yNew += par[0]*y;	}
void MGL_NO_EXPORT mgl_perspective_var30(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{
	mreal c1 = par[0]*par[2]/(par[2]-y*sin(par[1]));
	xNew += c1*x;	yNew += c1*y*cos(par[1]);
}
void MGL_NO_EXPORT mgl_noise_var31(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{
	mreal c1=par[0]*mgl_rnd(), c2=2*M_PI*mgl_rnd();
	xNew += c1*x*cos(c2);	yNew += c1*y*sin(c2);
}
void MGL_NO_EXPORT mgl_juliaN_var32(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{
	mreal c1=int(fabs(par[1])*mgl_rnd()), c2;
	c2 = (atan2(y,x) + 2*M_PI*c1)/par[1];
	c1 = par[0]*pow(hypot(x,y), par[2]/par[1]);
	xNew += c1*cos(c2);	yNew += c1*sin(c2);
}
void MGL_NO_EXPORT mgl_juliaScope_var33(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{
	mreal c1=int(fabs(par[1])*mgl_rnd()), c2;
	c2 = ((2*(rand()%2)-1)*atan2(y,x) + 2*M_PI*c1)/par[1];
	c1 = par[0]*pow(hypot(x,y), par[2]/par[1]);
	xNew += c1*cos(c2);	yNew += c1*sin(c2);
}
void MGL_NO_EXPORT mgl_blur_var34(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{
	mreal c1=par[0]*mgl_rnd(), c2=2*M_PI*mgl_rnd();
	xNew += c1*cos(c2);	yNew += c1*sin(c2);
}
void MGL_NO_EXPORT mgl_gaussian_var35(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{
	mreal c1=par[0]*(4*mgl_rnd()-2), c2=2*M_PI*mgl_rnd();
	xNew += c1*cos(c2);	yNew += c1*sin(c2);
}
void MGL_NO_EXPORT mgl_radialBlur_var36(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{
	mreal r=hypot(x,y), c1=par[1]*M_PI/2, c2=par[0]*(4*mgl_rnd()-2), c3;
	c3 = c2*cos(c1) - 1;	c2 = atan2(y,x) + c2 *sin(c1);
	xNew += r*cos(c2) + c3*x;	yNew += r*sin(c2) + c3*y;
}
void MGL_NO_EXPORT mgl_pie_var37(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{
	mreal c1=int(mgl_rnd()*par[1] + 0.5), c2;
	c2 = par[2] + 2*M_PI/par[1]*(c1 + mgl_rnd()*par[3]);
	c1 = par[0]*mgl_rnd();
	xNew += c1*cos(c2);	yNew += c1*sin(c2);
}
void MGL_NO_EXPORT mgl_ngon_var38(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{
	mreal c1=2*M_PI/par[2], c2;
	c2 = atan2(y,x) - c1*floor(atan2(y,x)/c1);
	if (c2 <= c1/2)    c2 -= c1;
	c1 = par[0]*(par[3]*(1/cos(c2) - 1) + par[4])/pow(hypot(x,y), par[1]);
	xNew += c1*x;		yNew += c1*y;
}
void MGL_NO_EXPORT mgl_curl_var39(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{
	mreal c1=1 + par[1]*x + par[2]*(x*x - y*y);
	mreal c2 = par[1]*y + 2*par[2]*x*y;
	mreal c3 = par[0]/(c1*c1 + c2*c2);
	
	xNew += c3*(c1*x + c2*y);	yNew += c3*(c1*y - c2*y);
}
void MGL_NO_EXPORT mgl_rectangles_var40(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{
	xNew += par[0]*((2*floor(x/par[1]) + 1)*par[1] - x);
	yNew += par[0]*((2*floor(y/par[2]) + 1)*par[2] - y);
}
void MGL_NO_EXPORT mgl_arch_var41(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{
	mreal c1=mgl_rnd()*M_PI*par[0], c2=sin(c1);
	xNew += par[0]*c2;	yNew += par[0]*c2*c2/cos(c1);
}
void MGL_NO_EXPORT mgl_tangent_var42(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{	xNew += par[0]*sin(x)/cos(y);	yNew += par[0]*tan(y);	}
void MGL_NO_EXPORT mgl_square_var43(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{
	xNew += par[0]*(mgl_rnd() - 0.5);
	yNew += par[0]*(mgl_rnd() - 0.5);
}
void MGL_NO_EXPORT mgl_rays_var43(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{
	mreal c1 = par[0]*par[0]*tan(mgl_rnd()*M_PI*par[0])/(x*x+y*y);
	xNew += c1*cos(x);	yNew += c1*sin(y);
}
void MGL_NO_EXPORT mgl_blade_var44(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{
	mreal c1=par[0]*x, c2=mgl_rnd()*hypot(x,y)*par[0];
	xNew += c1*(cos(c2) + sin(c2));	// TODO check use of c2
	yNew += c1*(cos(c2) - sin(c2));
}
void MGL_NO_EXPORT mgl_secant_var45(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{	xNew += par[0]*x;	yNew += 1/cos(par[0]*hypot(x,y));	}
void MGL_NO_EXPORT mgl_twintrian_var47(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{
	mreal c1=par[0]*x, c2, c3;
	c2 = mgl_rnd()*hypot(x,y)*par[0];
	c3 = log10(sin(c2)*sin(c2)) + cos(c2);
	xNew += c1*c3;	yNew += c1*(c3 - M_PI*sin(c2));
}
void MGL_NO_EXPORT mgl_cross_var48(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par)
{
	mreal c1 = par[0]/fabs(x*x - y*y);
	xNew += c1*x;	yNew += c1*y;
}
//-----------------------------------------------------------------------------
typedef void (*flame_func)(mreal &xNew, mreal &yNew, mreal x, mreal y, const mreal *par);
MGL_NO_EXPORT flame_func ffunc[mglFlameLAST] = {
	mgl_linear_var0,	mgl_sinusoidal_var1,	mgl_spherical_var2,	mgl_swirl_var3,		mgl_horseshoe_var4,
	mgl_polar_var5,		mgl_handkerchief_var6,	mgl_heart_var7,		mgl_disc_var8,		mgl_spiral_var9,
	mgl_hyperbolic_var10,	mgl_diamond_var11,	mgl_ex_var12,		mgl_julia_var13,	mgl_bent_var14,
	mgl_waves_var15,		mgl_fisheye_var16,	mgl_popcorn_var17,	mgl_exponential_var18,	mgl_power_var19,
	mgl_cosine_var20,		mgl_rings_var21,	mgl_fan_var22,		mgl_blob_var23,		mgl_pdj_var24,
	mgl_fan2_var25,			mgl_rings2_var26,	mgl_eyefish_var27,	mgl_bubble_var28,	mgl_cylinder_var29,
	mgl_perspective_var30,	mgl_noise_var31,	mgl_juliaN_var32,	mgl_juliaScope_var33,	mgl_blur_var34,
	mgl_gaussian_var35,	mgl_radialBlur_var36,	mgl_pie_var37,		mgl_ngon_var38,		mgl_curl_var39,
	mgl_rectangles_var40,	mgl_arch_var41,		mgl_tangent_var42,	mgl_square_var43,	mgl_rays_var43,
	mgl_blade_var44,		mgl_secant_var45,	mgl_twintrian_var47,mgl_cross_var48	};
//-----------------------------------------------------------------------------
long MGL_NO_EXPORT mgl_flame_2d_point(HCDT A, HCDT F, mreal& x, mreal& y, mreal amax)
{
	long i, n=A->GetNy(), m=F->GetNy(), last_func=0, l=F->GetNx();
	l = l>6?6:l;
	mreal r = amax*mgl_rnd(), sum_prob = 0, x1, y1;
	for(i=0;i<n;i++)
	{
		sum_prob += A->v(6,i);
		if(r<sum_prob)	break;
	}
	x1 = A->v(0,i)*x + A->v(1,i)*y + A->v(4,i);
	y1 = A->v(2,i)*x + A->v(3,i)*y + A->v(5,i);
	x = y = 0;
	for(long j=0;j<m;j++)
	{
		int v=int(F->v(0,j,i)+0.5);
		mreal par[5] = {F->v(1,j,i),0,0,0,0};
		for(int k=2;k<l;k++)	par[k-1]=F->v(k,j,i);
		if(v<0 || v>=mglFlameLAST)	{	v=0;	par[0]=1;	}
		ffunc[v](x,y,x1,y1,par);	last_func=v;
	}
	return last_func;
}
HMDT MGL_EXPORT mgl_data_flame_2d(HCDT A, HCDT F, long n, long skip)
{
	if(!A || A->GetNx()<7 || n<1)	return 0;	// incompatible dimensions
	if(!F || F->GetNx()<2 || F->GetNz()!=A->GetNy())	return 0;	// incompatible dimensions
	mreal amax=0;
	for(long i=0; i<A->GetNy(); i++)	amax += A->v(6,i);
	if(amax<=0)	return 0;
	
	mglData *f = new mglData(3,n);
	mreal x = 0, y = 0;
	for(long i=0; i<skip; i++)	mgl_flame_2d_point(A, F, x, y,amax);
	for(long i=0; i<n; i++)
	{
		f->a[3*i+2] = mgl_flame_2d_point(A, F, x, y, amax);	// TODO color information ?!!
		f->a[3*i] = x;	f->a[3*i+1] = y;
	}
	return f;
}
uintptr_t MGL_EXPORT mgl_data_flame_2d_(uintptr_t *d, uintptr_t *f, long *n, long *skip)
{	return uintptr_t(mgl_data_flame_2d(_DT_,_DA_(f),*n,*skip));	}
//-----------------------------------------------------------------------------
