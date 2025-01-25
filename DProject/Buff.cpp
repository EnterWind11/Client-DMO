
#include "stdafx.h"
#include "Buff.h"

cBuffData::cBuffData()
{
}

void cBuffData::Init(CsC_AvObject* pObject)
{
	m_pObject = pObject;
	m_dwUnusualCond = UNUSUAL_NONE;
	m_bIsOnePlayEffect = false;
}

void cBuffData::Delete()
{
	m_ListBuff.clear();
	m_ListDeBuff.clear();		
	m_ListSystemBuff.clear();
	m_ListEncyBuff.clear();
	m_dwUnusualCond = UNUSUAL_NONE;
}

void cBuffData::SetBuff( USHORT nBuffID, UINT nID, UINT nLifeTime, int nLV, u4 dwSkillCode )
{		
	CsBuff* pBuff = nsCsFileTable::g_pBuffMng->GetBuff( nBuffID );	

	if( !pBuff )
		return;
	
	sBUFF_DATA pInfo;
	pInfo.s_pBuffTable = pBuff;
	pInfo.s_nTargetUID = nID;
	pInfo.s_nBuffCurLV = nLV;	
	pInfo.s_nEndTime = (nLifeTime == UINT_MAX) ? UINT_MAX : nLifeTime;		
	pInfo.s_nLifeTime = (nLifeTime == UINT_MAX) ? UINT_MAX : nLifeTime - _TIME_TS;
	pInfo.s_dwSkillCode = dwSkillCode;
	pInfo.s_nTimeIverse = pInfo.s_nLifeTime;
#ifdef MONSTER_SKILL_GROWTH
	pInfo.m_nStack = 1;	//	버프 중첩수, 기본으로 1 잡아두자
#endif

	std::list< sBUFF_DATA >::iterator it;
	std::list< sBUFF_DATA >::iterator itEnd;
	bool bInsert = false;

	switch( pBuff->GetInfo()->s_nBuffType )
	{
	case 1:	//버프	
		{
			it = m_ListBuff.begin();
			itEnd = m_ListBuff.end();
				
			for( ; it != itEnd; it++ )
			{
				if( pInfo.s_nEndTime < it->s_nEndTime )
				{
					m_ListBuff.insert( it, pInfo );
					bInsert = true;
					break;
				}
			}	

			if( bInsert == false )
				m_ListBuff.push_back( pInfo );
			
		}		
		break;
	case 2:	//디버프	
		{
			it = m_ListDeBuff.begin();
			itEnd = m_ListDeBuff.end();

			for( ; it != itEnd; it++ )
			{
				if( pInfo.s_nEndTime < it->s_nEndTime )
				{
					m_ListDeBuff.insert( it, pInfo );
					bInsert = true;
					break;
				}
			}	

			if( bInsert == false )
				m_ListDeBuff.push_back( pInfo );
		}
		break;
	case 3: //시스템 버프
		{
			it = m_ListSystemBuff.begin();
			itEnd = m_ListSystemBuff.end();

			for( ; it != itEnd; it++ )
			{
				if( pInfo.s_nEndTime < it->s_nEndTime )
				{
					m_ListSystemBuff.insert( it, pInfo );
					bInsert = true;
					break;
				}
			}	

			if( bInsert == false )
				m_ListSystemBuff.push_back( pInfo );
		}
		break;
	default:
		assert_csm( false, L"잘못된 버프 타입" );
		break;
	}	

	// 일반 버프류 이펙트 붙여준다
	if( pBuff->GetInfo()->s_szEffectFile[ 0 ] != NULL )
	{
		char cEffectPath[ MAX_PATH ] = {0, };
		sprintf_s( cEffectPath, MAX_PATH, "System\\Buff\\%s.nif", pBuff->GetInfo()->s_szEffectFile );
		m_pObject->GetProp_Effect()->AddEffect_FT( cEffectPath );		
	}	


	// 상태 이상류 버프 확인	
	_SetUnusalCond( _GetUnusalCondLv() );
	_SetBuffEffect(pBuff, nBuffID, nID);	// 루틴이 길어져서 함수로 처리
}

