/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
 
 // FFT code: https://rosettacode.org/wiki/Fast_Fourier_transform#C++

#include <zephyr/kernel.h>
#include <stdio.h>
#include <math.h>
#include <complex.h>
 
float PI;
typedef float complex cplx;
 
void _fft(cplx buf[], cplx out[], int n, int step)
{
	if (step < n) {
		_fft(out, buf, n, step * 2);
		_fft(out + step, buf + step, n, step * 2);
 
		for (int i = 0; i < n; i += 2 * step) {
			cplx t = cexp(-I * PI * i / n) * out[i + step];
			buf[i / 2]     = out[i] + t;
			buf[(i + n)/2] = out[i] - t;
		}
	}
}
 
void fft(cplx buf[], int n)
{
	cplx out[n];
	for (int i = 0; i < n; i++) out[i] = buf[i];
 
	_fft(buf, out, n, 1);
}

void main(void)
{
	PI = atan2(1, 1) * 4;
	// infinite loop our computations
	while (1) {
		cplx buf[] = {0,0,1,1,0,1,0,0,0,1,1,1,0,0,0,0}; 
		fft(buf, 16); 
	}
}


