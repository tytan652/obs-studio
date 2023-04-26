/*
 * Copyright (c) 2007 Alexey Vatchenko <av@bsdua.org>
 *
 * SPDX-License-Identifier: ISC
 */

#pragma once

/*
 * utf8: implementation of UTF-8 charset encoding (RFC3629).
 */

#ifdef __cplusplus
extern "C" {
#endif

#define UTF8_IGNORE_ERROR 0x01
#define UTF8_SKIP_BOM 0x02

size_t utf8_to_wchar(const char *in, size_t insize, wchar_t *out,
		     size_t outsize, int flags);
size_t wchar_to_utf8(const wchar_t *in, size_t insize, char *out,
		     size_t outsize, int flags);

#ifdef __cplusplus
}
#endif
