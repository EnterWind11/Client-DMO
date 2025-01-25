

#pragma once

#include "nlib/cNetBase.h"
#include "common_vs2022/pLength.h"

extern bool IsNetOff(void);
extern void SetWindowTitle(int nIsOnline, int nMapID=-1);

#include "cCliAccount.h"
#include "cCliGate.h"
#include "cCliGame.h"

void CHAT_LOG(char *msg);
void CHAT_LOG(TCHAR *msg);
#define CHAT(x) CHAT_LOG(x)

namespace Cmd
{
	enum
	{
		None,
		ConnectGameServer,
		ConnectGateServer,
		ConnectAccountServer,
	};
};


namespace net
{
	namespace type
	{
		enum
		{
			game = 10,
			gate,
			account,
			none,
		};
	};


	extern cCliGame *game;
	extern cCliGame *game_tmp;

	extern cCliGate *gate;
	extern cCliGate *gate_tmp;

	extern cCliAccount *account;
	extern cCliAccount *account_tmp;

	extern cClient *net;
	extern bool receive_active;
	
	extern char ip[Language::pLength::ip+1];
	extern uint port;	// tmp ip and port
	extern uint cmd;

	extern uint cluster;

 	extern uint ssn1;
 	extern uint ssn2;	

	extern uint nIsMoveMapByCommand;

	inline bool IsMoveMapByCommand(void) { return nIsMoveMapByCommand ? true : false; }

	void start(void);
	void end(void);

	void load(void);
	void stop(void);
	void execute(void);
	void exit(uint code);

	bool start(int type, char const*ip, uint port);	


#ifdef DEF_CORE_NPROTECT
	void nProtect_BotDetect( UINT unUniqueIdx, const BYTE* Buffer, DWORD dwBufferSize );
	void nProtect_Send( UINT unUniqueIdx, const BYTE* Buffer, DWORD BufferSize ,DWORD dwGameServerIndex );
	void nProtect_BotDetect();
	void nProtect_Send();
#endif

	void Xigncode_SendPacket(char* Buffer, SIZE_T BufferSize);

	void chat(char *fmt, ...); //Saída somente quando há uma janela de bate-papo
	void chat(int type, char *fmt, ...); // Saída somente quando há uma janela de bate-papo



//////////////////////////////////////////////////
// current user's info
	extern uint account_idx; // current user's idx
	extern uint access_code; // user access code

	extern char id[Language::pLength::id + 1]; // current user's id and pass
	extern char pass[Language::pLength::pass+ 1];
	extern char secondPass[Language::pLength::pass+ 1];	// temporariamente armazenado aqui
	extern char MailAddress[Language::pLength::email + 1];	// Endereço de e-mail do usuário, tipo de string

	extern char szUsrType[8];

	//AERIA
	extern char Auth_Code[Language::pLength::str*2];				// binary Type
	extern char cAccessToken[Language::pLength::AccessToken + 1];	// string Type
	extern char cTicketingtime[MAX_ANAL_LEN];						// string Type
	extern bool IsAeria;
	extern uint IsLogin;
	//AERIA

 	extern bool IsWebLoginVersion;
 
 	static bool IsWebLogin()
 	{
 		return IsWebLoginVersion;		
 	}	
 
 	extern int UsrType;
 
 	static bool IsChannelingConnet()
 	{
 		switch( UsrType )
 		{
 		case nLoginType::PlayNet:	// Usuário do PlayNet		
 		case nLoginType::TooNiland:	// Tuni Land User
 		case nLoginType::IMBC:		// Usuário MBC Games
 		case nLoginType::MGAME:		// usuários MGAME
 			return true;
 		}
 
 		return false;
 	}

//////////////////////////////////////////////////
// next map info
	extern uint next_map_no;
	extern uint prev_map_no;
	extern bool bPortalMove;
	extern bool bChangeChannel;

	//extern char account_ip[Language::pLength::ip+1];
	//extern uint account_port;

//////////////////////////////////////////////////
// network state
	extern int  svr_type;		
	//extern bool disable;	// network disable
};




namespace nErr
{
	enum
	{
		Bind = 199,
		Connect,
		Run,
	};
};





