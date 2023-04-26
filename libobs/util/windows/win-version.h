/*
 * Copyright (c) 2015 Hugh Bailey <obs.jim@gmail.com>
 *
 * SPDX-License-Identifier: ISC
 */

#pragma once

#include "../c99defs.h"

#ifdef __cplusplus
extern "C" {
#endif

struct win_version_info {
	int major;
	int minor;
	int build;
	int revis;
};

static inline int win_version_compare(const struct win_version_info *dst,
				      const struct win_version_info *src)
{
	if (dst->major > src->major)
		return 1;
	if (dst->major == src->major) {
		if (dst->minor > src->minor)
			return 1;
		if (dst->minor == src->minor) {
			if (dst->build > src->build)
				return 1;
			if (dst->build == src->build)
				return 0;
		}
	}
	return -1;
}

EXPORT bool is_64_bit_windows(void);
EXPORT bool is_arm64_windows(void);
EXPORT bool get_dll_ver(const wchar_t *lib, struct win_version_info *info);
EXPORT void get_win_ver(struct win_version_info *info);
EXPORT uint32_t get_win_ver_int(void);

#ifdef __cplusplus
}
#endif
