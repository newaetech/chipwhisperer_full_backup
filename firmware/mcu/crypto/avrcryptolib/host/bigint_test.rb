#!/usr/bin/ruby
# bigint_test.rb
=begin
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2008, 2009  Daniel Otte (daniel.otte@rub.de)

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
$debug = false
require 'rubygems'
require 'serialport'
require 'getopt/std'
require 'fileutils'
require 'date'
$buffer_size = 0
$conffile_check = Hash.new
$conffile_check.default = 0

################################################################################
# readconfigfile                                                               #
################################################################################

def readconfigfile(fname, conf)
  return conf if $conffile_check[fname]==1
  $conffile_check[fname]=1
  section = "default"
  if ! File.exists?(fname)
    return conf
  end
  file = File.open(fname, "r")
  until file.eof
    line = file.gets()
	next if /[\s]*#/.match(line)
	if m=/\[[\s]*([^\s]*)[\s]*\]/.match(line)
	  section=m[1]
	  conf[m[1]] = Hash.new
	  next
	end
	next if ! /=/.match(line)
	m=/[\s]*([^\s]*)[\s]*=[\s]*([^\s]*)/.match(line)
	if m[1]=="include"
	  Dir.glob(m[2]){ |fn| conf = readconfigfile(fn, conf) }
	else
  	  conf[section][m[1]] = m[2]
	end
  end
  file.close()
  return conf
end

################################################################################
# expmod                                                                       #
################################################################################

def expmod(base, power, mod)
  result = 1
  while power > 0
    result = (result * base) % mod if power & 1 == 1
    base = (base * base) % mod
    power >>= 1;
  end
  return result
end

################################################################################
# gcdext                                                                 #
################################################################################

def gcdext(x,y)
  g=1
  while(x&1==0 && y&1==0) do
    x>>=1
    y>>=1
    g<<=1
  end
  u=x; v=y; a=1; b=0; c=0; d=1
  begin
    while(u&1==0) do
      if(a%2==1 || b%2==1)
        a += y
        b -= x
      end
      u>>=1; a>>=1; b>>=1
    end
    while(v&1==0) do
      if(c%2==1 || d%2==1)
        c += y
        d -= x
      end
      v>>=1; c>>=1; d>>=1;
    end
    if(u>=v)
      u -= v; a-=c; b-=d
    else
      v -= u; c-=a; d-=b
    end
  end while(u!=0)
  return[g*v, c, d]
end

################################################################################
# reset_system                                                                 #
################################################################################

def reset_system
  $sp.print("exit\r")
  sleep 0.1
  $sp.print("exit\r")
  sleep 0.1
end

################################################################################
# init_system                                                                  #
################################################################################

def init_system(test_prog)
  begin
    line = $sp.gets()
  end while line!=nil
  $sp.print("echo off \r")
  print("DBG i: " + "echo off \r"+"\n") if $debug
  sleep 0.1
  $sp.print("#{test_prog}\r")
  print("DBG i: " + "#{test_prog} \r"+"\n") if $debug
  sleep 1
end

################################################################################
# wait_for_prompt                                                                  #
################################################################################

def wait_for_prompt(prompt)
  prompt = /[\s]*#{prompt}[\s]*/ if(prompt.class == String)
  start_time = Time.now.to_i
  acc = ''
  begin
    line = $sp.gets()
    puts("DBG got (#{__LINE__}): "+line) if $debug && line
    line = "" if line==nil
    if /^(Error:|Crypto-VS).*/.match(line)
      puts line
      return false
    end
    if (Time.now.to_i- start_time) > $max_timeout
      return false
    end
  acc += line
  end while ! m=prompt.match(acc)
  return m
end

################################################################################
# screen_progress                                                              #
################################################################################
def screen_progress(v)
  if $linepos==0
    printf("\n%5d [%04d]: ", $testno, $size)
  end
  putc((v)?('*'):('!'))
  $testno += 1
  $linepos = ($linepos+1)%$linewidth
end

################################################################################
# add_test                                                                     #
################################################################################

