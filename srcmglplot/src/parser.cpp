/***************************************************************************
 * parse.cpp is part of Math Graphic Library
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
#include <ctype.h>
#include "mgl2/parser.h"
#include "mgl2/canvas_cf.h"
#include "mgl2/base.h"
//-----------------------------------------------------------------------------
MGL_EXPORT void (*mgl_ask_func)(const wchar_t *, wchar_t *)=0;
void MGL_EXPORT mgl_ask_gets(const wchar_t *quest, wchar_t *res)
{	printf("%ls\n",quest);	if(!fgetws(res,1024,stdin))	*res=0;	}
//-----------------------------------------------------------------------------
mglFunc::mglFunc(long p, const wchar_t *f)
{
	pos = p;	func = f;
	size_t i;
	for(i=0;(isalnum(f[i]) || f[i]=='_');i++);
	narg = wcstol(f+i+1,0,0);	func = func.substr(0,i);
	if(narg<0 || narg>9)	narg=0;
}
//-----------------------------------------------------------------------------
long mglParser::IsFunc(const std::wstring &name, int *na)
{
	for(size_t i=0;i<func.size();i++)
	{
		const mglFunc &f = func[i];
		if(f.func==name)
		{	if(na)	*na=f.narg;	return f.pos+1;	}
	}
	return 0;
}
//-----------------------------------------------------------------------------
void mglParser::ScanFunc(const wchar_t *line)
{
	static long num=0;
	if(!line)
	{	func.clear();	num=0;	return;	}
	num++;
	while(*line<=' ' && *line!=0)	line++;
	if(wcsncmp(line,L"func",4) || line[4]>' ')	return;
	register long i;
	for(i=4;line[i]<=' ' || line[i]=='\'';i++);
	func.push_back(mglFunc(num-1, line+i));
}
//-----------------------------------------------------------------------------
MGL_NO_EXPORT wchar_t *mgl_str_copy(const char *s)
{
	register size_t i,l=strlen(s);
	wchar_t *str = new wchar_t[l+1];
	for(i=0;i<l;i++)	str[i] = s[i];
	str[i] = 0;	return str;
}
//-----------------------------------------------------------------------------
int MGL_LOCAL_PURE mgl_cmd_cmp(const void *a, const void *b)
{
	const mglCommand *aa = (const mglCommand *)a;
	const mglCommand *bb = (const mglCommand *)b;
	return strcmp(aa->name, bb->name);
}
//-----------------------------------------------------------------------------
bool mglParser::CheckForName(const std::wstring &s)
{
	return !isalpha(s[0]) || s.find_first_of(L"!@#$%%^&*()-+|,.<>:")!=std::wstring::npos || s==L"rnd" || FindNum(s.c_str());
//	return !isalpha(s[0])||s.find_first_of(L".:()")!=std::wstring::npos;
}
//-----------------------------------------------------------------------------
const mglCommand *mglParser::FindCommand(const char *com) const
{
	if(!AllowFileIO && ( !strncmp(com,"read",4) || !strncmp(com,"save",4) || !strcmp(com,"fgets") || !strcmp(com,"import") || !strcmp(com,"export") ))
		return 0;
	mglCommand tst, *rts, *cmd = Cmd;
	long i;
	for(i=0;cmd[i].name[0];i++);	// determine the number of symbols
	tst.name = com;
	rts = (mglCommand *) bsearch(&tst, cmd, i, sizeof(mglCommand), mgl_cmd_cmp);
	return rts;
}
//-----------------------------------------------------------------------------
const mglCommand *mglParser::FindCommand(const wchar_t *com) const
{
	size_t s = 15<mgl_wcslen(com)?15:mgl_wcslen(com);
	char cmd[16];	wcstombs(cmd,com,s+1);	cmd[s]=0;
	return FindCommand(cmd);
}
//-----------------------------------------------------------------------------
// return values : 0 -- OK, 1 -- wrong arguments, 2 -- wrong command, 3 -- unclosed string
int mglParser::Exec(mglGraph *gr, const wchar_t *com, long n, mglArg *a, const std::wstring &/*var*/, const wchar_t *opt)
{
	int i;
	const char *id="dsn";
	std::string k;
	for(i=0;i<n;i++)
	{
		k += id[a[i].type];
		size_t len = wcstombs(NULL,a[i].w.c_str(),0)+1;
		char *buf = new char[len];	memset(buf,0,len);
		wcstombs(buf,a[i].w.c_str(),len);
		a[i].s = buf;	delete []buf;
	}
	const mglCommand *rts=FindCommand(com);
	if(!rts || rts->type==6)	return 2;
/*	if(rts->type == 4)
	{
		if(n<1 || CheckForName(var))	return 2;
		a[0].type = 0;	a[0].d = AddVar(var.c_str());
		a[0].w = var;	k[0] = 'd';
	}*/
	char *o=0;
	if(opt && *opt)	// TODO: parse arguments of options
	{
		long len = mgl_wcslen(opt);
		o = new char[len+1];
		for(i=0;i<len+1;i++)	o[i]=opt[i];
	}
	int res=rts->exec(gr, n, a, k.c_str(), o);
	if(o)	delete []o;
	return res;
}
//-----------------------------------------------------------------------------
mglParser::mglParser(bool setsize)
{
	InUse = 1;	curGr = 0;	Variant = 0;
	Skip=Stop=for_br=false;
	memset(for_stack,0,40*sizeof(int));
	memset(if_stack,0,40*sizeof(int));
	memset(if_for,0,40*sizeof(int));
	if_pos=for_addr=0;
	for(long i=0;i<40;i++)	par[i]=L"";

	Cmd = mgls_base_cmd;
	AllowSetSize=setsize;	AllowFileIO=true;	AllowDllCall=true;
	Once = true;
	fval = new mglData[40];
	mglNum *v;
	v = new mglNum(0);	v->s = L"off";	NumList.push_back(v);
	v = new mglNum(1);	v->s = L"on";	NumList.push_back(v);
	v = new mglNum(NAN);	v->s = L"nan";	NumList.push_back(v);
	v = new mglNum(M_PI);	v->s = L"pi";	NumList.push_back(v);
	v = new mglNum(INFINITY);	v->s = L"inf";	NumList.push_back(v);
#if MGL_HAVE_LTDL
	lt_dlinit();
#endif
}
//-----------------------------------------------------------------------------
mglParser::~mglParser()
{
	DeleteAll();	delete []fval;
	for(size_t i=0;i<NumList.size();i++)	// force delete built-in variables
		if(NumList[i])	delete NumList[i];
	NumList.clear();
#if MGL_HAVE_LTDL
	lt_dlexit();
#endif
}
//-----------------------------------------------------------------------------
void mglParser::DeleteAll()
{
	for(size_t i=0;i<DataList.size();i++)
		if(DataList[i])	delete DataList[i];
	DataList.clear();
	for(size_t i=0;i<NumList.size();i++)
		if(NumList[i])	delete NumList[i];
	NumList.clear();
	mglNum *v;
	v = new mglNum(0);	v->s = L"off";	NumList.push_back(v);
	v = new mglNum(1);	v->s = L"on";	NumList.push_back(v);
	v = new mglNum(NAN);	v->s = L"nan";	NumList.push_back(v);
	v = new mglNum(M_PI);	v->s = L"pi";	NumList.push_back(v);
	v = new mglNum(INFINITY);	v->s = L"inf";	NumList.push_back(v);
	if(Cmd && Cmd!=mgls_base_cmd)
	{	delete []Cmd;	Cmd = mgls_base_cmd;	}
#if MGL_HAVE_LTDL
	for(size_t i=0;i<DllOpened.size();i++)
		lt_dlclose(DllOpened[i]);
	DllOpened.clear();
#endif
}
//-----------------------------------------------------------------------------
void mglParser::AddParam(int n, const char *str)
{
	MGL_TO_WCS(str,AddParam(n,wcs));
}
//-----------------------------------------------------------------------------
int mglParser::Parse(mglGraph *gr, const char *str, long pos)
{
	int r=0;
	MGL_TO_WCS(str,r = Parse(gr,wcs,pos));
	return r;
}
//-----------------------------------------------------------------------------
mglDataA *mglParser::AddVar(const char *str)
{
	mglDataA *v=0;
	MGL_TO_WCS(str,v = AddVar(wcs));
	return v;
}
//-----------------------------------------------------------------------------
mglDataA *mglParser::FindVar(const char *str)
{
	mglDataA *v=0;
	MGL_TO_WCS(str,v = FindVar(wcs));
	return v;
}
//-----------------------------------------------------------------------------
mglNum *mglParser::AddNum(const char *str)
{
	mglNum *v=0;
	MGL_TO_WCS(str,v = AddNum(wcs));
	return v;
}
//-----------------------------------------------------------------------------
mglNum *mglParser::FindNum(const char *str)
{
	mglNum *v=0;
	MGL_TO_WCS(str,v = FindNum(wcs));
	return v;
}
//-----------------------------------------------------------------------------
void mglParser::AddParam(int n, const wchar_t *str)
{
//	if(str && n>=0 && n<40 && !wcschr(str,'$'))	par[n] = str;
	if(str && n>=0 && n<40)	par[n] = str;
}
//-----------------------------------------------------------------------------
mglDataA *mglParser::FindVar(const wchar_t *name)
{
	if(name[0]=='!')	name = name+1;	// ignore complex prefix
 	for(size_t i=0;i<DataList.size();i++)
 		if(DataList[i] && DataList[i]->s==name)	return DataList[i];
	return 0;
}
//-----------------------------------------------------------------------------
mglDataA *mglParser::AddVar(const wchar_t *name)
{	// TODO add list of forbidden names (like function names)
	mglDataA *d=FindVar(name);
	if(name[0]=='!' && dynamic_cast<mglDataC*>(d)==0)
	{	d = new mglDataC;	d->s=(name+1);	DataList.push_back(d);	}
	else if(!d)
	{	d = new mglData;	d->s = name;	DataList.push_back(d);	}
	return d;
}
//-----------------------------------------------------------------------------
mglNum *mglParser::FindNum(const wchar_t *name)
{
	for(size_t i=0;i<NumList.size();i++)
		if(NumList[i] && NumList[i]->s==name)	return NumList[i];
	return 0;
}
//-----------------------------------------------------------------------------
mglNum *mglParser::AddNum(const wchar_t *name)
{
	mglNum *v = FindNum(name);
	if(!v)	{	v=new mglNum;	v->s = name;	NumList.push_back(v);	}
	return v;
}
//-----------------------------------------------------------------------------
int MGL_LOCAL_PURE mglFindArg(const std::wstring &str)
{
	register long l=0,k=0;
	const size_t s=str.length();
	for(size_t i=0;i<s;i++)
	{
		if(str[i]=='\'') l++;
		if(str[i]=='{') k++;
		if(str[i]=='}') k--;
		if(l%2==0 && k==0)
		{
			if(str[i]=='#' || str[i]==';')	return -long(i);
			if(str[i]<=' ')	return long(i);
		}
	}
	return 0;
}
//-----------------------------------------------------------------------------
HMDT MGL_NO_EXPORT mglFormulaCalc(std::wstring string, mglParser *arg, const std::vector<mglDataA*> &head);
HADT MGL_NO_EXPORT mglFormulaCalcC(std::wstring string, mglParser *arg, const std::vector<mglDataA*> &head);
//-----------------------------------------------------------------------------
// convert substrings to arguments
void mglParser::FillArg(mglGraph *gr, int k, std::wstring *arg, mglArg *a)
{
	register long n;
	for(n=1;n<k;n++)
	{
		mglDataA *v;	mglNum *f;
		a[n-1].type = -1;
		std::wstring str = arg[n];
		size_t i1=0, i2=str.length(), j=0;
		bool s=true;
		for(size_t i=0;i<i2;i++)
		{
			if(str[i]=='\'')	s = !s;
			if(s && str[i]=='?')
			{
				if(j<Variant)	i1=i+1;
				else	i2=i;
				j++;
			}
		}
		str = str.substr(i1,i2-i1);

		if(str.empty())	a[n-1].type = -2;
		else if(str[0]=='|')	a[n-1].type = -1;
		else if(str[0]=='\'')	// this is string (simplest case)
		{
			a[n-1].type = 1;
			std::wstring &w=str,f;
			wchar_t buf[32];
			long i,i1,ll=w.length();
			for(i=1;i<ll;i++)
			{
				if(w[i]=='\'')
				{
					if(i==ll-1)	continue;
					i++;	i1 = i;
					if(w[i1]==',')	i1++;
					if(w[i1]==0)	continue;
					for(;i<ll && w[i]!='\'';i++);
					if(i>i1)
					{
						if(w[i1]=='!')
						{
							HADT d = mglFormulaCalcC(w.substr(i1+1,i-i1-(w[i]=='\''?1:0)), this, DataList);
							mreal di = imag(d->a[0]), dr = real(d->a[0]);
							if(di>0)	mglprintf(buf,32,L"%g+%gi",dr,di);
							else if(di<0)	mglprintf(buf,32,L"%g-%gi",dr,-di);	// TODO use \u2212 ???
							else	mglprintf(buf,32,L"%g",dr);
							a[n-1].w += buf;	delete d;
						}
						else
						{
							HMDT d = mglFormulaCalc(w.substr(i1,i-i1-(w[i]=='\''?1:0)), this, DataList);
							mglprintf(buf,32,L"%g",d->a[0]);	a[n-1].w += buf;	delete d;
						}
					}
				}
				else	a[n-1].w += w[i];
			}
		}
		else if(str[0]=='{')
		{	// this is temp data
			mglData *u=new mglData;
			std::wstring s = str.substr(1,str.length()-2);
			a[n-1].w = u->s = L"/*"+s+L"*/";
			a[n-1].type = 0;
			ParseDat(gr, s, *u);	a[n-1].d = u;
			u->temp=true;	DataList.push_back(u);
		}
		else if((v = FindVar(str.c_str()))!=0)	// try to find normal variables (for data creation)
		{	a[n-1].type=0;	a[n-1].d=v;	a[n-1].w=v->s;	}
		else if((f = FindNum(str.c_str()))!=0)	// try to find normal number (for data creation)
		{	a[n-1].type=2;	a[n-1].d=0;	a[n-1].v=f->d;	a[n-1].c=f->c;	a[n-1].w = f->s;	}
		else if(str[0]=='!')	// complex array is asked
		{	// parse all numbers and formulas by unified way
			HADT d = mglFormulaCalcC(str.substr(1), this, DataList);
			if(d->GetNN()==1)
			{
				if(CheckForName(str.substr(1)))
				{	a[n-1].type = 2;	a[n-1].v = d->v(0);	a[n-1].c = d->a[0];	}
				else
				{	a[n-1].type = 0;	a[n-1].d = AddVar(str.c_str());	}
				delete d;
			}
			else
			{
				a[n-1].w = L"/*"+str+L"*/";
				d->temp=true;	DataList.push_back(d);
				a[n-1].type = 0;	a[n-1].d = d;
			}
		}
		else
		{	// parse all numbers and formulas by unified way
			HMDT d = mglFormulaCalc(str, this, DataList);
			if(d->GetNN()==1)
			{
				if(CheckForName(str))
				{	a[n-1].type = 2;	a[n-1].c = a[n-1].v = d->v(0);	}
				else
				{	a[n-1].type = 0;	a[n-1].d = AddVar(str.c_str());	}
				delete d;
			}
			else
			{
				a[n-1].w = L"/*"+str+L"*/";
				d->temp=true;	DataList.push_back(d);
				a[n-1].type = 0;	a[n-1].d = d;
			}
		}
	}
}
//-----------------------------------------------------------------------------
// return values: 0 - not found, 1 - OK, 2 - wrong arguments, 3 - wrong command, 4 - string too long
int mglParser::PreExec(mglGraph *, long k, std::wstring *arg, mglArg *a)
{
	long n=0;
	if(!arg[0].compare(L"delete") && k==2)	// parse command "delete"
	{	DeleteVar(arg[1].c_str());	n=1;	}
	else if(!arg[0].compare(L"list"))	// parse command "list"
	{
		if(k<3 || CheckForName(arg[1]))	return 2;
		long nx=0, ny=1,j=0,i,t=0;
		for(i=2;i<k;i++)
		{
			char ch = arg[i][0];
			if(a[i-1].type==1)	return 2;
			if(a[i-1].type==0)
			{
				if(t==1)	return 2;
				t=2;	nx++;
			}
			if(a[i-1].type==2)
			{
				if(t==2)	return 2;
				j++;	t=1;
			}
			if(ch=='|' && t==1)		{	nx = j>nx ? j:nx;	j=0;	ny++;	}
		}
		mglDataA *vv = AddVar(arg[1].c_str());
		mglData *v = dynamic_cast<mglData*>(vv);
		mglDataC *vc = dynamic_cast<mglDataC*>(vv);
		if(v)
		{
			if(t==1)	nx = j>nx ? j:nx;
			if(t==1)	// list of numeric values
			{
				v->Create(nx,ny);
				j=t=0;
				for(i=2;i<k;i++)
				{
					if(arg[i][0]=='|')	{	t++;	j=0;	}
					else
					{	v->a[j+nx*t] = a[i-1].v;	j++;	}
				}
			}
			if(t==2)	// list of data
			{
				v->Set(a[1].d);
				for(long i=2;i<k;i++)	v->Join(*(a[i].d));
			}
			n=1;
		}
		if(vc)
		{
			if(t==1)	nx = j>nx ? j:nx;
			if(t==1)	// list of numeric values
			{
				vc->Create(nx,ny);
				j=t=0;
				for(i=2;i<k;i++)
				{
					if(arg[i][0]=='|')	{	t++;	j=0;	}
					else
					{	vc->a[j+nx*t] = a[i-1].c;	j++;	}
				}
			}
			if(t==2)	// list of data
			{
				vc->Set(a[1].d);
				for(long i=2;i<k;i++)	vc->Join(*(a[i].d));
			}
			n=1;
		}
	}
	return n;
}
//-----------------------------------------------------------------------------
void mglParser::PutArg(std::wstring &str, bool def)
{
	size_t pos = str.find('$',def?10:0);
	while(pos<str.length())
	{
		wchar_t ch = str[pos+1];
		if(ch>='0' && ch<='9')	str.replace(pos,2,par[ch-'0']);
		else if(ch>='a' && ch<='z')	str.replace(pos,2,par[ch-'a'+10]);
		else if(ch=='$')	str.replace(pos,2,L"\uffff");
		else str.replace(pos,1,L"\uffff");
		pos = str.find('$',def?10:0);
	}
	while((pos = str.find(L'\uffff'))<str.length())	str[pos]='$';
}
//-----------------------------------------------------------------------------
std::wstring mgl_trim_ws(const std::wstring &str)
{
	size_t n=str.length(), k, i;
	for(k=0;k<n;k++)	if(str[k]>' ')	break;
	for(i=n;i>k;i--)	if(str[i-1]>' ')	break;
	return str.substr(k,i-k);
}
//-----------------------------------------------------------------------------
int mglParser::ParseDef(std::wstring &str)
{
	if(!skip() && !str.compare(0,3,L"def") && (str[6]==' ' || str[6]=='\t'))
	{
		int res = 1;	mreal d;
		PutArg(str,true);
		const std::wstring s = mgl_trim_ws(str.substr(7));
		if(!str.compare(3,3,L"ine"))
		{
			int nn = s[1]<='9' ? s[1]-'0' : (s[1]>='a' ? s[1]-'a'+10:-1);
			if(s[0]=='$' && nn>=0 && nn<='z'-'a'+10)
			{
				AddParam(nn, mgl_trim_ws(s.substr(2)).c_str());	return 1;
			}
		}
		if(!str.compare(3,3,L"num"))
		{
			int nn = s[1]<='9' ? s[1]-'0' : (s[1]>='a' ? s[1]-'a'+10:-1);
			if(s[0]=='$' && nn>=0 && nn<='z'-'a'+10)
			{
				res = 0;
				HMDT dd = mglFormulaCalc(mgl_trim_ws(s.substr(2)), this, DataList);
				d = dd->a[0];	delete dd;
				char buf[32];	snprintf(buf,32,"%g",d);
				buf[31] = 0;	AddParam(nn, buf);
			}
			return res+1;
		}
		if(!str.compare(3,3,L"chr"))
		{
			int nn = s[1]<='9' ? s[1]-'0' : (s[1]>='a' ? s[1]-'a'+10:-1);
			if(s[0]=='$' && nn>=0 && nn<='z'-'a'+10)
			{
				res = 0;
				HMDT dd = mglFormulaCalc(mgl_trim_ws(s.substr(2)), this, DataList);
				d=dd->a[0];	delete dd;
				wchar_t buf[2]={0,0};	buf[0] = wchar_t(d);	AddParam(nn, buf);
			}
			return res+1;
		}
	}
	if(!skip() && !str.compare(0,3,L"ask") && (str[3]==' ' || str[3]=='\t'))
	{
		PutArg(str,true);
		std::wstring s = mgl_trim_ws(str.substr(4));
		int nn = s[1]<='9' ? s[1]-'0' : (s[1]>='a' ? s[1]-'a'+10:-1);
		if(s[0]=='$' && nn>=0 && nn<='z'-'a'+10)
		{
			s = mgl_trim_ws(s.substr(2));
			if(s[0]=='\'')	s=s.substr(1,s.length()-2);
			if(mgl_ask_func)
			{
				static wchar_t res[1024];
				mgl_ask_func(s.c_str(),res);
				if(*res)	AddParam(nn, res);
			}
			return mgl_ask_func?1:2;
		}
		else	return 2;
	}
	if(!skip() && !str.compare(0,3,L"for") && (str[3]==' ' || str[3]=='\t'))
	{
		size_t i;
		for(i=4;str[i]<=' ';i++);
		// if command have format 'for $N ...' then change it to 'for N ...'
		if(str[i]=='$' && str[i+1]>='0' && str[i+1]<='9')	str[i] = ' ';
		if(str[i]=='$' && str[i+1]>='a' && str[i+1]<='z')	str[i] = ' ';
	}
	return 0;
}
//-----------------------------------------------------------------------------
/*void MGL_NO_EXPORT print_data(const char *s, std::vector<mglDataA*> h)
{
	printf("%s\ts=%lu:\n",s,h.size());
	for(size_t i=0;i<h.size();i++)
	{
		printf("%lu - %p",i,h[i]);
		if(h[i])	printf(",t%d,'%ls'",h[i]->temp, h[i]->s.c_str());
		printf("\n");
	}
	fflush(stdout);
}*/
//-----------------------------------------------------------------------------
// return values: 0 - OK, 1 - wrong arguments, 2 - wrong command, 3 - string too long, 4 -- unclosed string
int mglParser::Parse(mglGraph *gr, std::wstring str, long pos)
{
	if(Stop || gr->NeedStop())	return 0;
	curGr = gr->Self();
	str=mgl_trim_ws(str);
	long n,k=0,m=0,mm=0,res;
	// try parse ':' -- several commands in line
	for(n=0;n<long(str.length());n++)
	{
		if(str[n]=='\'' && (n==0 || str[n-1]!='\\'))	k++;
		if(k%2)	continue;
		if(str[n]=='(')	m++;	if(str[n]==')')	m--;
		if(str[n]=='{')	mm++;	if(str[n]=='}')	mm--;
		if(str[n]=='#')	break;
		if((str[n]==':' || str[n]=='\n') && k%2==0 && m==0 && mm==0)
		{
			res=Parse(gr,str.substr(0,n),pos);
			if(!res)	res=Parse(gr,str.substr(n+1),pos);
			return res;
		}
	}
	if(k%2 || m || mm)	return 4;	// strings is not closed
	// define parameters or start cycle
	res = ParseDef(str);	if(res)	return res-1;
	// parse arguments (parameters $1, ..., $9)
	PutArg(str,false);	str=mgl_trim_ws(str);

	std::wstring opt;
	std::vector<std::wstring> arg;
	while(!str.empty())	// parse string to substrings (by spaces)
	{
		n = mglFindArg(str);
		if(n<1)	// this is option
		{
			if(str[-n]==';')	opt = str.substr(-n+1);
			if(n<0)	str = str.substr(0,-n);
			break;
		}
		arg.push_back(str.substr(0,n));
		str = mgl_trim_ws(str.substr(n+1));
	}
	// try to find last argument
	if(str[0]!=0 && str[0]!='#' && str[0]!=';')	arg.push_back(str);
	k = arg.size();
	if(k<1) n = 0;
	else
	{
		// fill arguments by its values
		mglArg *a = new mglArg[k];
		FillArg(gr, k, &(arg[0]), a);
		// execute first special (program-flow-control) commands
		if(!skip() && !arg[0].compare(L"stop"))
		{	Stop = true;	delete []a;	return 0;	}
		if(!arg[0].compare(L"func"))
		{	Stop = true;	delete []a;	return 0;	}
		n = FlowExec(gr, arg[0].c_str(),k-1,a);
		if(n)		{	delete []a;	return n-1;	}
		if(skip())	{	delete []a;	return 0;	}
		if(!arg[0].compare(L"load"))
		{
			int n = a[0].type==1?0:1;
			a[0].s.assign(a[0].w.begin(),a[0].w.end());
			if(!n)	mgl_parser_load(this,a[0].s.c_str());
			delete []a;	return n;
		}
		if(!arg[0].compare(L"define"))
		{
			if(k==3)
			{
				DeleteVar(arg[1].c_str());	// force to delete variable with the same name
				mglNum *v=AddNum(arg[1].c_str());
				if(arg[2][0]=='!')	// complex number is added
				{	HADT dd = mglFormulaCalcC(arg[2].substr(1),this, DataList);
					v->d=NAN;	v->c = dd->a[0];	delete dd;	}
				else
				{	HMDT dd = mglFormulaCalc(arg[2],this, DataList);
					v->c = v->d = dd->a[0];	delete dd;	}
			}
			delete []a;	return k==3?0:1;
		}
		if(!arg[0].compare(L"rkstep"))
		{
			int res=1;
			if(k>2 && a[0].type==1 && a[1].type==1)
			{
				std::wstring a1 = arg[1], a2=arg[2];	res = 0;
				if(a1[0]=='\'')	a1 = a1.substr(1,a1.length()-2);
				if(a2[0]=='\'')	a2 = a2.substr(1,a2.length()-2);
				mgl_rk_step_w(this, a1.c_str(), a2.c_str(), (k>3 && a[2].type==2)?a[2].v:1);
			}
			delete []a;	return res;
		}
		if(!arg[0].compare(L"variant"))
		{
			int res=1;
			if(k==2 && a[0].type==2)	{	SetVariant(a[0].v);	res=0;	}
			delete []a;	return res;
		}
		if(!arg[0].compare(L"call"))
		{
			n = 1;
			if(a[0].type==1)
			{
				int na=0;
				a[0].s.assign(a[0].w.begin(),a[0].w.end());
				n=-IsFunc(a[0].w.c_str(),&na);
				if(n && k!=na+2)
				{
					char buf[64];
					snprintf(buf,64,"Bad arguments for %ls: %ld instead of %d\n", a[0].w.c_str(),k-2,na);
					buf[63]=0;	gr->SetWarn(-1,buf);	n = 1;
				}
				else if(n)
				{
					mglFnStack fn;			fn.pos = pos;
					for(int i=0;i<10;i++)	{	fn.par[i] = par[i];	par[i]=L"";	}
					for(int i=1;i<k-1;i++)	AddParam(i,arg[i+1].c_str());
					fn_stack.push_back(fn);	n--;
				}
				else if(AllowFileIO)	// disable external scripts if AllowFileIO=false
				{
					FILE *fp = fopen(a[0].s.c_str(),"rt");
					if(fp)
					{
						register int i;
						mglParser *prs = new mglParser(AllowSetSize);
						prs->DataList.swap(DataList);	prs->NumList.swap(NumList);	prs->Cmd=Cmd;
						for(i=10;i<30;i++)	prs->AddParam(i,par[i].c_str());
						prs->Execute(gr,fp);
						for(i=10;i<30;i++)	AddParam(i,prs->par[i].c_str());
						DataList.swap(prs->DataList);	NumList.swap(prs->NumList);
						prs->Cmd=0;	delete prs;	fclose(fp);
					}
					else	n=1;
				}
			}
			delete []a;	return n;
		}
		if(!arg[0].compare(L"for"))
		{
			if(k<2)	{	delete []a;	return 1;	}
			n = 1;
			char ch = arg[1][0];
			int r = ch-'0';
			if(ch>='a' && ch<='z')	r = 10+ch-'a';
//			int r = int(a[0].v);
			if(arg[1][1]==0 && (r>=0 && r<40))
			{
				if(a[1].type==0)
				{
					n=0;		fval[r] = *(a[1].d);
					fval[r].nx *= fval[r].ny*fval[r].nz;
				}
				else if(a[1].type==2 && a[2].type==2 && a[2].v>a[1].v)
				{
					mreal step = a[3].type==2?a[3].v:1;
					mm = int(step>0 ? (a[2].v-a[1].v)/step : 0);
					if(mm>0)
					{
						n=0;	fval[r].Create(mm+1);
						for(int ii=0;ii<mm+1;ii++)
							fval[r].a[ii] = a[1].v + step*ii;
					}
				}
				if(n==0)
				{
					for(int i=39;i>0;i--)
					{	for_stack[i]=for_stack[i-1];	if_for[i]=if_for[i-1];	}
					for_stack[0] = r+1;		fval[r].nz = pos;	if_for[0]=if_pos;
					wchar_t buf[32];		mglprintf(buf,32,L"%g",fval[r].a[0]);
					AddParam(r, buf);	fval[r].ny = 1;
				}
			}
			delete []a;	return n;
		}
		// alocate new arrays and execute the command itself
		n = PreExec(gr, k, &(arg[0]), a);
		if(n>0)	n--;
		else if(!arg[0].compare(L"setsize") && !AllowSetSize)	n = 2;
		else	n = Exec(gr, arg[0].c_str(),k-1,a, k>1?arg[1]:L"", opt.c_str());
		delete []a;
	}
	// delete temporary data arrays
	for(size_t i=0;i<DataList.size();i++)	if(DataList[i] && DataList[i]->temp)
	{	mglDataA *u=DataList[i];	DataList[i]=0;	delete u;	}
	return n;
}
//-----------------------------------------------------------------------------
// return values: 0 - OK, 1 - wrong arguments, 2 - wrong command, 3 - string too long, 4 -- unclosed string
int mglParser::ParseDat(mglGraph *gr, std::wstring str, mglData &res)
{
	std::wstring arg[32];
	str = mgl_trim_ws(str);
	long n,k=0;
	for(k=0;k<32;k++)	// parse string to substrings (by spaces)
	{
		n = mglFindArg(str);
		if(n<1)	{	if(n<0)	str=str.substr(0,-n);	break;	}
		arg[k] = str.substr(0,n);//	k++;
		str = str.substr(n+1);	str = mgl_trim_ws(str);
	}
	// try to find last argument
	if(!str.empty())	{	arg[k] = str;	k++;	}
	if(k<1) n = 0;
	else
	{	// fill arguments by its values
		mglArg *a = new mglArg[k+1];
		FillArg(gr, k, arg, a+1);	a[0].type=0;	a[0].d=&res;
		// alocate new arrays and execute the command itself
		int i;
		std::string kk;
		const char *id="dsn";
		for(i=0;i<k;i++)
		{
			kk += id[a[i].type];
			a[i].s.assign(a[i].w.begin(),a[i].w.end());
		}
		const mglCommand *rts=FindCommand(arg[0].c_str());
		if(!rts || rts->type!=4)	n = 2;
		else n = rts->exec(gr, k, a, kk.c_str(), 0);
		delete []a;
	}
	return n;
}
//-----------------------------------------------------------------------------
int mglParser::FlowExec(mglGraph *, const std::wstring &com, long m, mglArg *a)
{
	int n=-1;
	if(!ifskip() && !com.compare(L"once"))
	{
		if(a[0].type==2)
		{
			n = 0;
			if(a[0].v)	Skip = !Once;
			else	Skip = Once = false;
		}
		else n = 1;
	}
	else if(!Skip && !com.compare(L"if"))
	{
		int cond;
		if(a[0].type==2)
		{	n = 0;	cond = (a[0].v!=0)?3:0;	}
		else if(a[0].type==0)
		{
			n = 0;	a[1].s.assign(a[1].w.begin(),a[1].w.end());
			cond = a[0].d->FindAny((m>1 && a[1].type==1) ? a[1].s.c_str():"u")?3:0;
		}
		else n = 1;
		if(n==0)
		{	if_stack[if_pos] = cond;	if_pos = if_pos<39 ? if_pos+1 : 39;	}
	}
	else if(!Skip && !com.compare(L"endif"))
	{	if_pos = if_pos>0 ? if_pos-1 : 0;	n = 0;	}
	else if(!Skip && !com.compare(L"else"))
	{
		if(if_pos>0)
		{	n=0; if_stack[if_pos-1] = (if_stack[if_pos-1]&2)?2:3;	}
		else n = 1;
	}
	else if(!Skip && !com.compare(L"elseif"))
	{
		int cond;
		if(if_pos<1 || m<1)	n = 1;
		else if(if_stack[if_pos-1]&2)	{	n = 0;	cond = 2;	}
		else if(a[0].type==2)
		{	n = 0;	cond = (a[0].v!=0)?3:0;	}
		else if(a[0].type==0)
		{
			n = 0;	a[1].s.assign(a[1].w.begin(),a[1].w.end());
			cond = a[0].d->FindAny((m>1 && a[1].type==1) ? a[1].s.c_str():"u")?3:0;
		}
		else n = 1;
		if(n==0)	if_stack[if_pos-1] = cond;
	}
	else if(!ifskip() && !Skip && !com.compare(L"break"))
	{
		if(if_pos==if_for[0])	if_pos = if_pos>0 ? if_pos-1 : 0;
	}
	else if(!skip() && !com.compare(L"return"))
	{
		if(fn_stack.size()<1)	return 2;
		const mglFnStack &fn=fn_stack.back();
		for(int i=0;i<10;i++)	par[i]=fn.par[i];
		n = -fn.pos-1;	fn_stack.pop_back();
	}
	else if(!ifskip() && !Skip && !com.compare(L"next"))
	{
		if(if_pos==if_for[0])	if_pos = if_pos>0 ? if_pos-1 : 0;
		int r = for_stack[0]-1;
		n = for_stack[0] ? 0:1;
		if(for_stack[0])
		{
			if(fval[r].ny<fval[r].nx && !for_br)
			{
				wchar_t buf[32];		mglprintf(buf,32,L"%g",fval[r].a[fval[r].ny]);
				AddParam(r, buf);	fval[r].ny += 1;
				n = -fval[r].nz-1;
			}
			else
			{
				for(int i=0;i<39;i++)
				{	for_stack[i]=for_stack[i+1];	if_for[i]=if_for[i+1];	}
				for_stack[39] = 0;	for_br=false;
			}
		}
	}
	else if(!ifskip() && !Skip && !com.compare(L"continue"))
	{
		if(if_pos==if_for[0])	if_pos = if_pos>0 ? if_pos-1 : 0;
		int r = for_stack[0]-1;
		n = for_stack[0] ? 0:1;
		if(for_stack[0])
		{
			if(fval[r].ny<fval[r].nx)
			{
				wchar_t buf[32];		mglprintf(buf,32,L"%g",fval[r].a[fval[r].ny]);
				AddParam(r, buf);	fval[r].ny += 1;
				n = -fval[r].nz-1;
			}
			else	for_br = true;
		}
	}
	return n+1;
}
//-----------------------------------------------------------------------------
void mglParser::Execute(mglGraph *gr, FILE *fp, bool print)
{
	if(gr==0 || fp==0)	return;
	std::wstring str;
	wchar_t ch;
	while(!feof(fp) && size_t(ch=fgetwc(fp))!=WEOF)	str.push_back(ch);
	Execute(gr,str.c_str());
	if(print)	printf("%s\n",gr->Message());
}
//-----------------------------------------------------------------------------
void mglParser::Execute(mglGraph *gr, int n, const wchar_t **text)
{
	if(n<1 || text==0)	return;
	long res=0;
	char buf[64];
	for_br=Skip=false;	if_pos=0;	ScanFunc(0);	fn_stack.clear();
	for(long i=0;i<n;i++)	ScanFunc(text[i]);
	for(long i=0;i<n;i++)
	{
		gr->SetWarn(-1, "");
		gr->SetObjId(i+1);
		long r = Parse(gr,text[i],i+1);
		if(r<0)	{	i = -r-2;	continue;	}
		if(r==1)		snprintf(buf,64,"\nWrong argument(s) in line %ld\n", i+1);
		else if(r==2)	snprintf(buf,64,"\nWrong command in line %ld\n", i+1);
		else if(r==3)	snprintf(buf,64,"\nString too long in line %ld\n", i+1);
		else if(r==4)	snprintf(buf,64,"\nUnbalanced ' in line %ld\n", i+1);
		else if(r==5)	snprintf(buf,64,"\nChange temporary data in line %ld\n", i+1);
		else if(gr->GetWarn()>0)	snprintf(buf,64," in line %ld\n", i+1);
		else *buf=0;
		buf[63] = 0;
		if(*buf)	gr->SetWarn(-2,buf);
		if(r>0 && r<5)	res=r;
	}
	int code[]={mglScrArg,	mglScrCmd,	mglScrLong,	mglScrStr, mglScrTemp};
	if(res>0)	gr->SetWarn(code[res-1],"MGL Parser");
}
//-----------------------------------------------------------------------------
void mglParser::Execute(mglGraph *gr, const wchar_t *text)
{
	size_t s = mgl_wcslen(text)+1, n=1;
	wchar_t *wcs = new wchar_t[s];
	const wchar_t **str;
	for(size_t i=0;i<s;i++)	if(text[i]=='\n')	n++;
	str = (const wchar_t **)malloc(n*sizeof(wchar_t *));
	memcpy(wcs, text, s*sizeof(wchar_t));
	str[0] = wcs;	n=1;
	long next=0;
	Stop = false;
	for(size_t i=0;i<s;i++)
	{
		if(text[i]=='\\')	next = i;
		else if(text[i]>' ')next = 0;
		if(text[i]=='\n')
		{	// if string need to be continued then I but ' ' instead of 0x0 and
			// pointer next string to 0x0. Last one for keeping number of strings.
			if(next)
			{	for(size_t ii=next;ii<=i;ii++)	wcs[ii]='\b';	str[n] = wcs+s-1;	next=0;	}
			else
			{	wcs[i]=0;	str[n] = wcs+i+1;	}
			n++;
		}
	}
	Execute(gr, n, str);
	delete []wcs;	free(str);
}
//-----------------------------------------------------------------------------
void mglParser::Execute(mglGraph *gr, const char *text)
{
	MGL_TO_WCS(text, Execute(gr, wcs));
}
//-----------------------------------------------------------------------------
void mglParser::DeleteVar(const char *name)
{
	MGL_TO_WCS(name,DeleteVar(wcs));
}
//-----------------------------------------------------------------------------
void mglParser::DeleteVar(const wchar_t *name)
{
	for(size_t i=0;i<DataList.size();i++)	if(DataList[i] && DataList[i]->s==name)
	{	mglDataA *u=DataList[i];	DataList[i]=0;	delete u;	}
}
//-----------------------------------------------------------------------------
void mglParser::AddCommand(mglCommand *cmd, int mc)
{
	int i, mp;
	if(mc<1)
	{	for(i=0;cmd[i].name[0];i++){};	mc = i;	}
	// determine the number of symbols
	for(i=0;Cmd[i].name[0];i++){};	mp = i;
	mglCommand *buf = new mglCommand[mp+mc+1];
	memcpy(buf, cmd, mc*sizeof(mglCommand));
	memcpy(buf+mc, Cmd, (mp+1)*sizeof(mglCommand));
	qsort(buf, mp+mc, sizeof(mglCommand), mgl_cmd_cmp);
#pragma omp critical(cmd_parser)
	{	if(Cmd!=mgls_base_cmd)   delete []Cmd;	Cmd = buf;	}
}
//-----------------------------------------------------------------------------
HMPR MGL_EXPORT mgl_create_parser()		{	return new mglParser;	}
void MGL_EXPORT mgl_delete_parser(HMPR p)	{	delete p;	}
void MGL_EXPORT mgl_parser_add_param(HMPR p, int id, const char *str)			{	p->AddParam(id,str);	}
void MGL_EXPORT mgl_parser_add_paramw(HMPR p, int id, const wchar_t *str)		{	p->AddParam(id,str);	}
MGL_EXPORT mglDataA *mgl_parser_add_var(HMPR p, const char *name)	{	return p->AddVar(name);	}
MGL_EXPORT mglDataA *mgl_parser_find_var(HMPR p, const char *name)	{	return p->FindVar(name);}
void MGL_EXPORT mgl_parser_del_var(HMPR p, const char *name)	{	p->DeleteVar(name);	}
MGL_EXPORT mglDataA *mgl_parser_add_varw(HMPR p, const wchar_t *name)	{	return p->AddVar(name);	}
MGL_EXPORT mglDataA *mgl_parser_find_varw(HMPR p, const wchar_t *name)	{	return p->FindVar(name);}
void MGL_EXPORT mgl_parser_del_varw(HMPR p, const wchar_t *name)	{	p->DeleteVar(name);	}
int MGL_EXPORT mgl_parse_line(HMGL gr, HMPR p, const char *str, int pos)
{	return p->Parse(gr, str, pos);	}
int MGL_EXPORT mgl_parse_linew(HMGL gr, HMPR p, const wchar_t *str, int pos)
{	return p->Parse(gr, str, pos);	}
void MGL_EXPORT mgl_parse_text(HMGL gr, HMPR p, const char *str)
{	p->Execute(gr, str);	}
void MGL_EXPORT mgl_parse_textw(HMGL gr, HMPR p, const wchar_t *str)
{	p->Execute(gr, str);	}
void MGL_EXPORT mgl_parse_file(HMGL gr, HMPR p, FILE *fp, int print)
{	p->Execute(gr,fp,print);	}
void MGL_EXPORT mgl_parser_restore_once(HMPR p)	{	p->RestoreOnce();	}
void MGL_EXPORT mgl_parser_stop(HMPR p)	{	p->Stop = true;		}
void MGL_EXPORT mgl_parser_allow_setsize(HMPR p, int a)	{	p->AllowSetSize= a;	}
void MGL_EXPORT mgl_parser_allow_file_io(HMPR p, int a)	{	p->AllowFileIO = a;	}
void MGL_EXPORT mgl_parser_allow_dll_call(HMPR p, int a){	p->AllowDllCall = a;	}
//-----------------------------------------------------------------------------
#define _PR_	((mglParser *)(*p))
uintptr_t MGL_EXPORT mgl_create_parser_()	{	return uintptr_t(new mglParser);	}
void MGL_EXPORT mgl_delete_parser_(uintptr_t* p)	{	delete _PR_;	}
void MGL_EXPORT mgl_parser_add_param_(uintptr_t* p, int *id, const char *str, int l)
{	char *s=new char[l+1];		memcpy(s,str,l);	s[l]=0;
	_PR_->AddParam(*id, s);		delete []s;	}
