// MainFrm.h : CMainFrame 클래스의 인터페이스
//


#pragma once

#include "ChildView.h"

class CMainFrame : public CFrameWnd
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// 특성입니다.
protected:
	CSplitterWnd		m_wndSplitter;
public:
	CSplitterWnd*		GetSplitterWnd(){ return &m_wndSplitter; }

public:
	void				_Update();
	void				_Render();


// 작업입니다.
public:

// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

// 구현입니다.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 컨트롤 모음이 포함된 멤버입니다.
	//CChildView    m_wndView;

// 생성된 메시지 맵 함수
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
};


extern float				g_fDeltaTime;
extern CMainFrame*			g_pMainWnd;
extern CFormView*			g_pProc3DWin;
extern CsGBDX9RendererPtr	g_pRenderer;