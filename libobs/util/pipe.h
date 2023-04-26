/*
 * Copyright (c) 2014 Hugh Bailey <obs.jim@gmail.com>
 *
 * SPDX-License-Identifier: ISC
 */

#pragma once

#include "c99defs.h"

#ifdef __cplusplus
extern "C" {
#endif

struct os_process_pipe;
typedef struct os_process_pipe os_process_pipe_t;

EXPORT os_process_pipe_t *os_process_pipe_create(const char *cmd_line,
						 const char *type);
EXPORT int os_process_pipe_destroy(os_process_pipe_t *pp);

EXPORT size_t os_process_pipe_read(os_process_pipe_t *pp, uint8_t *data,
				   size_t len);
EXPORT size_t os_process_pipe_read_err(os_process_pipe_t *pp, uint8_t *data,
				       size_t len);
EXPORT size_t os_process_pipe_write(os_process_pipe_t *pp, const uint8_t *data,
				    size_t len);

#ifdef __cplusplus
}
#endif