/*===!!! NOTE !!! You must not delete obtained data arrays !!!===============*/
uintptr_t MGL_EXPORT mgl_parser_add_var_(uintptr_t* p, const char *name, int l)
{	char *s=new char[l+1];		memcpy(s,name,l);	s[l]=0;
	mglDataA *v=_PR_->AddVar(s);	delete []s;	return uintptr_t(v);	}
/*===!!! NOTE !!! You must not delete obtained data arrays !!!===============*/
uintptr_t MGL_EXPORT mgl_parser_find_var_(uintptr_t* p, const char *name, int l)
{	char *s=new char[l+1];		memcpy(s,name,l);	s[l]=0;
	mglDataA *v=_PR_->FindVar(s);	delete []s;	return uintptr_t(v);	}
void MGL_EXPORT mgl_parser_del_var_(uintptr_t* p, const char *name, int l)
{	char *s=new char[l+1];		memcpy(s,name,l);	s[l]=0;
	_PR_->DeleteVar(s);	delete []s;	}
int MGL_EXPORT mgl_parse_line_(uintptr_t* gr, uintptr_t* p, const char *str, int *pos, int l)
{	char *s=new char[l+1];		memcpy(s,str,l);	s[l]=0;
	int r = _PR_->Parse(_GR_, s, *pos);	delete []s;	return r;	}
