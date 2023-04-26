/*
 * Copyright (c) 2020 Hugh Bailey <obs.jim@gmail.com>
 *
 * SPDX-License-Identifier: ISC
 */

#pragma once

#include <curl/curl.h>

#if defined(_WIN32) && LIBCURL_VERSION_NUM >= 0x072c00

#ifdef CURLSSLOPT_REVOKE_BEST_EFFORT
#define CURL_OBS_REVOKE_SETTING CURLSSLOPT_REVOKE_BEST_EFFORT
#else
#define CURL_OBS_REVOKE_SETTING CURLSSLOPT_NO_REVOKE
#endif

#define curl_obs_set_revoke_setting(handle) \
	curl_easy_setopt(handle, CURLOPT_SSL_OPTIONS, CURL_OBS_REVOKE_SETTING)

#else

#define curl_obs_set_revoke_setting(handle)

#endif
