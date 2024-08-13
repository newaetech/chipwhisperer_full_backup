#!/usr/bin/ruby 
# get_test.rb
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

require 'rubygems'
require 'serialport'

$debug = false
$progress_dots = false

def read_line(error_msg=true)
  begin
    i = $extended_wait
    begin
      s = $sp.gets()
      i -= 1
    end while !s && i > 0
    if s==nil
      puts("ERROR: read timeout!\n") if error_msg
      return nil
    end
  putc('.') if s.include?(6.chr) && $progress_dots	
  puts(s.inspect) if $debug
  end while s == 6.chr
  s.gsub(/\006/, '')
end

def readTestVector(param)
  fname=$dir;
  lb="";
  buffer="";
  set=0;
  vector=0;
  begin
    lb = read_line(false)
	if (m=/unknown command/.match(lb) || m=/[Ee][Rr]{2}[Oo][Rr]/.match(lb))
      puts("ERROR: "+lb);
      exit(2);
    end
    if(lb==nil)
      return false;
    end
  end while(m=/\*+/.match(lb));
  
  buffer += lb;
  begin
    lb = read_line()
    if(lb==nil)
      return false;
    end
    buffer+=lb;
  end while(m=/\*.*/.match(lb));

  while(!(m=/Test vectors/.match(lb))) 
    m=/[^:]*:[\s]([A-Za-z0-9_-]*)/.match(lb);
    if(m) 
      fname+=m[1]+".";
    end
    return false if lb==nil
    buffer+=lb;
    lb = read_line();
  end
  if(param!="")
    fname+=param+".";
  end
  puts("-> "+fname+"txt");
  file=File.new(fname+"txt", "w");
  file.sync = true
    buffer+=lb;
    file.write(buffer);
    begin
      if (m=/Test\ vectors\ \-\-\ set[\s]+([0-9]+)/.match(lb))
	set=m[1].to_i;
	print("\nSet "+m[1]+":");
      end
      if (m=/Set [0-9]*, vector#[\s]*([0-9]+):/.match(lb))
        vector=m[1].to_i;
	#print(" "+m[1]);
	if(vector!=0 && vector % $linewidth==0)
	  print("\n      ")
	end
        printf(" %4u", vector);
      end
      lb=read_line();
      if(lb==nil)
        file.close();
        return false;
      end
      file.write(lb);
    end while(!m=/End of test vectors/.match(lb));
    puts("\n");
  file.close();
  return true
end


if ARGV.size < 5
  STDERR.print <<EOF
  Usage: ruby #{$0} port bps nbits stopb command [target_dir] [additional specifier]
EOF
  exit(1)
end

command=ARGV[4]+"\r";
$dir=(ARGV.size>=6)?ARGV[5]:"";
param=(ARGV.size>=7)?ARGV[6]:"";

puts("\nPort: "+ARGV[0]+ "@"+ARGV[1]+" "+ARGV[2]+"N"+ARGV[3]+"\n");
$linewidth = 16
$sp = SerialPort.new(ARGV[0], ARGV[1].to_i, ARGV[2].to_i, ARGV[3].to_i, SerialPort::NONE);
$sp.read_timeout=1000; # 1 second
$extended_wait=100000;
$sp.write(command);

if(readTestVector(param)==false)
  puts("ERROR: test seems not to be implemented");
  exit(0); # 3 
end

while(readTestVector(param))
end

exit(0);