void MGL_EXPORT mgl_parse_text_(uintptr_t* gr, uintptr_t* p, const char *str, int l)
{	char *s=new char[l+1];		memcpy(s,str,l);	s[l]=0;
	_PR_->Execute(_GR_, s);	delete []s;	}
void MGL_EXPORT mgl_parser_restore_once_(uintptr_t* p)	{	_PR_->RestoreOnce();	}
void MGL_EXPORT mgl_parser_allow_setsize_(uintptr_t* p, int *a)	{	_PR_->AllowSetSize= *a;	}
void MGL_EXPORT mgl_parser_allow_file_io_(uintptr_t* p, int *a)	{	_PR_->AllowFileIO = *a;	}
void MGL_EXPORT mgl_parser_allow_dll_call_(uintptr_t* p, int *a){	_PR_->AllowDllCall= *a;	}
void MGL_EXPORT mgl_parser_stop_(uintptr_t* p)	{	_PR_->Stop = true;	}
//-----------------------------------------------------------------------------
long MGL_EXPORT mgl_use_parser(HMPR pr, int inc)
{	pr->InUse+=inc;	return pr->InUse;	}
long MGL_EXPORT mgl_use_parser_(uintptr_t *p, int *inc)
{	_PR_->InUse+=*inc;	return _PR_->InUse;	}
//---------------------------------------------------------------------------
MGL_EXPORT mglDataA *mgl_parser_get_var(HMPR p, unsigned long id)
{	return id<p->DataList.size()?p->DataList[id]:0;	}
uintptr_t MGL_EXPORT mgl_parser_get_var_(uintptr_t* p, unsigned long *id)
{	return uintptr_t(mgl_parser_get_var(_PR_,*id));	}
long MGL_EXPORT mgl_parser_num_var(HMPR p)
{	return p->DataList.size();	}
long MGL_EXPORT mgl_parser_num_var_(uintptr_t* p)
{	return mgl_parser_num_var(_PR_);	}
//---------------------------------------------------------------------------
int MGL_EXPORT mgl_parser_cmd_type(HMPR pr, const char *name)
{
	const mglCommand *cmd = pr->FindCommand(name);
	return cmd ? cmd->type + 1 : 0;
}
int MGL_EXPORT mgl_parser_cmd_type_(uintptr_t* p, const char *str, int l)
{	char *s=new char[l+1];	memcpy(s,str,l);	s[l]=0;
	l = mgl_parser_cmd_type(_PR_, s);	delete []s;	return l;	}
