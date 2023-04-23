/*
 * Copyright (c) 2014 Hugh Bailey <obs.jim@gmail.com>
 *
 * SPDX-License-Identifier: ISC
 */

#include "ff-threading.h"

long ff_atomic_inc_long(volatile long *val)
{
	return __sync_add_and_fetch(val, 1);
}

long ff_atomic_dec_long(volatile long *val)
{
	return __sync_sub_and_fetch(val, 1);
}
