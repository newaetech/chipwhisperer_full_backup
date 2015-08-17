#!/usr/bin/ruby
#

(0..19).each { |s|
  printf("0x%s%s, ", ((s+0)%5).to_s,((s+1)%5).to_s)
  printf("0x%s%s, ", ((s+2)%5).to_s,((s+3)%5).to_s)
}

puts("\n or (5)\n")
(0..19+3).each { |s|
  printf("0x%02x, ", ((s%5)*8))
}

puts("\n or (9)\n")
(0..19+7).each { |s|
  printf("0x%02x, ", ((s%9)*8))
}

puts("\n or (17)\n")
(0..21+15).each { |s|
  printf("0x%02x, ", ((s%17)*8))
}


puts("\n (3)\n")
(0..24).each { |s|
  printf("0x%02x, ", ((s%3)*8))
}
