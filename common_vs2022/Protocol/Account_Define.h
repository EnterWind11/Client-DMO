#pragma once
#include "../pTamer.h"
#include "../pLimit.h"
#include "../cItemData.h"
#include "../pLogin.h"
#include "../pPass2.h"
#include "../ccertify.h"

struct sServerInfo
{
public:
	enum eSTATE {
		RUNNING = 0,    // Server running
		REPAIR,         // Server under maintenance
		READY,          // Server ready
	};

	enum eCongestion {
		eComfortable,   // Comfortable
		eNormal,        // Normal
		eCrowded,       // Crowded
		eFull,          // Full
	};

	sServerInfo()
		: m_nIDX(0), m_nDisable(0), m_nClusterState(0),
		m_nTamerNumber(0), m_bNewServer(false),
		m_nMaxTamerSlot(0), m_nOpenTamerSlot(0)
	{
	};

	~sServerInfo() {};

public:
	n4              m_nIDX;             // Server index number
	u1              m_nDisable;         // Server availability: 0 = running, 1 = under maintenance, 2 = standby (eSTATE)
	u1              m_nClusterState;    // 0 = comfortable, 1 = normal, 2 = crowded, 3 = full (eCongestion)
	u1              m_nTamerNumber;     // Number of owned characters
	bool            m_bNewServer;       // Whether it is a new server
	std::wstring    m_szClusterName;    // Server name
	u1              m_nMaxTamerSlot;    // Maximum number of tamer slots available
	u1              m_nOpenTamerSlot;   // Number of tamer slots in use
};

struct sSimpleCharacterInfo
{
	sSimpleCharacterInfo():m_nMapID(0), m_nTamerObjectTBIdx(0), m_nTamerLevel(1)
		,m_nDigimonObjectTBIdx(0),m_nDigimonLevel(1),m_nDigimonScale(0),m_nPat_Leader(0),m_nSlotNumber(-1)
		,m_nRelocateState( 0 )
	{};

	~sSimpleCharacterInfo()
	{};

public:
	n1				m_nSlotNumber;			// número do slot
	n2				m_nMapID;				// último número de mapa salvo
	n4				m_nTamerObjectTBIdx;	// Índice da tabela de objetos Tamer
	u1				m_nTamerLevel;			// nível mais doméstico
	u4				m_nDigimonObjectTBIdx;	// Índice da tabela de objetos Digimon 
	u1				m_nDigimonLevel;		// nível digimon
	n2				m_nDigimonScale;		// escala digimon
	u2				m_nPat_Leader;			// índice de gordura
	cItemData		m_TamerEquip[nLimit::Equip];	// Informações do equipamento Tamer
	std::wstring	m_szTamerName;			// nome domador
	std::wstring	m_szDigimonName;		// nome do digimon
	n4				m_nRelocateState;		// estado pré-domador
};


namespace nsLogin
{
	enum eLOGINSUBTYPE {
		NONE = 0,
		LOGIN = 1,                 // Continue with login process
		CHECK2NDPASS = 2,          // Second password verification
		SET2NDPASS = 3,            // Second password setup

		MAX = 255
	};
}
