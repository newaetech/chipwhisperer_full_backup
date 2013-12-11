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
from pylab import plot,show,hist,figure,title,hold,xlabel,ylabel,subplot,subplots,ylim,xlim
import numpy as np
import pylab as py
from scipy import optimize


def autocorr(x):
    result = np.correlate(x, x, mode='full')
    return result[result.size/2:]


def PlotHistNorm(data, log=False):
    # distribution fitting
    param = norm.fit(data) 
    mean = param[0]
    sd = param[1]

    #Set large limits
    xlims = [-6*sd+mean, 6*sd+mean]

    #Plot histogram
    histdata = hist(data,bins=12,alpha=.3,log=log)

    #Generate X points
    x = linspace(xlims[0],xlims[1],500)

    #Get Y points via Normal PDF with fitted parameters
    pdf_fitted = norm.pdf(x,loc=mean,scale=sd)

    #Get histogram data, in this case bin edges
    xh = [0.5 * (histdata[1][r] + histdata[1][r+1]) for r in xrange(len(histdata[1])-1)]

    #Get bin width from this
    binwidth = (max(xh) - min(xh)) / len(histdata[1])           

    #Scale the fitted PDF by area of the histogram
    pdf_fitted = pdf_fitted * (len(data) * binwidth)

    #Plot PDF
    plot(x,pdf_fitted,'r-')


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
    
        xlims[0] = min(xlims[0], -5*param[1]+param[0])
        xlims[1] = max(xlims[1], 5*param[1]+param[0])

    if plotHist:
        figure()
        f, sb = subplots(3, 3, sharex='col') #sharey='row'
        sb = sb.flatten()
        f.suptitle('Fitted Distribution, byte=%d'%(bnum), fontsize=14)
        sb[3].set_ylabel('Occurances')
        sb[7].set_xlabel('Current Measurement (unitless, linear relation)')
        bn = [10,12,15,17,19,16,15,12,10]
        for i in range(0,9):
            x = linspace(xlims[0],xlims[1],500)
            # fitted distribution
            pdf_fitted = norm.pdf(x,loc=means[i],scale=sds[i])

            # original distribution
            #pdf = norm.pdf(x)

            data = sb[i].hist(hwlist[i],bins=bn[i],alpha=.3)

            # Generate data from bins as a set of points 
            xh = [0.5 * (data[1][r] + data[1][r+1]) for r in xrange(len(data[1])-1)]

            rangex = max(xh) - min(xh)
            binwidth = rangex / len(data[1])           

            pdf_fitted = pdf_fitted * (len(hwlist[i]) * binwidth)

            #subplot(3,3,i+1)
            sb[i].set_title('HW=%d'%(i))
            sb[i].plot(x,pdf_fitted,'r-')
            sb[i].set_xlim(xlims)

    if plotHist:
        figure()
        for i in range(0,9):
            hist(hwlist[i],alpha=.3)
        title('Histogram of Noise, byte=%d'%bnum)

    if plotHist:
        figure()
        i=4
        x = linspace(xlims[0],xlims[1],500)
        # fitted distribution
        pdf_fitted = norm.pdf(x,loc=means[i],scale=sds[i])
        # original distribution
        pdf = norm.pdf(x)
        histdata=hist(hwlist[i],25, log=True)
        title('HW=%d, Log Scales'%i)
        xh = [0.5 * (histdata[1][r] + histdata[1][r+1]) for r in xrange(len(histdata[1])-1)]
        binwidth = (max(xh) - min(xh)) / len(histdata[1])           
        pdf_fitted2 = pdf_fitted * (len(hwlist[i]) * binwidth)
        plot(x,pdf_fitted2,'r-')
        ylabel('Occurances')
        xlabel('Current Measurement (unitless, linear relation)')
        ylim( (1, 100000))

        figure()
        histdata=hist(hwlist[i],25)
        title('HW=%d, Linear Scales'%i)
        xh = [0.5 * (histdata[1][r] + histdata[1][r+1]) for r in xrange(len(histdata[1])-1)]
        binwidth = (max(xh) - min(xh)) / len(histdata[1])           
        pdf_fitted2 = pdf_fitted * (len(hwlist[i]) * binwidth)
        plot(x,pdf_fitted2,'r-')
        ylabel('Occurances')
        xlabel('Current Measurement (unitless, linear relation)')
        

    if plotNorms:
        figure()
        for i in range(0,9):
            x = linspace(xlims[0],xlims[1],500)
            # fitted distribution
            pdf_fitted = norm.pdf(x,loc=means[i],scale=sds[i])
            # original distribution
            pdf = norm.pdf(x)

            title('Plot of all Gaussian PDFs, byte=%d'%(bnum))
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
    #PlotOneHW(hwlist, plotHist=True, plotNorms=False, plotMeans=False, plotSDs=False, plotAutoCorr=False)
    #PlotOneHW(hwlist, plotHist=True, plotNorms=False, plotMeans=False, plotSDs=False, plotAutoCorr=False)
