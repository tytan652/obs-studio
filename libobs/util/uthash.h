/*
 * Copyright (C) 2023 by Dennis Sädtler <dennis@obsproject.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

/*
 * This file (re)defines various uthash settings for use in libobs
 */

#include <uthash/uthash.h>

/* Use OBS allocator */
#undef uthash_malloc
#undef uthash_free
#define uthash_malloc(sz) bmalloc(sz)
#define uthash_free(ptr, sz) bfree(ptr)

/* Use SFH (Super Fast Hash) function instead of JEN */
#undef HASH_FUNCTION
#define HASH_FUNCTION HASH_SFH
