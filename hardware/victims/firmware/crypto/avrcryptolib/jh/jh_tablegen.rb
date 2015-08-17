#!/usr/bin/ruby
# jh_tablegen.rb
=begin
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2010  Daniel Otte (daniel.otte@rub.de)

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

$sbox0 = [  9,  0,  4, 11, 13, 12,  3, 15,  1, 10,  2,  6,  7,  5,  8, 14]
$sbox1 = [  3, 12,  6, 13,  5,  7,  1,  9, 15,  2,  0,  4, 11, 10, 14,  8]

# (b) ^= ( ( (a) << 1) ^ ( (a) >> 3) ^ (( (a) >> 2) & 2) ) & 0xf;   
# (a) ^= ( ( (b) << 1) ^ ( (b) >> 3) ^ (( (b) >> 2) & 2) ) & 0xf;     

def L_transform(v,w)
  v = v&0x0f; w = w&0x0f
 # a = ((v<<1)|(v>>3))&0xf
 # d = a^w^((v>>2)&2)
 # d_ = ((d<<1)|(d>>3))&0xf
 # c = v^d_^((d>>2)&2)
 # return c, d
 w ^= ((v<<1)^(v>>3)^((v>>2)&2))&0x0f
 v ^= ((w<<1)^(w>>3)^((w>>2)&2))&0x0f
 return v,w
end

def L_transform_inv(v,w)
  a,b = L_transform(w,v)
  return b,a
end


def pi_permute(a, d=8)
  b=Array.new
  (2**(d-2)).times{ |i| b[4*i+0] = a[4*i+0]}
  (2**(d-2)).times{ |i| b[4*i+1] = a[4*i+1]}
  (2**(d-2)).times{ |i| b[4*i+2] = a[4*i+3]}
  (2**(d-2)).times{ |i| b[4*i+3] = a[4*i+2]}
  return b
end

def p_permute(a, d=8)
  b=Array.new
  (2**(d-1)).times{ |i| b[i] = a[2*i]}
  (2**(d-1)).times{ |i| b[i+2**(d-1)] = a[2*i+1]}
  return b
end

def phi_permute(a, d=8)
  b=Array.new
  (2**(d-1)).times{ |i| b[i] = a[i]}
  (2**(d-2)).times{ |i| b[2**(d-1)+2*i+0] = a[2**(d-1)+2*i+1]}
  (2**(d-2)).times{ |i| b[2**(d-1)+2*i+1] = a[2**(d-1)+2*i+0]}
  return b
end

def test_permutations
  a = Array.new
  16.times{|i| a[i]=i}
  puts a.inspect
  b = pi_permute(a,4)
  puts b.inspect
  b = p_permute(a,4)
  puts b.inspect
  b = phi_permute(a,4)
  puts b.inspect
end

def permutation(a, d=8)
  return phi_permute(p_permute(pi_permute(a,d),d),d)
end

$permutation_table=Array.new

def gen_permutation_table
  a = Array.new
  256.times {|i| a<<i}
  $permutation_table=permutation(a,8)
end

def print_permute_table
  printf("\npermutation table:\n\t")
  16.times do |y|
    16.times do |x| 
      printf("0x%02X, ", $permutation_table[y*16+x])
    end
    printf("\n\t")
  end
  puts("")
end

$nibble_invert=false


def split_byte(v)
  a = (v>>4)&0xf
  b = v&0xf
  return b,a if $nibble_invert
  return a,b
end

def join_nibbles(a,b)
  if($nibble_invert)
    v = ((b<<4)|(a&0xf))&0xff
  else
    v = ((a<<4)|(b&0xf))&0xff
  end
  return v
end

$lutbox=Array.new

def gen_lutboxes
  4.times do |box|
    $lutbox[box]=Array.new
    256.times do |i|
      a,b = split_byte(i)
      a = ((box&2)==2)?$sbox1[a]:$sbox0[a]
      b = ((box&1)==1)?$sbox1[b]:$sbox0[b]
      c,d = L_transform(a,b)
      $lutbox[box]<<join_nibbles(c,d)
    end
  end
end

def print_lutbox(n)
  printf("\nSBOX%d:\n\t",n)
  256.times do |i|
    printf("0x%02X, ",$lutbox[n][i])
    print("\n\t") if(i%16==15)
  end
end



