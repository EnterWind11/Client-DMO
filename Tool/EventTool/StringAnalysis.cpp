#include "stdafx.h"
#include "StringAnalysis.h"


cStringAnalysis*	g_pStringAnalysis = NULL;

CSGBMEMPOOL_CPP( cStringAnalysis::sPARCING );

void cStringAnalysis::GlobalInit()
{
	assert_cs( g_pStringAnalysis == NULL );
	g_pStringAnalysis = NiNew cStringAnalysis;
	g_pStringAnalysis->Init();

	CSGBMEMPOOL_INIT( cStringAnalysis::sPARCING, 8 );
}

void cStringAnalysis::GlobalShotDown()
{
	if( g_pStringAnalysis != NULL )
		g_pStringAnalysis->Delete();
	SAFE_NIDELETE( g_pStringAnalysis );

	CSGBMEMPOOL_DELETE( cStringAnalysis::sPARCING );
}

void cStringAnalysis::Init()
{
	m_pString = NULL;
}

void cStringAnalysis::Delete()
{
	assert_cs( m_pString == NULL );
	assert_cs( m_listParcing.empty() == true );
}

//============================================================================================
//
//		파싱
//
//============================================================================================
void cStringAnalysis::_Parcing( TCHAR* str, int nStrLen, int& nIndex )
{
	int nOrgIndex = nIndex;
	nIndex += 2;

	// 종료어 인가
	if( str[ nIndex ] == L'/' )
	{
		// 종료어 완료를 할수 있다면
		if( nIndex + 2 < nStrLen )
		{
			nIndex += 1;
			nIndex += _ParcingTypeEnd( &str[ nIndex ] );
		}
		// 종료어 완료가 안된다면 리턴
		else
		{
			nIndex = nOrgIndex;			
		}
	}
	// 시동어 인가
	else
	{
		// 시동어 완료를 할수 있다면
		if( nIndex + 2 < nStrLen )
		{
			ePARCING_TYPE pt;
			// 파싱 스타트
			nIndex += _ParcingTypeStart( &str[ nIndex ], pt );
			// 파싱값 가져 올수 있다면
			if( _ParcingValueEnableCheck( nStrLen, nIndex, pt ) == true )
			{
				nIndex += _ParcingValue( &str[ nIndex ], pt );
			}
			else
			{
				// 가져 올수 없다면 원상복귀
				nIndex = nOrgIndex;
			}
		}
		// 종료어 완료가 안된다면 원상복귀
		else
		{
			nIndex = nOrgIndex;			
		}
	}
}

int cStringAnalysis::_ParcingTypeStart( TCHAR* str, ePARCING_TYPE& pt )
{
	if( _tcsncmp( str, L"tc:", 3 ) == 0 )
	{
		pt = PT_TEXT_COLOR;
		return 3;
	}
	else if( _tcsncmp( str, L"tb:", 3 ) == 0 )
	{
		pt = PT_TEXT_BOLD;
		return 3;
	}

	// 파싱 실패
	pt = PT_NONE;
	return -1;
}

bool cStringAnalysis::_ParcingValueEnableCheck( int nStrLen, int nIndex, ePARCING_TYPE& pt )
{
	switch( pt )
	{
	case PT_TEXT_COLOR:
		// 0 0 0 -> 3자에 > 까지 4자
		return ( nIndex + 4 < nStrLen );
	case PT_TEXT_BOLD:
		// 0 -> 1자에 > 까지 2자
		return ( nIndex + 2 < nStrLen );
	}

	return false;
}

