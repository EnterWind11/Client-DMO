//---------------------------------------------------------------------------
//
//
//---------------------------------------------------------------------------
#include "StdAfx.h"
#include "SecondPasswordFlow.h"
#include "../_Interface/04.SecondPassword/SecondPassword.h"
#include "../_Interface/Game/Fade.h"
#include "../ContentsSystem/ContentsSystem.h"

//---------------------------------------------------------------------------

namespace Flow
{
	CSecondPasswordFlow::CSecondPasswordFlow(int p_iID) : CFlow(p_iID), m_bBgmPlay(false), m_pSecondPassword(NULL),m_pFadeUI(NULL)
	{
	}
	//---------------------------------------------------------------------------
	CSecondPasswordFlow::~CSecondPasswordFlow()
	{
	}
	//---------------------------------------------------------------------------
	BOOL CSecondPasswordFlow::Initialize()
	{
		if( g_pSoundMgr )
			g_pSoundMgr->PlayMusic( "Game_opening.mp3", 0.0f );

		m_pSecondPassword = NiNew cSecondPassword;
 		if( m_pSecondPassword && m_pSecondPassword->Construct() )
 			m_pSecondPassword->Init();

		m_pFadeUI = NiNew CFade;
		if( m_pFadeUI )
			m_pFadeUI->Init(CFade::FADE_IN, 0.5f);

		g_pEngine->SetGaussianBlurVal( 0.9f, 0.25f, 0.3f );
		return TRUE;
	}

	void CSecondPasswordFlow::OnEnter(void)
	{		
		CFlow::OnEnter();
		CONTENTSSYSTEM_PTR->InitializeContents(eContentsType::E_CT_SECOND_PASSWORD);
	}

	void CSecondPasswordFlow::OnExit(int p_iNextFlowID)
	{
		CFlow::OnExit(p_iNextFlowID);
		CONTENTSSYSTEM_PTR->InitializeContents(eContentsType::E_CT_SECOND_PASSWORD);
	}

	void CSecondPasswordFlow::ReservedChangeFlow(int p_iNextFlowID)
	{
		if( m_pFadeUI )
			m_pFadeUI->Reset(CFade::FADE_OUT, 0.5f);
	}
	//---------------------------------------------------------------------------
	void CSecondPasswordFlow::Terminate()
	{
		SAFE_NIDELETE(m_pFadeUI);
		SAFE_NIDELETE(m_pSecondPassword);
		RESOURCEMGR_ST.CleanUpResource();
	}

	BOOL CSecondPasswordFlow::OnMsgHandler(const MSG& p_kMsg)
	{
		CURSOR_ST.Input(p_kMsg);

		if( cMessageBox::UpdateList() )
			return FALSE;	

		if( m_pSecondPassword )
			m_pSecondPassword->UpdateKeyboard(p_kMsg);

		return FALSE;
	}

	void CSecondPasswordFlow::InputFrame()            // Input     
	{
		if( cMessageBox::UpdateList() )
			return;

		if( m_pSecondPassword )
			m_pSecondPassword->UpdateMouse();
	}
	//---------------------------------------------------------------------------
	bool CSecondPasswordFlow::LostDevice(void* p_pvData)
	{
		return true;
	}
	//---------------------------------------------------------------------------
	bool CSecondPasswordFlow::ResetDevice(bool p_bBeforeReset, void* p_pvData)
	{
		return true;
	}
	//---------------------------------------------------------------------------
	void CSecondPasswordFlow::RenderBackScreenFrame()
	{
		// 2D Screen
		if( m_pSecondPassword )
			m_pSecondPassword->RenderScreenUI();
	}
	//---------------------------------------------------------------------------
	void CSecondPasswordFlow::RenderSceneFrame()
	{
		// 3D 렌더, 월드, 캐릭터, 이팩트 등등 ...
		if( g_pEngine )
			g_pEngine->ResetRendererCamera();

		if( m_pSecondPassword )
			m_pSecondPassword->Render3DModel();

		if( g_pEngine )
			g_pEngine->Render();
	}
	//---------------------------------------------------------------------------
	void CSecondPasswordFlow::UpdateFrame()
	{
		CONTENTSSYSTEM_PTR->Update( (float)g_fDeltaTime );

		g_pSystemMsg->Update();

		if( m_pSecondPassword )
		{
			m_pSecondPassword->Update();
			m_pSecondPassword->Update3DAccumtime( g_fAccumTime );
		}

		if( m_pFadeUI )
			m_pFadeUI->Update(g_fDeltaTime);

		if( TOOLTIPMNG_STPTR )
			TOOLTIPMNG_STPTR->Update( g_fDeltaTime );

		CURSOR_ST.LoopReset();	//2016-02-23-nova
	}
	//---------------------------------------------------------------------------
	void CSecondPasswordFlow::RenderUIFrame()
	{
		if( g_pEngine )
			g_pEngine->ScreenSpace();

		if( m_pSecondPassword )
			m_pSecondPassword->Render();

		if( TOOLTIPMNG_STPTR )
			TOOLTIPMNG_STPTR->Render();

		if( g_pSystemMsg )
			g_pSystemMsg->Render();

		cMessageBox::RenderList();

		if( m_pFadeUI )
			m_pFadeUI->Render();
	}
}
//---------------------------------------------------------------------------