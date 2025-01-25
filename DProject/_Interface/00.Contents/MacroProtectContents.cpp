#include "StdAfx.h"
#include "../../Flow/FlowMgr.h"
#include "../../Flow/Flow.h"

#include "MacroProtectContents.h"
#include "../../ContentsSystem/ContentsSystemDef.h"

int const CMacroProtectContents::IsContentsIdentity(void)
{
	return E_CT_MACROPROTECT;
}

CMacroProtectContents::CMacroProtectContents(void):
#ifdef SDM_MACRO_PROTECT_RENEW_20181217
m_nMaxTimeSec(0),m_nRemainTimeSec(0),m_nRemainChanceCount(0),m_nQuestionType(nsMacroProtector::NONE)
#else
m_nMacroAnswer(0),m_nRandomAnswer(0),m_nTryCount(0)
#endif
{
#ifdef SDM_MACRO_PROTECT_RENEW_20181217
	ZeroMemory( &m_pQuestionData, CAPTCHA_BMP_SIZE);
#endif
	
	// Macro Protect é inspecionado pelo alvo do alvo
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::USING_MACROPROTECT, this, &CMacroProtectContents::_UsingMacroProtect );
	// Macro Protect é inspecionado pelo Target UIDX
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::USING_MACROPROTECT_TARGET_UIDX, this, &CMacroProtectContents::_UsingMacroProtect_UIDX );
	// Envie uma solicitação de proteção de macro para um usuário específico no servidor
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::SEND_MACROPROTECT_QUESTION, this, &CMacroProtectContents::_Send_MacroProtect_Using );
	// O resultado do jogador que solicitei para Macro Protect.
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::RECV_MACROPROTECT_QUESTION_RESULT, this, &CMacroProtectContents::_Recv_MacroProtect_Question_Result );	
	// O resultado de um macro player ou não
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::RECV_MACRO_PLAYER_RESULT, this, &CMacroProtectContents::_Recv_MacroPlayer_Result );	
	// Peço uma macro proteger.
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::NTF_MACROPROTECT_QUESTION_REQUEST, this, &CMacroProtectContents::_NTF_MacroProtect_Question_Request );	
}

CMacroProtectContents::~CMacroProtectContents(void)
{
	GAME_EVENT_STPTR->DeleteEventAll(this);
}

int const CMacroProtectContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool CMacroProtectContents::Initialize(void)
{
	return true;
}

void CMacroProtectContents::UnInitialize(void)
{

}

bool CMacroProtectContents::IntraConnection(ContentsSystem& System)
{
	return true;
}

void CMacroProtectContents::Update(float const& fElapsedTime)
{

}

void CMacroProtectContents::NotifyEvent(int const& iNotifiedEvt)
{

}

void CMacroProtectContents::MakeMainActorData(void)
{
	_ResetData();
}

void CMacroProtectContents::ClearMainActorData(void)
{
	_ResetData();
}

void CMacroProtectContents::MakeWorldData(void)
{

}

void CMacroProtectContents::ClearWorldData(void)
{

}

// Inspeções regionais que não podem usar macro proteger
bool CMacroProtectContents::_IsUsingCurrentMap()
{
	SAFE_POINTER_RETVAL( nsCsGBTerrain::g_pCurRoot, false );
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pBaseMng, false );

	CsGBTerrainRoot::sINFO* pMapInfo = nsCsGBTerrain::g_pCurRoot->GetInfo();
	SAFE_POINTER_RETVAL( pMapInfo, false );

	return nsCsFileTable::g_pBaseMng->IsEnableCheckMacro( pMapInfo->s_dwMapID );
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Ao usar itens no inventário
//////////////////////////////////////////////////////////////////////////
void CMacroProtectContents::_UsingMacroProtect(void* pData)
{
	SAFE_POINTER_RET( g_pCharResMng );

	// Verifique se há alvos de destino
	CsC_AvObject* pTarget = g_pCharResMng->GetTargetMark();
	SAFE_POINTER_RET( pTarget );

	_Show_AskWindow( pTarget );
}

