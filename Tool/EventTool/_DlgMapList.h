#pragma once
#include "afxwin.h"


// _DlgMapList 대화 상자입니다.

class _DlgMapList : public CDialog
{
	DECLARE_DYNAMIC(_DlgMapList)

public:
	_DlgMapList(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgMapList();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_MAPLIST };

public:
	void	Init();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()	
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnLbnDblclkListMap();
protected:
	CListBox m_MapList;
	
};
