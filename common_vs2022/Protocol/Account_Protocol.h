#pragma once

#include "Account_Define.h"
#include <list>
#include <minwindef.h>

/************************************************************************/
/* Server List Information Packet                                       */
/************************************************************************/
class GS2C_RECV_SERVERLIST
{
public:
	GS2C_RECV_SERVERLIST() : m_dwResult(0) {}
	~GS2C_RECV_SERVERLIST() {}

public:
	n4                      m_dwResult;         // Result code
	std::list<sServerInfo>  m_listServer;       // List of server information
};


/************************************************************************/
/* Character List Information Packet                                    */
/************************************************************************/
class GS2C_RECV_CHARACTER_LIST
{
public:
	GS2C_RECV_CHARACTER_LIST() : m_dwResult(0) {}
	~GS2C_RECV_CHARACTER_LIST() {}

public:
	n4                                  m_dwResult;         // Result code
	std::list<sSimpleCharacterInfo>     m_listCharacterInfo;// List of character info
};


/************************************************************************/
/* Newly Created Character Information Packet                           */
/************************************************************************/
class GS2C_RECV_NEW_CHARACTER_INFO
{
public:
	GS2C_RECV_NEW_CHARACTER_INFO() : m_dwResult(0) {}
	~GS2C_RECV_NEW_CHARACTER_INFO() {}

public:
	n4                      m_dwResult;         // Result code
	sSimpleCharacterInfo    m_NewCharInfo;      // Info of the newly created character
};


/************************************************************************/
/* Second Password Information Packet                                   */
/************************************************************************/
class GS2C_RECV_CHECK_SECONDPASSWORD
{
public:
	GS2C_RECV_CHECK_SECONDPASSWORD()
		: m_nResult(0)
		, m_u2SecondPasswordType(0)
	{
	}
	~GS2C_RECV_CHECK_SECONDPASSWORD() {}

public:
	n4  m_nResult;                  // Result code
	u2  m_u2SecondPasswordType;     // Indicates the second password type/status
};


/************************************************************************/
/* Login Request Response Packet                                        */
/************************************************************************/
class GS2C_RECV_LOGIN_REQUEST
{
public:
	GS2C_RECV_LOGIN_REQUEST()
		: nProtocol(pLogin::Request)
		, nResult(0)
		, nSubType(0)
	{
	}

	WORD GetProtocol() { return nProtocol; }

public:
	WORD nProtocol;     // Protocol code
	u4   nResult;       // Result code
	u1   nSubType;      // Subtype or additional status
};


/************************************************************************/
/* Second Password Change Response Packet                               */
/************************************************************************/
class GS2C_RECV_2ndPass_Change
{
public:
	GS2C_RECV_2ndPass_Change()
		: nProtocol(pPass2::ChangePass)
		, nResult(0)
	{
	}

	WORD GetProtocol() { return nProtocol; }

public:
	WORD nProtocol; // Protocol code
	u4   nResult;   // Result code
};


/************************************************************************/
/* Second Password Verification Response Packet                         */
/************************************************************************/
class GS2C_RECV_2ndPass_Certify
{
public:
	GS2C_RECV_2ndPass_Certify()
		: nProtocol(pPass2::Check)
		, nResult(0)
	{
	}

	WORD GetProtocol() { return nProtocol; }

public:
	WORD nProtocol; // Protocol code
	u4   nResult;   // Result code
};


/************************************************************************/
/* Second Password Registration Packet                                  */
/************************************************************************/
class GS2C_RECV_2ndPass_Register
{
public:
	GS2C_RECV_2ndPass_Register()
		: nProtocol(pPass2::Register)
		, nResult(0)
	{
	}

	WORD GetProtocol() { return nProtocol; }

public:
	WORD nProtocol; // Protocol code
	u4   nResult;   // Result code
};


/************************************************************************/
/* Resource-Based Hash Value Transmission Packet                        */
/************************************************************************/
class GS2C_RECV_Resource_Hash
{
public:
	GS2C_RECV_Resource_Hash()
		: nProtocol(pCertify::IntegrityHash)
	{
	}

	WORD GetProtocol() { return nProtocol; }

public:
	WORD        nProtocol;  // Protocol code
	std::string szHash;     // Resource hash value
	// std::string szURL;    // Uncomment if needed
};


/************************************************************************/
/* Account Ban Time Transfer Packet                                     */
/************************************************************************/
class GS2C_RECV_Account_Ban
{
public:
	GS2C_RECV_Account_Ban()
		: nProtocol(pLogin::AccountBan)
		, nTimeLeft(0)
	{
	}

	WORD GetProtocol() { return nProtocol; }

public:
	WORD        nProtocol;  // Protocol code
	u4          nTimeLeft;  // Remaining ban time
	std::string szReason;   // Reason for ban
};


/************************************************************************/
/* Shutdown System: Send Birth Date                                     */
/************************************************************************/
class GS2C_RECV_Shutdown_Birthday
{
public:
	GS2C_RECV_Shutdown_Birthday()
		: nProtocol(pLogin::Birthday)
	{
	}

	WORD GetProtocol() { return nProtocol; }

public:
	WORD        nProtocol;  // Protocol code
	std::string szBirthDay; // Birth date string (e.g., YYYY-MM-DD)
};


/************************************************************************/
/* Result Packet for Server Connection Request                          */
/************************************************************************/
class AC2C_RECV_SERVERSELECT_CLUSTER
{
public:
	AC2C_RECV_SERVERSELECT_CLUSTER()
		: nResult(0)
	{
	}
	~AC2C_RECV_SERVERSELECT_CLUSTER() {}

public:
	n4 nResult; // Result code
};
