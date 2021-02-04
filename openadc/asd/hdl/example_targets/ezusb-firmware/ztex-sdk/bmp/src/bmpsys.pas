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
unit bmpsys;

interface

const bmp_exit_error      = 1;
      bmp_exit_faterror   = 2;
      bmp_exit_paramerror = 3;
      bmp_exit_usererror  = 100;  { //error[...] }

const bmp_exit    : longint = 0;

procedure bmp_warning(const s:ansistring);
procedure bmp_error(const s:ansistring);
procedure bmp_faterror(const s:ansistring);
function min(i,j:longint):longint;
function max(i,j:longint):longint;
function int2str(i:longint):shortstring;

implementation

procedure bmp_warning(const s:ansistring);
begin
writeln(stderr,'Warning: ',s);
end;

procedure bmp_error(const s:ansistring);
begin
writeln(stderr,'Error: ',s);
bmp_exit:=1;
end;

procedure bmp_faterror(const s:ansistring);
begin
writeln(stderr,'Fatal error: ',s);
halt(2);
end;

function min(i,j:longint):longint;
begin
if i<j then min:=i
       else min:=j;
end;

function max(i,j:longint):longint;
begin
if i>j then max:=i
       else max:=j;
end;

function int2str(i:longint):shortstring;
begin
str(i,int2str);
end;


end.
