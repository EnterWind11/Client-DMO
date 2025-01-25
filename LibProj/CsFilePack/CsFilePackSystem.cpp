#include "stdafx.h"
#include "CsFilePackSystem.h"
#include "FPHeader.h"
#include "CsFileHash.h"
#include "CsFilePack.h"

// dr 0x20
#include "../CsDM/Dm-PCH.h"
#include "../CsDM/DmStringfn.h"

namespace CsFPS
{
	static void __stdcall UnicodeToUTF8(std::wstring const& wsSource, std::string& sDest)
	{
		if (wsSource.empty())
			return;

		int iConverted = ::WideCharToMultiByte(CP_UTF8, 0, wsSource.c_str(), wsSource.size(), NULL, 0, NULL, NULL);
		sDest.resize(iConverted);

		::WideCharToMultiByte(CP_UTF8, 0, wsSource.c_str(), wsSource.size(), &sDest.at(0), iConverted, NULL, NULL);
	}

	class ResSupplierImpl
	{
	private:
		struct sFILE
		{
			sFILE():s_pFilePack(NULL),s_pFileHash(NULL)
			{};
			~sFILE() {};

			CsFilePack*		s_pFilePack;
			CsFileHash*		s_pFileHash;			

			bool NewPackage( LPCSTR szPackName )
			{	
				std::string sPackFileName = szPackName;
				sPackFileName += PACK_EX_NAME;
				std::string sPackHashName = szPackName;
				sPackHashName += HASH_EX_NAME;

				s_pFilePack = new CsFilePack;
				s_pFileHash = new CsFileHash;

				// Gerar dados de empacotamento de arquivos	
				s_pFilePack->New( s_pFileHash, sPackFileName.c_str() );
				s_pFileHash->New( sPackHashName.c_str() );
				s_pFileHash->SaveFile();
				return true;
			}

			bool LoadPackage( LPCSTR szPackName, bool bWrite )
			{				
				std::string sPackHashName = szPackName;
				sPackHashName += HASH_EX_NAME;

				std::string sPackFileName = szPackName;
				sPackFileName += PACK_EX_NAME;

				if( !ExistFile( sPackHashName.c_str() ) || !ExistFile( sPackFileName.c_str() ) )
				{
					if( bWrite )
						return NewPackage( szPackName );
					return false;
				}

				s_pFileHash = new CsFileHash;
				if( ERROR_OK != s_pFileHash->Load( sPackHashName.c_str() ) )
				{
					SAFE_DELETE( s_pFileHash );
					return false;
				}

				s_pFilePack = new CsFilePack;
				if( ERROR_OK != s_pFilePack->Load( s_pFileHash, sPackFileName.c_str(), bWrite ) )
				{
					SAFE_DELETE( s_pFilePack );
					SAFE_DELETE( s_pFilePack );
					return false;
				}
				return true;
			}

			void Destroy(void)
			{
				SAFE_DELETE( s_pFilePack );
				SAFE_DELETE( s_pFilePack );
			}

			bool IsExistFile(LPCSTR szFileName) const
			{
				size_t hashCode = GetHashCode( szFileName );
				if( 0 == hashCode )
					return false;
				
				if( NULL== s_pFileHash )
					return false;

				return s_pFileHash->IsExist( hashCode );
			}

			bool IsExistFile(size_t HashCode) const
			{
				if( NULL== s_pFileHash )
					return false;

				return s_pFileHash->IsExist( HashCode );
			}

			int GetFileHandle( size_t HashCode )
			{
				CsFileHash::sINFO * pInfo = s_pFileHash->GetHashInfo( HashCode );
				if( NULL == pInfo )
					return 0;

				return s_pFilePack->_GetFileHandle( pInfo->s_nOffset );
			}

			size_t GetData( char** ppData, size_t HashCode )
			{
				if( NULL == s_pFilePack || NULL == s_pFileHash )
					return 0;

				CsFileHash::sINFO * pInfo = s_pFileHash->GetHashInfo( HashCode );
				if( NULL == pInfo )
					return 0;

				*ppData = new char[ pInfo->s_nDataSize ];
				s_pFilePack->_GetData( ppData, pInfo->s_nOffset, pInfo->s_nDataSize );
				return pInfo->s_nDataSize;
			}

