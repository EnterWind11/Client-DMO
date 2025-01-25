#include "stdafx.h"
#include "CharacterCreate.h"

#include "../../Flow/Flow.h"
#include "../../Flow/FlowMgr.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"

CCharacterCreate::CCharacterCreate() : 
m_pIntroSound(NULL),
m_pButtonOK(NULL),
m_pButtonCancel(NULL),
m_pTamerName(NULL),
m_pEditBox(NULL),
bIMECheck(false),
m_pTamerListBox(NULL),
m_pLefBtn(NULL),
m_pRightBtn(NULL),
m_pTamerSkills(NULL),
m_pSkillList(NULL),
m_pTamerDesc(NULL),
m_pBaseStateAT(NULL),
m_pBaseStateDP(NULL),
m_pBaseStateHP(NULL),
m_pBaseStateDS(NULL),
m_pCostumList(NULL)
{
}

CCharacterCreate::~CCharacterCreate()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );

	Destory();
}


void CCharacterCreate::Destory()
{
	g_IME.SetLockConidateList( false );
	DeleteScript();
	m_MainButtonUI.DeleteScript();
	m_CharacterListUI.DeleteScript();
	m_pCostumSelectWindow.DeleteScript();
	if( m_pIntroSound )
	{
		if( g_pSoundMgr )
		{
			g_pSoundMgr->StopSound( m_pIntroSound );
			g_pSoundMgr->Set_BGM_FadeVolume( m_fBackupMusic );
		}		
		m_pIntroSound = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////
bool CCharacterCreate::Init()
{
	SetCameraInfo();// [5/2/2016 hyun]

	GetSystem()->MakeCreatedTamerList();
	
	m_fBackupMusic = g_pResist->m_Global.s_fMusic;
	g_IME.SetLockConidateList( true );

	InitScript( "Lobby\\CharacterCreate\\Background.dds", CsPoint::ZERO, CsPoint( g_nScreenWidth, g_nScreenHeight ), false );

	makeMainButtonUI();
	makeCostumWindowUI();
	makeTamerWindowUI();

	// 제일 마지막에 호출 여기에서 테이머 선택을 하기 때문에 
	// 컨트롤이 생성 되기 전에 데이터 셋팅이 이루어지기 때문
	setTamerList();
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// 메인 버튼 UI 생성
//////////////////////////////////////////////////////////////////////////////////////////////
void CCharacterCreate::makeMainButtonUI()
{
	m_MainButtonUI.InitScript( NULL/*"CharSelect\\BottomBack.tga"*/, CsPoint::ZERO, CsPoint( g_nScreenWidth, g_nScreenHeight ), false );
	m_MainButtonUI.AddSprite( CsPoint((g_nScreenWidth - 446)/2, 0 ), CsPoint( 446, 118 ), "Lobby\\TopText_BG.tga" );

	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_16, FONT_WHITE );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT("CHARACTER_CREATE_TAMER_TITLE").c_str() );
		m_MainButtonUI.AddText( &ti, CsPoint(g_nScreenWidth/2, 49 ), true );
	}

	m_pButtonCancel = m_MainButtonUI.AddButton( CsPoint( (g_nScreenWidth - 176)/ 2 - 400, g_nScreenHeight - 80 ), CsPoint( 176, 50 ), CsPoint( 0, 50 ), "CommonUI\\Simple_btn_L.tga", cWindow::SD_Bu3);	
	if( m_pButtonCancel )
	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_14, NiColor::WHITE );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT( "CHARACTER_CREATE_BACK" ).c_str() ); // 뒤로
		m_pButtonCancel->SetText( &ti );
		m_pButtonCancel->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CCharacterCreate::PressCancelButton );
	}

	m_MainButtonUI.AddSprite( CsPoint( (g_nScreenWidth - 256) / 2, g_nScreenHeight - 80 ), CsPoint( 256, 68 ), "Lobby\\CharacterCreate\\input_field.tga", true );

	{
		m_pEditBox = NiNew cEditBox;
		if( m_pEditBox )
		{
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontSystem, CFont::FS_12 );
			ti.s_Color = NiColor::WHITE;
			ti.s_eTextAlign = DT_LEFT;
			m_pEditBox->Init( m_MainButtonUI.GetRoot(),  CsPoint( (g_nScreenWidth - 150) / 2, g_nScreenHeight - 55 ), CsPoint( 150, 28 ), &ti, false );	
			m_pEditBox->EnableUnderline();
			m_pEditBox->SetEmptyMsgText( UISTRING_TEXT("CHARACTER_CREATE_TAMER_NAME_INPUT_EMPTY_MSG").c_str(), NiColor(0.5f,0.5f,0.5f) );
			m_pEditBox->SetFontLength( NAME_MAX_LEN );
			m_pEditBox->SetFocus();
			m_MainButtonUI.AddChildControl( m_pEditBox );
		}
	}

	m_pButtonOK = m_MainButtonUI.AddButton( CsPoint( (g_nScreenWidth - 176)/ 2 + 400, g_nScreenHeight - 80 ), CsPoint( 176, 50 ), CsPoint( 0, 50 ), "CommonUI\\Simple_btn_L.tga", cWindow::SD_Bu3);	
	if( m_pButtonOK )
	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_14, NiColor::WHITE );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT( "CHARACTER_CREATE_CREATE" ).c_str() );
		m_pButtonOK->SetText( &ti );
		m_pButtonOK->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CCharacterCreate::PressOkButton );
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////
// 테이머 목록
//////////////////////////////////////////////////////////////////////////////////////////////
void CCharacterCreate::makeTamerWindowUI()
{
	m_CharacterListUI.InitScript( "Lobby\\CharacterCreate\\ListBack.tga", CsPoint(g_nScreenWidth - 530, (g_nScreenHeight - 592)/2), CsPoint( 512, 592 ), false );

	m_pLefBtn = m_CharacterListUI.AddButton( CsPoint(6,128), CsPoint(62, 98), CsPoint(62, 0), "CommonUI\\Arrow_Left.tga", cWindow::SD_Bu3 );
	if( m_pLefBtn )
		m_pLefBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CCharacterCreate::_ChangeSelectedLeft );
	m_pRightBtn = m_CharacterListUI.AddButton( CsPoint(450, 128), CsPoint(62, 98), CsPoint(62, 0), "CommonUI\\Arrow_Right.tga", cWindow::SD_Bu3  );
	if( m_pRightBtn )
		m_pRightBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CCharacterCreate::_ChangeSelectedRight );

	m_pTamerListBox = NiNew cGridListBox;
	SAFE_POINTER_RET( m_pTamerListBox );

	m_pTamerListBox->Init( GetRoot(), CsPoint( 70, 23 ), CsPoint(376, 300)
		, CsPoint::ZERO, CsPoint(94, 99), cGridListBox::LowRightDown, cGridListBox::LeftTop, NULL, false, 0 );
	m_pTamerListBox->AddEvent( cGridListBox::GRID_SELECT_CHANGE_ITEM, this, &CCharacterCreate::_SelectedTamer );
	m_CharacterListUI.AddChildControl(m_pTamerListBox);
	m_pTamerListBox->SetMouseOverImg( "Lobby\\CharacterCreate\\TamerIcon_glow.tga" );
	m_pTamerListBox->SetSelectedImg( "Lobby\\CharacterCreate\\TamerIcon_glow.tga" );
	m_pTamerListBox->SetBackOverAndSelectedImgRender(false);

	m_CharacterListUI.AddSprite( CsPoint(6, 337), CsPoint(500,38), "Lobby\\CharacterCreate\\char_gline.tga" );

	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_16, FONT_WHITE );
		m_pTamerName = m_CharacterListUI.AddText( &ti, CsPoint(21, 347 ), true );
	}

	m_CharacterListUI.AddSprite( CsPoint(6, 375), CsPoint(494,105), "Lobby\\CharacterCreate\\TextBack_line.tga" );
	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_11, FONT_WHITE );
		m_pTamerDesc = m_CharacterListUI.AddText( &ti, CsPoint(21, 380 ), true );
	}

	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, FONT_NEWGOLD );
		ti.SetText( UISTRING_TEXT("CHARACTER_CREATE_TAMER_HAVE_SKILL").c_str() );
		m_CharacterListUI.AddText( &ti, CsPoint(25, 510 ), true );
	}

	m_pTamerSkills = NiNew cGridListBox;
	if( m_pTamerSkills )
	{
		m_pTamerSkills->Init( NULL, CsPoint( 18, 536 ), CsPoint(460, 35), CsPoint(10,0), CsPoint(32, 32), cGridListBox::LowRightDown, cGridListBox::LeftTop, NULL, false, 0 );
		m_pTamerSkills->SetMouseOverImg( "Icon\\Mask_Over.tga" );
		m_pTamerSkills->SetBackOverAndSelectedImgRender(false);
		m_CharacterListUI.AddChildControl(m_pTamerSkills);
	}

	{
		m_CharacterListUI.AddSprite( CsPoint(204, 518 ), CsPoint(282, 50 ), "Lobby\\CharacterCreate\\Char_status.tga");

		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, FONT_GREEN );
		ti.s_eTextAlign = DT_CENTER;
		m_pBaseStateAT = m_CharacterListUI.AddText( &ti, CsPoint(239, 546 ) );
		m_pBaseStateDP = m_CharacterListUI.AddText( &ti, CsPoint(309, 546 ) );
		m_pBaseStateHP = m_CharacterListUI.AddText( &ti, CsPoint(379, 546 ) );
		m_pBaseStateDS = m_CharacterListUI.AddText( &ti, CsPoint(449, 546 ) );
	}	
}

