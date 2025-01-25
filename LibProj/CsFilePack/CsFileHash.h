
#pragma once 
#pragma pack( push, 4 )

namespace CsFPS
{
	class CsFileHash
	{
	public:

		struct sINFO
		{
			sINFO():s_nPlag(0),s_nDataSize(0),s_nAllocSize(0),s_nHash(0),s_nOffset(0)
			{};

			int				s_nPlag;
			size_t			s_nDataSize;				// Tamanho dos dados reais, exceto o cabeçalho do pedaço (pode ser menor se for dados variáveis ​​-additionais)
			size_t			s_nAllocSize;				// Tamanho dos dados reais, exceto o cabeçalho do pedaço (fixo -fixado pelo tamanho correspondente durante o add inicial)
			size_t			s_nHash;			
			UINT64			s_nOffset;					// Offset de dados reais cruzando o cabeçalho
		};

		struct sINFONew
		{
			size_t			s_nHash;
			int				s_nPlag;
			UINT64			s_nOffset;
			size_t			s_nDataSize;				// Tamanho dos dados reais, exceto o cabeçalho do pedaço (pode ser menor se for dados variáveis ​​-additionais)
			size_t			s_nAllocSize;				// Tamanho dos dados reais, exceto o cabeçalho do pedaço (fixo -fixado pelo tamanho correspondente durante o add inicial)
		};

		CsFileHash();
		~CsFileHash();
		
	private:
		enum ePLAG
		{
			PG_TYPE_DATA2	= 0x00000000, // from 0x19
			PG_TYPE_DATA	= 0x00000001,
			PG_TYPE_DUMMY	= 0x00000002,
			PG_TYPE_MASK	= 0x000000ff,
		};
	public:
		void			Delete();
		
		void			New( LPCSTR szHashPath );
		DWORD			Load( LPCSTR szHashFile );
		
		std::string		GetPath(){ return m_szFilePath; }
		FHHeader*		GetHeader(){ return &m_Header; }

		std::map< size_t, sINFO* >*		GetHashMap(){ return &m_mapHash; }
		std::list< sINFO* >*			GetDummyVector(){ return &m_vpDummy; }

		UINT64			CalOffset( size_t nData );
		UINT64			CalOffset( LPCSTR cFilePath, size_t nData );
		size_t			GetTotalFileCount();

		bool			IsExist( size_t nHash ) const;

		sINFO *			GetHashInfo( size_t nHash );

		void			SetAllocCount( int nAllocCount );
		void			SaveFile();

		// DIGIMON RPG 0x20
		bool			HaveChunk() const { return m_Header.s_nVersion != FILE_HASH_VERSION_DRPG; }
		auto			GetFileNames32() const { return m_vFileName32; }

	private:
		friend class CsFilePack;
		void			_AddFile( LPCSTR szPath, UINT64 nOffset, size_t nDataSize );
		void			_EditFile( LPCSTR szPath, UINT64 nOffset, size_t nDataSize );
		void			_PointerInsertMap();

	protected:
		std::string					m_szFilePath;
		FHHeader					m_Header;
		sINFO*						__m_pDeletePointer;						// Arranjo para a memória para remover a memória
		int							m_nAllocCount;
		std::map< size_t, sINFO* >	m_mapHash;
		std::list< sINFO* >			m_vpDummy;

		std::map<size_t, std::wstring> m_vFileName32; //  FILE NAME TABLE
	};
}

#pragma pack( pop )