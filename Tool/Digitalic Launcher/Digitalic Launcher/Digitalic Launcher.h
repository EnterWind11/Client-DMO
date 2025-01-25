#pragma once

// Digitalic Launcher.h : Digitalic Launcher.DLL의 기본 헤더 파일입니다.

#if !defined( __AFXCTL_H__ )
#error "이 파일을 포함하기 전에 'afxctl.h'를 포함하십시오."
#endif

#include "resource.h"       // 주 기호입니다.


// CDigitalicLauncherApp : 구현을 보려면 Digitalic Launcher.cpp를 참조하십시오.

class CDigitalicLauncherApp : public COleControlModule
{
public:
	BOOL InitInstance();
	int ExitInstance();
};

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

