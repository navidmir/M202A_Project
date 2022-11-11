/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
 
 // Convolution Code: https://lloydrochester.com/post/c/convolution/

#include <zephyr/kernel.h>
#include <stdlib.h>

float* convolve(float h[], float x[], int lenH, int lenX)
{
  int nconv = lenH+lenX-1; // total convolution length
  int i,j,h_start,x_start,x_end;

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
	// infinite loop our computations
	while (1) {
		float h[] = {1.0, 1.0, 1.0}; // moving average
		float x[] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0}; // DC
		int h_len = 3;
		int x_len = 10;
		float *y = convolve(h, x, h_len, x_len);
	}
}

