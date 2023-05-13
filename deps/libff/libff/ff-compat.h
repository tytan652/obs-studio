/*
 * Copyright (c) 2016 Hugh Bailey <obs.jim@gmail.com>
 *
 * SPDX-License-Identifier: ISC
 */

#pragma once

#if LIBAVCODEC_VERSION_MAJOR >= 57
#define av_free_packet av_packet_unref
static inline int av_dup_packet_2(AVPacket *pkt)
{
	AVPacket tmp = *pkt;
	int ret = av_packet_ref(pkt, &tmp);
	av_packet_unref(&tmp);
	return ret;
}
#define av_dup_packet av_dup_packet_2
#endif
