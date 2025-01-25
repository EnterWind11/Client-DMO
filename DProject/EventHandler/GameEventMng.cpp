//---------------------------------------------------------------------------
//
// Nome do arquivo: Game Event Mng.cpp
// Data de criação: 2009.6.29
// Autor: Th
// Descrito:
//
//---------------------------------------------------------------------------
#include "stdafx.h"
#include "GameEventMng.h"

//------------------------------------------------------------
GameEventMng::GameEventMng(void)
{

}
//------------------------------------------------------------
GameEventMng::~GameEventMng(void)
{
	MAP_EVENT_HANDLER_IT map_itr = m_handlers.begin();
	while( map_itr != m_handlers.end() )
	{
		LIST_EVENT_HANDLER_IT vec_itr = map_itr->second.begin();
		while( vec_itr != map_itr->second.end() )
		{
			SAFE_NIDELETE( *vec_itr );
			++vec_itr;
		}
		
		map_itr->second.clear();
		map_itr = m_handlers.erase( map_itr );
	}
	m_handlers.clear();
}

//------------------------------------------------ ------------
// Notifica manipuladores de eventos registrados sobre o evento.
//------------------------------------------------------------

void GameEventMng::OnEvent( unsigned int p_eEventCode, void * p_eAgs)
{
	MAP_EVENT_HANDLER_IT map_itr = m_handlers.find( p_eEventCode );
	if( map_itr == m_handlers.end() )
		return;

	LIST_EVENT_HANDLER_IT vec_itr = map_itr->second.begin();
	while( vec_itr != map_itr->second.end() )
	{
		(*vec_itr)->exec( p_eAgs );
		++vec_itr;
	}
}

//------------------------------------------------ ------------
// Remova manipuladores de eventos com código de evento.
//------------------------------------------------------------

void GameEventMng::DeleteEventCode(unsigned int uiEventCode )
{
	MAP_EVENT_HANDLER_IT map_itr = m_handlers.find( uiEventCode );
	if( map_itr == m_handlers.end() )
		return;
		
	LIST_EVENT_HANDLER_IT vec_itr = map_itr->second.begin();
	while( vec_itr != map_itr->second.end() )
	{
		if( *vec_itr )
		{
			SAFE_NIDELETE(*vec_itr);
			vec_itr = map_itr->second.erase( vec_itr );
			continue;
		}

		++vec_itr;
	}

	m_handlers.erase( map_itr );
}