int cStringAnalysis::_ParcingValue( TCHAR* str, ePARCING_TYPE& pt )
{
	switch( pt )
	{
	case PT_TEXT_COLOR:
		{			
			_MakeToken_Parcing( false );

			sPARCING* pParcing = sPARCING::NewInstance();
			pParcing->s_eType = pt;

			// 텍스트를 DWORD 형으로 변환
			TCHAR sz[ 3 ][ 2 ];
			for( int i=0; i<3; ++i ){
				sz[ i ][ 0 ] = str[ i ];
				sz[ i ][ 1 ] = NULL;
			}
			pParcing->s_Value = CS_XRGB(	(DWORD)(_ttoi( sz[ 0 ] )*255/9.0f),
											(DWORD)(_ttoi( sz[ 1 ] )*255/9.0f),
											(DWORD)(_ttoi( sz[ 2 ] )*255/9.0f) );
			_RemoveParcing( PT_TEXT_COLOR );
			m_listParcing.push_back( pParcing );
		}
		return 4;		// _ParcingValueEnableCheck 값
	case PT_TEXT_BOLD:
		{
			_MakeToken_Parcing( false );

			sPARCING* pParcing = sPARCING::NewInstance();
			pParcing->s_eType = pt;

			// 텍스트를 DWORD 형으로 변환
			TCHAR sz[ 2 ];
			sz[ 0 ] = str[ 0 ];
			sz[ 1 ] = NULL;
			pParcing->s_Value = _ttoi( sz );
			_RemoveParcing( PT_TEXT_BOLD );
			m_listParcing.push_back( pParcing );
		}
		return 2;		// _ParcingValueEnableCheck 값
	}
	return 0;
}

int cStringAnalysis::_ParcingTypeEnd( TCHAR* str )
{
	if( _tcsncmp( str, L"tc>", 3 ) == 0 )
	{		
		_RemoveParcing( PT_TEXT_COLOR );
		return 3;
	}
	else if( _tcsncmp( str, L"tb>", 3 ) == 0 )
	{
		_RemoveParcing( PT_TEXT_BOLD );
		return 3;
	}

	// 파싱 실패
	return -1;
}

void cStringAnalysis::_RemoveParcing( ePARCING_TYPE pt )
{
	_MakeToken_Parcing( false );

	std::list< sPARCING* >::iterator it = m_listParcing.begin();
	std::list< sPARCING* >::iterator itEnd = m_listParcing.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it)->s_eType == pt )
		{
			switch( pt )
			{
			case PT_TEXT_COLOR:
				m_pTextInfo->s_Color = m_OrgColor;
				break;
			case PT_TEXT_BOLD:			
				m_pTextInfo->SetBoldLevel( (cText::sTEXTINFO::eBOLD_LEVEL)m_nOrgBoldLevel );
				break;
			}

			sPARCING::DeleteInstance( *it );
			m_listParcing.erase( it );
			return;
		}
	}
}

//============================================================================================
//
//		색깔 구분 해서 문자열 자르기
//
//============================================================================================
void cStringAnalysis::_MakeToken_Parcing( bool bNewString )
{
	if( m_szToken[ 0 ] == NULL )
		return;

	std::list< sPARCING* >::iterator it = m_listParcing.begin();
	std::list< sPARCING* >::iterator itEnd = m_listParcing.end();
	for( ; it!=itEnd; ++it )
	{
		switch( (*it)->s_eType )
		{
		case PT_TEXT_COLOR:
			m_pTextInfo->s_Color = CsCOLOR2NiColor( (*it)->s_Value );
			break;
		case PT_TEXT_BOLD:			
			m_pTextInfo->SetBoldLevel( (cText::sTEXTINFO::eBOLD_LEVEL)(*it)->s_Value );
			break;
		}
	}

	// 현재 토큰 생성 하자
	m_pTextInfo->SetText( m_szToken );
	m_pString->AddText( m_pTextInfo );

	if( bNewString == true )
	{
		// 문자열이 넘어 갔으므로 스트링리스트에 담아주자
		m_pStringList->AddTail( m_pString );
		// 새로운 스트링 포인터
		m_pString = NiNew cString;
		m_nTotalWidth = 0;
	}

	// 초기화
	memset( m_szToken, 0, sizeof( TCHAR )*MAX_ANAL_LEN );
	m_nTokenIndex = 0;
}


void cStringAnalysis::_MakeToken( bool bNewString )
{
	if( m_szToken[ 0 ] == NULL )
		return;

	// 현재 토큰 생성 하자
	m_pTextInfo->SetText( m_szToken );
	m_pString->AddText( m_pTextInfo );

	if( bNewString == true )
	{
		// 문자열이 넘어 갔으므로 스트링리스트에 담아주자
		m_pStringList->AddTail( m_pString );
		// 새로운 스트링 포인터
		m_pString = NiNew cString;
		m_nTotalWidth = 0;
	}

	// 초기화
	memset( m_szToken, 0, sizeof( TCHAR )*MAX_ANAL_LEN );
	m_nTokenIndex = 0;
}

