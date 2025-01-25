

#include "stdafx.h"
#include "ToolTip.h"

#define TOOLTIP_WORLDMAP_NAME_COLOR				NiColor( 247/255.0f, 211/255.0f, 153/255.0f )

void cTooltip::_MakeTooltip_WorldMap()
{
	// MainID = ¸Ê Å¸ÀÔ eWORLDMAP
	// Sub1 = ID

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_13, TOOLTIP_WORLDMAP_NAME_COLOR );

	cString* pString;

	//=================================================================================================================
	//
	//		¸ÊÀÌ¸§
	//
	//=================================================================================================================
	switch( m_nMainID )
	{
	case WORLDMAP_AREA:
		{	
			CsAreaMap::sINFO* pFTArea = nsCsFileTable::g_pWorldMapMng->GetArea( static_cast<int>(m_nSubID_1) )->GetInfo();			
			pString = NiNew cString;
			ti.SetText( pFTArea->s_szName );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );

			_AddLine( true );

			ti.s_eFontSize = CFont::FS_12;
			ti.s_Color = FONT_WHITE;
			g_pStringAnalysis->Cut( &m_StringList, TOOLTIP_CUT_SIZE, pFTArea->s_szComment, &ti );
		}
		break;
	case WORLDMAP_WORLD:
		{
			CsWorldMap::sINFO* pFTWorld = nsCsFileTable::g_pWorldMapMng->GetWorld( static_cast<int>(m_nSubID_1)  )->GetInfo();
			pString = NiNew cString;
			ti.SetText( pFTWorld->s_szName );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );

			_AddLine( true );

			ti.s_eFontSize = CFont::FS_12;
			ti.s_Color = FONT_WHITE;
			g_pStringAnalysis->Cut( &m_StringList, TOOLTIP_CUT_SIZE, pFTWorld->s_szComment, &ti );
		}
		break;
	case WORLDMAP_MAIN:
		{			
			pString = NiNew cString;
			switch( m_nSubID_1 )
			{
			case 1:		ti.SetText( UISTRING_TEXT( "TOOLTIP_WORLD_MAP_REAL_WORLD" ).c_str() );				break;
			case 2:		ti.SetText( UISTRING_TEXT( "TOOLTIP_WORLD_MAP_DIGITAL_WORLD" ).c_str() );			break;
			default:	assert_cs( false );
			}
			
			pString->AddText( &ti );
			m_StringList.AddTail( pString );

			_AddLine( true );

			std::wstring wsMsg;
			switch( m_nSubID_1 )
			{
			case 1:
				wsMsg = UISTRING_TEXT( "TOOLTIP_WORLD_MAP_REAL_WORLD_EXPLAIN" );
				break;
			case 2:
				wsMsg = UISTRING_TEXT( "TOOLTIP_WORLD_MAP_DIGITAL_WORLD_EXPLAIN" );
				break;
			default:
				assert_cs( false );
			}

			ti.s_eFontSize = CFont::FS_12;
			ti.s_Color = FONT_WHITE;
			g_pStringAnalysis->Cut( &m_StringList, TOOLTIP_CUT_SIZE, wsMsg.c_str(), &ti );
		}
		break;
	default:
		assert_cs( false );
	}
}