//////////////////////////////////////////////////////////////////////////
// Quando usado na janela de destino
//////////////////////////////////////////////////////////////////////////
void CMacroProtectContents::_UsingMacroProtect_UIDX(void* pData)
{
	SAFE_POINTER_RET( pData );
	SAFE_POINTER_RET( g_pMngCollector );

	unsigned int nTargetUIDX = *static_cast<unsigned int*>(pData);
	CsC_AvObject* pTamer = g_pMngCollector->GetObject( nTargetUIDX );
	_Show_AskWindow( pTamer );
}

//////////////////////////////////////////////////////////////////////////
// Uma janela que pede mais uma vez ao usar um item de proteção de macro.
//////////////////////////////////////////////////////////////////////////
void CMacroProtectContents::_Show_AskWindow( CsC_AvObject* pTarget )
{
	if( !_IsUsingMacroProtect(pTarget) )
		return;

	cPrintMsg::PrintMsg( 30326, pTarget->GetName() );
	cMessageBox* pMsgBox = cMessageBox::GetMessageBox( 30326 );
	if( pMsgBox )
		pMsgBox->SetUintValue1( pTarget->GetUniqID() );

	// Pare
	g_pCharMng->GetTamerUser()->ReleaseArriveTarget();
	g_pCharMng->GetTamerUser()->DeletePath();
	g_pCharMng->GetDigimonUser( 0 )->DeletePath();
}