void cBuffData::ReleaseBuff( USHORT nBuffID )
{
	std::list< sBUFF_DATA >::iterator it;
	std::list< sBUFF_DATA >::iterator itEnd;

	DWORD dwTargetUID = 0;

	it = m_ListBuff.begin();	
	itEnd = m_ListBuff.end();	
	for( ; it != itEnd; it++ )
	{
		if( it->s_pBuffTable->GetInfo()->s_dwID == nBuffID )	
		{
			dwTargetUID = it->s_nTargetUID;
			m_ListBuff.erase( it );
			break;
		}
	}

	it = m_ListDeBuff.begin();	
	itEnd = m_ListDeBuff.end();
	for( ; it != itEnd; it++ )
	{		
		if( it->s_pBuffTable->GetInfo()->s_dwID == nBuffID )	
		{
			dwTargetUID = it->s_nTargetUID;
			m_ListDeBuff.erase( it );
			break;
		}
	}	

	it = m_ListSystemBuff.begin();	
	itEnd = m_ListSystemBuff.end();
	for( ; it != itEnd; it++ )
	{		
		if( it->s_pBuffTable->GetInfo()->s_dwID == nBuffID )	
		{
			m_ListSystemBuff.erase( it );
			break;
		}
	}

	// 상태 이상류 버프 확인	
	CTamer* tempTamer = (CTamer*)m_pObject;
	if( !tempTamer->IsRide() )
		_ReleaseUnusalCond( _GetUnusalCondLv() );

	// 루프 이펙트 삭제 추가

	if( dwTargetUID != 0 )
	{
		CsC_AvObject* pTarget = g_pMngCollector->GetObject( dwTargetUID );
		
		if(!pTarget)
		{
			assert_csm1(pTarget, "잘못된 디지몬 UID", dwTargetUID);	// 자꾸 걸리면 삭제 
			return;
		}

		if( pTarget->GetProp_Effect()->DeleteBuffLoopEffect(nBuffID) )
		{
			//std::wstring msg = GetVAString( _T("Delete LoopEffect BuffID : %d / TargetID : %d") , nBuffID, dwTargetUID );
			ST_CHAT_PROTOCOL	CProtocol;
			CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
			DmCS::StringFn::Format(CProtocol.m_wStr,  _T("Delete LoopEffect BuffID : %d / TargetID : %d") , nBuffID, dwTargetUID);
			//CProtocol.m_wStr = GetVAString( _T("Delete LoopEffect BuffID : %d / TargetID : %d") , nBuffID, dwTargetUID );
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
		}


		if(pTarget && g_pSEffectMgr->DeleteSEffect(pTarget, nBuffID) )
		{
			ST_CHAT_PROTOCOL	CProtocol;
			CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
			DmCS::StringFn::Format(CProtocol.m_wStr, _T("Delete SEffect Shading TargetID : %d"), dwTargetUID);
			//CProtocol.m_wStr = GetVAString( _T("Delete SEffect Shading TargetID : %d"), dwTargetUID );
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
		}
	}
}

cBuffData::sBUFF_DATA* cBuffData::GetBuffData( USHORT nID )
{
	LIST_BUFF_IT it;
	LIST_BUFF_IT itEnd;

	it = m_ListBuff.begin();	
	itEnd = m_ListBuff.end();	
	for( ; it != itEnd; it++ )
	{
		if( it->s_pBuffTable->GetInfo()->s_dwID == nID )	
			return &(*it);
	}

	it = m_ListDeBuff.begin();	
	itEnd = m_ListDeBuff.end();
	for( ; it != itEnd; it++ )
	{		
		if( it->s_pBuffTable->GetInfo()->s_dwID == nID )	
			return &(*it);
	}

	it = m_ListSystemBuff.begin();	
	itEnd = m_ListSystemBuff.end();
	for( ; it != itEnd; it++ )
	{		
		if( it->s_pBuffTable->GetInfo()->s_dwID == nID )	
			return &(*it);
	}
	
	assert_csm( false, L"존재 하지 않는 버프" );
	return NULL;	
}

int cBuffData::_GetUnusalCondLv()
{
	std::list< sBUFF_DATA >::iterator it;
	std::list< sBUFF_DATA >::iterator itEnd;
	CsBuff*		pBuff;
	int			nLv = 0;

	it = m_ListDeBuff.begin();	
	itEnd = m_ListDeBuff.end();	
	for( ; it != itEnd; it++ )
	{		
		pBuff = it->s_pBuffTable;	
		for( int i = 0 ; i < SKILL_APPLY_MAX_COUNT; i++ )
		{
			if( pBuff->GetInfo()->s_dwSkillCode == 0 )
				continue;

			CsSkill::sINFO::sAPPLY pSkillApply = nsCsFileTable::g_pSkillMng->GetSkill( pBuff->GetInfo()->s_dwSkillCode )->GetInfo()->s_Apply[ i ];

			if( pSkillApply.s_nID == 107 && pBuff->GetInfo()->s_nConditionLv > nLv ) // 상태이상 스킬apply 공식인지 체크
			{				
				nLv = pBuff->GetInfo()->s_nConditionLv;
			}
		}	
	}	
	return nLv;
}