//---------------------------------------------------------------------------
MGL_EXPORT const char *mgl_parser_cmd_desc(HMPR pr, const char *name)
{
	const mglCommand *cmd = pr->FindCommand(name);
	return cmd ? cmd->desc : 0;
}
MGL_EXPORT const char *mgl_parser_cmd_frmt(HMPR pr, const char *name)
{
	const mglCommand *cmd = pr->FindCommand(name);
	return cmd ? cmd->form : 0;
}
//---------------------------------------------------------------------------
MGL_EXPORT const char *mgl_parser_cmd_name(HMPR pr, long id)
{	return (id<mgl_parser_cmd_num(pr) && id>=0) ? pr->Cmd[id].name:"";	}
long MGL_EXPORT mgl_parser_cmd_num(HMPR pr)
{	register long i=0;	while(pr->Cmd[i].name[0])	i++; 	return i;	}
//---------------------------------------------------------------------------
HMDT MGL_EXPORT mgl_parser_calc(HMPR pr, const char *formula)
{	HMDT d=0;	MGL_TO_WCS(formula,d = mgl_parser_calcw(pr,wcs));	return d;	}
HMDT MGL_EXPORT mgl_parser_calcw(HMPR pr, const wchar_t *formula)
{	return mglFormulaCalc(formula,pr, pr->DataList);	}
uintptr_t MGL_EXPORT mgl_parser_calc_(uintptr_t *p, const char *str,int l)
{	char *s=new char[l+1];	memcpy(s,str,l);	s[l]=0;
	uintptr_t d = (uintptr_t)mgl_parser_calc(_PR_, s);	delete []s;	return d;	}
