#!/usr/bin/ruby 
# shavs_test.rb
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

shift_values = [ 5, 36, 13, 58, 26, 53, 11, 59, 
                56, 28, 46, 44, 20, 35, 42, 50,
               
			    38, 48, 34, 26, 33, 39, 29, 33, 
                30, 20, 14, 12, 49, 27, 26, 51,
                50, 43, 15, 58,  8, 41, 11, 39,
                53, 31, 27,  7, 42, 14,  9, 35,
               
			    55, 25, 33, 34, 28, 17, 58, 47,
                43, 25,  8, 43,  7,  6,  7, 49,
                37, 46, 18, 25, 47, 18, 32, 27,
                40, 13, 57, 60, 48, 25, 45, 58,
                16, 14, 21, 44, 51, 43, 19, 37,
                22, 13, 12,  9,  9, 42, 18, 48,
                38, 52, 32, 59, 35, 40,  2, 53,
                12, 57, 54, 34, 41, 15, 56, 56 ]

def transform_shift(value)
  byteshift = (value+3)/8
  singleshift = value%8
  if singleshift>4
 #   byteshift += 1
	singleshift -= 8
  end
  return [singleshift, byteshift]
end

def transform_singleshift(value)
  if(value>=0)
    return value
  end
  return 0x08+(value*-1)
end

bs_hist = Hash.new
bs_hist.default = 0
ss_hist = Hash.new
ss_hist.default = 0
shift_values.each{|v|
  
  a = transform_shift(v)
  printf("%2d = %2d * 8 %+2d\n", v, a[1], a[0])
  bs_hist[a[1]] += 1
  ss_hist[a[0]] += 1
}

puts("byteshift histogram:")
for i in 0..7
  printf("%d: %4d\n", i, bs_hist[i])
 end

puts("singleshift histogram:")
for i in -3..4
  printf("%+d: %4d\n", i, ss_hist[i])
 end

puts "\ntransformed:"
(0..shift_values.length-1).each{|i|
  puts " for 256 bit:" if i==0
  puts " for 512 bit:" if i==16
  puts " for 1024 bit:" if i==16+32
  
  a = transform_shift(shift_values[i])
  a[0] = transform_singleshift(a[0])
  printf("0x%01x%01x, ", a[1], a[0])
  puts("") if (i%8==7)
}


puts "\ntransformed (decryption):"
(0..shift_values.length-1).each{|i|
  puts " for 256 bit:" if i==0
  puts " for 512 bit:" if i==16
  puts " for 1024 bit:" if i==16+32
  
  a = transform_shift(shift_values[(i/8)*8+7-(i%8)])
  a[0] = transform_singleshift(a[0])
  printf("0x%01x%01x, ", a[1], a[0])
  puts("") if (i%8==7)
}
