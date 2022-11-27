/*
 * Copyright (c) 2020 Linumiz
 *
 * SPDX-License-Identifier: Apache-2.0
 */
 
 // Code from Zephyr example: samples/compression/lz4

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <zephyr/kernel.h>
#include <zephyr/timing/timing.h>
#include <zephyr/sys/printk.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/drivers/uart.h>
#include "lz4.h"

BUILD_ASSERT(DT_NODE_HAS_COMPAT(DT_CHOSEN(zephyr_console), zephyr_cdc_acm_uart),
	     "Console device is not ACM CDC UART device");

void main(void)
{
	// set up printing
	const struct device *const dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));
	uint32_t dtr = 0;

	if (usb_enable(NULL)) {
		return;
	}

	/* Poll if the DTR flag was set */
	while (!dtr) {
		uart_line_ctrl_get(dev, UART_LINE_CTRL_DTR, &dtr);
	}
	
	// for timing
	timing_t start_time, end_time;
	uint32_t total_ns;

	timing_init();
	timing_start();

	static const char *src = "Lorem ipsum dolor sit amet, consectetur "
		"adipiscing elit. Quisque sodales lorem lorem, sed congue enim "
		"vehicula a. Sed finibus diam sed odio ultrices pharetra. Nullam "
		"dictum arcu ultricies turpis congue,vel venenatis turpis venenatis. "
		"Nam tempus arcu eros, ac congue libero tristique congue. Proin velit "
		"lectus, euismod sit amet quam in, maximus condimentum urna. Cras vel "
		"erat luctus, mattis orci ut, varius urna. Nam eu lobortis velit."
		"\n"
		"Nullam sit amet diam vel odio sodales cursus vehicula eu arcu. Proin "
		"fringilla, enim nec consectetur mollis, lorem orci interdum nisi, "
		"vitae suscipit nisi mauris eu mi. Proin diam enim, mollis ac rhoncus "
		"vitae, placerat et eros. Suspendisse convallis, ipsum nec rhoncus "
		"aliquam, ex augue ultrices nisl, id aliquet mi diam quis ante. "
		"Pellentesque venenatis ornare ultrices. Quisque et porttitor lectus. "
		"Ut venenatis nunc et urna imperdiet porttitor non laoreet massa."
		"Donec eleifend eros in mi sagittis egestas. Sed et mi nunc. Nunc "
		"vulputate,mauris non ullamcorper viverra, lorem nulla vulputate "
		"diam, et congue dui velit non erat. Duis interdum leo et ipsum "
		"tempor consequat. In faucibus enim quis purus vulputate nullam."
		"\n";

	const int src_size = (int)(strlen(src) + 1);
	const int max_dst_size = LZ4_compressBound(src_size);
	char *compressed_data = malloc((size_t)max_dst_size);
	
	// run the algorithm forever
	while (1) {
		start_time = timing_counter_get();
		// time 100 runs
		for (int i = 0; i < 100; i++) {
			LZ4_compress_default(src, compressed_data, src_size, max_dst_size);
		}	
		end_time = timing_counter_get();
    		total_ns = timing_cycles_to_ns(timing_cycles_get(&start_time, &end_time));
		printk("%d ns\n", (int)total_ns);
	}
}
