#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013, Colin O'Flynn <coflynn@newae.com>
# All rights reserved.
#
# Find this and more at newae.com - this file is part of the chipwhisperer
# project, http://www.assembla.com/spaces/chipwhisperer
#
#    chipwhisperer is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    chipwhisperer is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with chipwhisperer.  If not, see <http://www.gnu.org/licenses/>.
#=================================================

# This file plots the distribution of all measured points corresponding to
# some specific hamming weight. assumes you've already run the other script
# which generates the hamming weight list.

from scipy.stats import norm
from numpy import linspace
from pylab import plot,show,hist,figure,title,hold,xlabel,ylabel,subplot,subplots
import numpy as np
import sys
import math


def autocorr(x):
    result = np.correlate(x, x, mode='full')
    return result[result.size/2:]

def PlotOneHW(hwlist, plotHist=True, plotNorms=True, plotMeans=True, plotSDs=True, plotAutoCorr=True, titlesuffix = ""):
    xlims = [+1E99,-1E99]

    means = [0]*9
    sds = [0]*9

    for i in range(0,9):
        data = hwlist[i]    
        param = norm.fit(data) # distribution fitting

        # now, param[0] and param[1] are the mean and 
        # the standard deviation of the fitted distribution

        means[i] = param[0]
        sds[i] = param[1]

        #print "mean = %f, sd=%f"%(param[0], param[1])

        xlims[0] = min(xlims[0], -4*param[1]+param[0])
        xlims[1] = max(xlims[1], 4*param[1]+param[0])

        print "%3.3f "%param[0], 
   
    mmean = np.mean(means)
    msd = np.mean(sds)

    print " mean/sd = %f"%(abs(mmean/msd))
    #print "mmean^2 / sd^2 = %f"%((mmean**2) / (msd**2))
    #hatEsNO = (mmean**2) / (2*(msd**2))
    #print "SNR = %f dB"%(math.log10(hatEsNO)*10)

    if plotHist:
        figure()
        f, sb = subplots(3, 3, sharex='col', sharey='row')
        sb = sb.flatten()
        f.suptitle('Fitted Distribution %s'%(titlesuffix), fontsize=14)
        sb[3].set_ylabel('Occurances (normalized to match Gaussian PDF)')
        sb[7].set_xlabel('Current Measurement (unitless, linear relation)')
        for i in range(0,9):
            x = linspace(xlims[0],xlims[1],500)
            # fitted distribution
            pdf_fitted = norm.pdf(x,loc=means[i],scale=sds[i])
            # original distribution
            pdf = norm.pdf(x)

            #subplot(3,3,i+1)
            sb[i].set_title('HW=%d'%(i))            
            sb[i].plot(x,pdf_fitted,'r-')
            sb[i].hist(hwlist[i],normed=1,alpha=.3)

    if plotHist:
        figure()
        for i in range(0,9):
            hist(hwlist[i],alpha=.3)
        title('Histogram of Noise %s'%titlesuffix)
        

    if plotNorms:
        figure()
        for i in range(0,9):
            x = linspace(xlims[0],xlims[1],500)
            # fitted distribution
            pdf_fitted = norm.pdf(x,loc=means[i],scale=sds[i])
            # original distribution
            pdf = norm.pdf(x)

            title('Plot of all Gaussian PDFs %s'%(titlesuffix))
            plot(x,pdf_fitted,'r-')
            xlabel('Current Measurement (unitless, linear relation)')

    if plotMeans:
        figure()
        plot(range(0,9), means)
        title('Measurement Mean vs. Hamming Weight')
        xlabel('Hamming Weight of Measurement')
        ylabel('Measurement Std-Dev')

    if plotSDs:
        figure()
        plot(range(0,9), sds)
        title('Measurement Std-Dev vs. Hamming Weight')
        xlabel('Hamming Weight of Measurement')
        ylabel('Measurement Mean (unitless, linear relation to current)')

    if plotAutoCorr:
        figure()
        f, sb = subplots(3, 3, sharey='row') #sharex='col'
        sb = sb.flatten()
        f.suptitle('Auto-Correlation of Noise %s'%(titlesuffix), fontsize=14)
        sb[3].set_ylabel('Auto-Correlation R[n], Normalized for R[0]=1.0')
        sb[7].set_xlabel('Sample No.')
        for i in range(0,9):
            data = hwlist[i]

            #Zero-Mean
            data = data - means[i]

            ac = autocorr(data)
            ac = ac / ac[0]
            sb[i].set_title('HW=%d'%(i))
            sb[i].plot(range(0,len(data)),ac,)

            #xmin, xmax = sb[i].get_xlim()
            #print xmax

            xl = sb[i].get_xticks()

            xstr = []
            for t in xl:
                if t < 1000:
                    s = "%d"%t
                elif t < 10000:
                    s = "%.1fk"%(t/1000.0)
                else:
                    s = "%dk"%(t/1000)
                                    
                xstr.append(s)

            sb[i].set_xticklabels(xstr)
            

    show()

def printUsage():
    print ""
    print ""
    print "Plotting of Noise Distributions"
    print "  by Colin O'Flynn"
    print ""
    print "Usage information: ./%s hwlistfile.npy bnum <options>"%sys.argv[0]
    print "  Options can either be nothing, or specify which plots to enable"
    print "  by list of 'True' or 'False' for each option. Available plots:"
    print "    1. Histogram of all hamming weights"
    print "    2. All gaussian-fitted results in one plot"
    print "    3. Mean vs Hamming Weight"
    print "    4. Std-Dev vs Hamming Weight"
    print "    5. Auto-Correlation of each Hamming Weight"
    print " e.g. to plot only gaussian fitting results:"
    print "   ./%s hwlistfile.npy False True False False False"%sys.argv[0]

if __name__ == "__main__":

    if len(sys.argv) > 1:
        if (sys.argv[1] == 'h') or (sys.argv[1] == '-h') or (sys.argv[1] == '--help'):
            printUsage()
            sys.exit()

        hwlist = np.load(sys.argv[1])

        plotinfo = [True]*5

        if len(sys.argv) > 2:
            if len(sys.argv) == 7:
                for t in range(2,7):
                    plotinfo[t-2] = sys.argv[t].lower() in ("yes", "true", "t", "1")
            else:
                printUsage()
                sys.exit()

        PlotOneHW(hwlist, plotHist=plotinfo[0], plotNorms=plotinfo[1], plotMeans=plotinfo[2], plotSDs=plotinfo[3], plotAutoCorr=plotinfo[4])
    else:
        #hwlist = np.load("hwlist_bnum=4.npy")
        #PlotOneHW(hwlist)
        #PlotOneHW(hwlist, plotHist=True, plotNorms=False, plotMeans=False, plotSDs=False, plotAutoCorr=False)
    	
        for bnum in range(0,16):
            print "%3d "%bnum,
            hwlist = np.load("aes256rsm-vcc-csi-hwlist/hwlist_bnum=%d.npy"%bnum)
            PlotOneHW(hwlist, False, False, False, False, False)
           