			size_t GetData( std::vector<unsigned char>& pData, size_t HashCode )
			{
				if( NULL == s_pFilePack || NULL == s_pFileHash )
					return 0;

				CsFileHash::sINFO * pInfo = s_pFileHash->GetHashInfo( HashCode );
				if( NULL == pInfo )
					return 0;

				pData.resize( pInfo->s_nDataSize );				
				s_pFilePack->_GetData( pData, pInfo->s_nOffset, pInfo->s_nDataSize );
				return pInfo->s_nDataSize;
			}

			bool AddFilePackage( LPCSTR cDataPath, LPCSTR cFilePath, char * pBuffer )
			{
				return s_pFilePack->_AddFile( cDataPath, cFilePath, pBuffer );	
			}

			bool EditFilePackage( LPCSTR cDataPath, LPCSTR cFilePath, char * pBuffer )
			{
				return s_pFilePack->_EditFile( cDataPath, cFilePath, pBuffer );	
			}

			bool EditFilePackage( sFILE & pAddPackage )
			{
				return s_pFilePack->_EditFile( pAddPackage.s_pFilePack, pAddPackage.s_pFileHash );	
			}

			size_t GetTotalCount()
			{
				return s_pFileHash->GetTotalFileCount();
			}
		};

		typedef std::vector<sFILE> CONT_PACKAGE_VEC;

	public:
		ResSupplierImpl(void)
		{

		}

