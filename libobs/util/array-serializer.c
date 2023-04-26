/*
 * Copyright (c) 2014 Hugh Bailey <obs.jim@gmail.com>
 *
 * SPDX-License-Identifier: ISC
 */

#include "darray.h"
#include "array-serializer.h"

static size_t array_output_write(void *param, const void *data, size_t size)
{
	struct array_output_data *output = param;
	da_push_back_array(output->bytes, (uint8_t *)data, size);
	return size;
}

static int64_t array_output_get_pos(void *param)
{
	struct array_output_data *data = param;
	return (int64_t)data->bytes.num;
}

void array_output_serializer_init(struct serializer *s,
				  struct array_output_data *data)
{
	memset(s, 0, sizeof(struct serializer));
	memset(data, 0, sizeof(struct array_output_data));
	s->data = data;
	s->write = array_output_write;
	s->get_pos = array_output_get_pos;
}

void array_output_serializer_free(struct array_output_data *data)
{
	da_free(data->bytes);
}