def add_test(a,b)
  begin
    line = $sp.gets()
    line = "" if line==nil
    puts("DBG got: "+line) if $debug
    if /^Error:.*/.match(line)
      puts line
      return false
    end
  end while ! /[\s]*enter a:[\s]*/.match(line)
  $sp.print(a.to_s(16)+" ")
  begin
    line = $sp.gets()
    line = "" if line==nil
    puts("DBG got: "+line) if $debug
    if /^Error:.*/.match(line)
      puts line
      return false
    end
  end while ! /[\s]*enter b:[\s]*/.match(line)
  $sp.print(b.to_s(16)+" ")
  begin
    line = $sp.gets()
    line = "" if line==nil
    puts("DBG got: "+line) if $debug
    if /^Error:.*/.match(line)
      puts line
      return false
    end
  end while ! m=/[\s]*([-]?[0-9a-fA-F]*)[\s]+\+[\s]+([+-]?[0-9a-fA-F]*)[\s]*=[\s]*([+-]?[0-9a-fA-F]*)/.match(line)
  a_ = m[1].to_i(16)
  b_ = m[2].to_i(16)
  c_ = m[3].to_i(16)
  line.chomp!
  if(a_== a && b_ == b && c_ == (a+b))
    $logfile.printf("[pass]: %s\n", line)
    return true
  else
    $logfile.printf("[fail (%s%s%s)]: %s", (a==a_)?"":"a", (b==b_)?"":"b", (c_==a+b)?"":"c",line)
    $logfile.printf(" ; should %s + %s = %s\n", a.to_s(16), b.to_s(16), (a+b).to_s(16))
    return false
  end
  return false
end

################################################################################
# mul_test                                                                     #
################################################################################

def mul_test(a,b)
  begin
    line = $sp.gets()
    line = "" if line==nil
    puts("DBG got: "+line) if $debug
    if /^Error:.*/.match(line)
      puts line
      return false
    end
  end while ! /[\s]*enter a:[\s]*/.match(line)
  $sp.print(a.to_s(16)+" ")
  begin
    line = $sp.gets()
    line = "" if line==nil
    puts("DBG got: "+line) if $debug
    if /^Error:.*/.match(line)
      puts line
      return false
    end
  end while ! /[\s]*enter b:[\s]*/.match(line)
  $sp.print(b.to_s(16)+" ")
  begin
    line = $sp.gets()
    line = "" if line==nil
    puts("DBG got: "+line) if $debug
    if /^Error:.*/.match(line)
      puts line
      return false
    end
  end while ! m=/[\s]*([+-]?[0-9a-fA-F]*)[\s]+\*[\s]+([+-]?[0-9a-fA-F]*)[\s]*=[\s]*([+-]?[0-9a-fA-F]*)/.match(line)
  a_ = m[1].to_i(16)
  b_ = m[2].to_i(16)
  c_ = m[3].to_i(16)
  line.chomp!
  if(a_== a && b_ == b && c_ == (a*b))
    $logfile.printf("[pass]: %s\n", line)
    return true
  else
    $logfile.printf("[fail (%s%s%s)]: %s", (a==a_)?"":"a", (b==b_)?"":"b", (c_==a*b)?"":"c",line)
    $logfile.printf(" ; should %s * %s = %s\n", a.to_s(16), b.to_s(16), (a*b).to_s(16))
    return false
  end
  return false
end

################################################################################
# add_scale_test                                                               #
################################################################################
def add_scale_test_dummy(a, b, scale)
  should = a + (b<<(8*scale))
  printf("[dummy] %s + %s <<8*%04x = %s\n",a.to_s(16), b.to_s(16), scale, should.to_s(16))
end

