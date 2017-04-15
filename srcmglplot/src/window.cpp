/***************************************************************************
 * window.cpp is part of Math Graphic Library
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
#include "mgl2/canvas_wnd.h"
//-----------------------------------------------------------------------------
mglCanvasWnd::mglCanvasWnd() : mglCanvas()
{
	Setup();	LoadFunc=0;	FuncPar=0;	DrawFunc=0;	ClickFunc=0;
	GG = 0;		NumFig = 0;	CurFig = -1;
#if MGL_HAVE_PTHR_WIDGET
	mutex=0;
#endif
}
//-----------------------------------------------------------------------------
mglCanvasWnd::~mglCanvasWnd()	{	if(GG) free(GG);	}
//-----------------------------------------------------------------------------
void mglCanvasWnd::SetCurFig(int c)
{
	CurFig=c;
	if(get(MGL_VECT_FRAME) && c>=0 && c<(long)DrwDat.size() && DrawFunc)
		GetFrame(c);
}
//-----------------------------------------------------------------------------
void mglCanvasWnd::ResetFrames()
{
	if(GG)	free(GG);	GG = 0;
	NumFig = CurFig = 0;
	mglCanvas::ResetFrames();
}
//-----------------------------------------------------------------------------
void mglCanvasWnd::SetSize(int w,int h,bool)
{
	if(DrawFunc)	ResetFrames();
	mglCanvas::SetSize(w,h,false);
//	if(Wnd)	Wnd->size(w,h);
}
//-----------------------------------------------------------------------------
void mglCanvasWnd::EndFrame()
{
	CurFig = CurFrameId-1;
	if(!GG)
	{
		GG = (unsigned char *)malloc(3*Width*Height);
		NumFig = 1;		CurFig = 0;
	}
	else if(CurFig>NumFig-1)
	{
		GG = (unsigned char *)realloc(GG,3*(CurFig+1)*Width*Height);
		NumFig = CurFig+1;
	}
	mglCanvas::EndFrame();
	memcpy(GG + CurFig*Width*Height*3,G,3*Width*Height);
	CurFig++;
}
//-----------------------------------------------------------------------------
void mglCanvasWnd::SetFrame(long i)
{
	mglCanvas::SetFrame(i);
	if(i>=0 && i<NumFig)	memcpy(GG + i*Width*Height*3,G,3*Width*Height);
}
//-----------------------------------------------------------------------------
void mglCanvasWnd::DelFrame(long i)
{
	if(i<0 || i>=CurFrameId)	return;
	if(CurFig>=i)	CurFig--;
	long n = Width*Height*3;
	if(CurFrameId-i>1)	memmove(GG+i*n, GG+i*n+n, n*(CurFrameId-i-1));
	mglCanvas::DelFrame(i);
}
//-----------------------------------------------------------------------------
void mglCanvasWnd::SetDrawFunc(int (*draw)(mglBase *gr, void *p), void *par, void (*reload)(void *p))
{
	if(draw)
	{
		ResetFrames();
		if(get(MGL_CLF_ON_UPD))	DefaultPlotParam();
		const std::string loc = setlocale(LC_NUMERIC, NULL);	setlocale(LC_NUMERIC, "C");
		// use frames for quickly redrawing while adding/changing primitives
		if(mgl_is_frames(this))	NewFrame();

		int n = draw(this,par);
		if(n<NumFig && n>=0)	NumFig = n;
		DrawFunc = draw;		FuncPar = par;
		LoadFunc = reload;

		if(mgl_is_frames(this))	EndFrame();
		if(n>=0)	SetCurFig(0);
		setlocale(LC_NUMERIC, loc.c_str());
	}
	else	LoadFunc = 0;
}
//-----------------------------------------------------------------------------
const unsigned char *mglCanvasWnd::GetBits()
{
	const unsigned char *g = mglCanvas::GetBits();
	if(GG && NumFig>0 && CurFig<NumFig && CurFig>=0 && !get(MGL_VECT_FRAME))
		g = GG + CurFig*Width*Height*3;
	return g;
}
//-----------------------------------------------------------------------------
void mglCanvasWnd::ReLoad()
{
	if(LoadFunc)
	{
		LoadFunc(FuncPar);
		// update number of slides
		ResetFrames();
		const std::string loc = setlocale(LC_NUMERIC, NULL);	setlocale(LC_NUMERIC, "C");
		// use frames for quickly redrawing while adding/changing primitives
		if(mgl_is_frames(this))	NewFrame();

		int n = DrawFunc ? DrawFunc(this,FuncPar) : 0;
		if(n<NumFig && n>=0)	NumFig = n;

		if(mgl_is_frames(this))	EndFrame();
		setlocale(LC_NUMERIC, loc.c_str());
		Update();
	}
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_wnd_set_func(HMGL gr, int (*draw)(HMGL gr, void *p), void *par, void (*reload)(void *p))
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>(gr);	if(g)	g->SetDrawFunc(draw, par, reload);	}
void MGL_EXPORT mgl_wnd_toggle_alpha(HMGL gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>(gr);	if(g)	g->ToggleAlpha();	}
void MGL_EXPORT mgl_wnd_toggle_light(HMGL gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>(gr);	if(g)	g->ToggleLight();	}
void MGL_EXPORT mgl_wnd_toggle_zoom(HMGL gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>(gr);	if(g)	g->ToggleZoom();	}
void MGL_EXPORT mgl_wnd_toggle_rotate(HMGL gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>(gr);	if(g)	g->ToggleRotate();	}
void MGL_EXPORT mgl_wnd_toggle_no(HMGL gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>(gr);	if(g)	g->ToggleNo();	}
void MGL_EXPORT mgl_wnd_update(HMGL gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>(gr);	if(g)	g->Update();	}
void MGL_EXPORT mgl_wnd_reload(HMGL gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>(gr);	if(g)	g->ReLoad();	}
void MGL_EXPORT mgl_wnd_adjust(HMGL gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>(gr);	if(g)	g->Adjust();	}
void MGL_EXPORT mgl_wnd_next_frame(HMGL gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>(gr);	if(g)	g->NextFrame();	}
void MGL_EXPORT mgl_wnd_prev_frame(HMGL gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>(gr);	if(g)	g->PrevFrame();	}
void MGL_EXPORT mgl_wnd_animation(HMGL gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>(gr);	if(g)	g->Animation();	}
void MGL_EXPORT mgl_setup_window(HMGL gr, int clf_upd, int showpos)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>(gr);	if(g)	g->Setup(clf_upd, showpos);	}
void MGL_EXPORT mgl_set_click_func(HMGL gr, void (*func)(void *p))
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>(gr);	if(g)	g->ClickFunc = func;	}
void MGL_EXPORT mgl_get_last_mouse_pos(HMGL gr, mreal *x, mreal *y, mreal *z)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>(gr);
	mglPoint p;	if(g)	p=g->GetMousePos();
	*x=p.x;	*y=p.y;	*z=p.z;	}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_wnd_toggle_alpha_(uintptr_t *gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>((HMGL)(*gr));
	if(g)	g->ToggleAlpha();	}
void MGL_EXPORT mgl_wnd_toggle_light_(uintptr_t *gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>((HMGL)(*gr));
	if(g)	g->ToggleLight();	}
void MGL_EXPORT mgl_wnd_toggle_zoom_(uintptr_t *gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>((HMGL)(*gr));
	if(g)	g->ToggleZoom();	}
void MGL_EXPORT mgl_wnd_toggle_rotate_(uintptr_t *gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>((HMGL)(*gr));
	if(g)	g->ToggleRotate();	}
void MGL_EXPORT mgl_wnd_toggle_no_(uintptr_t *gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>((HMGL)(*gr));
	if(g)	g->ToggleNo();	}
void MGL_EXPORT mgl_wnd_update_(uintptr_t *gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>((HMGL)(*gr));
	if(g)	g->Update();	}
void MGL_EXPORT mgl_wnd_reload_(uintptr_t *gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>((HMGL)(*gr));
	if(g)	g->ReLoad();	}
void MGL_EXPORT mgl_wnd_adjust_(uintptr_t *gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>((HMGL)(*gr));
	if(g)	g->Adjust();	}
void MGL_EXPORT mgl_wnd_next_frame_(uintptr_t *gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>((HMGL)(*gr));
	if(g)	g->NextFrame();	}
void MGL_EXPORT mgl_wnd_prev_frame_(uintptr_t *gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>((HMGL)(*gr));
	if(g)	g->PrevFrame();	}
void MGL_EXPORT mgl_wnd_animation_(uintptr_t *gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>((HMGL)(*gr));
	if(g)	g->Animation();	}
void MGL_EXPORT mgl_setup_window_(uintptr_t *gr, int *clf_upd, int *showpos)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>((HMGL)(*gr));
	if(g)	g->Setup(*clf_upd, *showpos);	}
void MGL_EXPORT mgl_get_last_mouse_pos_(uintptr_t *gr, mreal *x, mreal *y, mreal *z)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>((HMGL)(*gr));
	mglPoint p;	if(g)	p=g->GetMousePos();
	*x=p.x;	*y=p.y;	*z=p.z;	}
//-----------------------------------------------------------------------------
#if MGL_HAVE_PTHR_WIDGET
void MGL_EXPORT mgl_wnd_set_mutex(HMGL gr, pthread_mutex_t *mutex)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>(gr);
	if(g)	g->mutex = mutex;	}
#endif
//-----------------------------------------------------------------------------
//
//	mglDraw class handling
//
//-----------------------------------------------------------------------------
/*int mgl_draw_class(HMGL gr, void *p)	// so stupid way to save mglDraw class inheritance :(
{
	mglGraph g(gr);	mglWindow *w = (mglWindow *)p;
	return (w && w->dr) ? w->dr->Draw(&g) : 0;
}
void MGL_EXPORT mgl_reload_class(void *p)	// so stupid way to save mglDraw class inheritance :(
{	mglWindow *w = (mglWindow *)p;	if(w && w->dr)	w->dr->Reload();}
void MGL_EXPORT mgl_click_class(void *p)	// so stupid way to save mglDraw class inheritance :(
{	mglWindow *w = (mglWindow *)p;	if(w && w->dr)	w->dr->Click();	}*/
int MGL_EXPORT mgl_draw_class(HMGL gr, void *p)
{	mglGraph g(gr);	mglDraw *dr = (mglDraw *)p;	return dr?dr->Draw(&g):0;	}
void MGL_EXPORT mgl_reload_class(void *p)
{	mglDraw *dr = (mglDraw *)p;	if(dr)	dr->Reload();	}
void MGL_EXPORT mgl_click_class(void *p)
{	mglDraw *dr = (mglDraw *)p;	if(dr)	dr->Click();		}
//-----------------------------------------------------------------------------
typedef int (*draw_func)(mglGraph *gr);
int MGL_EXPORT mgl_draw_graph(HMGL gr, void *p)
{
	mglGraph g(gr);
	draw_func func = (draw_func)(p);
	return func ? func(&g) : 0;
}
//-----------------------------------------------------------------------------
MGL_EXPORT void *mgl_draw_calc(void *p)
{
#if MGL_HAVE_PTHR_WIDGET
	mglDraw *d = (mglDraw *)p;
	d->Calc();	d->running = false;
#endif
	return 0;
}
//-----------------------------------------------------------------------------