		~ResSupplierImpl(void)
		{

		}
		static bool CreateFolder(LPCSTR szFolder)
		{
			if (!szFolder || !lstrlenA(szFolder))
				return FALSE;

			DWORD dwAttrib = GetFileAttributesA(szFolder);

			// already exists ?
			if (dwAttrib != 0xffffffff)
				return ((dwAttrib & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);

			// criar recursivamente de cima para baixo
			char* szPath = _strdup(szFolder);
			char* p = strrchr(szPath, '\\');

			if (p)
			{
				// The parent is a dir, not a drive
				*p = '\0';

				// if can't create parent
				if (!CreateFolder(szPath))
				{
					free(szPath);
					return FALSE;
				}
				free(szPath);

				if (!::CreateDirectoryA(szFolder, NULL))
					return FALSE;
			}

			return TRUE;
		}

		static bool CreateFilePath(LPCSTR szFilePath)
		{
			char* szPath = _strdup(szFilePath);
			char* p = strrchr(szPath, '\\');

			bool bRes = FALSE;

			if (p)
			{
				*p = '\0';

				bRes = CreateFolder(szPath);
			}

			free(szPath);

			return bRes;
		}

		//	carregar o pacote
		bool LoadPackage(std::string const& sPackageName, bool bWrite)
		{
			sFILE loadPackage;
			if( !loadPackage.LoadPackage( sPackageName.c_str(), bWrite ) )
				return false;

			m_PackageVec.push_back( loadPackage );
			return true;
		}

		//	데이타를 읽어온다.
		size_t ReadData(int nIndex, size_t FileHashCode, char** pData)
		{
			if( (int)m_PackageVec.size() <= nIndex )
				return 0;

			return m_PackageVec[nIndex].GetData(pData, FileHashCode);
		}

		//	데이타를 읽어온다.
		size_t ReadData(int nIndex, size_t FileHashCode, std::vector< unsigned char >& pData)
		{
			if( (int)m_PackageVec.size() <= nIndex )
				return 0;

			return m_PackageVec[nIndex].GetData(pData, FileHashCode);
		}

		//	파일이 있나 확인
		bool IsExistFile( int nIdx, LPCSTR sFileName ) const
		{
			size_t HashCode = GetHashCode( sFileName );
			if( 0 == HashCode )
				return false;

			return IsExistFile( nIdx, HashCode );
		}

		bool IsExistFile( int nIdx, size_t HashCode ) const
		{
			if( (int)m_PackageVec.size() <= nIdx )
				return false;

			return m_PackageVec[nIdx].IsExistFile( HashCode );
		}

		//	파일이 있나 확인
		bool IsExistFile( LPCSTR sFileName ) const
		{
			size_t HashCode = GetHashCode( sFileName );
			if( 0 == HashCode )
				return false;

			return IsExistFile( HashCode );
		}

		//	파일이 있나 확인
		bool IsExistFile( size_t HashCode ) const
		{
			CONT_PACKAGE_VEC::const_iterator it = m_PackageVec.begin();
			for( ; it != m_PackageVec.end(); ++it )
			{
				if( (*it).IsExistFile( HashCode ) )
					return true;
			}			
			return false;
		}

		int GetFileHandle( int nIdx, LPCSTR sFileName )
		{
			size_t HashCode = GetHashCode( sFileName );
			if( 0 == HashCode )
				return 0;

			return GetFileHandle( nIdx, HashCode );
		}

		int GetFileHandle( int nIdx, size_t HashCode )
		{
			if( (int)m_PackageVec.size() <= nIdx )
				return 0;

			return m_PackageVec[nIdx].GetFileHandle( HashCode );
		}

		//	로드했던 패키지를 닫는다.
		void ReleasePackage(void)
		{
			CONT_PACKAGE_VEC::iterator it = m_PackageVec.begin();
			for( ; it != m_PackageVec.end(); ++it )
				(*it).Destroy();

			m_PackageVec.clear();
		}

		bool AddFile( LPCSTR cDataPath, LPCSTR cFilePath, char * pBuffer )
		{
			if( m_PackageVec.empty() )
				return false;

			return m_PackageVec[0].AddFilePackage( cDataPath, cFilePath, pBuffer );
		}

		bool EditFilePack( LPCSTR cDataPath, LPCSTR cFilePath, char * pBuffer )
		{
			if( m_PackageVec.empty() )
				return false;

			return m_PackageVec[0].EditFilePackage( cDataPath, cFilePath, pBuffer );
		}

		bool EditFilePack( int nDestIdx, int nSourceIdx )
		{
			if( (int)m_PackageVec.size() <= nDestIdx )
				return false;

			if( (int)m_PackageVec.size() <= nSourceIdx )
				return false;

			return m_PackageVec[nDestIdx].EditFilePackage( m_PackageVec[nSourceIdx] );
		}

		size_t GetTotalFileCount( int nIdx )
		{
			if( (int)m_PackageVec.size() <= nIdx )
				return false;

			return m_PackageVec[nIdx].GetTotalCount();
		}

		bool SetAllocCount(size_t nCount)
		{
			if( m_PackageVec.empty() )
				return false;

			m_PackageVec[0].s_pFileHash->SetAllocCount( (int)nCount );
			return true;
		}

		bool AddAllocCount(size_t nCount)
		{
			if( m_PackageVec.empty() )
				return false;

			CsFPS::FHHeader* pFileHeader = m_PackageVec[0].s_pFileHash->GetHeader();
			if( NULL == pFileHeader )
				return false;

			m_PackageVec[0].s_pFileHash->SetAllocCount( pFileHeader->s_nFileCount + (int)nCount );
			return true;
		}

		bool SaveHashPack(void)
		{
			if( m_PackageVec.empty() )
				return false;

			m_PackageVec[0].s_pFileHash->SaveFile();
			return true;
		}

		bool SeekLock(int nIdx)
		{
			if( (int)m_PackageVec.size() <= nIdx )
				return false;

			if( NULL == m_PackageVec[nIdx].s_pFilePack )
				return false;

			m_PackageVec[nIdx].s_pFilePack->SeekLock();
			return true;
		}

		void SeekUnLock(int nIdx)
		{
			if( (int)m_PackageVec.size() <= nIdx )
				return;

			if( NULL == m_PackageVec[nIdx].s_pFilePack )
				return;

			m_PackageVec[nIdx].s_pFilePack->SeekUnlock();
		}

		CsFileHash::sINFO* GetHashData( int nIdx, LPCSTR szName )
		{
			size_t hashCode = GetHashCode( szName );
			if( !IsExistFile( nIdx, hashCode ) )
				return NULL;

			return m_PackageVec[nIdx].s_pFileHash->GetHashInfo( hashCode );
		}

		void GetFileData( int nIdx, char** ppData, UINT64 nOffset, size_t nSize )
		{
			if( (int)m_PackageVec.size() <= nIdx )
				return ;

			m_PackageVec[nIdx].s_pFilePack->_GetData( ppData, nOffset, nSize );
		}

		size_t CheckNewFileCount( int nDestIdx, int nSourceIdx )
		{
			if( (int)m_PackageVec.size() <= nDestIdx )
				return 0;

			if( (int)m_PackageVec.size() <= nSourceIdx )
				return 0;

			size_t newCount = 0;
			CsFPS::sCHUNK Chunk;
			CsFilePack* pPatchPack = m_PackageVec[nSourceIdx].s_pFilePack;
			std::map< size_t, CsFileHash::sINFO* >* pMap = m_PackageVec[nSourceIdx].s_pFileHash->GetHashMap();			
			std::map< size_t, CsFileHash::sINFO* >::iterator it = pMap->begin();
			for( ; it != pMap->end(); ++it )
			{
				pPatchPack->_GetChunk( &Chunk, it->second->s_nOffset );

				if( !IsExistFile( nDestIdx, Chunk.s_szPath ) )
					newCount++;
			}

			return newCount;
		}

		void GetFileList( int const& nIdx, std::list<std::string>& files )
		{
			if( m_PackageVec.empty() )
				return;		

			if( (int)m_PackageVec.size() <= nIdx )
				return;

			CsFileHash* fileHash = m_PackageVec[nIdx].s_pFileHash;
			CsFilePack* filePack = m_PackageVec[nIdx].s_pFilePack;
			std::map< size_t, CsFileHash::sINFO* >* pMap = fileHash->GetHashMap();
			std::map< size_t, CsFileHash::sINFO* >::iterator it = pMap->begin();
			for( ; it!=pMap->end(); ++it )
			{
				CsFPS::sCHUNK Chunk;
				filePack->_GetChunk( &Chunk, it->second->s_nOffset );
				files.push_back( Chunk.s_szPath );
			}
		}

		bool UnPacking(void)
		{
			if( m_PackageVec.empty() )
				return false;		

			CsFileHash* fileHash = m_PackageVec[0].s_pFileHash;
			CsFilePack* filePack = m_PackageVec[0].s_pFilePack;

			std::map< size_t, CsFileHash::sINFO* >* pMap = fileHash->GetHashMap();
			std::map< size_t, CsFileHash::sINFO* >::iterator it = pMap->begin();
			std::map< size_t, CsFileHash::sINFO* >::iterator itEnd = pMap->end();
			
			char* pAlloc = NULL;
			size_t nAllocSize = 0;
			const auto v32Names = fileHash->GetFileNames32();

			for( ; it!=itEnd; ++it )
			{
				CsFPS::sCHUNK Chunk;

				if (fileHash->HaveChunk())
				{
					filePack->_GetChunk(&Chunk, it->second->s_nOffset);
				}
				else
				{
					auto itn = v32Names.find(it->first);
					assert(itn != v32Names.end());
					std::string sz;
					UnicodeToUTF8(itn->second, sz);
					Chunk.s_nNameLen = sz.size();
					Chunk.s_Plag = 0;
					strncpy_s(Chunk.s_szPath, _countof(Chunk.s_szPath), sz.c_str(), MAX_PATH);
				}

				if (nAllocSize < it->second->s_nDataSize)
				{
					nAllocSize = it->second->s_nDataSize;
					free(pAlloc);
					pAlloc = (char*)malloc(nAllocSize);
				}

				std::string path;
				std::string name;
				Chunk.GetFilePath(path, name);
				
				if( _stricmp(name.c_str(), "thumbs.db" ) == 0 )
					continue;

				CreateFilePath(path.c_str());

				DWORD modeBackup = GetFileAttributesA( Chunk.s_szPath );
				DWORD mode = modeBackup;
				mode &= ~( FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM );
				SetFileAttributesA( Chunk.s_szPath, mode );

				FILE* pCreateFile = NULL;
				std::string fullPath = path;
				fullPath += name;
				fopen_s( &pCreateFile, fullPath.c_str(), "wb" );
				if( NULL == pCreateFile)
					continue;

				filePack->_GetData(&pAlloc, it->second->s_nOffset, it->second->s_nDataSize );
				fwrite( pAlloc, sizeof( char ), it->second->s_nDataSize, pCreateFile );
				fclose( pCreateFile );
			}

			if( pAlloc != NULL )
			{
				free( pAlloc );
				pAlloc = NULL;
			}

			return true;
		}
		
	private:
		CONT_PACKAGE_VEC	m_PackageVec;
	};

