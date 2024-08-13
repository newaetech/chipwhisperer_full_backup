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


require 'rubygems'
require 'getopt/std'

=begin
|             Blake-28 ||   C ||   C 
| 10916<br> 
blake_small: 3494<br> 
blake_large: 7142<br> 
blake_common: 256<br> 
memxor: 24 
|   53 || ||  224 ||  512 ||    386 ||  71362 || 1115.03 ||  71893 || || || 
|-

=end


def fix_file(fin, fout, supress)
  loop do
    return if fin.eof()
    comp = Array.new
    i = 0
    lb1 = fin.readline()
    lb2 = fin.readline()
    begin
      comp[i] = fin.readline()
      i += 1
    end until comp[i-1].match(/^\|/)
    sum = 0
    (i-1).times{ |j| sum += comp[j].match(/[^:]*:[\s]*([\d]*)/)[1].to_i}
    fout.print(lb1.chomp)
    if supress
      fout.printf(" || %d |%s", sum, comp.last)    
    else
      fout.printf("\n| %d <br>\n", sum)
      comp.each{ |s| fout.puts(s)}
    end
    begin
      lb1 = fin.readline()
      fout.puts(lb1)
    end until lb1.match(/^\|\-/)
  end
end

################################################################################
# MAIN                                                                         #
################################################################################

fin = STDIN
fout = STDOUT

opts = Getopt::Std.getopts("s")

fin  = File.open(ARGV[0], "r") if ARGV.size > 0
fout = File.open(ARGV[1], "w") if ARGV.size > 1

fix_file(fin, fout, opts["s"])

fin.close  if ARGV.size > 0
fout.close if ARGV.size > 1