void cBuffData::_SetUnusalCond( int nLv )
{		
	if( nLv >= 1 )	// 1레벨 이동만 막음
	{
		m_pObject->DeletePath();
		m_pObject->SetPause( CsC_AvObject::PAUSE_PATH, true );
		m_dwUnusualCond = UNUSUAL_MOVE | UNUSUAL_CHANGE;
	}
	if( nLv >= 2 ) // 2레벨 이동, 아이템 사용막힘
	{
		// 별도 작업없음 IsItemUse()함수 안에 조건 처리됨
		m_dwUnusualCond |= UNUSUAL_ITEM;
	}
	if( nLv >= 3 ) // 3레벨 이동, 아이템, 공격 막힘
	{
		// 별도 작업없음 _IsApplyDUState() 함수안에 조건 처리됨
		m_dwUnusualCond |= UNUSUAL_ATTACK;
	}		
}

void cBuffData::_ReleaseUnusalCond( int nLv )
{		
	m_pObject->SetPause( CsC_AvObject::PAUSE_PATH, false );	

	_SetUnusalCond( nLv );
}

bool cBuffData::IsBuffData( USHORT nID )
{
	LIST_BUFF_IT it = m_ListBuff.begin();	
	LIST_BUFF_IT itEnd = m_ListBuff.end();

	for( ; it != itEnd; it++ )
	{
		if( it->s_pBuffTable->GetInfo()->s_dwID == nID )
			return true;
	}

	it = m_ListDeBuff.begin();	
	itEnd = m_ListDeBuff.end();

	for( ; it != itEnd; it++ )
	{
		if( it->s_pBuffTable->GetInfo()->s_dwID == nID )
			return true;
	}	

	it = m_ListSystemBuff.begin();	
	itEnd = m_ListSystemBuff.end();

	for( ; it != itEnd; it++ )
	{
		if( it->s_pBuffTable->GetInfo()->s_dwID == nID )
			return true;
	}	

	return false;
}

void cBuffData::Update()
{
	if( m_ListEncyBuff.size() != 0 )
	{
		LIST_ENCY_BUFF_IT itEncy = m_ListEncyBuff.begin();
		LIST_ENCY_BUFF_IT itEncyEnd = m_ListEncyBuff.end();
		for( ; itEncy != itEncyEnd ; ++itEncy )
		{
			if( itEncy->s_nEndTime <= _TIME_TS )
			{
				m_ListEncyBuff.erase( itEncy );
				break;
			}
		}
	}

	if( m_ListBuff.empty() && m_ListDeBuff.empty() && m_ListSystemBuff.empty() )
		return;	
	
	std::list< sBUFF_DATA >::iterator it;
	std::list< sBUFF_DATA >::iterator itEnd;

	it = m_ListBuff.begin();	
	itEnd = m_ListBuff.end();

	for( ; it != itEnd; it++)
	{
		if( it->s_nLifeTime == UINT_MAX )
			continue;

		it->s_nLifeTime = it->s_nEndTime - _TIME_TS;
	}

	it = m_ListDeBuff.begin();	
	itEnd = m_ListDeBuff.end();
	for( ; it != itEnd; it++ )
	{		
		if( it->s_nLifeTime == UINT_MAX )
			continue;

		it->s_nLifeTime = it->s_nEndTime - _TIME_TS;	

		if( it->s_nLifeTime <= 0 )
		{
			CsC_AvObject* pObject = g_pMngCollector->GetObject( it->s_nTargetUID );	
	
			if( pObject && pObject->GetAniPause() )
				pObject->SetAniPause( false );
			ReleaseBuff( it->s_pBuffTable->GetInfo()->s_dwID );
			break;
		}

		CsBuff* pBuff = it->s_pBuffTable;	
		for( int i = 0 ; i < SKILL_APPLY_MAX_COUNT; i++ )
		{
			if( pBuff->GetInfo()->s_dwSkillCode == 0 )
				continue;

			CsSkill::sINFO::sAPPLY pSkillApply = nsCsFileTable::g_pSkillMng->GetSkill( pBuff->GetInfo()->s_dwSkillCode )->GetInfo()->s_Apply[ i ];

			if( pSkillApply.s_nID == 107 ) // 상태이상 스킬apply 공식인지 체크
			{
				if( m_pObject->GetProp_Attack()->IsConditionProcess() == false )
				{
					CsC_AvObject* pObject = g_pMngCollector->GetObject( it->s_nTargetUID );	

					CsC_AttackProp::sDAMAGE_INFO DInfo;
					DInfo.s_pHitter = ( pObject != NULL ) ? pObject : NULL;
					DInfo.s_eDamageType = CsC_AttackProp::DT_Condition;
					DInfo.s_nNumType = ( m_pObject->GetLeafRTTI() == RTTI_MONSTER ) ? NUMTYPE::MONSTER_ATTACK : NUMTYPE::DIGIMON_ATTACK;
					DInfo.s_nNumEffect = NUMTYPE::ET_NORMAL;	
					DInfo.s_eActive = CsC_AttackProp::AT_ACTIVE;					
					DInfo.s_eMarbleType = CsC_AttackProp::MB_NONE;
					DInfo.s_fHitEventTime = 1.5f;
					DInfo.s_dwResistTime = pBuff->GetInfo()->s_dwID;	
					DInfo.s_pChildDamageInfo = NULL;

					m_pObject->GetProp_Attack()->InsertDamage( &DInfo );
				}
			}
		}
	}

	it = m_ListSystemBuff.begin();	
	itEnd = m_ListSystemBuff.end();
	for( ; it != itEnd; it++ )
	{		
		if( it->s_nLifeTime == UINT_MAX )
			continue;

		it->s_nLifeTime = it->s_nEndTime - _TIME_TS;	
	}	
}

