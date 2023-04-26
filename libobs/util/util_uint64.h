/*
 * Copyright (c) 2020 Hans Petter Selasky <hps@selasky.org>
 *
 * SPDX-License-Identifier: ISC
 */

#pragma once

#if defined(_MSC_VER) && defined(_M_X64)
#include <intrin.h>
#endif

static inline uint64_t util_mul_div64(uint64_t num, uint64_t mul, uint64_t div)
{
#if defined(_MSC_VER) && defined(_M_X64) && (_MSC_VER >= 1920)
	unsigned __int64 high;
	const unsigned __int64 low = _umul128(num, mul, &high);
	unsigned __int64 rem;
	return _udiv128(high, low, div, &rem);
#else
	const uint64_t rem = num % div;
	return (num / div) * mul + (rem * mul) / div;
#endif
}
