#!/usr/bin/ruby 
# get_performance.rb
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

$debug = false
require 'rubygems'
require 'serialport'
require 'getopt/std'
$conffile_check = Hash.new
$conffile_check.default = 0

################################################################################
# readconfigfile                                                               #
################################################################################

def readconfigfile(fname, conf)
  return conf if $conffile_check[fname]==1
  $conffile_check[fname]=1
  section = "default"
  if not File.exists?(fname)
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
  next if not /=/.match(line)
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
# read_line                                                                    #
################################################################################

def read_line(error_msg=true)
  i=$extended_wait
  begin
    s = $sp.gets()
  end until s or i==0
  if s==nil
    puts("ERROR: read timeout!\n") if error_msg
	return nil
  end	
  s.gsub(/\006/, '');	
end

################################################################################
# readPerformanceVector                                                        #
################################################################################

def readPerformanceVector(param)
  lb=""
  fname=""
  fout=0
  begin
    lb = read_line()
    if lb.match(/End of performance figures/)
      return false
	  end
  	if m=lb.match(/=== (.*) performance ===/) 
  	  fout.close if fout!=0
  	  fname=$dir+m[1]
  	  fname+="."+param if param != ""
  	  fname+=".txt"
  	  fout = File.open(fname, "w+")
  	  printf("> %s \n", fname)	
  	  fout.write(lb)
      else
  	  if fout!=0 && lb!=""
  	    fout.write(lb)
  	  end	
  	end
  end while true
end

################################################################################
# MAIN                                                                         #
################################################################################


opts = Getopt::Std.getopts("f:c:t:a:d")

conf = Hash.new
conf = readconfigfile("/etc/testport.conf", conf)
conf = readconfigfile("~/.testport.conf", conf)
conf = readconfigfile("testport.conf", conf)
conf = readconfigfile(opts["f"], conf) if opts["f"]

#puts conf.inspect

puts("serial port interface version: " + SerialPort::VERSION);
$linewidth = 64
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
=begin
if ARGV.size < 1
  STDERR.print <<EOF
  Usage: ruby #{$0} -c command [-t target_dir] [-a additional specifier]
EOF
  exit(1)
end
=end

command=opts['c']+"\r";
$dir=(opts['t'])?opts['t']:"";
param=(opts['a'])?opts['a']:"";

$linewidth = 16
$extended_wait=100;
$sp.write(command);

while(readPerformanceVector(param))
end

exit(0);


