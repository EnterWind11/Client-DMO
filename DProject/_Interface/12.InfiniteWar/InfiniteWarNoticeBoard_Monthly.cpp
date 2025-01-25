#include "stdafx.h"
#include "InfiniteWarNoticeBoard_Monthly.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"

CInfinitewarNoticeBoardMonthlyUI::CInfinitewarNoticeBoardMonthlyUI(int const& nType)
:CInfinitewarNoticeBoardRankBaseUI(nType)
{
}

CInfinitewarNoticeBoardMonthlyUI::~CInfinitewarNoticeBoardMonthlyUI()
{ 
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

bool CInfinitewarNoticeBoardMonthlyUI::Create(cWindow* pRoot)
{
	if( !CInfinitewarNoticeBoardRankBaseUI::Create(pRoot) )
		return false;

	if( m_pLastRankView )
		m_pLastRankView->SetText( UISTRING_TEXT( "INFINITEWAR_NOTICEBOARD_WINDOW_MONTHLY_PREVIOUSLY_RANKING_VIEW_BUTTON" ).c_str() );
	if( m_pRankLimitTime )
		m_pRankLimitTime->SetText(UISTRING_TEXT( "INFINITEWAR_NOTICEBOARD_WINDOW_MONTHLY_RANKING_END_TIME_TEXT" ).c_str());
	if( m_pNextRankLimitTime )
		m_pNextRankLimitTime->SetText( UISTRING_TEXT( "INFINITEWAR_NOTICEBOARD_WINDOW_MONTHLY_RANKING_START_REMAIN_TIME_TEXT" ).c_str() );
	if( m_pNoNextRankTextMsg )
		m_pNoNextRankTextMsg->SetText( UISTRING_TEXT( "INFINITEWAR_NOTICEBOARD_WINDOW_MONTHLY_RANKING_END_MESSAGE_TEXT" ).c_str() );

	if( m_pRankGetItemView )
		m_pRankGetItemView->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &CInfinitewarNoticeBoardMonthlyUI::_OpenMonthlyRankRewardItemList);
	if( m_pLastRankView )
		m_pLastRankView->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &CInfinitewarNoticeBoardMonthlyUI::_OpenMonthlyPreRankingList);

	UpdateRankData(m_nRankingType);
	UpdateRankerListData(m_nRankingType);
	return true;
}

void CInfinitewarNoticeBoardMonthlyUI::_OpenMonthlyRankRewardItemList(void* pSender, void* pData)
{
	GetSystem()->OpenRankRewardItemListWindow( TB::eInfiniteWarRewardType::eMonth );
}

void CInfinitewarNoticeBoardMonthlyUI::_OpenMonthlyPreRankingList(void* pSender, void* pData)
{
	GetSystem()->OpenPreRankingListWindow( TB::eInfiniteWarRewardType::eMonth );
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////
