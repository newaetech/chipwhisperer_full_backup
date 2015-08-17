#!/usr/bin/ruby
# performnce to wiki

=begin
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2009  Daniel Otte (daniel.otte@rub.de)

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

=begin
 === Twister-256 performance === 
    type:                     hash
    hashsize (bits):           256
    ctxsize (bytes):            80
    blocksize (bits):          512
    init (cycles):             425
    nextBlock (cycles):      36535
    lastBlock (cycles):       8071
    ctx2hash (cycles):       19431

   text    data     bss     dec     hex filename
   6801      32       0    6833    1ab1 bin/bmw_c/bmw_small.o
=end

def get_size_string(impl, algo)
  fmap = File.open('algo_implementation/'+impl+'.algos', 'r')
  fsize = File.open('size_log/'+impl+'.size', 'r')
  modules = nil
  while l=fmap.gets
    if m=l.match(/^([^:]*):(.*)$/)
      if m[1] == algo
        modules = m[2].split(/[\s]+/)
      end
    end
  end
  if modules==nil
    puts("ERROR: no module list found for #{impl}/#{algo} !")
    return nil
  end
  fmap.close()
  str = ''
  sum = 0
  lb = fsize.gets()
  while lb = fsize.gets()
    m = lb.match(/[\s]*([\w]*)[\s]*([\w]*)[\s]*([\w]*)[\s]*([\w]*)[\s]*([\w]*)[\s]*([\w_\/-]*)/)
    name = m[6]+'.o'
    if modules.include?(name)
      str += "<br> \n" + name+': '+m[4]
      sum += m[4].to_i
    end
  end
  fsize.close()
  return sum
end

def process_hashfunction(fin, name, impl)
  lb = fin.readline()
  m = lb.match(/hashsize \(bits\):[\s]*([\d]*)/)
  if(!m)
  	printf("unexpected string %s\n", lb)
  end
  hashsize = m[1].to_i()
  lb = fin.readline()
  m = lb.match(/ctxsize \(bytes\):[\s]*([\d]+)/)
  ctxsize = m[1].to_i()
  lb = fin.readline()
  m = lb.match(/blocksize \(bits\):[\s]*([\d]+)/)
  blocksize = m[1].to_i()
  lb = fin.readline()
  m = lb.match(/init \(cycles\):[\s]*([\d]+)/)
  inittime = m[1].to_i()
  lb = fin.readline()
  m = lb.match(/nextBlock \(cycles\):[\s]*([\d]+)/)
  nextblocktime = m[1].to_i()  
  lb = fin.readline()
  m = lb.match(/lastBlock \(cycles\):[\s]*([\d]+)/)
  lastblocktime = m[1].to_i()
  lb = fin.readline()
  m = lb.match(/ctx2hash \(cycles\):[\s]*([\d]+)/)
  convtime = m[1].to_i()
  begin
    lb = fin.gets()
  end until lb==nil || m = lb.match(/init \(bytes\):[\s]*([\d]*)/)
  if lb
    initstack = m[1].to_i()
    lb = fin.readline()
    m = lb.match(/nextBlock \(bytes\):[\s]*([\d]*)/)
    nextblockstack = m[1].to_i()
    lb = fin.readline()
    m = lb.match(/lastBlock \(bytes\):[\s]*([\d]*)/)
    lastblockstack = m[1].to_i()
    lb = fin.readline()
    m = lb.match(/ctx2hash \(bytes\):[\s]*([\d]*)/)
    convstack = m[1].to_i()
    s1 = (initstack>nextblockstack)?initstack:nextblockstack
    s2 = (lastblockstack>convstack)?lastblockstack:convstack
    stack = (s1>s2)?s1:s2  
  else
    stack = 0
  end  
  size = get_size_string(impl, name)
  printf("| %20s || %6s || %3s || %6d || %7d || %7d || %7d || %7d ||" +
         " %7d || %7d || %9.2f || %7d || || || \n|-\n" , 
        name, $variant, $lang, size, ctxsize, stack, hashsize, blocksize, 
	    inittime, nextblocktime, nextblocktime.to_f/(blocksize/8),
		lastblocktime+convtime)
end


$handlers = Hash.new
$handlers.default = 0
$handlers["hashfunction"] = 1 #process_hashfunction

def process_file(fname)
  fin = File.open(fname, "r")
  impl = fname.match(/([^.]*).txt$/)[1]
  $lang = "asm"
  $lang = "C" if impl.match(/^[^_]*_[cC]/)
  $variant = $lang
  if m=impl.match(/_([^_]*)$/)
    $variant = m[1]
  end
  if $variant == 'c'
    $variant = 'C'
  end
  begin
    begin
	  if fin.eof()
	  	return
	  end
      lb = fin.readline()
    end while !m=lb.match(/=== (.*) performance ===/)
    name = m[1];
    lb = fin.readline()
    m = lb.match(/type:[\s]*([\w]*)/)
    type = m[1]
    if $handlers[type] != 0
    #  handlers[type](fin, name)
    #  puts "DBG: process "fname+'-'+name+'-'+impl
      process_hashfunction(fin, name, impl)
    else
      printf("ERROR: unsupported type: %s !\n", type)
    end	
  end while(true)
  fin.close()
end

for i in (0..ARGV.size-1)
  process_file(ARGV[i])
end