void CCharacterCreate::makeCostumWindowUI()
{
	m_pCostumSelectWindow.InitScript( NULL, CsPoint( 10, (g_nScreenHeight - 350) /2 ), CsPoint( 156, 350 ), false );

	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_14, FONT_NEWGOLD );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT("CHARACTER_CREATE_TAMER_COSTUME_LIST_TITLE").c_str() );
		m_pCostumSelectWindow.AddText( &ti, CsPoint(78,0) );
	}

	m_pCostumList = NiNew cGridListBox;
	if( m_pCostumList )
	{
		m_pCostumList->Init( NULL, CsPoint(52,30), CsPoint(52, 310), CsPoint::ZERO, CsPoint(52, 51), cGridListBox::LowRightDown, cGridListBox::LeftTop, NULL, false, 0 );
		m_pCostumList->AddEvent( cGridListBox::GRID_BUTTON_UP, this, &CCharacterCreate::_ChangeCostume );
		m_pCostumSelectWindow.AddChildControl(m_pCostumList);
	}
}

// 선택한 테이머의 코스튬 정보 갱신
void CCharacterCreate::_UpdateCostumData( DWORD const& dwTamerIDX, std::list<DWORD> const& costum )
{
	SAFE_POINTER_RET( m_pCostumList );
	m_pCostumList->RemoveAllItem();

	if( costum.empty() )
	{
		m_pCostumSelectWindow.SetVisible(false);
		return;
	}

	m_pCostumSelectWindow.SetVisible(true);

	int n = 0;
	std::list<DWORD>::const_iterator it = costum.begin();
	for( ; it != costum.end(); ++it, ++n )
	{
		cString * pItem = NiNew cString;
		SAFE_POINTER_BEK( pItem );

		std::string file;
		DmCS::StringFn::FormatA( file, "Lobby\\CharacterCreate\\lobby_costume_%02d.tga", n + 1 );

		cButton* pBtn = NiNew cButton;
		if( pBtn )
		{
			pBtn->Init( NULL, CsPoint::ZERO, CsPoint( 52, 51), file.c_str(), false );
			pBtn->SetUserData( new sCostumeInfo( dwTamerIDX, (*it) ) );
			pBtn->SetTexToken(CsPoint(0,51));
			pBtn->SetSound( cWindow::SD_Bu3 );
		}

		cString::sBUTTON* paddImg = pItem->AddButton( pBtn, 0, CsPoint::ZERO, CsPoint( 52, 51) );
		if( paddImg )
			paddImg->SetAutoPointerDelete(true);

		cGridListBoxItem * addItem  = NiNew cGridListBoxItem(n, CsPoint( 52, 51));
		addItem->SetItem( pItem );
		std::wstring name = GetSystem()->GetItemName((*it));
		addItem->SetUserData( new sCostumeName( name ) );
		m_pCostumList->AddItem( addItem );
	}

	{
		cString * pItem = NiNew cString;
		cButton* pBtn = NiNew cButton;
		pBtn->Init( NULL, CsPoint::ZERO, CsPoint( 52, 51), "Lobby\\SelectServer\\lobby_server_refresh.tga", false );
		pBtn->SetUserData( new sCostumeInfo( dwTamerIDX, 0 ) );
		pBtn->SetTexToken(CsPoint(0,51));
		cString::sBUTTON* paddImg = pItem->AddButton( pBtn, 0, CsPoint::ZERO, CsPoint( 52, 51) );
		if( paddImg )
			paddImg->SetAutoPointerDelete(true);
		cGridListBoxItem * addItem  = NiNew cGridListBoxItem(n++, CsPoint( 52, 51));
		addItem->SetItem( pItem );
		addItem->SetUserData( new sCostumeName( UISTRING_TEXT("CHARACTER_CREATE_TAMER_COSTUME_UNEQUIP").c_str() ) );
		m_pCostumList->AddItem( addItem );
	}
}

