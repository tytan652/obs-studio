/*
 * Copyright (c) 2014 Hugh Bailey <obs.jim@gmail.com>
 * SPDX-FileCopyrightText: 2021 OBS Studio Contributors
 *
 * SPDX-License-Identifier: ISC
 */

#pragma once

#include <windows.h>

struct ipc_pipe_server {
	OVERLAPPED overlap;
	HANDLE handle;
	HANDLE ready_event;
	HANDLE stop_event;
	HANDLE thread;

	uint8_t *read_data;
	size_t size;
	size_t capacity;

	ipc_pipe_read_t read_callback;
	void *param;
};

struct ipc_pipe_client {
	HANDLE handle;
};

static inline bool ipc_pipe_client_valid(ipc_pipe_client_t *pipe)
{
	return pipe->handle != NULL && pipe->handle != INVALID_HANDLE_VALUE;
}
