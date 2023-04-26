/*
 * Copyright (c) 2014 Hugh Bailey <obs.jim@gmail.com>
 *
 * SPDX-License-Identifier: ISC
 */

#pragma once

template<typename T> class CoTaskMemPtr {
	T *ptr;

	inline void Clear()
	{
		if (ptr)
			CoTaskMemFree(ptr);
	}

public:
	inline CoTaskMemPtr() : ptr(NULL) {}
	inline CoTaskMemPtr(T *ptr_) : ptr(ptr_) {}
	inline ~CoTaskMemPtr() { Clear(); }

	inline operator T *() const { return ptr; }
	inline T *operator->() const { return ptr; }

	inline const T *Get() const { return ptr; }

	inline CoTaskMemPtr &operator=(T *val)
	{
		Clear();
		ptr = val;
		return *this;
	}

	inline T **operator&()
	{
		Clear();
		ptr = NULL;
		return &ptr;
	}
};