def add_scale_test(a, b, scale)
  m = wait_for_prompt("enter a:")
  return false if !m 
  puts("DBG put (#{__LINE__}): "+a.to_s(16)+" ") if $debug
  $sp.print(a.to_s(16)+" ")
  m = wait_for_prompt("enter b:")
  return false if !m 
  puts("DBG put (#{__LINE__}): "+b.to_s(16)+" ") if $debug
  $sp.print(b.to_s(16)+" ")
  m = wait_for_prompt("enter scale:")
  return false if !m 
  puts("DBG put (#{__LINE__}): "+scale.to_s(10)+" ") if $debug
  $sp.print(scale.to_s(10)+"\r")
  should = a + (b<<(8*scale))
  m = wait_for_prompt(/[\s]*([-]?[0-9a-fA-F]+)[\s]+\+[\s]+([+-]?[0-9a-fA-F]+)[\s]*<<8\*[\s]*([+-]?[0-9a-fA-F]+)[\s]*=[\s]*([+-]?[0-9a-fA-F]+)/)
  if !m 
    $logfile.printf("[fail (CRASH)]:")
    $logfile.printf(" ; should %s + %s << 8*%s = %s\n", a.to_s(16), b.to_s(16), scale.to_s(16), should.to_s(16))
    return false
  end 
  line = m[0]
  a_ = m[1].to_i(16)
  b_ = m[2].to_i(16)
  s_ = m[3].to_i(16)
  c_ = m[4].to_i(16)
  line.chomp!
  if(a_== a && b_ == b && s_ == scale && c_ == should )
    $logfile.printf("[pass]: %s\n", line)
    return true
  else
    $logfile.printf("[fail (%s%s%s%s)]: %s", (a==a_)?"":"a", (b==b_)?"":"b", (scale==s_)?"":"s",(c_==should)?"":"c",line)
    $logfile.printf(" ; should %s + %s << 8*%s = %s\n", a.to_s(16), b.to_s(16), scale.to_s(16), should.to_s(16))
    return false
  end
  return false
end

################################################################################
# square_test                                                                  #
################################################################################

def square_test(a)
  begin
    line = $sp.gets()
    line = "" if line==nil
    puts("DBG got: "+line) if $debug
    if /^Error:.*/.match(line)
      puts line
      return false
    end
  end while ! /[\s]*enter a:[\s]*/.match(line)
  $sp.print(a.to_s(16)+" ")
  begin
    line = $sp.gets()
    line = "" if line==nil
    puts("DBG got: "+line) if $debug
    if /^Error:.*/.match(line)
      puts line
      return false
    end
  end while ! m=/[\s]*([+-]?[0-9a-fA-F]*)[\s]*\*\*2[\s]*=[\s]*([+-]?[0-9a-fA-F]*)/.match(line)
  a_ = m[1].to_i(16)
  c_ = m[2].to_i(16)
  line.chomp!
  if(a_== a && c_ == (a**2))
    $logfile.printf("[pass]: %s\n", line)
    return true
  else
    $logfile.printf("[fail (%s%s)]: %s", (a==a_)?"":"a", (c_==a**2)?"":"c",line)
    $logfile.printf(" ; should %s **2 = %s\n", a.to_s(16), (a**2).to_s(16))
    return false
  end
  return false
end

################################################################################
# reduce_test                                                                  #
################################################################################

def reduce_test(a,b)
  begin
    line = $sp.gets()
    line = "" if line==nil
    puts("DBG got: "+line) if $debug
    if /^Error:.*/.match(line)
      puts line
      return false
    end
  end while ! /[\s]*enter a:[\s]*/.match(line)
  $sp.print(a.to_s(16)+" ")
  begin
    line = $sp.gets()
    line = "" if line==nil
    puts("DBG got: "+line) if $debug
    if /^Error:.*/.match(line)
      puts line
      return false
    end
  end while ! /[\s]*enter b:[\s]*/.match(line)
  $sp.print(b.to_s(16)+" ")
  line=''
  begin
    line_tmp = $sp.gets()
    line_tmp = '' if line_tmp==nil
    line += line_tmp
    puts("DBG got: "+line) if $debug
    if /^Error:.*/.match(line)
      puts line
      return false
    end
  end while ! m=/[\s]*([+-]?[0-9a-fA-F]*)[\s]+%[\s]+([+-]?[0-9a-fA-F]*)[\s]*=[\s]*([+-]?[0-9a-fA-F]+)/.match(line)
  a_ = m[1].to_i(16)
  b_ = m[2].to_i(16)
  c_ = m[3].to_i(16)
  line.chomp!
  if(a_== a && b_ == b && c_ == (a%b))
    $logfile.printf("[pass]: %s\n", line)
    return true
  else
    $logfile.printf("[fail (%s%s%s)]: %s", (a==a_)?"":"a", (b==b_)?"":"b", (c_==a%b)?"":"c",line)
    $logfile.printf(" ; should %s %% %s = %s\n", a.to_s(16), b.to_s(16), (a%b).to_s(16))
    return false
  end
  return false
