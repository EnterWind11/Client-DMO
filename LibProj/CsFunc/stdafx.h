// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.
//

#pragma once

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#include <windows.h>



extern double g_fAccumTime;

#include <tchar.h>
#include <stdio.h>
#include <io.h>

#include <stack>
#include <queue>
#include <map>
#include <list>

#include "Define.h"

#include "POINT.h"
#include "CsTarga.h"
#include "CsBmp.h"
#include "CsFile.h"
#include "Csini.h"
#include "assert_cs.h"
#include "MemPool.h"
#include "CsChunk.h"
#include "CsImageInterpolation.h"
#include "CsMessageBox.h"
#include "Code.h"
#include "CsLog.h"
#include "_VectorHeader.h"
#include "CsTimeSeq.h"
