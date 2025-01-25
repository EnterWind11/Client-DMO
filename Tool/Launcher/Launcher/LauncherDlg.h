//LauncherDlg.h: arquivo de cabeçalho
//

#pragma once

enum eSTATE { eMOUSE_DEFAULT = 0, eMOUSE_ON, eMOUSE_DOWN, eMOUSE_DISABLE };

#include "MyHtmlView.h"
#include "IniFile.h"
#include "UseRegistry.h"
#include "MyMessageBox.h"
#include "ServerCheck.h"

//Caixa de diálogo CLauncherDlg
class cProgressbar;
class cBmpButton;
class cStrButton;
class CLauncherDlg : public CDialog
{
//criação.
public:
	CLauncherDlg(CWnd* pParent = NULL);//Este é o construtor padrão.

//dados da caixa de diálogo.
	enum { IDD = IDD_LAUNCHER_DIALOG };	
	enum eGameState { eINSTALL = 0, eUPDATE, ePACK, eRUN };

public:	
	BOOL		Init( CString const& strCurrentPath, WORD const& wLauncherCurrentVersion, sConfiguration const& info );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);//Suporte DDX/DDV.
	virtual BOOL DestroyWindow();	


//Implementação.
protected:
//Função de mapa de mensagem gerada
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnKickIdle(WPARAM wParam, LPARAM lParam); 	
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);	

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg
		void CreateIni();
	void DeleteIni();
	void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	void StartTemporay();

	DECLARE_MESSAGE_MAP()	
protected:
	virtual void OnOK();
	virtual void OnCancel();	

public:	
	void		Init();
	void		Quit( TCHAR* sz );
	void		MyMessageBox( TCHAR* sz, CMyMessageBox::eMessageType eMT = CMyMessageBox::MT_NORMAL );
	
	void		InitVersion();//função de controle de versão
	void		UpdateVersion();
	void		ReturnVersion();

	void		MainButtonEnable( bool bEnable );//Função de configuração do botão principal

	bool		IsInstllEnd();//Confirma se o jogo foi instalado
	void		Install_End();//Instalação do jogo concluída
	void		Update_End();//Atualização do jogo concluída
	void		Pack_End();//empacotamento do jogo feito

	void		_FullClientDownLoadComplete();

protected:
	void		_Init_Window();
	void		_Init_Web();
	void		_Init_Control();
	void		_Init_Str();
	
	void		_CheckRegestry();
	void		_SelectGame();	

	void		_Render();
	void		_Update();	

//void _VersionDown();
	void		_VersionCheck();

	//	bool		_IsInstaller(); //Devemos induzir o arquivo de instalação a partir da página inicial?

	bool		_PlayGame( TCHAR* cExe, TCHAR* cCommand );//função de inicialização do jogo
//void _SetStartPage();
	
//função adicional
	bool		_CheckExeFile( TCHAR* szExeName );
	bool		_IsExecute();
	bool		_Findfolder( TCHAR* sz );			
	
private:
	bool		_IsServerMaintenance();
	void		ShowMaintenanceWindow();

	BOOL		_CheckRedist2015(TCHAR const* pPath);
	BOOL		_IsRedistInstalled();
	void GameStart();
//variável
/*---------------------------------------------------------------------
Str usado dentro
-----------------------------------------------------------------------*/
public:	
	sStrInfo*	m_strGameName;
	sStrInfo*	m_strGameState;
	sStrInfo*	m_strClientVersion;
	sStrInfo*	m_strGameFile;	

	sStrInfo*	m_strCurPer;
	sStrInfo*	m_strTotalPer;
	sStrInfo*	m_strTime;

/*---------------------------------------------------------------------
variáveis-chave
-----------------------------------------------------------------------*/
protected:
	CServerCheck	 m_dlgSeverCheck;//diálogo de instalação
	CMyMessageBox	 m_dlgMessageBox;//caixa de mensagem
	

	bool		m_bQuit;
	eSTATE		m_eMouseState;//estado do mouse

	int			m_nServerVersion;//versão do patch do servidor
	int			m_nClientVersion;//versão atual do cliente
	int			m_nLauncherVer;//versão do lançador
	TCHAR		m_szOldClientVersion[ 32 ];//versão original do cliente
	
public:			
	TCHAR		m_szFullDown[ 256 ];//Pasta de download da versão completa

/*---------------------------------------------------------------------
campo de controle
-----------------------------------------------------------------------*/
protected:
	HICON		m_hIcon;	
	CBitmap		m_BackBitmap;//fundo BMP

	CPoint		m_ptWindowPos;
	CPoint		m_ptBackupDownPos;
	bool		m_bClickTitle;		
	
	cProgressbar*	m_pProgressCur;
	cProgressbar*	m_pProgressTotal;
	
	eGameState		m_eGameState;//Valor do estado do botão de jogo
	cBmpButton*		m_pRun;
	cBmpButton*		m_pUpdate;
	cBmpButton*		m_pInstall;
	cBmpButton*		m_pFileCheck;	

	cBmpButton*		m_pGameButton[ GAME_END ];
	
	cBmpButton*		m_pMinimalize;
	cBmpButton*		m_pClose;	

	cStrButton*		m_pHomePage;
	cStrButton*		m_pOptionPage;
	cStrButton*		m_pPatch;

	// 	bool			m_bStartPage;
	// 	CBitmap			m_CheckBoxOn;
	// 	CBitmap			m_CheckBoxOff;
	// 	cStrButton*		m_pCheckbox;	

//controle web
	CMyHtmlView*	m_pMainHtmlView;
	CMyHtmlView*	m_pSubHtmlView;	

/*---------------------------------------------------------------------
Processo inicial de download de arquivo
-----------------------------------------------------------------------*/
public:	
	void		GoThread();
	void		DeleteWaitQueue();	
	bool		CheckFile( TCHAR* szFileName, DWORD& dwSize, DWORD dwPatchNum, sTHREAD_INPUT::eTYPE eThread );//Verifica se há atualizações ou arquivos de instalação

	bool		PassServerCheck();//Verifica se um patch está disponível mesmo durante a manutenção do servidor.

protected:
	HINTERNET		m_hSession;		
	CThreadSafeQueue< sTHREAD_INPUT* >	m_WaitQueue;

//================================================ ==================================================== ===============
//NOWCDN
//================================================ ==================================================== ===============
public:	
	CDownEngineSDK	m_DownEngine;//API para receber o Download do Patch
	bool			m_bDownStart;	

public:	
//Informações sobre o(s) arquivo(s) a serem baixados
	static void CallBack_File_OnFileInfo(IN void* pThisPointer, IN CHAR* pszFileNameA, IN WCHAR* pszFileNameW, IN ULONG ulFileTotalCount, IN ULONGLONG ullFileSize, IN ULONG ulCPCode, IN ULONG ulGameCode, IN ULONG ulFileID, IN ULONG ulFileType);
//Notifica que o download começou
	static void CallBack_File_OnDownloadStart(IN void* pThisPointer);
//Notificação de que o download foi concluído
	static void CallBack_File_OnDownloadComplete(IN void* pThisPointer);	

private:
	CString			m_strCurrentPath;
	sConfiguration	m_launcherInfos;
};

extern CLauncherDlg*	g_pDlg;
