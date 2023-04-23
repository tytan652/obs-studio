/*
 * Copyright (c) 2015 John R. Bradley <jrb@turrettech.com>
 *
 * SPDX-License-Identifier: ISC
 */

#include "ff-callbacks.h"

bool ff_callbacks_frame(struct ff_callbacks *callbacks, struct ff_frame *frame)
{
	if (callbacks->frame == NULL)
		return true;

	return callbacks->frame(frame, callbacks->opaque);
}

bool ff_callbacks_format(struct ff_callbacks *callbacks,
                         AVCodecContext *codec_context)
{
	if (callbacks->format == NULL)
		return true;

	return callbacks->format(codec_context, callbacks->opaque);
}

bool ff_callbacks_initialize(struct ff_callbacks *callbacks)
{
	if (callbacks->initialize == NULL)
		return true;

	return callbacks->initialize(callbacks->opaque);
}

bool ff_callbacks_frame_initialize(struct ff_frame *frame,
                                   struct ff_callbacks *callbacks)
{
	if (callbacks->frame_initialize == NULL)
		return true;

	return callbacks->frame_initialize(frame, callbacks->opaque);
}

bool ff_callbacks_frame_free(struct ff_frame *frame,
                             struct ff_callbacks *callbacks)
{
	if (callbacks->frame_free == NULL)
		return true;

	return callbacks->frame_free(frame, callbacks->opaque);
}
