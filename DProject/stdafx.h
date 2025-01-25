#pragma once

//#ifdef NDEBUG

// #ifdef _SECURE_SCL
// #undef _SECURE_SCL
// #endif /*_SECURE_SCL*/
// 
// #define _SECURE_SCL	0

#define  LANGUAGE_SUPPORT
//#define  _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS 1

//#endif /*NDEBUG*/

#ifdef VERSION_USA
#include "common_vs2022/Define/Define_USA.h"
#elif defined VERSION_KOR
#include "common_vs2022/Define/Define_KOR.h"
#endif

#include "common_vs2022/pCountry.h"
#include "nlib/base.h"
#include "nlib/cClient.h"
#include "nlib/cPacket.h"
//#include "nlib/cExcel.h"

#undef null
#include "../../LibProj/JsonLib/Include/json.h"
#define null 0

//////////////////////////////////////////////////
// Por favor, adicione abaixo.

//#define _CSTEST

#ifndef _LIB
#	define _LIB
#endif //_LIB


// ======= Gembrio
#include "NSBShaderLib.h"
#include "NSFParserLib.h"
#include "NiTriShape.h"
#include "NiD3DShaderFactory.h"
#include "NiAnimation.h"
#include "NiMemTracker.h"
#include "NiStandardAllocator.h"

//======= biblioteca
#include "..\..\LibProj\CsThread\stdafx.h"
#include "..\..\LibProj\CsFunc\stdafx.h"
#include "..\..\LibProj\CsFileTable\stdafx.h"
#include "..\..\LibProj\CsFilePack\stdafx.h"
#include "..\..\LibProj\CsGamebryo2.3\stdafx.h"
#include "..\..\LibProj\CsGBChar\stdafx.h"

#include "..\..\LibProj\CsDm\Dm-PCH.h"
// #include "..\..\LibProj\CsDm\DmCSMeasure.h"
// #include "..\..\LibProj\CsDm\DmCSRead.h"
// #include "..\..\LibProj\CsDm\DmCSWrite.h"
#include "..\..\LibProj\CsDm\DmXml.h"

#include "_TestDefine.h"

// Código de barras
#include "_Interface\\Barcode\\BarCodeManager.h"

#include "global.h"

#include <NiMilesAudio.h>
#include <NiMilesAudioSystem.h>
#include <NiAudioSystem.h>
#include <NiAudioSource.h>
#include "CSound.h"
#include "CSoundMgr.h"
#include "SoundDefine.h"

#include "Singleton.h"

#include "App/GameApp.h"

#include "GlobalData.h"

//======= Common
#include "common_vs2022/pSync.h"
#include "common_vs2022/pTamer.h"
#include "common_vs2022/pDigimon.h"
#include "common_vs2022/pInven.h"
#include "common_vs2022/pDigivice.h"
#include "common_vs2022/cItemData.h"
#include "common_vs2022/pQuest.h"
#include "common_vs2022/pItem.h"
#include "common_vs2022/cEvoUnit.h"
#include "common_vs2022/pGame.h"
#include "common_vs2022/cBattleInfo.h"
#include "common_vs2022/pmonster.h"
#include "common_vs2022/pSkill.h"
#include "common_vs2022/sShutdown.h"
#include "NetProtocol_Include.h"

#include "Resist.h"

#include "cIME.h"

//====== Cabeçalho de jogo diversos
#include "FmCommon.h"
#include "FmTamer.h"
#include "FmDigimon.h"
// #ifdef MOVIECAPTURE
// #include "MovieCapture.h"
// #endif

#include "_ResourceMng/ResourceMng.h"

#include "GlobalInput.h"

#include "ItemInfo.h"

#include "cCursor.h"

// XmlOption
#include "OptionUIBase.h"

// XmlChatWindow
#include "OptionUIChat.h"
#include "FileUIContents.h"		// ChatContents
#include "FileUIChatting.h"		// ChatMainWindow
#include "FileUISystem.h"		// ChatSystemWindow
#include "FileUIBattleLog.h"	// ChatBattleWindow
#include "PlayerUIOptionDocument.h"
#include "OptionMng.h"

