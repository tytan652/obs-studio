/*
 * Copyright (C) 2019 by Peter Geis <pgwipeout@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */


#pragma once

#if defined(_MSC_VER) && \
	((defined(_M_X64) && !defined(_M_ARM64EC)) || defined(_M_IX86))
#include <emmintrin.h>
#else
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#define SIMDE_ENABLE_NATIVE_ALIASES
#include "simde/x86/sse2.h"
#endif
