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

void main(void) {
	while (1) {
		const char *m = "abc";
		uint8_t digest[32];
		struct tc_sha256_state_struct s;
		(void)tc_sha256_init(&s);
		tc_sha256_update(&s, (const uint8_t *) m, strlen(m));
		(void)tc_sha256_final(digest, &s);
        }
}
