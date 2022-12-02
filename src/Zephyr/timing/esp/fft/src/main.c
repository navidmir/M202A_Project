/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
 
 // FFT code: https://rosettacode.org/wiki/Fast_Fourier_transform#C

#include <zephyr/kernel.h>
#include <stdio.h>
#include <math.h>
#include <complex.h>
#include <zephyr/timing/timing.h>
 
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
	
	// for timing
	timing_t start_time, end_time;
	uint32_t total_ns;

	timing_init();
	timing_start();

	// infinite loop our computations
	while (1) {
		start_time = timing_counter_get();
		// time 100 runs
		for (int i = 0; i < 100; i++) {
			cplx buf[] = {0,0,1,1,0,1,0,0,0,1,1,1,0,0,0,0}; 
			fft(buf, 16); 
		}
		end_time = timing_counter_get();
    		total_ns = timing_cycles_to_ns(timing_cycles_get(&start_time, &end_time));
		printk("%d ns\n", (int)total_ns);
	}
}