	//-------------------------------------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------------------------------
	
	ResSupplierImpl*	CsFPSystem::ms_pImpl = 0;
	bool				CsFPSystem::ms_bUsePackage = false;
	char*				CsFPSystem::m_pBuffer = 0;

	bool CsFPSystem::Initialize(bool bUsePackage, std::list<std::string> const& cnPkgNamevec, bool bWrite )
	{
		if( ms_pImpl )
			return false;

		ms_bUsePackage = bUsePackage;

		if( !ms_bUsePackage && cnPkgNamevec.empty() )
			return true;

		ms_pImpl = new ResSupplierImpl;
		if( !ms_pImpl )
			return false;


		std::list<std::string>::const_iterator it = cnPkgNamevec.begin();
		while( it != cnPkgNamevec.end() )
		{
			if (!ms_pImpl->LoadPackage((*it), bWrite))
			{
				SAFE_DELETE(ms_pImpl);
				return false;
			}

			++it;
		}

		if( bWrite )
			m_pBuffer = new char[ FP_BLOCK_SIZE ];

		return true;
	}

	bool CsFPSystem::Initialize(bool bUsePackage, std::string const& cnPkgNamevec, bool bWrite)
	{
		if( ms_pImpl )
			return false;

		ms_bUsePackage = bUsePackage;

		if( !ms_bUsePackage && 0 == cnPkgNamevec.length() )
			return true;

		ms_pImpl = new ResSupplierImpl;
		if( !ms_pImpl )
			return false;

		if( !ms_pImpl->LoadPackage( cnPkgNamevec, bWrite ) )
		{
			SAFE_DELETE(ms_pImpl);
			return false;
		}

		if( bWrite )
			m_pBuffer = new char[ FP_BLOCK_SIZE ];

		return true;
	}

