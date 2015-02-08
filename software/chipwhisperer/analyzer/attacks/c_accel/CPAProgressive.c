#include <stdio.h>
#include <stdint.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    double * sumhq;
    double * sumtq;
    double * sumt;
    double * sumh;
    double * sumht;
    unsigned int totalTraces;
    
    double * sumnum; //temp, 1xnpoints
    double * sumden1; //temp, 1xnpoints
    double * sumden2; //temp, 1xnpoints
    double * hyp; //temp, 1xntraces
} analysis_state_t;

#define trace_point(n,p) traces[tracedim_n * (traceoffset + t) + (pointoffset + p)]

__declspec(dllexport) void oneSubkey(
        double * traces,
        unsigned char * datain,
        unsigned char * dataout,
        
        size_t tracedim_n, /* Size of traces array, number of traces in array */
        size_t tracedim_p, /* Total size of traces array, points in each trace */
        
        size_t traceoffset, /* Starting trace to use in analysis */
        size_t ntraces,     /* Number of traces to use in analysis */
        size_t pointoffset, /* Starting point to use in analysis */
        size_t npoints,     /* Number of points to use in analysis */
        int bnum,           /* Byte number we are targetting */
        
        analysis_state_t * state,
        
        void * model,       /* Model */
        void * modeldata,
        
        

        double * diffoutput)
{    
    double htmp;
    const int nguess = 256;

    for(unsigned int guess = 0; guess < nguess; guess++){
        for(size_t t = 0; t < ntraces; t++){
            htmp = 0; //TODO
            state->hyp[t] = htmp;
            
            state->sumh[guess] += htmp;            
        }
        
        for(size_t t = 0; t < ntraces; t++){
            for(size_t p = 0; p < npoints; p++){
                state->sumt[nguess*guess + p] += trace_point(t,p);
                state->sumht[nguess*guess + p] += trace_point(t,p) * state->hyp[t];
            }
        }             
    }
}


#ifdef __cplusplus
}
#endif