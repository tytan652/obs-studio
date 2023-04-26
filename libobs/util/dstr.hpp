/*
 * Copyright (c) 2014 Hugh Bailey <obs.jim@gmail.com>
 *
 * SPDX-License-Identifier: ISC
 */

#pragma once

#include "dstr.h"

class DStr {
	dstr str;

	DStr(DStr const &) = delete;
	DStr &operator=(DStr const &) = delete;

public:
	inline DStr() { dstr_init(&str); }
	inline DStr(DStr &&other) : DStr() { dstr_move(&str, &other.str); }

	inline DStr &operator=(DStr &&other)
	{
		dstr_move(&str, &other.str);
		return *this;
	}

	inline ~DStr() { dstr_free(&str); }

	inline operator dstr *() { return &str; }
	inline operator const dstr *() const { return &str; }

	inline operator char *() { return str.array; }
	inline operator const char *() const { return str.array; }

	inline dstr *operator->() { return &str; }
};
