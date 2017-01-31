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

/* NOTE: This file uses the .cpp extension despite being basically C. This is to get around the
   fact that amazingly Visual Studio command-line compilers don't support C99 syntax, so instead
   we fake it with C++ compilation mode. 
 */

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "CPAProgressive.h"
#include "AESModels.h"

#ifdef __cplusplus
extern "C" {
#endif

const int hamming_weight[] = 
{0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 1,
2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 1, 2, 
2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 
4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 1, 2, 2, 3, 
2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 
4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 2, 3, 3, 4, 3, 4, 
4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 3, 4, 4, 5, 4, 5, 5, 
6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8};

/* Extracts a point from the passed 2-D NumPy array */
#define trace_point(n,p) traces[tracedim_p * (traceoffset + t) + (pointoffset + p)]


EXPORT void oneSubkey(
        double * traces,  /* double * */
        uint8_t * datain,  /* uint8_t * */
        uint8_t * dataout, /* uint8_t * */
        
        size_t tracedim_n, /* Size of traces array, number of traces in array */
        size_t tracedim_p, /* Total size of traces array, points in each trace */
        
        size_t traceoffset, /* Starting trace to use in analysis */
        size_t ntraces,     /* Number of traces to use in analysis */
        size_t pointoffset, /* Starting point to use in analysis */
        size_t npoints,     /* Number of points to use in analysis */
        
        analysis_state_t * state,
        
        void * model,       /* Model */
        void * modeldata,   /* Model Data */

        double * diffoutput)
{    
    double htmp;
    double sumden1, sumden2, sumnum;
    const int nguess = 256;
    
    //printf("ntraces = %d\nnpoints=%d\n", ntraces, npoints);
    state->totalTraces += ntraces;

    /* Things which don't require guesses */
    for(size_t t = 0; t < ntraces; t++){
        for(size_t p = 0; p < npoints; p++){
            state->sumtq[p] += trace_point(t,p) * trace_point(t,p);
            state->sumt[p] += trace_point(t,p);
        }
    }

    /* Things which require guesses */
    for(unsigned int guess = 0; guess < nguess; guess++){
        for(size_t t = 0; t < ntraces; t++){
            htmp = aes_model(guess, datain + 16*t, dataout + 16*t, modeldata);
            state->hyp[t] = htmp;            
            state->sumh[guess] += htmp; 
            state->sumhq[guess] += htmp*htmp;    
            //printf("%f\n", htmp);
        }
        
        for(size_t t = 0; t < ntraces; t++){
            for(size_t p = 0; p < npoints; p++){
                state->sumht[tracedim_p*guess + p] += trace_point(t,p) * state->hyp[t];
            }            
        }
        
        sumden1 = (state->sumh[guess] * state->sumh[guess]) - ((double)state->totalTraces) * state->sumhq[guess];
        //printf("%f %f\n", state->sumh[1], state->sumhq[1]);
        for(size_t p = 0; p < npoints; p++){
            sumden2 = (state->sumt[p] * state->sumt[p]) - (((double)state->totalTraces) * state->sumtq[p]);
            sumnum = ((double)state->totalTraces) * state->sumht[tracedim_p*guess + p] - state->sumh[guess]*state->sumt[p];
            diffoutput[tracedim_p*guess + p] = sumnum / sqrt(sumden1 * sumden2);
            //printf("%e %e %e\n", sumden1, sumden2, sumnum);
        }
    }
}


#ifdef __cplusplus
}
#endif