void cStringAnalysis::Cut_Parcing( cStringList* pStringList, int width, TCHAR* str, cText::sTEXTINFO* pTextInfo )
{
	if( pTextInfo->s_pFont->IsInitialize() == false )
	{
		return;
	}
	if( str[ 0 ] == NULL )
	{
		return;
	}

	assert_cs( m_pString == NULL );
	assert_cs( m_listParcing.empty() == true );

	m_pTextInfo = pTextInfo;
	m_OrgColor = m_pTextInfo->s_Color;
	m_nOrgBoldLevel = (int)m_pTextInfo->GetBoldLevel();
	
	m_pString = NiNew cString;
	m_pStringList = pStringList;
	m_nTotalWidth = 0;

	memset( m_szToken, 0, sizeof( TCHAR )*MAX_ANAL_LEN );
	m_nTokenIndex = 0;
	

	// 총 길이
	int nStrLen = (int)_tcslen( str );
	assert_cs( nStrLen > 0 );

	// 프리타입폰트
	FT_Face face = m_pTextInfo->s_pFont->GetFace( m_pTextInfo->s_nFontSize );

	int nCharSize;
	for( int i=0; i<nStrLen; ++i )
	{
		//=================================================================================================
		//	뉴라인
		//=================================================================================================
		if( str[ i ] == L'\n' )
		{
			_MakeToken_Parcing( true );
			continue;
		}

		//=================================================================================================
		//	파싱
		//=================================================================================================
		// 파싱하는데 최소 5개의 문자가 소요
		// 파싱 시작 - @<시동어:값>
		// 피상 종료 - @</종료어>
		if( i < nStrLen - 5 )
		{
PARCING:
			// 파싱해야 하는 문자인가
			if( ( str[ i ] == L'@' )&&( str[ i + 1 ] == L'<' ) )
			{
				_Parcing( str, nStrLen, i );
				if( i == nStrLen )
					break;
			}
			if( ( str[ i ] == L'@' )&&( str[ i + 1 ] == L'<' ) )
			{
				goto PARCING;
			}
		}

		// 문자의 길이 가져온다.
		nCharSize = cText::GetCharWidth( face, str[ i ] );		

		// 문자열 길이가 넘어 갔는가
		if( ( m_nTotalWidth + nCharSize ) > width )
		{
			_MakeToken_Parcing( true );
		}

		if( ( m_nTokenIndex != 0 )||( str[ i ] != L' ' ) )
		{
			m_szToken[ m_nTokenIndex ] = str[ i ];
			++m_nTokenIndex;
			m_nTotalWidth += nCharSize;
		}
	}

	if( m_nTokenIndex != 0 )
	{
		_MakeToken_Parcing( false );		
	}
	if( m_pString->GetListSize() == 0 )
	{
		SAFE_NIDELETE( m_pString );
	}
	if( m_pString != NULL )
	{
		m_pStringList->AddTail( m_pString );
	}

	// 포인터 초기화
	m_pString = NULL;

	// 파싱 종료
	if( m_listParcing.empty() == false )
	{
		std::list< sPARCING* >::iterator it = m_listParcing.begin();
		std::list< sPARCING* >::iterator itEnd = m_listParcing.end();
		for( ; it!=itEnd; ++it )
		{
			sPARCING::DeleteInstance( *it );
		}
		m_listParcing.clear();
	}	
}

void cStringAnalysis::Cut( cStringList* pStringList, int width, TCHAR* str, cText::sTEXTINFO* pTextInfo )
{
	if( pTextInfo->s_pFont->IsInitialize() == false )
	{
		return;
	}
	if( str[ 0 ] == NULL )
	{
		return;
	}

	assert_cs( m_pString == NULL );
	assert_cs( m_listParcing.empty() == true );

	m_pTextInfo = pTextInfo;
	m_OrgColor = m_pTextInfo->s_Color;

	m_pString = NiNew cString;
	m_pStringList = pStringList;
	m_nTotalWidth = 0;

	memset( m_szToken, 0, sizeof( TCHAR )*MAX_ANAL_LEN );
	m_nTokenIndex = 0;


	// 총 길이
	int nStrLen = (int)_tcslen( str );
	assert_cs( nStrLen > 0 );

	// 프리타입폰트
	FT_Face face = m_pTextInfo->s_pFont->GetFace( m_pTextInfo->s_nFontSize );

	int nCharSize;
	for( int i=0; i<nStrLen; ++i )
	{
		//=================================================================================================
		//	뉴라인
		//=================================================================================================
		if( str[ i ] == L'\n' )
		{
			_MakeToken( true );
			continue;
		}

		// 문자의 길이 가져온다.
		nCharSize = cText::GetCharWidth( face, str[ i ] );		

		// 문자열 길이가 넘어 갔는가
		if( ( m_nTotalWidth + nCharSize ) > width )
		{
			_MakeToken( true );
		}	

		if( ( m_nTokenIndex != 0 )||( str[ i ] != L' ' ) )
		{
			m_szToken[ m_nTokenIndex ] = str[ i ];
			++m_nTokenIndex;
			m_nTotalWidth += nCharSize;
		}
	}

	if( m_nTokenIndex != 0 )
	{
		_MakeToken( false );		
	}
	if( m_pString->GetListSize() == 0 )
	{
		SAFE_NIDELETE( m_pString );
	}
	if( m_pString != NULL )
	{
		m_pStringList->AddTail( m_pString );
	}

	// 포인터 초기화
	m_pString = NULL;	
}