// Verifique se a macro proteger pode ser usada
bool CMacroProtectContents::_IsUsingMacroProtect( CsC_AvObject* pTarget )
{
	// Verifique se há alvos de destino
	if( pTarget == NULL )
	{
		cPrintMsg::PrintMsg( 16001 );
		return false;
	}

	// Verifique se o destino do alvo é mais domínio
	if( pTarget->GetLeafRTTI() != RTTI_TAMER )
	{
		cPrintMsg::PrintMsg( 11101 );
		return false;
	}

	// Inspeções regionais que não podem usar macro proteger
	if( !_IsUsingCurrentMap() )
	{
		cPrintMsg::PrintMsg( 30336 );
		return false;
	}


	// Verificações de inventário para itens de proteção de macro
	SAFE_POINTER_RETVAL( g_pDataMng, false );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RETVAL( pInven, false );

	int nInvenIndex = pInven->GetFirstSlot_Item_TypeLS( ITEM_TYPELS_MACRO_CHECK );	
	if( nInvenIndex == cData_Inven::INVALIDE_INVEN_INDEX )
	{
		cPrintMsg::PrintMsg( 11040, ITEM_STR_MACRO_CHECK );
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
// Envie um item macro no servidor
//////////////////////////////////////////////////////////////////////////
void CMacroProtectContents::_Send_MacroProtect_Using(void* pData)
{
	SAFE_POINTER_RET( pData );
	unsigned int targetUIDX = *static_cast<unsigned int*>(pData);
	CsC_AvObject* pTarget = g_pMngCollector->GetObject( targetUIDX );
	if( _IsUsingMacroProtect(pTarget) )
	{
		std::wstring name = pTarget->GetName();
		net::game->SendMacroQuestion( name );
	}
}

//////////////////////////////////////////////////////////////////////////
// Se outro usuário me suspeita de uma macro.
//////////////////////////////////////////////////////////////////////////
void CMacroProtectContents::_NTF_MacroProtect_Question_Request(void* pData)
{
	SAFE_POINTER_RET( pData );
	GS2C_NTF_MACRO_QUESTION_REQUEST * pRecv = static_cast<GS2C_NTF_MACRO_QUESTION_REQUEST*>(pData);

#ifdef SDM_MACRO_PROTECT_RENEW_20181217
	m_nMaxTimeSec = pRecv->m_nTimeMax;
	m_nRemainTimeSec = pRecv->m_nTimeLeft;
	m_nRemainChanceCount = pRecv->m_nChanceLeft;

	ZeroMemory( &m_pQuestionData, CAPTCHA_BMP_SIZE);
	cCaptchaConvert::ConvertToBmp( pRecv->m_pData, m_pQuestionData );

	if( (MACROPROTECT_MAX_CHANCE_COUNT - 1) == m_nRemainChanceCount )
		cPrintMsg::PrintMsg( 30341 );
	else
		cPrintMsg::PrintMsg( 30343, &m_nRemainChanceCount);


	#ifdef SDM_MACRO_PROTECT_QUESTION_20190524
		m_nQuestionType = pRecv->m_nQuestionType;
		m_nQuestionNumbers.clear();
		for( int n = 0; n < CAPTCHA_QUESTION_LENGTH;++n )
		{
			if( 0 == pRecv->m_nQuestion[n] )
				break;
			m_nQuestionNumbers.push_back( pRecv->m_nQuestion[n] );
		}
	#endif
#else
	m_nMacroAnswer = pRecv->m_nHint - pRecv->m_nRandNum;	
	m_nTryCount = 0;
#endif

	NotifyContents( CONTENTS_EVENT::EEvt_MacroProtect_Start );	
	_ShowMacroProtectWindow();
}

void CMacroProtectContents::_ShowMacroProtectWindow()
{
	if( !g_pGameIF->IsActiveWindow( cBaseWindow::WT_PROTECT ) )
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_PROTECT );

#ifdef SDM_MACRO_PROTECT_RENEW_20181217
	ContentsStream kTemp;
	kTemp << m_nMaxTimeSec << m_nRemainTimeSec << m_nRemainChanceCount << m_nQuestionType;
	Notify( eResetAnswer, kTemp );
#else
	srand( (unsigned)time(NULL));
	m_nRandomAnswer	= rand() % 64;
	ContentsStream kTemp;
	kTemp << m_nRandomAnswer;
	Notify( eResetAnswer, kTemp );
#endif
}

//////////////////////////////////////////////////////////////////////////
// O resultado do pedido macro suspeito
//////////////////////////////////////////////////////////////////////////
void CMacroProtectContents::_Recv_MacroProtect_Question_Result(void* pData)
{
	SAFE_POINTER_RET( pData );
	C2GS_RECV_MACRO_QUESTION_RESULT * recv = static_cast<C2GS_RECV_MACRO_QUESTION_RESULT*>(pData);
	switch( recv->m_nResult )
	{
	case 0 :	
		{
			SAFE_POINTER_RET( g_pDataMng );
			cData_Inven* pInVen = g_pDataMng->GetInven();
			SAFE_POINTER_RET( pInVen );
			pInVen->DecreaseItem_TypeLS( ITEM_TYPELS_MACRO_CHECK, 1, true, false );		// Remover itens
			cPrintMsg::PrintMsg( 30339 );
		}break;	// Bloco Macro Usuário.
	case 1 :	cPrintMsg::PrintMsg( 30327 );		break;// Os usuários -alvo não estão acessando o mapa no momento.
	case 2 :	cPrintMsg::PrintMsg( 30328 );		break;// O usuário -alvo é gratuito.
	case 3 :	cPrintMsg::PrintMsg( 30329 );		break;// Já verificado por alguém.
	case 4 :	cPrintMsg::PrintMsg( 30331 );		break;// O tempo de refrigeração ainda permanece.
	case 6 :	cPrintMsg::PrintMsg( 30337 );		break;// O usuário -alvo está esvaziando.
	case 5 :	cPrintMsg::PrintMsg( 11040, ITEM_STR_MACRO_CHECK );		break;// Falta de itens de verificação macro
	case 7 :	cPrintMsg::PrintMsg( 30347 );		break;// A fadiga não está satisfeita com a condição, portanto você não pode usar o macro protetor.
	}
}

void CMacroProtectContents::_Recv_MacroPlayer_Result(void* pData)
{
	SAFE_POINTER_RET( pData );
	GS2C_RECV_MACRO_PLAYER_RESULT* pRecv = static_cast<GS2C_RECV_MACRO_PLAYER_RESULT*>(pData);

	if(pRecv->m_nResult)
		cPrintMsg::PrintMsg( 30333 );		// O assunto está errado
	else
		cPrintMsg::PrintMsg( 30332 );		// O alvo foi correspondido
}

#ifdef SDM_MACRO_PROTECT_RENEW_20181217
void CMacroProtectContents::Send_MacroAnswer( std::wstring const& wsAnswer )
{
	if( net::game )
		net::game->SendMacroAnswer( wsAnswer );
	_ResetData();
	_CloseWindow();
}

char const* CMacroProtectContents::GetQuestionImgData() const
{
	return m_pQuestionData;
}

#else
void CMacroProtectContents::Send_MacroAnswer( int const& nAnswerValue )
{
	bool bSuccess = (m_nRandomAnswer == nAnswerValue)? true : false;
	m_nTryCount++;

	if( !bSuccess && m_nTryCount < MAX_TRY_COUNT )
	{
		_ShowMacroProtectWindow();
		return;
	}

	int nRandNo = nBase::rand(9, 999);
	int nAnswer = nRandNo;
	if( bSuccess )
	{
		nAnswer += m_nMacroAnswer;
		cPrintMsg::PrintMsg( 30335 );
	}
	else
	{
		nAnswer += 19;
		cPrintMsg::PrintMsg( 30325 );
	}

	if( net::game )
		net::game->SendMacroAnswer( nRandNo, nAnswer );

	_ResetData();
	_CloseWindow();
}

//////////////////////////////////////////////////////////////////////////
// Uma função que obtém valores aleatórios tanto quanto o NigetCount, incluindo valores de nincludenum dentro do intervalo de Totalsize
//////////////////////////////////////////////////////////////////////////
void CMacroProtectContents::RandomShuffle( int TotalSize, int nIncludeNum, int nGetCount, std::vector<int>& getVector )
{
	std::vector<int> temp;
	temp.resize(TotalSize);
	for( int n = 0; n < TotalSize; ++n )
		temp[n] = n;

	std::random_shuffle( temp.begin(), temp.end() );

	int nFindPos = -1;
	for( int n = 0; n < temp.size(); ++n )
	{
		if( temp[n] == nIncludeNum )
		{
			nFindPos = n;
			break;
		}
	}

	int nStartPos = 0;
	if( nFindPos - nGetCount < 0 )// Se você está em frente ao NGETCount
		nStartPos = 0;
	else if( TotalSize - nFindPos < nGetCount ) // Se você está em frente ao NGETCount
		nStartPos = TotalSize - nGetCount;
	else
		nStartPos = nFindPos - rand() % nGetCount;

	for( int n = nStartPos; n < nStartPos + nGetCount; ++n )
		getVector.push_back( temp[n] );

	std::random_shuffle( getVector.begin(), getVector.end() );
}
#endif
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void CMacroProtectContents::_ResetData()
{
#ifdef SDM_MACRO_PROTECT_RENEW_20181217
	ZeroMemory( &m_pQuestionData, CAPTCHA_BMP_SIZE);
	m_nMaxTimeSec = 0;
	m_nRemainTimeSec = 0;
	m_nRemainChanceCount = 0;
	m_nQuestionType = nsMacroProtector::NONE;
	m_nQuestionNumbers.clear();
#else
	m_nMacroAnswer = 0;
	m_nRandomAnswer = 0;
	m_nTryCount = 0;
#endif
}

void CMacroProtectContents::_CloseWindow()
{
	if( g_pGameIF )
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_PROTECT );
}
// Verifique se a pergunta foi recebida como um macro protetor
bool CMacroProtectContents::IsMacroProtectUsing() const
{
#ifdef SDM_MACRO_PROTECT_RENEW_20181217
	if( m_nMaxTimeSec != 0 )
		return true;

	if( m_nRemainTimeSec != 0 )
		return true;

	if( m_nRemainChanceCount != 0 )
		return true;

	return false;
#else
	if( m_nMacroAnswer != 0 )
		return true;
	if( m_nRandomAnswer != 0 )
		return true;
	return false;
#endif
}

std::list<int> const& CMacroProtectContents::GetQuestionNumber() const
{
	return m_nQuestionNumbers;
}