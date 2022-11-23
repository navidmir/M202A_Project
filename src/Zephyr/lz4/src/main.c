/*
 * Copyright (c) 2020 Linumiz
 *
 * SPDX-License-Identifier: Apache-2.0
 */
 
 // Code from Zephyr example: samples/compression/lz4

#include <stdio.h>
#include <zephyr/kernel.h>
#include <string.h>
#include <stdlib.h>
#include "lz4.h"

void main(void)
{
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
		LZ4_compress_default(src, compressed_data, src_size, max_dst_size);
	}
}
