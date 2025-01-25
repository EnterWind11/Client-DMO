


#pragma once 

#define TAMER_MAX_SKILL_COUNT		5

#define TAMER_COMMENT_LEN		512

class CsTamer
{
public:
	typedef	std::map< DWORD, CsTamer* >				MAP;
	typedef	std::map< DWORD, CsTamer* >::iterator	MAP_IT;

public:
	CsTamer(){}

	// ==== Info
public:
	struct sINFO
	{
		struct sSKILL
		{
			DWORD	s_dwID;
		};

		DWORD				s_dwTamerID;
		TCHAR				s_szName[ MAX_FILENAME ];
		char				s_cSoundDirName[ MAX_FILENAME ];

		// 케릭터 타입
		BYTE				s_nTamerType;

		// 스킬
		sSKILL				s_Skill[ TAMER_MAX_SKILL_COUNT ];

		// 설명
		TCHAR				s_szComment[ TAMER_COMMENT_LEN ];
		// 소속 
		TCHAR				s_szPart[ MAX_FILENAME ];
#ifdef COSTUME_GENDER		// 성별 문자열 추가 2014/02/11 kimhoshok		
		// 성별 
		TCHAR				s_szGender[ MAX_FILENAME ];
#endif
	};	
protected:
	sINFO				m_Info;
public:
	sINFO*				GetInfo(){ return &m_Info; }


	// ==== Base
public:
	void				Delete(){}
	void				Init( sINFO* pSrcInfo ){ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }
};


#define EMOTION_STR_LEN			16
#define EMOTION_CMD_COUNT		5

class CsEmotion
{
public:
	typedef	std::map< DWORD, CsEmotion* >				MAP;
	typedef	std::map< DWORD, CsEmotion* >::iterator		MAP_IT;

public:
	CsEmotion(){}

	// ==== Info
public:
	struct sINFO
	{
		DWORD				s_nID;

		TCHAR				s_szName[ EMOTION_STR_LEN ];		
		TCHAR				s_szCmd[ EMOTION_CMD_COUNT ][ EMOTION_STR_LEN ];
		
		DWORD				s_nAniID;
		BYTE				s_nUseCmdCount;
	};	
protected:
	sINFO				m_Info;
public:
	sINFO*				GetInfo(){ return &m_Info; }


	// ==== Base
public:
	void				Delete(){}
	void				Init( sINFO* pSrcInfo ){ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }
};