end

################################################################################
# expmod_test                                                                  #
################################################################################

def expmod_test(a,b,c)
  begin
    printf("[testing] expmod(%#x, %#x, %#x)\n",a,b,c) if $debug
    line = $sp.gets()
    line = "" if line==nil
    puts("DBG got: "+line) if $debug
    if /^Error:.*/.match(line)
      puts line
      return false
    end
  end while ! /[\s]*enter a:[\s]*/.match(line)
  $sp.print(a.to_s(16)+" ")
  begin
    line = $sp.gets()
    line = "" if line==nil
    puts("DBG got: "+line) if $debug
    if /^Error:.*/.match(line)
      puts line
      return false
    end
  end while ! /[\s]*enter b:[\s]*/.match(line)
  $sp.print(b.to_s(16)+" ")
  begin
    line = $sp.gets()
    line = "" if line==nil
    puts("DBG got: "+line) if $debug
    if /^Error:.*/.match(line)
      puts line
      return false
    end
  end while ! /[\s]*enter c:[\s]*/.match(line)
  $sp.print(c.to_s(16)+" ")
  line=''
  begin
    line_tmp = $sp.gets()
    line_tmp = '' if line_tmp==nil
    line += line_tmp
    puts("DBG got: "+line) if $debug
    if /^Error:.*/.match(line)
      puts line
      return false
    end
  end while ! m=/[\s]*([+-]?[0-9a-fA-F]*)\*\*([+-]?[0-9a-fA-F]*)[\s]+%[\s]+([+-]?[0-9a-fA-F]*)[\s]*=[\s]*([+-]?[0-9a-fA-F]+)/.match(line)
  a_ = m[1].to_i(16)
  b_ = m[2].to_i(16)
  c_ = m[3].to_i(16)
  d_ = m[4].to_i(16)
  line.chomp!
  if(a_== a && b_ == b && c_ == c && d_ ==expmod(a,b,c) )
    $logfile.printf("[pass]: %s\n", line)
    return true
  else
    $logfile.printf("[fail (%s%s%s%s)]: %s", (a==a_)?'':'a', (b==b_)?'':'b', (c_==c)?'':'c', (d_==expmod(a,b,c))?'':'d',line)
    $logfile.printf(" ; should %s**%s %% %s = %s\n", a.to_s(16), b.to_s(16), c.to_s(16), expmod(a,b,c).to_s(16))
    return false
  end
  return false
end

################################################################################
# gcdext_test                                                                  #
################################################################################

def gcdext_test(a,b)
  begin
    line = $sp.gets()
    line = "" if line==nil
    puts("DBG got: "+line) if $debug
    if /^Error:.*/.match(line)
      puts line
      return false
    end
  end while ! /[\s]*enter a:[\s]*/.match(line)
  $sp.print(a.to_s(16)+" ")
  begin
    line = $sp.gets()
    line = "" if line==nil
    puts("DBG got: "+line) if $debug
    if /^Error:.*/.match(line)
      puts line
      return false
    end
  end while ! /[\s]*enter b:[\s]*/.match(line)
  $sp.print(b.to_s(16)+" ")
  line=''
  begin
    line_tmp = $sp.gets()
    line_tmp = '' if line_tmp==nil
    line = ''  if line.end_with?('\n')
    line += line_tmp
    puts("DBG got: "+line) if $debug
    if /^Error:.*/.match(line)
      puts line
      return false
    end
  end while ! m=/gcdext\([\s]*([+-]?[0-9a-fA-F]*)[\s]*,[\s]*([+-]?[0-9a-fA-F]*)[\s]*\)[\s]*=> a = ([+-]?[0-9a-fA-F]+); b = ([+-]?[0-9a-fA-F]+); gcd = ([+-]?[0-9a-fA-F]+)/.match(line)
  a_ = m[1].to_i(16)
  b_ = m[2].to_i(16)
  c_ = m[3].to_i(16)
  d_ = m[4].to_i(16)
  e_ = m[5].to_i(16)
  line.chomp!
  line.gsub!("\r",'')
  line.gsub!("\n",'')
  ref = gcdext(a,b)
  if(a_== a && b_ == b && c_ == ref[1] && d_ == ref[2] && e_== ref[0])
    $logfile.printf("[pass]: %s\n", line)
    return true
  else
    $logfile.printf("[fail (%s%s%s%s%s)]: %s", (a==a_)?'':'a', (b==b_)?'':'b', 
       (c_==ref[1])?'':'c', (d_==ref[2])?'':'d', (e_==ref[0])?'':'e', line)
    $logfile.printf(" ; should gcdext( %s, %s) => a = %s; b = %s; gcd = %s\n",
       a.to_s(16), b.to_s(16), ref[1].to_s(16), ref[2].to_s(16), ref[0].to_s(16))
    return false
  end
  return false
