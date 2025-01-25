

#include "stdafx.h"
#include "../CsDM/Dm-PCH.h"

#include "CsFileHash.h"

#include <tchar.h>

#include "../CsDM/DmSecuritySelf.h"
#include "../CsDM/DmCompressZip.h"
#include <CsFunc/CsMessageBox.h>

namespace CsFPS
{
	CsFileHash::CsFileHash()
	{
		__m_pDeletePointer = NULL;
		m_nAllocCount = 0;
	}

	CsFileHash::~CsFileHash()
	{
		Delete();
	}


	size_t CsFileHash::GetTotalFileCount()
	{
		return m_mapHash.size();
	}

	void CsFileHash::Delete()
	{
		SAFE_DELETE_ARRAY(__m_pDeletePointer);
		m_nAllocCount = 0;

		m_mapHash.clear();
		m_vpDummy.clear();
	}

	void CsFileHash::New(LPCSTR szHashPath)
	{
		m_szFilePath = szHashPath;
		m_Header.s_nVersion = FILE_HASH_VERSION;
		m_Header.s_nFileCount = 0;

		assert(__m_pDeletePointer == NULL);
	}

	DWORD CsFileHash::Load(LPCSTR szHashFile)
	{
		if (_access_s(szHashFile, 0) != 0)
			return ERROR_HASH_FILE_LOAD;

		m_szFilePath = szHashFile;

		int nHandle;
		_sopen_s(&nHandle, m_szFilePath.c_str(), O_RDONLY | O_BINARY, _SH_DENYWR, _S_IREAD); //저장할 소스파일 열기.
		if (nHandle == FP_INVALIDE_HANDLE)
		{
			CsMessageBox(MB_OK, _T("HashFile Error"));
			return ERROR_HASH_FILE_OPEN;
		}

		_read(nHandle, &m_Header.s_nVersion, 4);

		// Tratamento da versão mais tarde
		if (m_Header.s_nVersion == FILE_HASH_VERSION)
		{
			_read(nHandle, &m_Header.s_nFileCount, 4);

			// alocação de memória de matriz
			assert(__m_pDeletePointer == NULL);
			__m_pDeletePointer = (CsFileHash::sINFO*)calloc(m_Header.s_nFileCount, sizeof(sINFO));
			_read(nHandle, __m_pDeletePointer, sizeof(sINFO) * m_Header.s_nFileCount);
			_close(nHandle);
		}
		else if (m_Header.s_nVersion == FILE_HASH_VERSION_SECURITY || m_Header.s_nVersion == FILE_HASH_VERSION_SECURITY_NEW || m_Header.s_nVersion == FILE_HASH_VERSION_SECURITY_NEW2 || m_Header.s_nVersion == 0x20)
		{
			uint32_t size;
			_read(nHandle, &size, 4);

			if (m_Header.s_nVersion == FILE_HASH_VERSION_DRPG)
			{
				_read(nHandle, &size, 4);
				_read(nHandle, &size, 4);
			}

			dm_bytevec data;
			data.resize(size);
			_read(nHandle, &data[0], size);
			_close(nHandle);

			dm_bytevec ovec;

			if (m_Header.s_nVersion == FILE_HASH_VERSION_SECURITY)
			{
				if (!DmFS::SecuritySelf::Decript(DmFS::Enc::Kind::Xor | DmFS::Enc::Kind::Twist | DmFS::Enc::Kind::RvsFull, data, ovec))
					return ERROR_HASH_FILE_OPEN;
			}
			else if (m_Header.s_nVersion == FILE_HASH_VERSION_SECURITY_NEW)
			{
				if (!DmFS::SecuritySelf::Decript_V18(DmFS::Enc::Kind::Shuffle | DmFS::Enc::Kind::RvsFull | DmFS::Enc::Kind::Twist | DmFS::Enc::Kind::XorTwist, data, ovec)) // XORTwist is actually NOT USED, but that's the argument it passed during RE
					return ERROR_HASH_FILE_OPEN;
			}
			else if (m_Header.s_nVersion == FILE_HASH_VERSION_SECURITY_NEW2)
			{
				if (!DmFS::SecuritySelf::Decript_V19(DmFS::Enc::Kind::XorTwist | DmFS::Enc::Kind::RvsFull | DmFS::Enc::Kind::Twist | DmFS::Enc::Kind::RvsHalf, data, ovec))
					return ERROR_HASH_FILE_OPEN;
			}
			else if (m_Header.s_nVersion == FILE_HASH_VERSION_DRPG)
			{
				if (!DmFS::SecuritySelf::Decript_V32(DmFS::Enc::Kind::Xor | DmFS::Enc::Kind::RvsHalf | DmFS::Enc::Kind::RvsFull, data, ovec))
					return ERROR_HASH_FILE_OPEN;
			}

			dm_bytevec r_ovec;

			if (!DmCS::CompressZip::UnCompress(ovec, r_ovec))
				return ERROR_HASH_FILE_OPEN;

			m_Header.s_nFileCount = *(int*)&r_ovec[0];

			// Existe uma nova estrutura s INFO, precisamos entendê-la agora...

			assert(__m_pDeletePointer == NULL);
			__m_pDeletePointer = (CsFileHash::sINFO*)calloc(m_Header.s_nFileCount, sizeof(sINFO));

			uint64_t ppos = 4;

			if (m_Header.s_nVersion == FILE_HASH_VERSION_DRPG)
			{

				for (auto i = 0; i < m_Header.s_nFileCount; i++)
				{
					size_t fileNameLen = *(uint32_t*)&r_ovec[ppos];
					fileNameLen *= sizeof(wchar_t);
					ppos += 4;
					std::wstring data;
					data.resize(fileNameLen + 1);
					memcpy_s(&data[0], data.size(), &r_ovec[ppos], fileNameLen);
					ppos += fileNameLen;

					__m_pDeletePointer[i].s_nOffset = *(uint64_t*)&r_ovec[ppos];
					ppos += 8;
					__m_pDeletePointer[i].s_nDataSize = *(uint32_t*)&r_ovec[ppos];
					ppos += 4;

					__m_pDeletePointer[i].s_nAllocSize = __m_pDeletePointer[i].s_nDataSize;
					__m_pDeletePointer[i].s_nHash = CsFPS::GetHashCode(data.c_str());
					__m_pDeletePointer[i].s_nPlag = PG_TYPE_DATA;

					m_vFileName32.insert_or_assign(__m_pDeletePointer[i].s_nHash, data);
				}
			}
			else
			{
				for (auto i = 0; i < m_Header.s_nFileCount; i++, ppos += 24)
				{
					sINFONew* n = (sINFONew*)&r_ovec[ppos];

					__m_pDeletePointer[i].s_nAllocSize = n->s_nAllocSize;
					__m_pDeletePointer[i].s_nDataSize = n->s_nDataSize;
					__m_pDeletePointer[i].s_nHash = n->s_nHash;
					__m_pDeletePointer[i].s_nOffset = n->s_nOffset;
					__m_pDeletePointer[i].s_nPlag = n->s_nPlag;
				}
			}

		}
		else
		{
			_close(nHandle);
			return ERROR_HASH_FILE_VERSION;
		}

		m_nAllocCount = m_Header.s_nFileCount;
		_PointerInsertMap();
		return ERROR_OK;
	}

