#pragma once

#ifndef __FILE_PACK_SYSTEM
#define __FILE_PACK_SYSTEM

namespace CsFPS
{
	class ResSupplierImpl;
	class CsFPSystem
	{
	public:
		CsFPSystem(){};
		~CsFPSystem(){};

		static bool Initialize(bool bUsePack, std::list<std::string> const& vecPackName, bool bWrite);
		static bool Initialize(bool bUsePackage, std::string const& cnPkgNamevec, bool bWrite);
		static void Destroy(void);
		static bool IsUsePackage(void);

		static bool IsExist( LPCSTR psPath );
		static bool IsExist( int nIdx, LPCSTR p_psPath );
		static bool IsExistOnlyPack( int nIdx, LPCSTR p_psPath );
		static bool IsExistFromDrive( LPCSTR psPath );
		static bool IsExistPackFile(LPCSTR pPackName);

		static int	GetFileHandle( int nFilePackIndex, LPCSTR szPath );

		static bool SeekLock(int nIdx);
		static void SeekUnLock(int nIdx);

		static CsFileHash::sINFO* GetHashData( int nIdx, LPCSTR szName );
		static void GetFileData( int nIdx, char** ppData, UINT64 nOffset, size_t nSize );
		static size_t GetFileData( int nIdx, LPCSTR szName, std::vector<unsigned char>& vecData );

		//Usado por Fontes e Path Engine.//////////////////////////////////
		static size_t Allocate_GetFileData( int nFilePackIndex, char** ppData, size_t nHashCode );		// Alocar memória e gravar dados
		static size_t Allocate_GetFileData( int nFilePackIndex, char** ppData, LPCSTR szPath );		// Alocar memória e gravar dados
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		//	Funções relacionadas à embalagem
		static bool AddFile( LPCSTR cDataPath, LPCSTR cFilePath );
		static bool EditFile( LPCSTR cDataPath, LPCSTR cFilePath );
		static bool EditFile( int nDestIdx, int nSourceIdx );
		static bool SetAllocCount( size_t nCount );
		static bool AddAllocCount( size_t nCount );
		static size_t CheckNewFileCount( int nDestIdx, int nSourceIdx );
		static bool SaveHashPack(void);
		static size_t GetTotalFileCount(int nIdx);
		static bool UnPacking();

		static void GetFileList( int nIdx,std::list<std::string>& files );
		//////////////////////////////////////////////////////////////////////////
	private:
		static ResSupplierImpl* ms_pImpl;
		static bool				ms_bUsePackage;
		static char*			m_pBuffer;
	};
};

#endif