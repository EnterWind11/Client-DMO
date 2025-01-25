
#include "stdafx.h"
#include <tchar.h>
#include <stdarg.h>
#include "assert_cs.h"

nsCSDEBUG::sDEBUG_FUNC* nsCSDEBUG::g_pDebugFunc = NULL;

void nsCSDEBUG::_log( char* file, int line, char* Exp )
{
//	char strBuffer[1024] = {0 };
//	sprintf_s( strBuffer, 1024, "소 스   : %s\n\n라 인   : %d\n\n조 건   : %s     \n\n", file, line, Exp );
//	MessageBoxA( NULL , strBuffer, "CsAssert", MB_OK );	
//
//#ifdef _DEBUG
////	__asm int 3;
//#endif /*_DEBUG*/
//
//	if( g_pDebugFunc != NULL )
//	{
//	#ifndef _DEBUG
//		g_pDebugFunc->AssertRelease();
//	#endif /*_DEBUG*/
//	}
}

void nsCSDEBUG::_log( char* file, int line, char* Exp, LPTSTR strApp , ... )	
{
	TCHAR strFile[ 1024 ] = {0, };
	TCHAR strExp[ 1024 ] = {0, };
	TCHAR strBuffer[1024] = {0 };

	M2W( strFile, file, 1024 );
	M2W( strExp, Exp, 1024 );	

	if( strApp )
	{
		TCHAR App[512] = {0 };
		va_list args;
		va_start(args, strApp);
		_vsntprintf_s( App, 512, strApp, args );
		va_end(args);
		_stprintf_s( strBuffer, 1024, _T( "소 스   : %s\n\n라 인   : %d\n\n조 건   : %s \n\n설 명   :\n\n%s     \n\n" ), strFile, line, strExp, App );
	}
	else
	{
		_stprintf_s( strBuffer, 1024, _T( "소 스   : %s\n\n라 인   : %d\n\n조 건   : %s     \n\n" ), strFile, line, strExp );
	}

	MessageBox( NULL , strBuffer, _T( "CsAssert" ), MB_OK );	

#ifdef _DEBUG
//	__asm int 3;
#endif /*_DEBUG*/

	if( g_pDebugFunc != NULL )
	{
#ifndef _DEBUG
		g_pDebugFunc->AssertRelease();
#endif /*_DEBUG*/
	}
}

void nsCSDEBUG::_log( char* file, int line, char* Exp, char* cApp , ... )
{
	char strBuffer[1024] = {0 };

	if( cApp )
	{
		char App[512] = {0 };
		va_list args;
		va_start(args, cApp);
		_vsnprintf_s( App, 512, cApp, args );
		va_end(args);
		sprintf_s( strBuffer, 1024, "소 스   : %s\n\n라 인   : %d\n\n조 건   : %s \n\n설 명   :\n\n%s     \n\n", file, line, Exp, App );
	}
	else
	{
		sprintf_s( strBuffer, 1024, "소 스   : %s\n\n라 인   : %d\n\n조 건   : %s     \n\n", file, line, Exp );
	}

	MessageBoxA( NULL , strBuffer, "CsAssert", MB_OK );	

#ifdef _DEBUG
//	__asm int 3;
#endif /*_DEBUG*/

	if( g_pDebugFunc != NULL )
	{
#ifndef _DEBUG
		g_pDebugFunc->AssertRelease();
#endif /*_DEBUG*/
	}
}