end

################################################################################
# run_test_add                                                                 #
################################################################################

def run_test_add(skip=0)
  length_a_B = skip+1
  length_b_B = skip+1
  begin
    $size = length_a_B
    (0..16).each do |i|
      s_a = rand(2)*2-1
      s_b = rand(2)*2-1
      a = rand(256**length_a_B) * s_a
      b = rand(256**length_a_B) * s_b
      v = add_test(a, b)
      screen_progress(v)
      v = add_test(b, a)
      screen_progress(v)
    end
    (0..16).each do |i|
      s_a = rand(2)-1
      s_b = rand(2)-1
      b_size = rand(length_b_B+1)
      a = rand(256**length_a_B) * s_a
      b = rand(256**b_size) * s_b
      v = add_test(a, b)
      screen_progress(v)      
      v = add_test(b, a)
      screen_progress(v)

    end
    length_a_B += 1
    length_b_B += 1
  end while length_a_B<4096/8
end

################################################################################
# run_test_add_scale                                                           #
################################################################################

def run_test_add_scale(skip=0)
  length_a_B = skip+1
  length_b_B = skip+1
  begin
    $size = length_a_B
    (0..4).each do |i|
      scales = [0, 300]
      16.times { scales << rand(301) }
      scales.sort!
      scales.each do |scale|
        a = rand(256**length_a_B)
        b = rand(256**length_a_B)
        v = add_scale_test(a, b, scale)
        screen_progress(v)
        v = add_scale_test(b, a, scale)
        screen_progress(v)
      end
    end
    (0..4).each do |i|
      scales = [0, 300]
      16.times { scales << rand(301) }
      scales.sort!
      scales.each do |scale|
        b_size = rand(length_b_B+1)+1
        a = rand(256**length_a_B)
        b = rand(256**b_size)
        v = add_scale_test(a, b, scale)
        screen_progress(v)      
        v = add_scale_test(b, a, scale)
        screen_progress(v)
      end
    end
    length_a_B += 10
    length_b_B += 10
  end while length_a_B<4096/8
end

def run_test_add_scale_dummy(skip=0)
  length_a_B = skip+1
  length_b_B = skip+1
  begin
    $size = length_a_B
    (0..4).each do |i|
      scales = [0, 300]
      16.times { scales << rand(301) }
      scales.sort!
      scales.each do |scale|
        a = rand(256**length_a_B)
        b = rand(256**length_a_B)
        v = add_scale_test_dummy(a, b, scale)
        v = add_scale_test_dummy(b, a, scale)
      end
    end
    (0..4).each do |i|
      scales = [0, 300]
      16.times { scales << rand(301) }
      scales.sort!
      scales.each do |scale|
        b_size = rand(length_b_B+1)
        a = rand(256**length_a_B)
        b = rand(256**b_size)
        v = add_scale_test_dummy(a, b, scale)
        v = add_scale_test_dummy(b, a, scale)
      end
    end
    length_a_B += 10
    length_b_B += 10
  end while length_a_B<4096/8
