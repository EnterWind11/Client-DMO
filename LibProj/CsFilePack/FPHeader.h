#pragma once

#include <string>
#ifndef FILE_SYSTEM_HEADER_H
#define FILE_SYSTEM_HEADER_H

#pragma pack( push, 4 )

namespace CsFPS
{
	//	공통
	namespace FS_def
	{
		#define FP_BLOCK_SIZE			25600 //	25*1024 
		#define DEFAULT_FOLDER_NAME		"Data"
		#define ASB_FD_MARK				"\\"
		#define PACK_EX_NAME			".pf"
		#define HASH_EX_NAME			".hf"

		#define RESOURCE_PACK_NAME		"Pack01"
		#define PATCH_PACK_NAME			"Patch_"
        #define RESOURCE_PACK03_NAME		"Pack03"
	}

	typedef struct tagFH_HEADER
	{
		tagFH_HEADER():s_nVersion(0),s_nFileCount(0)
		{};

		DWORD		s_nVersion;
		int			s_nFileCount;
	}FHHeader;

	// cabeçalho total do arquivo
	typedef struct tagFP_HEADER
	{
		tagFP_HEADER():s_nVersion(0),s_Plag(0)
		{};

		DWORD		s_nVersion;
		DWORD		s_Plag;
	}FPHeader;

	typedef struct tagCHUNK
	{		
		tagCHUNK():s_Plag(0),s_nNameLen(0)
		{
			ZeroMemory(&s_szPath,MAX_PATH);
		}

		void		GetFilePath(std::string & paht, std::string& name);

		char		s_szPath[ MAX_PATH ];
		DWORD		s_Plag;
		int			s_nNameLen;
	}sCHUNK;

	size_t			GetHashCode( LPCSTR sz );
	std::string		StringToLower( LPCSTR szTrans );
	std::string		StringToUpper( LPCSTR szTrans );

	size_t			GetHashCode( LPCWSTR sz );
	std::wstring	StringToLower( LPCWSTR szTrans );
	std::wstring	StringToUpper( LPCWSTR szTrans );
	bool			ExistFile(LPCSTR szPath);
}

#endif//FILE_SYSTEM_HEADER_H

#pragma pack( pop )
