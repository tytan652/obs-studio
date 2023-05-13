/*
 * Copyright (c) 2015 John R. Bradley <jrb@turrettech.com>
 * SPDX-FileCopyrightText: 2015 OBS Studio Contributors
 *
 * SPDX-License-Identifier: ISC
 */

#pragma once

#define AV_SYNC_THRESHOLD 0.01
#define AV_NOSYNC_THRESHOLD 10.0

#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

enum ff_av_sync_type {
	AV_SYNC_AUDIO_MASTER,
	AV_SYNC_VIDEO_MASTER,
	AV_SYNC_EXTERNAL_MASTER,
};

typedef double (*ff_sync_clock)(void *opaque);

struct ff_clock {
	ff_sync_clock sync_clock;
	enum ff_av_sync_type sync_type;
	uint64_t start_time;

	pthread_mutex_t mutex;
	pthread_cond_t cond;
	volatile long retain;
	bool started;

	void *opaque;
};

typedef struct ff_clock ff_clock_t;

struct ff_clock *ff_clock_init(void);
double ff_get_sync_clock(struct ff_clock *clock);
struct ff_clock *ff_clock_retain(struct ff_clock *clock);
struct ff_clock *ff_clock_move(struct ff_clock **clock);
void ff_clock_release(struct ff_clock **clock);

int64_t ff_clock_start_time(struct ff_clock *clock);
bool ff_clock_start(struct ff_clock *clock, enum ff_av_sync_type sync_type,
                    const bool *abort);

#ifdef __cplusplus
}
#endif
