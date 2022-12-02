/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
 
 // code from tests/crypto/tinycrypt and https://github.com/intel/tinycrypt/blob/master/tests/test_sha256.c

#include <tinycrypt/sha256.h>
#include <tinycrypt/constants.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zephyr/types.h>

#include <zephyr/kernel.h>
#include <zephyr/timing/timing.h>

void main(void) {

	// for timing 
	timing_t start_time, end_time;
	uint32_t total_ns;

	timing_init();
	timing_start();
	
	while (1) {
		start_time = timing_counter_get();
		// time 100 runs
		for (int i = 0; i < 100; i++) {
			const char *m = "abc";
			uint8_t digest[32];
			struct tc_sha256_state_struct s;
			(void)tc_sha256_init(&s);
			tc_sha256_update(&s, (const uint8_t *) m, strlen(m));
			(void)tc_sha256_final(digest, &s);
		}
		end_time = timing_counter_get();
    		total_ns = timing_cycles_to_ns(timing_cycles_get(&start_time, &end_time));
		printk("%d ns\n", (int)total_ns);
        }
}