end

################################################################################
# run_test_mul                                                                 #
################################################################################

def run_test_mul(skip=0)
  length_a_B = skip+1
  length_b_B = skip+1
  begin
    $size = length_a_B
    (0..16).each do |i|
      s_a = rand(2)*2-1
      s_b = rand(2)*2-1
      a = rand(256**length_a_B) * s_a
      b = rand(256**length_a_B) * s_b
      v = mul_test(a, b)
      screen_progress(v)
      v = mul_test(b, a)
      screen_progress(v)
    end
    (0..16).each do |i|
      s_a = rand(2)-1
      s_b = rand(2)-1
      b_size = rand(length_b_B+1)
      a = rand(256**length_a_B) * s_a
      b = rand(256**b_size) * s_b
      v = mul_test(a, b)
      screen_progress(v)      
      v = mul_test(b, a)
      screen_progress(v)
    end
    length_a_B += 1
    length_b_B += 1
  end while length_a_B<4096/8
end

################################################################################
# run_test_square                                                              #
################################################################################

def run_test_square(skip=0)
  length_a_B = skip+1
  begin
    $size = length_a_B
    (0..16).each do |i|
      a = rand(256**length_a_B)
      v = square_test(a)
      screen_progress(v)
    end
    length_a_B += 1
  end while length_a_B<4096/8
end

################################################################################
# run_test_reduce                                                              #
################################################################################

def run_test_reduce(skip=0)
  length_a_B = skip+1
  length_b_B = skip+1
  begin
    $size = length_a_B
    (0..16).each do |i|
      a = rand(256**length_a_B)
      b = rand(256**length_a_B)+1
      v = reduce_test(a, b)
      screen_progress(v)
      end
    (0..16).each do |i|
      b_size = rand(length_b_B+1)
      a = rand(256**length_a_B)
      b = rand(256**b_size)+1 
      v = reduce_test(a, b)
      screen_progress(v)      
      end
    length_a_B += 1
    length_b_B += 1
  end while length_a_B<4096/8
end

################################################################################
# run_test_expmod                                                              #
################################################################################

def run_test_expmod(skip=0)
  length_a_B = skip+1
  length_b_B = skip+1
  length_c_B = skip+1
  begin
    $size = length_a_B
    (0..16).each do |i|
      a = rand(256**length_a_B)
      b = rand(256**length_b_B)+1
      c = rand(256**length_c_B)+1
      v = expmod_test(a, b, c)
      screen_progress(v)
      end
    (0..16).each do |i|
      b_size = rand(length_b_B+1)
      a = rand(256**length_a_B)
      b = rand(256**b_size)+1 
      c = rand(256**b_size)+1
      v = expmod_test(a, b, c)
      screen_progress(v)      
      end
    length_a_B += 1
    length_b_B += 1
  end while length_a_B<4096/8
end

################################################################################
# run_test_gcdext                                                              #
################################################################################

def run_test_gcdext(skip=0)
  length_a_B = skip+1
  length_b_B = skip+1
  begin
    $size = length_a_B
    (0..16).each do |i|
      a = rand(256**length_a_B)
      b = rand(256**length_a_B)+1
      v = gcdext_test(a, b)
      $logfile.flush()
      screen_progress(v)
      end
    (0..16).each do |i|
      b_size = rand(length_b_B+1)
      a = rand(256**length_a_B)
      b = rand(256**b_size)+1 
      v = gcdext_test(a, b)
      $logfile.flush()
      screen_progress(v)      
      end
    length_a_B += 1
    length_b_B += 1
  end while length_a_B<4096/8
end

