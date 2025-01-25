


#include "stdafx.h"

#include "cNetwork.h"
#include "cCliAccount.h"

#include "nlib/cProfile.h"

#include "common_vs2022/pLogin.h"
#include "common_vs2022/pServer.h"
#include "common_vs2022/pLength.h"
#include "common_vs2022/pPass2.h"
#include "common_vs2022/cVersion.h"

#include "nlib/cSHA1.h"
#include "nlib/md5.h"

bool cCliAccount::SendLogin(char const* pUserID, char const* pUserPassword)
{
	std::string userID(pUserID);
	std::string userPass(pUserPassword);

	newp(pLogin::Request);
	push(g_nNetVersion);
	push(GLOBALDATA_ST.GetUserType());	// tipo de usuário

#ifdef VERSION_STEAM
	Steam::UserData sUserData;
	STEAM_ST.GetUserData( sUserData );

	std::wstring strSteamID;
	DmCS::StringFn::From( strSteamID, sUserData.m_cSteamID.ConvertToUint64() );
	DmCS::StringFn::ToMB( strSteamID, userID );
#endif

	push(userID);		// ID do usuário

#ifdef PASSWORD_ENCODE //Hong Kong/Taiwan usa apenas MD5
	std::cout << "Encoding user PW" << std::endl;
	char szFirst[2048] = {0,}; // 33 byte
	md5( const_cast<char*>(userPass.c_str()), szFirst );
	char szLast[Language::pLength::Md5] = {0,};	
	memcpy( szLast, szFirst, sizeof(szLast) * Language::pLength::Md5 );
	push( szLast, sizeof(char) * Language::pLength::Md5 );
#else
	u1 DummyByte = 0;
	push(DummyByte);
	push(userPass);
#endif		

	std::string szCpuName = nBase::GetCpuName();
	push(szCpuName);

	std::string szGpuName = nBase::GetGpuName();
	push(szGpuName);

	
	u4 nPhyMemory = (u4)(nBase::GetTotalMemory()/1024);
	push(nPhyMemory);

	std::string szOS = nBase::GetOS();
	push(szOS);

	std::string szDxVersion = nBase::GetDxVersion();
	push(szDxVersion);

	int nCodePage = GetACP();
	push(nCodePage);
	
 	if(GLOBALDATA_ST.IsCountry( GData::eCountry_Aeria ))
 	{
 		u1 szIsLogin = (u1)net::IsLogin;//O primeiro login é 0
 		push(szIsLogin);
 
 		char Auth_Code[Language::pLength::str*2] = {0,};
 		if(net::IsLogin == 0)//Uma vez logado 1
 		{
 			net::IsLogin =1;
 			std::string authCode = GLOBALDATA_ST.GetAuthCode();
 			authCode.copy( Auth_Code, sizeof(char)*(Language::pLength::str*2) );			
 		}
 		else
 			//strcpy_s( Auth_Code, sizeof( Auth_Code ), net::cAccessToken );
 
 		push(Auth_Code, sizeof(Auth_Code));
 	}

#ifdef VERSION_STEAM
	push( sUserData.m_cSteamID.ConvertToUint64() );
	push( sUserData.m_nTokenLen );
	push( sUserData.m_rgchToken, sizeof( sUserData.m_rgchToken ) );
#endif

	endp(pLogin::Request);
	DoSend();

	cPrintMsg::PrintMsg( 10019 );

	return true;
}

bool cCliAccount::SendLogin()
{
#ifdef DEBUG_NETWORK
	DBG("Sending login with ID -> %s and PW -> %s", GLOBALDATA_ST.GetAccountID().c_str(), GLOBALDATA_ST.GetAccountPW().c_str());
#endif
	return SendLogin( GLOBALDATA_ST.GetAccountID().c_str(), GLOBALDATA_ST.GetAccountPW().c_str() );	
}

bool cCliAccount::SendSelectCluster(UINT const& nSendServerIdx)
{
	newp(pSvr::SelectCluster);
		push(nSendServerIdx); // gate svr idx
	endp(pSvr::SelectCluster);	
	send();

	return true;
}


void cCliAccount::SendReqClusterList(void)
{
// 	static uint s_nLastReqTick = 0;
// 	if(s_nLastReqTick > nBase::GetTickCount())
// 		return;
// 
// 	s_nLastReqTick = nBase::GetTickCount() + 3939;
#ifdef DEBUG_NETWORK
	DBG("Send ReqClusterList\n");
#endif
	newp(pSvr::ClusterList);
	endp(pSvr::ClusterList);
	send();
#ifdef DEBUG_NETWORK
	DBG("Send ReqClusterList135\n");
#endif
}

