//---------------------------------------------------------------------------
//
// 파일명 : LoginFlow.cpp
// 작성일 : 
// 작성자 : 
// 설  명 : 
//
//---------------------------------------------------------------------------
#include "StdAfx.h"
#include "CharacterSelectFlow.h"

#include "../_Interface/06.CharacterSelect/CharacterSelect.h"
#include "../_Interface/Game/Fade.h"
#include "../ContentsSystem/ContentsSystem.h"

//---------------------------------------------------------------------------

namespace Flow
{
	CCharacterSelectFlow::CCharacterSelectFlow(int p_iID) : CFlow(p_iID), m_bBgmPlay(false),m_pFadeUI(NULL),m_pCharSelect(NULL)
	{
	}
	//---------------------------------------------------------------------------
	CCharacterSelectFlow::~CCharacterSelectFlow()
	{
	}

	void CCharacterSelectFlow::OnOverride(int p_iNextFlowID)
	{
		SetPaused( true );
	}

	void CCharacterSelectFlow::OnResume(int p_iNextFlowID)
	{
		SetPaused( false );	
		if( m_pFadeUI )
			m_pFadeUI->Reset(CFade::FADE_IN, 0.5f);
	}

	void CCharacterSelectFlow::ReservedChangeFlow(int p_iNextFlowID)
	{
		if( m_pFadeUI )
			m_pFadeUI->Reset(CFade::FADE_OUT, 0.5f);
	}

	void CCharacterSelectFlow::ReservedPushFlow(int p_iNextFlowID)
	{
		if( m_pFadeUI )
			m_pFadeUI->Reset(CFade::FADE_OUT, 0.5f);
	}

	//---------------------------------------------------------------------------
	BOOL CCharacterSelectFlow::Initialize()
	{
		m_pCharSelect = NiNew CCharacterSelect;		
		m_pFadeUI = NiNew CFade;

		if( m_pCharSelect && m_pCharSelect->Construct() )
			m_pCharSelect->Init();

		if( m_pFadeUI )
			m_pFadeUI->Init(CFade::FADE_IN, 0.5f);

		if( g_pSoundMgr )
			g_pSoundMgr->PlayMusic( "Game_opening.mp3", 0.0f );

		g_pEngine->SetGaussianBlurVal( 0.9f, 0.25f, 0.3f );

		return TRUE;
	}
	void CCharacterSelectFlow::OnEnter(void)
	{		
		CFlow::OnEnter();
		CONTENTSSYSTEM_PTR->InitializeContents(eContentsType::E_CT_CHARACTER_SELECT);
	}

	void CCharacterSelectFlow::OnExit(int p_iNextFlowID)
	{
		CFlow::OnExit(p_iNextFlowID);
		CONTENTSSYSTEM_PTR->InitializeContents(eContentsType::E_CT_CHARACTER_SELECT);
	}
	//---------------------------------------------------------------------------
	void CCharacterSelectFlow::Terminate()
	{
		SAFE_NIDELETE(m_pFadeUI);
		SAFE_NIDELETE(m_pCharSelect);
		RESOURCEMGR_ST.CleanUpResource();
	}
	//---------------------------------------------------------------------------
	bool CCharacterSelectFlow::LostDevice(void* p_pvData)
	{
		return true;
	}
	//---------------------------------------------------------------------------
	bool CCharacterSelectFlow::ResetDevice(bool p_bBeforeReset, void* p_pvData)
	{
		return true;
	}
	//---------------------------------------------------------------------------

	BOOL CCharacterSelectFlow::OnMsgHandler(const MSG& p_kMsg)
	{
		CURSOR_ST.Input(p_kMsg);

		if( cMessageBox::UpdateList() )
			return FALSE;	

		if( m_pCharSelect )
			m_pCharSelect->UpdateKeyboard(p_kMsg);

		return FALSE;
	}

	void CCharacterSelectFlow::InputFrame()
	{
		if( cMessageBox::UpdateList() )
			return;

		if( m_pCharSelect )
			m_pCharSelect->UpdateMouse();
	}
	//---------------------------------------------------------------------------
	void CCharacterSelectFlow::UpdateFrame()
	{
		CONTENTSSYSTEM_PTR->Update( (float)g_fDeltaTime );

		if( g_pSystemMsg )
			g_pSystemMsg->Update();

		if(m_pCharSelect)
			m_pCharSelect->Update( g_fAccumTime, g_fDeltaTime);

		if( m_pFadeUI )
			m_pFadeUI->Update(g_fDeltaTime);

		if( TOOLTIPMNG_STPTR )
			TOOLTIPMNG_STPTR->Update( g_fDeltaTime );

		CURSOR_ST.LoopReset();	//2016-02-23-nova
	}
	//---------------------------------------------------------------------------
	void CCharacterSelectFlow::CullFrame()
	{
	}
	//---------------------------------------------------------------------------
	void CCharacterSelectFlow::RenderSceneFrame()
	{
		// 3D 렌더, 월드, 캐릭터, 이팩트 등등 ...
		if( g_pEngine )
			g_pEngine->ResetRendererCamera();

		if( m_pCharSelect )
			m_pCharSelect->Render3DModel();

		if( g_pEngine )
			g_pEngine->Render();
	}

	void CCharacterSelectFlow::RenderBackScreenFrame()
	{
		// 2D Screen
		if( m_pCharSelect )
			m_pCharSelect->RenderScreenUI();
	}

	//---------------------------------------------------------------------------
	void CCharacterSelectFlow::RenderScreenFrame()
	{
	}
	//---------------------------------------------------------------------------
	void CCharacterSelectFlow::RenderUIFrame()
	{
		if( g_pEngine )
			g_pEngine->ScreenSpace();

		if( m_pCharSelect )
			m_pCharSelect->Render();

		if( TOOLTIPMNG_STPTR )
			TOOLTIPMNG_STPTR->Render();

		if( g_pSystemMsg )
			g_pSystemMsg->Render();

		cMessageBox::RenderList();

		if( m_pFadeUI )
			m_pFadeUI->Render();
	}

	BOOL CCharacterSelectFlow::LoadResource()
	{
		return TRUE;
	}

	void CCharacterSelectFlow::ReleaseResource()
	{
	}
}
//---------------------------------------------------------------------------