def init_serialport(conf)
  puts("serial port interface version: " + SerialPort::VERSION);
  $linewidth = 64
  $linepos = 0
  $testno = 0
  params = { "baud"       => conf["PORT"]["baud"].to_i,
              "data_bits" => conf["PORT"]["databits"].to_i,
              "stop_bits" => conf["PORT"]["stopbits"].to_i,
              "parity"    => SerialPort::NONE }
  params["paraty"] = SerialPort::ODD   if conf["PORT"]["paraty"].downcase == "odd"
  params["paraty"] = SerialPort::EVEN  if conf["PORT"]["paraty"].downcase == "even"
  params["paraty"] = SerialPort::MARK  if conf["PORT"]["paraty"].downcase == "mark"
  params["paraty"] = SerialPort::SPACE if conf["PORT"]["paraty"].downcase == "space"
  
  puts("\nPort: "+conf["PORT"]["port"]+"@"    +
                  params["baud"].to_s      +
                  " "                      +
                  params["data_bits"].to_s +
                  conf["PORT"]["paraty"][0,1].upcase +
                  params["stop_bits"].to_s +
                  "\n")
  
  $sp = SerialPort.new(conf["PORT"]["port"], params)
  
  $sp.read_timeout=1000; # 5 minutes
  $sp.flow_control = SerialPort::SOFT
  
  reset_system()
end

################################################################################
# MAIN                                                                         #
################################################################################

opts = Getopt::Std.getopts("s:f:i:a:hd")

conf = Hash.new
conf = readconfigfile("/etc/testport.conf", conf)
conf = readconfigfile("~/.testport.conf", conf)
conf = readconfigfile("testport.conf", conf)
conf = readconfigfile(opts["f"], conf) if opts["f"]

#puts conf.inspect
init_serialport(conf)

  $max_timeout = 5 * 60

if opts['d']
  $debug = true
end

logfilename = conf['PORT']['testlogbase']+'bigint.txt'
if File.exists?(logfilename)
  i=1
  begin
    logfilename = sprintf('%s%04d%s', conf['PORT']['testlogbase']+'bigint_',i,'.txt')
    i+=1
  end while(File.exists?(logfilename))
  while(i>2) do
    n1 = sprintf('%s%04d%s', conf['PORT']['testlogbase']+'bigint_',i-2,'.txt')
    n2 = sprintf('%s%04d%s', conf['PORT']['testlogbase']+'bigint_',i-1,'.txt')
    File.rename(n1, n2)
    printf("%s -> %s\n", n1, n2) 
    i-=1
  end
  n1 = sprintf('%s%s', conf['PORT']['testlogbase'],'bigint.txt')
  n2 = sprintf('%s%04d%s', conf['PORT']['testlogbase']+'bigint_',1,'.txt')
  File.rename(n1, n2) 
  printf("%s -> %s\n", n1, n2)  
  logfilename = conf['PORT']['testlogbase']+'bigint.txt' 
end
$logfile = File.open(logfilename, 'w')
printf("logfile: %s\n", logfilename)
$logfile.sync = true
$logfile.printf("bigint test from: %s\n", Time.now.to_s)
$logfile.printf("skip = %s\n", opts['s']) if opts['s']
$logfile.printf("seed = 0x%X\n", 0xdeadbeef)

tests = Hash.new
tests['a'] = proc {|x| run_test_add(x) }
tests['m'] = proc {|x| run_test_mul(x) }
tests['x'] = proc {|x| run_test_add_scale(x) }
tests['s'] = proc {|x| run_test_square(x) }
tests['r'] = proc {|x| run_test_reduce(x) }
tests['e'] = proc {|x| run_test_expmod(x) }
tests['g'] = proc {|x| run_test_gcdext(x) }
init_str = Hash.new
init_str['a'] = 'add-test'
init_str['x'] = 'add-scale-test'
init_str['m'] = 'mul-test'
init_str['s'] = 'square-test'
init_str['r'] = 'reduce-test'
init_str['e'] = 'expmod-test'
init_str['g'] = 'gcdext-test'

srand(0xdeadbeef)

if opts['a']
  opts['a'].each_char do |x|
    if tests[x]
      puts init_str[x]
      init_system(init_str[x])
      tests[x].call(opts['s']?opts['s'].to_i():0) 
    else
      puts "no test defiened for '#{x}'"
    end  
  end
else
  'amsre'.each_char do |x|
    if tests[x]
      puts init_str[x]
      init_system(init_str[x])
      tests[x].call(opts['s']?opts['s'].to_i():0) 
    else
      puts "no test defiened for '#{x}'"
    end  
  end
end


$logile.close()