void CCharacterCreate::_ChangeCostume(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cString::sBUTTON* pButton = static_cast< cString::sBUTTON* >(pData);
	SAFE_POINTER_RET(pButton->s_pButton);

	sCostumeInfo* pUserData = dynamic_cast<sCostumeInfo*>(pButton->s_pButton->GetUserData());
	SAFE_POINTER_RET(pUserData);

	SAFE_POINTER_RET(GetSystem());

	GetSystem()->ChangeTamerCostume( pUserData->m_dwCostumeID );
}

void CCharacterCreate::setTamerList()
{
	SAFE_POINTER_RET( m_pTamerListBox );

	m_pTamerListBox->RemoveAllItem();
	CharacterCreateContents::LIST_TAMER_INFO const& pList = GetSystem()->GetMakeTamerList();
	CharacterCreateContents::LIST_TAMER_INFO_CIT it = pList.begin();
	for( int n = 0; it != pList.end(); ++it, ++n )
	{
		cString * pItem = NiNew cString;
		SAFE_POINTER_BEK( pItem );

		// 테이머 이지미
		cImage* pTamerImage = NiNew cImage;	// 1
		if( pTamerImage )
		{
			pTamerImage->Init( NULL, CsPoint::ZERO, CsPoint( 85, 91 ), "Lobby\\CharacterCreate\\TamerIcon_L.tga", false );
			pTamerImage->SetTexToken( CsPoint( 85, 0 ) );
			cString::sIMAGE* sImg = pItem->AddImage( pTamerImage, (*it).m_nIconIdx, CsPoint(4,4), CsPoint( 85, 91 )  );	
			if( sImg )
				sImg->SetAutoPointerDelete(true);

			if( !(*it).m_nEnableCreated )
				sImg->SetColor( NiColor(0.3f,0.3f,0.3f) );
		}		
		
		cGridListBoxItem * addItem  = NiNew cGridListBoxItem(n, CsPoint( 94, 99 ));
		addItem->SetItem( pItem );
		addItem->SetUserData( new sTamerInfo( (*it).m_nFileTableID, (*it).m_nEnableCreated ) );
		m_pTamerListBox->AddItem( addItem );
	}
	
	m_pTamerListBox->SetSelectedItemFromIdx(0, true);
}
//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////
void CCharacterCreate::Update3DModel(float fDeltaTime)
{
	if( GetSystem() )
		GetSystem()->Update_TamerModel(fDeltaTime);
}

