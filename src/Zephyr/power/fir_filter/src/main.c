/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
 
 // Convolution Code: https://lloydrochester.com/post/c/convolution/

#include <zephyr/kernel.h>
#include <stdlib.h>

float* convolve(float h[], float x[], uint8_t lenH, uint8_t lenX)
{
  uint8_t nconv = lenH+lenX-1; // total convolution length
  uint8_t i,j,h_start,x_start,x_end;

  float *y = (float*) calloc(nconv, sizeof(float));
  for (i=0; i<nconv; i++)
  {
    x_start = MAX(0,i-lenH+1);
    x_end   = MIN(i+1,lenX);
    h_start = MIN(i,lenH-1);
    for(j=x_start; j<x_end; j++)
    {
      y[i] += h[h_start--]*x[j]; // flip h and slide across x
    }
  }
  return y;
}

void main(void)
{
	float h[] = {0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1}; // moving average
	float x[] = {0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1}; // random signal
	float* y;

	// infinite loop our computations
	while (1) {
		y = convolve(h, x, 10, 64);		
	}
}


