/*
 * Copyright (c) 2014 Hugh Bailey <obs.jim@gmail.com>
 *
 * SPDX-License-Identifier: ISC
 */

#include "ff-threading.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN

long ff_atomic_inc_long(volatile long *val)
{
	return InterlockedIncrement(val);
}

long ff_atomic_dec_long(volatile long *val)
{
	return InterlockedDecrement(val);
}
