//---------------------------------------------------------------------------
//
// 파일명 : LoginFlow.cpp
// 작성일 : 
// 작성자 : 
// 설  명 : 
//
//---------------------------------------------------------------------------
#include "StdAfx.h"
#include "CharacterCreateFlow.h"

#include "../_Interface/07.CharacterCreate/CharacterCreate.h"
#include "../_Interface/Game/Fade.h"
#include "../ContentsSystem/ContentsSystem.h"

//---------------------------------------------------------------------------

namespace Flow
{
	CCharacterCreateFlow::CCharacterCreateFlow(int p_iID) : CFlow(p_iID), m_bBgmPlay(false),m_pFadeUI(NULL),m_pCharCreate(NULL)
	{
	}
	//---------------------------------------------------------------------------
	CCharacterCreateFlow::~CCharacterCreateFlow()
	{
	}
	//---------------------------------------------------------------------------
	BOOL CCharacterCreateFlow::Initialize()
	{
		m_pCharCreate = NiNew CCharacterCreate;		
		if( m_pCharCreate && m_pCharCreate->Construct() )
			m_pCharCreate->Init();

		m_pFadeUI = NiNew CFade;
		if( m_pFadeUI )
			m_pFadeUI->Init(CFade::FADE_IN, 0.5f);

	
		if( g_pSoundMgr )
			g_pSoundMgr->PlayMusic( "Game_opening.mp3", 0.0f );

		g_pEngine->SetGaussianBlurVal( 0.9f, 0.25f, 0.3f );

		return TRUE;
	}

	void CCharacterCreateFlow::OnEnter(void)
	{		
		CFlow::OnEnter();
		CONTENTSSYSTEM_PTR->InitializeContents(eContentsType::E_CT_CHARACTER_CREATE);
	}

	void CCharacterCreateFlow::OnExit(int p_iNextFlowID)
	{
		CFlow::OnExit(p_iNextFlowID);
		CONTENTSSYSTEM_PTR->InitializeContents(eContentsType::E_CT_CHARACTER_CREATE);
	}

	void CCharacterCreateFlow::ReservedChangeFlow(int p_iNextFlowID)
	{
		if( m_pFadeUI )
			m_pFadeUI->Reset(CFade::FADE_OUT, 0.5f);
	}

	void CCharacterCreateFlow::ReservedPopFlow()
	{
		if( m_pFadeUI )
			m_pFadeUI->Reset(CFade::FADE_OUT, 0.5f);
	}

	//---------------------------------------------------------------------------
	void CCharacterCreateFlow::Terminate()
	{
		SAFE_NIDELETE(m_pCharCreate);
		SAFE_NIDELETE(m_pFadeUI);
		RESOURCEMGR_ST.CleanUpResource();
	}
	//---------------------------------------------------------------------------
	bool CCharacterCreateFlow::LostDevice(void* p_pvData)
	{
		return true;
	}
	//---------------------------------------------------------------------------
	bool CCharacterCreateFlow::ResetDevice(bool p_bBeforeReset, void* p_pvData)
	{
		return true;
	}
	//---------------------------------------------------------------------------

	BOOL CCharacterCreateFlow::OnMsgHandler(const MSG& p_kMsg)
	{
		CURSOR_ST.Input(p_kMsg);

		if( cMessageBox::UpdateList() )
			return FALSE;	

		if( m_pCharCreate )
			m_pCharCreate->UpdateKeyboard(p_kMsg);

		return FALSE;
	}

	void CCharacterCreateFlow::InputFrame()
	{
		if( cMessageBox::UpdateList() )
			return;

		if( m_pCharCreate )
			m_pCharCreate->UpdateMouse();
	}
	//---------------------------------------------------------------------------
	void CCharacterCreateFlow::UpdateFrame()
	{
		CONTENTSSYSTEM_PTR->Update( (float)g_fDeltaTime );

		g_pSystemMsg->Update();

		if(m_pCharCreate)
			m_pCharCreate->Update(g_fDeltaTime);

		if( m_pFadeUI )
			m_pFadeUI->Update(g_fDeltaTime);

		if( TOOLTIPMNG_STPTR )
			TOOLTIPMNG_STPTR->Update( g_fDeltaTime );

		CURSOR_ST.LoopReset();	//2016-02-23-nova
	}
	//---------------------------------------------------------------------------
	void CCharacterCreateFlow::CullFrame()
	{
	}
	//---------------------------------------------------------------------------
	void CCharacterCreateFlow::RenderSceneFrame()
	{
		// 3D 렌더, 월드, 캐릭터, 이팩트 등등 ...
		if( g_pEngine )
			g_pEngine->ResetRendererCamera();

		if( m_pCharCreate )
			m_pCharCreate->Render3DModel();

		if( g_pEngine )
			g_pEngine->Render();
	}

	void CCharacterCreateFlow::RenderBackScreenFrame()
	{
		if(m_pCharCreate)
			m_pCharCreate->RenderScreenUI();
	}

	//---------------------------------------------------------------------------
	void CCharacterCreateFlow::RenderScreenFrame()
	{
	}
	//---------------------------------------------------------------------------
	void CCharacterCreateFlow::RenderUIFrame()
	{
		if( g_pEngine )
			g_pEngine->ScreenSpace();

		if(m_pCharCreate)
			m_pCharCreate->Render();

		if( TOOLTIPMNG_STPTR )
			TOOLTIPMNG_STPTR->Render();

		if( g_pSystemMsg )
			g_pSystemMsg->Render();

		cMessageBox::RenderList();
		
		if( m_pFadeUI )
			m_pFadeUI->Render();
	}

	BOOL CCharacterCreateFlow::LoadResource()
	{
		return TRUE;
	}

	void CCharacterCreateFlow::ReleaseResource()
	{
	}
}
//---------------------------------------------------------------------------