#!/usr/bin/ruby
# cmacvs_test.rb
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
  $sp.print("cmacvs_list\r")
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

def init_system(algo_select, algo_id)
  $sp.print("echo off \r")
  print("DBG i: " + "echo off \r"+"\n") if $debug
 sleep 1
  $sp.print("cmacvs_set #{algo_select}\r")
  print("DBG i: " + "cmacvs_set #{$algo_select} \r"+"\n") if $debug
  sleep 1
  $sp.print("cmacvs_test#{algo_id} \r")
  print("DBG i: " + "cmacvs_test#{algo_id} \r"+"\n") if $debug
  begin
    line=$sp.gets()
  end while not m=/buffer_size[\s]*=[\s]*0x([0-9A-Fa-f]*)/.match(line)
  $buffer_size = m[1].to_i(16)
end

################################################################################
# get_md                                                                       #
################################################################################

def get_mac
  begin
    line = $sp.gets()
	line = "" if line==nil
	puts("DBG got: "+line) if $debug
  end while not /[\s]*Mac[\s]*=.*/.match(line)
  return line
end

################################################################################
# get_result                                                                   #
################################################################################

def get_result
  begin
    line = $sp.gets()
    line = "" if line==nil
    puts("DBG got: "+line) if $debug
  end while not /[\s]*Result[\s]*=.*/.match(line)
  puts "DBG i: got result: "+line if $debug
  return line
end

################################################################################
# send_md                                                                      #
################################################################################

def send_test(klen, mlen, tlen, key, msg, mac=nil)
  $sp.printf("Klen = %s\n\r", klen)
  $sp.printf("Mlen = %s\n\r", mlen)
  $sp.printf("Tlen = %s\n\r", tlen)
  $sp.printf("Key = %s\n\r",  key)
  $sp.print("Msg = ")
  for i in 0..msg.length-1
    $sp.print(msg[i].chr)
#   print("DBG s: "+ md_string[i].chr) if $debug
#   sleep(0.001)
    if((i%($buffer_size*2)==0)&&(i!=0))
      begin
      line=$sp.gets()
      end while not /\./.match(line)
    end
  end
  $sp.printf("Mac = %s\n\r",  mac) if mac
end

################################################################################
# get_next_kv_pair                                                             #
################################################################################

def get_next_kv_pair(file)
  loop do
    return nil if file.eof
    lb = file.gets()
    m=lb.match(/[\s]*([\w\d_-]*)[\s]*=[\s]*([\w\d_-]*)/)
    puts "DBG i: found #{m[1]} with value #{m[2]}" if m && $debug
    return [m[1],m[2]] if m
  end
end

################################################################################
# run_test_gen                                                                 #
################################################################################

def run_test_gen(filename, skip=0)
  nerrors = 0
  line=1
  if not File.exist?(filename)
  	puts("ERROR file "+filename+" does not exist!")
  	return nerrors
  end
  pos = 0
  file = File.new(filename, "r");
  until file.eof
    params = Hash.new
    begin
      m = get_next_kv_pair(file)
      return nerrors if m==nil
      params[m[0]] = m[1]
    end until m[0]=='Mac'
    if(skip>0)
  	  skip -= 1
  	  redo
  	end
    puts("DBG sending: ") if $debug
	  send_test(params['Klen'], params['Mlen'], params['Tlen'], params['Key'], params['Msg'])
	  avr_md = get_mac()
    a = params['Mac'];
	  b = (/[\s]*Mac[\s]*=[\s]*([0-9a-fA-F]*).*/.match(avr_md))[1];
  	a.upcase!
  	b.upcase!
	  printf("\n%4d (%4d) [%5d]: ", line, (line-1)*$linewidth, params['Count']) if (pos%$linewidth==0 and $linewidth!=0)
	  line += 1               if (pos%$linewidth==0 and $linewidth!=0)
	  #sleep(1)
	  #putc((a==b)?'*':'!')
  	if(a==b)
	    putc('*')
  	else
	    putc('!')
	 #  printf("<%d>",len)
	    printf("\nError @%05d: %s [should]\n           != %s [is]- ",  params['Count'].to_i , a, b)
	    nerrors += 1
	  end
	  pos += 1
  end
  file.close()
  return nerrors
end

################################################################################
# run_test_ver                                                                 #
################################################################################

def run_test_ver(filename, skip=0)
  nerrors = 0
  line=1
  if not File.exist?(filename)
    puts("ERROR file "+filename+" does not exist!")
    return nerrors
  end
  pos = 0
  file = File.new(filename, "r");
  until file.eof
    params = Hash.new
    begin
      m = get_next_kv_pair(file)
      return nerrors if m==nil
      params[m[0]] = m[1]
    end until m[0]=='Result'
    if(skip>0)
      skip -= 1
      redo
    end
    puts("DBG sending: ") if $debug
    send_test(params['Klen'], params['Mlen'], params['Tlen'], params['Key'], params['Msg'], params['Mac'])
    avr_res = get_result()
    a = params['Result'].match(/[\s]*([PF])/)[1];
    b = /[\s]*Result[\s]*=[\s]*([PF])/.match(avr_res)[1];
    a.upcase!
    b.upcase!
    printf("\n%4d (%4d) [%5d]: ", line, (line-1)*$linewidth, params['Count']) if (pos%$linewidth==0 and $linewidth!=0)
    line += 1               if (pos%$linewidth==0 and $linewidth!=0)
    #sleep(1)
    #putc((a==b)?'*':'!')
    if(a==b)
      putc('*')
    else
      putc('!')
   #  printf("<%d>",len)
      printf("\nError @%05d: %s [should]\n           != %s [is]- ",  params['Count'].to_i , a, b)
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

opts = Getopt::Std.getopts("s:f:i:j:hdca")

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
	i = opts["j"] if opts["j"]
	logfile=File.open(conf["PORT"]["testlogbase"]+algo+".txt", "a")
	while conf[algo]["file_#{i}"] != nil
	  puts("Testing #{algo} with #{conf[algo]["file_#{i}"]}")
	  reset_system()
	  init_system(algoa[1], (conf[algo]["file_#{i}_test"]=='gen')?'1':'2')
	  skip=0
	  skip=opts["i"].to_i if opts["i"]
    nerrors=run_test_gen(conf[algo]["file_#{i}"], skip) if conf[algo]["file_#{i}_test"]=='gen'
    nerrors=run_test_ver(conf[algo]["file_#{i}"], skip) if conf[algo]["file_#{i}_test"]=='ver'
	    if nerrors == 0
        puts("\n[ok]")
        logfile.puts("[ok] "+conf[algo]["file_#{i}"]+ " ("+Time.now.to_s()+")")
      else
        puts("\n[errors: "+ nerrors.to_s() +"]")
        logfile.puts("[error] "+nerrors.to_s+" "+conf[algo]["file_#{i}"]+ " ("+Time.now.to_s()+")")
      end
      i += 1
    end
    logfile.close()
  end
end


