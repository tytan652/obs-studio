// SPDX-FileCopyrightText: 2015 Intel Corporation.
//
// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-3-Clause
// SPDX-License-Identifier: LicenseRef-QSV11-License-Clarification-Email

#include <windows.h>

#define MSDK_FOPEN(FH, FN, M)           { fopen_s(&FH, FN, M); }
#define MSDK_SLEEP(X)                   { Sleep(X); }

typedef LARGE_INTEGER mfxTime;
