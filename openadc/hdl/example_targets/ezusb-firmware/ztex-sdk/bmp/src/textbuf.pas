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

{$mode objfpc}
unit textbuf;

interface

const maxspathc   = 1024;
      lineInfoPattern : shortstring = '';

type  pdword = ^dword;

      CTextBuf = class;
      CTextBuf = class
        buf          : pchar;    { character buffer }
	li           : pdword;   { line info lineNum shl 12 + file }
        lastbuf,d,d0 : longint;
	allocsize    : longint;  { allocated size }
        last         : CTextBuf;
        killme       : boolean;
        constructor create(var tb:CTextBuf);
        constructor create(var tb:CTextBuf; asize:longint);
        constructor create(var tb:CTextBuf; cb:pchar; lib:pdword; bufsize:longint);
        destructor destroy; override;
        procedure endbuf; virtual;
        end;

      CFileBuf = class(CTextBuf)
        constructor create(var tb:CTextBuf; const fn,ep:ansistring);
        procedure endbuf; override;
       private
        fileNum : longint;
	line    : longint;
        fname   : ansistring;
        errStr  : ansistring; { error trefix }
        stat    : longint;
        f       : file;
        end;

var spath  : array[0..maxspathc-1] of ansistring;
    spathc : longint;

function getFileNum(const fn:ansistring):longint;
function lineInfoStr(li:dword):ansistring;
procedure lineInfoStr(var fo:text; li:dword);

implementation

uses dos,bmpsys;
const textbuf_size = 2048;

{ ********************************************************************* }
{ ****** CTextBuf ***************************************************** }
{ ********************************************************************* }
{ ****** create ******************************************************* }
constructor CTextBuf.create(var tb:CTextBuf);
begin
lastbuf:=-1;
d:=0; d0:=0;
killme:=false;
last:=tb;
tb:=self;
end;

constructor CTextBuf.create(var tb:CTextBuf; asize:longint);
begin
create(tb);
getmem(buf,asize);
getmem(li,asize*4);
allocsize:=asize;
end;

constructor CTextBuf.create(var tb:CTextBuf; cb:pchar; lib:pdword; bufsize:longint);
begin
create(tb);
buf:=cb;
li:=lib;
lastbuf:=bufsize-1;
allocsize:=0;
end;

{ ****** destroy ****************************************************** }
destructor CTextBuf.destroy;
begin
if allocsize>0 then 
  begin
  freemem(buf,allocsize); 
  freemem(li,allocsize*4);
  end;
end;

{ ****** endbuf ******************************************************* }
procedure CTextBuf.endbuf;
begin
killme:=true;
end;

{ ********************************************************************* }
{ ****** CFileBuf ***************************************************** }
{ ********************************************************************* }
{ ****** create ******************************************************* }
constructor CFileBuf.create(var tb:CTextBuf; const fn,ep:ansistring);
begin
inherited create(tb,textbuf_size);
stat:=0;
pointer(fname):=nil;
fname:=fn;
pointer(errStr):=nil;
errStr:=ep;
fileNum:=-1;
line:=0;
end;

{ ****** endbuf ******************************************************* }
procedure CFileBuf.endbuf;
var i,j  : longint;
    e    : dword;
    sr   : searchrec;
    dir  : dirstr;
    name : namestr;
    ext  : extstr;
    b    : boolean;
begin
lastbuf:=-1;
d0+=d;
d:=0;
killme:=true;

if stat=0 then
  begin
  stat:=1;
  if fname<>'' then
     begin
     j:=-1;
//     write(fname+':  ');
     for i:=0 to spathc-1 do
       begin
//       write(spath[i]+'   ');
       findfirst(spath[i]+fname,$27,sr);
       if doserror=0 then 
          begin
          if j<>-1 
	     then bmp_warning(errstr+'`'+spath[i]+fname+''' found, using `'+spath[j]+fname+'''')
             else j:=i;
          end;
       end;
//     writeln(':  '+spath[j]+fname);

     if j=-1 then 
	begin
        bmp_error(errstr+'Can not find file: `'+fname+'''');
        stat:=2;
        exit;
        end;

     fsplit(spath[j]+fname,dir,name,ext);
     b:=true;
     for i:=0 to spathc-1 do
       if dir=spath[i] then b:=false;
     if b then 
        begin
        if spathc>=maxspathc 
           then bmp_error(errstr+'Maximum amount of search paths reached')
           else begin
                spath[spathc]:=dir;
                spathc+=1;
                end;
        end;

     fileNum:=getFileNum(spath[j]+fname);
     line:=0;
     try
       filemode:=0;
       assign(f,spath[j]+fname);
       reset(f,1);
     except
       bmp_error(errstr+'Can not open file: '+spath[j]+fname);
       fname:='';
       stat:=2;
       end;
     end;
  end;

if stat=1 then 
  begin
  if fname='' 
     then begin
          lastbuf:=-1;
          while (not(eof(input))) and (lastbuf<textbuf_size-1) do
            begin
            lastbuf+=1;
            read(buf[lastbuf]);
            end;
          end
     else begin
          try
            blockread(f,buf^,textbuf_size,lastbuf);
            lastbuf-=1;
	  except
	    bmp_error(errstr+'Can read from file: '+fname);
	    lastbuf:=-1;
	    end;
	  end;     

  e:=(line shl 12) + fileNum;
  for i:=0 to lastbuf do
    begin
    if buf[i]=#10 then 
      begin
      e+=4096;
      line+=1;
      end;
    li[i]:=e;
    end;
  	  
  if lastbuf<textbuf_size-1 then 
    begin
    stat:=2;
    if fname<>'' then 
      begin
      try
        close(f);
      except
        end;
      end;
    end;
  end;

if lastbuf>=0 then killme:=false;
end;


{ ********************************************************************* }
{ ****** FileCache **************************************************** }
{ ********************************************************************* }
const fileCount : longint = 0;
var fileCache : array[0..2047] of ansistring;

{ ****** getFileNum *************************************************** }
function getFileNum(const fn:ansistring):longint;
var i  : longint;
begin
i:=0;
while (i<fileCount) and (i<2047) and (fn<>fileCache[i]) do
  i+=1;
if fn<>fileCache[i] then
  begin
  i:=fileCount and 2047;
  fileCache[i]:=fn;
  fileCount+=1;
  end;  
result:=i;
end;

{ ****** lineInfoStr ************************************************* }
function lineInfoStr(li:dword):ansistring;
begin
result:=fileCache[li and 2047]+'('+int2str((li shr 12)+1)+')';
end;

procedure lineInfoStr(var fo:text; li:dword);
var s   : ansistring;
    i,j : longint;
begin
if li and 2048 <> 0 then exit;
pointer(s):=nil;
i:=1;
while i<=length(lineInfoPattern) do
  begin
  if (lineInfoPattern[i]='%') and (i<length(lineInfoPattern)) 
    then case lineInfoPattern[i+1] of
           '1' : begin
		 for j:=1 to length(fileCache[li and 2047]) do
		   if fileCache[li and 2047][j]='\' 
		     then s+='\\'
		     else s+=fileCache[li and 2047][j];
	         i+=1;
	         end;
	   '2' : begin
	         s+=int2str(li shr 12);
     	         i+=1;
     	         end;
	   '%' : begin
                 s+='%';
                 i+=1;
                 end;
           else s+='%';
	 end
    else s+=lineInfoPattern[i];
  i+=1;
  end;
writeln(fo,s);
end;

procedure initFileCache;
var i : longint;
begin
fileCache[0]:='?';
for i:=1 to 2047 do
  fileCache[i]:=fileCache[0];
end;

begin
initFileCache;
end.