bool cCliAccount::SendRegister2ndPass(char *pass)	// 2º cadastro de senha
{
	SAFE_POINTER_RETVAL( pass, false );

	char pass2[Language::pLength::SecondPass + 1] = {0,};
	char LastPass[Language::pLength::SecondPass + 1] = {0,};
	md5( pass, pass2 );
	memcpy(LastPass,pass2,sizeof(char)*Language::pLength::SecondPass);	

	newp(pPass2::Register);
		push(LastPass, Language::pLength::SecondPass);
	endp(pPass2::Register);
	DoSend();

	return true;
}

void cCliAccount::SendChange2ndPass(char *szPrevPass, char *szNextPass)		// 2ª troca de senha
{
	SAFE_POINTER_RET( szPrevPass );
	SAFE_POINTER_RET( szNextPass );

	int nPrevPassLength = strlen(szPrevPass);
	int nNextPassLength = strlen(szNextPass);

	xstop(nPrevPassLength>=4 && nPrevPassLength<=8, "A senha (anterior) nao atende as especificacoes.");
	xstop(nNextPassLength>=4 && nNextPassLength<=8, "A senha (seguinte) nao esta de acordo com a especificacao.");

	char nLastPrev[Language::pLength::SecondPass + 1] = {0,};
	char pPrevPass[Language::pLength::SecondPass + 1] = {0,};
	md5( szPrevPass, pPrevPass );
	memcpy(nLastPrev,pPrevPass,sizeof(char)*Language::pLength::SecondPass);	

	char nLastNext[Language::pLength::SecondPass + 1] = {0,};
	char pNextPass[Language::pLength::SecondPass + 1] = {0,};
	md5( szNextPass, pNextPass );
	memcpy(nLastNext,pNextPass,sizeof(char)*Language::pLength::SecondPass);	

	newp(pPass2::ChangePass);
		push(nLastPrev, Language::pLength::SecondPass);// 2º passe usado antes
		push(nLastNext, Language::pLength::SecondPass);// 2ª passagem para novo uso
	endp(pPass2::ChangePass);
	DoSend();

	return;
}

void cCliAccount::SendSecondPassCertified( char const *pass)
{
	SAFE_POINTER_RET( pass );

	char LastPass[Language::pLength::SecondPass + 1] = {0,};
	char __pass2[Language::pLength::SecondPass + 1] = {0,};
	md5( const_cast<char*>(pass), __pass2 );
	memcpy(LastPass,__pass2,sizeof(char)*Language::pLength::SecondPass);	

	newp(pPass2::Check);
		push((u2) nPass2::Certified );
		push(LastPass, Language::pLength::SecondPass);
	endp(pPass2::Check);
	DoSend();
}

void cCliAccount::SendSkipSecondPass(void)
{
	char empty[Language::pLength::SecondPass + 1] = {0,};
	newp(pPass2::Check);
	push((u2) nPass2::Skiped );
	push(empty, Language::pLength::SecondPass);
	endp(pPass2::Check);
	DoSend();
}

void cCliAccount::NTF_Send_nProtect_BotDetect( UINT unUniqueIdx, BYTE* byBotPacket, DWORD dwSize)
{
#ifdef DEF_CORE_NPROTECT
 	bool bSendBlock = cClient::IsSendBlock();
 	cClient::SetSendBlock(false);
 
 	BYTE bPacket[4096] = {0,};
 	memcpy(bPacket, byPacket, dwSize*sizeof(char));
 
 	newp( pGameTwo::GameGuardCheck );
  push( unUniqueIdx ); 
 	push( bPacket );
 	push( dwSize );
 	endp( pGame::GameGuardCheck);
 	send();
 
 	cClient::SetSendBlock( bSendBlock );	
#endif
}

void cCliAccount::ACK_Send_nProtect( UINT unUniqueIdx, BYTE* byPacket, DWORD dwSize )
{
#ifdef DEF_CORE_NPROTECT
 	bool bSendBlock = cClient::IsSendBlock();
 	cClient::SetSendBlock(false);
 
 	BYTE bPacket[4096] = {0,};
 	memcpy(bPacket, byPacket, dwSize*sizeof(char));
 
 	newp( pGameTwo::GameGuardCheck );
  push( unUniqueIdx ); 
 	push( bPacket );
 	push( dwSize );
 	endp( pGame::GameGuardCheck);
 	send();
 
 	cClient::SetSendBlock( bSendBlock );		
#endif
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
void cCliAccount::NTF_Send_Xigncode( char* pBuffer, SIZE_T BufferSize )
{
#ifdef SDM_DEF_XIGNCODE3_20181107
	 	C2GS_SEND_XignCodePacket sendPacket;
	 	sendPacket.nSize = BufferSize;
	 	memcpy( sendPacket.szPacketData, pBuffer, BufferSize );
	 
	 	newp( sendPacket.GetProtocol() );
	 	push( sendPacket.szPacketData, BufferSize );
	 	push( sendPacket.nSize );
	 	endp( sendPacket.GetProtocol() );
	 	send();
#endif
}