
#pragma once 

class CsBase
{
public:
	CsBase() {};
	~CsBase() {};

	struct sINFO
	{
		DWORD		s_dwID;

		USHORT		s_nLevel;
		__int64		s_dwExp;
		int			s_nHP;
		int			s_nDS;
		USHORT		s_nMoveSpeed;
		USHORT		s_nDefence;
		USHORT		s_nEvasion;
		USHORT		s_nCritical;
		USHORT		s_nAttack;
		USHORT		s_nHitRate;
	};

protected:
	sINFO			m_Info;
public:
	void			Delete();
	void			Init( sINFO* pSrcInfo );

	sINFO*			GetInfo(){ return &m_Info; }
};

class CsBaseMapInfo
{
public:
	struct sINFO
	{
		DWORD		s_nMapID;
		DWORD		s_nShoutSec;
		bool		s_bEnableCheckMacro;
	};
public:
	CsBaseMapInfo() {};
	~CsBaseMapInfo() {};

protected:
	sINFO			m_Info;
public:
	void			Delete(){}
	void			Init( sINFO* pSrcInfo ){ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }

	sINFO*			GetInfo(){ return &m_Info; }
};

class CsBaseJumpBuster
{
protected:
	std::map< DWORD, DWORD >		m_mapJumpMapID;

public:
	CsBaseJumpBuster() {};
	~CsBaseJumpBuster() {};

	void			Delete();
	void			Insert( DWORD nDestMapID );

	bool			IsMap( DWORD nDestMapID ){ return ( m_mapJumpMapID.find( nDestMapID ) != m_mapJumpMapID.end() ); }

	std::map< DWORD, DWORD >*	GetMap(){ return &m_mapJumpMapID; }
};

struct sBaseEvolutionApply
{
public:
	sBaseEvolutionApply():m_nApplyValue(0) {};
	~sBaseEvolutionApply() {};
	
	std::wstring		m_EvolutionTypeName;
	int					m_nApplyValue;	// Porcentagem de habilidade aplicada
};

#ifdef GUILD_RENEWAL

class CsGuild_Skill
{
public:
	CsGuild_Skill(){}
	~CsGuild_Skill(){}

	enum eSkillType{ BASE = 0, TIME = 1, NONE = 2, };
	struct sINFO
	{
		USHORT		s_nSkillType;		// 0: Habilidade básica, 1: Habilidade temporária
		USHORT		s_nGroup;			// 0: habilidade básica, 1 ~: mesmo número, mesmo grupo
		USHORT		s_nSkillLevel;		// Nível de Habilidade (Nível)
		UINT		s_nConsGsp;			// Gsp de consumo (ponto de habilidade da guilda)
		USHORT      s_Unknow;
		USHORT		s_nReqLevel;		// Nível de Guilda Necessário
		USHORT      s_nConsGsp1;
		UINT        s_unknowvalue;
		UINT		s_nSkillCode;		// código de habilidade
		TCHAR		s_szSkillName[ MAX_FILENAME ];	// nome da habilidade
		TCHAR		s_szSkillComment[ 128 ];		// descrição da habilidade
	};

protected:
	std::list< sINFO* >	m_pListInfo;

public:
	void			Delete(){}
	void			InsertList( sINFO* pSrcInfo ){ m_pListInfo.push_back( pSrcInfo ); }

	std::list< sINFO* >*	GetList() { return &m_pListInfo; }

};
#endif