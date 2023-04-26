/*
 * Copyright (c) 2015 Hugh Bailey <obs.jim@gmail.com>
 *
 * SPDX-License-Identifier: ISC
 */

#pragma once

#include "c99defs.h"

EXPORT uint32_t calc_crc32(uint32_t crc, const void *buf, size_t size);
