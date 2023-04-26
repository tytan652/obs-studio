/*
 * Copyright (c) 2013 Hugh Bailey <obs.jim@gmail.com>
 *
 * SPDX-License-Identifier: ISC
 */

#pragma once

struct HRError {
	const char *str;
	HRESULT hr;

	inline HRError(const char *str, HRESULT hr) : str(str), hr(hr) {}
};