void CCharacterCreate::UpdateSound()
{
	SAFE_POINTER_RET( g_pSoundMgr );
	SAFE_POINTER_RET( m_pIntroSound );

	if( g_pSoundMgr->IsSound( m_pIntroSound ) == false )
	{
		g_pSoundMgr->Set_BGM_FadeVolume( m_fBackupMusic );
		m_pIntroSound = NULL;
	}
	else
		g_pSoundMgr->Set_BGM_Volume( 0.07f );
}

void CCharacterCreate::ChangeCharacterSound( std::string const& soundFile )
{
	SAFE_POINTER_RET( g_pSoundMgr );

	if( m_pIntroSound )
	{
		g_pSoundMgr->StopSound( m_pIntroSound );
		m_pIntroSound = NULL;
	}

	if( soundFile.empty() )
		return;

	std::string file = soundFile;
	m_pIntroSound = g_pSoundMgr->PlaySystemSound( const_cast<char*>(file.c_str()) );
	// 사운드 플레이가 된다면
	if( m_pIntroSound )
		m_pIntroSound->SetVolume( 1.0f );
}

BOOL CCharacterCreate::UpdateMouse()
{
	if(m_pButtonOK && m_pButtonOK->Update_ForMouse() == cButton::ACTION_CLICK )
		return TRUE;

	if(m_pButtonCancel && m_pButtonCancel->Update_ForMouse() == cButton::ACTION_CLICK )
		return TRUE;

	if(m_pLefBtn && m_pLefBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		return TRUE;

	if(m_pRightBtn && m_pRightBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		return TRUE;

	if(m_pTamerListBox && m_pTamerListBox->Update_ForMouse(CURSOR_ST.GetPos()) )
		return TRUE;
	
	if( m_pCostumSelectWindow.IsVisible() )
	{
		if(m_pCostumList&& m_pCostumList->Update_ForMouse(CURSOR_ST.GetPos()) )
		{
			cGridListBoxItem const* pOverGrid = m_pCostumList->GetMouseOverItem();
			if(pOverGrid)
			{
				sCostumeName* pCostumeInfo = dynamic_cast< sCostumeName* >(pOverGrid->GetUserData());
				if( pCostumeInfo )
					TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip_Text( pOverGrid->GetWorldPos(), CsPoint(52,51), pCostumeInfo->m_sItemName.c_str(), CFont::FS_13);
			}	
			return TRUE;
		}
	}
	
	if(m_pTamerSkills&& m_pTamerSkills->Update_ForMouse(CURSOR_ST.GetPos()) )
	{
		cGridListBoxItem const* pOverGrid = m_pTamerSkills->GetMouseOverItem();
		if(pOverGrid)
		{
			sSkillInfo* pSkillInfo = dynamic_cast< sSkillInfo* >(pOverGrid->GetUserData());
			if( pSkillInfo )
				TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pOverGrid->GetWorldPos(), CsPoint(32,32), 360, cTooltip::SKILL_SIMPLE, pSkillInfo->m_nTableIDX, 0 );
		}	
		return TRUE;
	}

// 	if(m_pSkillList && m_pSkillList->Update_ForMouse(CURSOR_ST.GetPos()) )
// 		return TRUE;	
	
	return FALSE;
}

BOOL CCharacterCreate::UpdateKeyboard(const MSG& p_kMsg)
{
	const int isBitSet = (DWORD)p_kMsg.lParam & 0x40000000; 

	switch(p_kMsg.message)
	{
	case WM_IME_NOTIFY:
		{
			if( !g_IME.IsLockChangeLanguage() )
			{
				if( g_IME.OnNotify( GAMEAPP_ST.GetHWnd(), p_kMsg.wParam, p_kMsg.lParam ) == true )
					bIMECheck = true;
			}
			return TRUE;
		}		
		break;

	case WM_KEYDOWN:
		{
			switch(p_kMsg.wParam)
			{
			case VK_RETURN:
				{
					if(isBitSet == 0 && m_pButtonOK && m_pButtonOK->IsEnable())
					{
						m_pButtonOK->KeyboardBtnDn();
					}
					return TRUE;
				}
				break;

			case VK_ESCAPE:
				{
					if(isBitSet == 0)
					{
						if(m_pButtonCancel)
							m_pButtonCancel->KeyboardBtnDn();
					}
					return TRUE;
				}
				break;
// 			case VK_LEFT:	GetSystem()->TamerSelChange_backward();		return TRUE;
// 			case VK_RIGHT:	GetSystem()->TamerSelChange_forward();		return TRUE;
			}
			break;
		}
		break;

	case WM_KEYUP:
		{
			switch(p_kMsg.wParam)
			{
			case VK_RETURN:
				{
					if(bIMECheck)
					{
						bIMECheck = false;
						return FALSE;
					}

#ifndef NOT_ENTER_CREATENAME	// 대만에서는 글자 조합 선택 시 Enter로 하므로 조합 중 Enter로 생성되지 않도록 함

					if( m_pButtonOK && m_pButtonOK->IsEnable() )
					{
						PressOkButton(m_pButtonOK,NULL);
						m_pButtonOK->KeyboardBtnUp();
						return TRUE;
					}
#endif					
					return TRUE;
				}
				break;

			case VK_ESCAPE:
				{
					if(bIMECheck)
					{
						bIMECheck = false;
						return FALSE;
					}

					if( m_pButtonCancel && GetSystem() )
					{
						m_pButtonCancel->KeyboardBtnUp();
						GetSystem()->GotoBack();
					}

					return TRUE;
				}
				break;
			}
		}
		break;
	}

	return FALSE;
}

void CCharacterCreate::Update( float fDeltaTime )
{
	Update3DModel(fDeltaTime);
	UpdateSound();
}


//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////

void CCharacterCreate::RenderScreenUI()
{
	RenderScript();	
}

void CCharacterCreate::Render3DModel()
{
	if( GetSystem() )
		GetSystem()->Render_TamerModel();
}

void CCharacterCreate::Render()
{
	m_MainButtonUI.RenderScript();
	m_CharacterListUI.RenderScript();
	m_pCostumSelectWindow.RenderScript();
}

void CCharacterCreate::ResetDevice()
{
	ResetDeviceScript();
	m_MainButtonUI.ResetDeviceScript();
	m_CharacterListUI.ResetDeviceScript();
	m_pCostumSelectWindow.ResetDeviceScript();
}

void CCharacterCreate::CharSelectedChange(unsigned int const& nChangeSelectIdx)
{
	SAFE_POINTER_RET( GetSystem() );

	CharacterCreateContents::sTamerCreatedInfo const* pInfo = GetSystem()->GetTamerInfo( nChangeSelectIdx );
	SAFE_POINTER_RET( pInfo );

	if( m_pTamerName )
	{
		std::wstring text = UISTRING_TEXT( "CHARACTER_CREATE_TAMER_SELECTED_NAME" );
		DmCS::StringFn::ReplaceAll( text, L"#TargetName#", pInfo->m_szName );
		DmCS::StringFn::ReplaceAll( text, L"#Season#", pInfo->m_szSeasonName );
		m_pTamerName->SetText( text.c_str() );
	}
	if( m_pTamerDesc )
		m_pTamerDesc->SetText( pInfo->m_szComment.c_str(), 450 );

	if( m_pButtonOK )
		m_pButtonOK->SetEnable(pInfo->m_nEnableCreated);

	ChangeCharacterSound( pInfo->m_soundFileName );
	_ResetTamerSkill( pInfo );

	// 테이머 기본 능력치 갱신
	_UpdateTamerBaseState( pInfo->m_mapTamerState );
	// 선택한 테이머의 코스튬 정보 갱신
	_UpdateCostumData( pInfo->m_nFileTableID, pInfo->m_listCostume );
}

void CCharacterCreate::_ResetTamerSkill(CharacterCreateContents::sTamerCreatedInfo const* pInfo )
{
// 	SAFE_POINTER_RET( pInfo );
// 	SAFE_POINTER_RET( m_pSkillList );
// 	m_pSkillList->RemoveAllItem();
// 	
// 	CharacterCreateContents::LIST_SKILL_INFO_CIT it = pInfo->m_skilList.begin();
// 	for( int n = 0; it != pInfo->m_skilList.end(); ++it, ++n )
// 	{
// 		cString * pItem = NiNew cString;
// 		SAFE_POINTER_BEK( pItem );
// 
// 		cString::sICON* pSkillIcon = pItem->AddIcon( CsPoint(32,32), ICONITEM::SKILL1, (*it).m_dwIconIndex, true );
// 		if( pSkillIcon )
// 			pSkillIcon->SetAutoPointerDelete(true);
// 
// 		{		
// 			NiColor niTextColor = FONT_YELLOW;
// 			std::wstring msg;
// 			switch( (*it).m_nAttType )
// 			{
// 			case 0:	// 테이머의 경우 일반 스킬
// 				{
// 					DmCS::StringFn::Format( msg, L"%s - %s", (*it).m_wsName.c_str(), _LAN( "기본 스킬" ) );
// 				}
// 				break;
// 			case 1:	// 엑티브 스킬
// 				{
// 					//niTextColor = FONT_WHITE;
// 					DmCS::StringFn::Format( msg, L"%s - %s", (*it).m_wsName.c_str(), _LAN( "엑티브 스킬" ) );
// 				}
// 				break;
// 			case 4:	// 패시브 스킬
// 				{
// 					//niTextColor = FONT_WHITE;
// 					DmCS::StringFn::Format( msg, L"%s - %s", (*it).m_wsName.c_str(), _LAN( "패시브 스킬" ) );
// 				}
// 				break;
// 			default:
// 				DmCS::StringFn::Format( msg, L"%s", (*it).m_wsName.c_str() );		
// 				break;
// 			}
// 			cText::sTEXTINFO ti;
// 			ti.Init( CFont::FS_11, niTextColor );
// 			ti.SetText( msg.c_str() );
// 			pItem->AddText( &ti, CsPoint(40,0) );
// 		}
// 
// 		{
// 			cText::sTEXTINFO ti;
// 			ti.Init( CFont::FS_11, FONT_WHITE );
// 			ti.SetText( L"" );
// 			cString::sTEXT* pText = pItem->AddText( &ti, CsPoint(40,20) );
// 			if( pText )
// 				pText->SetText( (*it).m_wsDesc.c_str(), 420 );
// 		}
// 
// 		cListBoxItem * addItem  = NiNew cListBoxItem;
// 		addItem->SetItem( pItem );
// 		m_pSkillList->AddItem( addItem );
// 	}
	
	SAFE_POINTER_RET( m_pTamerSkills );
	m_pTamerSkills->RemoveAllItem();

	SAFE_POINTER_RET( pInfo );
	CharacterCreateContents::LIST_SKILL_INFO_CIT it = pInfo->m_skilList.begin();
	for( int n = 0; it != pInfo->m_skilList.end(); ++it, ++n )
	{
		cString * pItem = NiNew cString;
		SAFE_POINTER_BEK( pItem );

		ICONITEM::eTYPE type = ICONITEM::SKILL1;
		switch( (*it).m_dwIconIndex )
		{
		case CsSkill::IT_CHANGE3:
		case CsSkill::IT_CHANGE1: 
		case CsSkill::IT_CHANGE2:	type = ICONITEM::SKILL_MASK;	break;			
		default:
			{
				if( (*it).m_dwIconIndex >= 4000 )
					type = ICONITEM::SKILL4;
				else if( (*it).m_dwIconIndex >= 3000 )
					type = ICONITEM::SKILL3;
				else if( (*it).m_dwIconIndex >= 2000 )
					type = ICONITEM::SKILL2;
			}
			break;
		}	

		cString::sICON* pSkillIcon = NULL;

		if( type != ICONITEM::SKILL_MASK )
			pSkillIcon =  pItem->AddIcon( CsPoint(32,32), type, (*it).m_dwIconIndex%1000, 1 );
		else
			pSkillIcon =  pItem->AddIcon( CsPoint(32,32), type, (*it).m_dwIconIndex, 1 );
		
		if( pSkillIcon )
			pSkillIcon->SetAutoPointerDelete(true);

		cGridListBoxItem * addItem  = NiNew cGridListBoxItem(n, CsPoint( 32, 32 ));
		addItem->SetItem( pItem );
		addItem->SetUserData( new sSkillInfo( (*it).m_dwSkillCode ) );
		m_pTamerSkills->AddItem( addItem );
	}
}

// 테이머 기본 능력치 갱신
void CCharacterCreate::_UpdateTamerBaseState( std::map<int,int> const& baseState )
{
	if( m_pBaseStateAT )	m_pBaseStateAT->SetText( L"" );
	if( m_pBaseStateDP )	m_pBaseStateDP->SetText( L"" );
	if( m_pBaseStateHP )	m_pBaseStateHP->SetText( L"" );
	if( m_pBaseStateDS )	m_pBaseStateDS->SetText( L"" );

	std::map<int,int>::const_iterator it = baseState.begin();
	for( ; it != baseState.end(); ++it )
	{
		switch( it->first )
		{
		case eBaseStats::AP:
			if( m_pBaseStateAT )
				m_pBaseStateAT->SetText( it->second );
			break;
		case eBaseStats::DP:
			if( m_pBaseStateDP )
				m_pBaseStateDP->SetText( it->second );
			break;
		case eBaseStats::HP:
			if( m_pBaseStateHP )
				m_pBaseStateHP->SetText( it->second );
			break;
		case eBaseStats::DS:
			if( m_pBaseStateDS )
				m_pBaseStateDS->SetText( it->second );
			break;
		}
	}
}


void CCharacterCreate::PressOkButton( void* pSender, void *pData )
{
	SAFE_POINTER_RET( m_pEditBox );
	std::wstring name = m_pEditBox->GetTextAll();
	if( GetSystem() )
		GetSystem()->SendCheckTamerName( name );
}

void CCharacterCreate::PressCancelButton( void* pSender, void *pData )
{
	if( GetSystem() )
		GetSystem()->GotoBack();
}

void CCharacterCreate::_SelectedTamer(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pData);

	sTamerInfo* pUserData = dynamic_cast<sTamerInfo*>(pItem->GetUserData());
	SAFE_POINTER_RET(pUserData);

	SAFE_POINTER_RET(GetSystem());
	GetSystem()->SetSelectTamerIdx( pUserData->m_nTableIDX );
}

void CCharacterCreate::SetCameraInfo()
{
	// 카메라 셋팅
	sCAMERAINFO ci;
	ci.s_fDist = 380.0f;
	ci.s_fFarPlane = 100000.0f;
	ci.s_fInitPitch = 0.0f;
	CAMERA_ST.Reset( &ci );
	CAMERA_ST.ReleaseDistRange();
	CAMERA_ST.ReleaseRotationLimit();
	CAMERA_ST.SetDeltaHeight( 0.0f );
	CAMERA_ST._UpdateCamera();
}


void CCharacterCreate::_ChangeSelectedLeft(void* pSender, void* pData)
{
	if( m_pTamerListBox )
		m_pTamerListBox->ChangeSelectFront();		
}

void CCharacterCreate::_ChangeSelectedRight(void* pSender, void* pData)
{
	if( m_pTamerListBox )
		m_pTamerListBox->ChangeSelectNext();		
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

bool CCharacterCreate::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eTamer_Selected, this );

	return true;
}

void CCharacterCreate::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case SystemType::eTamer_Selected:
		{
			unsigned int nSelectedIdx = 0;
			kStream >> nSelectedIdx;
			CharSelectedChange( nSelectedIdx );
		}
		break;
	}
}