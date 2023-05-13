/*
 * Copyright (c) 2015 John R. Bradley <jrb@turrettech.com>
 *
 * SPDX-License-Identifier: ISC
 */

#pragma once

#include "ff-clock.h"

#include <libavcodec/avcodec.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ff_frame {
	AVFrame *frame;
	struct ff_clock *clock;
	double pts;
	int64_t duration;
};

typedef struct ff_frame ff_frame_t;

#ifdef __cplusplus
}
#endif