$round_const = [
  0x6a09e667f3bcc908b2fb1366ea957d3e3adec17512775099da2f590b0667322a,
  0xbb896bf05955abcd5281828d66e7d99ac4203494f89bf12817deb43288712231,
  0x1836e76b12d79c55118a1139d2417df52a2021225ff6350063d88e5f1f91631c,
  0x263085a7000fa9c3317c6ca8ab65f7a7713cf4201060ce886af855a90d6a4eed,
  0x1cebafd51a156aeb62a11fb3be2e14f60b7e48de85814270fd62e97614d7b441,
  0xe5564cb574f7e09c75e2e244929e9549279ab224a28e445d57185e7d7a09fdc1,
  0x5820f0f0d764cff3a5552a5e41a82b9eff6ee0aa615773bb07e8603424c3cf8a,
  0xb126fb741733c5bfcef6f43a62e8e5706a26656028aa897ec1ea4616ce8fd510,
  0xdbf0de32bca77254bb4f562581a3bc991cf94f225652c27f14eae958ae6aa616,
  0xe6113be617f45f3de53cff03919a94c32c927b093ac8f23b47f7189aadb9bc67,
  0x80d0d26052ca45d593ab5fb3102506390083afb5ffe107dacfcba7dbe601a12b,
  0x43af1c76126714dfa950c368787c81ae3beecf956c85c962086ae16e40ebb0b4,
  0x9aee8994d2d74a5cdb7b1ef294eed5c1520724dd8ed58c92d3f0e174b0c32045,
  0x0b2aa58ceb3bdb9e1eef66b376e0c565d5d8fe7bacb8da866f859ac521f3d571,
  0x7a1523ef3d970a3a9b0b4d610e02749d37b8d57c1885fe4206a7f338e8356866,
  0x2c2db8f7876685f2cd9a2e0ddb64c9d5bf13905371fc39e0fa86e1477234a297,
  0x9df085eb2544ebf62b50686a71e6e828dfed9dbe0b106c9452ceddff3d138990,
  0xe6e5c42cb2d460c9d6e4791a1681bb2e222e54558eb78d5244e217d1bfcf5058,
  0x8f1f57e44e126210f00763ff57da208a5093b8ff7947534a4c260a17642f72b2,
  0xae4ef4792ea148608cf116cb2bff66e8fc74811266cd641112cd17801ed38b59,
  0x91a744efbf68b192d0549b608bdb3191fc12a0e83543cec5f882250b244f78e4,
  0x4b5d27d3368f9c17d4b2a2b216c7e74e7714d2cc03e1e44588cd9936de74357c,
  0x0ea17cafb8286131bda9e3757b3610aa3f77a6d0575053fc926eea7e237df289,
  0x848af9f57eb1a616e2c342c8cea528b8a95a5d16d9d87be9bb3784d0c351c32b,
  0xc0435cc3654fb85dd9335ba91ac3dbde1f85d567d7ad16f9de6e009bca3f95b5,
  0x927547fe5e5e45e2fe99f1651ea1cbf097dc3a3d40ddd21cee260543c288ec6b,
  0xc117a3770d3a34469d50dfa7db020300d306a365374fa828c8b780ee1b9d7a34,
  0x8ff2178ae2dbe5e872fac789a34bc228debf54a882743caad14f3a550fdbe68f,
  0xabd06c52ed58ff091205d0f627574c8cbc1fe7cf79210f5a2286f6e23a27efa0,
  0x631f4acb8d3ca4253e301849f157571d3211b6c1045347befb7c77df3c6ca7bd,
  0xae88f2342c23344590be2014fab4f179fd4bf7c90db14fa4018fcce689d2127b,
  0x93b89385546d71379fe41c39bc602e8b7c8b2f78ee914d1f0af0d437a189a8a4,
  0x1d1e036abeef3f44848cd76ef6baa889fcec56cd7967eb909a464bfc23c72435,
  0xa8e4ede4c5fe5e88d4fb192e0a0821e935ba145bbfc59c2508282755a5df53a5,
  0x8e4e37a3b970f079ae9d22a499a714c875760273f74a9398995d32c05027d810,
  0x61cfa42792f93b9fde36eb163e978709fafa7616ec3c7dad0135806c3d91a21b
]

def group(a)
  q = Array.new
  128.times do |i|
    q[2*i] = ((a[i/8])>>4)&0x8 | ((a[i/8+32])>>5)&0x4 | ((a[i/8+64])>>6)&0x2 | ((a[i/8+96])>>7)&0x1
    a[i/8] <<= 1; a[i/8+32]<<=1; a[i/8+64]<<=1; a[i/8+96]<<=1;
    q[2*i+1] = ((a[i/8+16])>>4)&0x8 | ((a[i/8+48])>>5)&0x4 | ((a[i/8+80])>>6)&0x2 | ((a[i/8+112])>>7)&0x1
    a[i/8+16] <<= 1; a[i/8+48]<<=1; a[i/8+80]<<=1; a[i/8+112]<<=1;  
  end
  b = Array.new
  128.times {|i| b<<join_nibbles(q[2*i], q[2*i+1])}
  return b
end

