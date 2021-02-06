{*!
   bmp -- babel macro processor
   Copyright (C) 2009-2011 ZTEX GmbH.
   http://www.ztex.de

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 3 as
   published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, see http://www.gnu.org/licenses/.
!*}

{ todo:
Ausgabe als Eingabe (mehrfache Bearbeitung)
}

{ codes
    bc_pm : string
    bc_ob : char
    bc_cb : char
    bc_pa : char

meta macros:
  define
    define(sym)         define symb
    define(m1)(r1)      define macro m1 with replacement r1
    define(m2)(p2)(r2)  define macro m2 with parmaters p2 and replacement r1

  udefine
  uadefine

  include

  ifdef/ifndef
  elifdef/elfndef
  ifeq/ifneq		macros are expanded in the compared strings, but no meta macros, except of \\noexapnd which disbales the expansion
  elifeq/elifneq	
  else
  endif
  
  nolf
  
  error
  warning

  disablaeout
  enableout

  ignorecase
  exactcase

  noexpand

  disablelineinfo 	disables line info for next define
  
}

{$mode objfpc}

{$define USE_SHORTSTRINGS}  // shortstrings are two times faster
{ $define ENABLE_DEBUGOUT}    

uses dos,strings,textbuf,bmpsys,sysutils;

{ ******************************************************************** }
{ ****** constants *************************************************** }
{ ******************************************************************** }
{$ifdef USE_SHORTSTRINGS}
type bmpstring = shortstring;
{$else}
type bmpstring = ansistring;
{$endif}

const rbufmax     = 65535;  { must be 2^n-1 ! }
      maxmacros   = 16383;  { max amount of macros }
      maxifs      = 4095;   { max input files }
      maxbs       = 255;    { brace stack }
      maxoutfiles = 256;    { max input files }
      maxparams   = 35;     { max amount of macro parameters }
      
      bc_pm : bmpstring = '//';    { default for pascal mode }
      bc_ob : char = '[';          { default }
      bc_cb : char = ']';          { default }
      bc_pa : char = '#';          { default for pascal mode } 
      bc_lf = #10;
      bp_icase : boolean = false;  { ignore case }
      
{$ifdef WINDOWS}
      dirsep = '\';		   { directory seperator }
{$else} 
      dirsep = '/';                { directory seperator }
{$endif}    

      enable_hints : boolean = true;    { enable hints }
      printdefs    : boolean = false;   { print definitions }
{$ifdef ENABLE_DEBUGOUT}
      debugoutput  : boolean = false;   { debug mode }
{$endif}    
      paramids : array[0..maxparams] of char = ('0','1','2','3','4','5','6','7','8','9',   { parameters (in that order!) }
                                                'a','b','c','d','e','f','g','h','i','j',
                                                'k','l','m','n','o','p','q','r','s','t',
                                                'u','v','w','x','y','z');