	void CsFileHash::_PointerInsertMap()
	{
		m_mapHash.clear();
		m_vpDummy.clear();

		assert(m_nAllocCount >= m_Header.s_nFileCount);

		// coloque no mapa
		for (int i = 0; i < m_Header.s_nFileCount; ++i)
		{
			sINFO* pInfo = &__m_pDeletePointer[i];

			switch (pInfo->s_nPlag & PG_TYPE_MASK)
			{
			case PG_TYPE_DATA2:
			case PG_TYPE_DATA:
				m_mapHash[pInfo->s_nHash] = pInfo;
				break;

			case PG_TYPE_DUMMY:
				m_vpDummy.push_back(pInfo);
				break;

			default:
				assert(false);
				break;
			}
		}
	}


	UINT64 CsFileHash::CalOffset(size_t nData)
	{
		size_t nCount = m_vpDummy.size();
		if (nCount == 0)
			return FP_INVALIDE_SIZE;

		// Vamos encontrar o intervalo
		return 0;
	}

	UINT64 CsFileHash::CalOffset(LPCSTR cFilePath, size_t nData)
	{
		size_t nHash = GetHashCode(cFilePath);

		// Comparar o tamanho dos dados, se presente
		sINFO* pHashInfo = GetHashInfo(nHash);
		if (NULL == pHashInfo)
			return FP_INVALIDE_SIZE;

		// Se o tamanho dos dados adicionais for maior que o tamanho da alocação
		if (pHashInfo->s_nAllocSize < nData)
			return FP_CHANGE_SIZE;

		// Se existir e o tamanho adicional for igual ou menor, coloque-o nesse local
		return pHashInfo->s_nOffset;
	}

