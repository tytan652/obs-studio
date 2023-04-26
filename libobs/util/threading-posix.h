/*
 * Copyright (c) 2015 Hugh Bailey <obs.jim@gmail.com>
 *
 * SPDX-License-Identifier: ISC
 */

#pragma once

static inline long os_atomic_inc_long(volatile long *val)
{
	return __atomic_add_fetch(val, 1, __ATOMIC_SEQ_CST);
}

static inline long os_atomic_dec_long(volatile long *val)
{
	return __atomic_sub_fetch(val, 1, __ATOMIC_SEQ_CST);
}

static inline void os_atomic_store_long(volatile long *ptr, long val)
{
	__atomic_store_n(ptr, val, __ATOMIC_SEQ_CST);
}

static inline long os_atomic_set_long(volatile long *ptr, long val)
{
	return __atomic_exchange_n(ptr, val, __ATOMIC_SEQ_CST);
}

static inline long os_atomic_exchange_long(volatile long *ptr, long val)
{
	return os_atomic_set_long(ptr, val);
}

static inline long os_atomic_load_long(const volatile long *ptr)
{
	return __atomic_load_n(ptr, __ATOMIC_SEQ_CST);
}

static inline bool os_atomic_compare_swap_long(volatile long *val, long old_val,
					       long new_val)
{
	return __atomic_compare_exchange_n(val, &old_val, new_val, false,
					   __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
}

static inline bool os_atomic_compare_exchange_long(volatile long *val,
						   long *old_val, long new_val)
{
	return __atomic_compare_exchange_n(val, old_val, new_val, false,
					   __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
}

static inline void os_atomic_store_bool(volatile bool *ptr, bool val)
{
	__atomic_store_n(ptr, val, __ATOMIC_SEQ_CST);
}

static inline bool os_atomic_set_bool(volatile bool *ptr, bool val)
{
	return __atomic_exchange_n(ptr, val, __ATOMIC_SEQ_CST);
}

static inline bool os_atomic_exchange_bool(volatile bool *ptr, bool val)
{
	return os_atomic_set_bool(ptr, val);
}

static inline bool os_atomic_load_bool(const volatile bool *ptr)
{
	return __atomic_load_n(ptr, __ATOMIC_SEQ_CST);
}
