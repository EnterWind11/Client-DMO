//---------------------------------------------------------------------------
//
// 파일명 : Flow.inl
// 작성일 : 
// 작성자 : 
// 설  명 : 
//
//---------------------------------------------------------------------------
#include "Flow.h"
namespace Flow
{	
	inline BOOL CFlow::IsInitialized() const
	{
		return m_bInitialized;
	}
	//---------------------------------------------------------------------------
	inline BOOL CFlow::IsCreated() const
	{
		return m_bCreated;
	}
	//---------------------------------------------------------------------------
	inline BOOL CFlow::IsPaused() const
	{
		return m_bPaused;
	}
	//---------------------------------------------------------------------------
	inline void CFlow::SetInitialized(BOOL p_bOn)
	{
		m_bInitialized = p_bOn;
	}
	//---------------------------------------------------------------------------
	inline void CFlow::SetCreated(BOOL p_bOn)
	{
		m_bCreated = p_bOn;
	}
	//---------------------------------------------------------------------------
	inline void CFlow::SetLoaded(BOOL p_bOn)
	{
		m_bLoaded = p_bOn;
	}
	//---------------------------------------------------------------------------
	inline void CFlow::SetPaused(BOOL p_bOn)
	{
		m_bPaused = p_bOn;
	}
	//---------------------------------------------------------------------------
	inline void CFlow::SetID(int p_iID)
	{
		m_iID = p_iID;
	}
	//---------------------------------------------------------------------------
	inline int CFlow::GetID() const
	{
		return m_iID;
	}
	//---------------------------------------------------------------------------
}