/*
 * Copyright (c) 2014 Hugh Bailey <obs.jim@gmail.com>
 *
 * SPDX-License-Identifier: ISC
 */

#pragma once

class WinHandle {
	HANDLE handle = INVALID_HANDLE_VALUE;

	inline void Clear()
	{
		if (handle && handle != INVALID_HANDLE_VALUE)
			CloseHandle(handle);
	}

public:
	inline WinHandle() {}
	inline WinHandle(HANDLE handle_) : handle(handle_) {}
	inline ~WinHandle() { Clear(); }

	inline operator HANDLE() const { return handle; }

	inline WinHandle &operator=(HANDLE handle_)
	{
		if (handle_ != handle) {
			Clear();
			handle = handle_;
		}

		return *this;
	}

	inline HANDLE *operator&() { return &handle; }

	inline bool Valid() const
	{
		return handle && handle != INVALID_HANDLE_VALUE;
	}
};

class WinModule {
	HMODULE handle = NULL;

	inline void Clear()
	{
		if (handle)
			FreeLibrary(handle);
	}

public:
	inline WinModule() {}
	inline WinModule(HMODULE handle_) : handle(handle_) {}
	inline ~WinModule() { Clear(); }

	inline operator HMODULE() const { return handle; }

	inline WinModule &operator=(HMODULE handle_)
	{
		if (handle_ != handle) {
			Clear();
			handle = handle_;
		}

		return *this;
	}

	inline HMODULE *operator&() { return &handle; }

	inline bool Valid() const { return handle != NULL; }
};
