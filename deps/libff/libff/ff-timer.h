/*
 * Copyright (c) 2015 John R. Bradley <jrb@turrettech.com>
 *
 * SPDX-License-Identifier: ISC
 */

#pragma once

#include <libavutil/avutil.h>
#include <pthread.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*ff_timer_callback)(void *opaque);

struct ff_timer {
	ff_timer_callback callback;
	void *opaque;

	pthread_mutex_t mutex;
	pthread_mutexattr_t mutexattr;
	pthread_cond_t cond;

	pthread_t timer_thread;
	uint64_t next_wake;
	bool needs_wake;

	bool abort;
};

typedef struct ff_timer ff_timer_t;

bool ff_timer_init(struct ff_timer *timer, ff_timer_callback callback,
                   void *opaque);
void ff_timer_free(struct ff_timer *timer);
void ff_timer_schedule(struct ff_timer *timer, uint64_t microseconds);

#ifdef __cplusplus
}
#endif