def degroup(a)
  b = Array.new
  128.times {|i| b[i]=0}
  128.times do |i|
    b[i/8+  0]<<=1; b[i/8+  0] |= (a[i]>>7)&1; a[i]<<=1
    b[i/8+ 32]<<=1; b[i/8+ 32] |= (a[i]>>7)&1; a[i]<<=1
    b[i/8+ 64]<<=1; b[i/8+ 64] |= (a[i]>>7)&1; a[i]<<=1
    b[i/8+ 96]<<=1; b[i/8+ 96] |= (a[i]>>7)&1; a[i]<<=1
    b[i/8+ 16]<<=1; b[i/8+ 16] |= (a[i]>>7)&1; a[i]<<=1
    b[i/8+ 48]<<=1; b[i/8+ 48] |= (a[i]>>7)&1; a[i]<<=1
    b[i/8+ 80]<<=1; b[i/8+ 80] |= (a[i]>>7)&1; a[i]<<=1
    b[i/8+112]<<=1; b[i/8+112] |= (a[i]>>7)&1; a[i]<<=1
  end
  return b
end

# data is an array of bytes
#
def single_round(data, round)
  a = Array.new
  rc = $round_const[round]
=begin
  printf("\n== round %2d==\n\t", round)
  16.times do |y|
    8.times do |x|
      printf("%02X ", data[8*y+x])
    end
    print("\n\t")
  end
=end
  128.times do |idx| 
    x,y=split_byte($lutbox[((rc)>>(254))&0x3][data[idx]])
    rc <<= 2
    a << x << y
  end 
  a = permutation(a)
  b = Array.new
  128.times {|idx| b << join_nibbles(a[2*idx],a[2*idx+1])}
  return b
end

def next_rc(data)
  a = Array.new
=begin
  printf("\n== rc round ==\n\t")
  4.times do |y|
    8.times do |x|
      printf("%02X ", data[8*y+x])
    end
    print("\n\t")
  end
=end
  32.times do |idx| 
    x,y=split_byte($lutbox[0][data[idx]])
    a << x << y
#    if(x==nil)or(y==nil)
#      printf("DBG: idx=%2d, x=%2x, y=%2x", idx, x, y)
#    end
  end 
  a = permutation(a, 6)
  b = Array.new
  32.times {|idx| b << join_nibbles(a[2*idx],a[2*idx+1])}
  return b
end

def encrypt(data)
=begin  
  print("\n== ENCRYPT ==\n")
  print("=== pre group: ===\n\t")
  16.times do |y|
    8.times do |x|
      printf("%02X ", data[8*y+x])
    end
    print("\n\t")
  end
  puts("")
=end
  data = group(data)
=begin
  print("=== post group: ===\n\t")
  16.times do |y|
    8.times do |x|
      printf("%02X ", data[8*y+x])
    end
    print("\n\t")
  end
  puts("")
=end
  35.times do |round|
    data = single_round(data, round)
  end
  a = Array.new
  rc = $round_const[35]
  data.length.times do |idx| 
    x, y = split_byte($lutbox[(rc>>254)&3][data[idx]])
    rc <<= 2
    v, w = L_transform_inv(x, y)
    a << join_nibbles(v,w)
  end  
  a = degroup(a)
=begin
  print("\n=== post enc: ===\n\t")
  16.times do |y|
    8.times do |x|
      printf("%02X ", a[8*y+x])
    end
    print("\n\t")
  end
  puts("")
=end
  return a
end

def hash_block(state_a, block)
  64.times { |i| state_a[i] ^= block[i]}
  state_a = encrypt(state_a)
  64.times { |i| state_a[64+i] ^= block[i]}
  return state_a
end

def test_L_inverse
  print("\nL_inv_test:\n\t")
  256.times do |i|
    a,b = split_byte(i)
    c,d = L_transform(a, b)
    a,b = L_transform_inv(c, d)
    if join_nibbles(a,b)==i
      print('*')
    else
      print('!')
    end
    print("\n\t") if(i%16==15)
  end
end
#test_permutations

gen_lutboxes
gen_permutation_table
print_permute_table
4.times{ |i| print_lutbox(i) }
test_L_inverse
print("\nsingle round:\n\t")
h =  single_round([0,0,0,0,0,0,0,0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
              0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
              0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
              0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],0)
16.times do |y|
  8.times do |x|
    printf("%02X, ",h[y*8+x])
  end
  print("\n\t")
end
#=begin
print("\nH(0):\n\t")
h =  hash_block([0x01,0,0,0,0,0,0,0x0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],
                [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0])
h =  hash_block(h,
                [0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0])
16.times do |y|
  8.times do |x|
    printf("%02X, ",h[y*8+x])
  end
  print("\n\t")
end
#=end
puts("")

c0 = [  0x6a, 0x09, 0xe6, 0x67, 0xf3, 0xbc, 0xc9, 0x08, 
        0xb2, 0xfb, 0x13, 0x66, 0xea, 0x95, 0x7d, 0x3e, 
        0x3a, 0xde, 0xc1, 0x75, 0x12, 0x77, 0x50, 0x99, 
        0xda, 0x2f, 0x59, 0x0b, 0x06, 0x67, 0x32, 0x2a ]

rc = c0
42.times do |i|
  printf("/* C_%02d: */\n", i)
  4.times do |y|
    printf("\t")
    8.times do |x|
      printf("0x%02x, ", rc[y*8+x])
    end
    printf("\n")
  end
  rc = next_rc(rc)
end