//---------------------------------------------------------------------------
HADT MGL_EXPORT mgl_parser_calc_complex(HMPR pr, const char *formula)
{	HADT d=0;	MGL_TO_WCS(formula,d = mgl_parser_calc_complexw(pr,wcs));	return d;	}
HADT MGL_EXPORT mgl_parser_calc_complexw(HMPR pr, const wchar_t *formula)
{	return mglFormulaCalcC(formula,pr, pr->DataList);	}
uintptr_t MGL_EXPORT mgl_parser_calc_complex_(uintptr_t *p, const char *str,int l)
{	char *s=new char[l+1];	memcpy(s,str,l);	s[l]=0;
	uintptr_t d = (uintptr_t)mgl_parser_calc_complex(_PR_, s);	delete []s;	return d;	}
//---------------------------------------------------------------------------
void MGL_EXPORT mgl_parser_del_all(HMPR p)	{	p->DeleteAll();	}
void MGL_EXPORT mgl_parser_del_all_(uintptr_t *p)	{	_PR_->DeleteAll();	}
//---------------------------------------------------------------------------
void MGL_EXPORT mgl_parser_load(HMPR pr, const char *so_name)
{
	if(!pr->AllowDllCall)	return;
#if MGL_HAVE_LTDL
	lt_dlhandle so = lt_dlopen(so_name);
	if(!so)	return;
	const mglCommand *cmd = (const mglCommand *)lt_dlsym(so,"mgl_cmd_extra");
	if(!cmd)	return;

	int i, mp, mc, exist=true;
	// determine the number of symbols
	for(i=0;pr->Cmd[i].name[0];i++){};	mp = i;
	for(i=0;cmd[i].name[0];i++)
		if(!pr->FindCommand(cmd[i].name))	exist=false;
	if(exist)	{	lt_dlclose(so);	return;	}	// all commands already presents
	else	pr->DllOpened.push_back(so);
	mc = i;
	mglCommand *buf = new mglCommand[mp+mc+1];
	memcpy(buf, pr->Cmd, mp*sizeof(mglCommand));
	memcpy(buf+mp, cmd, (mc+1)*sizeof(mglCommand));
	qsort(buf, mp+mc, sizeof(mglCommand), mgl_cmd_cmp);
	if(pr->Cmd!=mgls_base_cmd)	delete []pr->Cmd;
	pr->Cmd = buf;
#endif
}
void MGL_EXPORT mgl_parser_load_(uintptr_t *p, const char *dll_name,int l)
{	char *s=new char[l+1];	memcpy(s,dll_name,l);	s[l]=0;
	mgl_parser_load(_PR_, s);	delete []s;	}
