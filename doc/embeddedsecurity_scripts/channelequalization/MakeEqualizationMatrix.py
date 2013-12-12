import numpy as np
import scipy.io as sio
import os
import sys
from pylab import plot,show,hist,figure,title,hold,xlabel,ylabel,subplot,subplots


sys.path.append('../../../software/analyzerv2')
sys.path.append('../../../software/common')
import attacks.models.AES128_8bit as AES128_8bit

def myFunc(pinvRx, traces, textin, key, bnum, ntraces, npoints):
    
    #For 8-bit this is 4.. can verify with hwlist.mean()
    hwmean = 4
    
    hwlist = np.zeros( (ntraces, 1) )
    for i in range(0, ntraces):
        if len(key) > 32:
            k = key[i]
        else:
            k = key
        hwlist[i][0] = AES128_8bit.HypHW(textin[i], None, k[bnum], bnum) - hwmean

    hb = np.zeros( (npoints, 1) )
    for i in range(0, ntraces):        
        hb = hb + traces[i:i+1,:].T * hwlist[i]
        
    hb = hb / (ntraces - 1)

    f = np.dot(pinvRx, hb)
    equalizer = f.T

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
    #key = np.load(directory + prefix + "keylist.npy")
    #Following may be required if traces differs from length of textin
    traces = traces[0:-1,:]

    ntraces = np.size(traces[:,0])
    npoints = np.size(traces[0,:])

    tmean = traces.mean(axis=0)
    traces = traces - tmean
  
    ntraces = 5000

    print "traces = %d"%ntraces
    print "points = %d"%npoints

    calcRxx = False
    calcRInv = False

    if calcRxx:
        print "Calculating Rxx..."
        
        #Estimate the covariance matrix Rxx. The following will work:
        Rxx = np.zeros( (npoints, npoints ) )
        for i in range(0, ntraces):
            Rxx = Rxx + np.dot(traces[i:i+1,:].T, traces[i:i+1,:])
        Rxx = Rxx / (ntraces-1)

        #NumPy function is faster, but gives answer which results in worse MSE,
        #unsure why, even using bias=1
        #Rxx = np.cov(traces[0:ntraces,:], rowvar=0)
        np.save("Rxx", Rxx)

    else:
        if calcRInv:
            Rxx = np.load("Rxx.npy")

    if calcRInv:
        print "Starting Inverse"
        #May need pinv if inv not stable
        #invRxx = np.linalg.pinv(Rxx)
        invRxx = np.linalg.inv(Rxx)       
        np.save("invRxx", invRxx)
    else:
        invRxx = np.load("invRxx.npy")

    hinfo = np.zeros( (16, npoints) )

    print "Generating equalization matrix..."
    for bnum in range(0,16):
        print "Byte %d"%bnum
        hinfo[bnum] = myFunc(invRxx, traces, textin, key, bnum, ntraces, npoints)

    np.save("equalization", hinfo)
    np.save("tmean", tmean)


if __name__ == "__main__":
    # Point this to the 500 000 traces recorded of AES-128 on Mega328P
    # Available from ChipWhisperer.com
    #directory = "../../../../traces_500000/traces/"
    directory = "../../../software/scripting-examples/default-data-dir/traces-avr-dpav4-vcc-10000-oldkey/"
#    directory = "../../../software/scripting-examples/default-data-dir/traces-avr-aes128-oldkey/"

    main(directory)
   

