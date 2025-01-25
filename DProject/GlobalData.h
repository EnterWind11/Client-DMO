//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------

namespace GData
{
	enum eCountryType
	{
		eCountry_None,
		eCountry_Kor,
		eCountry_GSP,
		eCountry_Aeria,
		eCountry_Steam,
		eCountry_Hongkong,
		eCountry_Thailand,
		eCountry_Taiwan,
		eCountry_Brasil,
	};

	enum eLanguageType
	{
		eLanguage_None,
		eLanguage_Kor,
		eLanguage_Eng,
		eLanguage_Thailand,
		eLanguage_Hongkong,
		eLanguage_Taiwan,
		eLanguage_Brasil,
	};

	enum e2ndPassType
	{
		eNone,
		eEmail,
		eAccountPass,
		e2ndNumberPass,
		eStringPass,
	};

	struct sServerDataInfo
	{
		sServerDataInfo():m_nServerIDX(0),m_nHaveCharacterCount(0),m_nMaxCharacterSlotCount(0),m_nLockSlotCount(0)
		{}

		~sServerDataInfo() {};
		int				m_nServerIDX;
		std::wstring	m_ServerName;				// nome do servidor
		unsigned short	m_nHaveCharacterCount;		// número de personagens criados
		unsigned short	m_nMaxCharacterSlotCount;	// Número máximo de caracteres que podem ser criados
		unsigned short	m_nLockSlotCount;			// número de slots bloqueados
	};

	struct sConnetServerInfo
	{
		sConnetServerInfo():m_nServerIDX(0)
		{}

		~sConnetServerInfo() {};

		int				m_nServerIDX;
		std::wstring	m_ServerName;				// nome do servidor
	};

	class CGlobalData : public CSingleton<CGlobalData>
	{
	public:
		CGlobalData();
		~CGlobalData();

		void			Init();
		void			Clear2ndPass();

		std::string		GetAccountIP() const;
		unsigned int	GetAccountPort() const;

		void			SetLanguage(eLanguageType eType);
		eLanguageType	GetLanguageType() const;
		std::string		GetSystemFontFileName() const;
		std::string		GetTextFontFileName() const;

		void			SetConnectionCountryType(eCountryType eType);// configuração do país
		eCountryType	GetConnectionCountryType(void) const;
		std::string		GetCountryName(void) const;
		bool			IsCountry(eCountryType eType) const;
		bool			IsLoginSkiped(void) const;				// Verifique a capacidade de pular a página de login por área de serviço.

		std::string		GetLoadTableFile(void) const;			// Nome do arquivo da tabela do Excel por área de serviço
		std::string		GetLoadResourceFile(void) const;		// Nome do arquivo de recurso por área de serviço

		void			SetAuthCode(std::string const& authCode);
		std::string		GetAuthCode(void) const;

		void			SetAccountID(std::string const& auID);
		std::string		GetAccountID(void) const;
		std::wstring	GetAccountIDW(void) const;
		void			ResetAccountInfo(void);

		void			SetAccountPW(std::string const& auPW);
		std::string		GetAccountPW(void) const;

		void			SetUserType(std::string const& cChannelName);
		std::string		GetUserType(void) const;
		bool			IsChannelingService(void) const;

		void			Set2ndPassType(e2ndPassType nType);
		e2ndPassType	Get2ndPassType(void) const;
		bool			Is2ndPassType(e2ndPassType nType);

		void			Set2ndPassword(std::string const& strPass);
		std::string		Get2ndPassword(void) const;
		void			SetSkip2ndPass(void);
		bool			Is2ndPassSkiped();
		bool			Is2ndPassInputPass() const;// Posso pular a digitação da 2ª senha?
		bool			Is2ndPassEncry() const;

		std::wstring	GetCashShopWebUrl(void) const;
		std::wstring	GetNoticeWebUrl(void) const;
		std::string		GetCashChargeUrl(void) const;
		std::wstring	GetWebCashPageUrl(void) const;
		std::wstring	GetHomePageUrl(void) const;


		void			ClearServerListInfo();
		void			AddServerListInfo(int const& nServerIdx,
			std::wstring const& serverName,
			unsigned short const& nHaveCharacterCount,
			unsigned short const& nMaxCharacterCount,
			unsigned short const& nOpenedSlotCount);
		void			SetConnetServerInfo(int const& nServerIdx, std::wstring const& serverName);

		int				GetCurrentServerIDX() const;
		std::wstring	GetCurrentServerName() const;

		// Retorna o número de slots de caracteres que podem ser criados nesse servidor como um índice de servidor.
		// Número total de slots de personagem - (número de slots bloqueados + número de slots criados)
		int				GetEmptySlotCountFromServer(int const& nServerIDX) const;

		std::list<sServerDataInfo> const& GetServerListInfo() const;
		int				GetCurrentServerLockSlotCount() const;

		void			GetServerSlotCountInfo(int const& nServerIDX, int& nOpenSlotCount, int& nLockSlotCount);

		void			LoadInfo_AccountIP_Port();

		void			SetDownloadPatchVersion(int const& nVersion);
		int				GetDownloadPatchVersion() const;

		std::string		GetAccessToken(void) const;
		std::string		GetMailAddress(void) const;
		std::string		GetTicketingtime(void) const;

		void			SetTicketingtime(std::string const& ticketingTime);
		void			SetMailAddress(std::string const& mailAddress);
		void			SetAccessToken(std::string const& acctoken);

		void			ToggleLiveServer();
	 
	private:
		void			_SetNoticeWebUrl(eCountryType const& eType);
		void			_Init2ndPasswordType(eCountryType const& eType);
		void			_SetAccountIP_Port(eCountryType const& eType);
	
	private:
		bool			bWasManuallySet;
		int				m_nDownLoadVersion;
		eCountryType	m_nConnectionCountryType;	// área de acesso
		eLanguageType	m_nLanguageType;			// tipo de idioma

		std::string		m_Account_IP;
		unsigned int	m_Account_Port;

		std::string		m_strAccountID;		// ID da conta
		std::string		m_strAccountPW;		// Senha da conta

		e2ndPassType	m_n2ndPassType;		// 2nd PasswordType
		std::string		m_str2ndPass;		// 2nd Password

		std::string		m_LoadTableFileName;
		std::string		m_LoadResourceFileName;

		std::string		m_Auth_Code;
		std::string		m_cAccessToken;
		std::string		m_cMailAddress;
		std::string		m_cTicketingtime;
		
		std::string		m_cUserType;

		std::wstring	m_wsCashShopWebUrl;
		std::wstring	m_wsNoticeWebUrl;
		std::string		m_cCashChargeUrl;
		std::wstring	m_wsWebCashPageUrl;
		std::wstring	m_wsHomePageWebUrl;
		std::string		m_wsHashDataUrl;

		// Posso pular a digitação da 2ª senha?
		// Atualmente aplica-se apenas a países GSP.
		// Se outros países inserirem a função de entrada de senha secundária 
		// Definido por país
		bool			m_b2ndPasswordInputPassUsing;
		std::list<sServerDataInfo> m_ServerDataList;
		sConnetServerInfo		m_ConnetServerInfo;
	};
};

// Singleton
#define GLOBALDATA_ST			GData::CGlobalData::GetSingleton()
#define GLOBALDATA_STPTR		GData::CGlobalData::GetSingletonPtr()
//---------------------------------------------------------------------------