/*
 * Copyright (c) 2015 John R. Bradley <jrb@turrettech.com>
 *
 * SPDX-License-Identifier: ISC
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

long ff_atomic_inc_long(volatile long *val);
long ff_atomic_dec_long(volatile long *val);

#ifdef __cplusplus
}
#endif
