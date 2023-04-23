// SPDX-FileCopyrightText: 2015 Intel Corporation.
//
// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-3-Clause
// SPDX-License-Identifier: LicenseRef-QSV11-License-Clarification-Email

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MSDK_FOPEN(FH, FN, M)           { FH=fopen(FN,M); }
#define MSDK_SLEEP(X)                   { usleep(1000*(X)); }

typedef timespec mfxTime;
