//---------------------------------------------------------------------------
//
// 파일명 : FlowMgr.cpp
// 작성일 : 
// 작성자 : 
// 설  명 :
//
//---------------------------------------------------------------------------
#include "StdAfx.h"
#include "FlowMgr.h"
#include "FlowFactory.h"
#include "FlowCmdQueue.h"
#include "FlowStack.h"
#include "Flow.h"

//---------------------------------------------------------------------------
namespace Flow
{
	CFlowMgr::CFlowMgr()
	:m_pkFlowCmdQueue(NULL)
	,m_pkFlowStack(NULL)
	,m_pkFlowInstMgr(NULL)
	,m_bFlowLock(FALSE)
	,m_bProcessingLock(FALSE)
	{
	}
//---------------------------------------------------------------------------
	CFlowMgr::~CFlowMgr()
	{
	}
//---------------------------------------------------------------------------
	BOOL CFlowMgr::Create()
	{
		// CmdQueue
		if(!m_pkFlowCmdQueue)
			m_pkFlowCmdQueue = NiNew CFlowCmdQueue;
		// Stack
		if(!m_pkFlowStack)
			m_pkFlowStack = NiNew CFlowStack;
		// InstMgr
		if(!m_pkFlowInstMgr)
			m_pkFlowInstMgr = NiNew CInstMgr<int, CFlow*>;

		return TRUE;
	}
//---------------------------------------------------------------------------
	void CFlowMgr::Destroy()
	{
		CurFlowImmediatelyDestroy();

		// CmdQueue
		if(m_pkFlowCmdQueue)
			m_pkFlowCmdQueue->Clear();
		// Stack
		if(m_pkFlowStack)
			m_pkFlowStack->Clear();
		// InstMgr
		if(m_pkFlowInstMgr)
			m_pkFlowInstMgr->AllDelInst();

		// Delete
		SAFE_NIDELETE(m_pkFlowCmdQueue)
		SAFE_NIDELETE(m_pkFlowStack)
		SAFE_NIDELETE(m_pkFlowInstMgr)
	}

	void CFlowMgr::CurFlowImmediatelyDestroy()
	{
		// 현재 ID, Size Get
		int iID   = GetCurTopFlowID();
		int iSize = GetCurFlowSize();

		while(iSize > 0)
		{
			// 현재 상태 종료
			if(IsCurTopFlow())
				GetCurTopFlow()->OnExit(-1);

			// 현재 상태 제거
			EraseFlow(iID);

			// 현재 ID, Size Get
			iID   = GetCurTopFlowID();
			iSize = GetCurFlowSize();
		}
	}
//---------------------------------------------------------------------------
	BOOL CFlowMgr::StartFlow(int p_iID)
	{
		// 처음 시작할 때 Flow는 비워야함.
		assert(m_pkFlowStack->IsEmpty() && "StartFlow Failed!");

		// Push Flow
		PushFlow(p_iID);		

		return TRUE;
	}
//---------------------------------------------------------------------------
	void CFlowMgr::PushFlow(int p_iID)
	{
		if( !m_pkFlowCmdQueue->IsEmpty() )
			return;
// 		if( m_pkFlowCmdQueue->IsFrontPushFlow( p_iID ) )
// 			return;

		int iSize = GetCurFlowSize();

		for(int i = 0; i < iSize; ++i)
		{
			if(IsCurPosFlow(i))
				GetCurPosFlow(i)->ReservedPushFlow( p_iID );
		}

		// Push Flow
		m_pkFlowCmdQueue->PushFlow(p_iID);
	}