void cBuffData::_SetEncyBuff( u2 nIdx, u4 nEndTime )
{
	LIST_ENCY_BUFF_IT it = m_ListEncyBuff.begin();
	LIST_ENCY_BUFF_IT itEnd = m_ListEncyBuff.end();

	bool bExist = false;
	for( ; it != itEnd ; ++it )
	{
		if( it->s_nOptIdx == nIdx )		//해당 인덱스의
		{
			it->s_nEndTime = nEndTime;	// 종료시간 셋
			bExist = true;
			break;
		}
	}

	if( bExist == false )	//리스트에 존재하지 않는 버프 일 경우 PushBack
	{
		sENCY_BUFF_DATA pData;// = csnew sENCY_BUFF_DATA;
		pData.s_nOptIdx = nIdx;
		pData.s_nEndTime = nEndTime;
		m_ListEncyBuff.push_back( pData );
	}
}

void cBuffData::_ReleaseEncyBuff( u2 nIdx )
{
	LIST_ENCY_BUFF_IT it = m_ListEncyBuff.begin();
	LIST_ENCY_BUFF_IT itEnd = m_ListEncyBuff.end();

	for( ; it != itEnd ; ++it )
	{
		if( it->s_nOptIdx == nIdx )
		{
			m_ListEncyBuff.erase( it );
			break;
		}
	}
}

void cBuffData::_ReleaseAllEncyBuff()
{
	LIST_ENCY_BUFF_IT it = m_ListEncyBuff.begin();
	LIST_ENCY_BUFF_IT itEnd = m_ListEncyBuff.end();

	for( ; it != itEnd ; ++it )
		m_ListEncyBuff.erase( it );

}

