/*
 * Copyright (c) 2013 Hugh Bailey <obs.jim@gmail.com>
 *
 * SPDX-License-Identifier: ISC
 */

#pragma once

#include "bmem.h"

inline void *operator new(size_t size)
{
	return bmalloc(size);
}

inline void operator delete(void *data)
{
	bfree(data);
}

inline void *operator new[](size_t size)
{
	return bmalloc(size);
}

inline void operator delete[](void *data)
{
	bfree(data);
}
