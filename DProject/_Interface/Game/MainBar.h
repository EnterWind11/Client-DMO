
#pragma once 

#include "../00.Contents/MainFrame_Contents.h"

class cMainBar : public cBaseWindow, public CMainFrameContents::ObserverType
{
	struct sMainBtnData : public CUSTOMDATA
	{
		sMainBtnData(int nBtnID, int nHotKeyID, std::wstring wsTooltipName)
			: m_nBtnID(nBtnID),wsName(wsTooltipName),m_nHotkeyID(nHotKeyID)
		{}
		~sMainBtnData() {};
		int m_nBtnID;
		int m_nHotkeyID;
		std::wstring wsName;
	};
public:
	cMainBar();
	~cMainBar();

	enum eButtonID
	{
		MAIN_BT_OPT,		// 옵션 버튼
		MAIN_BT_STORE,		// 상점 버튼
		MAIN_BT_COMMUNITY,	// 커뮤니티 버튼
		MAIN_BT_QUEST,		// 퀘스트 버튼
		MAIN_BT_INVEN,		// 인벤 버튼
		MAIN_BT_ENCY,		// 도감 버튼
		MAIN_BT_SEAL,		// 씰 마스터 버튼

#ifdef REWARD_SYSTEM_UI
		MAIN_BT_REWARD,		// 접속보상
#endif
#ifdef SDM_VIP_SYSTEM_20181105
		MAIN_BT_MEMBERSHIP,
#endif
		MAIN_BT_MAX,
	};

private:
	virtual	bool			Construct(void);
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

public:
	virtual eWINDOW_TYPE		GetWindowType(){ return WT_MAIN_BAR; }
	virtual void				Destroy();
	virtual void				DeleteResource();
	virtual void				Create( int nValue = 0 );

	virtual void				Update(float const& fDeltaTime);
	virtual eMU_TYPE			Update_ForMouse();

	virtual void				Render();	
	virtual void				ResetDevice();

	void						SetButtonEnable(eButtonID ebtID, bool bEnable);

private:
	void						_CreateMainBarBtn();
	void						_SetToolTip( int const& hotkeytype, std::wstring const& pText, CsPoint pos, CsPoint patch );
	void						_MainBarBtnClick( void* pSender, void* pData );
#ifdef UI_INVENTORY_RENEWAL
	void						_SetNotifyNew( eButtonID ebtID, bool bEnable );
#endif

private:
	cSprite*							m_pMainBarBg;
	cSprite*							m_pMainBarSubBg;
	cGridListBox*						m_pMainBarGrid;
	std::map<int,cGridListBoxItem*>		m_pmapControls;
};