void cBuffData::ClearBuffLoopEffect()
{
	// 버프 or 디버프로 추가된 LoopEffect 제거
	// SetBuff 시 자신의 UID+버프 코드를 제대로 넣어주지 않으면 이펙트가 제거 되지 못함.

	std::list< sBUFF_DATA >::iterator it;
	std::list< sBUFF_DATA >::iterator itEnd;

	DWORD dwTargetUID = 0;
	DWORD dwBuffID = 0;

	if( !m_ListBuff.empty() )
	{
		it = m_ListBuff.begin();	
		itEnd = m_ListBuff.end();	
		for( ; it != itEnd; it++ )		
		{
 			dwTargetUID = it->s_nTargetUID;
 			dwBuffID = it->s_pBuffTable->GetInfo()->s_dwID;
 			CDigimon* pTarget = dynamic_cast<CDigimon*>(g_pMngCollector->GetObject( dwTargetUID ));
			if(pTarget)
			{
				pTarget->GetProp_Effect()->DeleteBuffLoopEffect(dwBuffID);
				if(g_pSEffectMgr->DeleteSEffect(pTarget, dwBuffID) )
				{
					ST_CHAT_PROTOCOL	CProtocol;
					CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
					//CProtocol.m_wStr = GetVAString( _T("Delete SEffect Shading TargetID : %d"), dwTargetUID );
					DmCS::StringFn::Format(CProtocol.m_wStr, _T("Delete SEffect Shading TargetID : %d"), dwTargetUID);
					GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
				}
			}
		}
		m_ListBuff.clear();
	}

	if( !m_ListDeBuff.empty() )
	{
		it = m_ListDeBuff.begin();	
		itEnd = m_ListDeBuff.end();	
		for( ; it != itEnd; it++ )
		{
			dwTargetUID = it->s_nTargetUID;
			dwBuffID = it->s_pBuffTable->GetInfo()->s_dwID;
			CDigimon* pTarget = dynamic_cast<CDigimon*>(g_pMngCollector->GetObject( dwTargetUID ));
			if(pTarget)
			{
				pTarget->GetProp_Effect()->DeleteBuffLoopEffect(dwBuffID);
				if( g_pSEffectMgr->DeleteSEffect(pTarget, dwBuffID) )
				{
					ST_CHAT_PROTOCOL	CProtocol;
					CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
					//CProtocol.m_wStr = GetVAString( _T("Delete SEffect Shading TargetID : %d"), dwTargetUID );
					DmCS::StringFn::Format(CProtocol.m_wStr, _T("Delete SEffect Shading TargetID : %d"), dwTargetUID);

					GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
				}
			}

		}
		m_ListDeBuff.clear();
	}
}