//
#include ".\_Interface\Base\_BaseHeader.h"
#include "CharObject_Header.h"
#include ".\_Interface\Base\_BaseExpertHeader.h"
#include ".\_Interface\Game\_GameIF.h"
//#include ".\_Interface\NoneGame\_NoneGameIFHeader.h"

#include "CharResMng.h"
#include "MngCollector.h"
#include "ObjectController.h"

//#include ".\Dummper\cDumper.hpp"

//======= network
#include "network/cSyncData.h"
#include "network/cNetwork.h"

#include "Main.h"

#include "cFrame.h"
#include "CInput.h"
#include "Engine.h"

#pragma warning( disable : 4995 )
#include "DirectShow.h"

#include "./EventHandler/GameEventMng.h"
#include "./EventHandler/GameEventCode.h"

#include "_Interface/Game/Tooltip_Mng.h"


//////////////////////////////////////////////////////////////////////////
// Definição do valor da chave ao usar o timer no jogo
//////////////////////////////////////////////////////////////////////////
#define COMPULSORY_TIMER				1	// Eventos chamados às 0 horas
#define COMPULSORY_TIMER_NOTICE			2	// Eventos chamados às 23 horas

#define SELECTION_TIMER					3	// Valor da chave do timer
#define SELECTION_TIMER_NOTICE_10		4	// Chamado 10 minutos antes do tempo de restrição de acesso
#define SELECTION_TIMER_NOTICE_30		5	// Chamado 30 minutos antes do tempo de restrição de acesso

#define TIMER_IDEVENT_TIMEOUT			6	// Horário de verificação de tempo de inspeção máxima de recurso
//////////////////////////////////////////////////////////////////////////

//#define USECONSOLE


inline void BhPrtf(char* msg,...)
{

	char App[MAX_PATH] = { 0 };
	va_list args;
	va_start(args, msg);
	_vsnprintf_s( App, MAX_PATH, msg, args );
	va_end(args);

	SYSTEMTIME st;
	GetLocalTime( &st );
	printf( "[%d:%d:%d] %s\n", st.wHour, st.wMinute, st.wSecond, App);
}

#ifdef	USECONSOLE
#define BHPRT(msg,...)	BhPrtf(msg,__VA_ARGS__)
#else
#define BHPRT
#endif

inline TCHAR* GetVAString( TCHAR* szText, ... )
{

	static 	TCHAR App[512] = { 0 };

	va_list args;
	va_start(args, szText);
	_vsntprintf_s( App, 512, szText, args );
	va_end(args);

	return App; 
}

#undef PlaySound

// 
 inline LPCWSTR getNumberFormatW( const UINT64 i64Value )
 {
 	NUMBERFMTW fmt = {0,0,3,L".",L",",1};
 
 	wchar_t MoneyText[32]={0,};
 	swprintf_s( MoneyText, L"%I64d",i64Value );
 
 	static wchar_t moneyFormat[255]={0,};
 	memset(moneyFormat, 0, sizeof(moneyFormat));
 
 	::GetNumberFormatW( NULL, NULL, MoneyText, &fmt, moneyFormat, 255 );
 	return moneyFormat;
 };
 
 inline LPCSTR getNumberFormatA( const UINT64 i64Value )
 {
 	NUMBERFMTA fmt = {0,0,3,".",",",1};
 
 	char MoneyText[32]={0,};
 	sprintf_s( MoneyText, 32, "%I64d",i64Value );
 
 	static char moneyFormat[255]={0,};
 	memset(moneyFormat, 0, sizeof(moneyFormat));
 
 	::GetNumberFormatA( NULL, NULL, MoneyText, &fmt, moneyFormat, 255 );
 	return moneyFormat;
 };


//////////////////////////////////////////////////
// Por favor, adicione acima ..