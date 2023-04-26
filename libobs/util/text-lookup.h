/*
 * Copyright (c) 2013 Hugh Bailey <obs.jim@gmail.com>
 *
 * SPDX-License-Identifier: ISC
 */

#pragma once

/*
 * Text Lookup interface
 *
 *   Used for storing and looking up localized strings.  Stores localization
 *   strings in a hashmap to efficiently look up associated strings via a
 *   unique string identifier name.
 */

#include "c99defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/* opaque typedef */
struct text_lookup;
typedef struct text_lookup lookup_t;

/* functions */
EXPORT lookup_t *text_lookup_create(const char *path);
EXPORT bool text_lookup_add(lookup_t *lookup, const char *path);
EXPORT void text_lookup_destroy(lookup_t *lookup);
EXPORT bool text_lookup_getstr(lookup_t *lookup, const char *lookup_val,
			       const char **out);

#ifdef __cplusplus
}
#endif
