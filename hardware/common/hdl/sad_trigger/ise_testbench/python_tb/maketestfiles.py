import numpy as np
from numpy import random as nprnd

inputlength = 1000
featurelength = 128

#feature = nprnd.randint(2**10, size=featurelength) - 2**9
feature = [0] * featurelength
feature[64] = 25
data = nprnd.randint(2**10, size=inputlength) - 2**9

data[inputlength/2:(inputlength/2 + featurelength)] = feature

#Now do SAD calculation to get expected result... only valid after some point
sad = np.zeros(inputlength-featurelength)
for offset in range(0, inputlength-featurelength):
    diff = data[offset:(offset+featurelength)] - feature
    sad[offset] = sum(abs(diff))
    
ffeature = open('feature.txt', 'w')
fdata = open('data.txt', 'w')
fout = open('sad.txt', 'w')

ffeature.write("%d\n"%len(feature))
fdata.write("%d\n"%len(data))

f2feature = open('f2feature.txt', 'w')
for item in feature:
    #Verilog doesn't support negative in fscanf
    f2feature.write("%d\n"%(item + 2**9))

feature = feature[::-1]
for item in feature:
    #Verilog doesn't support negative in fscanf
    ffeature.write("%d\n"%(item + 2**9))

for item in data:
    #Verilog doesn't support negative in fscanf
    fdata.write("%d\n"%(item + 2**9))

for item in sad:
    fout.write("%d\n"%item)

ffeature.close()
fdata.close()
fout.close()
f2feature.close()
