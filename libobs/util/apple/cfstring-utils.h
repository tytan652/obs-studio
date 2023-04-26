// SPDX-FileCopyrightText: 2018 Marvin Scholz <epirat07@gmail.com>
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "../c99defs.h"
#include "../dstr.h"

#ifdef __cplusplus
extern "C" {
#endif

EXPORT char *cfstr_copy_cstr(CFStringRef cfstr, CFStringEncoding cfstr_enc);

EXPORT bool cfstr_copy_dstr(CFStringRef cfstr, CFStringEncoding cfstr_enc,
			    struct dstr *str);

#ifdef __cplusplus
}
#endif
