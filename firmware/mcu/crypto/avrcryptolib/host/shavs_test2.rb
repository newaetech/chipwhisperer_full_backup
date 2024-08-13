#!/usr/bin/ruby
# shavs_test.rb
=begin
    This file is part of the ARM-Crypto-Lib.
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

$buffer_size = 0 # set automatically in init_system
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
# reset_system                                                                 #
################################################################################

def reset_system
  $sp.print("exit\r")
  sleep 0.1
  $sp.print("exit\r")
  sleep 0.1
end

################################################################################
# scan_system                                                                  #
################################################################################

def scan_system
  algos = Hash.new
  $sp.print("shavs_list\r")
  while true
    line=$sp.gets()
    return algos if /^>$/.match(line)
    if m = /[\*\ ]([a-z]):[\s]*([a-zA-Z0-9+_-]+)/.match(line)
      algos[m[2]]=m[1]
    end
  end
end

################################################################################
# init_system                                                                  #
################################################################################

def init_system(algo_select)
  $sp.print("echo off \r")
  print("DBG i: " + "echo off \r"+"\n") if $debug
 sleep 0.1
  $sp.print("shavs_set #{algo_select}\r")
  print("DBG i: " + "shavs_set #{$algo_select} \r"+"\n") if $debug
  sleep 0.1
  $sp.print("shavs_test1 \r")
  print("DBG i: " + "shavs_test1 \r"+"\n") if $debug
  begin
    line=$sp.gets()
  end while not m=/buffer_size[\s]*=[\s]*0x([0-9A-Fa-f]*)/.match(line)
  $buffer_size = m[1].to_i(16)
  printf("buffer_size = %d\n", $buffer_size)
end

################################################################################
# get_md                                                                       #
################################################################################

def get_md
  begin
    line = $sp.gets()
        puts("DBG got: " + line.inspect) if $debug
	line = "" if line==nil
   #	puts("DBG got: "+line) if $debug
  end while not /[\s]*MD[\s]*=.*/.match(line)
  return line
end

################################################################################
# send_md                                                                      #
################################################################################
=begin
def send_md(md_string)
  $sp.print("Msg = ")
  for i in 0..md_string.length-1
    $sp.print(md_string[i].chr)
	  if((i%($buffer_size*2)==0)&&(i!=0))
	    begin
		    line=$sp.gets()
	    end while not /\./.match(line)
 	  end
  end
end
=end
def send_md(md_string)
#  puts 'DBG: send_md; md_string.length = '+md_string.length.to_s+'; buffer_size = '+$buffer_size.to_s
  bs = $buffer_size*2
  $sp.print("Msg = ")
  $sp.print(md_string[0])
  md_string = md_string[1..-1]
  for i in 0..((md_string.length)/bs)
#    puts 'DBG bulk send'
    if(md_string.length-i*bs<=bs)
 #     puts "DBG: i="+i.to_s()
      t = md_string[(i*bs)..-1]
      printf("sending final %d chars: %s\n", t.length, t[-10..-1]) if $debug
      $sp.print(t) 
      return
    end
    t = md_string[(i*bs)..((i+1)*bs-1)]
    printf("sending %d chars: %s\n", t.length, t[-10..-1]) if $debug
    $sp.print(t)
    sleep(0.1)
    print("going to wait ... : ") if $debug
    begin
     line=$sp.gets()
     puts(line.inspect) if $debug
     line='' if !line
    end while ! /\./.match(line) 
  end
end


################################################################################
# run_test                                                                     #
################################################################################

def run_test(filename, skip=0)
  nerrors = 0
  line=1
  if not File.exist?(filename)
  	puts("ERROR file "+filename+" does not exist!")
  	return nerrors
  end
  pos = 0
  file = File.new(filename, "r");
  until file.eof
    begin
      lb=file.gets()
#	  printf("DBG info: file read: %s", lb)
    end while not (file.eof or (/[\s]*Len[\s]*=/.match(lb)))
