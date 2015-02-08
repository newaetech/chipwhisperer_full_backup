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

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "CPAProgressive.h"

#ifdef __cplusplus
extern "C" {
#endif

const int hamming_weight[] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8};

const unsigned char sbox[256] = 
{
   0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
   0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
   0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
   0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
   0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
   0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
   0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
   0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
   0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
   0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
   0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
   0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
   0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
   0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
   0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
   0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
};

double aes_model(uint8_t guess, uint8_t * datain, uint8_t * dataout, model_setup_t * modeldata)
{
    unsigned int bnum = modeldata->bnum;
    
    return (double)hamming_weight[sbox[datain[bnum] ^ guess]];
}


/* Extracts a point from the passed 2-D NumPy array */
#define trace_point(n,p) traces[tracedim_p * (traceoffset + t) + (pointoffset + p)]

__declspec(dllexport) void oneSubkey(
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
        model_setup_t * modeldata,
        
        

        double * diffoutput)
{    
    double htmp;
    double sumden1, sumden2, sumnum;
    const int nguess = 256;
    
    printf("ntraces = %d\nnpoints=%d\n", ntraces, npoints);
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
        //printf("Guess = %d\n", guess);
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