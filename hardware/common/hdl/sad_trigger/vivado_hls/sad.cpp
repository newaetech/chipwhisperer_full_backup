
#include "ap_int.h"

#define num_pts 128

void sad(ap_uint<7> max_points, ap_uint<10> ref_points_in[num_pts], ap_uint<10> * datain, ap_uint<20> * sumout)
{
#pragma HLS INTERFACE ap_fifo port=ref_points_in
#pragma HLS INTERFACE ap_fifo port=datain
#pragma HLS INTERFACE ap_fifo port=sumout
#pragma HLS INTERFACE ap_stable port=max_points

	ap_uint<10> datamem[num_pts];
	ap_uint<10> sumtree[num_pts];
	ap_uint<20> totalsum;
	ap_uint<10> ref_points[num_pts];
	int i;

#pragma HLS ARRAY_PARTITION variable=ref_points complete dim=1
#pragma HLS ARRAY_PARTITION variable=datamem complete dim=1
#pragma HLS ARRAY_PARTITION variable=sumtree complete dim=1

	for(i = 0; i < num_pts; i++){
		ref_points[i] = ref_points_in[i];
	}

	for(i = 0; i < num_pts; i++){
		sumtree[i] = 0;
	}


	while(1){
#pragma HLS PIPELINE II=1
		//Shift data, load new
		for(i = num_pts-1; i > 0; i--){
			datamem[i] = datamem[i-1];
		}
		datamem[0] = *(datain++);

		//Subtract
		for(i = 0; i < num_pts; i++){

			//if (i < max_points){
				if (datamem[i] > ref_points[i]) {
					sumtree[i] = datamem[i] - ref_points[i];
				} else {
					sumtree[i] = ref_points[i] - datamem[i];
				}
			//}
		}

		//Add
		totalsum = 0;
		for (i = 0; i < num_pts; i++){
				totalsum += sumtree[i];
		}

		*(sumout++) = totalsum;
	}


}
