#pragma once

#include "targetver.h"

#define _WTL_NO_CSTRING

#include <ctime>
#include <cmath>
#include <windows.h>
#include <atlstr.h>
#include <atlapp.h>
#include <atlmisc.h>
#include <atlctrls.h>
#include <strsafe.h>

#include "resources.h"

extern CAppModule _Module;

// custom windows messages
#define WM_PLAYER_UPDATE (WM_USER + 1)

#define STATUS_STOPPED      0x00000000
#define STATUS_WAITING_POE  0x00000002
#define STATUS_NOT_IN_LEVEL 0x00000004
#define STATUS_NO_STATS		0x00000008
#define PLAYER_UPDATE_EXP   0x80000001

#define IS_STATUS_UPDATE(x) ((x & 0x80000000) == 0)
