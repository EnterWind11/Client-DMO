// SequnceTool.h : SequnceTool 응용 프로그램에 대한 주 헤더 파일
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

// CSequnceToolApp:
// 이 클래스의 구현에 대해서는 SequnceTool.cpp을 참조하십시오.
//

class CSequnceToolApp : public CWinApp
{
public:
	CSequnceToolApp();


// 재정의입니다.
public:
	virtual BOOL InitInstance();

// 구현입니다.

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnIdle(LONG lCount);
};

extern CSequnceToolApp theApp;