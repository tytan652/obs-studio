/*
 * Copyright (c) 2014 Hugh Bailey <obs.jim@gmail.com>
 *
 * SPDX-License-Identifier: ISC
 */

#pragma once

#include "serializer.h"
#include "darray.h"

struct array_output_data {
	DARRAY(uint8_t) bytes;
};

EXPORT void array_output_serializer_init(struct serializer *s,
					 struct array_output_data *data);
EXPORT void array_output_serializer_free(struct array_output_data *data);