	void CFlowMgr::ChangePopAllFlow( int p_iID )
	{
		if( !m_pkFlowCmdQueue->IsEmpty() )
			return;
// 		if( m_pkFlowCmdQueue->IsFrontChangePopAll( p_iID ) )
// 			return;

		int iSize = GetCurFlowSize();
		for(int i = 0; i < iSize; ++i)
		{
			if(IsCurPosFlow(i))
				GetCurPosFlow(i)->ReservedChangeFlow( p_iID );
		}

		m_pkFlowCmdQueue->ChangePopAllFlow(p_iID);
	}

//---------------------------------------------------------------------------
	void CFlowMgr::ChangeFlow(int p_iID)
	{
#ifdef DEBUG_NETWORK
		DBG("Changing flow\n");
#endif
		if( !m_pkFlowCmdQueue->IsEmpty() )
			return;
 		if( m_pkFlowCmdQueue->IsFrontChange( p_iID ) )
 			return;

		int iSize = GetCurFlowSize();
#ifdef DEBUG_NETWORK
		DBG("Flow size -> %d\n", iSize);
#endif
		for(int i = 0; i < iSize; ++i)
		{
			if (IsCurPosFlow(i)) {

				GetCurPosFlow(i)->ReservedChangeFlow( p_iID );
			}
		}

		// Change Flow
		m_pkFlowCmdQueue->ChangeFlow(p_iID);
	}
//---------------------------------------------------------------------------
	void CFlowMgr::PopFlow(int p_iID)
	{
		if( !m_pkFlowCmdQueue->IsEmpty() )
			return;
// 		if( m_pkFlowCmdQueue->IsFrontPop( p_iID ) )
// 			return;

		int iSize = GetCurFlowSize();

		for(int i = 0; i < iSize; ++i)
		{
			if(IsCurPosFlow(i))
				GetCurPosFlow(i)->ReservedPopFlow();
		}

		// Pop Flow
		m_pkFlowCmdQueue->PopFlow(p_iID);
	}
//---------------------------------------------------------------------------
	void CFlowMgr::PopAllFlow()
	{
		if( m_pkFlowCmdQueue->IsFrontPopAll() )
			return;

		// PopAll Flow
		m_pkFlowCmdQueue->PopAllFlow();
	}
//---------------------------------------------------------------------------
	void CFlowMgr::CMDUpdate()
	{
		while(!m_pkFlowCmdQueue->IsEmpty())
		{
			if( m_bFlowLock )
				return;
			m_bProcessingLock = TRUE;

			// Cmd 얻기
			CFlowCmdQueue::FLOWCMD kCmd;
			ZeroMemory(&kCmd, sizeof(kCmd));
			kCmd = m_pkFlowCmdQueue->GetAt();
#ifdef DEBUG_NETWORK
			DBG("Flow changing CMD to -> %d\n", kCmd.eCmdType);
#endif
			switch(kCmd.eCmdType)
			{

			case CFlowCmdQueue::CMD_PUSH:
				{
					// 현재 ID Get
					int iID = GetCurTopFlowID();

					if(iID != kCmd.iID)
					{
						// 현재 상태 겹칩
						if(IsCurTopFlow())
							GetCurTopFlow()->OnOverride( kCmd.iID );

						// 새 상태 넣음
						RegisterFlow(kCmd.iID);

						// 새 상태 초기화
						if(IsCurTopFlow())
							GetCurTopFlow()->OnEnter();
					}
				}
				break;
			case CFlowCmdQueue::CMD_CHANGE:
				{
					// 현재 ID Get
					int iID = GetCurTopFlowID();

					if(iID != kCmd.iID)
					{
						// 현재 상태 종료
						if (IsCurTopFlow()) {
							GetCurTopFlow()->OnExit(kCmd.iID);
						}

						// 현재 상태 제거
						EraseFlow(iID);

						// 새 상태 넣음
						RegisterFlow(kCmd.iID);

						// 새 상태 초기화
						if (IsCurTopFlow()) {
							GetCurTopFlow()->OnEnter();
						}
					}
				}
				break;
			case CFlowCmdQueue::CMD_CHANGE_POP_ALL:
				{
					// 현재 ID Get
					int iID = GetCurTopFlowID();

					while(iID != CFlow::FLW_NONE)
					{
						// 현재 상태 종료
						if(IsCurTopFlow())
							GetCurTopFlow()->OnExit(-1);

						// 현재 상태 제거
						EraseFlow(iID);

						// 현재 ID
						iID   = GetCurTopFlowID();
					}

					// 새 상태 넣음
					RegisterFlow(kCmd.iID);

					// 새 상태 초기화
					if(IsCurTopFlow())
						GetCurTopFlow()->OnEnter();
				}
				break;
			case CFlowCmdQueue::CMD_POP:
				{
					// 제거 ID 현존 검사
					if(!m_pkFlowStack->IsExistID(kCmd.iID))
						break;

					// 현재 ID Get
					int iID = GetCurTopFlowID();

					while(iID != CFlow::FLW_NONE)
					{
						// 현재 상태 종료
						if(IsCurTopFlow())
							GetCurTopFlow()->OnExit(-1);

						// 현재 상태 제거
						EraseFlow(iID);

						// 바뀐 상태 재시작
						if(IsCurTopFlow())
							GetCurTopFlow()->OnResume(kCmd.iID);

						// 해당 Flow 제거되면 종료
						if(iID == kCmd.iID)
							break;

						// 현재 ID Get
						iID = GetCurTopFlowID();
					}
				}
				break;
			case CFlowCmdQueue::CMD_POP_ALL:
				{
					// 현재 ID, Size Get
					int iID   = GetCurTopFlowID();
					while(iID != CFlow::FLW_NONE)
					{
						// 현재 상태 종료
						if(IsCurTopFlow())
							GetCurTopFlow()->OnExit(-1);

						// 현재 상태 제거
						EraseFlow(iID);

						// 현재 ID, Size Get
						iID   = GetCurTopFlowID();
					}
				} 
				break;
			};
			m_bProcessingLock = FALSE;

		}
	}
//---------------------------------------------------------------------------
	void CFlowMgr::OnIdle()
	{
		// Update Cmd Queue
		CMDUpdate();

		// OnIdle Flow
 		int iSize = GetCurFlowSize();		
 
 		for(int i = 0; i < iSize; ++i)
 		{
 			if(IsCurPosFlow(i))
			{
				if(!GetCurPosFlow(i)->IsPaused())
					GetCurPosFlow(i)->OnIdle();
			}
 		}
	}
//---------------------------------------------------------------------------
	BOOL CFlowMgr::OnMsgHandler(const MSG& p_kMsg)
	{
		BOOL bResult = FALSE;

		if(m_bProcessingLock)
			return bResult;

		if( m_bFlowLock )
			return bResult;

		// 최상위 Flow만 받는다.
		if(IsCurTopFlow() )
			bResult = GetCurTopFlow()->OnMsgHandler(p_kMsg);

		return bResult;
	}
//---------------------------------------------------------------------------
	bool CFlowMgr::LostDevice(void* p_pvData)
	{
		int iSize = GetCurFlowSize();

		for(int i = 0; i < iSize; ++i)
		{
			if(GetCurPosFlow(i))
				GetCurPosFlow(i)->LostDevice(p_pvData);
		}

		return true;
	}
//---------------------------------------------------------------------------	
	bool CFlowMgr::ResetDevice(bool p_bBeforeReset, void* p_pvData)
	{
		int iSize = GetCurFlowSize();

		for(int i = 0; i < iSize; ++i)
		{
			if(GetCurPosFlow(i))
				GetCurPosFlow(i)->ResetDevice(p_bBeforeReset, p_pvData);
		}

		return true;
	}
//---------------------------------------------------------------------------
	BOOL CFlowMgr::RegisterFlow(int p_iID)
	{
		// Stack
		if (!m_pkFlowStack->Push(p_iID))
		{
#ifdef DEBUG_FLOW
			DBG("Register flow 401\N");
#endif
			return FALSE;
		}

		// InstMgr
		if (m_pkFlowInstMgr->GetInst(p_iID)) {
#ifdef DEBUG_FLOW
			DBG("Register flow 409\N");
#endif
			return FALSE;
		}

		CFlow* pkFlow = CFlowFactory::CreateFlow(p_iID);
		m_pkFlowInstMgr->AddInst(p_iID, pkFlow);
#ifdef DEBUG_FLOW
		DBG("Register flow\N");
#endif
		return TRUE;
	}
//---------------------------------------------------------------------------
	BOOL CFlowMgr::EraseFlow(int p_iID)
	{
		// Stack
		int iID = m_pkFlowStack->Top();

		if (iID != p_iID)
		{
#ifdef DEBUG_FLOW
			DBG("ERASE FLOW 430\N");
#endif
			return FALSE;
		}

		m_pkFlowStack->Pop();
#ifdef DEBUG_FLOW
		DBG("ERASE FLOW 437\N");
#endif
		// InstMgr
		m_pkFlowInstMgr->DelInst(p_iID);
			
		return TRUE;
	}
//---------------------------------------------------------------------------
	int CFlowMgr::GetCurFlowSize() const
	{
		return m_pkFlowStack->GetSize();
	}
//---------------------------------------------------------------------------
	BOOL CFlowMgr::IsCurTopFlow() const
	{
		return GetCurTopFlow() != NULL ? TRUE : FALSE;
	}
//---------------------------------------------------------------------------
	CFlow* CFlowMgr::GetCurTopFlow() const
	{
#ifdef DEBUG_FLOW
		DBG("GET CUR TOP FLOW");
#endif
		int iID = CFlow::FLW_NONE;
		CFlow* pkFlow = NULL;

		iID = m_pkFlowStack->GetTopAt();
		if(	iID >-1 )
			pkFlow = m_pkFlowInstMgr->GetInst(iID);
		
		return pkFlow;
	}
//---------------------------------------------------------------------------
	int CFlowMgr::GetCurTopFlowID() const
	{
		return m_pkFlowStack->GetTopAt();
	}
//---------------------------------------------------------------------------
	BOOL CFlowMgr::IsCurPosFlow(int p_iPos) const
	{
#ifdef DEBUG_FLOW
		DBG("IS CUR POS FLOW");
#endif
		return GetCurPosFlow(p_iPos) != NULL ? TRUE : FALSE;
	}
//---------------------------------------------------------------------------
	CFlow* CFlowMgr::GetCurPosFlow(int p_iPos) const
	{
		int iID = CFlow::FLW_NONE;
		CFlow* pkFlow = NULL;

		iID = m_pkFlowStack->GetPosAt(p_iPos);
		pkFlow = m_pkFlowInstMgr->GetInst(iID);
		
		return pkFlow;
	}
//---------------------------------------------------------------------------
	int CFlowMgr::GetCurPosFlowID(int p_iPos) const
	{
		return m_pkFlowStack->GetPosAt(p_iPos);
	}

	void CFlowMgr::LockFlow()
	{
		m_bFlowLock = TRUE;

		int iSize = GetCurFlowSize();
		for(int i = 0; i < iSize; ++i)
		{
			if(IsCurPosFlow(i))
				GetCurPosFlow(i)->SetInputLock();
		}

	}

	void CFlowMgr::UnLockFlow()
	{
		m_bFlowLock = FALSE;
		int iSize = GetCurFlowSize();
		for(int i = 0; i < iSize; ++i)
		{
			if(IsCurPosFlow(i))
				GetCurPosFlow(i)->SetInputUnLock();
		}
	}

	BOOL CFlowMgr::IsCurrentFlow(int p_iID) const
	{
		int iSize = GetCurFlowSize();		

		for(int i = 0; i < iSize; ++i)
		{
			if( GetCurPosFlowID(i) == p_iID )
				return TRUE;
		}

		return FALSE;
	}
}
//---------------------------------------------------------------------------