{ ****** modes ******************************************************* }
const bm_plain   = 0;
      bm_neplain = 1;         { no expand plain }

      bm_comm = 10;           { comment }

      bm_em = 100;            { (macro) expanding mode }
      bm_em_p0 = 101;         { zero paramter (#0) }
      bm_em_pz = bm_em_p0+maxparams;     { last paramter }

      bm_pm = 1000;           { programming mode }

      bm_def = 1100;          { define mode }
      bm_def_sob2 = 1101;     { searching 2nd opening bracket }
      bm_def_sob3 = 1102;     { searching 3rd opening bracket }
      bm_def_scb1 = 1103;     { searching 1st closing bracket }
      bm_def_scb2 = 1104;     { searching 2nd closing bracket }
      bm_def_scb3 = 1105;     { searching 3rd closing bracket }
      bm_def_scbu = 1120;     { searching udefine closing bracket }
      bm_def_scba = 1130;     { searching uadefine closing bracket }
      
      bm_if        = 1200;     { if mode }
      bm_if_scb    = 1201;     { searching closing bracket }
      bm_ifn_scb   = 1202;     { searching closing bracket }
      bm_if_not    = 1203;     { marks the ignored part, searching for else }

      bm_ifeq_sob      = 1210;
      bm_ifneq_sob     = 1211;
      bm_ifeq_scb1     = 1212;
      bm_ifeq_scb2     = 1213;
      bm_ifneq_scb1    = 1214;
      bm_ifneq_scb2    = 1215;
      bm_ifeq_scb1_ne  = 1216;
      bm_ifeq_scb2_ne  = 1217;
      bm_ifneq_scb1_ne = 1218;
      bm_ifneq_scb2_ne = 1219;
      bm_ifb           = 1220;
      
      bm_inc = 1300;          { include mode }
      bm_inc_scb = 1301;      { searching closing bracket }

      bm_noexpand = 1400;     { noexpand mode }
      bm_noexpand_scb = 1401; { searching closing bracket }
      
      bm_err = 1500;          { error }
      bm_err_scb = 1501;      { searching closing bracket }

      bm_warn = 1510;         { warning }
      bm_warn_scb = 1511;     { searching closing bracket }
      

      bm_outfile_a = 10000;   { outfile number }
      bm_outfile_z = bm_outfile_a+maxoutfiles;

      bm_invalid = -1;        { invalid character }


{ ******************************************************************** }
{ ****** helper functions ******************************************** }
{ ******************************************************************** }
function upcase(ch:char):char;  { no Umlaute }
begin
if (ch>='a') and (ch<='z') 
  then result:=chr(ord(ch)-32)
  else result:=ch;
end;

function upstr(const s:bmpstring):bmpstring;
var i  : longint;
begin
setlength(upstr, length(s));
for i:=1 to length(s) do
  begin
  if (s[i]>='a') and (s[i]<='z') 
     then upstr[i]:=chr(ord(s[i])-32)
     else upstr[i]:=s[i];
  end;
end;

procedure upstr2(var s:shortstring);
var i : longint;
begin
for i:=1 to length(s) do
  if (s[i]>='a') and (s[i]<='z') then byte(s[i])-=32;
end;  


{ ******************************************************************** }
{ ****** rbufstr ***************************************************** }
{ ******************************************************************** }
type RBufStr = record              { string within ring,  }
       rbuf      : pchar;          { buffer }
       pt        : longint;        { where the ring starts }
       length    : longint;        { length of the string }
       end;

function rbuf(const rb:rbufstr):bmpstring;
var i,j,k : longint;
begin
k:=rb.length;
{$ifdef USE_SHORTSTRINGS}
if k>255 then k:=255;
{$endif}
j:=rb.pt;
setlength(result,k);
for i:=1 to k do
 begin
 result[i]:=rb.rbuf[j and rbufmax];
 j+=1;
 end;
end;

function cmp_rbuf(const rb:rbufstr; const s:bmpstring):boolean;
var i,j : longint;
begin
result:=false;
if rb.length<>length(s) then exit;
j:=rb.pt;
for i:=1 to rb.length do
  begin
  if rb.rbuf[j  and rbufmax]<>s[i] then exit;
  j+=1;
  end;
result:=true;
end;

function cmpcase_rbuf(const rb:rbufstr; const s:bmpstring):boolean;
var i,j : longint;
begin
result:=false;
if rb.length<>length(s) then exit;
j:=rb.pt;
for i:=1 to rb.length do
  begin
  if lowercase(rb.rbuf[j and rbufmax])<>lowercase(s[i]) then exit;
  j+=1;
  end;
result:=true;
end;


{ ******************************************************************** }
{ ****** TBabelMacro ************************************************* }
{ ******************************************************************** }
type PBabelMacro=^TBabelMacro;
     TBabelMacro=record
       name       : bmpstring;              { macro name }
       mmatch_s   : array[0..1] of dword;   { quick match string (normal, upercase) }
       mmatch_m   : dword;                  { quick match mask }
       pn         : longint;                { number of parameters }
       rsize      : longint;                { replacement size }
       r          : ^char;                  { replacement }
       li         : ^dword;                 { lnie info }
       quick      : boolean;                { quick comparison }
       enabled    : boolean;		    { set to false to avoid recursions }
       paramsep   : array[0..maxparams] of ansistring;    { parameter seperators }
       end;

{ ****** freemacro *************************************************** }
procedure freemacro(var mac:PBabelMacro);
var i : longint;
begin
if (mac^.r<>nil) and (mac^.rsize>0) then freemem(mac^.r, mac^.rsize);
if (mac^.li<>nil) and (mac^.rsize>0) then freemem(mac^.li, mac^.rsize*4);
{$ifndef USE_SHORTSTRINGS}
mac^.name:='';
{$endif}  
for i:=1 to maxparams do
  mac^.paramsep[i]:='';
freemem(mac,sizeof(TBabelMacro));
mac:=nil;
end;

{ ******************************************************************** }
{ ****** CMacroBuf *************************************************** }
{ ******************************************************************** }
const macrobuf_size = 1024;
type CMacroBuf = class(CTextBuf)
       constructor create(var tb:CTextBuf; m:PBabelMacro; clone:boolean);
       constructor insert(var mb:CMacroBuf);
       destructor destroy; override;
      private
       mac : PBabelMacro;
       end;

{ ****** create ****************************************************** }
constructor CMacroBuf.create(var tb:CTextBuf; m:PBabelMacro; clone:boolean);
begin
if clone 
    then inherited create(tb, m^.r, m^.li, m^.rsize)
    else inherited create(tb, macrobuf_size);
m^.enabled:=false;
mac:=m;
end;

constructor CMacroBuf.insert(var mb:CMacroBuf);
var db : CTextBuf;
begin
inherited create(db, macrobuf_size);
last:=mb.last;
mb.last:=self;
mac:=mb.mac;
mb.mac:=nil;
mb:=self;
end;
	    
{ ****** destroy ***************************************************** }
destructor CMacroBuf.destroy;
begin
if mac<>nil then mac^.enabled:=true;
end;


{ ******************************************************************** }
{ ****** CMainBmp **************************************************** }
{ ******************************************************************** }
type CMainBmp=class
       buf        : CTextBuf;
       macros     : array[0..maxmacros] of PBabelMacro;

       constructor create;
       destructor destroy; override;
       procedure run(const mf:ansistring; var fo:text);
       procedure initsymb(const s:bmpstring);

      private

       rbuf           : array[0..rbufmax] of char;
       bufm           : array[0..rbufmax] of smallint;
       li             : array[0..rbufmax] of dword;
       rbp,lastmacro  : longint;  { buffer pointer }
       lineInfo       : dword;

       procedure error_int(const t:ansistring);
       procedure error(const s:ansistring);
       procedure faterror(const s:ansistring);
       procedure warning(const s:ansistring);
       procedure hint(const s:ansistring);

       function copylaststr(l,mode:longint):rbufstr;         { last l continuous chars with mode m }
       function copylaststr(l:longint):rbufstr;              { last l chars }
       function copylaststr_br(l,mode:longint):bmpstring;    { last l chars with mode m, breaks allowed }

       function matchstr(const s:bmpstring;mode:longint):boolean;
       function matchstr(const s:bmpstring):boolean;
       function matchstr(ic:boolean;const s:bmpstring;mode:longint):boolean;
       function matchstr(ic:boolean;const s:bmpstring):boolean;
       function matchstr_br(const s:bmpstring;mode:longint):boolean;
       function matchstr_br(ic:boolean;const s:bmpstring;mode:longint):boolean;

       function lastmodestr(mode:longint):bmpstring;
       function lastmodestr1(mode:longint):bmpstring;
       procedure lastmodestr(mode:longint;var size:longint; var ptc:pchar; var ptli:pdword);
       procedure lastmodestr1(mode:longint;var size:longint; var ptc:pchar; var ptli:pdword);
       procedure lastmodestr(mode:longint;var fst,l:longint);

       procedure setmode(num,om,nm:longint);  { om: old mode; nm: new mode}

       procedure printmacros(const m : array of pbabelmacro; mm:longint);
       procedure printmacros;
{$ifdef ENABLE_DEBUGOUT }    
       procedure debugwrite;
{$endif}
       end;

type TS4 = packed record
       case integer of
         0 : ( s : string[4]; );
	 1 : ( len : byte;
	       i   : dword; );
	 end;


{ ****** create ****************************************************** }
constructor CMainBmp.create;
var i   : longint;
begin
for i:=0 to maxmacros do
  macros[i]:=nil;
lastmacro:=-1;
{$ifdef UNIX}
initsymb('UNIX');
{$endif}
{$ifdef WINDOWS}
initsymb('WINDOWS');
{$endif}
{$ifdef LINUX}
initsymb('LINUX');
{$endif}
end;

{ ****** destroy **************************************************** }
destructor CMainBmp.destroy;
var i  : longint;
begin
for i:=0 to lastmacro do
  if macros[i]<>nil then freemacro(macros[i]);
end;

{ ****** run ******************************************************** }
procedure CMainBmp.run(const mf:ansistring; var fo:text);
var i,mode,j,k,l,ifc     : longint;
    bl,bl_ne             : longint;
    outfile,bm_expand    : longint;   
    prevli,ampos         : dword;
    sx,endnoexpand       : bmpstring;
    s4                   : array[0..3] of char;
    i4                   : dword absolute s4;
    tmpbuf               : CTextBuf;
    tmpmbuf              : CMacroBuf;
    amacro               : PBabelMacro;
    writebuf             : shortstring;

    pm_s,pm_m            : dword; 
    pm_q                 : boolean;
    t4                   : TS4;

    ifs                  : array[0..maxifs] of byte;
    ifli                 : array[0..maxifs] of dword;
    bli		         : array[0..maxbs] of dword;

    disablelineinfo      : boolean;

label endparse;

begin
amacro:=nil;
t4.s:=copy('    '+bc_pm, length(bc_pm)+1,4);
pm_s:=t4.i;
pm_m:=$ffffffff shl ((4-min(length(bc_pm),4))*8);
pm_q:=length(bc_pm)<=4;

writebuf:='';
CFileBuf.create(buf,mf,'');

bl:=0;
fillchar(bufm, sizeof(bufm), 0);
fillchar(rbuf, sizeof(rbuf), #32);
fillchar(li, sizeof(li), 0);
bufm[0]:=bm_outfile_a;
bufm[rbufmax]:=bm_outfile_a+1;

rbp:=0;
mode:=bm_plain;
ifc:=-1;
outfile:=1;
bm_expand:=bm_plain;
disablelineinfo:=false;

prevli:=dword(-1);
ampos:=0;
while (buf<>nil) and (bmp_exit<>bmp_exit_usererror) do
  begin
  if buf.d>buf.lastbuf
    then begin
         buf.endbuf;
         if buf.killme then 
	   begin
           tmpbuf:=buf.last;
           buf.destroy;
           buf:=tmpbuf;
           end;
         end
    else if buf.buf[buf.d]=#13 then buf.d+=1
    else begin
{$ifdef ENABLE_DEBUGOUT}
         if debugoutput then debugwrite;
{$endif}	 
	 i:=bufm[rbp];
         if (i<10) and (i>=0) and (outfile>0) then 
	      begin
	      j:=ord(writebuf[0])+1;	{ avoid the range checking }
	      writebuf[0]:=char(j);
	      writebuf[j]:=rbuf[rbp];
	      if rbuf[rbp]=#10 then
	         begin
		 if (li[rbp]<>prevli+4096) and (lineInfoPattern<>'') then lineInfoStr(fo,li[rbp]);
		 prevli:=li[rbp];
		 byte(writebuf[0])-=1;
	         writeln(fo,writebuf);
	         writebuf:='';
	         end;
	      if (j>=255) then
	         begin
	         write(fo,writebuf);
	         writebuf:='';
	         end;
	      end
	    else if (i>=bm_outfile_a) and (i<=bm_outfile_z) then outfile:=i-bm_outfile_a;

         rbuf[rbp]:=buf.buf[buf.d];
	 lineInfo:=buf.li[buf.d];
         li[rbp]:=lineInfo;
         bufm[rbp]:=mode;
         buf.d+=1;

{brackets}
         if rbuf[rbp]=bc_ob then 
	   begin
	   if (bl>=0) and (bl<maxbs) then bli[bl]:=lineInfo;
	   bl+=1;
	   end;
         if rbuf[rbp]=bc_cb then bl-=1;
{if's}
	 if mode<>bm_plain then
	   begin
           if (mode=bm_pm) and matchstr(bc_pm+'ifdef'+bc_ob,bm_pm) then
              begin
              setmode(length(bc_pm+'ifdef'+bc_ob),bm_pm,bm_if);
              mode:=bm_if_scb;
	      bli[0]:=lineInfo;
              bl:=1;
              end;
           if (mode=bm_pm) and matchstr(bc_pm+'ifndef'+bc_ob,bm_pm) then
              begin
              setmode(length(bc_pm+'ifndef'+bc_ob),bm_pm,bm_if);
              mode:=bm_ifn_scb;
	      bli[0]:=lineInfo;
              bl:=1;
              end;
	   if (mode=bm_pm) and matchstr(bc_pm+'ifeq'+bc_ob,bm_pm) then
              begin
              setmode(length(bc_pm+'ifeq'+bc_ob),bm_pm,bm_if);
              mode:=bm_ifeq_scb1;
	      bm_expand:=bm_ifeq_scb1;
	      bli[0]:=lineInfo;
              bl:=1;
              end;
	   if (mode=bm_pm) and matchstr(bc_pm+'ifneq'+bc_ob,bm_pm) then
              begin
              setmode(length(bc_pm+'ifeq'+bc_ob),bm_pm,bm_if);
              mode:=bm_ifneq_scb1;
	      bm_expand:=bm_ifneq_scb1;
	      bli[0]:=lineInfo;
              bl:=1;
              end;
	   
           if (mode=bm_if_not) and matchstr(bc_pm+'ifdef'+bc_ob,bm_if_not) then
              begin
              setmode(length(bc_pm+'ifdef'+bc_ob),bm_if_not,bm_if);
              ifc+=1;
              if ifc>maxifs then faterror('if memory exceeded');
	      ifli[ifc]:=lineInfo;
              ifs[ifc]:=2;
              end;
           if (mode=bm_if_not) and matchstr(bc_pm+'ifndef'+bc_ob,bm_if_not) then
              begin
              setmode(length(bc_pm+'ifndef'+bc_ob),bm_if_not,bm_if);
              ifc+=1;
              if ifc>maxifs then faterror('if memory exceeded');
	      ifli[ifc]:=lineInfo;
              ifs[ifc]:=2;
              end;
           if (mode=bm_if_not) and matchstr(bc_pm+'ifeq'+bc_ob,bm_if_not) then
              begin
              setmode(length(bc_pm+'ifeq'+bc_ob),bm_if_not,bm_if);
              ifc+=1;
              if ifc>maxifs then faterror('if memory exceeded');
	      ifli[ifc]:=lineInfo;
              ifs[ifc]:=2;
              end;
           if (mode=bm_if_not) and matchstr(bc_pm+'ifneq'+bc_ob,bm_if_not) then
              begin
              setmode(length(bc_pm+'ifneq'+bc_ob),bm_if_not,bm_if);
              ifc+=1;
              if ifc>maxifs then faterror('if memory exceeded');
	      ifli[ifc]:=lineInfo;
              ifs[ifc]:=2;
              end;

           if ((mode=bm_ifeq_scb1) or (mode=bm_ifeq_scb1_ne) or (mode=bm_ifneq_scb1) or (mode=bm_ifneq_scb1_ne)) and (bl=0) then
	      begin
	      bufm[rbp]:=bm_ifb;
	      if (mode=bm_ifeq_scb1) or (mode=bm_ifeq_scb1_ne) 
	         then mode:=bm_ifeq_sob
		 else mode:=bm_ifneq_sob;
	      ampos:=lineInfo;	 
	      bm_expand:=bm_plain;
	      end;

           if ((mode=bm_ifeq_sob) or (mode=bm_ifneq_sob)) and (rbuf[rbp]=bc_ob) then
	      begin
	      bli[0]:=lineInfo;
	      bl:=1;
	      if mode=bm_ifeq_sob
	         then mode:=bm_ifeq_scb2
	         else mode:=bm_ifneq_scb2;
	      bm_expand:=mode;
	      end;

           if ((mode=bm_if_scb) or (mode=bm_ifn_scb)) and (bl=0) then
              begin
              sx:=lastmodestr1(mode);
              if length(sx)<1
                 then begin
                      warning('empty symbol name');
                      mode:=bm_pm;
                      end
                 else begin
                      ifc+=1;
                      if ifc>maxifs then faterror('if memory exceeded');
	              ifli[ifc]:=lineInfo;
                      if mode=bm_if_scb
		         then ifs[ifc]:=0
                         else ifs[ifc]:=1;
		      i:=lastmacro;
		      while i>=0 do
		        begin
                        if (macros[i]^.name=sx) then
                          begin
                          if mode=bm_if_scb
			     then ifs[ifc]:=1
                             else ifs[ifc]:=0;
                          i:=0;
                          end;
		        i-=1;
			end;
                      if ifs[ifc]=0 
		        then mode:=bm_if_not
                        else mode:=bm_pm;
                      end;
                 end;
           if ((mode=bm_ifeq_scb2) or (mode=bm_ifeq_scb2_ne) or (mode=bm_ifneq_scb2) or (mode=bm_ifneq_scb2_ne)) and (bl=0) then
              begin
	      i:=0;  
	      j:=0;  
	      repeat
	        repeat
		  i+=1;
		  k:=bufm[(rbp-i) and rbufmax];
//		  writeln(stderr,'  i=',i,'  k=',k,' rbuf=',rbuf[(rbp-i) and rbufmax]);
		  until (i>=rbufmax) or (k=bm_if) or (k=bm_ifeq_scb2) or (k=bm_ifeq_scb2_ne) or (k=bm_ifneq_scb2) or (k=bm_ifneq_scb2_ne);
	        repeat
		  j+=1;
		  l:=bufm[(rbp-j) and rbufmax];
//		  writeln(stderr,'  j=',j,'  l=',l,' rbuf=',rbuf[(rbp-j) and rbufmax]);
		  until (j>=rbufmax) or (l=bm_if) or (l=bm_ifeq_scb1) or (l=bm_ifeq_scb1_ne) or (l=bm_ifneq_scb1) or (l=bm_ifneq_scb1_ne);
//		writeln(stderr,rbp,',',i,',',j,'-->',rbuf[(rbp-i) and rbufmax],'<-->',rbuf[(rbp-j) and rbufmax],'<--');
	        until (j>=rbufmax) or (l=bm_if) or (k=bm_if) or (rbuf[(rbp-i) and rbufmax]<>rbuf[(rbp-j) and rbufmax]);
//	      writeln(stderr);	
              ifc+=1;
              if ifc>maxifs then faterror('if memory exceeded');
	      ifli[ifc]:=lineInfo;
	      
	      if (i=j) = ((mode=bm_ifeq_scb2) or (mode=bm_ifeq_scb2_ne))
	        then ifs[ifc]:=1
                else ifs[ifc]:=0;
	      
              if ifs[ifc]=0 
	         then mode:=bm_if_not
                 else mode:=bm_pm;
	      bm_expand:=bm_plain;	 
	      end;

           if (mode=bm_pm) and matchstr(bc_pm+'else',bm_pm) then
              begin
              if ifc<0 then error('else without ifdef');
              if ifs[ifc]<>1 then error('internal error 5');
              ifs[ifc]:=0;
              mode:=bm_if_not;
              end;
           if (mode=bm_pm) and ( matchstr(bc_pm+'elifdef'+bc_ob,bm_pm) 
			       or matchstr(bc_pm+'elifndef'+bc_ob,bm_pm) 
			       or matchstr(bc_pm+'elifeq'+bc_ob,bm_pm) 
			       or matchstr(bc_pm+'elifneq'+bc_ob,bm_pm) ) then
              begin
              if ifc<0 then error('else without ifdef');
              if ifs[ifc]<>1 then error('internal error 5');
              ifs[ifc]:=2;
              mode:=bm_if_not;
              end;

           if (mode=bm_if_not) and matchstr(bc_pm+'else',bm_if_not) then
              begin
              setmode(length(bc_pm+'else'+bc_ob),bm_if_not,bm_if);
              if ifc<0 
	        then error('else without ifdef')
                else if ifs[ifc]=0
                        then begin
                             ifs[ifc]:=1;
                             mode:=bm_pm;
                             end
                        else begin
                             if ifs[ifc]<>2 then error('internal error 6')
                             end;
              end;
           if (mode=bm_if_not) and matchstr(bc_pm+'elifdef'+bc_ob,bm_if_not) then
              begin
              setmode(length(bc_pm+'elifdef'+bc_ob),bm_if_not,bm_if);
              if ifc<0 
	         then error('elifdef without ifdef')
                 else if ifs[ifc]=0
                         then begin
			      ifc-=1;
                              mode:=bm_if_scb;
	    	    	      bli[0]:=lineInfo;
            		      bl:=1;
                              end
                         else begin
                              if ifs[ifc]<>2 then error('internal error 6a')
                              end;
              end;
           if (mode=bm_if_not) and matchstr(bc_pm+'elifndef'+bc_ob,bm_if_not) then
              begin
              setmode(length(bc_pm+'elifndef'+bc_ob),bm_if_not,bm_if);
              if ifc<0 
	         then error('elifndef without ifdef')
                 else if ifs[ifc]=0
                        then begin
			     ifc-=1;
                             mode:=bm_ifn_scb;
			     bli[0]:=lineInfo;
            		     bl:=1;
                             end
                        else begin
                             if ifs[ifc]<>2 then error('internal error 6b')
                             end;
              end;
           if (mode=bm_if_not) and matchstr(bc_pm+'elifeq'+bc_ob,bm_if_not) then
              begin
              setmode(length(bc_pm+'elifeq'+bc_ob),bm_if_not,bm_if);
              if ifc<0 
	         then error('elifeq without ifdef')
                 else if ifs[ifc]=0
                        then begin
			     ifc-=1;
                             mode:=bm_ifeq_scb1;
			     bm_expand:=bm_ifeq_scb1;
			     bli[0]:=lineInfo;
            		     bl:=1;
                             end
                        else begin
                             if ifs[ifc]<>2 then error('internal error 6c')
                             end;
              end;
           if (mode=bm_if_not) and matchstr(bc_pm+'elifneq'+bc_ob,bm_if_not) then
              begin
              setmode(length(bc_pm+'elifneq'+bc_ob),bm_if_not,bm_if);
              if ifc<0 
	         then error('elifneq without ifdef')
                 else if ifs[ifc]=0
                        then begin
			     ifc-=1;
                             mode:=bm_ifneq_scb1;
			     bm_expand:=bm_ifneq_scb1;
			     bli[0]:=lineInfo;
            		     bl:=1;
                             end
                        else begin
                             if ifs[ifc]<>2 then error('internal error 6d')
                             end;
              end;

           if ((mode=bm_if_not) or (mode=bm_pm)) and (matchstr(bc_pm+'endif',bm_if_not) or matchstr(bc_pm+'endif',bm_pm)) then
              begin
              if ifc<0 
	         then error('endif without if')
	         else ifc-=1;
              if (ifc=-1) or (ifs[ifc]=1) 
	         then mode:=bm_pm
                 else mode:=bm_if_not;
              end;
           if mode=bm_if_not then goto endparse;
{define mode}
           if mode=bm_def_sob2 then
              begin
              if rbuf[rbp]=bc_ob
                 then begin
                      mode:=bm_def_scb2;
	              bli[0]:=lineInfo;
                      bl:=1;
                      end
                 else begin
                      mode:=bm_pm;
                      setmode(1,-1,bm_pm);
                      end;
              end;
           if mode=bm_def_sob3 then
              begin
              if rbuf[rbp]=bc_ob
                 then begin
                      mode:=bm_def_scb3;
		      bli[0]:=lineInfo;
                      bl:=1;
                      end
                 else begin
                      lastmodestr1(bm_def_scb2, macros[lastmacro]^.rsize, macros[lastmacro]^.r, macros[lastmacro]^.li);
                      mode:=bm_pm;
                      setmode(1,-1,bm_pm);
                      end;
              end;
           if (mode=bm_def_scb1) and (bl=0) then
              begin
              mode:=bm_def_sob2;
              sx:=lastmodestr1(bm_def_scb1);
              if length(sx)<1
                 then begin
                      warning('empty macro name');
                      mode:=bm_pm;
                      end
                 else initsymb(sx);
              end;
           if (mode=bm_def_scb2) and (bl=0) then
              begin
              mode:=bm_def_sob3;
              end;
           if (mode=bm_def_scb3) and (bl=0) then
              begin
              mode:=bm_pm;
              lastmodestr(bm_def_scb2,i,j);
              j-=1;
	      k:=1;
	      while k<=j do
                begin
                if rbuf[i]=bc_pa
                   then begin
                        if macros[lastmacro]^.pn>=maxparams then error('only '+int2str(maxparams)+' paramters allowed');
                        if k=j then error(paramids[macros[lastmacro]^.pn+1]+' expected after `'+bc_pa+''', found '+bc_cb)
                               else begin
                                    inc(k);
	    		  	    i:=(i+1) and rbufmax;
                                    if rbuf[i]=paramids[macros[lastmacro]^.pn+1]
                                       then inc(macros[lastmacro]^.pn)
                                       else error(paramids[macros[lastmacro]^.pn+1]+' expected after `'+bc_pa+''', found '+rbuf[i]);
                                    end;
                        end
                   else macros[lastmacro]^.paramsep[macros[lastmacro]^.pn]+=rbuf[i];
 	        i:=(i+1) and rbufmax;
		k+=1;
                end;
              if (macros[lastmacro]^.pn>0) then
                begin
                if macros[lastmacro]^.paramsep[0]='' then macros[lastmacro]^.paramsep[0]:=bc_ob;
                if macros[lastmacro]^.paramsep[macros[lastmacro]^.pn]='' then macros[lastmacro]^.paramsep[macros[lastmacro]^.pn]:=bc_cb;
                end;
              for i:=1 to macros[lastmacro]^.pn-1 do
                if macros[lastmacro]^.paramsep[i]='' then macros[lastmacro]^.paramsep[i]:=bc_cb+bc_ob;
              lastmodestr1(bm_def_scb3, macros[lastmacro]^.rsize, macros[lastmacro]^.r, macros[lastmacro]^.li);
              if disablelineinfo then
                for i:=0 to macros[lastmacro]^.rsize-1 do
                  macros[lastmacro]^.li[i]:=macros[lastmacro]^.li[i] or 2048;
              disablelineinfo:=false;
              end;
           if (mode=bm_def_scbu) and (bl=0) then
              begin
              sx:=lastmodestr1(bm_def_scbu);
              j:=0;
              if length(sx)<1
                 then warning('empty symbol name')
                 else begin
		      i:=lastmacro;
		      while i>=0 do
		        begin
                        if macros[i]^.name=sx then
                          begin
                          freemacro(macros[i]);
                          for j:=i to lastmacro-1 do
                            macros[j]:=macros[j+1];
                          macros[lastmacro]:=nil;
                          dec(lastmacro);
                          j:=-10;
                          i:=0;
                          end;
			i-=1;  
			end;
                      if j<>-10 then warning('`'+sx+''' not defined');
                      end;
              mode:=bm_pm;
              end;
           if (mode=bm_def_scba) and (bl=0) then
              begin
              sx:=lastmodestr1(bm_def_scba);
              j:=0;
              if length(sx)<1
                 then warning('empty symbol name')
                 else begin
                      i:=0;k:=0;
                      while i<=lastmacro do
                       if macros[i]^.name=sx
                          then begin
                               freemacro(macros[i]);
                               for j:=i to lastmacro-1 do
                                 macros[j]:=macros[j+1];
                               macros[lastmacro]:=nil;
                               dec(lastmacro);
                               k:=-10;
                               end
                          else inc(i);
                      if k<>-10 then warning('`'+sx+''' not defined');
                      end;
              mode:=bm_pm;
              end;
{include mode}
           if (mode=bm_inc_scb) and (bl=0) then
              begin
              sx:=lastmodestr1(bm_inc_scb);
              if length(sx)<1
                 then warning('empty include file name')
                 else CFileBuf.create(buf,sx, lineInfoStr(lineInfo)+': ');
	      mode:=bm_plain;
              end;
{noexpand mode}
           if ((mode=bm_pm) or (mode=bm_ifeq_scb1) or (mode=bm_ifeq_scb2) or (mode=bm_ifneq_scb1) or (mode=bm_ifneq_scb2)) and matchstr(bc_pm+'noexpand'+bc_ob,mode) then
              begin
              setmode(length(bc_pm+'noexpand'+bc_ob),mode,bm_noexpand);
	      mode:=bm_noexpand_scb;
	      bli[0]:=lineInfo;
              bl_ne:=bl;
              bl:=1;
              end;
           if (mode=bm_noexpand_scb) and (bl=0) then
              begin
              endnoexpand:=lastmodestr1(bm_noexpand_scb);
	      case bm_expand of
	        bm_ifeq_scb1  : mode:=bm_ifeq_scb1_ne;
		bm_ifeq_scb2  : mode:=bm_ifeq_scb2_ne;
		bm_ifneq_scb1 : mode:=bm_ifneq_scb1_ne;
		bm_ifneq_scb2 : mode:=bm_ifneq_scb2_ne;
		else mode:=bm_neplain;
		end;
	      ampos:=lineInfo;
	      bl:=bl_ne-1;	
              end;
           if ((mode=bm_neplain) or (mode=bm_ifeq_scb1_ne) or (mode=bm_ifeq_scb2_ne) or (mode=bm_ifneq_scb1_ne) or (mode=bm_ifneq_scb2_ne)) and (endnoexpand<>'') and matchstr(endnoexpand,mode) then
              begin
              setmode(length(endnoexpand),mode,bm_noexpand);
	      case bm_expand of
	        bm_ifeq_scb1, bm_ifeq_scb2, bm_ifneq_scb1, bm_ifneq_scb2 : mode:=bm_expand;
		else mode:=bm_plain;
		end;
              end;
{error mode}
           if (mode=bm_err_scb) and (bl=0) then
              begin
              error(lastmodestr1(bm_err_scb));
	      mode:=bm_plain;
	      bmp_exit:=bmp_exit_usererror;
              end;
{warning mode}
           if (mode=bm_warn_scb) and (bl=0) then
              begin
              warning(lastmodestr1(bm_warn_scb));
	      mode:=bm_plain;
              end;
{programming mode}
           if mode=bm_pm then
              begin
              if matchstr(bc_pm+bc_pm,bm_pm) then
                 begin
                 setmode(length(bc_pm),bm_pm,bm_plain);
                 mode:=bm_plain;
                 end;
              if matchstr(bc_lf,bm_pm) then mode:=bm_plain;
              if matchstr(bc_pm+'define'+bc_ob,bm_pm) then
                 begin
                 setmode(length(bc_pm+'define'+bc_ob),bm_pm,bm_def);
                 mode:=bm_def_scb1;
	         bli[0]:=lineInfo;
                 bl:=1;
                 end;
              if matchstr(bc_pm+'udefine'+bc_ob,bm_pm) then
                 begin
                 setmode(length(bc_pm+'udefine'+bc_ob),bm_pm,bm_def);
                 mode:=bm_def_scbu;
  	         bli[0]:=lineInfo;
                 bl:=1;
                 end;
              if matchstr(bc_pm+'uadefine'+bc_ob,bm_pm) then
                 begin
                 setmode(length(bc_pm+'uadefine'+bc_ob),bm_pm,bm_def);
                 mode:=bm_def_scba;
	         bli[0]:=lineInfo;
                 bl:=1;
                 end;
              if matchstr(bc_pm+'nolf',bm_pm) then
                 begin
                 i:=rbp; j:=0; k:=0;
                 while (j<=rbufmax+1-length(bc_lf)) and (k=0) do
                   begin
    		   rbp:=(rbp-1) and rbufmax;
                   if matchstr(bc_lf,bm_plain) then 
		      begin
                      setmode(length(bc_lf),-1,bm_comm);
                      k:=1;
                      end;
                  j+=1;
                  end;
                 rbp:=i;
                 end;
              if matchstr(bc_pm+'include'+bc_ob,bm_pm) then
                 begin
                 setmode(length(bc_pm+'include'+bc_ob),bm_pm,bm_inc);
                 mode:=bm_inc_scb;
	         bli[0]:=lineInfo;
                 bl:=1;
                 end;
              if matchstr(bc_pm+'error'+bc_ob,bm_pm) then
                 begin
                 setmode(length(bc_pm+'error'+bc_ob),bm_pm,bm_err);
                 mode:=bm_err_scb;
	         bli[0]:=lineInfo;
                 bl:=1;
                 end;
              if matchstr(bc_pm+'warning'+bc_ob,bm_pm) then
                 begin
                 setmode(length(bc_pm+'warning'+bc_ob),bm_pm,bm_warn);
                 mode:=bm_warn_scb;
	         bli[0]:=lineInfo;
                 bl:=1;
                 end;
              if matchstr(bc_pm+'disableout',bm_pm) then bufm[rbp]:=bm_outfile_a;
              if matchstr(bc_pm+'enableout',bm_pm) then bufm[rbp]:=bm_outfile_a+1;
              if matchstr(bc_pm+'ignorecase',bm_pm) then bp_icase:=true;
              if matchstr(bc_pm+'exactcase',bm_pm) then bp_icase:=false;
              if matchstr(bc_pm+'disablelineinfo',bm_pm) then disablelineinfo:=true;
              end;

{expanding mode}
           if ((mode>=bm_em_p0) and (mode<=bm_em_pz)) and (matchstr(bp_icase, amacro^.paramsep[mode-bm_em_p0], mode)) then
              begin
              setmode(length(amacro^.paramsep[mode-bm_em_p0])-1,mode,bm_em);
              if (mode=bm_em_pz) or (mode-bm_em_p0=amacro^.pn)
                 then begin
                      mode:=bm_expand;
                      if amacro^.rsize<0 then error('internal error 2');
                      if amacro^.rsize>0 then
                         begin
			 tmpmbuf:=CMacroBuf.create(buf, amacro, false);
                         i:=0;
                         while i<amacro^.rsize do
                           begin
			   if tmpmbuf.lastbuf>=macrobuf_size-1 then CMacroBuf.insert(tmpmbuf);
                           if (amacro^.r[i]=bc_pa) and (i+1<amacro^.rsize)
                              then begin
                                   inc(i);
                                   if amacro^.r[i]=bc_pa
                                      then begin
                                           while amacro^.r[i]=bc_pa do
                                            begin
					    if tmpmbuf.lastbuf>=macrobuf_size-1 then CMacroBuf.insert(tmpmbuf);
                                            inc(tmpmbuf.lastbuf);
                                            tmpmbuf.buf[tmpmbuf.lastbuf]:=bc_pa;
					    tmpmbuf.li[tmpmbuf.lastbuf]:=amacro^.li[i];
                                            inc(i);
                                            end;
                                           end
                                      else begin
                                           l:=-1;
                                           repeat
                                             inc(l);
                                             until (l>amacro^.pn) or (paramids[l]=amacro^.r[i]);
                                           if l<=amacro^.pn
                                               then begin
                                                    lastmodestr(bm_em_p0+l,j,k);
                                                    dec(k);
                                                    while (k>0) do
                                                      begin
						      if tmpmbuf.lastbuf>=macrobuf_size-1 then CMacroBuf.insert(tmpmbuf);
                                                      inc(tmpmbuf.lastbuf);
                                                      tmpmbuf.buf[tmpmbuf.lastbuf]:=rbuf[j];
                                                      tmpmbuf.li[tmpmbuf.lastbuf]:=li[j];
		   				      j:=(j+1) and rbufmax;
                                                      dec(k);
                                                      end;
                                                    inc(i);
                                                    end
                                               else begin
                                                    inc(tmpmbuf.lastbuf);
                                                    tmpmbuf.buf[tmpmbuf.lastbuf]:=bc_pa;
                                                    tmpmbuf.li[tmpmbuf.lastbuf]:=amacro^.li[i];
                                                    end;
                                            end;
                                   end
                              else begin
                                   inc(tmpmbuf.lastbuf);
                                   tmpmbuf.buf[tmpmbuf.lastbuf]:=amacro^.r[i];
                                   tmpmbuf.li[tmpmbuf.lastbuf]:=amacro^.li[i];
                                   inc(i);
                                   end;
                           end;
                         end;
                      end
                 else inc(mode);
              end;
	   end;      
{plain mode}
         if mode=bm_expand then
	    begin
            i:=3; j:=0; k:=rbp;		{ equal to copylaststr_br(4,bm_expand); }
	    i4:=0;
            while (i>=0) and (j<=rbufmax) do
              begin
              if bufm[k]=bm_expand then
                 begin
	         s4[i]:=rbuf[k];
                 i-=1;
                 end;
               k:=(k-1) and rbufmax;
	       j+=1;
	       end;

	    if (mode=bm_plain) and ((i4 xor pm_s) and pm_m=0) and (pm_q or matchstr(bc_pm,bm_plain)) and (matchstr(bc_lf+bc_pm) or (buf.d+buf.d0=length(bc_pm))) 
	      then begin
                   mode:=bm_pm;
                   setmode(length(bc_pm),bm_plain,mode);
                   end
	      else begin
                   if bp_icase 
       	              then begin
  	                   if (s4[0]>='a') and (s4[0]<='z') then byte(s4[0])-=32;
	                   if (s4[1]>='a') and (s4[1]<='z') then byte(s4[1])-=32;
	                   if (s4[2]>='a') and (s4[2]<='z') then byte(s4[2])-=32;
	                   if (s4[3]>='a') and (s4[3]<='z') then byte(s4[3])-=32;
		           j:=1;
			   end
	              else j:=0;
	       
	    	   i:=lastmacro;
		   while i>=0 do
	             begin
	             amacro:=macros[i];
                     if (amacro^.rsize>=0) and (amacro^.enabled) and ((i4 xor amacro^.mmatch_s[j]) and amacro^.mmatch_m=0) and (amacro^.quick or matchstr_br(bp_icase,amacro^.name,bm_expand)) then
                        begin
                        setmode(length(amacro^.name),bm_expand,bm_em);
                        ampos:=lineInfo;
                        if amacro^.paramsep[0]='' 
			   then begin
                                if amacro^.rsize>0 then CMacroBuf.create(buf, amacro, true);
                                end
                           else mode:=bm_em_p0;
                        i:=0;
                        end;
		     i-=1;
	             end;
	           end;
            end;
endparse:
         rbp:=(rbp+1) and rbufmax;
         end;
 end;

if bmp_exit=0 then case mode of
  bm_plain, bm_pm, bm_def_sob2, bm_def_sob3, bm_if_not
     : if ifc>=0 then
         begin
	 if ifc>0 then sx:='if''s without endif''s at '+lineInfoStr(ifli[0])
	          else sx:='if without endif at '+lineInfoStr(ifli[0]);
	 for i:=1 to ifc do
	   sx+=', '+lineInfoStr(ifli[i]);
	 error(sx);  
         end;
  bm_neplain, bm_ifeq_scb1_ne, bm_ifeq_scb2_ne, bm_ifneq_scb1_ne, bm_ifneq_scb2_ne 
     : error('Unexpected end of file, `'+endnoexpand+''' expected for '+bc_pm+'noexpand at '+lineInfoStr(ampos));
  bm_em_p0..bm_em_pz 
     : error('Unexpected end of file, `'+amacro^.paramsep[mode-bm_em_p0]+''' expected for '+amacro^.name+' at '+lineInfoStr(ampos));
  bm_def_scb1, bm_def_scb2, bm_def_scb3, bm_def_scbu, bm_def_scba, 
  bm_if_scb, bm_ifn_scb, bm_ifeq_scb1, bm_ifeq_scb2, bm_ifneq_scb1, bm_ifneq_scb2, 
  bm_inc_scb, bm_err, bm_err_scb, bm_warn
     : begin
       if bl>1 then begin
    		    if bl>2 then sx:=',  there are unclosed `'+bc_ob+'''s at '+lineInfoStr(bli[1])
			    else sx:=',  there is a unclosed `'+bc_ob+''' at '+lineInfoStr(bli[1]);
		    for i:=2 to bl-1 do
		      sx+=', '+lineInfoStr(bli[i]);
		    end
	       else sx:='  (maybe there are unclosed `'+bc_ob+'''s)';
       error('Unexpected end of file, `'+bc_cb+''' expected for `'+bc_ob+''' at '+lineInfoStr(bli[0])+sx);
       end;
  bm_ifeq_sob 
     : error('Unexpected end of file, `'+bc_ob+''' expected for '+bc_pm+'ifeq at '+lineInfoStr(ampos));
  bm_ifneq_sob 
     : error('Unexpected end of file, `'+bc_ob+''' expected for '+bc_pm+'ifneq at '+lineInfoStr(ampos));
  else error('Unexpected end of file ('+int2str(mode)+')');
  end;

for k:=0 to rbufmax do
  begin
{$ifdef ENABLE_DEBUGOUT }    
  if debugoutput then debugwrite;
{$endif}  
  i:=bufm[rbp];
  if (i<10) and (i>=0) and (outfile>0) then 
      begin
      j:=ord(writebuf[0])+1;	{ avoid the range checking }
      writebuf[0]:=char(j);                       
      writebuf[j]:=rbuf[rbp];
      if rbuf[rbp]=#10 then
         begin
	 if (li[rbp]<>prevli+4096) and (lineInfoPattern<>'') then lineInfoStr(fo,li[rbp]);
	 prevli:=li[rbp];
	 byte(writebuf[0])-=1;
         writeln(fo,writebuf);
         writebuf:='';
         end;
      if j>=255 then
        begin
        write(fo,writebuf);
        writebuf:='';
        end;
      end
    else if (i>=bm_outfile_a) and (i<=bm_outfile_z) then outfile:=i-bm_outfile_a;
  rbp:=(rbp+1) and rbufmax;
  end;
write(fo,writebuf);  
if printdefs then printmacros;
end;

{ ******** initsymb ************************************************** }
procedure CMainBmp.initsymb(const s:bmpstring);
var s4 : TS4;
begin
lastmacro+=1;
if lastmacro>maxmacros then faterror('Macro memory exceeded');

getmem(macros[lastmacro],sizeof(TBabelMacro));
fillchar( macros[lastmacro]^, sizeof(TBabelMacro), 0 );

s4.s:=copy('    '+s,length(s)+1,4);
macros[lastmacro]^.mmatch_s[0]:=s4.i;
upstr2(s4.s);
macros[lastmacro]^.mmatch_s[1]:=s4.i;
macros[lastmacro]^.mmatch_m:=$ffffffff shl ((4-min(length(s),4))*8);
macros[lastmacro]^.quick:=length(s)<=4;
macros[lastmacro]^.enabled:=true;

macros[lastmacro]^.rsize:=-1;
macros[lastmacro]^.pn:=0;
macros[lastmacro]^.name:=s;
end;

{ ****** error_int ************************************************** }
procedure CMainBmp.error_int(const t:ansistring);
var i,j,k : longint;
    tmp   : CTextBuf;
    s     : shortstring;
begin
tmp:=buf;
i:=0;
while tmp<>nil do
  begin
  tmp:=CTextBuf(tmp.last);
  i+=1;
  end;
s:='';
for j:=1 to i do
  begin
  tmp:=buf;
  for k:=1 to i-j do
    tmp:=tmp.last;
  k:=tmp.d;
  if k>0 then k-=1;
  writeln(stderr, s, lineInfoStr(tmp.li[k]), ':');
  s+='  ';
  end;
writeln(stderr,s,t);
end;

{ ****** hint ******************************************************* }
procedure CMainBmp.hint(const s:ansistring);
begin
if enable_hints then writeln(stderr,'Hint: '+lineInfoStr(lineInfo)+': '+s);
end;

{ ****** warning **************************************************** }
procedure CMainBmp.warning(const s:ansistring);
begin
error_int('Warning: '+s);
end;

{ ****** error ****************************************************** }
procedure CMainBmp.error(const s:ansistring);
begin
error_int('Error: '+s);
bmp_exit:=bmp_exit_error;
end;

{ ****** faterror *************************************************** }
procedure CMainBmp.faterror(const s:ansistring);
begin
error_int('Fatal error: '+s);
halt(bmp_exit_faterror);
end;

{ ****** copylaststr ************************************************ }
function CMainBmp.copylaststr(l,mode:longint):rbufstr;
var i,k : longint;
begin
l:=l and rbufmax;
i:=0; k:=rbp;
while (i<l) and ((bufm[k]=mode) or (mode<0)) do
  begin
  i+=1;
  k:=(k-1) and rbufmax;
  end;
result.rbuf:=@rbuf;
result.pt:=k+1;
result.length:=i;
end;

function CMainBmp.copylaststr(l:longint):rbufstr;
begin
result:=copylaststr(l,-1);
end;

function CMainBmp.copylaststr_br(l,mode:longint):bmpstring;
var i,j,k : longint;
begin
{$ifdef USE_SHORTSTRINGS}
if l>255 then l:=255;
{$endif}
i:=0; j:=0; k:=rbp;
setlength(result,l);
while (i<l) and (j<=rbufmax) do
  begin
  if (bufm[k]=mode) or (mode<0) then
    begin
    result[l-i]:=rbuf[k];
    i+=1;
    end;
  k:=(k-1) and rbufmax;
  j+=1;
  end;
if i<l then result:=copy(result, length(result)-i+1, i);
end;

{ ****** matchstr **************************************************** }
function CMainBmp.matchstr(const s:bmpstring; mode:longint):boolean;
begin
matchstr:=cmp_rbuf(copylaststr(length(s),mode), s);
end;

function CMainBmp.matchstr(const s:bmpstring):boolean;
begin
matchstr:=cmp_rbuf(copylaststr(length(s),-1),s);
end;

function CMainBmp.matchstr(ic:boolean; const s:bmpstring; mode:longint):boolean;
begin
if ic then matchstr:=cmpcase_rbuf(copylaststr(length(s),mode),s)
      else matchstr:=cmp_rbuf(copylaststr(length(s),mode),s);
end;

function CMainBmp.matchstr(ic:boolean;const s:bmpstring):boolean;
begin
if ic then matchstr:=cmpcase_rbuf(copylaststr(length(s),-1),s)
      else matchstr:=cmp_rbuf(copylaststr(length(s),-1),s);
end;

function CMainBmp.matchstr_br(const s:bmpstring; mode:longint):boolean;
begin
matchstr_br:=copylaststr_br(length(s),mode)=s;
end;

function CMainBmp.matchstr_br(ic:boolean; const s:bmpstring; mode:longint):boolean; 
begin
if ic then matchstr_br:=upstr(copylaststr_br(length(s),mode))=upstr(s)
      else matchstr_br:=copylaststr_br(length(s),mode)=s
end;

{ ****** lastmodestr ************************************************* }
procedure CMainBmp.lastmodestr(mode:longint; var fst,l:longint);
var i,j,k  : longint;
begin
i:=rbp; j:=0; k:=0;
while (bufm[i]<>mode) and (j<=rbufmax) do
  begin
  i:=(i-1) and rbufmax;
  j+=1;
  end;
while (bufm[i]=mode) and (j<=rbufmax) do
  begin
  i:=(i-1) and rbufmax;
  j+=1;
  k+=1;
  end;
i:=(i+1) and rbufmax;
fst:=i;
l:=k;
end;

procedure CMainBmp.lastmodestr(mode:longint; var size:longint; var ptc:pchar; var ptli:pdword);
var i,j,k  : longint;
begin
lastmodestr(mode,i,j);
if j>0 then begin
            size:=j;
            getmem(ptc,j);
            getmem(ptli,j*4);
            for k:=0 to j-1 do
              begin
              ptc[k]:=rbuf[i];
              ptli[k]:=li[i];
	      i:=(i+1) and rbufmax;
              end;
            end
       else begin
            size:=0;
            ptc:=nil;
	    ptli:=nil;
            end;
end;

function CMainBmp.lastmodestr(mode:longint):bmpstring;
var i,j,k  : longint;
begin
lastmodestr(mode,i,j);
{$ifdef USE_SHORTSTRINGS}
if j>255 then j:=255;
{$endif}
setlength(lastmodestr,j);
for k:=1 to j do
  begin
  lastmodestr[k]:=rbuf[i];
  i:=(i+1) and rbufmax;
  end;
end;

procedure CMainBmp.lastmodestr1(mode:longint; var size:longint; var ptc:pchar; var ptli:pdword);
var i,j,k  : longint;
begin
lastmodestr(mode,i,j);
j-=1;
if j>0 then begin
            size:=j;
            getmem(ptc,j);
            getmem(ptli,j*4);
            for k:=0 to j-1 do
              begin
              ptc[k]:=rbuf[i];
              ptli[k]:=li[i];
	      i:=(i+1) and rbufmax;
              end;
            end
       else begin
            size:=0;
            ptc:=nil;
	    ptli:=nil;
            end;
end;

function CMainBmp.lastmodestr1(mode:longint):bmpstring;
var i,j,k  : longint;
begin
lastmodestr(mode,i,j);
j-=1;
{$ifdef USE_SHORTSTRINGS}
if j>255 then j:=255;
{$endif}
setlength(lastmodestr1,j);
for k:=1 to j do
  begin
  lastmodestr1[k]:=rbuf[i];
  i:=(i+1) and rbufmax;
  end;
end;


{ ****** setmode ***************************************************** }
procedure CMainBmp.setmode(num,om,nm:longint);
var i,j,k  : longint;
begin
k:=rbp;
i:=0; j:=0;
while (i<num) and (j<=rbufmax) do
  begin
  if (bufm[k]=om) or (om<0) then
    begin
    i+=1;
    bufm[k]:=nm;
    end;
  j+=1;
  k:=(k-1) and rbufmax;
  end;
end;

{ ******** printmacros *********************************************** }
procedure CMainBmp.printmacros(const m:array of pbabelmacro; mm:longint);
var i,j : longint;
begin
for j:=0 to mm do
  begin
  writeln(stderr,'   ----- definition ',j+1,' -----');
  writeln(stderr,m[j]^.name);
  if m[j]^.paramsep[0]<>'' then write(stderr,m[j]^.paramsep[0]);
  for i:=1 to m[j]^.pn do
    write(stderr,bc_pa,i,m[j]^.paramsep[i]);
  if (m[j]^.pn>0) or (m[j]^.paramsep[0]<>'') then writeln(stderr);
  for i:=0 to m[j]^.rsize-1 do
    write(stderr,m[j]^.r[i]);
  if m[j]^.rsize>0 then writeln(stderr);
  if m[j]^.rsize=0 then writeln(stderr,'<empty>');
  writeln(stderr);
  writeln(stderr);
  end;
end;

procedure CMainBmp.printmacros;
begin
printmacros(macros, lastmacro);
end;

{$ifdef ENABLE_DEBUGOUT }    
{ ******** debugwrite ************************************************ }
procedure CMainBmp.debugwrite;
var i  : longint;
    lf : boolean;
begin
if rbuf[rbp]=#10 
  then begin
       writeln;
       lf:=true;
       end
  else lf:=false;
if (bufm[rbp]<>bm_invalid) and (rbuf[rbp]<>#13) and (rbuf[rbp]<>#10) then write(rbuf[rbp]);
i:=(rbp+1) and rbufmax;
if bufm[rbp]<>bufm[i]
  then begin
       if not lf then writeln;
       write(bufm[i]);
       case bufm[i] of
         0..9       : write('     ');
         10..99     : write('    ');
         200..999   : write('   ');
         1000..9999 : write('  ');
         else         write(' ');
         end;
       end
  else if lf then write('      ');
end;
{$endif}

{ ********************************************************************* }
{ ****** main ********************************************************* }
{ ********************************************************************* }

{ ****** paramerr ***************************************************** }
procedure paramerr(msg:ansistring);
begin
writeln(stderr,'Usage: '+paramstr(0)+' [<Options>] [<filename1> [<filename2> ...]]');
writeln(stderr,'Options: ');
writeln(stderr,'	-o <fileneme>    Output file');
writeln(stderr,'	-p  	         Pascal mode (default), equal to -mm "//" -mo "[" -mc "]" -mp "#"');
writeln(stderr,'	-c               C mode, equal to -mm "#" -mo "[" -mc "]" -mp "$" -l ''#line %2 "%1"''');
writeln(stderr,'	-i               Ignore upper/lower case');
writeln(stderr,'        -l		 Line info (default for C mode: ''#line %2 "%1"'')');
writeln(stderr,'	-I  <directory>  Include path');
writeln(stderr,'	-D  <symbol>     Define symbol <symbol>');
writeln(stderr,'	-mm <string>     Meta macro start string');
writeln(stderr,'	-mo <char>       Open bracket');
writeln(stderr,'	-mc <char>       Close bracket');
writeln(stderr,'	-mp <char>       Parameter character');
//writeln(stderr,'	-nh              Disable hints');
{$ifdef ENABLE_DEBUGOUT}    
writeln(stderr,'	-do              Enable debug output');
{$endif}
writeln(stderr,'	-dd              Print definitions');
writeln(stderr,'	-h               Help');
if msg<>'' 
  then begin
       writeln(stderr,msg);
       halt(bmp_exit_paramerror);
       end
  else halt(0);
end;

{ ****** main ********************************************************* }
const maxInfilec = 1024;

var main          : CMainBmp;
    infile        : array[0..maxInfilec-1] of ansistring;
    i,j,infilec   : longint;
    s,t           : ansistring;
    fo            : text;
    
    fofn             : ansistring;
    bc_pm_d,bc_ob_d  : boolean;
    bc_cb_d,bc_pa_d  : boolean;
    pasmode,lidef    : boolean;

begin
spath[0]:='';
spathc:=1;

main:=CMainBmp.create;

fofn:='';
bc_pm_d:=false;
bc_ob_d:=false;
bc_cb_d:=false;
bc_pa_d:=false;
pasmode:=true;
lidef:=false;

infilec:=0;
i:=1;
while i<=paramcount do
  begin
  s:=paramstr(i);
  if (length(s)>1) and (s[1]='-') 
    then begin
         if s='-o' then 
	     begin 
	     if i>=paramcount then paramerr('File name expected after -o');
	     i+=1;
	     fofn:=paramstr(i);
	     end
	   else if s='-p' then pasmode:=true
	   else if s='-c' then pasmode:=false
	   else if s='-i' then bp_icase:=true
           else if s='-l' then 
	     begin 
	     if i>=paramcount then paramerr('String expected after -l');
	     i+=1;
	     lineInfoPattern:=paramstr(i);
{$ifdef WINDOWS}
	     for j:=1 to length(lineInfoPattern) do
	       if lineInfoPattern[j]='\' then lineInfoPattern[j]:='"';
{$endif}	       
	     lidef:=true;
	     end
	   else if s='-I' then 
	     begin
	     if i>=paramcount then paramerr('Path expected after -I');
	     i+=1;
	     t:=paramstr(i);
{$ifdef WINDOWS}
	     for j:=1 to length(t) do
	       if t[j]='/' then t[j]:='\';
{$endif}	       
	     if t<>'' then
	       begin
	       if t[length(t)]<>dirsep then t+=dirsep;
	       if spathc>=maxspathc 
	         then bmp_error('Maximum amount of search paths reached')
	         else begin
		      spath[spathc]:=t;
                      spathc+=1;
		      end;
	       end;
	     end
	   else if s='-D' then 
	     begin
	     if i>=paramcount then paramerr('Symbol expected after -D');
	     i+=1;
	     main.initsymb(paramstr(i));
	     end
	   else if s='-mm' then 
	     begin
	     if i>=paramcount then paramerr('String expected after -mm');
	     i+=1;
	     bc_pm_d:=true;
	     bc_pm:=paramstr(i);
	     end
	   else if s='-mo' then 
	     begin
	     if (i>=paramcount) or (length(paramstr(i+1))<>1) then paramerr('Character expected after -mo');
	     i+=1;
	     bc_ob_d:=true;
	     bc_ob:=paramstr(i)[1];
	     end
	   else if s='-mc' then 
	     begin
	     if (i>=paramcount) or (length(paramstr(i+1))<>1) then paramerr('Character expected after -mc');
	     i+=1;
	     bc_cb_d:=true;
	     bc_cb:=paramstr(i)[1];
	     end
	   else if s='-mp' then 
	     begin
	     if (i>=paramcount) or (length(paramstr(i+1))<>1) then paramerr('Character expected after -mp');
	     i+=1;
	     bc_pa_d:=true;
	     bc_pa:=paramstr(i)[1];
	     end
           else if s='-eh' then enable_hints:=false
{$ifdef ENABLE_DEBUGOUT}
           else if s='-do' then debugoutput:=true
{$endif}	   
           else if s='-dd' then printdefs:=true
           else if s='-h' then paramerr('')
           else paramerr('Invalid option: '+s);
         end
    else begin
	 if infilec>=maxinfilec 
	    then bmp_error('Maximum amount of input files reached')
	    else begin
{$ifdef WINDOWS}
		 for j:=1 to length(s) do
	    	   if s[j]='/' then s[j]:='\';
{$endif}	       
                 if s='-' 
	           then infile[infilec]:=''
 	           else infile[infilec]:=s;
                 infilec+=1;
		 end;
	 end;
  i+=1;
  end;

if pasmode 
  then begin
       if not bc_pm_d then bc_pm:='//';
       if not bc_ob_d then bc_ob:='[';
       if not bc_cb_d then bc_cb:=']';
       if not bc_pa_d then bc_pa:='#';
       end
  else begin
       if not bc_pm_d then bc_pm:='#';
       if not bc_ob_d then bc_ob:='[';
       if not bc_cb_d then bc_cb:=']';
       if not bc_pa_d then bc_pa:='$';
       if not lidef then lineInfoPattern:='#line %2 "%1"';
       end;

if infilec=0 then 
  begin
  infile[0]:='';
  infilec:=1;
  end;

if fofn<>'' 
  then begin
       try
    	 assign(fo,fofn);
	 rewrite(fo);
	 for i:=0 to infilec-1 do
           main.run(infile[i],fo);
	 close(fo);
       except
         bmp_faterror('Error writing to file '+fofn);
         end;
       end
  else for i:=0 to infilec-1 do
         main.run(infile[i],output);

main.destroy;
halt(bmp_exit);
end.
