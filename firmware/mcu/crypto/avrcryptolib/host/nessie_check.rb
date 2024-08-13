#!/usr/bin/ruby
# nessie_check.rb
=begin
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2008  Daniel Otte (daniel.otte@rub.de)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
=end

$debug = true

def skip_header(file)
  begin
    l = file.gets().strip
    l = '' if !l
  end until /[*]{10,}.*/.match(l)
  begin
    l = file.gets().strip
    l = '' if !l
  end until /[*]{10,}.*/.match(l)
  begin
    l = file.gets().strip
    l = '' if !l
  end until /[=]{5,}.*/.match(l)
  begin
    l = file.gets().strip
    l = '' if !l
  end until /[=]{5,}.*/.match(l)
end

def get_next_assign(file, i)
  key = String.new
  value = String.new
  if($last_assign[i]==nil)
    begin
	  return nil if file.eof
      l = file.gets().strip()
    end until m=/[\s]*([\w]*)[\s]*=[\s]*([0-9a-fA-F]*).*/.match(l)
	value = m[2]
	key = m[1]
	begin
	  return nil if file.eof
	  l = file.gets().strip()
	  if not /[^=]+=[^=]+/.match(l)
	    value += l if /^[0-9A-Fa-f]{5}/.match(l)
	  end
    end until /[^=]+=[^=]+/.match(l)
	$last_assign[i] = l
  else
    m=/[\s]*([\w]*)[\s]*=[\s]*([0-9a-fA-F]*).*/.match($last_assign[i])
	value = m[2]
	key = m[1]
	begin
	  return nil if file.eof
	  l = file.gets().strip()
	  if not /[^=]+=[^=]+/.match(l)
	    value += l if /^[0-9A-Fa-f]{5}/.match(l)
	  end
    end until /[^=]+=[^=]+/.match(l)
	$last_assign[i] = l
  end
  return [key, value]
end

def compare(fname1, fname2)
  file1 = File.new(fname1, "r")
  file2 = File.new(fname2, "r")
  skip_header(file1)
  skip_header(file2)
  pos=0
  begin
#  	puts("checking set")
    a = get_next_assign(file1, 0)
    b = get_next_assign(file2, 1)
	return if(a==nil or b==nil)
	if not $quiet
      puts("") if pos%$linewidth==0 and pos!=0
	  putc((a==b)?'*':'!')
      pos +=1
	end
	if(a!=b and a!=nil and b!=nil)
	  $error += 1
	  puts("a key: "+a[0]+" value: "+a[1]) if $debug
	  puts("b key: "+b[0]+" value: "+b[1]) if $debug
	end
  end until a==nil or b==nil
end

$error = 0
$linewidth=64
$last_assign=[nil, nil]

if ARGV.size<2 or ARGV.size>3
  STDERR.print <<EOF
  Usage: ruby #{$0} [-q|-v] file1 file2
EOF
  exit(1)
end
$quiet = false
if ARGV.size==3
  f1 = ARGV[1]
  f2 = ARGV[2]
  if ARGV[0]=="-q"
    $quiet=true
  end
else
  f1 = ARGV[0]
  f2 = ARGV[1]
end

puts("compare("+f1+", "+f2+")")
compare(f1, f2)
if $error!=0
  puts("[failed] ("+$error.to_s()+")")
else
  puts("[ok]")
end

exit($error)