void cBuffData::_SetBuffEffect( CsBuff* pBuff, USHORT nBuffID, UINT nTargetUID )
{

	DWORD dwBuffToSkillCode = nsCsFileTable::g_pBuffMng->GetBuff(nBuffID)->GetInfo()->s_dwSkillCode;
	
	if( dwBuffToSkillCode == 0 )	// 장비 버프들 스킬 코드가 없음.
		return;	

	CsSkill* pSkill = nsCsFileTable::g_pSkillMng->GetSkill(dwBuffToSkillCode);
 	
	if( !pSkill )
	{
		assert_cs(pSkill);
		return;
	}

	// nID가 0인 곳은 버프를 보여줄 필요가 없는 GameScene 이므로, 에러가 아님.
	CsC_AvObject* pTarget = g_pMngCollector->GetObject( nTargetUID );

	if( !pTarget )
		return;
	
	switch(pTarget->GetLeafRTTI())
	{
	case RTTI_DIGIMON:
	case RTTI_DIGIMON_USER:
	case RTTI_MONSTER:
		break;

	default:
		return;
	}

	if( pSkill->GetInfo()->s_Apply[1].s_nID == 0 )	// 스킬 데미지 말고 추가 효과가 없으면 종료
		return;

	if( pBuff->GetInfo()->s_szEffectFile[0] == NULL )	// 이펙트가 없으면 종료
		return;

	char szBuf[OBJECT_PATH_LEN] = {0,};
	if( pBuff->GetInfo()->s_nBuffType == 2 )
		sprintf_s( szBuf, OBJECT_PATH_LEN, "System\\DeBuff\\%s.nif", pBuff->GetInfo()->s_szEffectFile );
	else
		sprintf_s( szBuf, OBJECT_PATH_LEN, "System\\Buff\\%s.nif", pBuff->GetInfo()->s_szEffectFile );
	TCHAR szWide[OBJECT_PATH_LEN] = {0,};
	M2W(szWide, szBuf, OBJECT_PATH_LEN);
	
	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
	//CProtocol.m_wStr = GetVAString(_T("EffectPath : %s"), szWide);

	DmCS::StringFn::Format(CProtocol.m_wStr, _T("EffectPath : %s"), szWide);
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
	//스킬 공식이 아닌 스킬 효과로 구분하도록 수정 (데미지 흡수 반사 스킬이 206공식 사용으로 인함) chu8820 
	char szSoundBuf[OBJECT_PATH_LEN] = {0,};			

	#define CENTERFLAG	nsEFFECT::LIVE_LOOP | nsEFFECT::POS_CHARPOS | nsEFFECT::SPOS_BOUND_CENTER
	#define FOOTFLAG	nsEFFECT::LIVE_LOOP | nsEFFECT::POS_CHARPOS
	#define HEIGHTFLAG	nsEFFECT::LIVE_LOOP | nsEFFECT::POS_CHARPOS | nsEFFECT::SPOS_THEIGHT

	// 디폴트는 캐릭터 발 좌표 + 루프 이펙트 + 따라 댕김
	DWORD dwFlag = FOOTFLAG;

	switch( pSkill->GetInfo()->s_Apply[1].s_nA )
	{
	case APPLY_CA:	// 6 - 크리티컬 증가
	case APPLY_EV:	// 8 - 회피 증가
	case APPLY_SCD:	// 24 - 스킬데미지 상승
		{
			if( m_bIsOnePlayEffect )	// RecvBuff에서만 호출되는 플래그 사용, 버프 싱크로는 발생하지 않도록 하기 위해서
			{
				pTarget->GetProp_Effect()->AddEffect( szBuf, 1.f, nsEFFECT::POS_CHARPOS );
				pTarget->PlaySound(SOUND_LEVEL_UP);
			}
			return;
		}
	case APPLY_UB:  	// 33- 무적
		{
			dwFlag = CENTERFLAG;
			sprintf_s(szSoundBuf, "%s", SOUND_BUFF_UNBEATABLE);


					/* 협의된 적이 없어서 주석처리함. */
// #ifdef BUFF_EFFECT_SYSTEM	// 외곽선 깜빡이
// 					SEffectMgr::st_SHADING_FLAG flag(SEffectMgr::FT_OUTLINE_ARMOR, 
// 						SEffectMgr::DC_WHITE, CGeometry::Color_Gray, nBuffID,
// 						SEffectMgr::UT_INFINITY_TIME, SEffectMgr::ET_OUTLINE);
// 					g_pSEffectMgr->AddSEffect(0.f, pTarget, flag, 0.17f);
// #endif
		}
		break;
	case APPLY_DOT:		// 37 - 지속데미지(도트)
	case APPLY_DOT2:	// 38 - 지연데미지
		{
			if( m_bIsOnePlayEffect ) // RecvBuff에서만 호출되는 플래그 사용, 버프 싱크로는 발생하지 않도록 하기 위해서
				pTarget->GetProp_Effect()->AddEffect( szBuf, 1.f, nsEFFECT::POS_CHARPOS );
			return;
		}
	case APPLY_STUN:	// 39 - 행동불능(스턴,석화)
		{
			if( nBuffID == 40165 || nBuffID == 40172 )				// 기절
			{
				sprintf_s(szSoundBuf, "%s", SOUND_DEBUFF_STUN);
				dwFlag = HEIGHTFLAG;
			}

			else if( nBuffID == 40167 )			// 석화
			{
				sprintf_s(szSoundBuf, "%s", SOUND_DEBUFF_PETRIFY);

				// 그레이 셰이딩 - 14.05.28 적용
					SEffectMgr::st_SHADING_FLAG flag(SEffectMgr::FT_ONECOLOR, 
						SEffectMgr::DC_NONE, CGeometry::Color_Gray, nBuffID,
						SEffectMgr::UT_INFINITY_TIME, SEffectMgr::ET_COLOR);
				g_pSEffectMgr->AddSEffect(0.f, pTarget, flag, 0.f);
			}
		}
		break;
	case APPLY_DR:	// 40 - 데미지 반사
	case APPLY_AB:	// 41 - 데미지 흡수
		{
			dwFlag = FOOTFLAG;
			sprintf_s(szSoundBuf, "%s", SOUND_LEVEL_UP);
		}
		break;
	default:
		// ACHIEVE_BUFF 업적버프
		{
			if( m_bIsOnePlayEffect )	// RecvBuff에서만 호출되는 플래그 사용, 버프 싱크로는 발생하지 않도록 하기 위해서
			{
				pTarget->GetProp_Effect()->AddEffect( szBuf, 1.f, nsEFFECT::POS_CHARPOS );
				pTarget->PlaySound(SOUND_LEVEL_UP);
			}
			return;
		}
		//assert_csm(false, "Loop 이펙트 처리 스킬 분류가 없음, 기본 위치 사용, 클라이언트 팀 문의 바람");
		//break;
	}
	pTarget->GetProp_Effect()->AddBuffLoopEffect( szBuf, nBuffID, dwFlag );

	if( m_bIsOnePlayEffect )
		pTarget->PlaySound(szSoundBuf);

//이전 코드
// 	switch(pSkill->GetInfo()->s_Apply[1].s_nID)
// 	{
// 	case 205:
// 		if( m_bIsOnePlayEffect ) // RecvBuff에서만 호출되는 플래그 사용, 버프 싱크로는 발생하지 않도록 하기 위해서
// 			pTarget->GetProp_Effect()->AddEffect( szBuf, 1.f, nsEFFECT::POS_CHARPOS );
// 		return;
// 
// 	case 206:
// 	case 207:
// 		if( m_bIsOnePlayEffect )	// RecvBuff에서만 호출되는 플래그 사용, 버프 싱크로는 발생하지 않도록 하기 위해서
// 		{
// 			pTarget->GetProp_Effect()->AddEffect( szBuf, 1.f, nsEFFECT::POS_CHARPOS );
// 			pTarget->PlaySound(SOUND_LEVEL_UP);
// 		}
// 		return;
// 
// 	case 208:	// 2014.05.14 패치	
// 		{
// 			char szSoundBuf[OBJECT_PATH_LEN] = {0,};			
// 
// 			#define CENTERFLAG	nsEFFECT::LIVE_LOOP | nsEFFECT::POS_CHARPOS | nsEFFECT::SPOS_BOUND_CENTER
// 			#define FOOTFLAG	nsEFFECT::LIVE_LOOP | nsEFFECT::POS_CHARPOS
// 			#define HEIGHTFLAG	nsEFFECT::LIVE_LOOP | nsEFFECT::POS_CHARPOS | nsEFFECT::SPOS_THEIGHT
// 
// 			// 디폴트는 캐릭터 발 좌표 + 루프 이펙트 + 따라 댕김
// 			DWORD dwFlag = FOOTFLAG;
// 
// 			switch(pSkill->GetInfo()->s_Apply[1].s_nA)
// 			{
// 			case 33:  	// 무적
// 				{
// 					dwFlag = CENTERFLAG;
// 					sprintf_s(szSoundBuf, "%s", SOUND_BUFF_UNBEATABLE);
// #ifdef BUFF_EFFECT_SYSTEM	// 외곽선 깜빡이
// 					SEffectMgr::st_SHADING_FLAG flag(SEffectMgr::FT_OUTLINE_ARMOR, 
// 						SEffectMgr::DC_WHITE, CGeometry::Color_Gray, nBuffID,
// 						SEffectMgr::UT_INFINITY_TIME, SEffectMgr::ET_OUTLINE);
// 					g_pSEffectMgr->AddSEffect(0.f, pTarget, flag, 0.17f);
// #endif
// 				}
// 				break;
// 			case 39:	// 행동불능(스턴,석화)
// 				if( nBuffID == 40165 )				// 기절
// 				{
// 					sprintf_s(szSoundBuf, "%s", SOUND_DEBUFF_STUN);
// 					dwFlag = HEIGHTFLAG;
// 				}
// 
// 				else if( nBuffID == 40167 )			// 석화
// 				{
// 					sprintf_s(szSoundBuf, "%s", SOUND_DEBUFF_PETRIFY);
// 
// #ifdef BUFF_EFFECT_SYSTEM	// 그레이 셰이딩
// 					SEffectMgr::st_SHADING_FLAG flag(SEffectMgr::FT_ONECOLOR, 
// 						SEffectMgr::DC_NONE, CGeometry::Color_Gray, nBuffID,
// 						SEffectMgr::UT_INFINITY_TIME, SEffectMgr::ET_COLOR);
// 
// 					g_pSEffectMgr->AddSEffect(0.f, pTarget, flag, 0.f);
// #endif
// 				}
// 				break;
// 			case 40:	// 데미지 반사
// 				break;
// 			case 41:	// 데미지 흡수
// 				break;
// 
// 			default:
// 				assert_csm(false, "Loop 이펙트 처리 스킬 분류가 없음, 기본 위치 사용, 클라이언트 팀 문의 바람");
// 				break;
// 			}
// 			pTarget->GetProp_Effect()->AddBuffLoopEffect( szBuf, nBuffID, dwFlag );
// 
// 			if( m_bIsOnePlayEffect )
// 				pTarget->PlaySound(szSoundBuf);
// 		}
// 		break;
// 	}
}