#	puts("got ya")
	if file.eof
	  file.close()
	  return nerrors
	end
	len = /[\s]*Len[\s]*=[\s]*([0-9]*)/.match(lb)[1].to_i
	if(skip>0)
	  skip -= 1
	  redo
	end
    puts("DBG sending: "+lb) if $debug
	$sp.print(lb.strip)
	$sp.print("\r")
    begin
	    lb=file.gets()
    end while not (file.eof or (m=/[\s]*Msg[\s]*=[\s]*([0-9a-fA-F]*)/.match(lb)))
    return if file.eof
    puts("DBG sending: "+lb) if $debug
	send_md(m[1])
    puts("DBG sending [done] getting...") if $debug
	avr_md = get_md()
    puts("DBG getting [done]") if $debug
    begin
	    lb=file.gets()
    end while not /[\s]*MD[\s]*=.*/.match(lb)
	a = (/[\s]*MD[\s]*=[\s]*([0-9a-fA-F]*).*/.match(lb))[1];
	b = (/[\s]*MD[\s]*=[\s]*([0-9a-fA-F]*).*/.match(avr_md))[1];
	a.upcase!
	b.upcase!
	printf("\n%4d (%4d) [%5d]: ", line, (line-1)*$linewidth, len) if (pos%$linewidth==0 and $linewidth!=0)
	line += 1               if (pos%$linewidth==0 and $linewidth!=0)
	#sleep(1)
	#putc((a==b)?'*':'!')
	if(a==b)
	  putc('*')
	else
	  putc('!')
	#  printf("<%d>",len)
	  printf("\nError @%05d: %s [should]\n           != %s [is]- ",len, a, b)
	  nerrors += 1
	end
	pos += 1
  end
  file.close()
  return nerrors
end


################################################################################
# MAIN                                                                         #
################################################################################
#
# Options:
#  -s {algo_letter} run only specified algos
#  -f <file>        also read config from <file>
#  -i <n>           skip until test nr. <n>
#  -j <n>           start with testfile <n>
#  -o               use just one testfile
#  -h ???
#  -d               enable debug mode
#  -c ???
#  -a ???

opts = Getopt::Std.getopts("s:f:i:j:hdcao")

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

reset_system()
algos=scan_system()
#puts algos.inspect

if opts["d"]
  $debug = true
end

if opts["s"]
  algos_rev = algos.invert
  algo_tasks = Array.new
  opts["s"].each_byte{ |x|
    if algos_rev[x.chr]
      algo_tasks << [algos_rev[x.chr],x.chr]
    end
  }
else
  algo_tasks=algos.sort
end

algo_tasks.each do |algoa|
  algo = algoa[0]
  if conf[algo]==nil
    puts("No test-set defined for #{algo} \r\n")
    next
  else
    i=0
    i = opts["j"].to_i if opts["j"]
    logfile=File.open(conf["PORT"]["testlogbase"]+algo+".txt", "a")
    while conf[algo]["file_#{i}"] != nil
      puts("Testing #{algo} with #{conf[algo]["file_#{i}"]}")
      reset_system()
      init_system(algoa[1])
      skip=0
      skip=opts["i"].to_i if opts["i"]
      nerrors=run_test(conf[algo]["file_#{i}"], skip)
      if nerrors == 0
        puts("\n[ok]")
        logfile.puts("[ok] "+conf[algo]["file_#{i}"]+ " ("+Time.now.to_s()+")")
      else
        puts("\n[errors: "+ nerrors.to_s() +"]")
        logfile.puts("[error] "+nerrors.to_s+" "+conf[algo]["file_#{i}"]+ " ("+Time.now.to_s()+")")
      end
      i = i+1
      break if opts["o"]
    end
    logfile.close()
  end
end

=begin
nerrors = 0
for i in (5..(ARGV.size-1))
  nerrors = run_test(ARGV[i])
  if nerrors == 0
    puts("\n[ok]")
  else
    puts("\n[errors: "+ nerrors.to_s() +"]")
  end
end
 $sp.print("EXIT\r");

#exit(0);
=end

