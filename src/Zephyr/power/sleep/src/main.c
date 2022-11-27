/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
 
 // from samples/board/nrf/system_off

#include <zephyr/kernel.h>
#include <zephyr/pm/pm.h>
 
void main(void)
{
	// infinite loop our computations
	while (1) {
		// pm_state_force(0u, &(struct pm_state_info){PM_STATE_SOFT_OFF, 0, 0});
		pm_state_set(0u, PM_STATE_SOFT_OFF);
		k_sleep(K_SECONDS(2U));
	}
}


