/*
 * Copyright (c) 2015 Hugh Bailey <obs.jim@gmail.com>
 * Copyright (c) 2017 Ryan Foster <RytoEX@gmail.com>
 *
 * SPDX-License-Identifier: ISC
 */

#pragma once

#include <windows.h>
#include "../c99defs.h"

#ifdef __cplusplus
extern "C" {
#endif

struct reg_dword {
	LSTATUS status;
	DWORD size;
	DWORD return_value;
};

EXPORT void get_reg_dword(HKEY hkey, LPCWSTR sub_key, LPCWSTR value_name,
			  struct reg_dword *info);

#ifdef __cplusplus
}
#endif
