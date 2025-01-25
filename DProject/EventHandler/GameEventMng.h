//---------------------------------------------------------------------------
//
// Nome do arquivo: Game Event Mng.h
// Data de criação: 2009.6.29
// Autor: Th
// Descrito:
//
//---------------------------------------------------------------------------
#pragma once
#include "EventHandler.h"
#include <map>
#include <list>

typedef std::list< EventHandlerBase* >								LIST_EVENT_HANDLER;
typedef LIST_EVENT_HANDLER::iterator								LIST_EVENT_HANDLER_IT;

typedef std::map< unsigned int, LIST_EVENT_HANDLER >				MAP_EVENT_HANDLER;
typedef std::map< unsigned int, LIST_EVENT_HANDLER >::iterator		MAP_EVENT_HANDLER_IT;

class GameEventMng : public CSingleton< GameEventMng >
{
public:
	GameEventMng();
	virtual ~GameEventMng();

	template <class T>
	void AddEvent( unsigned int p_eEventCode, T* obj, void (T::*memFn)(void*))	// 이벤트 추가	
	{
		MAP_EVENT_HANDLER_IT map_itr = m_handlers.find( p_eEventCode );
		if( map_itr == m_handlers.end() )
		{
			auto _result = m_handlers.insert( std::make_pair( p_eEventCode, LIST_EVENT_HANDLER() ) );
			if( !_result.second )
				return; // Falha ao criar um novo número de código de evento no mapa

			map_itr = _result.first;
		}

		if( map_itr != m_handlers.end() )
		{
			EventHandler<T> * p_EventHandler = NiNew EventHandler<T>(obj,memFn);
			if( NULL == p_EventHandler )
				return; // novo falhou

			LIST_EVENT_HANDLER_IT vec_itr = map_itr->second.begin();
			while( vec_itr != map_itr->second.end() )
			{
				EventHandler<T>* pHandler = static_cast<EventHandler<T>*>(*vec_itr);
				if( *pHandler == *p_EventHandler )
				{
					SAFE_NIDELETE( p_EventHandler );
					return;// Falha porque o mesmo manipulador de eventos já existe.
				}
				++vec_itr;
			}

			map_itr->second.push_back( p_EventHandler );
		}
	}
	
	template <class T>
	void DeleteEvent( unsigned int p_eEventCode , T* pCaller)			// remover evento
	{
		MAP_EVENT_HANDLER_IT map_itr = m_handlers.find( p_eEventCode );
		if( map_itr == m_handlers.end() )
			return;

		LIST_EVENT_HANDLER_IT vec_itr = map_itr->second.begin();
		while( vec_itr != map_itr->second.end() )
		{
			if( *vec_itr )
			{
				EventHandler<T>* pHandler = static_cast<EventHandler<T>*>(*vec_itr);
				if( pHandler->CompareCaller( pCaller ) )
				{
					SAFE_NIDELETE( *vec_itr );
					vec_itr = map_itr->second.erase( vec_itr );
					continue;
				}
			}
			++vec_itr;
		}

		if( map_itr->second.empty() )
			m_handlers.erase( map_itr );
	}

	template <class T>
	void DeleteEventAll( T* pCaller )
	{
		MAP_EVENT_HANDLER_IT map_itr = m_handlers.begin();
		while( map_itr != m_handlers.end() )
		{
			LIST_EVENT_HANDLER_IT vec_itr = map_itr->second.begin();
			while( vec_itr != map_itr->second.end() )
			{
				if( *vec_itr )
				{
					EventHandler<T>* pHandler = static_cast<EventHandler<T>*>(*vec_itr);
					if( pHandler->CompareCaller( pCaller ) )
					{
						SAFE_NIDELETE( *vec_itr );
						vec_itr = map_itr->second.erase( vec_itr );
						continue;
					}
				}

				++vec_itr;
			}

			if( map_itr->second.empty() )
			{
				map_itr = m_handlers.erase( map_itr );
				continue;
			}

			++map_itr;
		}
	}

	void OnEvent( unsigned int p_eEventCode , void *p_eAgs = NULL );					// evento ocorre
	void DeleteEventCode(unsigned int p_eEventCode );
	
private:
 	MAP_EVENT_HANDLER		m_handlers;
};

//---------------------------------------------------------------------------
// Singleton
#define GAME_EVENT_ST		GameEventMng::GetSingleton()
#define GAME_EVENT_STPTR	GameEventMng::GetSingletonPtr()
//---------------------------------------------------------------------------