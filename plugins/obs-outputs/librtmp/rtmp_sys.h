/*
 * Copyright (C) 2010 Howard Chu
 * SPDX-FileCopyrightText: 2014-2015, 2018, 2020 OBS Studio Contributors
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef __RTMP_SYS_H__
#define __RTMP_SYS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <errno.h>
#include <stdarg.h>
#include <limits.h>
#include <time.h>
#include <stdint.h>

#ifdef _WIN32

#include <winsock2.h>
#include <ws2tcpip.h>
#include <Mstcpip.h>


#ifdef _MSC_VER	/* MSVC */
#define snprintf _snprintf
#define strcasecmp stricmp
#define strncasecmp strnicmp
#define vsnprintf _vsnprintf
#endif

#define GetSockError()	WSAGetLastError()
#define SetSockError(e)	WSASetLastError(e)
#define setsockopt(a,b,c,d,e)	(setsockopt)(a,b,c,(const char *)d,(int)e)
#ifdef EWOULDBLOCK
#undef EWOULDBLOCK
#endif
#define EWOULDBLOCK	WSAETIMEDOUT	/* we don't use nonblocking, but we do use timeouts */
#define sleep(n)	Sleep(n*1000)
#define msleep(n)	Sleep(n)
#define SET_RCVTIMEO(tv,s)	int tv = s*1000
#else /* !_WIN32 */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/times.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#define GetSockError()	errno
#define SetSockError(e)	errno = e
#undef closesocket
#define closesocket(s)	close(s)
#define msleep(n)	usleep(n*1000)
#define SET_RCVTIMEO(tv,s)	struct timeval tv = {s,0}
#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1
#endif
#endif

#include "rtmp.h"
#endif
