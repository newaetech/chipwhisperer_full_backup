#!/usr/local/bin/ruby

def scanFile (filename)
bytes = 0

#   text	   data	    bss	    dec	    hex	filename
#    420	      0	      0	    420	    1a4	shabea.o
#   1600	      0	      0	   1600	    640	sha256-asm.o


file=File.open(filename)
name= /(.*)_size.txt/.match(filename)[1]
if (not name)
  name=filename
end

while line=file.gets do
  m = /^\s*\d+\s+\d+\s+\d+\s+(\d+)\s+.*/.match(line)
  if m
    bytes += m[1].to_i
  end 
end

#puts "#{name}:\t#{bytes} bytes"
printf("%6d bytes\t%s\n", bytes, name)

end



#========================================================
# MAIN
#========================================================

ARGV.each do |x| scanFile(x) end

