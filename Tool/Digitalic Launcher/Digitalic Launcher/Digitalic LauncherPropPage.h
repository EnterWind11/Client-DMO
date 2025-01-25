#pragma once
#include "afxcmn.h"

// Digitalic LauncherPropPage.h : CDigitalicLauncherPropPage 속성 페이지 클래스의 선언입니다.


// CDigitalicLauncherPropPage : 구현을 보려면 Digitalic LauncherPropPage.cpp을(를) 참조하십시오.

class CDigitalicLauncherPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CDigitalicLauncherPropPage)
	DECLARE_OLECREATE_EX(CDigitalicLauncherPropPage)

// 생성자입니다.
public:
	CDigitalicLauncherPropPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_PROPPAGE_DIGITALICLAUNCHER };	

// 구현입니다.
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 메시지 맵입니다.
protected:
	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_Progress;
};

