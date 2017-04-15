/***************************************************************************
 * exec.cpp is part of Math Graphic Library
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
#ifdef WIN32
#include <io.h>
#include <direct.h>
#else
#include <unistd.h>
#endif

#include "mgl2/base.h"
#include "mgl2/parser.h"
wchar_t *mgl_str_copy(const char *s);
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_addlegend(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"ss"))	gr->AddLegend(a[0].w.c_str(),a[1].s.c_str());
	else	res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_addto(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d && !strcmp(k,"dd"))		*d += *(a[1].d);
	else if(d && !strcmp(k,"dn"))	*d += a[1].v;
	else if(c && !strcmp(k,"dd"))	*c += *(a[1].d);
	else if(c && !strcmp(k,"dn"))	*c += a[1].c;
	else	res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_sort(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	if(d && !strcmp(k,"dn"))	d->Sort(a[1].v, -1);
	else if(d && !strcmp(k,"dnn"))	d->Sort(a[1].v, a[2].v);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_alpha(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]==0)	gr->Alpha(true);
	else if(!strcmp(k,"n"))	gr->Alpha(a[0].v!=0);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_gray(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]==0)	gr->Gray(true);
	else if(!strcmp(k,"n"))	gr->Gray(a[0].v!=0);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_pendelta(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"n"))	gr->SetPenDelta(a[0].v);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_plotid(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"s"))	gr->SetPlotId(a[1].s.c_str());
	else  res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_mask(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"sn"))	gr->SetMask(a[0].s[0],a[1].v);
	else if(!strcmp(k,"ss"))	gr->SetMask(a[0].s[0],a[1].s.c_str());
	else if(!strcmp(k,"n"))		gr->SetMaskAngle(mgl_int(a[0].v));
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_alphadef(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"n"))	gr->SetAlphaDef(a[0].v);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_ambient(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"n"))	gr->SetAmbient(a[0].v);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_diffuse(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"n"))	gr->SetDiffuse(a[0].v);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_area(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->Area(*(a[0].d),"",opt);
	else if(!strcmp(k,"ds"))	gr->Area(*(a[0].d),a[1].s.c_str(),opt);
	else if(!strcmp(k,"dd"))	gr->Area(*(a[0].d),*(a[1].d),"",opt);
	else if(!strcmp(k,"dds"))	gr->Area(*(a[0].d),*(a[1].d),a[2].s.c_str(),opt);
	else if(!strcmp(k,"ddd"))	gr->Area(*(a[0].d),*(a[1].d),*(a[2].d),"",opt);
	else if(!strcmp(k,"ddds"))	gr->Area(*(a[0].d),*(a[1].d),*(a[2].d),a[3].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_aspect(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"nn"))	gr->Aspect(a[0].v, a[1].v, 1);
	else if(!strcmp(k,"nnn"))	gr->Aspect(a[0].v, a[1].v, a[2].v);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_shear(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"nn"))	gr->Shear(a[0].v, a[1].v);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_axial(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->Axial(*(a[0].d),"",opt);
	else if(!strcmp(k,"ds"))	gr->Axial(*(a[0].d),a[1].s.c_str(),opt);
	else if(!strcmp(k,"ddd"))	gr->Axial(*(a[0].d),*(a[1].d),*(a[2].d),"",opt);
	else if(!strcmp(k,"ddds"))	gr->Axial(*(a[0].d),*(a[1].d),*(a[2].d),a[3].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_axis(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(k[0]==0)	gr->Axis("xyz","",opt);
	else if(!strcmp(k,"s"))		gr->Axis(a[0].s.c_str(), "",opt);
	else if(!strcmp(k,"ss"))	gr->Axis(a[0].s.c_str(), a[1].s.c_str(),opt);
	else if(!strcmp(k,"sss"))	gr->SetFunc(a[0].s.c_str(),a[1].s.c_str(),a[2].s.c_str(),"");
	else if(!strcmp(k,"ssss"))	gr->SetFunc(a[0].s.c_str(),a[1].s.c_str(),a[2].s.c_str(),a[3].s.c_str());
	else if(!strcmp(k,"n"))		gr->SetCoor(mgl_int(a[0].v));
	else if(!strcmp(k,"nnnn"))	gr->SetRanges(a[0].v,a[2].v, a[1].v,a[3].v);
	else if(!strcmp(k,"nnnnnn"))gr->SetRanges(a[0].v,a[3].v, a[1].v,a[4].v, a[2].v,a[5].v);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_ball(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;	gr->Self()->SaveState(opt);
	if(!strcmp(k,"nn"))	gr->Mark(mglPoint(a[0].v,a[1].v,NAN),"r.");
	else if(!strcmp(k,"nns"))	gr->Mark(mglPoint(a[0].v,a[1].v,NAN),a[2].s.c_str());
	else if(!strcmp(k,"nnn"))	gr->Mark(mglPoint(a[0].v,a[1].v,a[2].v),"r.");
	else if(!strcmp(k,"nnns"))	gr->Mark(mglPoint(a[0].v,a[1].v,a[2].v),a[3].s.c_str());
	else res = 1;	gr->Self()->LoadState();	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_box(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;	gr->Self()->SaveState(opt);
	if(k[0]==0)	gr->Box();
	else if(!strcmp(k,"s"))		gr->Box(a[0].s.c_str());
	else if(!strcmp(k,"sn"))	gr->Box(a[0].s.c_str(), a[1].v);
	else res = 1;	gr->Self()->LoadState();	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_ohlc(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"dddd"))	gr->OHLC(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d), "",opt);
	else if(!strcmp(k,"dddds"))	gr->OHLC(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d), a[4].s.c_str(),opt);
	else if(!strcmp(k,"ddddd"))	gr->OHLC(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d), "",opt);
	else if(!strcmp(k,"ddddds"))	gr->OHLC(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d), a[5].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_bars(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->Bars(*(a[0].d), "",opt);
	else if(!strcmp(k,"ds"))	gr->Bars(*(a[0].d), a[1].s.c_str(),opt);
	else if(!strcmp(k,"dd"))	gr->Bars(*(a[0].d), *(a[1].d), "",opt);
	else if(!strcmp(k,"dds"))	gr->Bars(*(a[0].d), *(a[1].d), a[2].s.c_str(),opt);
	else if(!strcmp(k,"ddd")) 	gr->Bars(*(a[0].d), *(a[1].d), *(a[2].d), "",opt);
	else if(!strcmp(k,"ddds"))	gr->Bars(*(a[0].d), *(a[1].d), *(a[2].d), a[3].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_barh(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->Barh(*(a[0].d), "",opt);
	else if(!strcmp(k,"ds"))	gr->Barh(*(a[0].d), a[1].s.c_str(),opt);
	else if(!strcmp(k,"dd"))	gr->Barh(*(a[0].d), *(a[1].d), "",opt);
	else if(!strcmp(k,"dds"))	gr->Barh(*(a[0].d), *(a[1].d), a[2].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_cones(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->Cones(*(a[0].d), "",opt);
	else if(!strcmp(k,"ds"))	gr->Cones(*(a[0].d), a[1].s.c_str(),opt);
	else if(!strcmp(k,"dd"))	gr->Cones(*(a[0].d), *(a[1].d), "",opt);
	else if(!strcmp(k,"dds"))	gr->Cones(*(a[0].d), *(a[1].d), a[2].s.c_str(),opt);
	else if(!strcmp(k,"ddd")) 	gr->Cones(*(a[0].d), *(a[1].d), *(a[2].d), "",opt);
	else if(!strcmp(k,"ddds"))	gr->Cones(*(a[0].d), *(a[1].d), *(a[2].d), a[3].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_belt(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->Belt(*(a[0].d),"",opt);
	else if(!strcmp(k,"ds"))	gr->Belt(*(a[0].d),a[1].s.c_str(),opt);
	else if(!strcmp(k,"ddd"))	gr->Belt(*(a[0].d),*(a[1].d),*(a[2].d),"",opt);
	else if(!strcmp(k,"ddds"))	gr->Belt(*(a[0].d),*(a[1].d),*(a[2].d),a[3].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_boxs(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->Boxs(*(a[0].d),"",opt);
	else if(!strcmp(k,"ds"))	gr->Boxs(*(a[0].d),a[1].s.c_str(),opt);
	else if(!strcmp(k,"ddd"))	gr->Boxs(*(a[0].d),*(a[1].d),*(a[2].d),"",opt);
	else if(!strcmp(k,"ddds"))	gr->Boxs(*(a[0].d),*(a[1].d),*(a[2].d),a[3].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_beam(mglGraph *gr, long , mglArg *a, const char *k, const char *)		// NOTE beam can be made obsolete ???
{
	int res=0;
	if(!strcmp(k,"ddddn"))
		gr->Beam(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),a[4].v,"",0, 3);
	else if(!strcmp(k,"ddddns"))
		gr->Beam(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),a[4].v,a[5].s.c_str(),0, 3);
	else if(!strcmp(k,"ddddnsn"))
		gr->Beam(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),a[4].v,a[5].s.c_str(),mgl_int(a[6].v), 3);
	else if(!strcmp(k,"ddddnsnn"))
		gr->Beam(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),a[4].v,a[5].s.c_str(),mgl_int(a[6].v), mgl_int(a[7].v));
	else if(!strcmp(k,"nddddn"))
		gr->Beam(a[0].v,*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),a[5].v,"",0);
	else if(!strcmp(k,"nddddns"))
		gr->Beam(a[0].v,*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),a[5].v,a[6].s.c_str(),0);
	else if(!strcmp(k,"nddddnsn"))
		gr->Beam(a[0].v,*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),a[5].v,a[6].s.c_str(),mgl_int(a[7].v));
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_clearlegend(mglGraph *gr, long , mglArg *, const char *k, const char *)
{
	int res=0;
	if(k[0]==0)	gr->ClearLegend();
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_rasterize(mglGraph *gr, long , mglArg *, const char *, const char *)
{
	gr->Rasterize();	return 0;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_background(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"s"))	gr->LoadBackground(a[0].s.c_str());
	else if(!strcmp(k,"sn"))	gr->LoadBackground(a[0].s.c_str(),a[1].v);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_clf(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]==0)	gr->Clf();
	else if(!strcmp(k,"s"))	gr->Clf(a[0].s.c_str());
	else if(!strcmp(k,"nnn"))	gr->Clf(a[0].v,a[1].v,a[2].v);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_reset(mglGraph *gr, long , mglArg *, const char *k, const char *)
{
	int res=0;
	if(k[0]==0)	gr->DefaultPlotParam();
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_chart(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->Chart(*(a[0].d), "",opt);
	else if(!strcmp(k,"ds"))	gr->Chart(*(a[0].d), a[1].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_cloud(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->Cloud(*(a[0].d),"",opt);
	else if(!strcmp(k,"ds"))	gr->Cloud(*(a[0].d),a[1].s.c_str(),opt);
	else if(!strcmp(k,"dddd"))	gr->Cloud(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),"",opt);
	else if(!strcmp(k,"dddds"))	gr->Cloud(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),a[4].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_crange(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->SetRange('c',*(a[0].d));
	else if(!strcmp(k,"dn"))	gr->SetRange('c',*(a[0].d),a[1].v);
	else if(!strcmp(k,"nn"))	gr->SetRange('c', a[0].v, a[1].v);
	else if(!strcmp(k,"nnn"))
	{
		if(a[2].v)	gr->AddRange('c', a[0].v, a[1].v);
		else	gr->SetRange('c', a[0].v, a[1].v);
	}
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_crop(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d && !strcmp(k,"dnns"))	d->Crop(mgl_int(a[1].v),mgl_int(a[2].v),a[3].s.c_str()[0]);
	else if(c && !strcmp(k,"dnns"))	c->Crop(mgl_int(a[1].v),mgl_int(a[2].v),a[3].s.c_str()[0]);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_clean(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	if(d && !strcmp(k,"dn"))	d->Clean(mgl_int(a[1].v));
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_cumsum(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d && !strcmp(k,"ds"))	d->CumSum(a[1].s.c_str());
	else if(c && !strcmp(k,"ds"))	c->CumSum(a[1].s.c_str());
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_curve(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"nnnnnnnn"))
		gr->Curve(mglPoint(a[0].v,a[1].v,NAN),
			mglPoint(a[2].v,a[3].v),
			mglPoint(a[4].v,a[5].v,NAN),
			mglPoint(a[6].v,a[7].v));
	else if(!strcmp(k,"nnnnnnnns"))
		gr->Curve(mglPoint(a[0].v,a[1].v,NAN),
			mglPoint(a[2].v,a[3].v),
			mglPoint(a[4].v,a[5].v,NAN),
			mglPoint(a[6].v,a[7].v), a[8].s.c_str());
	else if(!strcmp(k,"nnnnnnnnnnnn"))
		gr->Curve(mglPoint(a[0].v,a[1].v,a[2].v),
			mglPoint(a[3].v,a[4].v,a[5].v),
			mglPoint(a[6].v,a[7].v,a[8].v),
			mglPoint(a[9].v,a[10].v,a[11].v));
	else if(!strcmp(k,"nnnnnnnnnnnns"))
		gr->Curve(mglPoint(a[0].v,a[1].v,a[2].v),
			mglPoint(a[3].v,a[4].v,a[5].v),
			mglPoint(a[6].v,a[7].v,a[8].v),
			mglPoint(a[9].v,a[10].v,a[11].v), a[12].s.c_str());
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_cut(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"n"))	gr->SetCut(a[0].v != 0);
	else if(!strcmp(k,"nnnnnn"))
		gr->SetCutBox(mglPoint(a[0].v,a[1].v,a[2].v), mglPoint(a[3].v,a[4].v,a[5].v));
	else if(!strcmp(k,"s"))	gr->CutOff(a[0].s.c_str());
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_crust(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"ddd"))	gr->Crust(*(a[0].d),*(a[1].d),*(a[2].d),"",opt);
	else if(!strcmp(k,"ddds"))	gr->Crust(*(a[0].d),*(a[1].d),*(a[2].d),a[3].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_colorbar(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;	gr->Self()->SaveState(opt);
	if(k[0]==0)	gr->Colorbar();
	else if(!strcmp(k,"s"))		gr->Colorbar(a[0].s.c_str());
	else if(!strcmp(k,"d"))		gr->Colorbar(*(a[0].d));
	else if(!strcmp(k,"ds"))	gr->Colorbar(*(a[0].d), a[1].s.c_str());
	else if(!strcmp(k,"snn"))	gr->Colorbar(a[0].s.c_str(), a[1].v, a[2].v);
	else if(!strcmp(k,"snnn"))	gr->Colorbar(a[0].s.c_str(), a[1].v, a[2].v, a[3].v,1);
	else if(!strcmp(k,"snnnn"))	gr->Colorbar(a[0].s.c_str(), a[1].v, a[2].v, a[3].v,a[4].v);
	else if(!strcmp(k,"dsnn"))	gr->Colorbar(*(a[0].d), a[1].s.c_str(), a[2].v, a[3].v);
	else if(!strcmp(k,"dsnnn"))	gr->Colorbar(*(a[0].d), a[1].s.c_str(), a[2].v, a[3].v, a[4].v,1);
	else if(!strcmp(k,"dsnnnn"))
		gr->Colorbar(*(a[0].d), a[1].s.c_str(), a[2].v, a[3].v, a[4].v,a[5].v);
	else res = 1;	gr->Self()->LoadState();	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_copy(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(c && !strcmp(k,"dd"))	c->Set(a[1].d);
	else if(!d)	return 1;
	else if(!strcmp(k,"dd"))	d->Set(a[1].d);
	else if(!strcmp(k,"dds"))
	{	d->Set(a[1].d);	gr->Fill(*d, a[2].s.c_str());	}
	else if(!strcmp(k,"ddd"))
	{
		mglData *D = dynamic_cast<mglData *>(a[1].d);
		mglDataC *C = dynamic_cast<mglDataC *>(a[2].d);
		if(D && C)	{	d->Set(C->Real());	D->Set(C->Imag());	}
		else	res = 1;
	}
	else if(!strcmp(k,"dn"))	*d = a[1].v;
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_cont(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->Cont(*(a[0].d),"",opt);
	else if(!strcmp(k,"ds"))	gr->Cont(*(a[0].d),a[1].s.c_str(),opt);
	else if(!strcmp(k,"dd"))	gr->Cont(*(a[0].d), *(a[1].d), "",opt);
	else if(!strcmp(k,"dds"))	gr->Cont(*(a[0].d), *(a[1].d), a[2].s.c_str(),opt);
	else if(!strcmp(k,"ddd"))	gr->Cont(*(a[0].d), *(a[1].d), *(a[2].d), "",opt);
	else if(!strcmp(k,"ddds"))	gr->Cont(*(a[0].d), *(a[1].d), *(a[2].d), a[3].s.c_str(),opt);
	else if(!strcmp(k,"dddd"))	gr->Cont(*(a[0].d), *(a[1].d), *(a[2].d), *(a[3].d), "",opt);
	else if(!strcmp(k,"dddds"))	gr->Cont(*(a[0].d), *(a[1].d), *(a[2].d), *(a[3].d), a[4].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_contv(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->ContV(*(a[0].d),"",opt);
	else if(!strcmp(k,"ds"))	gr->ContV(*(a[0].d),a[1].s.c_str(),opt);
	else if(!strcmp(k,"dd"))	gr->ContV(*(a[0].d), *(a[1].d), "",opt);
	else if(!strcmp(k,"dds"))	gr->ContV(*(a[0].d), *(a[1].d), a[2].s.c_str(),opt);
	else if(!strcmp(k,"ddd"))	gr->ContV(*(a[0].d), *(a[1].d), *(a[2].d), "",opt);
	else if(!strcmp(k,"ddds"))	gr->ContV(*(a[0].d), *(a[1].d), *(a[2].d), a[3].s.c_str(),opt);
	else if(!strcmp(k,"dddd"))	gr->ContV(*(a[0].d), *(a[1].d), *(a[2].d), *(a[3].d), "",opt);
	else if(!strcmp(k,"dddds"))	gr->ContV(*(a[0].d), *(a[1].d), *(a[2].d), *(a[3].d), a[4].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_contf(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->ContF(*(a[0].d),"",opt);
	else if(!strcmp(k,"ds"))	gr->ContF(*(a[0].d),a[1].s.c_str(),opt);
	else if(!strcmp(k,"dd"))	gr->ContF(*(a[0].d), *(a[1].d), "",opt);
	else if(!strcmp(k,"dds"))	gr->ContF(*(a[0].d), *(a[1].d), a[2].s.c_str(),opt);
	else if(!strcmp(k,"ddd"))	gr->ContF(*(a[0].d), *(a[1].d), *(a[2].d), "",opt);
	else if(!strcmp(k,"ddds"))	gr->ContF(*(a[0].d), *(a[1].d), *(a[2].d), a[3].s.c_str(),opt);
	else if(!strcmp(k,"dddd"))	gr->ContF(*(a[0].d), *(a[1].d), *(a[2].d), *(a[3].d), "",opt);
	else if(!strcmp(k,"dddds"))	gr->ContF(*(a[0].d), *(a[1].d), *(a[2].d), *(a[3].d), a[4].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_contd(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->ContD(*(a[0].d),"",opt);
	else if(!strcmp(k,"ds"))	gr->ContD(*(a[0].d),a[1].s.c_str(),opt);
	else if(!strcmp(k,"dd"))	gr->ContD(*(a[0].d), *(a[1].d), "",opt);
	else if(!strcmp(k,"dds"))	gr->ContD(*(a[0].d), *(a[1].d), a[2].s.c_str(),opt);
	else if(!strcmp(k,"ddd"))	gr->ContD(*(a[0].d), *(a[1].d), *(a[2].d), "",opt);
	else if(!strcmp(k,"ddds"))	gr->ContD(*(a[0].d), *(a[1].d), *(a[2].d), a[3].s.c_str(),opt);
	else if(!strcmp(k,"dddd"))	gr->ContD(*(a[0].d), *(a[1].d), *(a[2].d), *(a[3].d), "",opt);
	else if(!strcmp(k,"dddds"))	gr->ContD(*(a[0].d), *(a[1].d), *(a[2].d), *(a[3].d), a[4].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_cont3(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->Cont3(*(a[0].d), "", -1,opt);
	else if(!strcmp(k,"ds"))	gr->Cont3(*(a[0].d), a[1].s.c_str(), -1,opt);
	else if(!strcmp(k,"dsn"))	gr->Cont3(*(a[0].d), a[1].s.c_str(), mgl_int(a[2].v),opt);
	else if(!strcmp(k,"dd"))	gr->Cont3(*(a[0].d), *(a[1].d), "", -1,opt);
	else if(!strcmp(k,"dds"))	gr->Cont3(*(a[0].d), *(a[1].d), a[2].s.c_str(),-1,opt);
	else if(!strcmp(k,"ddsn"))	gr->Cont3(*(a[0].d), *(a[1].d), a[2].s.c_str(),mgl_int(a[3].v),opt);
	else if(!strcmp(k,"dddd"))	gr->Cont3(*(a[0].d), *(a[1].d), *(a[2].d), *(a[3].d), "", -1,opt);
	else if(!strcmp(k,"dddds"))	gr->Cont3(*(a[0].d), *(a[1].d), *(a[2].d), *(a[3].d), a[4].s.c_str(),-1,opt);
	else if(!strcmp(k,"ddddsn"))	gr->Cont3(*(a[0].d), *(a[1].d), *(a[2].d), *(a[3].d), a[4].s.c_str(),mgl_int(a[5].v),opt);
	else if(!strcmp(k,"ddddd"))	gr->Cont3(*(a[0].d), *(a[1].d), *(a[2].d), *(a[3].d), *(a[4].d), "", -1,opt);
	else if(!strcmp(k,"ddddds"))	gr->Cont3(*(a[0].d), *(a[1].d), *(a[2].d), *(a[3].d), *(a[4].d), a[5].s.c_str(),-1,opt);
	else if(!strcmp(k,"dddddsn"))gr->Cont3(*(a[0].d), *(a[1].d), *(a[2].d), *(a[3].d), *(a[4].d), a[5].s.c_str(),mgl_int(a[6].v),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_contf3(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->ContF3(*(a[0].d), "", -1,opt);
	else if(!strcmp(k,"ds"))	gr->ContF3(*(a[0].d), a[1].s.c_str(), -1,opt);
	else if(!strcmp(k,"dsn"))	gr->ContF3(*(a[0].d), a[1].s.c_str(), mgl_int(a[2].v),opt);
	else if(!strcmp(k,"dd"))	gr->ContF3(*(a[0].d), *(a[1].d), "", -1,opt);
	else if(!strcmp(k,"dds"))	gr->ContF3(*(a[0].d), *(a[1].d), a[2].s.c_str(),-1,opt);
	else if(!strcmp(k,"ddsn"))	gr->ContF3(*(a[0].d), *(a[1].d), a[2].s.c_str(),mgl_int(a[3].v),opt);
	else if(!strcmp(k,"dddd"))	gr->ContF3(*(a[0].d), *(a[1].d), *(a[2].d), *(a[3].d), "", -1,opt);
	else if(!strcmp(k,"dddds"))	gr->ContF3(*(a[0].d), *(a[1].d), *(a[2].d), *(a[3].d), a[4].s.c_str(),-1,opt);
	else if(!strcmp(k,"ddddsn"))	gr->ContF3(*(a[0].d), *(a[1].d), *(a[2].d), *(a[3].d), a[4].s.c_str(),mgl_int(a[5].v),opt);
	else if(!strcmp(k,"ddddd"))	gr->ContF3(*(a[0].d), *(a[1].d), *(a[2].d), *(a[3].d), *(a[4].d), "", -1,opt);
	else if(!strcmp(k,"ddddds"))	gr->ContF3(*(a[0].d), *(a[1].d), *(a[2].d), *(a[3].d), *(a[4].d), a[5].s.c_str(),-1,opt);
	else if(!strcmp(k,"dddddsn"))gr->ContF3(*(a[0].d), *(a[1].d), *(a[2].d), *(a[3].d), *(a[4].d), a[5].s.c_str(),mgl_int(a[6].v),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_contx(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->ContX(*(a[0].d),"",NAN,opt);
	else if(!strcmp(k,"ds"))	gr->ContX(*(a[0].d),a[1].s.c_str(),NAN,opt);
	else if(!strcmp(k,"dsn"))	gr->ContX(*(a[0].d),a[1].s.c_str(),a[2].v,opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_contfx(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->ContFX(*(a[0].d),"",NAN,opt);
	else if(!strcmp(k,"ds"))	gr->ContFX(*(a[0].d),a[1].s.c_str(),NAN,opt);
	else if(!strcmp(k,"dsn"))	gr->ContFX(*(a[0].d),a[1].s.c_str(),a[2].v,opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_conty(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->ContY(*(a[0].d),"",NAN,opt);
	else if(!strcmp(k,"ds"))	gr->ContY(*(a[0].d),a[1].s.c_str(),NAN,opt);
	else if(!strcmp(k,"dsn"))	gr->ContY(*(a[0].d),a[1].s.c_str(),a[2].v,opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_contfy(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->ContFY(*(a[0].d),"",NAN,opt);
	else if(!strcmp(k,"ds"))	gr->ContFY(*(a[0].d),a[1].s.c_str(),NAN,opt);
	else if(!strcmp(k,"dsn"))	gr->ContFY(*(a[0].d),a[1].s.c_str(),a[2].v,opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_contz(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->ContZ(*(a[0].d),"",NAN,opt);
	else if(!strcmp(k,"ds"))	gr->ContZ(*(a[0].d),a[1].s.c_str(),NAN,opt);
	else if(!strcmp(k,"dsn"))	gr->ContZ(*(a[0].d),a[1].s.c_str(),a[2].v,opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_contfz(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->ContFZ(*(a[0].d),"",NAN,opt);
	else if(!strcmp(k,"ds"))	gr->ContFZ(*(a[0].d),a[1].s.c_str(),NAN,opt);
	else if(!strcmp(k,"dsn"))	gr->ContFZ(*(a[0].d),a[1].s.c_str(),a[2].v,opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_cone(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;	gr->Self()->SaveState(opt);
	if(!strcmp(k,"nnnnnnn"))	gr->Cone(mglPoint(a[0].v,a[1].v,a[2].v), mglPoint(a[3].v,a[4].v,a[5].v), a[6].v);
	else if(!strcmp(k,"nnnnnnns"))	gr->Cone(mglPoint(a[0].v,a[1].v,a[2].v), mglPoint(a[3].v,a[4].v,a[5].v), a[6].v,-1, a[7].s.c_str());
	else if(!strcmp(k,"nnnnnnnn"))	gr->Cone(mglPoint(a[0].v,a[1].v,a[2].v), mglPoint(a[3].v,a[4].v,a[5].v), a[6].v, a[7].v);
	else if(!strcmp(k,"nnnnnnnns"))	gr->Cone(mglPoint(a[0].v,a[1].v,a[2].v), mglPoint(a[3].v,a[4].v,a[5].v), a[6].v, a[7].v, a[8].s.c_str());
	else res = 1;	gr->Self()->LoadState();	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_ellipse(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;	gr->Self()->SaveState(opt);
	if(!strcmp(k,"nnnnn"))
		gr->Ellipse(mglPoint(a[0].v,a[1].v,NAN), mglPoint(a[2].v,a[3].v,NAN), a[4].v);
	else if(!strcmp(k,"nnnnns"))
		gr->Ellipse(mglPoint(a[0].v,a[1].v,NAN), mglPoint(a[2].v,a[3].v,NAN), a[4].v, a[5].s.c_str());
	else if(!strcmp(k,"nnnnnnn"))
		gr->Ellipse(mglPoint(a[0].v,a[1].v,a[2].v), mglPoint(a[3].v,a[4].v,a[5].v), a[6].v);
	else if(!strcmp(k,"nnnnnnns"))
		gr->Ellipse(mglPoint(a[0].v,a[1].v,a[2].v), mglPoint(a[3].v,a[4].v,a[5].v), a[6].v, a[7].s.c_str());
	else res = 1;	gr->Self()->LoadState();	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_circle(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;	gr->Self()->SaveState(opt);
	if(!strcmp(k,"nnn"))	gr->Circle(mglPoint(a[0].v,a[1].v, NAN), a[2].v);
	else if(!strcmp(k,"nnns"))	gr->Circle(mglPoint(a[0].v,a[1].v, NAN), a[2].v, a[3].s.c_str());
	else if(!strcmp(k,"nnnn"))	gr->Circle(mglPoint(a[0].v,a[1].v,a[2].v), a[3].v);
	else if(!strcmp(k,"nnnns"))	gr->Circle(mglPoint(a[0].v,a[1].v,a[2].v), a[3].v, a[4].s.c_str());
	else res = 1;	gr->Self()->LoadState();	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_rhomb(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;	gr->Self()->SaveState(opt);
	if(!strcmp(k,"nnnnn"))
		gr->Rhomb(mglPoint(a[0].v,a[1].v,NAN), mglPoint(a[2].v,a[3].v,NAN), a[4].v);
	else if(!strcmp(k,"nnnnns"))
		gr->Rhomb(mglPoint(a[0].v,a[1].v,NAN), mglPoint(a[2].v,a[3].v,NAN), a[4].v, a[5].s.c_str());
	else if(!strcmp(k,"nnnnnnn"))
		gr->Rhomb(mglPoint(a[0].v,a[1].v,a[2].v), mglPoint(a[3].v,a[4].v,a[5].v), a[6].v);
	else if(!strcmp(k,"nnnnnnns"))
		gr->Rhomb(mglPoint(a[0].v,a[1].v,a[2].v), mglPoint(a[3].v,a[4].v,a[5].v), a[6].v, a[7].s.c_str());
	else res = 1;	gr->Self()->LoadState();	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_polygon(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;	gr->Self()->SaveState(opt);
	if(!strcmp(k,"nnnnn"))
		gr->Polygon(mglPoint(a[0].v,a[1].v,NAN), mglPoint(a[2].v,a[3].v,NAN), mgl_int(a[4].v));
	else if(!strcmp(k,"nnnnns"))
		gr->Polygon(mglPoint(a[0].v,a[1].v,NAN), mglPoint(a[2].v,a[3].v,NAN), mgl_int(a[4].v), a[5].s.c_str());
	else if(!strcmp(k,"nnnnnnn"))
		gr->Polygon(mglPoint(a[0].v,a[1].v,a[2].v), mglPoint(a[3].v,a[4].v,a[5].v), mgl_int(a[6].v));
	else if(!strcmp(k,"nnnnnnns"))
		gr->Polygon(mglPoint(a[0].v,a[1].v,a[2].v), mglPoint(a[3].v,a[4].v,a[5].v), mgl_int(a[6].v), a[7].s.c_str());
	else res = 1;	gr->Self()->LoadState();	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_arc(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;	gr->Self()->SaveState(opt);
	if(!strcmp(k,"nnnnn"))
		gr->Arc(mglPoint(a[0].v,a[1].v,NAN), mglPoint(a[2].v,a[3].v,NAN), a[4].v);
	else if(!strcmp(k,"nnnnns"))
		gr->Arc(mglPoint(a[0].v,a[1].v,NAN), mglPoint(a[2].v,a[3].v,NAN), a[4].v, a[5].s.c_str());
	else if(!strcmp(k,"nnnnnn"))
		gr->Arc(mglPoint(a[0].v,a[1].v,a[2].v), mglPoint(a[3].v,a[4].v), a[5].v);
	else if(!strcmp(k,"nnnnnns"))
		gr->Arc(mglPoint(a[0].v,a[1].v,a[2].v), mglPoint(a[3].v,a[4].v), a[5].v, a[6].s.c_str());
	else if(!strcmp(k,"nnnnnnnnnn"))
		gr->Arc(mglPoint(a[0].v,a[1].v,a[2].v), mglPoint(a[3].v,a[4].v, a[5].v), mglPoint(a[6].v,a[7].v, a[8].v), a[9].v);
	else if(!strcmp(k,"nnnnnnnnnns"))
		gr->Arc(mglPoint(a[0].v,a[1].v,a[2].v), mglPoint(a[3].v,a[4].v, a[5].v), mglPoint(a[6].v,a[7].v, a[8].v), a[9].v, a[10].s.c_str());
	else res = 1;	gr->Self()->LoadState();	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_dens(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->Dens(*(a[0].d),"",opt);
	else if(!strcmp(k,"ds"))	gr->Dens(*(a[0].d),a[1].s.c_str(),opt);
	else if(!strcmp(k,"ddd"))	gr->Dens(*(a[0].d), *(a[1].d), *(a[2].d), "",opt);
	else if(!strcmp(k,"ddds"))	gr->Dens(*(a[0].d), *(a[1].d), *(a[2].d), a[3].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_dens3(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->Dens3(*(a[0].d),"",-1,opt);
	else if(!strcmp(k,"ds"))	gr->Dens3(*(a[0].d),a[1].s.c_str(),-1,opt);
	else if(!strcmp(k,"dsn"))	gr->Dens3(*(a[0].d),a[1].s.c_str(),mgl_int(a[2].v),opt);
	else if(!strcmp(k,"dddd"))	gr->Dens3(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),"", -1,opt);
	else if(!strcmp(k,"dddds"))	gr->Dens3(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),a[4].s.c_str(),-1,opt);
	else if(!strcmp(k,"ddddsn"))	gr->Dens3(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),a[4].s.c_str(),mgl_int(a[5].v),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_densx(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->DensX(*(a[0].d),"",NAN,opt);
	else if(!strcmp(k,"ds"))	gr->DensX(*(a[0].d),a[1].s.c_str(),NAN,opt);
	else if(!strcmp(k,"dsn"))	gr->DensX(*(a[0].d),a[1].s.c_str(),a[2].v,opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_densy(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->DensY(*(a[0].d),"",NAN,opt);
	else if(!strcmp(k,"ds"))	gr->DensY(*(a[0].d),a[1].s.c_str(),NAN,opt);
	else if(!strcmp(k,"dsn"))	gr->DensY(*(a[0].d),a[1].s.c_str(),a[2].v,opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_densz(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->DensZ(*(a[0].d),"",NAN,opt);
	else if(!strcmp(k,"ds"))	gr->DensZ(*(a[0].d),a[1].s.c_str(),NAN,opt);
	else if(!strcmp(k,"dsn"))	gr->DensZ(*(a[0].d),a[1].s.c_str(),a[2].v,opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_divto(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d && !strcmp(k,"dd"))		*d /= *(a[1].d);
	else if(d && !strcmp(k,"dn"))	*d /= a[1].v;
	else if(c && !strcmp(k,"dd"))	*c /= *(a[1].d);
	else if(c && !strcmp(k,"dn"))	*c /= a[1].c;
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_multo(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d && !strcmp(k,"dd"))		*d *= *(a[1].d);
	else if(d && !strcmp(k,"dn"))	*d *= a[1].v;
	else if(c && !strcmp(k,"dd"))	*c *= *(a[1].d);
	else if(c && !strcmp(k,"dn"))	*c *= a[1].c;
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_subto(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d && !strcmp(k,"dd"))		*d -= *(a[1].d);
	else if(d && !strcmp(k,"dn"))	*d -= a[1].v;
	else if(c && !strcmp(k,"dd"))	*c -= *(a[1].d);
	else if(c && !strcmp(k,"dn"))	*c -= a[1].c;
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_dots(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"ddd"))	gr->Dots(*(a[0].d),*(a[1].d),*(a[2].d),"",opt);
	else if(!strcmp(k,"ddds"))	gr->Dots(*(a[0].d),*(a[1].d),*(a[2].d),a[3].s.c_str(),opt);
	else if(!strcmp(k,"dddd"))	gr->Dots(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),"",opt);
	else if(!strcmp(k,"dddds"))	gr->Dots(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),a[4].s.c_str(),opt);
	else if(!strcmp(k,"ddddd"))	gr->Dots(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),"",opt);
	else if(!strcmp(k,"ddddds"))gr->Dots(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),a[5].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_diff(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d && !strcmp(k,"ds"))	d->Diff(a[1].s.c_str());
	else if(d && !strcmp(k,"ddd"))	d->Diff(*(a[1].d), *(a[2].d));
	else if(d && !strcmp(k,"dddd"))	d->Diff(*(a[1].d), *(a[2].d), *(a[3].d));
	else if(c && !strcmp(k,"ds"))	c->Diff(a[1].s.c_str());
//	else if(c && !strcmp(k,"ddd"))	c->Diff(*(a[1].d), *(a[2].d));	// TODO Add later
//	else if(c && !strcmp(k,"dddd"))	c->Diff(*(a[1].d), *(a[2].d), *(a[3].d));
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_diff2(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d && !strcmp(k,"ds"))	d->Diff2(a[1].s.c_str());
	else if(c && !strcmp(k,"ds"))	c->Diff2(a[1].s.c_str());
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_drop(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;	gr->Self()->SaveState(opt);
	if(!strcmp(k,"nnnnn"))
		gr->Drop(mglPoint(a[0].v,a[1].v), mglPoint(a[2].v,a[3].v), a[4].v);
	else if(!strcmp(k,"nnnnns"))
		gr->Drop(mglPoint(a[0].v,a[1].v), mglPoint(a[2].v,a[3].v), a[4].v, a[5].s.c_str());
	else if(!strcmp(k,"nnnnnsn"))
		gr->Drop(mglPoint(a[0].v,a[1].v), mglPoint(a[2].v,a[3].v), a[4].v, a[5].s.c_str(), a[6].v);
	else if(!strcmp(k,"nnnnnsnn"))
		gr->Drop(mglPoint(a[0].v,a[1].v), mglPoint(a[2].v,a[3].v), a[4].v, a[5].s.c_str(), a[6].v, a[7].v);
	else if(!strcmp(k,"nnnnnnn"))
		gr->Drop(mglPoint(a[0].v,a[1].v,a[2].v), mglPoint(a[3].v,a[4].v,a[5].v), a[6].v);
	else if(!strcmp(k,"nnnnnnns"))
		gr->Drop(mglPoint(a[0].v,a[1].v,a[2].v), mglPoint(a[3].v,a[4].v,a[5].v), a[6].v, a[7].s.c_str());
	else if(!strcmp(k,"nnnnnnnsn"))
		gr->Drop(mglPoint(a[0].v,a[1].v,a[2].v), mglPoint(a[3].v,a[4].v,a[5].v), a[6].v, a[7].s.c_str(), a[8].v);
	else if(!strcmp(k,"nnnnnnnsnn"))
		gr->Drop(mglPoint(a[0].v,a[1].v,a[2].v), mglPoint(a[3].v,a[4].v,a[5].v), a[6].v, a[7].s.c_str(), a[8].v, a[9].v);
	else res = 1;	gr->Self()->LoadState();	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_dew(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"dd"))	gr->Dew(*(a[0].d),*(a[1].d),"",opt);
	else if(!strcmp(k,"dds"))	gr->Dew(*(a[0].d),*(a[1].d),a[2].s.c_str(),opt);
	else if(!strcmp(k,"dddd"))	gr->Dew(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),"",opt);
	else if(!strcmp(k,"dddds"))	gr->Dew(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),a[4].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_fall(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->Fall(*(a[0].d),"",opt);
	else if(!strcmp(k,"ds"))	gr->Fall(*(a[0].d),a[1].s.c_str(),opt);
	else if(!strcmp(k,"ddd"))	gr->Fall(*(a[0].d),*(a[1].d),*(a[2].d),"",opt);
	else if(!strcmp(k,"ddds"))	gr->Fall(*(a[0].d),*(a[1].d),*(a[2].d),a[3].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_mesh(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->Mesh(*(a[0].d),"",opt);
	else if(!strcmp(k,"ds"))	gr->Mesh(*(a[0].d),a[1].s.c_str(),opt);
	else if(!strcmp(k,"ddd"))	gr->Mesh(*(a[0].d),*(a[1].d),*(a[2].d),"",opt);
	else if(!strcmp(k,"ddds"))	gr->Mesh(*(a[0].d),*(a[1].d),*(a[2].d),a[3].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_surf(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->Surf(*(a[0].d),"",opt);
	else if(!strcmp(k,"ds"))	gr->Surf(*(a[0].d),a[1].s.c_str(),opt);
	else if(!strcmp(k,"ddd"))	gr->Surf(*(a[0].d),*(a[1].d),*(a[2].d),"",opt);
	else if(!strcmp(k,"ddds"))	gr->Surf(*(a[0].d),*(a[1].d),*(a[2].d),a[3].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_surfc(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"dd"))	gr->SurfC(*(a[0].d),*(a[1].d),"",opt);
	else if(!strcmp(k,"dds"))	gr->SurfC(*(a[0].d),*(a[1].d),a[2].s.c_str(),opt);
	else if(!strcmp(k,"dddd"))	gr->SurfC(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),"",opt);
	else if(!strcmp(k,"dddds"))	gr->SurfC(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),a[4].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_surfa(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"dd"))	gr->SurfA(*(a[0].d),*(a[1].d),"",opt);
	else if(!strcmp(k,"dds"))	gr->SurfA(*(a[0].d),*(a[1].d),a[2].s.c_str(),opt);
	else if(!strcmp(k,"dddd"))	gr->SurfA(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),"",opt);
	else if(!strcmp(k,"dddds"))	gr->SurfA(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),a[4].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_surfca(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"ddd"))	gr->SurfCA(*(a[0].d),*(a[1].d),*(a[2].d),"",opt);
	else if(!strcmp(k,"ddds"))	gr->SurfCA(*(a[0].d),*(a[1].d),*(a[2].d),a[3].s.c_str(),opt);
	else if(!strcmp(k,"ddddd"))	gr->SurfCA(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),"",opt);
	else if(!strcmp(k,"ddddds"))gr->SurfCA(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),a[5].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_flow(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"dd"))	gr->Flow(*(a[0].d),*(a[1].d),"",opt);
	else if(!strcmp(k,"dds"))	gr->Flow(*(a[0].d),*(a[1].d),a[2].s.c_str(),opt);
	else if(!strcmp(k,"ddd"))	gr->Flow(*(a[0].d),*(a[1].d),*(a[2].d),"",opt);
	else if(!strcmp(k,"ddds"))	gr->Flow(*(a[0].d),*(a[1].d),*(a[2].d),a[3].s.c_str(),opt);
	else if(!strcmp(k,"dddd"))	gr->Flow(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),"",opt);
	else if(!strcmp(k,"dddds"))	gr->Flow(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),a[4].s.c_str(),opt);
	else if(!strcmp(k,"dddddd"))
		gr->Flow(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),*(a[5].d),"",opt);
	else if(!strcmp(k,"dddddds"))
		gr->Flow(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),*(a[5].d),a[6].s.c_str(),opt);
	else if(!strcmp(k,"nndd"))
		gr->FlowP(mglPoint(a[0].v,a[1].v,NAN), *(a[2].d),*(a[3].d),"",opt);
	else if(!strcmp(k,"nndds"))
		gr->FlowP(mglPoint(a[0].v,a[1].v,NAN), *(a[2].d),*(a[3].d),a[4].s.c_str(),opt);
	else if(!strcmp(k,"nndddd"))
		gr->FlowP(mglPoint(a[0].v,a[1].v,NAN), *(a[2].d),*(a[3].d),*(a[4].d),*(a[5].d),"",opt);
	else if(!strcmp(k,"nndddds"))
		gr->FlowP(mglPoint(a[0].v,a[1].v,NAN), *(a[2].d),*(a[3].d),*(a[4].d),*(a[5].d),a[6].s.c_str(),opt);
	else if(!strcmp(k,"nnnddd"))
		gr->FlowP(mglPoint(a[0].v,a[1].v,a[2].v), *(a[3].d),*(a[4].d),*(a[5].d),"",opt);
	else if(!strcmp(k,"nnnddds"))
		gr->FlowP(mglPoint(a[0].v,a[1].v,a[2].v), *(a[3].d),*(a[4].d),*(a[5].d),a[6].s.c_str(),opt);
	else if(!strcmp(k,"nnndddddd"))
		gr->FlowP(mglPoint(a[0].v,a[1].v,a[2].v), *(a[3].d),*(a[4].d),*(a[5].d),*(a[6].d),*(a[7].d),*(a[8].d),"",opt);
	else if(!strcmp(k,"nnndddddds"))
		gr->FlowP(mglPoint(a[0].v,a[1].v,a[2].v), *(a[3].d),*(a[4].d),*(a[5].d),*(a[6].d),*(a[7].d),*(a[8].d),a[9].s.c_str(),opt);
	else res = 1;
	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_grad(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->Grad(*(a[0].d), "",opt);
	else if(!strcmp(k,"ds"))	gr->Grad(*(a[0].d), a[1].s.c_str(),opt);
	else if(!strcmp(k,"ddd"))	gr->Grad(*(a[0].d),*(a[1].d),*(a[2].d), "",opt);
	else if(!strcmp(k,"ddds"))	gr->Grad(*(a[0].d),*(a[1].d),*(a[2].d), a[3].s.c_str(),opt);
	else if(!strcmp(k,"dddd"))	gr->Grad(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d), "",opt);
	else if(!strcmp(k,"dddds"))	gr->Grad(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d), a[4].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_fill(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d && k[0]=='d')
	{
		if(!strcmp(k,"dnn"))	d->Fill(a[1].v,a[2].v);
		else if(!strcmp(k,"dnns"))	d->Fill(a[1].v,a[2].v,a[3].s.c_str()[0]);
		else if(!strcmp(k,"ds"))	gr->Fill(*d,a[1].s.c_str(),opt);
		else if(!strcmp(k,"dsd"))	gr->Fill(*d,a[1].s.c_str(), *(a[2].d),opt);
		else if(!strcmp(k,"dsdd"))	gr->Fill(*d,a[1].s.c_str(), *(a[2].d), *(a[3].d),opt);
		else res = 1;
	}
	else if(c && k[0]=='d')
	{
		if(!strcmp(k,"dnn"))	c->Fill(a[1].v,a[2].v);
		else if(!strcmp(k,"dnns"))	c->Fill(a[1].v,a[2].v,a[3].s.c_str()[0]);
		else if(!strcmp(k,"ds"))	gr->Fill(*c,a[1].s.c_str(),opt);
		else if(!strcmp(k,"dsd"))	gr->Fill(*c,a[1].s.c_str(), *(a[2].d),opt);
		else if(!strcmp(k,"dsdd"))	gr->Fill(*c,a[1].s.c_str(), *(a[2].d), *(a[3].d),opt);
		else res = 1;
	}
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_refill(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d && k[0]=='d' && k[1]=='d' && k[2]=='d')
	{
		if(k[3]==0)	gr->Refill(*d,*(a[1].d),*(a[2].d),-1,opt);
		else if(!strcmp(k+3,"n"))	gr->Refill(*d,*(a[1].d),*(a[2].d),mgl_int(a[3].v),opt);
		else if(!strcmp(k+3,"d"))	gr->Refill(*d,*(a[1].d),*(a[2].d),*(a[3].d),-1,opt);
		else if(!strcmp(k+3,"dn"))	gr->Refill(*d,*(a[1].d),*(a[2].d),*(a[3].d),mgl_int(a[4].v),opt);
		else if(!strcmp(k+3,"dd"))	gr->Refill(*d,*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),opt);
		else res = 1;
	}
	else if(c && k[0]=='d' && k[1]=='d' && k[2]=='d')
	{
		if(k[3]==0)	gr->Refill(*c,*(a[1].d),*(a[2].d),-1,opt);
		else if(!strcmp(k+3,"n"))	gr->Refill(*c,*(a[1].d),*(a[2].d),mgl_int(a[3].v),opt);
		else if(!strcmp(k+3,"d"))	gr->Refill(*c,*(a[1].d),*(a[2].d),*(a[3].d),-1,opt);
		else if(!strcmp(k+3,"dn"))	gr->Refill(*c,*(a[1].d),*(a[2].d),*(a[3].d),mgl_int(a[4].v),opt);
		else if(!strcmp(k+3,"dd"))	gr->Refill(*c,*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),opt);
		else res = 1;
	}
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_gspline(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d && !strcmp(k,"ddd"))	d->RefillGS(*(a[1].d),*(a[2].d),gr->Self()->Min.x,gr->Self()->Max.x,-1);
	else if(d && !strcmp(k,"dddn"))	d->RefillGS(*(a[1].d),*(a[2].d),gr->Self()->Min.x,gr->Self()->Max.x,mgl_int(a[3].v));
	else if(c && !strcmp(k,"ddd"))	c->RefillGS(*(a[1].d),*(a[2].d),gr->Self()->Min.x,gr->Self()->Max.x,-1);
	else if(c && !strcmp(k,"dddn"))	c->RefillGS(*(a[1].d),*(a[2].d),gr->Self()->Min.x,gr->Self()->Max.x,mgl_int(a[3].v));
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_fillsample(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	if(d && !strcmp(k,"ds"))	d->FillSample(a[1].s.c_str());
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_fog(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"n"))	gr->Fog(a[0].v);
	else if(!strcmp(k,"nn"))	gr->Fog(a[0].v,a[1].v);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_font(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"s"))	gr->SetFontDef(a[0].s.c_str());
	else if(!strcmp(k,"sn"))
	{	gr->SetFontDef(a[0].s.c_str());	gr->SetFontSize(a[1].v);	}
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_loadfont(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]==0 || (!strcmp(k,"s") && a[0].s[0]==0))	gr->RestoreFont();
	else if(!strcmp(k,"s"))	gr->LoadFont(a[0].s.c_str());
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_grid(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(k[0]==0)	gr->Grid("xyzt", "B",opt);
	else if(!strcmp(k,"s"))	gr->Grid(a[0].s.c_str(), "B",opt);
	else if(!strcmp(k,"ss"))gr->Grid(a[0].s.c_str(), a[1].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_grid2(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->Grid(*(a[0].d),"",opt);
	else if(!strcmp(k,"ds"))	gr->Grid(*(a[0].d),a[1].s.c_str(),opt);
	else if(!strcmp(k,"ddd"))	gr->Grid(*(a[0].d), *(a[1].d), *(a[2].d), "",opt);
	else if(!strcmp(k,"ddds"))	gr->Grid(*(a[0].d), *(a[1].d), *(a[2].d), a[3].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_grid3(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->Grid3(*(a[0].d),"",-1,opt);
	else if(!strcmp(k,"ds"))	gr->Grid3(*(a[0].d),a[1].s.c_str(),-1,opt);
	else if(!strcmp(k,"dsn"))	gr->Grid3(*(a[0].d),a[1].s.c_str(),mgl_int(a[2].v),opt);
	else if(!strcmp(k,"dddd"))	gr->Grid3(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),"",-1,opt);
	else if(!strcmp(k,"dddds"))	gr->Grid3(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),a[4].s.c_str(),-1,opt);
	else if(!strcmp(k,"ddddsn"))gr->Grid3(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),a[4].s.c_str(),mgl_int(a[5].v),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_light(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]==0)	gr->Light(true);
	else if(!strcmp(k,"n"))	gr->Light(a[0].v!=0);
	else if(!strcmp(k,"nn"))	gr->Light(mgl_int(a[0].v),a[1].v!=0);
	else if(!strcmp(k,"nnnn"))	gr->AddLight(mgl_int(a[0].v),mglPoint(a[1].v,a[2].v,a[3].v));
	else if(!strcmp(k,"nnnns"))	gr->AddLight(mgl_int(a[0].v),mglPoint(a[1].v,a[2].v,a[3].v), a[4].s.c_str()[0]);
	else if(!strcmp(k,"nnnnsn"))gr->AddLight(mgl_int(a[0].v),mglPoint(a[1].v,a[2].v,a[3].v), a[4].s.c_str()[0],a[5].v);
	else if(!strcmp(k,"nnnnsnn"))
		gr->AddLight(mgl_int(a[0].v),mglPoint(a[1].v,a[2].v,a[3].v), a[4].s.c_str()[0],a[5].v,a[6].v);
	else if(!strcmp(k,"nnnnnnn"))
		gr->AddLight(mgl_int(a[0].v),mglPoint(a[1].v,a[2].v,a[3].v),mglPoint(a[4].v,a[5].v,a[6].v));
	else if(!strcmp(k,"nnnnnnns"))
		gr->AddLight(mgl_int(a[0].v),mglPoint(a[1].v,a[2].v,a[3].v),mglPoint(a[4].v,a[5].v,a[6].v), a[7].s.c_str()[0]);
	else if(!strcmp(k,"nnnnnnnsn"))
		gr->AddLight(mgl_int(a[0].v),mglPoint(a[1].v,a[2].v,a[3].v),mglPoint(a[4].v,a[5].v,a[6].v), a[7].s.c_str()[0],a[8].v);
	else if(!strcmp(k,"nnnnnnnsnn"))
		gr->AddLight(mgl_int(a[0].v),mglPoint(a[1].v,a[2].v,a[3].v),mglPoint(a[4].v,a[5].v,a[6].v), a[7].s.c_str()[0],a[8].v,a[9].v);
	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_attachlight(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"n"))	gr->AttachLight(a[0].v!=0);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_line(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;	gr->Self()->SaveState(opt);
	if(!strcmp(k,"nnnn"))
		gr->Line(mglPoint(a[0].v,a[1].v,NAN), mglPoint(a[2].v,a[3].v,NAN));
	else if(!strcmp(k,"nnnns"))
		gr->Line(mglPoint(a[0].v,a[1].v,NAN), mglPoint(a[2].v,a[3].v,NAN), a[4].s.c_str());
	else if(!strcmp(k,"nnnnnn"))
		gr->Line(mglPoint(a[0].v,a[1].v,a[2].v), mglPoint(a[3].v,a[4].v,a[5].v));
	else if(!strcmp(k,"nnnnnns"))
		gr->Line(mglPoint(a[0].v,a[1].v,a[2].v), mglPoint(a[3].v,a[4].v,a[5].v), a[6].s.c_str());
	else res = 1;	gr->Self()->LoadState();	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_lamerey(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"nd"))	gr->Lamerey(a[0].v,*(a[1].d),"",opt);
	else if(!strcmp(k,"nds"))	gr->Lamerey(a[0].v,*(a[1].d),a[2].s.c_str(),opt);
	else if(!strcmp(k,"ns"))	gr->Lamerey(a[0].v,a[1].s.c_str(),"",opt);
	else if(!strcmp(k,"nss"))	gr->Lamerey(a[0].v,a[1].s.c_str(),a[2].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_bifurcation(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"nd"))	gr->Bifurcation(a[0].v,*(a[1].d),"",opt);
	else if(!strcmp(k,"nds"))	gr->Bifurcation(a[0].v,*(a[1].d),a[2].s.c_str(),opt);
	else if(!strcmp(k,"ns"))	gr->Bifurcation(a[0].v,a[1].s.c_str(),"",opt);
	else if(!strcmp(k,"nss"))	gr->Bifurcation(a[0].v,a[1].s.c_str(),a[2].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_errbox(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;	gr->Self()->SaveState(opt);
	if(!strcmp(k,"nnnn"))
		gr->Error(mglPoint(a[0].v,a[1].v,NAN), mglPoint(a[2].v,a[3].v,NAN));
	else if(!strcmp(k,"nnnns"))
		gr->Error(mglPoint(a[0].v,a[1].v,NAN), mglPoint(a[2].v,a[3].v,NAN), a[4].s.c_str());
	else if(!strcmp(k,"nnnnnn"))
		gr->Error(mglPoint(a[0].v,a[1].v,a[2].v), mglPoint(a[3].v,a[4].v,a[5].v));
	else if(!strcmp(k,"nnnnnns"))
		gr->Error(mglPoint(a[0].v,a[1].v,a[2].v), mglPoint(a[3].v,a[4].v,a[5].v), a[6].s.c_str());
	else res = 1;	gr->Self()->LoadState();	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_legend(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(k[0]==0)	gr->Legend(3, "#", opt);
	else if(!strcmp(k,"n"))	gr->Legend(mgl_int(a[0].v), "#", opt);
	else if(!strcmp(k,"ns"))	gr->Legend(mgl_int(a[0].v), a[1].s.c_str(), opt);
	else if(!strcmp(k,"nn"))	gr->Legend(a[0].v, a[1].v, "#", opt);
	else if(!strcmp(k,"nns"))	gr->Legend(a[0].v, a[1].v, a[2].s.c_str(), opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_barwidth(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"n"))	gr->SetBarWidth(a[0].v);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_legendmarks(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"n"))	gr->SetLegendMarks(mgl_int(a[0].v));
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_modify(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d && !strcmp(k,"ds"))	d->Modify(a[1].s.c_str());
	else if(d && !strcmp(k,"dsn"))	d->Modify(a[1].s.c_str(), mgl_int(a[2].v));
	else if(d && !strcmp(k,"dsd"))	d->Modify(a[1].s.c_str(),*(a[2].d));
	else if(d && !strcmp(k,"dsdd"))	d->Modify(a[1].s.c_str(),*(a[2].d),*(a[3].d));
	else if(c && !strcmp(k,"ds"))	c->Modify(a[1].s.c_str());
	else if(c && !strcmp(k,"dsn"))	c->Modify(a[1].s.c_str(), mgl_int(a[2].v));
	else if(c && !strcmp(k,"dsd"))	c->Modify(a[1].s.c_str(),*(a[2].d));
	else if(c && !strcmp(k,"dsdd"))	c->Modify(a[1].s.c_str(),*(a[2].d),*(a[3].d));
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_max(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	if(d && !strcmp(k,"dds"))	*d = mglData(true,mgl_data_max_dir(a[1].d,a[2].s.c_str()));
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_min(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	if(d && !strcmp(k,"dds"))	*d = mglData(true,mgl_data_min_dir(a[1].d,a[2].s.c_str()));
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_sum(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d && !strcmp(k,"dds"))	*d = mglData(true,mgl_data_sum(a[1].d,a[2].s.c_str()));
	else if(c && !strcmp(k,"dds"))	*c = mglDataC(true,mgl_datac_sum(a[1].d,a[2].s.c_str()));
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_meshnum(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"n"))	gr->SetMeshNum(a[0].v);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_facenum(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"n"))	gr->SetFaceNum(a[0].v);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_quality(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]==0)	gr->SetQuality();
	else if(!strcmp(k,"n"))	gr->SetQuality(mgl_int(a[0].v));
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_marksize(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"n"))	gr->SetMarkSize(a[0].v);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_mark(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"dd"))	gr->Mark(*(a[0].d), *(a[1].d), "",opt);
	else if(!strcmp(k,"dds"))	gr->Mark(*(a[0].d), *(a[1].d), a[2].s.c_str(),opt);
	else if(!strcmp(k,"ddd")) 	gr->Mark(*(a[0].d), *(a[1].d), *(a[2].d), "",opt);
	else if(!strcmp(k,"ddds"))	gr->Mark(*(a[0].d), *(a[1].d), *(a[2].d), a[3].s.c_str(),opt);
	else if(!strcmp(k,"dddd")) 	gr->Mark(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d), "",opt);
	else if(!strcmp(k,"dddds"))	gr->Mark(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d), a[4].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_pmap(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"dd"))	gr->Pmap(*(a[0].d), *(a[1].d), "",opt);
	else if(!strcmp(k,"dds"))	gr->Pmap(*(a[0].d), *(a[1].d), a[2].s.c_str(),opt);
	else if(!strcmp(k,"ddd")) 	gr->Pmap(*(a[0].d), *(a[1].d), *(a[2].d), "",opt);
	else if(!strcmp(k,"ddds"))	gr->Pmap(*(a[0].d), *(a[1].d), *(a[2].d), a[3].s.c_str(),opt);
	else if(!strcmp(k,"dddd")) 	gr->Pmap(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d), "",opt);
	else if(!strcmp(k,"dddds"))	gr->Pmap(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d), a[4].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_map(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"dd"))	gr->Map(*(a[0].d),*(a[1].d),"",opt);
	else if(!strcmp(k,"dds"))	gr->Map(*(a[0].d),*(a[1].d),a[2].s.c_str(),opt);
	else if(!strcmp(k,"dddd"))	gr->Map(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),"",opt);
	else if(!strcmp(k,"dddds"))	gr->Map(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),a[4].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_read(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	bool rr=true;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	if(k[1]=='d' && a[1].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglData *f = dynamic_cast<mglData *>(a[1].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(c)
	{
		if(!strcmp(k,"ds"))			rr=c->Read(a[1].s.c_str());
		else if(!strcmp(k,"dsn"))	rr=c->Read(a[1].s.c_str(), mgl_int(a[2].v));
		else if(!strcmp(k,"dsnn"))	rr=c->Read(a[1].s.c_str(), mgl_int(a[2].v),mgl_int(a[3].v));
		else if(!strcmp(k,"dsnnn"))	rr=c->Read(a[1].s.c_str(), mgl_int(a[2].v),mgl_int(a[3].v),mgl_int(a[4].v));
		else res = 1;
	}
	else if(d && f && k[0]=='d' && k[1]=='d' && k[2]=='s')
	{
		mglDataC r;
		if(k[3]==0)	rr=r.Read(a[2].s.c_str());
		else if(!strcmp(k+3,"n"))	rr=r.Read(a[2].s.c_str(), mgl_int(a[3].v));
		else if(!strcmp(k+3,"nn"))	rr=r.Read(a[2].s.c_str(), mgl_int(a[3].v),mgl_int(a[4].v));
		else if(!strcmp(k+3,"nnn"))	rr=r.Read(a[2].s.c_str(), mgl_int(a[3].v),mgl_int(a[4].v),mgl_int(a[5].v));
		else res = 1;
		if(res==0)	{	*d = r.Real();	*f = r.Imag();	}
	}
	else if(d)
	{
		if(!strcmp(k,"ds"))	rr=d->Read(a[1].s.c_str());
		else if(!strcmp(k,"dsn"))	rr=d->Read(a[1].s.c_str(), mgl_int(a[2].v));
		else if(!strcmp(k,"dsnn"))	rr=d->Read(a[1].s.c_str(), mgl_int(a[2].v),mgl_int(a[3].v));
		else if(!strcmp(k,"dsnnn"))	rr=d->Read(a[1].s.c_str(), mgl_int(a[2].v),mgl_int(a[3].v),mgl_int(a[4].v));
		else res = 1;
	}
	else res = 1;
	if(!rr)	gr->SetWarn(mglWarnFile,"Read");
	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_readmat(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	bool rr=true;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d && !strcmp(k,"ds"))		rr=d->ReadMat(a[1].s.c_str());
	else if(d && !strcmp(k,"dsn"))	rr=d->ReadMat(a[1].s.c_str(), mgl_int(a[2].v));
	else if(c && !strcmp(k,"ds"))	rr=c->ReadMat(a[1].s.c_str());
	else if(c && !strcmp(k,"dsn"))	rr=c->ReadMat(a[1].s.c_str(), mgl_int(a[2].v));
	else res = 1;
	if(!rr)	gr->SetWarn(mglWarnFile,"ReadMat");
	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_readall(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	bool rr=true;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d && !strcmp(k,"ds"))	rr=d->ReadAll(a[1].s.c_str());
	else if(d && !strcmp(k,"dsn"))		rr=d->ReadAll(a[1].s.c_str(), a[2].v);
	else if(d && !strcmp(k,"dsnn"))		rr=d->ReadRange(a[1].s.c_str(), a[2].v, a[3].v);
	else if(d && !strcmp(k,"dsnnn"))	rr=d->ReadRange(a[1].s.c_str(), a[2].v, a[3].v, a[4].v);
	else if(d && !strcmp(k,"dsnnnn"))	rr=d->ReadRange(a[1].s.c_str(), a[2].v, a[3].v, a[4].v, a[5].v);
	else if(c && !strcmp(k,"ds"))		rr=c->ReadAll(a[1].s.c_str());
	else if(c && !strcmp(k,"dsn"))		rr=c->ReadAll(a[1].s.c_str(), a[2].v);
	else if(c && !strcmp(k,"dsnn"))		rr=c->ReadRange(a[1].s.c_str(), a[2].v, a[3].v);
	else if(c && !strcmp(k,"dsnnn"))	rr=c->ReadRange(a[1].s.c_str(), a[2].v, a[3].v, a[4].v);
	else if(c && !strcmp(k,"dsnnnn"))	rr=c->ReadRange(a[1].s.c_str(), a[2].v, a[3].v, a[4].v, a[5].v);
	else res = 1;
	if(!rr)	gr->SetWarn(mglWarnFile,"ReadAll");
	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_readhdf(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d && !strcmp(k,"dss"))	d->ReadHDF(a[1].s.c_str(), a[2].s.c_str());
	else if(c && !strcmp(k,"dss"))	c->ReadHDF(a[1].s.c_str(), a[2].s.c_str());
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_savehdf(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"dss"))	a[0].d->SaveHDF(a[1].s.c_str(), a[2].s.c_str());
	else if(!strcmp(k,"dssn"))	a[0].d->SaveHDF(a[1].s.c_str(), a[2].s.c_str(),mgl_int(a[3].v));
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_rect(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;	gr->Self()->SaveState(opt);
	if(!strcmp(k,"nnnn"))
		gr->Face(mglPoint(a[0].v,a[1].v,NAN), mglPoint(a[0].v,a[3].v,NAN),
				 mglPoint(a[2].v,a[1].v,NAN), mglPoint(a[2].v,a[3].v,NAN));
	else if(!strcmp(k,"nnnns"))
		gr->Face(mglPoint(a[0].v,a[1].v,NAN), mglPoint(a[0].v,a[3].v,NAN),
					mglPoint(a[2].v,a[1].v,NAN), mglPoint(a[2].v,a[3].v,NAN),a[4].s.c_str());
	else if(!strncmp(k,"nnnnnn",6))
	{
		if(a[0].v==a[3].v)	gr->Face(mglPoint(a[0].v,a[1].v,a[2].v),
			mglPoint(a[0].v,a[4].v,a[2].v),
			mglPoint(a[3].v,a[1].v,a[5].v),
			mglPoint(a[3].v,a[4].v,a[5].v),
			k[6]=='s' ? a[6].s.c_str() : 0);
		else	gr->Face(mglPoint(a[0].v,a[1].v,a[2].v),
			mglPoint(a[0].v,a[4].v,a[5].v),
			mglPoint(a[3].v,a[1].v,a[2].v),
			mglPoint(a[3].v,a[4].v,a[5].v),
			k[6]=='s' ? a[6].s.c_str() : 0);
	}
	else res = 1;	gr->Self()->LoadState();	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_face(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;	gr->Self()->SaveState(opt);
	if(!strcmp(k,"nnnnnnnn"))
		gr->Face(mglPoint(a[0].v,a[1].v,NAN), mglPoint(a[2].v,a[3].v,NAN),
				mglPoint(a[4].v,a[5].v,NAN), mglPoint(a[6].v,a[7].v,NAN));
	else if(!strcmp(k,"nnnnnnnns"))
		gr->Face(mglPoint(a[0].v,a[1].v,NAN), mglPoint(a[2].v,a[3].v,NAN),
				mglPoint(a[4].v,a[5].v,NAN), mglPoint(a[6].v,a[7].v,NAN), a[8].s.c_str());
	else if(!strcmp(k,"nnnnnnnnnnnn"))
		gr->Face(mglPoint(a[0].v,a[1].v,a[2].v), mglPoint(a[3].v,a[4].v,a[5].v),
				mglPoint(a[6].v,a[7].v,a[8].v), mglPoint(a[9].v,a[10].v,a[11].v));
	else if(!strcmp(k,"nnnnnnnnnnnns"))
		gr->Face(mglPoint(a[0].v,a[1].v,a[2].v), mglPoint(a[3].v,a[4].v,a[5].v),
				mglPoint(a[6].v,a[7].v,a[8].v), mglPoint(a[9].v,a[10].v,a[11].v), a[12].s.c_str());
	else res = 1;	gr->Self()->LoadState();	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_logo(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"s"))	gr->Logo(a[0].s.c_str(),false,opt);
	else if(!strcmp(k,"sn"))	gr->Logo(a[0].s.c_str(),mgl_int(a[1].v),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_resize(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d && !strcmp(k,"ddn"))	*d = mglData(true,mgl_data_resize_box(a[1].d, mgl_int(a[2].v),0,0, 0,1, 0,1, 0,1));
	else if(d && !strcmp(k,"ddnn"))	*d = mglData(true,mgl_data_resize_box(a[1].d, mgl_int(a[2].v),mgl_int(a[3].v),0, 0,1, 0,1, 0,1));
	else if(d && !strcmp(k,"ddnnn"))*d = mglData(true,mgl_data_resize_box(a[1].d, mgl_int(a[2].v),mgl_int(a[3].v),mgl_int(a[4].v), 0,1, 0,1, 0,1));
	else if(c && !strcmp(k,"ddn"))	*c = mglDataC(true,mgl_datac_resize_box(a[1].d, mgl_int(a[2].v),0,0, 0,1, 0,1, 0,1));
	else if(c && !strcmp(k,"ddnn"))	*c = mglDataC(true,mgl_datac_resize_box(a[1].d, mgl_int(a[2].v),mgl_int(a[3].v),0, 0,1, 0,1, 0,1));
	else if(c && !strcmp(k,"ddnnn"))*c = mglDataC(true,mgl_datac_resize_box(a[1].d, mgl_int(a[2].v),mgl_int(a[3].v),mgl_int(a[4].v), 0,1, 0,1, 0,1));
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_rotate(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"nn"))	gr->Rotate(a[0].v, a[1].v, 0);
	else if(!strcmp(k,"nnn"))	gr->Rotate(a[0].v, a[1].v, a[2].v);
	else if(!strcmp(k,"nnnn"))	gr->RotateN(a[0].v, a[1].v, a[2].v, a[3].v);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_rotatetext(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"n"))	gr->SetRotatedText(a[0].v!=0);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_tuneticks(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"n"))	gr->SetTuneTicks(mgl_int(a[0].v));
	else if(!strcmp(k,"nn"))	gr->SetTuneTicks(mgl_int(a[0].v),a[1].v);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_ticktime(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"s"))	gr->SetTicksTime(a[0].s[0]);
	else if(!strcmp(k,"sn"))	gr->SetTicksTime(a[0].s[0],a[1].v);
	else if(!strcmp(k,"sns"))	gr->SetTicksTime(a[0].s[0],a[1].v,a[2].s.c_str());
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_save(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"ds"))	a[0].d->Save(a[1].s.c_str());
	else if(!strcmp(k,"ss"))
	{
		FILE *fp = fopen(a[1].s.c_str(),"a");
		size_t pos;	std::string s=a[0].s;
		while((pos=s.find("\\n"))!=std::string::npos)
		{	s[pos]=' ';	s[pos+1]='\n';	}
		while((pos=s.find("\b\b"))!=std::string::npos)	s.erase(pos,2);
		fprintf(fp,"%s\n",s.c_str());	fclose(fp);
	}
	else if(!strcmp(k,"sss"))
	{
		FILE *fp = fopen(a[1].s.c_str(),a[2].s.c_str());
		size_t pos;	std::string s=a[0].s;
		while((pos=s.find("\\n"))!=std::string::npos)
		{	s[pos]=' ';	s[pos+1]='\n';	}
		while((pos=s.find("\b\b"))!=std::string::npos)	s.erase(pos,2);
		fprintf(fp,"%s\n",s.c_str());	fclose(fp);
	}
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_smooth(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d && !strcmp(k,"d"))	d->Smooth();
	else if(d && !strcmp(k,"ds"))	d->Smooth(a[1].s.c_str());
	else if(c && !strcmp(k,"d"))	c->Smooth();
	else if(c && !strcmp(k,"ds"))	c->Smooth(a[1].s.c_str());
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_swap(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d && !strcmp(k,"ds"))	d->Swap(a[1].s.c_str());
	else if(c && !strcmp(k,"ds"))	c->Swap(a[1].s.c_str());
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_idset(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d && !strcmp(k,"ds"))	d->SetColumnId(a[1].s.c_str());
	else if(c && !strcmp(k,"ds"))	c->SetColumnId(a[1].s.c_str());
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_stem(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->Stem(*(a[0].d),"",opt);
	else if(!strcmp(k,"ds"))	gr->Stem(*(a[0].d),a[1].s.c_str(),opt);
	else if(!strcmp(k,"dd"))	gr->Stem(*(a[0].d),*(a[1].d),"",opt);
	else if(!strcmp(k,"dds"))	gr->Stem(*(a[0].d),*(a[1].d),a[2].s.c_str(),opt);
	else if(!strcmp(k,"ddd"))	gr->Stem(*(a[0].d),*(a[1].d),*(a[2].d),"",opt);
	else if(!strcmp(k,"ddds"))	gr->Stem(*(a[0].d),*(a[1].d),*(a[2].d),a[3].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_step(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->Step(*(a[0].d),"",opt);
	else if(!strcmp(k,"ds"))	gr->Step(*(a[0].d),a[1].s.c_str(),opt);
	else if(!strcmp(k,"dd"))	gr->Step(*(a[0].d),*(a[1].d),"",opt);
	else if(!strcmp(k,"dds"))	gr->Step(*(a[0].d),*(a[1].d),a[2].s.c_str(),opt);
	else if(!strcmp(k,"ddd"))	gr->Step(*(a[0].d),*(a[1].d),*(a[2].d),"",opt);
	else if(!strcmp(k,"ddds"))	gr->Step(*(a[0].d),*(a[1].d),*(a[2].d),a[3].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_plot(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->Plot(*(a[0].d),"",opt);
	else if(!strcmp(k,"ds"))	gr->Plot(*(a[0].d),a[1].s.c_str(),opt);
	else if(!strcmp(k,"dd"))	gr->Plot(*(a[0].d),*(a[1].d),"",opt);
	else if(!strcmp(k,"dds"))	gr->Plot(*(a[0].d),*(a[1].d),a[2].s.c_str(),opt);
	else if(!strcmp(k,"ddd"))	gr->Plot(*(a[0].d),*(a[1].d),*(a[2].d),"",opt);
	else if(!strcmp(k,"ddds"))	gr->Plot(*(a[0].d),*(a[1].d),*(a[2].d),a[3].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_tape(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->Tape(*(a[0].d),"",opt);
	else if(!strcmp(k,"ds"))	gr->Tape(*(a[0].d),a[1].s.c_str(),opt);
	else if(!strcmp(k,"dd"))	gr->Tape(*(a[0].d),*(a[1].d),"",opt);
	else if(!strcmp(k,"dds"))	gr->Tape(*(a[0].d),*(a[1].d),a[2].s.c_str(),opt);
	else if(!strcmp(k,"ddd"))	gr->Tape(*(a[0].d),*(a[1].d),*(a[2].d),"",opt);
	else if(!strcmp(k,"ddds"))	gr->Tape(*(a[0].d),*(a[1].d),*(a[2].d),a[3].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_boxplot(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->BoxPlot(*(a[0].d),"",opt);
	else if(!strcmp(k,"ds"))	gr->BoxPlot(*(a[0].d),a[1].s.c_str(),opt);
	else if(!strcmp(k,"dd"))	gr->BoxPlot(*(a[0].d),*(a[1].d),"",opt);
	else if(!strcmp(k,"dds"))	gr->BoxPlot(*(a[0].d),*(a[1].d),a[2].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_candle(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->Candle(*(a[0].d),"",opt);
	else if(!strcmp(k,"ds"))	gr->Candle(*(a[0].d),a[1].s.c_str(),opt);
	else if(!strcmp(k,"dd"))	gr->Candle(*(a[0].d),*(a[1].d),"",opt);
	else if(!strcmp(k,"dds"))	gr->Candle(*(a[0].d),*(a[1].d),a[2].s.c_str(),opt);
	else if(!strcmp(k,"ddd"))	gr->Candle(*(a[0].d),*(a[1].d),*(a[2].d),"",opt);
	else if(!strcmp(k,"ddds"))	gr->Candle(*(a[0].d),*(a[1].d),*(a[2].d),a[3].s.c_str(),opt);
	else if(!strcmp(k,"dddd"))	gr->Candle(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),"",opt);
	else if(!strcmp(k,"dddds"))	gr->Candle(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),a[4].s.c_str(),opt);
	else if(!strcmp(k,"ddddd"))	gr->Candle(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),"",opt);
	else if(!strcmp(k,"ddddds"))gr->Candle(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),a[5].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_radar(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->Radar(*(a[0].d),"",opt);
	else if(!strcmp(k,"ds"))	gr->Radar(*(a[0].d),a[1].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_squeeze(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d && !strcmp(k,"dn"))	d->Squeeze(mgl_int(a[1].v));
	else if(d && !strcmp(k,"dnn"))	d->Squeeze(mgl_int(a[1].v), mgl_int(a[2].v));
	else if(d && !strcmp(k,"dnnn"))	d->Squeeze(mgl_int(a[1].v), mgl_int(a[2].v),mgl_int(a[3].v));
	else if(d && !strcmp(k,"dnnnn"))d->Squeeze(mgl_int(a[1].v), mgl_int(a[2].v),mgl_int(a[3].v), a[4].v);
	else if(c && !strcmp(k,"dn"))	c->Squeeze(mgl_int(a[1].v));
	else if(c && !strcmp(k,"dnn"))	c->Squeeze(mgl_int(a[1].v), mgl_int(a[2].v));
	else if(c && !strcmp(k,"dnnn"))	c->Squeeze(mgl_int(a[1].v), mgl_int(a[2].v),mgl_int(a[3].v));
	else if(c && !strcmp(k,"dnnnn"))c->Squeeze(mgl_int(a[1].v), mgl_int(a[2].v),mgl_int(a[3].v), a[4].v);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_stfad(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	if(!d)	return 1;
	if(!strcmp(k,"dddn"))		*d = mglSTFA(*(a[1].d),*(a[2].d), mgl_int(a[3].v));
	else if(!strcmp(k,"dddns"))	*d = mglSTFA(*(a[1].d),*(a[2].d), mgl_int(a[3].v), a[4].s.c_str()[0]);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_setsize(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"nn") && a[1].v>1 && a[0].v>1)
		gr->SetSize(mgl_int(a[0].v), mgl_int(a[1].v));
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_setsizescl(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"n"))	gr->SetSizeScl(a[0].v);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_sphere(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;	gr->Self()->SaveState(opt);
	if(!strcmp(k,"nnn"))	gr->Sphere(mglPoint(a[0].v,a[1].v), a[2].v);
	else if(!strcmp(k,"nnns"))	gr->Sphere(mglPoint(a[0].v,a[1].v), a[2].v, a[3].s.c_str());
	else if(!strcmp(k,"nnnn"))	gr->Sphere(mglPoint(a[0].v,a[1].v,a[2].v), a[3].v);
	else if(!strcmp(k,"nnnns"))	gr->Sphere(mglPoint(a[0].v,a[1].v,a[2].v), a[3].v, a[4].s.c_str());
	else res = 1;	gr->Self()->LoadState();	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_stfa(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"ddn"))
		gr->STFA(*(a[0].d),*(a[1].d), mgl_int(a[2].v), "",opt);
	else if(!strcmp(k,"ddns"))
		gr->STFA(*(a[0].d),*(a[1].d), mgl_int(a[2].v), a[3].s.c_str(),opt);
	else if(!strcmp(k,"ddddn"))
		gr->STFA(*(a[0].d),*(a[1].d), *(a[2].d),*(a[3].d), mgl_int(a[4].v), "",opt);
	else if(!strcmp(k,"ddddns"))
		gr->STFA(*(a[0].d),*(a[1].d), *(a[2].d),*(a[3].d), mgl_int(a[4].v), a[5].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_surf3(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->Surf3(*(a[0].d),"",opt);
	else if(!strcmp(k,"ds"))	gr->Surf3(*(a[0].d),a[1].s.c_str(),opt);
	else if(!strcmp(k,"dn"))	gr->Surf3(a[1].v,*(a[0].d),"",opt);
	else if(!strcmp(k,"dns"))	gr->Surf3(a[1].v,*(a[0].d),a[2].s.c_str(),opt);
	else if(!strcmp(k,"dddd"))	gr->Surf3(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d), "",opt);
	else if(!strcmp(k,"dddds"))	gr->Surf3(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d), a[4].s.c_str(),opt);
	else if(!strcmp(k,"ddddn"))	gr->Surf3(a[4].v,*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),"",opt);
	else if(!strcmp(k,"ddddns"))gr->Surf3(a[4].v,*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),a[5].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_surf3c(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"dd"))	gr->Surf3C(*(a[0].d),*(a[1].d),"",opt);
	else if(!strcmp(k,"dds"))	gr->Surf3C(*(a[0].d),*(a[1].d),a[2].s.c_str(),opt);
	else if(!strcmp(k,"ddn"))	gr->Surf3C(a[2].v,*(a[0].d),*(a[1].d),"",opt);
	else if(!strcmp(k,"ddns"))	gr->Surf3C(a[2].v,*(a[0].d),*(a[1].d),a[3].s.c_str(),opt);
	else if(!strcmp(k,"ddddd"))
		gr->Surf3C(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d), "",opt);
	else if(!strcmp(k,"ddddds"))
		gr->Surf3C(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d), a[5].s.c_str(),opt);
	else if(!strcmp(k,"dddddn"))
		gr->Surf3C(a[5].v,*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),"",opt);
	else if(!strcmp(k,"dddddns"))
		gr->Surf3C(a[5].v,*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),a[6].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_surf3a(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"dd"))	gr->Surf3A(*(a[0].d),*(a[1].d),"",opt);
	else if(!strcmp(k,"dds"))	gr->Surf3A(*(a[0].d),*(a[1].d),a[2].s.c_str(),opt);
	else if(!strcmp(k,"ddn"))	gr->Surf3A(a[2].v,*(a[0].d),*(a[1].d),"",opt);
	else if(!strcmp(k,"ddns"))	gr->Surf3A(a[2].v,*(a[0].d),*(a[1].d),a[3].s.c_str(),opt);
	else if(!strcmp(k,"ddddd"))
		gr->Surf3A(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d), "",opt);
	else if(!strcmp(k,"ddddds"))
		gr->Surf3A(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d), a[5].s.c_str(),opt);
	else if(!strcmp(k,"dddddn"))
		gr->Surf3A(a[5].v,*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),"",opt);
	else if(!strcmp(k,"dddddns"))
		gr->Surf3A(a[5].v,*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),a[6].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_surf3ca(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"ddd"))	gr->Surf3CA(*(a[0].d),*(a[1].d),*(a[2].d),"",opt);
	else if(!strcmp(k,"ddds"))	gr->Surf3CA(*(a[0].d),*(a[1].d),*(a[2].d),a[3].s.c_str(),opt);
	else if(!strcmp(k,"dddn"))	gr->Surf3CA(a[4].v,*(a[0].d),*(a[1].d),*(a[2].d),"",opt);
	else if(!strcmp(k,"dddns"))	gr->Surf3CA(a[4].v,*(a[0].d),*(a[1].d),*(a[2].d),a[4].s.c_str(),opt);
	else if(!strcmp(k,"dddddd"))
		gr->Surf3CA(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),*(a[5].d), "",opt);
	else if(!strcmp(k,"dddddds"))
		gr->Surf3CA(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),*(a[5].d), a[6].s.c_str(),opt);
	else if(!strcmp(k,"ddddddn"))
		gr->Surf3CA(a[6].v,*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),*(a[5].d),"",opt);
	else if(!strcmp(k,"ddddddns"))
		gr->Surf3CA(a[6].v,*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),*(a[5].d),a[7].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_subplot(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"nnn"))	gr->SubPlot(mgl_int(a[0].v), mgl_int(a[1].v), mgl_int(a[2].v));
	else if(!strcmp(k,"nnns"))	gr->SubPlot(mgl_int(a[0].v), mgl_int(a[1].v), mgl_int(a[2].v), a[3].s.c_str());
	else if(!strcmp(k,"nnnsnn"))	gr->SubPlot(mgl_int(a[0].v), mgl_int(a[1].v), mgl_int(a[2].v), a[3].s.c_str(), a[4].v,a[5].v);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_multiplot(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"nnnnn"))
		gr->MultiPlot(mgl_int(a[0].v), mgl_int(a[1].v), mgl_int(a[2].v), mgl_int(a[3].v), mgl_int(a[4].v));
	else if(!strcmp(k,"nnnnns"))
		gr->MultiPlot(mgl_int(a[0].v), mgl_int(a[1].v), mgl_int(a[2].v), mgl_int(a[3].v), mgl_int(a[4].v), a[5].s.c_str());
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_title(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;	gr->Self()->SaveState(opt);
	if(!strcmp(k,"s"))	gr->Title(a[0].w.c_str());
	else if(!strcmp(k,"ss"))	gr->Title(a[0].w.c_str(), a[1].s.c_str());
	else if(!strcmp(k,"ssn"))	gr->Title(a[0].w.c_str(), a[1].s.c_str(),a[2].v);
	else res = 1;	gr->Self()->LoadState();	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_column(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d && !strcmp(k,"dds"))	*d = mglData(true,mgl_data_column(a[1].d,a[2].s.c_str()));
	else if(c && !strcmp(k,"dds"))	*c = mglDataC(true,mgl_datac_column(a[1].d,a[2].s.c_str()));
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_subdata(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d)
	{
		if(!strcmp(k,"ddn"))		*d = mglData(true,mgl_data_subdata(a[1].d, mgl_int(a[2].v), -1, -1));
		else if(!strcmp(k,"ddnn"))	*d = mglData(true,mgl_data_subdata(a[1].d, mgl_int(a[2].v), mgl_int(a[3].v), -1));
		else if(!strcmp(k,"ddnnn"))	*d = mglData(true,mgl_data_subdata(a[1].d, mgl_int(a[2].v), mgl_int(a[3].v), mgl_int(a[4].v)));
		else if(!strcmp(k,"ddd"))	*d = mglData(true,mgl_data_subdata_ext(a[1].d, a[2].d, 0, 0));
		else if(!strcmp(k,"dddd"))	*d = mglData(true,mgl_data_subdata_ext(a[1].d, a[2].d, a[3].d, 0));
		else if(!strcmp(k,"ddddd"))	*d = mglData(true,mgl_data_subdata_ext(a[1].d, a[2].d, a[3].d, a[4].d));
		else res = 1;
	}
	else if(c)
	{
		if(!strcmp(k,"ddn"))		*c = mglDataC(true,mgl_datac_subdata(a[1].d, mgl_int(a[2].v), -1, -1));
		else if(!strcmp(k,"ddnn"))	*c = mglDataC(true,mgl_datac_subdata(a[1].d, mgl_int(a[2].v), mgl_int(a[3].v), -1));
		else if(!strcmp(k,"ddnnn"))	*c = mglDataC(true,mgl_datac_subdata(a[1].d, mgl_int(a[2].v), mgl_int(a[3].v), mgl_int(a[4].v)));
		else if(!strcmp(k,"ddd"))	*c = mglDataC(true,mgl_datac_subdata_ext(a[1].d, a[2].d, 0, 0));
		else if(!strcmp(k,"dddd"))	*c = mglDataC(true,mgl_datac_subdata_ext(a[1].d, a[2].d, a[3].d, 0));
		else if(!strcmp(k,"ddddd"))	*c = mglDataC(true,mgl_datac_subdata_ext(a[1].d, a[2].d, a[3].d, a[4].d));
		else res = 1;
	}
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_trace(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d && !strcmp(k,"dd"))	*d = mglData(true,mgl_data_trace(a[1].d));
	else if(c && !strcmp(k,"dd"))	*c = mglDataC(true,mgl_datac_trace(a[1].d));
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_tile(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->Tile(*(a[0].d),"",opt);
	else if(!strcmp(k,"ds"))	gr->Tile(*(a[0].d),a[1].s.c_str(),opt);
	else if(!strcmp(k,"ddd"))	gr->Tile(*(a[0].d), *(a[1].d), *(a[2].d), "",opt);
	else if(!strcmp(k,"ddds"))	gr->Tile(*(a[0].d), *(a[1].d), *(a[2].d), a[3].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_tiles(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"dd"))	gr->TileS(*(a[0].d),*(a[1].d),"",opt);
	else if(!strcmp(k,"dds"))	gr->TileS(*(a[0].d),*(a[1].d),a[2].s.c_str(),opt);
	else if(!strcmp(k,"dddd"))	gr->TileS(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),"",opt);
	else if(!strcmp(k,"dddds"))	gr->TileS(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),a[4].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_text(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)		// NOTE don't use options -- Puts can be part of group
{
	int res=0;
	if(k[0]=='n')
	{
		gr->Self()->SaveState(opt);
		if(!strcmp(k,"nns"))	gr->Putsw(mglPoint(a[0].v,a[1].v,NAN),a[2].w.c_str());
		else if(!strcmp(k,"nnss"))	gr->Putsw(mglPoint(a[0].v,a[1].v,NAN),a[2].w.c_str(), a[3].s.c_str());
		else if(!strcmp(k,"nnssn"))	gr->Putsw(mglPoint(a[0].v,a[1].v,NAN),a[2].w.c_str(), a[3].s.c_str(),a[4].v);
		else if(!strcmp(k,"nnns"))		gr->Putsw(mglPoint(a[0].v,a[1].v,a[2].v),a[3].w.c_str());
		else if(!strcmp(k,"nnnss"))		gr->Putsw(mglPoint(a[0].v,a[1].v,a[2].v),a[3].w.c_str(), a[4].s.c_str());
		else if(!strcmp(k,"nnnssn"))	gr->Putsw(mglPoint(a[0].v,a[1].v,a[2].v),a[3].w.c_str(), a[4].s.c_str(),a[5].v);
		else if(!strcmp(k,"nnnns"))		gr->Putsw(mglPoint(a[0].v,a[1].v,NAN), mglPoint(a[2].v,a[3].v), a[4].w.c_str());
		else if(!strcmp(k,"nnnnss"))	gr->Putsw(mglPoint(a[0].v,a[1].v,NAN), mglPoint(a[2].v,a[3].v), a[4].w.c_str(), a[5].s.c_str());
		else if(!strcmp(k,"nnnnssn"))	gr->Putsw(mglPoint(a[0].v,a[1].v,NAN), mglPoint(a[2].v,a[3].v), a[4].w.c_str(), a[5].s.c_str(),a[6].v);
		else if(!strcmp(k,"nnnnnns"))	gr->Putsw(mglPoint(a[0].v,a[1].v,a[2].v), mglPoint(a[3].v,a[4].v,a[5].v), a[6].w.c_str());
		else if(!strcmp(k,"nnnnnnss"))	gr->Putsw(mglPoint(a[0].v,a[1].v,a[2].v), mglPoint(a[3].v,a[4].v,a[5].v), a[6].w.c_str(), a[7].s.c_str());
		else if(!strcmp(k,"nnnnnnssn"))	gr->Putsw(mglPoint(a[0].v,a[1].v,a[2].v), mglPoint(a[3].v,a[4].v,a[5].v), a[6].w.c_str(), a[7].s.c_str(),a[8].v);
		else	res=1;
		gr->Self()->LoadState();
	}
	else if(!strcmp(k,"ds"))	gr->Text(*(a[0].d),a[1].w.c_str(),"",opt);
	else if(!strcmp(k,"dss"))	gr->Text(*(a[0].d),a[1].w.c_str(),a[2].s.c_str(),opt);
	else if(!strcmp(k,"dds"))	gr->Text(*(a[0].d),*(a[1].d),a[2].w.c_str(),"",opt);
	else if(!strcmp(k,"ddss"))	gr->Text(*(a[0].d),*(a[1].d),a[2].w.c_str(),a[3].s.c_str(),opt);
	else if(!strcmp(k,"ddds"))	gr->Text(*(a[0].d),*(a[1].d),*(a[2].d),a[3].w.c_str(),"",opt);
	else if(!strcmp(k,"dddss"))	gr->Text(*(a[0].d),*(a[1].d),*(a[2].d),a[3].w.c_str(),a[4].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_torus(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"dd"))	gr->Torus(*(a[0].d),*(a[1].d),"",opt);
	else if(!strcmp(k,"dds"))	gr->Torus(*(a[0].d),*(a[1].d),a[2].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_transptype(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"n"))	gr->SetTranspType(a[0].v);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_fourier(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *re = dynamic_cast<mglData *>(a[0].d), *im = dynamic_cast<mglData *>(a[1].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(!strcmp(k,"dds") && re && im)	mglFourier(*re,*im,a[2].s.c_str());
	else if(!strcmp(k,"ds") && c)		c->FFT(a[1].s.c_str());
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_transform(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	if(!strcmp(k,"dsdd") && d)	*d = mglTransform(*(a[2].d),*(a[3].d),a[1].s.c_str());
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_transforma(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	if(!strcmp(k,"dsdd") && d)	*d = mglTransformA(*(a[2].d),*(a[3].d),a[1].s.c_str());
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_tube(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"dn"))
		gr->Tube(*(a[0].d),a[1].v,"",opt);
	else if(!strcmp(k,"dns"))
		gr->Tube(*(a[0].d),a[1].v,a[2].s.c_str(),opt);
	else if(!strcmp(k,"dd"))
		gr->Tube(*(a[0].d),*(a[1].d),"",opt);
	else if(!strcmp(k,"dds"))
		gr->Tube(*(a[0].d),*(a[1].d),a[2].s.c_str(),opt);
	else if(!strcmp(k,"ddn"))
		gr->Tube(*(a[0].d),*(a[1].d),a[2].v,"",opt);
	else if(!strcmp(k,"ddns"))
		gr->Tube(*(a[0].d),*(a[1].d),a[2].v,a[3].s.c_str(),opt);
	else if(!strcmp(k,"ddd"))
		gr->Tube(*(a[0].d),*(a[1].d),*(a[2].d),"",opt);
	else if(!strcmp(k,"ddds"))
		gr->Tube(*(a[0].d),*(a[1].d),*(a[2].d),a[3].s.c_str(),opt);
	else if(!strcmp(k,"dddn"))
		gr->Tube(*(a[0].d),*(a[1].d),*(a[2].d),a[3].v,"",opt);
	else if(!strcmp(k,"dddns"))
		gr->Tube(*(a[0].d),*(a[1].d),*(a[2].d),a[3].v,a[4].s.c_str(),opt);
	else if(!strcmp(k,"dddd"))
		gr->Tube(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),"",opt);
	else if(!strcmp(k,"dddds"))
		gr->Tube(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),a[4].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_textmark(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"ds"))	gr->TextMark(*(a[0].d),a[1].w.c_str(),"",opt);
	else if(!strcmp(k,"dss"))	gr->TextMark(*(a[0].d),a[1].w.c_str(),a[2].s.c_str(),opt);
	else if(!strcmp(k,"dds"))	gr->TextMark(*(a[0].d),*(a[1].d),a[2].w.c_str(),"",opt);
	else if(!strcmp(k,"ddss"))	gr->TextMark(*(a[0].d),*(a[1].d),a[2].w.c_str(),a[3].s.c_str(),opt);
	else if(!strcmp(k,"ddds"))	gr->TextMark(*(a[0].d),*(a[1].d),*(a[2].d),a[3].w.c_str(),"",opt);
	else if(!strcmp(k,"dddss"))	gr->TextMark(*(a[0].d),*(a[1].d),*(a[2].d),a[3].w.c_str(),a[4].s.c_str(),opt);
	else if(!strcmp(k,"dddds"))	gr->TextMark(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),a[4].w.c_str(),"",opt);
	else if(!strcmp(k,"ddddss"))gr->TextMark(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),a[4].w.c_str(),a[5].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_triplot(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"ddd"))	gr->TriPlot(*(a[0].d),*(a[1].d),*(a[2].d),"",opt);
	else if(!strcmp(k,"ddds"))	gr->TriPlot(*(a[0].d),*(a[1].d),*(a[2].d),a[3].s.c_str(),opt);
	else if(!strcmp(k,"dddd"))	gr->TriPlot(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),"",opt);
	else if(!strcmp(k,"dddds"))	gr->TriPlot(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),a[4].s.c_str(),opt);
	else if(!strcmp(k,"ddddd"))	gr->TriPlot(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),"",opt);
	else if(!strcmp(k,"ddddds"))gr->TriPlot(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),a[5].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_quadplot(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"ddd"))	gr->QuadPlot(*(a[0].d),*(a[1].d),*(a[2].d),"",opt);
	else if(!strcmp(k,"ddds"))	gr->QuadPlot(*(a[0].d),*(a[1].d),*(a[2].d),a[3].s.c_str(),opt);
	else if(!strcmp(k,"dddd"))	gr->QuadPlot(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),"",opt);
	else if(!strcmp(k,"dddds"))	gr->QuadPlot(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),a[4].s.c_str(),opt);
	else if(!strcmp(k,"ddddd"))	gr->QuadPlot(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),"",opt);
	else if(!strcmp(k,"ddddds"))gr->QuadPlot(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),a[5].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_tricont(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"dddd"))
		gr->TriCont(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),"",opt);
	else if(!strcmp(k,"dddds"))
		gr->TriCont(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),a[4].s.c_str(),opt);
	else if(!strcmp(k,"ddddd"))
		gr->TriContV(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),"",opt);
	else if(!strcmp(k,"ddddds"))
		gr->TriContV(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),a[5].s.c_str(),opt);
	else if(!strcmp(k,"dddddd"))
		gr->TriCont(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),*(a[5].d),"",opt);
	else if(!strcmp(k,"dddddds"))
		gr->TriCont(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),*(a[5].d),a[6].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_tricontv(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"dddd"))
		gr->TriContVt(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),"",opt);
	else if(!strcmp(k,"dddds"))
		gr->TriContVt(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),a[4].s.c_str(),opt);
	else if(!strcmp(k,"ddddd"))
		gr->TriContVt(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),"",opt);
	else if(!strcmp(k,"ddddds"))
		gr->TriContVt(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),a[5].s.c_str(),opt);
	else if(!strcmp(k,"dddddd"))
		gr->TriContVt(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),*(a[5].d),"",opt);
	else if(!strcmp(k,"dddddds"))
		gr->TriContVt(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),*(a[5].d),a[6].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_ternary(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"n"))	gr->Ternary(int(a[0].v));
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_transpose(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d && !strcmp(k,"d"))	d->Transpose();
	else if(d && !strcmp(k,"ds"))	d->Transpose(a[1].s.c_str());
	else if(c && !strcmp(k,"d"))	c->Transpose();
	else if(c && !strcmp(k,"ds"))	c->Transpose(a[1].s.c_str());
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_vect(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"dd"))	gr->Vect(*(a[0].d),*(a[1].d),"",opt);
	else if(!strcmp(k,"dds"))	gr->Vect(*(a[0].d),*(a[1].d),a[2].s.c_str(),opt);
	else if(!strcmp(k,"ddd"))	gr->Vect(*(a[0].d),*(a[1].d),*(a[2].d),"",opt);
	else if(!strcmp(k,"ddds"))	gr->Vect(*(a[0].d),*(a[1].d),*(a[2].d),a[3].s.c_str(),opt);
	else if(!strcmp(k,"dddd"))	gr->Vect(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),"",opt);
	else if(!strcmp(k,"dddds"))	gr->Vect(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),a[4].s.c_str(),opt);
	else if(!strcmp(k,"dddddd"))	gr->Vect(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),*(a[5].d),"",opt);
	else if(!strcmp(k,"dddddds"))	gr->Vect(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),*(a[5].d),a[6].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_vect3(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"ddd"))
		gr->Vect3(*(a[0].d),*(a[1].d),*(a[2].d),"",-1,opt);
	else if(!strcmp(k,"ddds"))
		gr->Vect3(*(a[0].d),*(a[1].d),*(a[2].d),a[3].s.c_str(),-1,opt);
	else if(!strcmp(k,"dddsn"))
		gr->Vect3(*(a[0].d),*(a[1].d),*(a[2].d),a[3].s.c_str(),a[4].v,opt);
	else if(!strcmp(k,"dddddd"))
		gr->Vect3(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),*(a[5].d),"",-1,opt);
	else if(!strcmp(k,"dddddds"))
		gr->Vect3(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),*(a[5].d),a[6].s.c_str(),-1,opt);
	else if(!strcmp(k,"ddddddsn"))
		gr->Vect3(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),*(a[5].d),a[6].s.c_str(),a[7].v,opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_traj(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"dddd"))
		gr->Traj(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),"",opt);
	else if(!strcmp(k,"dddds"))
		gr->Traj(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),a[4].s.c_str(),opt);
	else if(!strcmp(k,"dddddd"))
		gr->Traj(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),*(a[5].d),"",opt);
	else if(!strcmp(k,"dddddds"))
		gr->Traj(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),*(a[5].d),a[6].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_xlabel(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"s"))	gr->Label('x', a[0].w.c_str(), 1, opt);
	else if(!strcmp(k,"sn"))	gr->Label('x', a[0].w.c_str(), a[1].v, opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_ylabel(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"s"))	gr->Label('y', a[0].w.c_str(), 1, opt);
	else if(!strcmp(k,"sn"))	gr->Label('y', a[0].w.c_str(), a[1].v, opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_zlabel(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"s"))	gr->Label('z', a[0].w.c_str(), 1, opt);
	else if(!strcmp(k,"sn"))	gr->Label('z', a[0].w.c_str(), a[1].v, opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_tlabel(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"s"))	gr->Label('t', a[0].w.c_str(), 1, opt);
	else if(!strcmp(k,"sn"))	gr->Label('t', a[0].w.c_str(), a[1].v, opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_label(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"ds"))	gr->Label(*(a[0].d), a[1].w.c_str(), "",opt);
	else if(!strcmp(k,"dss"))	gr->Label(*(a[0].d), a[1].w.c_str(), a[2].s.c_str(),opt);
	else if(!strcmp(k,"dds"))	gr->Label(*(a[0].d), *(a[1].d), a[2].w.c_str(), "",opt);
	else if(!strcmp(k,"ddss"))	gr->Label(*(a[0].d), *(a[1].d), a[2].w.c_str(), a[3].s.c_str(),opt);
	else if(!strcmp(k,"ddds"))	gr->Label(*(a[0].d), *(a[1].d), *(a[2].d), a[3].w.c_str(), "",opt);
	else if(!strcmp(k,"dddss"))	gr->Label(*(a[0].d), *(a[1].d), *(a[2].d), a[3].w.c_str(), a[4].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_table(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->Table(*(a[0].d), L"", "#|",opt);
	else if(!strcmp(k,"ds"))	gr->Table(*(a[0].d), a[1].w.c_str(), "#|",opt);
	else if(!strcmp(k,"dss"))	gr->Table(*(a[0].d), a[1].w.c_str(), a[2].s.c_str(),opt);
	else if(!strcmp(k,"nnd"))	gr->Table(a[0].v, a[1].v, *(a[2].d), L"", "#|",opt);
	else if(!strcmp(k,"nnds"))	gr->Table(a[0].v, a[1].v, *(a[2].d), a[3].w.c_str(), "#|",opt);
	else if(!strcmp(k,"nndss"))	gr->Table(a[0].v, a[1].v, *(a[2].d), a[3].w.c_str(), a[4].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_xrange(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->SetRange('x',*(a[0].d));
	else if(!strcmp(k,"dn"))	gr->SetRange('x',*(a[0].d),a[1].v);
	else if(!strcmp(k,"nn"))	gr->SetRange('x', a[0].v, a[1].v);
	else if(!strcmp(k,"nnn"))
	{
		if(a[2].v)	gr->AddRange('x', a[0].v, a[1].v);
		else	gr->SetRange('x', a[0].v, a[1].v);
	}
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_yrange(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->SetRange('y',*(a[0].d));
	else if(!strcmp(k,"dn"))	gr->SetRange('y',*(a[0].d),a[1].v);
	else if(!strcmp(k,"nn"))	gr->SetRange('y', a[0].v, a[1].v);
	else if(!strcmp(k,"nnn"))
	{
		if(a[2].v)	gr->AddRange('y', a[0].v, a[1].v);
		else	gr->SetRange('y', a[0].v, a[1].v);
	}
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_zrange(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->SetRange('z',*(a[0].d));
	else if(!strcmp(k,"dn"))	gr->SetRange('z',*(a[0].d),a[1].v);
	else if(!strcmp(k,"nn"))	gr->SetRange('z', a[0].v, a[1].v);
	else if(!strcmp(k,"nnn"))
	{
		if(a[2].v)	gr->AddRange('z', a[0].v, a[1].v);
		else	gr->SetRange('z', a[0].v, a[1].v);
	}
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_ctick(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"s"))	gr->SetTickTempl('c',a[0].w.c_str());
	else if(!strcmp(k,"n"))	gr->SetTicks('c',a[0].v,0,0);
	else if(!strcmp(k,"ns"))	gr->SetTicks('c',a[0].v,0,0,a[1].w.c_str());
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_xtick(mglGraph *gr, long n, mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"n"))	gr->SetTicks('x', a[0].v);
	else if(!strcmp(k,"ns"))	gr->SetTicks('x', a[0].v, 0, NAN, a[1].w.c_str());
	else if(!strcmp(k,"nn"))	gr->SetTicks('x', a[0].v, mgl_int(a[1].v));
	else if(!strcmp(k,"nns"))	gr->SetTicks('x', a[0].v, mgl_int(a[1].v), NAN, a[2].w.c_str());
	else if(!strcmp(k,"nnn"))	gr->SetTicks('x', a[0].v, mgl_int(a[1].v), a[2].v);
	else if(!strcmp(k,"nnns"))	gr->SetTicks('x', a[0].v, mgl_int(a[1].v), a[2].v, a[3].w.c_str());
	else if(!strcmp(k,"s"))		gr->SetTickTempl('x',a[0].w.c_str());
	else if(!strcmp(k,"ds"))	gr->SetTicksVal('x', *(a[0].d), a[1].w.c_str());
	else if(!strcmp(k,"dsn"))	gr->SetTicksVal('x', *(a[0].d), a[1].w.c_str(), a[2].v);
	else if(!strncmp(k,"ns",2))
	{
		mreal v[50];	std::wstring s;	int i;
		for(i=0;i<50 && i<n/2;i++)
		{
			if(a[2*i].type==2 && a[2*i+1].type==1)
			{	v[i] = a[2*i].v;	s += a[2*i+1].w+L"\n";	}
			else	break;
		}
		gr->SetTicksVal('x',mglData(i,v),s.c_str(),a[2*i].type==2?a[2*i].v:0);
	}
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_ytick(mglGraph *gr, long n, mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"n"))	gr->SetTicks('y', a[0].v);
	else if(!strcmp(k,"ns"))	gr->SetTicks('y', a[0].v, 0, NAN, a[1].w.c_str());
	else if(!strcmp(k,"nn"))	gr->SetTicks('y', a[0].v, mgl_int(a[1].v));
	else if(!strcmp(k,"nns"))	gr->SetTicks('y', a[0].v, mgl_int(a[1].v), NAN, a[2].w.c_str());
	else if(!strcmp(k,"nnn"))	gr->SetTicks('y', a[0].v, mgl_int(a[1].v), a[2].v);
	else if(!strcmp(k,"nnns"))	gr->SetTicks('y', a[0].v, mgl_int(a[1].v), a[2].v, a[3].w.c_str());
	else if(!strcmp(k,"s"))		gr->SetTickTempl('y',a[0].w.c_str());
	else if(!strcmp(k,"ds"))	gr->SetTicksVal('y', *(a[0].d), a[1].w.c_str());
	else if(!strcmp(k,"dsn"))	gr->SetTicksVal('y', *(a[0].d), a[1].w.c_str(), a[2].v);
	else if(!strncmp(k,"ns",2))
	{
		mreal v[50];	std::wstring s;	int i;
		for(i=0;i<50 && i<n/2;i++)
		{
			if(a[2*i].type==2 && a[2*i+1].type==1)
			{	v[i] = a[2*i].v;	s += a[2*i+1].w+L"\n";	}
			else	break;
		}
		gr->SetTicksVal('y',mglData(i,v),s.c_str());
	}
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_ztick(mglGraph *gr, long n, mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"n"))	gr->SetTicks('z', a[0].v);
	else if(!strcmp(k,"ns"))	gr->SetTicks('z', a[0].v, 0, NAN, a[1].w.c_str());
	else if(!strcmp(k,"nn"))	gr->SetTicks('z', a[0].v, mgl_int(a[1].v));
	else if(!strcmp(k,"nns"))	gr->SetTicks('z', a[0].v, mgl_int(a[1].v), NAN, a[2].w.c_str());
	else if(!strcmp(k,"nnn"))	gr->SetTicks('z', a[0].v, mgl_int(a[1].v), a[2].v);
	else if(!strcmp(k,"nnns"))	gr->SetTicks('z', a[0].v, mgl_int(a[1].v), a[2].v, a[3].w.c_str());
	else if(!strcmp(k,"s"))		gr->SetTickTempl('z',a[0].w.c_str());
	else if(!strcmp(k,"ds"))	gr->SetTicksVal('z', *(a[0].d), a[1].w.c_str());
	else if(!strcmp(k,"dsn"))	gr->SetTicksVal('z', *(a[0].d), a[1].w.c_str(), a[2].v);
	else if(!strncmp(k,"ns",2))
	{
		mreal v[50];	std::wstring s;	int i;
		for(i=0;i<50 && i<n/2;i++)
		{
			if(a[2*i].type==2 && a[2*i+1].type==1)
			{	v[i] = a[2*i].v;	s += a[2*i+1].w+L"\n";	}
			else	break;
		}
		gr->SetTicksVal('z',mglData(i,v),s.c_str());
	}
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_error(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"dd"))	gr->Error(*(a[0].d),*(a[1].d), "",opt);
	else if(!strcmp(k,"dds"))	gr->Error(*(a[0].d),*(a[1].d), a[2].s.c_str(),opt);
	else if(!strcmp(k,"ddd"))	gr->Error(*(a[0].d),*(a[1].d),*(a[2].d), "",opt);
	else if(!strcmp(k,"ddds"))	gr->Error(*(a[0].d),*(a[1].d),*(a[2].d), a[3].s.c_str(),opt);
	else if(!strcmp(k,"dddd"))	gr->Error(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d), "",opt);
	else if(!strcmp(k,"dddds"))	gr->Error(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d), a[4].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_extend(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d && !strcmp(k,"dn"))	d->Extend(mgl_int(a[1].v));
	else if(d && !strcmp(k,"dnn"))	d->Extend(mgl_int(a[1].v),mgl_int(a[2].v));
	else if(c && !strcmp(k,"dn"))	c->Extend(mgl_int(a[1].v));
	else if(c && !strcmp(k,"dnn"))	c->Extend(mgl_int(a[1].v),mgl_int(a[2].v));
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_join(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if((!d && !c) || k[1]!='d')	res = 1;
	else if(d)	for(long i=1;k[i]=='d';i++)	d->Join(*(a[i].d));
	else if(c)	for(long i=1;k[i]=='d';i++)	c->Join(*(a[i].d));
	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_datas(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"s"))
	{
		char *buf=new char[1024];
		mgl_datas_hdf(a[0].s.c_str(),buf,1024);
		gr->SetWarn(-1,buf);
		delete []buf;
	}
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_info(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->SetWarn(-1,a[0].d->PrintInfo());
	else if(!strcmp(k,"s"))	gr->SetWarn(-1,a[0].s.c_str());
	else if(!strcmp(k,"n"))
	{	char buf[128];	snprintf(buf,128,"value = %g",a[0].v);	buf[127]=0;	gr->SetWarn(-1,buf);	}
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_print(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"d"))	printf("%s\n",a[0].d->PrintInfo());
	else if(!strcmp(k,"s"))	printf("%s\n",a[0].s.c_str());
	else if(!strcmp(k,"n"))	printf("value = %g\n",a[0].v);
	else res = 1;	fflush(stdout);	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_echo(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"d"))	gr->SetWarn(-1,a[0].d->Get().c_str());
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_integrate(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d && !strcmp(k,"ds"))	d->Integral(a[1].s.c_str());
	else if(c && !strcmp(k,"ds"))	c->Integral(a[1].s.c_str());
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_inplot(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"nnnn"))	gr->InPlot(a[0].v, a[1].v, a[2].v, a[3].v);
	else if(!strcmp(k,"nnnnn"))	gr->InPlot(a[0].v, a[1].v, a[2].v, a[3].v, a[4].v!=0);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_columnplot(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"nn"))	gr->ColumnPlot(mgl_int(a[0].v), mgl_int(a[1].v));
	else if(!strcmp(k,"nnn"))	gr->ColumnPlot(mgl_int(a[0].v), mgl_int(a[1].v), a[2].v);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_gridplot(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"nnn"))	gr->GridPlot(mgl_int(a[0].v), mgl_int(a[1].v), mgl_int(a[2].v));
	else if(!strcmp(k,"nnnn"))	gr->GridPlot(mgl_int(a[0].v), mgl_int(a[1].v), mgl_int(a[2].v), a[3].v);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_stickplot(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"nnnn"))	gr->StickPlot(mgl_int(a[0].v), mgl_int(a[1].v), a[2].v, a[3].v);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_shearplot(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"nnnn"))	gr->ShearPlot(mgl_int(a[0].v), mgl_int(a[1].v), a[2].v, a[3].v);
	else if(!strcmp(k,"nnnnnn"))	gr->ShearPlot(mgl_int(a[0].v), mgl_int(a[1].v), a[2].v, a[3].v, a[4].v, a[5].v);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_pipe(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"dd"))	gr->Pipe(*(a[0].d),*(a[1].d),"",0.05,opt);
	else if(!strcmp(k,"dds"))	gr->Pipe(*(a[0].d),*(a[1].d),a[2].s.c_str(),0.05,opt);
	else if(!strcmp(k,"ddsn"))	gr->Pipe(*(a[0].d),*(a[1].d),a[2].s.c_str(),a[3].v,opt);
	else if(!strcmp(k,"dddd"))
		gr->Pipe(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),"",0.05,opt);
	else if(!strcmp(k,"dddds"))
		gr->Pipe(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),a[4].s.c_str(),0.05,opt);
	else if(!strcmp(k,"ddddsn"))
		gr->Pipe(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),a[4].s.c_str(),a[5].v,opt);
	else if(!strcmp(k,"ddd"))
		gr->Pipe(*(a[0].d),*(a[1].d),*(a[2].d),"",0.05,opt);
	else if(!strcmp(k,"ddds"))
		gr->Pipe(*(a[0].d),*(a[1].d),*(a[2].d),a[3].s.c_str(),0.05,opt);
	else if(!strcmp(k,"dddsn"))
		gr->Pipe(*(a[0].d),*(a[1].d),*(a[2].d),a[3].s.c_str(),a[4].v,opt);
	else if(!strcmp(k,"dddddd"))
		gr->Pipe(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),*(a[5].d),"",0.05,opt);
	else if(!strcmp(k,"dddddds"))
		gr->Pipe(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),*(a[5].d),a[6].s.c_str(),0.05,opt);
	else if(!strcmp(k,"ddddddsn"))
		gr->Pipe(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),*(a[5].d),a[6].s.c_str(),a[7].v,opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_origin(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"nn"))	gr->SetOrigin(a[0].v,a[1].v,NAN);
	else if(!strcmp(k,"nnn"))	gr->SetOrigin(a[0].v,a[1].v,a[2].v);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_norm(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	if(!d)	return 1;
	if(!strcmp(k,"dnn"))	d->Norm(a[1].v,a[2].v);
	else if(!strcmp(k,"dnnn"))	d->Norm(a[1].v,a[2].v,a[3].v!=0);
	else if(!strcmp(k,"dnnnn"))	d->Norm(a[1].v,a[2].v,a[3].v!=0,mgl_int(a[4].v));
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_limit(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d && !strcmp(k,"dn"))		d->Limit(a[1].v);
	else if(c && !strcmp(k,"dn"))	c->Limit(a[1].v);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_hist(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	if(!d)	return 1;
	if(!strcmp(k,"ddd"))		*d = gr->Hist(*(a[1].d), *(a[2].d),opt);
	else if(!strcmp(k,"dddd"))	*d = gr->Hist(*(a[1].d), *(a[2].d), *(a[3].d),opt);
	else if(!strcmp(k,"ddddd"))	*d = gr->Hist(*(a[1].d), *(a[2].d), *(a[3].d), *(a[4].d),opt);
	else if(!strcmp(k,"ddnnn"))	*d = mglData(true,mgl_data_hist(a[1].d,int(a[2].v+0.5), a[3].v, a[4].v, 0));
	else if(!strcmp(k,"ddnnnn"))	*d = mglData(true,mgl_data_hist(a[1].d,mgl_int(a[2].v), a[3].v, a[4].v, mgl_int(a[5].v)));
	else if(!strcmp(k,"dddnnn"))	*d = mglData(true,mgl_data_hist_w(a[1].d,a[2].d, mgl_int(a[3].v), a[4].v, a[5].v, 0));
	else if(!strcmp(k,"dddnnnn"))	*d = mglData(true,mgl_data_hist_w(a[1].d,a[2].d, mgl_int(a[3].v), a[4].v, a[5].v, mgl_int(a[6].v)));
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_mirror(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d && !strcmp(k,"ds"))		d->Mirror(a[1].s.c_str());
	else if(c && !strcmp(k,"ds"))	c->Mirror(a[1].s.c_str());
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_hankel(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d && !strcmp(k,"ds"))		d->Hankel(a[1].s.c_str());
	else if(c && !strcmp(k,"ds"))	c->Hankel(a[1].s.c_str());
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_sinfft(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	if(!strcmp(k,"ds") && d)	d->SinFFT(a[1].s.c_str());
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_cosfft(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	if(!strcmp(k,"ds") && d)	d->CosFFT(a[1].s.c_str());
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_wavelet(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	if(!strcmp(k,"dsn") && d)	d->Wavelet(a[1].s.c_str(), mgl_int(a[2].v));
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_new(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d && !strcmp(k,"dn"))	d->Create(mgl_int(a[1].v));
	else if(d && !strcmp(k,"dns"))
	{	d->Create(mgl_int(a[1].v));	d->Fill(gr->Self(),a[2].s.c_str(),opt);	}
	else if(d && !strcmp(k,"dnn"))	d->Create(mgl_int(a[1].v),mgl_int(a[2].v));
	else if(d && !strcmp(k,"dnns"))
	{	d->Create(mgl_int(a[1].v),mgl_int(a[2].v));	d->Fill(gr->Self(),a[3].s.c_str(),opt);	}
	else if(d && !strcmp(k,"dnnn"))	d->Create(mgl_int(a[1].v),mgl_int(a[2].v),mgl_int(a[3].v));
	else if(d && !strcmp(k,"dnnns"))
	{	d->Create(mgl_int(a[1].v),mgl_int(a[2].v),mgl_int(a[3].v));	d->Fill(gr->Self(),a[4].s.c_str(),opt);	}
	else if(c && !strcmp(k,"dn"))	c->Create(mgl_int(a[1].v));
	else if(c && !strcmp(k,"dns"))
	{	c->Create(mgl_int(a[1].v));	c->Fill(gr->Self(),a[2].s.c_str(),opt);	}
	else if(c && !strcmp(k,"dnn"))	c->Create(mgl_int(a[1].v),mgl_int(a[2].v));
	else if(c && !strcmp(k,"dnns"))
	{	c->Create(mgl_int(a[1].v),mgl_int(a[2].v));	c->Fill(gr->Self(),a[3].s.c_str(),opt);	}
	else if(c && !strcmp(k,"dnnn"))	c->Create(mgl_int(a[1].v),mgl_int(a[2].v),mgl_int(a[3].v));
	else if(c && !strcmp(k,"dnnns"))
	{	c->Create(mgl_int(a[1].v),mgl_int(a[2].v),mgl_int(a[3].v));	c->Fill(gr->Self(),a[4].s.c_str(),opt);	}
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_var(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);	// TODO use mglDataV here?!
	if(!d)	return 1;
	if(!strcmp(k,"dnn"))
	{	d->Create(mgl_int(a[1].v));	d->Fill(a[2].v, NAN);	}
	else if(!strcmp(k,"dnnn"))
	{	d->Create(mgl_int(a[1].v));	d->Fill(a[2].v, a[3].v);	}
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_chdir(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"s"))
	{	if(chdir(a[0].s.c_str()))	gr->SetWarn(mglWarnFile,"chdir");	}
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_perspective(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"n"))	gr->Perspective(a[0].v);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_facex(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;	gr->Self()->SaveState(opt);
	if(!strcmp(k,"nnnnn"))	gr->FaceX(mglPoint(a[0].v, a[1].v, a[2].v), a[3].v, a[4].v);
	else if(!strcmp(k,"nnnnns"))	gr->FaceX(mglPoint(a[0].v, a[1].v, a[2].v), a[3].v, a[4].v, a[5].s.c_str());
	else if(!strcmp(k,"nnnnnsnn"))	gr->FaceX(mglPoint(a[0].v, a[1].v, a[2].v), a[3].v, a[4].v, a[5].s.c_str(),a[6].v,a[7].v);
	else res = 1;	gr->Self()->LoadState();	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_facey(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;	gr->Self()->SaveState(opt);
	if(!strcmp(k,"nnnnn"))	gr->FaceY(mglPoint(a[0].v, a[1].v, a[2].v), a[3].v, a[4].v);
	else if(!strcmp(k,"nnnnns"))	gr->FaceY(mglPoint(a[0].v, a[1].v, a[2].v), a[3].v, a[4].v, a[5].s.c_str());
	else if(!strcmp(k,"nnnnnsnn"))	gr->FaceY(mglPoint(a[0].v, a[1].v, a[2].v), a[3].v, a[4].v, a[5].s.c_str(),a[6].v,a[7].v);
	else res = 1;	gr->Self()->LoadState();	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_facez(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;	gr->Self()->SaveState(opt);
	if(!strcmp(k,"nnnnn"))	gr->FaceZ(mglPoint(a[0].v, a[1].v, a[2].v), a[3].v, a[4].v);
	else if(!strcmp(k,"nnnnns"))	gr->FaceZ(mglPoint(a[0].v, a[1].v, a[2].v), a[3].v, a[4].v, a[5].s.c_str());
	else if(!strcmp(k,"nnnnnsnn"))	gr->FaceZ(mglPoint(a[0].v, a[1].v, a[2].v), a[3].v, a[4].v, a[5].s.c_str(),a[6].v,a[7].v);
	else res = 1;	gr->Self()->LoadState();	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_normsl(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	if(!d)	return 1;
	if(!strcmp(k,"dnn"))	d->NormSl(a[1].v, a[2].v);
	else if(!strcmp(k,"dnns"))	d->NormSl(a[1].v, a[2].v, a[3].s.c_str()[0]);
	else if(!strcmp(k,"dnnsn"))	d->NormSl(a[1].v, a[2].v, a[3].s.c_str()[0],a[4].v);
	else if(!strcmp(k,"dnnsnn"))d->NormSl(a[1].v, a[2].v, a[3].s.c_str()[0],a[4].v,a[5].v);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_momentum(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d && !strcmp(k,"dds"))	*d = mglData(true,mgl_data_momentum(a[1].d,'z', a[2].s.c_str()));
	else if(d && !strcmp(k,"ddss"))	*d = mglData(true,mgl_data_momentum(a[1].d,a[3].s.c_str()[0], a[2].s.c_str()));
	else if(c && !strcmp(k,"dds"))	*c = mglDataC(true,mgl_datac_momentum(a[1].d,'z', a[2].s.c_str()));
	else if(c && !strcmp(k,"ddss"))	*c = mglDataC(true,mgl_datac_momentum(a[1].d,a[3].s.c_str()[0], a[2].s.c_str()));
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_pulse(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	if(d && !strcmp(k,"dds"))	*d = mglData(true,mgl_data_pulse(a[1].d,a[2].s[0]));
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_fit(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	if(!d)	return 1;
	if(!strcmp(k,"dddddssd"))
	{
		mglData *i = dynamic_cast<mglData *>(a[7].d);
		if(i)	*d = gr->Fit(*(a[1].d), *(a[2].d), *(a[3].d), *(a[4].d), a[5].s.c_str(), a[6].s.c_str(), *i,opt);
		else	res = 1;
	}
	else if(!strcmp(k,"dddddss"))
		*d = gr->Fit(*(a[1].d), *(a[2].d), *(a[3].d), *(a[4].d), a[5].s.c_str(), a[6].s.c_str(),opt);
	else if(!strcmp(k,"ddddssd"))
	{
		mglData *i = dynamic_cast<mglData *>(a[6].d);
		if(i)	*d = gr->Fit(*(a[1].d), *(a[2].d), *(a[3].d), a[4].s.c_str(), a[5].s.c_str(), *i,opt);
		else	res = 1;
	}
	else if(!strcmp(k,"ddddss"))
		*d = gr->Fit(*(a[1].d), *(a[2].d), *(a[3].d), a[4].s.c_str(), a[5].s.c_str(),opt);
	else if(!strcmp(k,"dddssd"))
	{
		mglData *i = dynamic_cast<mglData *>(a[5].d);
		if(i)	*d = gr->Fit(*(a[1].d), *(a[2].d), a[3].s.c_str(), a[4].s.c_str(), *i,opt);
		else	res = 1;
	}
	else if(!strcmp(k,"dddss"))
		*d = gr->Fit(*(a[1].d), *(a[2].d), a[3].s.c_str(), a[4].s.c_str(),opt);
	else if(!strcmp(k,"ddssd"))
	{
		mglData *i = dynamic_cast<mglData *>(a[4].d);
		if(i)	*d = gr->Fit(*(a[1].d), a[2].s.c_str(), a[3].s.c_str(), *i,opt);
		else	res = 1;
	}
	else if(!strcmp(k,"ddss"))
		*d = gr->Fit(*(a[1].d), a[2].s.c_str(), a[3].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_fits(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	if(!d)	return 1;
	if(!strcmp(k,"ddddddssd"))
	{
		mglData *i = dynamic_cast<mglData *>(a[8].d);
		if(i)	*d = gr->FitS(*(a[1].d), *(a[2].d), *(a[3].d), *(a[4].d), *(a[5].d), a[6].s.c_str(), a[7].s.c_str(), *i,opt);
		else	res = 1;
	}
	else if(!strcmp(k,"ddddddss"))
		*d = gr->FitS(*(a[1].d), *(a[2].d), *(a[3].d), *(a[4].d), *(a[5].d), a[6].s.c_str(), a[7].s.c_str(),opt);
	else if(!strcmp(k,"dddddssd"))
	{
		mglData *i = dynamic_cast<mglData *>(a[7].d);
		if(i)	*d = gr->FitS(*(a[1].d), *(a[2].d), *(a[3].d), *(a[4].d), a[5].s.c_str(), a[6].s.c_str(), *i,opt);
		else	res = 1;
	}
	else if(!strcmp(k,"dddddss"))
		*d = gr->FitS(*(a[1].d), *(a[2].d), *(a[3].d), *(a[4].d), a[5].s.c_str(), a[6].s.c_str(),opt);
	else if(!strcmp(k,"ddddssd"))
	{
		mglData *i = dynamic_cast<mglData *>(a[6].d);
		if(i)	*d = gr->FitS(*(a[1].d), *(a[2].d), *(a[3].d), a[4].s.c_str(), a[5].s.c_str(), *i,opt);
		else	res = 1;
	}
	else if(!strcmp(k,"ddddss"))
		*d = gr->FitS(*(a[1].d), *(a[2].d), *(a[3].d), a[4].s.c_str(), a[5].s.c_str(),opt);
	else if(!strcmp(k,"dddssd"))
	{
		mglData *i = dynamic_cast<mglData *>(a[5].d);
		if(i)	*d = gr->FitS(*(a[1].d), *(a[2].d), a[3].s.c_str(), a[4].s.c_str(), *i,opt);
		else	res = 1;
	}
	else if(!strcmp(k,"dddss"))
		*d = gr->FitS(*(a[1].d), *(a[2].d), a[3].s.c_str(), a[4].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_putsfit(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;	gr->Self()->SaveState(opt);
	if(!strcmp(k,"nn"))		gr->PutsFit(mglPoint(a[0].v,a[1].v));
	else if(!strcmp(k,"nns"))	gr->PutsFit(mglPoint(a[0].v,a[1].v), a[2].s.c_str());
	else if(!strcmp(k,"nnss"))	gr->PutsFit(mglPoint(a[0].v,a[1].v), a[2].s.c_str(),a[3].s.c_str());
	else if(!strcmp(k,"nnssn"))	gr->PutsFit(mglPoint(a[0].v,a[1].v), a[2].s.c_str(),a[3].s.c_str(),a[4].v);
	else if(!strcmp(k,"nnn"))	gr->PutsFit(mglPoint(a[0].v,a[1].v,a[2].v));
	else if(!strcmp(k,"nnns"))	gr->PutsFit(mglPoint(a[0].v,a[1].v,a[2].v), a[3].s.c_str());
	else if(!strcmp(k,"nnnss"))	gr->PutsFit(mglPoint(a[0].v,a[1].v,a[2].v), a[3].s.c_str(),a[4].s.c_str());
	else if(!strcmp(k,"nnnssn"))gr->PutsFit(mglPoint(a[0].v,a[1].v,a[2].v), a[3].s.c_str(),a[4].s.c_str(),a[5].v);
	else res = 1;	gr->Self()->LoadState();	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_arrowsize(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"n"))	gr->SetArrowSize(a[0].v);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_rearrange(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d && !strcmp(k,"dn"))	d->Rearrange(mgl_int(a[1].v));
	else if(d && !strcmp(k,"dnn"))	d->Rearrange(mgl_int(a[1].v), mgl_int(a[2].v));
	else if(d && !strcmp(k,"dnnn"))	d->Rearrange(mgl_int(a[1].v), mgl_int(a[2].v), mgl_int(a[3].v));
	else if(c && !strcmp(k,"dn"))	c->Rearrange(mgl_int(a[1].v));
	else if(c && !strcmp(k,"dnn"))	c->Rearrange(mgl_int(a[1].v), mgl_int(a[2].v));
	else if(c && !strcmp(k,"dnnn"))	c->Rearrange(mgl_int(a[1].v), mgl_int(a[2].v), mgl_int(a[3].v));
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_fplot(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"s"))	gr->FPlot(a[0].s.c_str(), "",opt);
	else if(!strcmp(k,"ss"))	gr->FPlot(a[0].s.c_str(), a[1].s.c_str(),opt);
	else if(!strcmp(k,"sss"))	gr->FPlot(a[0].s.c_str(), a[1].s.c_str(), a[2].s.c_str(), "",opt);
	else if(!strcmp(k,"ssss"))	gr->FPlot(a[0].s.c_str(), a[1].s.c_str(), a[2].s.c_str(), a[3].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_fsurf(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"s"))	gr->FSurf(a[0].s.c_str(), "",opt);
	else if(!strcmp(k,"ss"))	gr->FSurf(a[0].s.c_str(), a[1].s.c_str(),opt);
	else if(!strcmp(k,"sss"))	gr->FSurf(a[0].s.c_str(), a[1].s.c_str(), a[2].s.c_str(), "",opt);
	else if(!strcmp(k,"ssss"))	gr->FSurf(a[0].s.c_str(), a[1].s.c_str(), a[2].s.c_str(), a[3].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_fgets(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;	gr->Self()->SaveState(opt);
	char *buf;	buf = new char[4096];
	FILE *fp;
	if(!strncmp(k,"nns",3))
	{
		int i, n = (k[3]=='n'?mgl_int(a[3].v):0);
		fp = fopen(a[2].s.c_str(),"rt");
		if(!fp)
		{
			gr->SetWarn(mglWarnOpen,a[2].s.c_str());
			delete []buf;	return res;
}
		for(i=0;i<n;i++)	if(!fgets(buf,4096,fp))	continue;
		memset(buf,0,4096);
		if(!fgets(buf,4096,fp))
		{
			char b[32];	snprintf(b,32,"%d",n);	b[31]=0;
			gr->SetWarn(mglWarnOpen,(a[2].s+" - line "+b).c_str());
			fclose(fp);	delete []buf;	return res;
		}
		fclose(fp);
		gr->Puts(mglPoint(a[0].v,a[1].v,NAN),buf, (k[4]=='s')?a[4].s.c_str():"", k[5]=='n'?a[5].v:-1);
	}
	else if(!strncmp(k,"nnns",4))
	{
		int i, n = (k[4]=='n'?mgl_int(a[4].v):0);
		fp = fopen(a[3].s.c_str(),"rt");
		if(!fp)
		{
			gr->SetWarn(mglWarnOpen,a[3].s.c_str());
			delete []buf;	return res;
		}
		for(i=0;i<n;i++)	if(!fgets(buf,4096,fp))	continue;
		memset(buf,0,4096);
		if(!fgets(buf,4096,fp))
		{
			char b[32];	snprintf(b,32,"%d",n);	b[31]=0;
			gr->SetWarn(mglWarnOpen,(a[3].s+" - line "+b).c_str());
			fclose(fp);	delete []buf;	return res;
		}
		fclose(fp);
		gr->Puts(mglPoint(a[0].v,a[1].v,a[2].v),buf, (k[5]=='s')?a[5].s.c_str():"", k[6]=='n'?a[6].v:-1);
	}
	else res = 1;	gr->Self()->LoadState();	delete []buf;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_scanfile(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"dss"))
	{
		mglData *d = dynamic_cast<mglData *>(a[0].d);
		if(!d)	return 1;
		d->ScanFile(a[1].s.c_str(), a[2].s.c_str());
	}
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_import(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	if(!d)	return 1;
	if(!strcmp(k,"dss"))	d->Import(a[1].s.c_str(), a[2].s.c_str());
	else if(!strcmp(k,"dssnn"))	d->Import(a[1].s.c_str(), a[2].s.c_str(), a[3].v,a[4].v);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_export(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"dss"))	a[0].d->Export(a[1].s.c_str(), a[2].s.c_str());
	else if(!strcmp(k,"dssnn"))	a[0].d->Export(a[1].s.c_str(), a[2].s.c_str(), a[3].v,a[4].v);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_write(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]==0)	gr->WriteFrame("", "MathGL");
	else if(!strcmp(k,"s"))	gr->WriteFrame(a[0].s.c_str(), "MathGL");
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_region(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"dd"))		gr->Region(*(a[0].d),*(a[1].d),"",opt);
	else if(!strcmp(k,"dds"))	gr->Region(*(a[0].d),*(a[1].d),a[2].s.c_str(),opt);
	else if(!strcmp(k,"ddd"))	gr->Region(*(a[0].d),*(a[1].d),*(a[2].d),"",opt);
	else if(!strcmp(k,"ddds"))	gr->Region(*(a[0].d),*(a[1].d),*(a[2].d),a[3].s.c_str(),opt);
	else if(!strcmp(k,"dddd"))	gr->Region(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),"",opt);
	else if(!strcmp(k,"dddds"))	gr->Region(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),a[4].s.c_str(),opt);
	else if(!strcmp(k,"dddddd"))	gr->Region(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),*(a[5].d),"",opt);
	else if(!strcmp(k,"dddddds"))	gr->Region(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d),*(a[4].d),*(a[5].d),a[6].s.c_str(),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_envelop(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	if(!d)	return 1;
	if(!strcmp(k,"d"))	d->Envelop();
	else if(!strcmp(k,"ds"))	d->Envelop(a[1].s.c_str()[0]);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_sew(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	if(!d)	return 1;
	if(!strcmp(k,"d"))	d->Sew();
	else if(!strcmp(k,"ds"))	d->Sew(a[1].s.c_str());
	else if(!strcmp(k,"dsn"))	d->Sew(a[1].s.c_str(), a[2].v);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_evaluate(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d && k[0]=='d' && k[1]=='d' && k[2]=='d')
	{
		if(k[3]==0)	*d = mglData(true,mgl_data_evaluate(a[1].d,a[2].d,0,0,true));
		else if(!strcmp(k+3,"n"))	*d = mglData(true,mgl_data_evaluate(a[1].d,a[2].d,0,0, a[3].v!=0));
		else if(!strcmp(k+3,"d"))	*d = mglData(true,mgl_data_evaluate(a[1].d,a[2].d,a[3].d,0,true));
		else if(!strcmp(k+3,"dn"))	*d = mglData(true,mgl_data_evaluate(a[1].d,a[2].d,a[3].d,0, a[4].v!=0));
		else if(!strcmp(k+3,"dd"))	*d = mglData(true,mgl_data_evaluate(a[1].d,a[2].d,a[3].d,a[4].d,true));
		else if(!strcmp(k+3,"ddn"))	*d = mglData(true,mgl_data_evaluate(a[1].d,a[2].d,a[3].d,a[4].d, a[5].v!=0));
		else res = 1;
	}
	else if(c && k[0]=='d' && k[1]=='d' && k[2]=='d')
	{
		if(k[3]==0)	*c = mglDataC(true,mgl_datac_evaluate(a[1].d,a[2].d,0,0,true));
		else if(!strcmp(k+3,"n"))	*c = mglDataC(true,mgl_datac_evaluate(a[1].d,a[2].d,0,0, a[3].v!=0));
		else if(!strcmp(k+3,"d"))	*c = mglDataC(true,mgl_datac_evaluate(a[1].d,a[2].d,a[3].d,0,true));
		else if(!strcmp(k+3,"dn"))	*c = mglDataC(true,mgl_datac_evaluate(a[1].d,a[2].d,a[3].d,0, a[4].v!=0));
		else if(!strcmp(k+3,"dd"))	*c = mglDataC(true,mgl_datac_evaluate(a[1].d,a[2].d,a[3].d,a[4].d,true));
		else if(!strcmp(k+3,"ddn"))	*c = mglDataC(true,mgl_datac_evaluate(a[1].d,a[2].d,a[3].d,a[4].d, a[5].v!=0));
		else res = 1;
	}
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_solve(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	if(!d)	return 1;
	if(!strcmp(k,"ddns"))	*d = mglData(true,mgl_data_solve(a[1].d, a[2].v, a[3].s[0], 0, true));
	else if(!strcmp(k,"ddnsn"))	*d = mglData(true,mgl_data_solve(a[1].d, a[2].v, a[3].s[0], 0, a[4].v!=0));
	else if(!strcmp(k,"ddnsd"))	*d = mglData(true,mgl_data_solve(a[1].d, a[2].v, a[3].s[0], a[4].d, true));
	else if(!strcmp(k,"ddnsdn"))*d = mglData(true,mgl_data_solve(a[1].d, a[2].v, a[3].s[0], a[4].d, a[5].v!=0));
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_put(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d)
	{
		if(!strcmp(k,"dn"))	d->Put(a[1].v);
		else if(!strcmp(k,"dnn"))	d->Put(a[1].v, mgl_int(a[2].v));
		else if(!strcmp(k,"dnnn"))	d->Put(a[1].v, mgl_int(a[2].v),mgl_int(a[3].v));
		else if(!strcmp(k,"dnnnn"))	d->Put(a[1].v, mgl_int(a[2].v),mgl_int(a[3].v),mgl_int(a[4].v));
		else if(!strcmp(k,"dd"))	d->Put(*(a[1].d));
		else if(!strcmp(k,"ddn"))	d->Put(*(a[1].d), mgl_int(a[2].v));
		else if(!strcmp(k,"ddnn"))	d->Put(*(a[1].d), mgl_int(a[2].v),mgl_int(a[3].v));
		else if(!strcmp(k,"ddnnn"))	d->Put(*(a[1].d), mgl_int(a[2].v),mgl_int(a[3].v),mgl_int(a[4].v));
		else res = 1;
	}
	else if(c)
	{
		if(!strcmp(k,"dn"))	c->Put(a[1].c);
		else if(!strcmp(k,"dnn"))	c->Put(a[1].c, mgl_int(a[2].v));
		else if(!strcmp(k,"dnnn"))	c->Put(a[1].c, mgl_int(a[2].v),mgl_int(a[3].v));
		else if(!strcmp(k,"dnnnn"))	c->Put(a[1].c, mgl_int(a[2].v),mgl_int(a[3].v),mgl_int(a[4].v));
		else if(!strcmp(k,"dd"))	c->Put(*(a[1].d));
		else if(!strcmp(k,"ddn"))	c->Put(*(a[1].d), mgl_int(a[2].v));
		else if(!strcmp(k,"ddnn"))	c->Put(*(a[1].d), mgl_int(a[2].v),mgl_int(a[3].v));
		else if(!strcmp(k,"ddnnn"))	c->Put(*(a[1].d), mgl_int(a[2].v),mgl_int(a[3].v),mgl_int(a[4].v));
		else res = 1;
	}
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_palette(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"s"))	gr->SetPalette(a[0].s.c_str());
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_combine(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d && !strcmp(k,"ddd"))	*d = mglData(true,mgl_data_combine(a[1].d, a[2].d));
	else if(c && !strcmp(k,"ddd"))	*c = mglDataC(true,mgl_datac_combine(a[1].d, a[2].d));
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_correl(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d && !strcmp(k,"ddds"))	*d = mglData(true,mgl_data_correl(a[1].d, a[2].d, a[3].s.c_str()));
	else if(c && !strcmp(k,"ddds"))	*c = mglDataC(true,mgl_datac_correl(a[1].d, a[2].d, a[3].s.c_str()));
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_roots(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	if(!d)	return 1;
	if(!strcmp(k,"dsds"))	*d = mglData(true,mgl_data_roots(a[1].s.c_str(), a[2].d, a[3].s[0]));
	else if(!strcmp(k,"dsns"))	d->a[0] = mgl_find_root_txt(a[1].s.c_str(), a[2].v, a[3].s[0]);
	else if(!strcmp(k,"dsd"))	*d = mglData(true,mgl_data_roots(a[1].s.c_str(), a[2].d, 'x'));
	else if(!strcmp(k,"dsn"))	d->a[0] = mgl_find_root_txt(a[1].s.c_str(), a[2].v, 'x');
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_ode(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	if(!d)	return 1;
	if(!strcmp(k,"dssd"))
		*d = mglODE(a[1].s.c_str(), a[2].s.c_str(), *(a[3].d));
	else if(!strcmp(k,"dssdnn"))
		*d = mglODE(a[1].s.c_str(), a[2].s.c_str(), *(a[3].d), a[4].v, a[5].v);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_tridmat(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(c && !strcmp(k,"ddddds"))
		*c = mglTridMatC(*(a[1].d), *(a[2].d), *(a[3].d), *(a[4].d), a[5].s.c_str());
	else if(d && !strcmp(k,"ddddds"))
		*d = mglTridMat(*(a[1].d), *(a[2].d), *(a[3].d), *(a[4].d), a[5].s.c_str());
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_diffract(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(c && !strcmp(k,"dsn"))	c->Diffraction(a[1].s.c_str(), a[2].v);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_pde(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	if(k[1]=='d' && a[1].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d), *f = dynamic_cast<mglData *>(a[1].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d && f)
	{
		mglDataC r;
		if(!strcmp(k,"ddsdd"))
			r = gr->PDEc(a[2].s.c_str(), *(a[3].d), *(a[4].d), 0.1,100,opt);
		else if(!strcmp(k,"ddsddn"))
			r = gr->PDEc(a[2].s.c_str(), *(a[3].d), *(a[4].d), a[5].v,100,opt);
		else if(!strcmp(k,"ddsddnn"))
			r = gr->PDEc(a[2].s.c_str(), *(a[3].d), *(a[4].d), a[5].v,a[6].v,opt);
		else res = 1;
		if(res==0)	{	*d = r.Abs();	*f = r.Arg();	}
	}
	else if(d)
	{
		if(!strcmp(k,"dsdd"))
			*d = gr->PDE(a[1].s.c_str(), *(a[2].d), *(a[3].d), 0.1,100,opt);
		else if(!strcmp(k,"dsddn"))
			*d = gr->PDE(a[1].s.c_str(), *(a[2].d), *(a[3].d), a[4].v,100,opt);
		else if(!strcmp(k,"dsddnn"))
			*d = gr->PDE(a[1].s.c_str(), *(a[2].d), *(a[3].d), a[4].v,a[5].v,opt);
		else res = 1;
	}
	else if(c)
	{
		if(!strcmp(k,"dsdd"))
			*c = gr->PDEc(a[1].s.c_str(), *(a[2].d), *(a[3].d), 0.1,100,opt);
		else if(!strcmp(k,"dsddn"))
			*c = gr->PDEc(a[1].s.c_str(), *(a[2].d), *(a[3].d), a[4].v,100,opt);
		else if(!strcmp(k,"dsddnn"))
			*c = gr->PDEc(a[1].s.c_str(), *(a[2].d), *(a[3].d), a[4].v,a[5].v,opt);
		else res = 1;
	}
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_pde_adv(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	if(k[1]=='d' && a[1].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d), *f = dynamic_cast<mglData *>(a[1].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d && f)
	{
		mglDataC r;
		if(!strcmp(k,"ddsdd"))
			r = gr->APDEc(a[2].s.c_str(), *(a[3].d), *(a[4].d), 0.1,100,opt);
		else if(!strcmp(k,"ddsddn"))
			r = gr->APDEc(a[2].s.c_str(), *(a[3].d), *(a[4].d), a[5].v,100,opt);
		else if(!strcmp(k,"ddsddnn"))
			r = gr->APDEc(a[2].s.c_str(), *(a[3].d), *(a[4].d), a[5].v,a[6].v,opt);
		else res = 1;
		if(res==0)	{	*d = r.Abs();	*f = r.Arg();	}
	}
	else if(d)
	{
		if(!strcmp(k,"dsdd"))
			*d = gr->APDE(a[1].s.c_str(), *(a[2].d), *(a[3].d), 0.1,100,opt);
		else if(!strcmp(k,"dsddn"))
			*d = gr->APDE(a[1].s.c_str(), *(a[2].d), *(a[3].d), a[4].v,100,opt);
		else if(!strcmp(k,"dsddnn"))
			*d = gr->APDE(a[1].s.c_str(), *(a[2].d), *(a[3].d), a[4].v,a[5].v,opt);
		else res = 1;
	}
	else if(c)
	{
		if(!strcmp(k,"dsdd"))
			*c = gr->APDEc(a[1].s.c_str(), *(a[2].d), *(a[3].d), 0.1,100,opt);
		else if(!strcmp(k,"dsddn"))
			*c = gr->APDEc(a[1].s.c_str(), *(a[2].d), *(a[3].d), a[4].v,100,opt);
		else if(!strcmp(k,"dsddnn"))
			*c = gr->APDEc(a[1].s.c_str(), *(a[2].d), *(a[3].d), a[4].v,a[5].v,opt);
		else res = 1;
	}
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_qo2d(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	if(k[1]=='d' && a[1].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d), *f = dynamic_cast<mglData *>(a[1].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d && f)
	{
		mglDataC r;
		if(!strcmp(k,"ddsddd"))
			r = mglDataC(true, mgl_qo2d_solve_c(a[2].s.c_str(), a[3].d, a[4].d, a[5].d, 1,100, 0,0));
		else if(!strcmp(k,"ddsdddn"))
			r = mglDataC(true, mgl_qo2d_solve_c(a[2].s.c_str(), a[3].d, a[4].d, a[5].d, a[6].v,100, 0,0));
		else if(!strcmp(k,"ddsdddnn"))
			r = mglDataC(true, mgl_qo2d_solve_c(a[2].s.c_str(), a[3].d, a[4].d, a[5].d, a[6].v,a[7].v, 0,0));
		else if(!strcmp(k,"ddsdddnndd"))
			r = mglDataC(true, mgl_qo2d_solve_c(a[2].s.c_str(), a[3].d, a[4].d, a[5].d, a[6].v,a[7].v, dynamic_cast<mglData *>(a[8].d),dynamic_cast<mglData *>(a[9].d)));
		else res = 1;
		if(res==0)	{	*d = r.Abs();	*f = r.Arg();	}
	}
	else if(d)
	{
		if(!strcmp(k,"dsddd"))
			*d = mglData(true, mgl_qo2d_solve(a[1].s.c_str(), a[2].d, a[3].d, a[4].d, 1,100, 0,0));
		else if(!strcmp(k,"dsdddn"))
			*d = mglData(true, mgl_qo2d_solve(a[1].s.c_str(), a[2].d, a[3].d, a[4].d, a[5].v,100, 0,0));
		else if(!strcmp(k,"dsdddnn"))
			*d = mglData(true, mgl_qo2d_solve(a[1].s.c_str(), a[2].d, a[3].d, a[4].d, a[5].v,a[6].v, 0,0));
		else if(!strcmp(k,"dsdddnndd"))
			*d = mglData(true, mgl_qo2d_solve(a[1].s.c_str(), a[2].d, a[3].d, a[4].d, a[5].v,a[6].v, dynamic_cast<mglData *>(a[7].d),dynamic_cast<mglData *>(a[8].d)));
		else res = 1;
	}
	else if(c)
	{
		if(!strcmp(k,"dsddd"))
			*c = mglDataC(true, mgl_qo2d_solve_c(a[1].s.c_str(), a[2].d, a[3].d, a[4].d, 1,100, 0,0));
		else if(!strcmp(k,"dsdddn"))
			*c = mglDataC(true, mgl_qo2d_solve_c(a[1].s.c_str(), a[2].d, a[3].d, a[4].d, a[5].v,100, 0,0));
		else if(!strcmp(k,"dsdddnn"))
			*c = mglDataC(true, mgl_qo2d_solve_c(a[1].s.c_str(), a[2].d, a[3].d, a[4].d, a[5].v,a[6].v, 0,0));
		else if(!strcmp(k,"dsdddnndd"))
			*c = mglDataC(true, mgl_qo2d_solve_c(a[1].s.c_str(), a[2].d, a[3].d, a[4].d, a[5].v,a[6].v, dynamic_cast<mglData *>(a[7].d),dynamic_cast<mglData *>(a[8].d)));
		else res = 1;
	}
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_qo3d(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	if(k[1]=='d' && a[1].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d), *f = dynamic_cast<mglData *>(a[1].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d && f)
	{
		mglDataC r;
		if(!strcmp(k,"ddsddd"))
			r = mglDataC(true, mgl_qo3d_solve_c(a[2].s.c_str(), a[3].d, a[4].d, a[5].d, 1,100, 0,0,0));
		else if(!strcmp(k,"ddsdddn"))
			r = mglDataC(true, mgl_qo3d_solve_c(a[2].s.c_str(), a[3].d, a[4].d, a[5].d, a[6].v,100, 0,0,0));
		else if(!strcmp(k,"ddsdddnn"))
			r = mglDataC(true, mgl_qo3d_solve_c(a[2].s.c_str(), a[3].d, a[4].d, a[5].d, a[6].v,a[7].v, 0,0,0));
		else if(!strcmp(k,"ddsdddnnddd"))
			r = mglDataC(true, mgl_qo3d_solve_c(a[2].s.c_str(), a[3].d, a[4].d, a[5].d, a[6].v,a[7].v, dynamic_cast<mglData *>(a[8].d),dynamic_cast<mglData *>(a[9].d),dynamic_cast<mglData *>(a[10].d)));
		else res = 1;
		if(res==0)	{	*d = r.Abs();	*f = r.Arg();	}
	}
	else if(d)
	{
		if(!strcmp(k,"dsddd"))
			*d = mglData(true, mgl_qo3d_solve(a[1].s.c_str(), a[2].d, a[3].d, a[4].d, 1,100, 0,0,0));
		else if(!strcmp(k,"dsdddn"))
			*d = mglData(true, mgl_qo3d_solve(a[1].s.c_str(), a[2].d, a[3].d, a[4].d, a[5].v,100, 0,0,0));
		else if(!strcmp(k,"dsdddnn"))
			*d = mglData(true, mgl_qo3d_solve(a[1].s.c_str(), a[2].d, a[3].d, a[4].d, a[5].v,a[6].v, 0,0,0));
		else if(!strcmp(k,"dsdddnnddd"))
			*d = mglData(true, mgl_qo3d_solve(a[1].s.c_str(), a[2].d, a[3].d, a[4].d, a[5].v,a[6].v, dynamic_cast<mglData *>(a[7].d),dynamic_cast<mglData *>(a[8].d),dynamic_cast<mglData *>(a[9].d)));
		else res = 1;
	}
	else if(c)
	{
		if(!strcmp(k,"dsddd"))
			*c = mglDataC(true, mgl_qo3d_solve_c(a[1].s.c_str(), a[2].d, a[3].d, a[4].d, 1,100, 0,0,0));
		else if(!strcmp(k,"dsdddn"))
			*c = mglDataC(true, mgl_qo3d_solve_c(a[1].s.c_str(), a[2].d, a[3].d, a[4].d, a[5].v,100, 0,0,0));
		else if(!strcmp(k,"dsdddnn"))
			*c = mglDataC(true, mgl_qo3d_solve_c(a[1].s.c_str(), a[2].d, a[3].d, a[4].d, a[5].v,a[6].v, 0,0,0));
		else if(!strcmp(k,"dsdddnnddd"))
			*c = mglDataC(true, mgl_qo3d_solve_c(a[1].s.c_str(), a[2].d, a[3].d, a[4].d, a[5].v,a[6].v, dynamic_cast<mglData *>(a[7].d),dynamic_cast<mglData *>(a[8].d),dynamic_cast<mglData *>(a[9].d)));
		else res = 1;
	}
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_ray(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	if(!d)	return 1;
	if(!strcmp(k,"dsnnnn"))
		*d = mglRay(a[1].s.c_str(), mglPoint(a[2].v, a[3].v), mglPoint(a[4].v, a[5].v));
	else if(!strcmp(k,"dsnnnnnn"))
		*d = mglRay(a[1].s.c_str(), mglPoint(a[2].v, a[3].v, a[4].v), mglPoint(a[5].v, a[6].v, a[7].v));
	else if(!strcmp(k,"dsnnnnnnn"))
		*d = mglRay(a[1].s.c_str(), mglPoint(a[2].v, a[3].v, a[4].v), mglPoint(a[5].v, a[6].v, a[7].v), a[8].v);
	else if(!strcmp(k,"dsnnnnnnnn"))
		*d = mglRay(a[1].s.c_str(), mglPoint(a[2].v, a[3].v, a[4].v), mglPoint(a[5].v, a[6].v, a[7].v), a[8].v,a[9].v);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_jacobian(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	if(!d)	return 1;
	if(!strcmp(k,"ddd"))	*d = mglJacobian(*(a[1].d), *(a[2].d));
	else if(!strcmp(k,"dddd"))	*d = mglJacobian(*(a[1].d), *(a[2].d), *(a[3].d));
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_tens(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(!strcmp(k,"dd"))	gr->Tens(*(a[0].d),*(a[1].d), "",opt);
	else if(!strcmp(k,"dds"))	gr->Tens(*(a[0].d),*(a[1].d), a[2].s.c_str(),opt);
	else if(!strcmp(k,"ddd"))	gr->Tens(*(a[0].d),*(a[1].d),*(a[2].d), "",opt);
	else if(!strcmp(k,"ddds"))	gr->Tens(*(a[0].d),*(a[1].d),*(a[2].d), a[3].s.c_str(),opt);
	else if(!strcmp(k,"dddd")) 	gr->Tens(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d), "",opt);
	else if(!strcmp(k,"dddds"))	gr->Tens(*(a[0].d),*(a[1].d),*(a[2].d),*(a[3].d), a[4].s.c_str(),opt);
	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_ticklen(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"n"))	gr->SetTickLen(a[0].v);
	else if(!strcmp(k,"nn"))	gr->SetTickLen(a[0].v, a[1].v);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_tickshift(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"n"))	gr->SetTickShift(mglPoint(a[0].v));
	else if(!strcmp(k,"nn"))	gr->SetTickShift(mglPoint(a[0].v, a[1].v));
	else if(!strcmp(k,"nnn"))	gr->SetTickShift(mglPoint(a[0].v, a[1].v, a[2].v));
	else if(!strcmp(k,"nnnn"))	gr->SetTickShift(mglPoint(a[0].v, a[1].v, a[2].v, a[3].v));
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_origintick(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"n"))	gr->SetOriginTick(a[0].v);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_axisstl(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]==0)	gr->SetAxisStl();
	else if(!strcmp(k,"s"))	gr->SetAxisStl(a[0].s.c_str());
	else if(!strcmp(k,"ss"))	gr->SetAxisStl(a[0].s.c_str(), a[1].s.c_str());
	else if(!strcmp(k,"sss"))	gr->SetAxisStl(a[0].s.c_str(), a[1].s.c_str(), a[2].s.c_str());
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_ranges(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"nnnn"))	gr->SetRanges(a[0].v,a[1].v, a[2].v,a[3].v);
	else if(!strcmp(k,"nnnnnn"))gr->SetRanges(a[0].v,a[1].v,a[2].v, a[3].v,a[4].v,a[5].v);
	else if(!strcmp(k,"dd"))	gr->SetRanges(*(a[0].d),*(a[1].d));
	else if(!strcmp(k,"ddd"))	gr->SetRanges(*(a[0].d),*(a[1].d), *(a[2].d));
	else if(!strcmp(k,"dddd"))	gr->SetRanges(*(a[0].d),*(a[1].d), *(a[2].d),*(a[3].d));
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_adjust(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]==0)	gr->Adjust();
	else if(!strcmp(k,"s"))	gr->Adjust(a[0].s.c_str());
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_insert(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d && !strcmp(k,"ds"))	d->Insert(a[1].s.c_str()[0]);
	else if(d && !strcmp(k,"dsn"))	d->Insert(a[1].s.c_str()[0], mgl_int(a[2].v));
	else if(d && !strcmp(k,"dsnn"))	d->Insert(a[1].s.c_str()[0], mgl_int(a[2].v), mgl_int(a[3].v));
	else if(c && !strcmp(k,"ds"))	c->Insert(a[1].s.c_str()[0]);
	else if(c && !strcmp(k,"dsn"))	c->Insert(a[1].s.c_str()[0], mgl_int(a[2].v));
	else if(c && !strcmp(k,"dsnn"))	c->Insert(a[1].s.c_str()[0], mgl_int(a[2].v), mgl_int(a[3].v));
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_delete(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d && !strcmp(k,"ds"))	d->Delete(a[1].s.c_str()[0]);
	else if(d && !strcmp(k,"dsn"))	d->Delete(a[1].s.c_str()[0], mgl_int(a[2].v));
	else if(d && !strcmp(k,"dsnn"))	d->Delete(a[1].s.c_str()[0], mgl_int(a[2].v), mgl_int(a[3].v));
	else if(c && !strcmp(k,"ds"))	c->Delete(a[1].s.c_str()[0]);
	else if(c && !strcmp(k,"dsn"))	c->Delete(a[1].s.c_str()[0], mgl_int(a[2].v));
	else if(c && !strcmp(k,"dsnn"))	c->Delete(a[1].s.c_str()[0], mgl_int(a[2].v), mgl_int(a[3].v));
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_roll(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	mglDataC *c = dynamic_cast<mglDataC *>(a[0].d);
	if(d && !strcmp(k,"dsn"))	d->Roll(a[1].s.c_str()[0], mgl_int(a[2].v));
	else if(c && !strcmp(k,"dsn"))	c->Roll(a[1].s.c_str()[0], mgl_int(a[2].v));
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_datagrid(mglGraph *gr, long , mglArg *a, const char *k, const char *opt)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	if(!strcmp(k,"dddd") && d)	gr->DataGrid(*d, *(a[1].d), *(a[2].d), *(a[3].d),opt);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_triangulate(mglGraph *, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]=='d' && a[0].d->temp)	return 5;
	mglData *d = dynamic_cast<mglData *>(a[0].d);
	if(!d)	return 1;
	if(!strcmp(k,"ddd"))		*d = mglTriangulation(*(a[1].d), *(a[2].d));
	else if(!strcmp(k,"dddd"))	*d = mglTriangulation(*(a[1].d), *(a[2].d), *(a[3].d));
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_view(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"nn"))		gr->View(a[0].v, a[1].v);
	else if(!strcmp(k,"nnn"))	gr->View(a[0].v, a[1].v, a[2].v);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_zoom(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"nnnn"))	gr->Zoom(a[0].v, a[1].v, a[2].v, a[3].v);
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_zoomaxis(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(!strcmp(k,"nn"))	gr->ZoomAxis(mglPoint(a[0].v), mglPoint(a[1].v));
	else if(!strcmp(k,"nnnn"))	gr->ZoomAxis(mglPoint(a[0].v, a[1].v), mglPoint(a[2].v, a[3].v));
	else if(!strcmp(k,"nnnnnn"))	gr->ZoomAxis(mglPoint(a[0].v, a[1].v, a[2].v), mglPoint(a[3].v, a[4].v, a[5].v));
	else if(!strcmp(k,"nnnnnnnn"))	gr->ZoomAxis(mglPoint(a[0].v, a[1].v, a[2].v, a[3].v), mglPoint(a[4].v, a[5].v, a[6].v, a[7].v));
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_drawreg(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	if(k[0]==0)	gr->SetDrawReg();
	else if(!strcmp(k,"nnn"))	gr->SetDrawReg(mgl_int(a[0].v), mgl_int(a[1].v), mgl_int(a[2].v));
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int MGL_NO_EXPORT mgls_version(mglGraph *gr, long , mglArg *a, const char *k, const char *)
{
	int res=0;
	char buf[32];	sprintf(buf,"MathGL version is 2.%g",MGL_VER2);
	if(k[0]==0)	gr->SetWarn(-1,buf);
	else if(!strcmp(k,"s"))	res = mgl_check_version(a[0].s.c_str())?1:0;
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int mgls_flame2d(mglGraph *, long, mglArg *a, const char *k, const char *)
{
	mglData *fx = dynamic_cast<mglData*>(a[0].d);
	mglData *fy = dynamic_cast<mglData*>(a[1].d);
	if(!fx)	return 1;
	int res = 0;
	if (!strcmp(k, "dddn"))	fx->Set(mglFlame2d(*(a[1].d), *(a[2].d), mgl_int(a[3].v)));
	else if (!strcmp(k, "ddddn") && fy)
	{
		mglData f(mglFlame2d(*(a[2].d), *(a[3].d), mgl_int(a[4].v)));
		fx->Set(f.SubData(0));	fy->Set(f.SubData(1));
	}
	else if (!strcmp(k, "dddnn"))	fx->Set(mglFlame2d(*(a[1].d), *(a[2].d), mgl_int(a[3].v), mgl_int(a[4].v)));
	else if (!strcmp(k, "ddddnn") && fy)
	{
		mglData f(mglFlame2d(*(a[2].d), *(a[3].d), mgl_int(a[4].v), mgl_int(a[5].v)));
		fx->Set(f.SubData(0));	fy->Set(f.SubData(1));
	}
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int mgls_ifs2d(mglGraph *, long, mglArg *a, const char *k, const char *)
{
	mglData *fx = dynamic_cast<mglData*>(a[0].d);
	mglData *fy = dynamic_cast<mglData*>(a[1].d);
	if(!fx)	return 1;
	int res = 0;
	if (!strcmp(k, "ddn"))	fx->Set(mglIFS2d(*(a[1].d), mgl_int(a[2].v)));
	else if (!strcmp(k, "dddn") && fy)
	{
		mglData f(mglIFS2d(*(a[2].d), mgl_int(a[3].v)));
		fx->Set(f.SubData(0));	fy->Set(f.SubData(1));
	}
	else if (!strcmp(k, "ddnn"))	fx->Set(mglIFS2d(*(a[1].d), mgl_int(a[2].v), mgl_int(a[3].v)));
	else if (!strcmp(k, "dddnn") && fy)
	{
		mglData f(mglIFS2d(*(a[2].d), mgl_int(a[3].v), mgl_int(a[4].v)));
		fx->Set(f.SubData(0));	fy->Set(f.SubData(1));
	}
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int mgls_ifs3d(mglGraph *, long, mglArg *a, const char *k, const char *)
{
	mglData *f = dynamic_cast<mglData*>(a[0].d);
	if(!f)	return 1;
	int res = 0;
	if (!strcmp(k, "ddn"))	f->Set(mglIFS3d(*(a[1].d), mgl_int(a[2].v)));
	else if (!strcmp(k, "ddnn"))	f->Set(mglIFS3d(*(a[1].d), mgl_int(a[2].v), mgl_int(a[3].v)));
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
int mgls_ifsfile(mglGraph *, long, mglArg *a, const char *k, const char *)
{
	mglData *f = dynamic_cast<mglData*>(a[0].d);
	if(!f)	return 1;
	int res = 0;
	if (!strcmp(k, "dssn"))	f->Set(mglIFSfile(a[1].s.c_str(), a[2].s.c_str(), mgl_int(a[3].v)));
	else if (!strcmp(k, "dssnn"))	f->Set(mglIFSfile(a[1].s.c_str(), a[2].s.c_str(), mgl_int(a[3].v), mgl_int(a[4].v)));
	else res = 1;	return res;
}
//-----------------------------------------------------------------------------
mglCommand mgls_base_cmd[] = {
	{"addlegend","Add legend entry","addlegend 'txt' 'fmt'", mgls_addlegend,15},
	{"addto","Add data or number","addto Var Dat|Var num", mgls_addto ,3},
	{"adjust","Adjust ticks for best view","adjust ['dir']", mgls_adjust ,14},
	{"alpha","Switch on/off transparency","alpha [val]", mgls_alpha ,2},
	{"alphadef","Set default transparency","alphadef val", mgls_alphadef ,2},
	{"ambient","Set ambient light brightness","ambient val", mgls_ambient ,2},
	{"apde","Solve PDE using advanced method (X-Y only)","apde Res 'ham' IniRe IniIm [dz k0]", mgls_pde_adv ,4},
	{"arc","Draw angle arc","arc x0 y0 x1 y1 a ['fmt']|x0 y0 z0 x1 y1 a ['fmt']|x0 y0 z0 xr yr zr x1 y1 z1 a ['fmt']", mgls_arc ,13},
	{"area","Draw area plot for 1D data","area Ydat ['fmt']|Xdat Ydat ['fmt']|Xdat Ydat Zdat ['fmt']", mgls_area ,7},
	{"arrowsize","Set size of arrows","arrowsize val", mgls_arrowsize ,2},
	{"ask","Define parameter from user input","ask $N 'question'", 0, 6},
	{"aspect","Set aspect ration","aspect valx valy [valz]", mgls_aspect ,5},
	{"attachlight","Attach light settings to inplot","attachlight val", mgls_attachlight ,2},
	{"axial","Draw surfaces of contour lines rotation","axial Zdat ['fmt' num]|Xdat Ydat Zdat ['fmt' num]", mgls_axial ,8},
	{"axis","Setup or draw axis","axis ['dir' 'fmt']|'fx' 'fy' 'fz' ['fc']|how", mgls_axis ,12},
	{"axisstl","Set axis and tick style","axisstl 'stl' ['sub']", mgls_axisstl ,14},
	{"background","Load image for background","background 'fname'", mgls_background ,12},
	{"ball","Draw point (ball)","ball posx posy ['fmt']|posx posy posz ['fmt']", mgls_ball ,13},
	{"barh","Draw horizontal bars for 1D data", "barh Ydat ['fmt' above]|Xdat Ydat ['fmt' above]", mgls_barh ,7},
	{"bars","Draw bars for 1D data","bars Ydat ['fmt' above]|Xdat Ydat ['fmt' above]|Xdat Ydat Zdat ['fmt' above]", mgls_bars ,7},
	{"barwidth","Set default bars width","barwidth val", mgls_barwidth ,2},
	{"beam","Draw quasi-optical beam","beam Tr G1 G2 Adat r ['sch' flag num] ", mgls_beam ,9},
	{"belt","Draw belts","belt Zdat ['fmt']|Xdat Ydat Zdat ['fmt']", mgls_belt ,8},
	{"bifurcation","Draw Bifurcation diagram","bifurcation dx Func ['fmt']|dx 'func' ['fmt']", mgls_bifurcation,13},
	{"box","Draw bounding box","box ['fmt' ticks]", mgls_box ,12},
	{"boxplot","Draw boxplot for 2D data","boxplot Ydat ['fmt']|Xdat Ydat ['fmt']", mgls_boxplot ,7},
	{"boxs","Draw boxes","boxs Zdat ['fmt']|Xdat Ydat Zdat ['fmt']", mgls_boxs ,8},
	{"break","Break for-cycle","break", 0, 6},
	{"call","Execute script in external file","call 'name' [args]", 0, 6},
	{"candle","Draw candlestick chart","candle candle Vdat1 ['fmt']|Vdat1 Vdat2 ['fmt']|Vdat1 Ydat1 Ydat2 ['fmt']||Vdat1 Vdat2 Ydat1 Ydat2 ['fmt']|Xdat Vdat1 Vdat2 Ydat1 Ydat2 ['fmt']", mgls_candle ,7},
	{"chart","Draw chart","chart Dat ['fmt']", mgls_chart ,7},
	{"chdir","Change current directory","chdir 'path'", mgls_chdir ,2},
	{"circle","Draw circle","circle x y r ['fmt']|x y z r ['fmt']", mgls_circle ,13},
	{"clean","Remove duplicate rows","clean Dat id", mgls_clean ,3},
	{"clearlegend","Clear legend entries","clearlegend", mgls_clearlegend ,15},
	{"clf","Clear picture","clf|'col'|r g b", mgls_clf ,12},
	{"cloud","Draw cloud","cloud Adat ['fmt']|Xdat Ydat Zdat Adat ['fmt']", mgls_cloud ,9},
	{"colorbar","Draw colorbar","colorbar ['fmt' pos]|Vdat ['fmt' pos]|'sch' pos x y [w h]|Vdat 'sch' pos x y [w h]", mgls_colorbar ,12},
	{"column","Get data column filled by formula on column ids","column Res Dat 'eq'", mgls_column ,4},
	{"columnplot","Set position of plot inside cell of column", "columnplot num ind [d]", mgls_columnplot ,5},
	{"combine", "Direct multiplication of arrays", "combine Res Adat Bdat", mgls_combine ,4},
	{"cone","Draw cone","cone x1 y1 z1 x2 y2 z2 r1 [r2 'fmt' edge]", mgls_cone ,13},
	{"cones","Draw cones for 1D data","cones Ydat ['fmt' above]|Xdat Ydat ['fmt' above]|Xdat Ydat Zdat ['fmt' above]", mgls_cones ,7},
	{"cont","Draw contour lines","cont Zdat ['fmt' num zpos]|Vdat Zdat ['fmt' zpos]|Xdat Ydat Zdat ['fmt' num zpos]|Vdat Xdat Ydat Zdat ['fmt' zpos]", mgls_cont ,8},
	{"cont3","Draw contour lines for 3D data","cont3 Adat 'dir' [val 'fmt' num]|Vdat Adat 'dir' [val 'fmt']|Xdat Ydat Zdat Adat 'dir' [val 'fmt' num]|Vdat Xdat Ydat Zdar Adat 'dir' [val 'fmt']", mgls_cont3 ,9},
	{"contd","Draw solid contours with manual colors","contd Zdat ['fmt' num zpos]|Vdat Zdat ['fmt' zpos]|Xdat Ydat Zdat ['fmt' num zpos]|Vdat Xdat Ydat Zdat ['fmt' zpos]", mgls_contd ,8},
	{"contf","Draw solid contours","contf Zdat ['fmt' num zpos]|Vdat Zdat ['fmt' zpos]|Xdat Ydat Zdat ['fmt' num zpos]|Vdat Xdat Ydat Zdat ['fmt' zpos]", mgls_contf ,8},
	{"contf3","Draw solid contour lines for 3D data","contf3 Adat 'dir' [val 'fmt' num]|Vdat Adat 'dir' [val 'fmt']|Xdat Ydat Zdat Adat 'dir' [val 'fmt' num]|Vdat Xdat Ydat Zdar Adat 'dir' [val 'fmt']", mgls_contf3 ,9},
	{"contfx","Draw solid contour lines at x-slice (or x-plane)","contfx Dat ['fmt' pos num]", mgls_contfx ,0},
	{"contfy","Draw solid contour lines at y-slice (or y-plane)","contfy Dat ['fmt' pos num]", mgls_contfy ,0},
	{"contfz","Draw solid contour lines at z-slice (or z-plane)","contfz Dat ['fmt' pos num]", mgls_contfz ,0},
	{"continue","Skip commands and iterate for-cycle again","continue", 0, 6},
	{"contv","Draw contour tubes","contv Zdat ['fmt' num zpos]|Vdat Zdat ['fmt' zpos]|Xdat Ydat Zdat ['fmt' num zpos]|Vdat Xdat Ydat Zdat ['fmt' zpos]", mgls_contv ,0},
	{"contx","Draw contour lines at x-slice (or x-plane)","contx Dat ['fmt' pos num]", mgls_contx ,0},
	{"conty","Draw contour lines at y-slice (or y-plane)","conty Dat ['fmt' pos num]", mgls_conty ,0},
	{"contz","Draw contour lines at z-slice (or z-plane)","contz Dat ['fmt' pos num]", mgls_contz ,0},
	{"copy","Copy data from another variable","copy Dat1 Dat2 ['eq' onaxis]", mgls_copy ,4},
	{"correl", "Find correlation between data arrays", "correl Res Adat Bdat 'dir'", mgls_correl ,4},
	{"cosfft","Cos-Fourier transform at some direction","cosfft Dat 'dir'", mgls_cosfft ,16},
	{"crange","Set color range","crange Dat [add] | c1 c2 [add]", mgls_crange ,14},
	{"crop","Crop edge of data","crop Dat n1 n2 'dir'", mgls_crop ,16},
	{"crust","Draw reconstructed surface for arbitrary data points","crust Xdat Ydat Zdat ['fmt']", mgls_crust ,0},
	{"ctick","Set ticks for colorbar","ctick 'tmpl' | dc ['factor']", mgls_ctick ,14},
	{"cumsum","Cumulative summation","cumsum Dat 'dir'", mgls_cumsum ,16},
	{"curve","Draw curve","curve x1 y1 dx1 dy1 x2 y2 dx2 dy2 ['fmt']|x1 y1 z1 dx1 dy1 dz1 x2 y2 z2 dx2 dy2 dz2 ['fmt']", mgls_curve ,13},
	{"cut","Setup plot points cutting","cut val|x1 y1 z1 x2 y2 z2|'cond'", mgls_cut ,2},
	{"datagrid","Fill data by triangulated values","datagrid Var Xdat Ydat Zdat", mgls_datagrid ,3},
	{"datas","Print list of data names in HDF file","datas 'fname'", mgls_datas ,3},
	{"defchr","Define parameter as character","defchr $N val", 0, 6},
	{"define","Define constant or parameter","define $N sth | Var val", 0, 6},
	{"defnum","Define parameter as numerical value","defnum $N val", 0, 6},
//	{"defpal","Define parameter as palette color","defpal $N val", 0, 6},
	{"delete","Delete slice of data","delete Dat 'dir' [pos=0 num=1]", mgls_delete ,3},
	{"dens","Draw density plot","dens Zdat ['fmt' zpos]|Xdat Ydat Zdat ['fmt' zpos]", mgls_dens ,8},
	{"dens3","Draw density plot at slices of 3D data","dens3 Adat 'dir' [pos 'fmt']|Xdat Ydat Zdat Adat 'dir' [pos 'fmt']", mgls_dens3 ,9},
	{"densx","Draw density plot at x-slice (or x-plane)","densx Dat ['fmt' pos]", mgls_densx ,0},
	{"densy","Draw density plot at y-slice (or y-plane)","densy Dat ['fmt' pos]", mgls_densy ,0},
	{"densz","Draw density plot at z-slice (or z-plane)","densz Dat ['fmt' pos]", mgls_densz ,0},
	{"dew","Draw dew plot","dew Udat Vdat ['fmt']|Xdat Ydat Udat Vdat ['fmt']", mgls_dew ,11},
	{"diff","Numerically differentiate data","diff Var 'dir'", mgls_diff ,16},
	{"diff2","Numerically double differentiate data","diff2 Var 'dir'", mgls_diff2 ,16},
	{"diffract","Step for pulse diffraction","diffract Res 'how' q", mgls_diffract ,16},
	{"diffuse","Set diffusive light brightness","diffuse val", mgls_diffuse ,2},
	{"divto","Divide by data or number","divto Var Dat|Var num", mgls_divto ,3},
	{"dots","Draw dots for arbitrary data points","dots Xdat Ydat Zdat ['fmt']|Xdat Ydat Zdat Adat ['fmt']|Xdat Ydat Zdat Cdat Adat ['fmt']", mgls_dots ,9},
	{"drawreg","Set draw region for quality&4","drawreg|nx ny m", mgls_drawreg ,2},
	{"drop","Draw drop","drop x0 y0 dx dy r ['col' sh asp]|x0 y0 z0 dx dy dz r ['col' sh asp]", mgls_drop ,13},
	{"echo","Print content of the data","echo Dat", mgls_echo ,3},
	{"ellipse","Draw ellipse","ellipse x1 y1 x2 y2 r ['fmt']|x1 y1 z1 x2 y2 z2 r ['fmt']", mgls_ellipse ,13},
	{"else","Execute if condition is false","else", 0, 6},
	{"elseif","Conditional operator","elseif val|Dat ['cond']", 0, 6},
	{"endif","Finish if/else block","endif", 0, 6},
	{"envelop","Find envelop for the data","envelop Dat ['dir']", mgls_envelop ,16},
	{"errbox","Draw error box","errbox x y ex ey ['fmt']|x y z ex ey ez ['fmt']", mgls_errbox ,13},
	{"error","Draw error boxes","error Ydat Yerr ['fmt']|Xdat Ydat Yerr ['fmt']|Xdat Ydat Xerr Yerr ['fmt']", mgls_error ,7},
	{"evaluate","Evaluate (interpolate) values of array Dat at points i=idat,j=jdat,k=kdat","evaluate Res Dat Idat [norm]|Res Dat Idat Jdat [norm]|Res Dat Idat Jdat Kdat [norm]", mgls_evaluate ,4},
	{"export","Export data to PNG picture","export Dat 'fname' 'sch' [v1 v2]", mgls_export ,3},
	{"extend","Extend data array","extend Dat dim1 [dim2]", mgls_extend ,3},
	{"face","Draw face (quadrangle)","face x1 y1 x2 y2 x3 y3 x4 y4 ['fmt']|x1 y1 z1 x2 y2 z2 x3 y3 z3 x4 y4 z4 ['fmt']", mgls_face ,13},
	{"facenum","Set number of visible faces","facenum val", mgls_facenum ,2},
	{"facex","Draw face perpendicular to x-axis","facex x0 y0 z0 wy wz ['fmt' d1 d2]", mgls_facex ,13},
	{"facey","Draw face perpendicular to y-axis","facex x0 y0 z0 wx wz ['fmt' d1 d2]", mgls_facey ,13},
	{"facez","Draw face perpendicular to z-axis","facex x0 y0 z0 wy wz ['fmt' d1 d2]", mgls_facez ,13},
	{"fall","Draw waterfalls","fall Zdat ['fmt']|Xdat Ydat Zdat ['fmt']", mgls_fall ,8},
	{"fgets","Print string from file","fgets x y z 'fname' [pos=0 'fmt' size]|x y z 'fname' [pos=0 'fmt' size]", mgls_fgets ,15},
	{"fill","Fill data linearly in range [v1, v2]","fill Var v1 v2 ['dir'] | Var 'eq' [Vdat Wdat]", mgls_fill ,3},
	{"fillsample","Fill x-,k-samples for transforms","fillsample Var 'how'", mgls_fillsample ,3},
	{"fit","Fit data to formula","fit Res A 'eq' 'var' [Ini]|Res X A 'eq' 'var' [Ini]|Res X Y A 'eq' 'var' [Ini]|Res X Y Z A 'eq' 'var' [Ini]", mgls_fit ,4},
	{"fits","Fit data to formula","fits Res A S 'eq' 'var' [Ini]|Res X A S 'eq' 'var' [Ini]|Res X Y A S 'eq' 'var' [Ini]|Res X Y Z A S 'eq' 'var' [Ini]", mgls_fits ,4},
	{"flame2d", "Computes the flame fractal", "flame2d F A B n [skip]|Fx Fy A B n [skip]", mgls_flame2d, 4},
	{"flow","Draw flow threads for vector field","flow Udat Vdat ['fmt' num]|Xdat Ydat Udat Vdat ['fmt' num]|Udat Vdat Wdat ['fmt' num]|Xdat Ydat Zdat Udat Vdat ['fmt' num]|\
	x0 y0 Udat Vdat ['fmt']|x0 y0 Xdat Ydat Udat Vdat ['fmt']|x0 y0 z0 Udat Vdat Wdat ['fmt']|x0 y0 z0 Xdat Ydat Zdat Udat Vdat Wdat ['fmt']", mgls_flow ,11},
	{"fog","Switch on/off fog","fog val [pos]", mgls_fog ,2},
	{"font","Setup font","font 'fmt' [size]", mgls_font ,15},
	{"for","For cycle","for $N v1 v2 [dv] | $N Dat", 0, 6},
	{"fourier","In-place Fourier transform","fourier ReDat ImDat 'dir'|Cmplx 'dir'", mgls_fourier , 16},
	{"fplot","Plot curve by formula","fplot 'y(x)' ['fmt']|'x(t)' 'y(t)' 'z(t)' ['fmt']", mgls_fplot ,1},
	{"fsurf","Plot surface by formula","fsurf 'z(x,y)' ['fmt']|'x(u,v)' 'y(u,v)' 'z(u,v)' ['fmt']", mgls_fsurf ,1},
	{"func","Start function definition and stop execution of main script","func 'name' [narg]", 0, 6},
	{"grad","Draw gradient lines for scalar field","grad Phi ['fmt' num]|Xdat Ydat Phi ['fmt' num]|Xdat Ydat Zdat Phi ['fmt' num]", mgls_grad ,8},
	{"gray","Switch on/off gray-scale mode","gray [val]", mgls_gray ,2},
	{"grid","Draw grid","grid ['dir' 'fmt']", mgls_grid ,12},
	{"grid2","Draw grid for data array(s)","grid Zdat ['fmt']|Xdat Ydat Zdat ['fmt']", mgls_grid2 ,8},
	{"grid3","Draw grid at slices of 3D data","grid3 Adat 'dir' [pos 'fmt']|Xdat Ydat Zdat Adat 'dir' [pos 'fmt']", mgls_grid3 ,9},
	{"gridplot","Set position of plot inside cell of matrix", "gridplot nx ny ind [d]", mgls_gridplot ,5},
	{"gspline","Fill data by global spline of Vdat","gspline Dat Xdat Vdat [sl]", mgls_gspline ,3},
	{"hankel","Hankel transform at some direction","hankel Dat 'dir'", mgls_hankel ,16},
	{"hist","Create histogram (distribution) of data values","hist Res Dat num v1 v2 [nsub]|Res Dat Wdat num v1 v2 [nsub]", mgls_hist ,4},
	{"idset","Set column id for data","idset Dat 'ids'", mgls_idset ,3},
	{"if","Conditional operator","if val|Dat ['cond']", 0, 6},
	{"ifs2d", "Computes the attractor of an IFS", "ifs2d F A n [skip]|Fx Fy A n [skip]", mgls_ifs2d, 4},
	{"ifs3d", "Computes the attractor of an IFS for 3d case", "ifs3d F A n [skip]", mgls_ifs3d, 4},
	{"ifsfile", "Computes the attractor of an IFS with parameters from *.ifs file", "ifsfile F 'fname' 'name' n [skip]", mgls_ifsfile, 4},
	{"import","Import data from PNG picture","import Dat 'fname' 'scheme' [v1 v2]", mgls_import ,4},
	{"info","Print message or information about the data","info Dat [detail]|'message'|const", mgls_info ,3},
	{"inplot","Set position of plot in picture","x1 x2 y1 y2 [rel]", mgls_inplot ,5},
	{"insert","Insert slice of data","insert Dat 'dir' [pos=0 num=1]", mgls_insert ,3},
	{"integrate","Integrate data","integrate Dat 'dir'", mgls_integrate ,16},
	{"jacobian","Get Jacobian","jacobian Res Xdat Ydat [Zdat]", mgls_jacobian ,4},
	{"join","Join data arrays","join Dat Add", mgls_join ,3},
	{"label","Draw label at arbitrary position","label Ydat 'txt' ['fmt'='']|Xdat Ydat 'txt' ['fmt'='']|Xdat Ydat Zdat 'txt' ['fmt'='']", mgls_label ,7},
	{"lamerey","Draw Lamerey diagram","lamerey x0 Func ['fmt']|x0 'func' ['fmt']", mgls_lamerey ,13},
	{"legend","Draw legend","legend [pos 'fmt']|x y ['fmt']", mgls_legend ,15},
	{"legendmarks","Set number of marks in the legend","legendmarks val", mgls_legendmarks ,15},
	{"light","Setup light","light [val] | val num | num xpos ypos zpos ['fmt' br]", mgls_light ,2},
	{"limit","Limit data to be inside [-v,v]","limit Dat v", mgls_limit ,16},
	{"line","Draw line","line x1 y1 x2 y2 ['fmt']|x1 y1 z1 x2 y2 z2 ['fmt']", mgls_line ,13},
	{"list","Creates new variable from list of numbers or data","list Var v1 ...|Var D1 ...", 0, 4},
	{"load","Load commands from external DLL","load 'fname'", 0, 6},
	{"loadfont","Load fontfaces","loadfont ['face']", mgls_loadfont ,15},
	{"logo","Draw bitmap (logo) along axis range","logo 'fname' [smooth]", mgls_logo ,13},
	{"map","Draw mapping plot","map Udat Vdat ['fmt']|Xdat Ydat Udat Vdat ['fmt']", mgls_map ,10},
	{"mark","Draw mark plot for 1D data","mark Ydat Rdat ['fmt']|Xdat Ydat Rdat ['fmt']|Xdat Ydat Zdat Rdat ['fmt']", mgls_mark ,7},
	{"marksize","Set size of markers","marksize val", mgls_marksize ,2},
	{"mask","Set brush for given mask","mask 'id' 'val'|'id' val|angle", mgls_mask ,2},
	{"max","Find maximal value over direction","max Res Dat 'dir'", mgls_max ,4},
	{"mesh","Draw mesh surface","mesh Zdat ['fmt']|Xdat Ydat Zdat ['fmt']", mgls_mesh ,8},
	{"meshnum","Set number of lines in mesh/fall/vect and so on","meshnum val", mgls_meshnum ,2},
	{"min","Find minimal value over direction","min Res Dat 'dir'", mgls_min ,4},
	{"mirror","Mirror data at some direction","mirror Dat 'dir'", mgls_mirror ,16},
	{"modify","Modify data values by formula","modify Dat 'eq' [num] | Dat 'eq' Vdat [Wdat]", mgls_modify ,3},
	{"momentum","Get momentum along direction","momentum Res Dat 'how' ['dir']", mgls_momentum ,4},
	{"multiplot","Set position of plot","multiplot m n pos dx dy 'style'", mgls_multiplot ,5},
	{"multo","Multiply by data or number","multo Var Dat|Var num", mgls_multo ,3},
	{"new","Create new data","new Dat nx ny nz ['eq']|new Dat nx ny ['eq']|new Dat nx ['eq']", mgls_new ,4},
	{"next","Start next for-cycle iteration","next", 0, 6},
	{"norm","Normalize data","norm Dat v1 v2 [sym dim]", mgls_norm ,16},
	{"normsl","Normalize data slice by slice","normsl Dat v1 v2 ['dir' keep sym] ", mgls_normsl ,16},
	{"ode","Solve ODE","ode Res 'df' 'var' Ini [dt tmax]", mgls_ode ,4},
	{"ohlc","Draw Open-High-Low-Close (OHLC) diagram","ohlc Odat Hdat Ldat Cdat ['fmt']|Xdat Odat Hdat Ldat Cdat ['fmt']", mgls_ohlc ,7},
	{"once","Start/close commands which should executed only once","once val", 0, 6},
	{"origin","Set axis origin","origin x0 y0 [z0]", mgls_origin ,14},
	{"origintick","Set tick labels drawing at origin","origintick val", mgls_origintick ,14},
	{"palette","Set palette for 1D plots","palette 'colors'", mgls_palette ,2},
	{"pde","Solve PDE","pde Res 'ham' IniRe IniIm [dz k0]", mgls_pde ,4},
	{"pendelta","Set size of semi-transparent area","pen_delta val", mgls_pendelta ,2},
	{"perspective","Set perspective","perspective val", mgls_perspective ,2},
	{"pipe","Draw flow pipes for vector field","pipe Udat Vdat ['fmt' rad num]|Xdat Ydat Udat Vdat ['fmt' rad num]|Udat Vdat Wdat ['fmt' rad num]|Xdat Ydat Zdat Udat Vdat Wdat ['fmt' rad num]", mgls_pipe ,11},
	{"plot","Draw usual plot for 1D data","plot Ydat ['fmt']|Xdat Ydat ['fmt']|Xdat Ydat Zdat ['fmt']", mgls_plot ,7},
	{"plotid","Set default filename","plotid 'name'", mgls_plotid ,2},
	{"pmap","Draw Poincare map","pmap Ydat Rdat ['fmt']|Xdat Ydat Rdat ['fmt']|Xdat Ydat Zdat Rdat ['fmt']", mgls_pmap ,7},
	{"polygon","Draw polygon","polygon x1 y1 x2 y2 num ['fmt']|x1 y1 z1 x2 y2 z2 num ['fmt']", mgls_polygon ,13},
	{"print","Immediately print the message","info 'message'|Dat [detail]|const", mgls_print ,3},
	{"pulse","Get pulse properties","pulse Res Dat 'dir'", mgls_pulse ,4},
	{"put","Put value (numeric or array) to given data element","put Dat val [i j k] | Dat Val [i j k]", mgls_put ,3},
	{"putsfit","Print fitted formula","putsfit x y ['pre' 'font' size]|x y z ['pre' 'font' size]", mgls_putsfit ,15},
	{"qo2d","Solve PDE in accompanied coordinates for 2d case","qo2d Res 'ham' IniRe IniIm Ray [r k0 Xout Yout]", mgls_qo2d ,4},
	{"qo3d","Solve PDE in accompanied coordinates for 3d case","qo3d Res 'ham' IniRe IniIm Ray [r k0 Xout Yout Zout]", mgls_qo3d ,4},
	{"quadplot","Draw surface of quadrangles","quadplot Idat Xdat Ydat ['fmt']|Idat Xdat Ydat Zdat ['fmt']|Idat Xdat Ydat Zdat Cdat ['fmt'] ", mgls_quadplot ,0},
	{"quality","Set plot quality","quality [val]", mgls_quality ,2},
	{"radar","Draw radar chart","radar Rdat ['fmt']", mgls_radar ,7},
	{"ranges","Set axis ranges","ranges x1 x2 y1 y2 [z1 z2]", mgls_ranges ,14},
	{"rasterize","Rasterize plot and save to background","rasterize", mgls_rasterize ,12},
	{"ray","Solve Hamiltonian ODE (find GO ray or trajectory)","ray Res 'ham' x0 y0 z0 px0 py0 pz0 [dz=0.1 tmax=10]", mgls_ray ,4},
	{"read","Read data from file","read Dat 'file' [nx ny nz] | ReDat ImDat 'file' [nx ny nz]", mgls_read ,4},
	{"readall","Read and join data from several files","readall Dat 'templ' [slice]", mgls_readall ,4},
	{"readhdf","Read data from HDF5 file","readhdf Dat 'file' 'id'", mgls_readhdf ,4},
	{"readmat","Read data from file with sizes specified in first row","readmat Dat 'file' [dim]", mgls_readmat ,4},
	{"rearrange","Rearrange data dimensions","rearrange Dat mx [my mz]", mgls_rearrange ,3},
	{"rect","Draw rectangle","rect x1 y1 x2 y2 ['fmt']|x1 y1 z1 x2 y2 z2 ['fmt']", mgls_rect ,13},
	{"refill","Fill data by interpolation of Vdat","refill Dat Xdat Vdat [sl] | Dat Xdat Ydat Vdat [sl] | Dat Xdat Ydat Zdat Vdat", mgls_refill ,3},
	{"region","Draw filled region (ribbon) between 2 curves","region Ydat1 Ydat2 ['fmt']|Xdat Ydat1 Ydat2 ['fmt']||Xdat1 Ydat1 Xdat2 Ydat2 ['fmt']|Xdat1 Ydat1 Zdat1 Xdat2 Ydat2 Zdat2 ['fmt']", mgls_region ,7},
	{"reset","Reset settings and clear picture","reset", mgls_reset ,12},
	{"resize","Resize data","resize Res Dat mx [my mz]", mgls_resize ,4},
	{"return","Return from function","return", 0, 6},
	{"rhomb","Draw rhombus","rhomb x1 y1 x2 y2 r ['fmt']|x1 y1 z1 x2 y2 z2 r ['fmt']", mgls_rhomb ,13},
	{"rkstep","Apply Runge-Kutta","rkstep 'Diff1;Diff2;...' 'Var1;Var2;...' [dt]", 0, 6},
	{"roll","Roll data along direction","roll Dat 'dir' num", mgls_roll ,16},
	{"roots", "Find roots using data as initial values", "roots Res 'func' Ini ['var']|Res 'func' ini ['var']", mgls_roots ,4},
	{"rotate","Rotate plot","rotate tetz tetx [tety] | tet x y z", mgls_rotate ,5},
	{"rotatetext","Set to auto rotate text or not","rotatetext val", mgls_rotatetext ,15},
	{"save","Save data to file","save Dat 'file'|'str' 'file'|'str' 'file' 'how'", mgls_save ,3},
	{"savehdf","Save data to HDF5 file","savehdf Dat 'file' 'id' [rewrite]", mgls_savehdf ,3},
	{"scanfile","Get formated data from file","scanfile Dat 'fname 'templ'", mgls_scanfile ,4},
	{"setsize","Set picture size","setsize width height", mgls_setsize ,2},
	{"setsizescl","Set scaling factor for further setsize","setsizescl val", mgls_setsizescl ,2},
	{"sew","Remove jump into the data, like phase jumps","sew Dat ['dir' da]", mgls_sew ,16},
	{"shear","Shear plot","shear valx valy", mgls_shear ,5},
	{"shearplot","Set position of plot inside cell of sheared stick", "shearplot num ind sx sy [xd yd]", mgls_shearplot ,5},
	{"sinfft","Sin-Fourier transform at some direction","sinfft Dat 'dir'", mgls_sinfft ,16},
	{"smooth","Smooth data","smooth Dat [kind 'dir']", mgls_smooth ,16},
	{"solve","Find root Dat_{i,j,k}=val (inverse evaluate)","solve Res Dat val 'dir' [Idat norm]", mgls_solve ,4},
	{"sort","Sort data by values in column","sort Dat idx [idy]", mgls_sort ,3},
	{"sphere","Draw sphere","sphere x0 y0 r ['fmt']|x0 y0 z0 r ['fmt']", mgls_sphere ,13},
	{"squeeze","Squeeze data","squeeze Dat kx [ky kz]", mgls_squeeze ,3},
	{"stem","Draw stem plot for 1D data","stem Ydat ['fmt']|Xdat Ydat ['fmt']|Xdat Ydat Zdat ['fmt']", mgls_stem ,7},
	{"step","Draw step plot for 1D data","step Ydat ['fmt']|Xdat Ydat ['fmt']|Xdat Ydat Zdat ['fmt']", mgls_step ,7},
	{"stfa","Draw STFA diagram","stfa Udat Vdat dn ['fmt']|Xdat Ydat Udat Vdat dn ['fmt']", mgls_stfa ,10},
	{"stfad","Do STFA transform","stfad Res Real Imag dn ['dir']", mgls_stfad ,4},
	{"stickplot","Set position of plot inside cell of rotated stick", "stickplot num ind tet phi", mgls_stickplot ,5},
	{"stop","Stop execution","stop", 0, 6},
	{"subdata","Extract sub-array","subdata Res Dat nx [ny nz]", mgls_subdata ,4},
	{"subplot","Set position of plot","subplot m n pos ['style' dx dy]", mgls_subplot ,5},
	{"subto","Subtract data or number","subto Var Dat|Var num", mgls_subto ,3},
	{"sum","Find summation over direction","sum Res Dat 'dir'", mgls_sum ,4},
	{"surf","Draw solid surface","surf Zdat ['fmt']|Xdat Ydat Zdat ['fmt']", mgls_surf ,8},
	{"surf3","Draw isosurface for 3D data","surf3 Adat ['fmt' num]|Xdat Ydat Zdat Adat ['fmt' num]|Adat val ['fmt']|Xdat Ydat Zdat Adat val ['fmt']", mgls_surf3 ,9},
	{"surf3a","Draw isosurface for 3D data transpared by other data","surf3a Adat Cdat ['fmt' num]|Xdat Ydat Zdat Adat Cdat ['fmt' num]|Adat Cdat val ['fmt']|Xdat Ydat Zdat Adat Cdat val ['fmt']", mgls_surf3a ,10},
	{"surf3c","Draw isosurface for 3D data colored by other data","surf3c Adat Cdat ['fmt' num]|Xdat Ydat Zdat Adat Cdat ['fmt' num]|Adat Cdat val ['fmt']|Xdat Ydat Zdat Adat Cdat val ['fmt']", mgls_surf3c ,10},
	{"surf3ca","Draw isosurface for 3D data colored and transpared by other data","surf3c Adat Cdat Bdat ['fmt' num]|Xdat Ydat Zdat Adat Cdat Bdat ['fmt' num]|Adat Cdat Bdat val ['fmt']|Xdat Ydat Zdat Adat Cdat Bdat val ['fmt']", mgls_surf3ca ,10},
	{"surfa","Draw solid surface transpared by other data","surfa Zdat Cdat ['fmt']|Xdat Ydat Zdat Cdat ['fmt']", mgls_surfa ,10},
	{"surfc","Draw solid surface colored by other data","surfc Zdat Cdat ['fmt']|Xdat Ydat Zdat Cdat ['fmt']", mgls_surfc ,10},
	{"surfca","Draw solid surface colored and transpared by other data","surfca Zdat Cdat Adat ['fmt']|Xdat Ydat Zdat Cdat Adat ['fmt']", mgls_surfca ,10},
	{"swap","Swap data (usefull after Fourier transform)","swap Dat 'dir'", mgls_swap ,16},
	{"table","Draw table with data values","table Dat ['txt' 'fmt']|x y Dat ['txt' 'fmt']", mgls_table ,7},
	{"tape","Draw binormales for 1D data","tape Ydat ['fmt']|Xdat Ydat ['fmt']|Xdat Ydat Zdat ['fmt']", mgls_tape ,7},
	{"tens","Draw tension plot for 1D data","tens Ydat Cdat ['fmt']|Xdat Ydat Cdat ['fmt']|Xdat Ydat Zdat Cdat ['fmt']", mgls_tens ,7},
	{"ternary","Switch on/off to use ternary axis","ternary val", mgls_ternary ,14},
	{"text","Draw text at some position or along curve","text x y 'txt' ['fmt' size]|x y z 'txt' ['fmt' size]|x y dx dy 'txt' ['fmt' size]|x y z dx dy dz 'txt' ['fmt' size]|Ydat 'txt' ['font' sise]|Xdat Ydat 'txt' ['font' sise]", mgls_text ,15},
	{"textmark","Draw TeX mark at point position","textmark Ydat Rdat 'text' ['fmt']|Xdat Ydat Rdat 'text' ['fmt']|Xdat Ydat Zdat Rdat 'text' ['fmt']", mgls_textmark ,7},
	{"ticklen","Set tick length","ticklen val [stt]", mgls_ticklen ,14},
	{"tickshift","Set additional tick and axis labels shift","tickshift dx [dy dz dc]", mgls_tickshift ,14},
	{"ticktime","Set ticks in time format","ticktime 'dir' [dv 'tmpl']", mgls_ticktime ,14},
	{"tile","Draw horizontal tiles","tile Zdat ['fmt']|Xdat Ydat Zdat ['fmt']", mgls_tile ,8},
	{"tiles","Draw horizontal tiles with variable size","tiles Zdat Rdat ['fmt']|Xdat Ydat Zdat Rdat ['fmt']", mgls_tiles ,10},
	{"title","Add title for current subplot/inplot","title 'txt' ['fmt' size]", mgls_title ,5},
	{"tlabel","Draw label for t-axis","tlabel 'txt' [pos]", mgls_tlabel ,12},
	{"torus","Draw surface of curve rotation","torus Rdat ['fmt']|Zdat Rdat ['fmt']", mgls_torus ,7},
	{"trace","Get trace of array","trace Res Dat", mgls_trace ,4},
	{"traj","Draw vectors along a curve","traj Xdat Ydat Udat Vdat ['fmt' len]|Xdat Ydat Zdat Udat Vdat Wdat ['fmt' len]", mgls_traj ,11},
	{"transform","Do integral transform of data","transform Res 'how' Rdat Idat", mgls_transform ,4},
	{"transforma","Do integral transform of data","transforma Res 'how' Adat Pdat", mgls_transforma ,4},
	{"transpose","Transpose data array","transpose Dat ['dir']", mgls_transpose ,16},
	{"transptype","Set type transparency","transptype val", mgls_transptype ,2},
	{"triangulate","Find triangles of randomly placed points","triangulate Res Xdat Ydat [er]|Res Xdat Ydat Zdat [er]", mgls_triangulate ,4},
	{"tricont","Draw contour lines for surface of triangles","tricont Idat Xdat Ydat Cdat ['fmt']|Idat Xdat Ydat Zdat Cdat ['fmt']|Vdat Idat Xdat Ydat Cdat ['fmt']|Vdat Idat Xdat Ydat Zdat Cdat ['fmt']", mgls_tricont ,0},
	{"tricontv","Draw contour tubes for surface of triangles","tricontv Idat Xdat Ydat Cdat ['fmt']|Idat Xdat Ydat Zdat Cdat ['fmt']|Vdat Idat Xdat Ydat Cdat ['fmt']|Vdat Idat Xdat Ydat Zdat Cdat ['fmt']", mgls_tricontv ,0},
	{"tridmat","Solve tridiagonal matrix","tridmat Res A B C D 'how'", mgls_tridmat ,4},
	{"triplot","Draw surface of triangles","triplot Idat Xdat Ydat ['fmt']|Idat Xdat Ydat Zdat ['fmt']|Idat Xdat Ydat Zdat Cdat ['fmt'] ", mgls_triplot ,0},
	{"tube","Draw curve by tube","tube Ydat Rdat ['fmt']|Ydat rval ['fmt']|Xdat Ydat Rdat ['fmt']|Xdat Ydat rval ['fmt']|Xdat Ydat Zdat Rdat ['fmt']|Xdat Ydat Zdat rval ['fmt']", mgls_tube ,7},
	{"tuneticks","Set ticks tuning","tuneticks val [fctr]", mgls_tuneticks ,14},
	{"var","Create new 1D data and fill it in range","var Dat nx x1 [x2]", mgls_var ,4},
	{"variant","Select variant of plot style(s)","variant var", 0, 6},
	{"vect","Draw vector field","vect Udat Vdat ['fmt']|Xdat Ydat Udat Vdat ['fmt']|Udat Vdat Wdat ['fmt']|Xdat Ydat Zdat Udat Vdat Wdat ['fmt']", mgls_vect ,11},
	{"vect3","Draw vector field at slices of 3D data","vect Udat Vdat Wdat ['fmt' sval]|Xdat Ydat Zdat Udat Vdat Wdat ['fmt' sval]", mgls_vect3 ,11},
	{"version","Print MathGL version or check if it is valid","version |'ver'", mgls_version, 2},
	{"view","Change view angles - use 'rotate' for plot rotation","view tetz tetx [tety]", mgls_view ,5},
	{"wavelet","Wavelet transform at some direction","wavelet Dat 'dir' k", mgls_wavelet ,16},
	{"write","Write current image to graphical file","write ['fname']", mgls_write ,2},
	{"xlabel","Draw label for x-axis","xlabel 'txt' [pos]", mgls_xlabel ,12},
	{"xrange","Set range for x-axis","xrange Dat [add] | x1 x2 [add]", mgls_xrange ,14},
	{"xtick","Set ticks for x-axis","xtick dx ['factor'] | dx sx ['factor'] | dx sx tx ['factor'] | 'tmpl' | Xdat 'lbl' [add] | v1 'lbl1' ...", mgls_xtick,14},
	{"ylabel","Draw label for y-axis","ylabel 'txt' [pos]", mgls_ylabel,12},
	{"yrange","Set range for y-axis","yrange Dat [add] | y1 y2 [add]", mgls_yrange,14},
	{"ytick","Set ticks for y-axis","ytick dy ['factor'] | dy sy ['factor'] | dy sy ty ['factor'] | 'tmpl' | Ydat 'lbl' [add] | v1 'lbl1' ...", mgls_ytick,14},
	{"zlabel","Draw label for z-axis","zlabel 'txt' [pos]", mgls_zlabel,12},
	{"zoom","Zoom plot region","zoom x1 x2 y1 y2", mgls_zoom,5},
	{"zoomaxis","Zoom axis range","zoomaxis x1 x2|x1 x2 y1 y2|x1 x2 y1 y2 z1 z2|x1 x2 y1 y2 z1 z2 c1 c2", mgls_zoomaxis,14},
	{"zrange","Set range for z-axis","yrange Dat [add] | z1 z2 [add]", mgls_zrange ,14},
	{"ztick","Set ticks for z-axis","ztick dz ['factor'] | dz sz ['factor'] | dz sz tz ['factor'] | 'tmpl' | Zdat 'lbl' [add] | v1 'lbl1' ...", mgls_ztick,14},
{"","","",NULL,0}};
//-----------------------------------------------------------------------------
