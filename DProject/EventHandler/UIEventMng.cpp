//---------------------------------------------------------------------------
//
// nome do arquivo : GameEventMng.cpp
// Data Criada : 2009.6.29
// Escritor : Th
// descrito :
//
//---------------------------------------------------------------------------
#include "stdafx.h"
#include "UIEventMng.h"

namespace UI
{
	//------------------------------------------------------------
	UIEventMng::UIEventMng(void)
	{
		m_handlers.clear();
	}

	//------------------------------------------------------------
	UIEventMng::~UIEventMng(void)
	{
		DeleteEventAll();
	}

	//------------------------------------------------------------
	// Os eventos são notificados aos manipuladores de eventos registrados.
	//------------------------------------------------------------

	void UIEventMng::OnEvent( unsigned int p_eEventCode, void* p_eAgs)
	{
		CONT_EVENTHANDLER_IT map_itr = m_handlers.find( p_eEventCode );
		if( map_itr == m_handlers.end() )
			return;

		if( map_itr->second )
			map_itr->second->exec(p_eAgs);
	}

	//------------------------------------------------------------
	// Remova manipuladores de eventos com código de evento.
	//------------------------------------------------------------

	void UIEventMng::DeleteEventCode(unsigned int uiEventCode )
	{
		CONT_EVENTHANDLER_IT map_itr = m_handlers.find( uiEventCode );
		if( map_itr == m_handlers.end() )
			return;

		SAFE_NIDELETE( map_itr->second );
		m_handlers.erase( map_itr );
	}

	void UIEventMng::DeleteEventAll()
	{
		CONT_EVENTHANDLER_IT map_itr = m_handlers.begin();
		while( map_itr != m_handlers.end() )
		{
			SAFE_NIDELETE( map_itr->second );
			map_itr = m_handlers.erase( map_itr );
		}
		m_handlers.clear();
	}
}