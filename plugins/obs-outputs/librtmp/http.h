/* Copyright (C) 2010 Howard Chu
 * Copyright (C) 2010 Antti Ajanki
 * SPDX-FileCopyrightText: 2014 OBS Studio Contributors
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef __RTMP_HTTP_H__
#define __RTMP_HTTP_H__

typedef enum
{
    HTTPRES_OK,               /* result OK */
    HTTPRES_OK_NOT_MODIFIED,  /* not modified since last request */
    HTTPRES_NOT_FOUND,        /* not found */
    HTTPRES_BAD_REQUEST,      /* client error */
    HTTPRES_SERVER_ERROR,     /* server reported an error */
    HTTPRES_REDIRECTED,       /* resource has been moved */
    HTTPRES_LOST_CONNECTION   /* connection lost while waiting for data */
} HTTPResult;

struct HTTP_ctx
{
    char *date;
    int size;
    int status;
    void *data;
};

typedef size_t (HTTP_read_callback)(void *ptr, size_t size, size_t nmemb, void *stream);

HTTPResult HTTP_get(struct HTTP_ctx *http, const char *url, HTTP_read_callback *cb);

#endif