/*
TCHAR* cStringAnalysis::StringTransfer( TCHAR* szOut, int nOutSize, TCHAR* szIn )
{
	int nLen = (int)_tcslen( szIn );
	int nDestIndex = 0;
	memset( szOut, 0, sizeof( TCHAR )*nOutSize );

	for( int i=0; i<nLen; ++i )
	{
		if( i < (int)( nLen - 3 ) )
		{
			if( ( szIn[ i ] == L'#' )&&( szIn[ i + 1 ] == L'<' ) )
			{
				TCHAR* szInsert = NULL;
				int nJump = 0;
				if( _tcsncmp( &szIn[ i + 2 ], L"my_tname>", 9 ) == 0 )
				{
					szInsert = g_pCharMng->GetTamerUser()->GetName();
					nJump = 9;
				}
				else if( _tcsncmp( &szIn[ i + 2 ], L"my_dname>", 9 ) == 0 )
				{
					szInsert = g_pCharMng->GetDigimonUser( 0 )->GetName();
					nJump = 9;
				}

				if( szInsert )
				{
					if( ( nDestIndex + (int)_tcslen( szInsert ) ) < ( nOutSize - 1 ) )
					{
						szOut[ nDestIndex ] = NULL;
						_tcscat_s( szOut, nOutSize, szInsert );
						nDestIndex += (int)_tcslen( szInsert );
						i+= ( nJump + 1 );
						continue;
					}
					else
					{
						break;
					}					
				}				
			}
		}
		szOut[ nDestIndex ] = szIn[ i ];
		++nDestIndex;

		if( nDestIndex == ( nOutSize - 1 ) )
			break;
	}
	szOut[ nDestIndex ] = NULL;
	return szOut;
}

TCHAR* cStringAnalysis::Quest_Parcing( TCHAR* szDest, const int nDestSize, TCHAR* szSrc, CsQuestCompRequireGroup* pApplyGroup )
{
#define QUEST_PARCING_LEN		32
	int nSrcLen = _tcslen( szSrc );
	assert_cs( nDestSize > nSrcLen );

	int nDestIndex = 0;
	int nSrcIndex = 0;
	TCHAR szTemp[ QUEST_PARCING_LEN ];	
	while( nSrcIndex < nSrcLen )
	{
		if( szSrc[ nSrcIndex ] == L'%' )
		{
			++nSrcIndex;
			if( _tcsncmp( &szSrc[ nSrcIndex ], L"cc", 2 ) == 0 )
			{
				nSrcIndex += 2;
				// 그룹 인덱스 - 오직 하나의 숫자만
				szTemp[ 0 ] = szSrc[ nSrcIndex ];
				++nSrcIndex;
				szTemp[ 1 ] = NULL;
				int nCompRequireIndex = _ttoi( szTemp );

				assert_csm1( nCompRequireIndex != 0, L"명령어 인자가 잘못 되었습니다. 명령어 %s", szSrc );

				CsQuestCompRequire* pFTComp = pApplyGroup->Get( nCompRequireIndex - 1 );
				int nR = 9 - (int)( (float)pFTComp->GetCurCount() / (float)pFTComp->GetTypeCount() * 9.0f );
				assert_cs( nR<10 );

				_stprintf_s( szTemp, QUEST_PARCING_LEN, L"@<tc:%d90>%d", nR, pApplyGroup->Get( nCompRequireIndex-1 )->GetCurCount() );
				_tcscpy_s( &szDest[ nDestIndex ], nDestSize - nDestIndex - 1, szTemp );
				nDestIndex += (int)_tcslen( szTemp );
			}
			else if( _tcsncmp( &szSrc[ nSrcIndex ], L"mc", 2 ) == 0 )
			{
				nSrcIndex += 2;
				// 그룹 인덱스 - 오직 하나의 숫자만
				szTemp[ 0 ] = szSrc[ nSrcIndex ];
				++nSrcIndex;
				szTemp[ 1 ] = NULL;
				int nCompRequireIndex = _ttoi( szTemp );

				assert_csm1( nCompRequireIndex != 0, L"명령어 인자가 잘못 되었습니다. 명령어 %s", szSrc );
				_stprintf_s( szTemp, QUEST_PARCING_LEN, L"%d@</tc>", pApplyGroup->Get( nCompRequireIndex-1 )->GetTypeCount() );
				_tcscpy_s( &szDest[ nDestIndex ], nDestSize - nDestIndex - 1, szTemp );
				nDestIndex += (int)_tcslen( szTemp );
			}
			else if( _tcsncmp( &szSrc[ nSrcIndex ], L"tcc", 3 ) == 0 )
			{
				nSrcIndex += 3;

				int nCompCount = 0;
				int nCount = pApplyGroup->GetListCount();
				for( int i=0; i<nCount; ++i )
				{
					if( pApplyGroup->Get( i )->IsCompleat() == true )
						++nCompCount;
				}

				int nR = 9 - (int)( (float)nCompCount / (float)pApplyGroup->GetListCount() * 9.0f );
				_stprintf_s( szTemp, QUEST_PARCING_LEN, L"@<tc:%d90>%d", nR, nCompCount );				
				_tcscpy_s( &szDest[ nDestIndex ], nDestSize - nDestIndex - 1, szTemp );
				nDestIndex += (int)_tcslen( szTemp );
			}
			else if( _tcsncmp( &szSrc[ nSrcIndex ], L"tmc", 3 ) == 0 )
			{
				nSrcIndex += 3;

				_stprintf_s( szTemp, QUEST_PARCING_LEN, L"%d@</tc>", pApplyGroup->GetListCount() );
				_tcscpy_s( &szDest[ nDestIndex ], nDestSize - nDestIndex - 1, szTemp );
				nDestIndex += (int)_tcslen( szTemp );
			}
			else if( _tcsncmp( &szSrc[ nSrcIndex ], L"qs", 2 ) == 0 )
			{
				nSrcIndex += 3;

				
				nSrcIndex += 2;

				// 그룹 인덱스 - 오직 하나의 숫자만
				szTemp[ 0 ] = szSrc[ nSrcIndex ];
				++nSrcIndex;
				szTemp[ 1 ] = NULL;
				int nCompRequireIndex = _ttoi( szTemp );

				assert_csm1( nCompRequireIndex > 0, L"명령어 인자가 잘못 되었습니다. 명령어 %s", szSrc );
				assert_csm2( pApplyGroup->Get( nCompRequireIndex - 1 ), L"qs%d 그룹이 존재 하지 않습니다. \n%s", nCompRequireIndex, szSrc );
				if( pApplyGroup->Get( nCompRequireIndex - 1 )->IsCompleat() == true )
				{
					assert_csm1( nCompRequireIndex != 0, L"명령어 인자가 잘못 되었습니다. 명령어 %s", szSrc );
					_tcscpy_s( szTemp, QUEST_PARCING_LEN, L"@<tc:090>" );
					_tcscpy_s( &szDest[ nDestIndex ], nDestSize - nDestIndex - 1, szTemp );
					nDestIndex += (int)_tcslen( szTemp );
				}
			}
			else if( _tcsncmp( &szSrc[ nSrcIndex ], L"qe", 2 ) == 0 )
			{
				nSrcIndex += 2;
				_tcscpy_s( szTemp, QUEST_PARCING_LEN, L"@</tc>" );
				_tcscpy_s( &szDest[ nDestIndex ], nDestSize - nDestIndex - 1, szTemp );
				nDestIndex += (int)_tcslen( szTemp );
			}
			else
			{
				assert_csm1( false, L"알 수 없는 명령어 입니다. 명령어 %s", szSrc );
			}
		}
		else
		{
			szDest[ nDestIndex ] = szSrc[ nSrcIndex ];
			++nDestIndex;
			++nSrcIndex;
		}		
	}
	szDest[ nDestIndex ] = NULL;
	return szDest;
}

void cStringAnalysis::Equip_EItemComment_Parcing( cStringList* pList, cString* pBeginString, cItemInfo* pEquip, int nSocketIndex )
{
#define EC_PARCING_LEN		32
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, NiColor( 0, 1, 0 ) );

	assert_cs( pEquip->m_nSockItemType[ nSocketIndex ] != 0 );
	CsItem::sINFO* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pEquip->m_nSockItemType[ nSocketIndex ] )->GetInfo();
	CsSkill::sINFO* pFTSkill = nsCsFileTable::g_pSkillMng->GetSkill( pFTItem->s_dwSkill )->GetInfo();
	cString* pString;
	int nTabSize = 0;	
	if( pBeginString != NULL )
	{
		pBeginString->CalMaxSize();
		nTabSize = pBeginString->GetMaxSize().x;
	}

	TCHAR szTemp[ EC_PARCING_LEN ];
	TCHAR szInt[ 16 ];
	for( int i=0; i<SKILL_APPLY_MAX_COUNT; ++i )
	{
		if( pFTSkill->s_Apply[ i ].s_nID == 0 )
			continue;

		if( pBeginString != NULL )
		{
			pString = pBeginString;
			pBeginString = NULL;
		}
		else
		{
			pString = NiNew cString;
			if( nTabSize != 0 )
			{
				ti.SetText( L"" );
				pString->AddText( &ti )->s_ptSize.x = nTabSize;
			}
		}

		switch( pFTSkill->s_Apply[ i ].s_nA )
		{
		case 1:		_stprintf_s( szTemp, EC_PARCING_LEN, L"생명력 " );			break;
		case 2:		_stprintf_s( szTemp, EC_PARCING_LEN, L"디지소울 " );		break;
		case 3:		_stprintf_s( szTemp, EC_PARCING_LEN, L"최대생명력 " );		break;
		case 4:		_stprintf_s( szTemp, EC_PARCING_LEN, L"최대디지소울 " );	break;
		case 5:		_stprintf_s( szTemp, EC_PARCING_LEN, L"공격력 " );			break;
		case 6:		_stprintf_s( szTemp, EC_PARCING_LEN, L"치명타 " );			break;
		case 7:		_stprintf_s( szTemp, EC_PARCING_LEN, L"방어력 " );			break;
		case 8:		_stprintf_s( szTemp, EC_PARCING_LEN, L"회피력 " );			break;
		case 9:		_stprintf_s( szTemp, EC_PARCING_LEN, L"이동속도 " );		break;
		case 10:	_stprintf_s( szTemp, EC_PARCING_LEN, L"공격속도 " );		break;
		case 11:	_stprintf_s( szTemp, EC_PARCING_LEN, L"공격사거리 " );		break;
		case 20:	_stprintf_s( szTemp, EC_PARCING_LEN, L"모든능력치 " );		break;
		default:	assert_cs( false );
		}

		float fPV = pFTSkill->s_Apply[ i ].s_nB * pEquip->m_nSockAppRate[ nSocketIndex ] * 0.01f;
		bool bMinus = ( fPV < 0.0f );
		int nValue = CsFloat2Int( abs( fPV ) );
		if( bMinus )
			nValue = -nValue;

		nValue = CsFloat2Int( nValue*pEquip->m_nRate*0.01f );
		
		_stprintf_s( szInt, 16, L"%d", nValue );
		_tcscat_s( szTemp, EC_PARCING_LEN, szInt );

		switch( pFTSkill->s_Apply[ i ].s_nID )
		{
		case 101:
			{
				if( bMinus == false )
					_tcscat_s( szTemp, EC_PARCING_LEN, L" 상승" );
				else
					_tcscat_s( szTemp, EC_PARCING_LEN, L" 감소" );
			}
			break;
		case 102:
			{
				if( bMinus == false )
					_tcscat_s( szTemp, EC_PARCING_LEN, L"% 증가" );
				else
					_tcscat_s( szTemp, EC_PARCING_LEN, L"% 감소" );
			}
			break;
		default:		assert_cs( false );
		}

		ti.SetText( szTemp );
		pString->AddText( &ti );
		pList->AddTail( pString );
	}
}

void cStringAnalysis::EItemComment_Parcing( cStringList* pList, cItemInfo* pEItem )
{
	if( pEItem->m_nRate == 0 )
	{
		return EItemComment_Parcing( pList, pEItem->GetType() );
	}

#define EC_PARCING_LEN		32
	assert_cs( pEItem->m_nRate > 0 );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
	CsSkill::sINFO* pFTSkill = nsCsFileTable::g_pSkillMng->GetSkill( nsCsFileTable::g_pItemMng->GetItem( pEItem->GetType() )->GetInfo()->s_dwSkill )->GetInfo();
	cString* pString;

	TCHAR szTemp[ EC_PARCING_LEN ];
	TCHAR szInt[ 16 ];
	for( int i=0; i<SKILL_APPLY_MAX_COUNT; ++i )
	{
		if( pFTSkill->s_Apply[ i ].s_nID == 0 )
			continue;

		pString = NiNew cString;
		switch( pFTSkill->s_Apply[ i ].s_nA )
		{
		case 1:		_stprintf_s( szTemp, EC_PARCING_LEN, L"생명력 " );			break;
		case 2:		_stprintf_s( szTemp, EC_PARCING_LEN, L"디지소울 " );		break;
		case 3:		_stprintf_s( szTemp, EC_PARCING_LEN, L"최대생명력 " );		break;
		case 4:		_stprintf_s( szTemp, EC_PARCING_LEN, L"최대디지소울 " );	break;
		case 5:		_stprintf_s( szTemp, EC_PARCING_LEN, L"공격력 " );			break;
		case 6:		_stprintf_s( szTemp, EC_PARCING_LEN, L"치명타 " );			break;
		case 7:		_stprintf_s( szTemp, EC_PARCING_LEN, L"방어력 " );			break;
		case 8:		_stprintf_s( szTemp, EC_PARCING_LEN, L"회피력 " );			break;
		case 9:		_stprintf_s( szTemp, EC_PARCING_LEN, L"이동속도 " );		break;
		case 10:	_stprintf_s( szTemp, EC_PARCING_LEN, L"공격속도 " );		break;
		case 11:	_stprintf_s( szTemp, EC_PARCING_LEN, L"공격사거리 " );		break;
		case 12:	_stprintf_s( szTemp, EC_PARCING_LEN, L"적중도 " );			break;
		case 13:	_stprintf_s( szTemp, EC_PARCING_LEN, L"피로도 " );			break;
		case 20:	_stprintf_s( szTemp, EC_PARCING_LEN, L"모든능력치 " );		break;
		default:	assert_cs( false );
		}

		float fPV = pFTSkill->s_Apply[ i ].s_nB * pEItem->m_nRate * 0.01f;
		bool bMinus = ( fPV < 0.0f );
		int nValue = CsFloat2Int( abs( fPV ) );
		if( bMinus )
			nValue = -nValue;

		_stprintf_s( szInt, 16, L"%d", nValue );
		_tcscat_s( szTemp, EC_PARCING_LEN, szInt );

		switch( pFTSkill->s_Apply[ i ].s_nID )
		{
		case 101:
			{
				if( bMinus == false )
					_tcscat_s( szTemp, EC_PARCING_LEN, L" 상승" );
				else
					_tcscat_s( szTemp, EC_PARCING_LEN, L" 감소" );
			}
			break;
		case 102:
			{
				if( bMinus == false )
					_tcscat_s( szTemp, EC_PARCING_LEN, L"% 증가" );
				else
					_tcscat_s( szTemp, EC_PARCING_LEN, L"% 감소" );
			}
			break;
		default:		assert_cs( false );
		}

		ti.SetText( szTemp );
		pString->AddText( &ti );
		pList->AddTail( pString );
	}
}


void cStringAnalysis::EItemComment_Parcing( cStringList* pList, int nEItemType )
{
#define EC_PARCING_LEN		32

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );

	CsItem::sINFO* pFTItem = nsCsFileTable::g_pItemMng->GetItem( nEItemType )->GetInfo();
	CsSkill::sINFO* pFTSkill = nsCsFileTable::g_pSkillMng->GetSkill( pFTItem->s_dwSkill )->GetInfo();
	cString* pString;

	TCHAR szTemp[ EC_PARCING_LEN ];
	TCHAR szInt[ 32 ];
	for( int i=0; i<SKILL_APPLY_MAX_COUNT; ++i )
	{
		if( pFTSkill->s_Apply[ i ].s_nID == 0 )
			continue;

		pString = NiNew cString;
		switch( pFTSkill->s_Apply[ i ].s_nA )
		{
		case 1:		_stprintf_s( szTemp, EC_PARCING_LEN, L"생명력 " );			break;
		case 2:		_stprintf_s( szTemp, EC_PARCING_LEN, L"디지소울 " );		break;
		case 3:		_stprintf_s( szTemp, EC_PARCING_LEN, L"최대생명력 " );		break;
		case 4:		_stprintf_s( szTemp, EC_PARCING_LEN, L"최대디지소울 " );	break;
		case 5:		_stprintf_s( szTemp, EC_PARCING_LEN, L"공격력 " );			break;
		case 6:		_stprintf_s( szTemp, EC_PARCING_LEN, L"치명타 " );			break;
		case 7:		_stprintf_s( szTemp, EC_PARCING_LEN, L"방어력 " );			break;
		case 8:		_stprintf_s( szTemp, EC_PARCING_LEN, L"회피력 " );			break;
		case 9:		_stprintf_s( szTemp, EC_PARCING_LEN, L"이동속도 " );		break;
		case 10:	_stprintf_s( szTemp, EC_PARCING_LEN, L"공격속도 " );		break;
		case 11:	_stprintf_s( szTemp, EC_PARCING_LEN, L"공격사거리 " );		break;
		case 12:	_stprintf_s( szTemp, EC_PARCING_LEN, L"적중도 " );			break;
		case 13:	_stprintf_s( szTemp, EC_PARCING_LEN, L"피로도 " );			break;
		case 20:	_stprintf_s( szTemp, EC_PARCING_LEN, L"모든능력치 " );		break;
		default:	assert_cs( false );
		}

		bool bMinus = ( pFTSkill->s_Apply[ i ].s_nB < 0 );
		int nMin = CsFloat2Int( abs( pFTSkill->s_Apply[ i ].s_nB * pFTItem->s_btApplyRateMin * 0.01f ) );
		int nMax = CsFloat2Int( abs( pFTSkill->s_Apply[ i ].s_nB * pFTItem->s_btApplyRateMax * 0.01f ) );
		if( bMinus )
		{
			nMin = -nMin;
			nMax = -nMax;
		}

		// 둘다 값이 0이라면 비율은 1로 적용
		if( ( nMin || nMax ) == false )
		{
			nMin = nMax = pFTSkill->s_Apply[ i ].s_nB;
		}

		if( nMin != nMax )
			_stprintf_s( szInt, 32, L"%d~%d", nMin, nMax );
		else
			_stprintf_s( szInt, 32, L"%d", nMin );

		_tcscat_s( szTemp, EC_PARCING_LEN, szInt );

		switch( pFTSkill->s_Apply[ i ].s_nID )
		{
		case 101:
			{
				if( bMinus == false )
					_tcscat_s( szTemp, EC_PARCING_LEN, L" 상승" );
				else
					_tcscat_s( szTemp, EC_PARCING_LEN, L" 감소" );
			}
			break;
		case 102:
			{
				if( bMinus == false )
					_tcscat_s( szTemp, EC_PARCING_LEN, L"% 증가" );
				else
					_tcscat_s( szTemp, EC_PARCING_LEN, L"% 감소" );
			}
			break;
		default:		assert_cs( false );
		}
		ti.SetText( szTemp );
		pString->AddText( &ti );
		pList->AddTail( pString );
	}
}
*/

TCHAR* cStringAnalysis::FindToken( TCHAR* szMsg )
{
	size_t len = _tcslen( szMsg );
	if( len == 0 )
		return szMsg;

	for( uint i=0; i<len; ++i )
	{
		if( _tcsncmp( &szMsg[ i ], L"\\n", 2 ) == 0 )
		{
			szMsg[ i ] = L'\n';
			for( size_t n=i+1; n<len-1; ++n )
			{
				szMsg[ n ] = szMsg[ n + 1 ];
			}
			szMsg[ len-1 ] = NULL;
			--len;
		}		
	}
	return szMsg;
}