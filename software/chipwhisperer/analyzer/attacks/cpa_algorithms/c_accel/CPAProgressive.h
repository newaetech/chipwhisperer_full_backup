/*
 Copyright (c) 2015, NewAE Technology Inc
 All rights reserved.

 Authors: Colin O'Flynn

 Find this and more at newae.com - this file is part of the chipwhisperer
 project, http://www.assembla.com/spaces/chipwhisperer

    This file is part of chipwhisperer.

    chipwhisperer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    chipwhisperer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with chipwhisperer.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _CPA_PROGRESSIVE_H
#define _CPA_PROGRESSIVE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
  #define EXPORT __declspec(dllexport)
#else
  #define EXPORT __attribute__((visibility("default")))
#endif
extern const int hamming_weight[];

typedef struct {
    double * sumhq; // 1 x nguess
    double * sumtq;
    double * sumt;  // 1 x npoints
    double * sumh;  // 1 x nguess
    double * sumht; // nguess x npoints
    unsigned int totalTraces;
    
    double * hyp; //temp, 1xntraces
} analysis_state_t;


/* Perform CPA Attack on a single subkey */
EXPORT void oneSubkey(
        double * traces,            /* trace data, 2d array * */
        uint8_t * datain,           /* data in to model, 2d array * */
        uint8_t * dataout,          /* data out from model, 2d array * */
        
        size_t tracedim_n,          /* Size of traces array, number of traces in array */
        size_t tracedim_p,          /* Total size of traces array, points in each trace */
        
        size_t traceoffset,         /* Starting trace to use in analysis */
        size_t ntraces,             /* Number of traces to use in analysis */
        size_t pointoffset,         /* Starting point to use in analysis */
        size_t npoints,             /* Number of points to use in analysis */
        
        analysis_state_t * state,   /* Holds CPA State */
        
        void * model,               /* Model */
        void * modeldata,           /* Data passed to model */
        
        double * diffoutput         /* Output */);

#ifdef __cplusplus
}
#endif

#endif //_CPA_PROGRESSIVE_H