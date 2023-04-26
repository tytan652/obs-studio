/*
 * Copyright (c) 2015 Hugh Bailey <obs.jim@gmail.com>
 *
 * SPDX-License-Identifier: ISC
 */

#pragma once

#include "serializer.h"

EXPORT bool file_input_serializer_init(struct serializer *s, const char *path);
EXPORT void file_input_serializer_free(struct serializer *s);

EXPORT bool file_output_serializer_init(struct serializer *s, const char *path);
EXPORT bool file_output_serializer_init_safe(struct serializer *s,
					     const char *path,
					     const char *temp_ext);
EXPORT void file_output_serializer_free(struct serializer *s);