	void CsFPSystem::Destroy(void)
	{
		if( ms_pImpl )
			ms_pImpl->ReleasePackage();

		SAFE_DELETE( ms_pImpl );
		SAFE_DELETE_ARRAY( m_pBuffer );
	}

	bool CsFPSystem::IsUsePackage(void)
	{
		return ms_bUsePackage;
	}

	// 
	bool CsFPSystem::IsExist( LPCSTR p_psPath )
	{
		if( ms_pImpl && ms_pImpl->IsExistFile( p_psPath ) )
			return true;

		return IsExistFromDrive(p_psPath);
	}

	bool CsFPSystem::IsExist( int nIdx, LPCSTR p_psPath )
	{
		if( ms_pImpl && ms_pImpl->IsExistFile( nIdx, p_psPath ) )
			return true;

		return IsExistFromDrive(p_psPath);
	}

	bool CsFPSystem::IsExistOnlyPack( int nIdx, LPCSTR p_psPath )
	{
		if( ms_pImpl && ms_pImpl->IsExistFile( nIdx, p_psPath ) )
			return true;

		return false;
	}


	bool CsFPSystem::IsExistFromDrive( LPCSTR p_psPath)
	{
		DWORD dwAttr = ::GetFileAttributesA( p_psPath );
		if( dwAttr == INVALID_FILE_ATTRIBUTES )
			return false;

		return ( dwAttr & FILE_ATTRIBUTE_DIRECTORY )?( false ):( true );
	}

	bool CsFPSystem::IsExistPackFile(LPCSTR pPackName)
	{
		std::string packFile = pPackName;
		packFile += PACK_EX_NAME;
		if( !IsExistFromDrive( packFile.c_str() ) )
			return false;

		std::string hashFile = pPackName;
		hashFile += HASH_EX_NAME;
		if( !IsExistFromDrive( hashFile.c_str() ) )
			return false;
		return true;
	}

	int	CsFPSystem::GetFileHandle( int nFilePackIndex, LPCSTR szPath )
	{
		if( NULL == ms_pImpl )
			return 0;

		return ms_pImpl->GetFileHandle( nFilePackIndex, szPath );
	}

