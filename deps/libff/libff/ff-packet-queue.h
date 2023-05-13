/*
 * Copyright (c) 2015 John R. Bradley <jrb@turrettech.com>
 * SPDX-FileCopyrightText: 2015 OBS Studio Contributors
 *
 * SPDX-License-Identifier: ISC
 */

#pragma once

#include "ff-clock.h"

#include <libavformat/avformat.h>
#include <pthread.h>
#include <stdbool.h>

#define FF_PACKET_FAIL -1
#define FF_PACKET_EMPTY 0
#define FF_PACKET_SUCCESS 1

#ifdef __cplusplus
extern "C" {
#endif

struct ff_packet {
	AVPacket base;
	ff_clock_t *clock;
};

struct ff_packet_list {
	struct ff_packet packet;
	struct ff_packet_list *next;
};

struct ff_packet_queue {
	struct ff_packet_list *first_packet;
	struct ff_packet_list *last_packet;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	struct ff_packet flush_packet;
	int count;
	unsigned int total_size;
	bool abort;
};

typedef struct ff_packet_queue ff_packet_queue_t;

bool packet_queue_init(struct ff_packet_queue *q);
void packet_queue_abort(struct ff_packet_queue *q);
void packet_queue_free(struct ff_packet_queue *q);
int packet_queue_put(struct ff_packet_queue *q, struct ff_packet *packet);
int packet_queue_put_flush_packet(struct ff_packet_queue *q);
int packet_queue_get(struct ff_packet_queue *q, struct ff_packet *packet,
                     bool block);

void packet_queue_flush(struct ff_packet_queue *q);

#ifdef __cplusplus
}
#endif
