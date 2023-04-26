// SPDX-FileCopyrightText: 2005-2014 Intel Corporation.
//
// SPDX-License-Identifier: LicenseRef-QSV11-License-Clarification-Email
// SPDX-License-Identifier: GPL-2.0-or-later

#include <windows.h>

#define MSDK_FOPEN(FH, FN, M)           { fopen_s(&FH, FN, M); }
#define MSDK_SLEEP(X)                   { Sleep(X); }

typedef LARGE_INTEGER mfxTime;