	size_t CsFPSystem::Allocate_GetFileData( int nFilePackIndex, char** ppData, size_t nHashCode )
	{
		if( NULL == ms_pImpl )
			return 0;

		return ms_pImpl->ReadData( nFilePackIndex, nHashCode, ppData );
	}

	size_t CsFPSystem::Allocate_GetFileData( int nFilePackIndex, char** ppData, LPCSTR szPath )
	{
		if( NULL == ms_pImpl )
			return 0;

		return ms_pImpl->ReadData( nFilePackIndex, GetHashCode(szPath), ppData );
	}

	//////////////////////////////////////////////////////////////////////////
	//	패킹 관련 함수
	//////////////////////////////////////////////////////////////////////////
	size_t CsFPSystem::GetTotalFileCount(int nIdx)
	{
		if( NULL == ms_pImpl )
			return 0;

		return ms_pImpl->GetTotalFileCount( nIdx );
	}

	bool CsFPSystem::AddFile( LPCSTR cDataPath, LPCSTR cFilePath )
	{
		if( NULL == ms_pImpl )
			return false;

		return ms_pImpl->AddFile( cDataPath, cFilePath, m_pBuffer );
	}

	bool CsFPSystem::EditFile( LPCSTR cDataPath, LPCSTR cFilePath )
	{
		if( NULL == ms_pImpl )
			return false;

		return ms_pImpl->EditFilePack( cDataPath, cFilePath, m_pBuffer );
	}

	bool CsFPSystem::EditFile( int nDestIdx, int nSourceIdx )
	{
		if( NULL == ms_pImpl )
			return false;

		return ms_pImpl->EditFilePack( nDestIdx, nSourceIdx );		
	}

	bool CsFPSystem::SetAllocCount( size_t nCount )
	{
		if( NULL == ms_pImpl )
			return false;

		return ms_pImpl->SetAllocCount( nCount );	
	}

	bool CsFPSystem::AddAllocCount( size_t nCount )
	{
		if( NULL == ms_pImpl )
			return false;

		return ms_pImpl->AddAllocCount( nCount );	
	}

	bool CsFPSystem::SaveHashPack(void)
	{
		if( NULL == ms_pImpl )
			return false;

		return ms_pImpl->SaveHashPack();	
	}

	bool CsFPSystem::UnPacking(void)
	{
		if( NULL == ms_pImpl )
			return false;

		return ms_pImpl->UnPacking();
	}

	size_t CsFPSystem::CheckNewFileCount( int nDestIdx, int nSourceIdx )
	{
		if( NULL == ms_pImpl )
			return false;

		return ms_pImpl->CheckNewFileCount(nDestIdx, nSourceIdx);
	}

	bool CsFPSystem::SeekLock(int nIdx)
	{
		if( NULL == ms_pImpl )
			return false;

		return ms_pImpl->SeekLock(nIdx);
	}

	void CsFPSystem::SeekUnLock(int nIdx)
	{
		if( NULL == ms_pImpl )
			return;

		ms_pImpl->SeekUnLock(nIdx);
	}

	CsFileHash::sINFO* CsFPSystem::GetHashData( int nIdx, LPCSTR szName )
	{
		if( NULL == ms_pImpl )
			return NULL;

		return ms_pImpl->GetHashData(nIdx, szName);
	}

	void CsFPSystem::GetFileData( int nIdx, char** ppData, UINT64 nOffset, size_t nSize )
	{
		if( NULL == ms_pImpl )
			return;

		ms_pImpl->GetFileData(nIdx, ppData, nOffset, nSize);
	}

	size_t CsFPSystem::GetFileData( int nIdx, LPCSTR szName, std::vector< unsigned char >& vecData )
	{
		if( NULL == ms_pImpl )
			return 0 ;

		return ms_pImpl->ReadData(nIdx, GetHashCode(szName), vecData);
	}

	void CsFPSystem::GetFileList( int nIdx,std::list<std::string>& files )
	{
		if( NULL == ms_pImpl )
			return;

		return ms_pImpl->GetFileList(nIdx,files);
	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
}