	void CsFileHash::SetAllocCount(int nAllocCount)
	{
		// ainda não cuidei
		assert(m_Header.s_nFileCount <= nAllocCount);
		if (m_Header.s_nFileCount < nAllocCount)
		{
			m_nAllocCount = nAllocCount;
			__m_pDeletePointer = (CsFileHash::sINFO*)realloc(__m_pDeletePointer, sizeof(sINFO) * m_nAllocCount);
			_PointerInsertMap();
		}
	}

	void CsFileHash::_AddFile(LPCSTR szPath, UINT64 nOffset, size_t nDataSize)
	{
		size_t hashCode = GetHashCode(szPath);
		if (0 == hashCode)
			return;

		++m_Header.s_nFileCount;

		assert(m_nAllocCount >= m_Header.s_nFileCount);

		sINFO* pData = &__m_pDeletePointer[m_Header.s_nFileCount - 1];
		pData->s_nPlag = PG_TYPE_DATA;
		pData->s_nHash = hashCode;
		pData->s_nOffset = nOffset;
		pData->s_nDataSize = nDataSize;
		pData->s_nAllocSize = nDataSize;

		//assert( IsExistHashCode( pData->s_nHash ) == false );  //TODO Linha descomentado
		m_mapHash[pData->s_nHash] = pData;
		//assert( pData->s_nPlag == PG_TYPE_DATA );		//TODO Linha descomentado
	}

	void CsFileHash::_EditFile(char const* szPath, UINT64 nOffset, size_t nDataSize)
	{
		size_t nHash = (size_t)GetHashCode(szPath);
		sINFO* pData = GetHashInfo(nHash);
		pData->s_nOffset = nOffset;
		pData->s_nDataSize = nDataSize;

		// O Alloc Size também é alterado para arquivos anexados no verso do Pack devido ao seu tamanho grande.
		if (pData->s_nAllocSize < nDataSize)
			pData->s_nAllocSize = nDataSize;

		assert(pData->s_nPlag == PG_TYPE_DATA);
	}

	//#include <dos.h>
	void CsFileHash::SaveFile()
	{
		if (_access_s(m_szFilePath.c_str(), 0) == 0)
		{
			DWORD modeBackup = GetFileAttributesA(m_szFilePath.c_str());
			DWORD mode = modeBackup;
			mode &= ~(FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM);
			SetFileAttributesA(m_szFilePath.c_str(), mode);
		}


		int nHandle;
		_sopen_s(&nHandle, m_szFilePath.c_str(), O_CREAT | O_RDWR | O_BINARY | O_TRUNC, _SH_DENYNO, _S_IWRITE | _S_IREAD);
		if (nHandle == FP_INVALIDE_HANDLE)
		{
			CsMessageBox( MB_OK, _T( "HashFile Error" ) );
			return;
		}

		_lseeki64(nHandle, 0, SEEK_SET);
		_write(nHandle, &m_Header, sizeof(FHHeader));

		std::map< size_t, sINFO* >::iterator it = m_mapHash.begin();
		for (; it != m_mapHash.end(); ++it)
			_write(nHandle, it->second, sizeof(sINFO));

		std::list<sINFO*>::iterator dumIT = m_vpDummy.begin();
		for (; dumIT != m_vpDummy.end(); ++dumIT)
			_write(nHandle, (*dumIT), sizeof(sINFO));
		_close(nHandle);
	}

	bool CsFileHash::IsExist(size_t nHash) const
	{
		std::map< size_t, sINFO* >::const_iterator it = m_mapHash.find(nHash);
		if (it == m_mapHash.end())
			return false;

		return true;
	}

	CsFileHash::sINFO* CsFileHash::GetHashInfo(size_t nHash)
	{
		std::map< size_t, sINFO* >::iterator it = m_mapHash.find(nHash);
		if (it == m_mapHash.end())
			return NULL;

		return it->second;
	}
}