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

def autocorr(x):
    result = np.correlate(x, x, mode='full')
    return result[result.size/2:]

def PlotOneHW(hwlist, plotHist=True, plotNorms=True, plotMeans=True, plotSDs=True, plotAutoCorr=True):
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

        print "mean = %f, sd=%f"%(param[0], param[1])

        xlims[0] = min(xlims[0], -4*param[1]+param[0])
        xlims[1] = max(xlims[1], 4*param[1]+param[0])

    if plotHist:
        figure()
        f, sb = subplots(3, 3, sharex='col', sharey='row')
        sb = sb.flatten()
        f.suptitle('Fitted Distribution, byte=%d'%(bnum), fontsize=14)
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
            sb[i].plot(x,pdf_fitted,'r-',x,pdf,'b-')
            sb[i].hist(hwlist[i],normed=1,alpha=.3)

    if plotHist:
        figure()
        for i in range(0,9):
            hist(hwlist[i],alpha=.3)
        title('Histogram of Noise, byte=%d'%bnum)
        

    if plotNorms:
        figure()
        for i in range(0,9):
            x = linspace(xlims[0],xlims[1],500)
            # fitted distribution
            pdf_fitted = norm.pdf(x,loc=means[i],scale=sds[i])
            # original distribution
            pdf = norm.pdf(x)

            title('Plot of all Gaussian PDFs, byte=%d'%(bnum))
            plot(x,pdf_fitted,'r-',x,pdf,'b-')
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
        f.suptitle('Auto-Correlation of Noise, byte=%d'%(bnum), fontsize=14)
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

if __name__ == "__main__":
    bnum=7
    hwlist = np.load("hwlist_bnum=%d.npy"%bnum)
    PlotOneHW(hwlist)
    #PlotOneHW(hwlist, plotHist=False, plotNorms=False, plotMeans=False, plotSDs=False, plotAutoCorr=True)
