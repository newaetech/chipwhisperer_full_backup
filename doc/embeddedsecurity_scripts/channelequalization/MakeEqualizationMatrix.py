import numpy as np
import scipy.io as sio
import os
import sys
from pylab import plot,show,hist,figure,title,hold,xlabel,ylabel,subplot,subplots


sys.path.append('../../../software/analyzerv2')
sys.path.append('../../../software/common')
import attacks.models.AES128_8bit as AES128_8bit

def myFunc(pinvRx, traces, textin, key, bnum, ntraces, npoints):
    
    
    hwlist = np.zeros( (ntraces, 1) )
    for i in range(0, ntraces):
        hwlist[i][0] = AES128_8bit.HypHW(textin[i], None, key[bnum], bnum)

    Ryx = np.zeros( (npoints, 1) )
    for i in range(0, ntraces):        
        hb = hb + traces[i:i+1,:].T * hwlist[i]
        
    hb = hb / np.linalg.norm(hb)

    f = np.dot(pinvRx, hb)
    equalizer = f.T / (np.dot(f.T, hb))

    return equalizer

def main(directory):    
    preflist = []

    files = os.listdir(directory)
    for f in files:
        if "_traces.npy" in f:
            f = f.replace("traces.npy", "")
            preflist.append(f)

    if len(preflist) == 0:
        print "WARNING: No traces found. Check directory."

    prefix = preflist[0]

    traces = np.load(directory + prefix + "traces.npy")
    textin = np.load(directory + prefix + "textin.npy")
    key = np.load(directory + prefix + "knownkey.npy")
    #Following may be required if traces differs from length of textin
    traces = traces[0:-1,:]

    ntraces = np.size(traces[:,0])
    npoints = np.size(traces[0,:])

    ntraces = 500

    print "traces = %d"%ntraces
    print "points = %d"%npoints

    calcRxx = False

    if calcRxx:
        print "Calculating Rxx..."
        Rxx = np.zeros( (npoints, npoints ) )
        for i in range(0, ntraces):
            #Rxx = Rxx + np.dot(traces[i,:], traces[i,:].T)
            Rxx = Rxx + np.dot(traces[i:i+1,:].T, traces[i:i+1,:])
        Rxx = Rxx / ntraces

        np.save("Rxx", Rxx)

        print "Starting PInv..."
        pinvRxx = np.linalg.pinv(Rxx)
        
        np.save("pinvRxx", pinvRxx)
    else:
        pinvRxx = np.load("pinvRxx.npy")

    hinfo = np.zeros( (16, 3000) )

    print "Generating equalization matrix..."
    for bnum in range(0,16):
        print "Byte %d"%bnum
        hinfo[bnum] = myFunc(pinvRxx, traces, textin, key, bnum, ntraces, npoints)

    np.save("equalization", hinfo)


if __name__ == "__main__":
    # Point this to the 500 000 traces recorded of AES-128 on Mega328P
    # Available from ChipWhisperer.com
    directory = "../../../../traces_500000/traces/"

    main(directory)
   