//---------------------------------------------------------------------------
struct mglRKdat
{
	mglDataA *v;
	std::wstring e;
	bool cmplx;
	mglDataC cin,c1,c2,c3,c4, *cc;
	mglData  din,d1,d2,d3,d4, *dd;
	mglRKdat(mglDataA *var, std::wstring &eq):v(var), e(eq)
	{	cmplx = dynamic_cast<mglDataC*>(var);	cc=0;	dd=0;	}
	void allocate()
	{
		if(cmplx)
		{	cc = dynamic_cast<mglDataC*>(v);	cin.Set(v);	}
		else
		{	dd = dynamic_cast<mglData*>(v);		din.Set(v);	}
	}
};
void MGL_EXPORT mgl_rk_step_w(HMPR pr, const wchar_t *Eqs, const wchar_t *Vars, mreal dt)
{
	const std::wstring eqs(Eqs);
	const std::wstring vars(Vars);
	std::vector<mglRKdat> rkv;
	size_t iv=0,jv=0,ie=0,je=0;
	while(1)
	{
		iv = vars.find(';',jv);	ie = eqs.find(';',je);
		mglDataA *vv=mgl_parser_find_varw(pr,vars.substr(jv,iv-jv).c_str());
		std::wstring eq = eqs.substr(je,ie-je).c_str();
		if(vv)	rkv.push_back(mglRKdat(vv, eq ));
		jv = iv+1;	je = ie+1;
		if(iv==std::wstring::npos || ie==std::wstring::npos)	break;
	}
	for(size_t i=0;i<rkv.size();i++)	rkv[i].allocate();
	mreal hh = dt/2;
	for(size_t i=0;i<rkv.size();i++)
	{
		mglRKdat &rk = rkv[i];
		if(rk.cmplx)	rk.c1.Move(mglFormulaCalcC(rk.e, pr, pr->DataList));
		else	rk.d1.Move(mglFormulaCalc(rk.e, pr, pr->DataList));
	}
	for(size_t i=0;i<rkv.size();i++)
	{
		mglRKdat &rk = rkv[i];
		if(rk.cc)
		{
			long n = rk.cc->GetNN();	dual a = hh*rk.c1.a[0];
			if(rk.c1.GetNN()==n)
#pragma omp parallel for
				for(long j=0;j<n;j++)	rk.cc->a[j] = rk.cin.a[j] + hh*rk.c1.a[j];
			else
#pragma omp parallel for
				for(long j=0;j<n;j++)	rk.cc->a[j] = rk.cin.a[j] + a;
		}
		if(rk.dd)
		{
			long n = rk.dd->GetNN();	mreal a = hh*rk.d1.a[0];
			if(rk.d1.GetNN()==n)
#pragma omp parallel for
				for(long j=0;j<n;j++)	rk.dd->a[j] = rk.din.a[j] + hh*rk.d1.a[j];
			else
#pragma omp parallel for
				for(long j=0;j<n;j++)	rk.dd->a[j] = rk.din.a[j] + a;
		}
	}

	for(size_t i=0;i<rkv.size();i++)
	{
		mglRKdat &rk = rkv[i];
		if(rk.cmplx)	rk.c2.Move(mglFormulaCalcC(rk.e, pr, pr->DataList));
		else	rk.d2.Move(mglFormulaCalc(rk.e, pr, pr->DataList));
	}
	for(size_t i=0;i<rkv.size();i++)
	{
		mglRKdat &rk = rkv[i];
		if(rk.cc)
		{
			long n = rk.cc->GetNN();	dual a = hh*rk.c2.a[0];
			if(rk.c2.GetNN()==n)
#pragma omp parallel for
				for(long j=0;j<n;j++)	rk.cc->a[j] = rk.cin.a[j] + hh*rk.c2.a[j];
			else
#pragma omp parallel for
				for(long j=0;j<n;j++)	rk.cc->a[j] = rk.cin.a[j] + a;
		}
		if(rk.dd)
		{
			long n = rk.dd->GetNN();	mreal a = hh*rk.d2.a[0];
			if(rk.d2.GetNN()==n)
#pragma omp parallel for
				for(long j=0;j<n;j++)	rk.dd->a[j] = rk.din.a[j] + hh*rk.d2.a[j];
			else
#pragma omp parallel for
				for(long j=0;j<n;j++)	rk.dd->a[j] = rk.din.a[j] + a;
		}
	}

	for(size_t i=0;i<rkv.size();i++)
	{
		mglRKdat &rk = rkv[i];
		if(rk.cmplx)	rk.c3.Move(mglFormulaCalcC(rk.e, pr, pr->DataList));
		else	rk.d3.Move(mglFormulaCalc(rk.e, pr, pr->DataList));
	}
	for(size_t i=0;i<rkv.size();i++)
	{
		mglRKdat &rk = rkv[i];
		if(rk.cc)
		{
			long n = rk.cc->GetNN();	dual a = dt*rk.c3.a[0];
			if(rk.c3.GetNN()==n)
#pragma omp parallel for
				for(long j=0;j<n;j++)	rk.cc->a[j] = rk.cin.a[j] + dt*rk.c3.a[j];
			else
#pragma omp parallel for
				for(long j=0;j<n;j++)	rk.cc->a[j] = rk.cin.a[j] + a;
		}
		if(rk.dd)
		{
			long n = rk.dd->GetNN();	mreal a = dt*rk.d3.a[0];
			if(rk.d3.GetNN()==n)
#pragma omp parallel for
				for(long j=0;j<n;j++)	rk.dd->a[j] = rk.din.a[j] + dt*rk.d3.a[j];
			else
#pragma omp parallel for
				for(long j=0;j<n;j++)	rk.dd->a[j] = rk.din.a[j] + a;
		}
	}

	for(size_t i=0;i<rkv.size();i++)
	{
		mglRKdat &rk = rkv[i];
		if(rk.cmplx)	rk.c4.Move(mglFormulaCalcC(rk.e, pr, pr->DataList));
		else	rk.d4.Move(mglFormulaCalc(rk.e, pr, pr->DataList));
	}
	for(size_t i=0;i<rkv.size();i++)
	{
		mglRKdat &rk = rkv[i];
		if(rk.cc)
		{
			long n = rk.cc->GetNN();
			dual a = (rk.c1.a[0]+rk.c2.a[0]+mreal(2)*(rk.c3.a[0]+rk.c4.a[0]))*(dt/6);
			if(rk.c1.GetNN()==n)
#pragma omp parallel for
				for(long j=0;j<n;j++)	rk.cc->a[j] = rk.cin.a[j] + (rk.c1.a[j]+rk.c2.a[j]+mreal(2)*(rk.c3.a[j]+rk.c4.a[j]))*(dt/6);
			else
#pragma omp parallel for
				for(long j=0;j<n;j++)	rk.cc->a[j] = rk.cin.a[j] + a;
		}
		if(rk.dd)
		{
			long n = rk.dd->GetNN();
			mreal a = (rk.d1.a[0]+rk.d2.a[0]+2*(rk.d3.a[0]+rk.d4.a[0]))*(dt/6);
			if(rk.d1.GetNN()==n)
#pragma omp parallel for
				for(long j=0;j<n;j++)	rk.dd->a[j] = rk.din.a[j] + (rk.d1.a[j]+rk.d2.a[j]+2*(rk.d3.a[j]+rk.d4.a[j]))*(dt/6);
			else
#pragma omp parallel for
				for(long j=0;j<n;j++)	rk.dd->a[j] = rk.din.a[j] + a;
		}
	}
}
void MGL_EXPORT mgl_rk_step(HMPR pr, const char *Eqs, const char *Vars, mreal dt)
{
	if(Eqs && *Eqs && Vars && *Vars)
	{
		size_t s=mbstowcs(0,Eqs,0), w=mbstowcs(0,Vars,0);
		wchar_t *eqs=new wchar_t[s+1];	mbstowcs(eqs,Eqs ,s);	eqs[s]=0;
		wchar_t *wcs=new wchar_t[s+1];	mbstowcs(wcs,Vars,s);	wcs[w]=0;
		mgl_rk_step_w(pr,eqs,wcs,dt);	delete []wcs;	delete []eqs;
	}
}
void MGL_EXPORT mgl_rk_step_(uintptr_t *p, const char *eqs, const char *vars, double *dt,int l,int m)
{	char *e=new char[l+1];	memcpy(e,eqs,l);	e[l]=0;
	char *s=new char[m+1];	memcpy(s,vars,m);	s[m]=0;
	mgl_rk_step(_PR_,e,s,*dt);	delete []e;	delete []s;	}
//---------------------------------------------------------------------------
void MGL_EXPORT mgl_parser_variant(HMPR p, int var)	{	p->SetVariant(var);	}
void MGL_EXPORT mgl_parser_variant_(uintptr_t *p, int *var)	{	mgl_parser_variant(_PR_,*var);	}
//---------------------------------------------------------------------------
