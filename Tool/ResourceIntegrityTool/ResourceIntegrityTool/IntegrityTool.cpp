// IntegrityTool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ResourceIntegrityTool.h"
#include "IntegrityTool.h"
//#include "utils.hpp"
#include "_DlgResourceView.h"

#include "../../../FtpClient/FTPClient.h"

#define NEWLINE "\r\n"
#define TOKKEN ":"
#define CONFIGE_FILE "ResourceIntegrityConfig.json"

//////////////////////////////////////////////////////////////////////////
#include "../../../../common_vs2022/pServer.h"

//////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNAMIC(CIntegrityTool, CDialog)

CIntegrityTool::CIntegrityTool(CWnd* pParent /*=NULL*/)
	: CDialog(CIntegrityTool::IDD, pParent)
{
	m_ConfigFileName = CONFIGE_FILE;
}

CIntegrityTool::~CIntegrityTool()
{
}

void CIntegrityTool::SetMakeHashDirFile( std::wstring const& hashDirFile, std::wstring const& makeHashDataFolder, std::wstring const& uploadFile )
{
	m_makeDirFile = hashDirFile;
	m_saveHashDirDir = makeHashDataFolder;
	DmCS::StringFn::ToMB( uploadFile, m_uploadZipFile );
}

BOOL CIntegrityTool::OnInitDialog()
{
	m_Result.clear();

	if( !_LoadConfigeFile() )
		return FALSE;

	//::DeleteFileA( m_ConfigeData.GetUpDownFileName().c_str() );

	if( !m_uploadZipFile.empty() )// Se houver um nome de arquivo para carregar
	{
		std::string strCheckFolder = m_uploadZipFile;
		std::string sFileName;	std::string sFileExt;
		DmCS::FileFn::FindFileNameExt( strCheckFolder.c_str(), sFileName, sFileExt );
		if( sFileName.empty() || 0 != sFileExt.compare(".zip") )
		{
			CsMessageBoxA( MB_OK, "O arquivo a ser carregado nao e um arquivo zip.\n%s", m_uploadZipFile.c_str() );
			OnCancel();
			return TRUE;
		}

		sFTPInfo const& UploadFtpInfo = m_ConfigeData.GetPatchUploadFTPInfo();		
		// 업로드 데이터 정보로 파일을 업로드 함.
		if( DmCS::FileFn::IsAccessFile( m_uploadZipFile.c_str() ) )
		{
			int nRe = CsMessageBoxA( MB_OKCANCEL, "Carregar para a Akamai?\n%s\n%s", m_uploadZipFile.c_str(), UploadFtpInfo.m_Directory.c_str() );
			if( nRe == IDOK )
			{
				if( _FTPFileUpload( UploadFtpInfo.m_Host, UploadFtpInfo.m_Port, UploadFtpInfo.m_ID, UploadFtpInfo.m_PW, UploadFtpInfo.m_Directory, m_uploadZipFile ) )
					CsMessageBoxA( MB_OK, "Carregado para a Akamai..\n%s\n%s", m_uploadZipFile.c_str(), UploadFtpInfo.m_Directory.c_str() );
			}
			else
			{
				OnCancel();
				return TRUE;
			}
		}	
		else
		{
			CsMessageBoxA( MB_OK, "Nao ha arquivos para carregar na Akamai.\n%s", m_uploadZipFile.c_str() );
			OnCancel();
			return TRUE;
		}

		DmCS::StringFn::Replace( strCheckFolder, sFileExt, "" );
		std::wstring wsCheckFolder;
		DmCS::StringFn::From( wsCheckFolder, strCheckFolder );
		if( _OnMakeHashData( wsCheckFolder ) )
		{
			std::string saveFile = strCheckFolder;
			saveFile += "\\";
			saveFile += sFileName;
			saveFile += "_resourcehash.txt";
			std::string resultString = _MakeHashDataToString( m_Result );
			_SaveFile( resultString, saveFile, false );
			if( _SendUpdateHashData() )
				OnBnClickedUpdataServerHash();
		}
		OnCancel();
		return TRUE;
	}

	CDialog::OnInitDialog();
	_CreateUI();
	_SetLastFolder();
	//GetDlgItem( IDC_EDIT_PATCH_NUMBER )->SetFocus();
	return FALSE;
}

int GetNumberInString(std::wstring const str)
{
	std::wstring findNum;
	for( int n = 0; n < str.size(); ++n )
	{
		WCHAR ch = str.at(n);
		if( ch >= '0' && ch <= '9' )
			findNum += ch;
	}
	
	int nNumber = _ttoi( findNum.c_str() );
	return nNumber;
}

void CIntegrityTool::_SetLastFolder()
{
	WCHAR rootDir[MAX_PATH] = {0,};  // O local onde o arquivo está localizado é tratado como o diretório atual.	
	GetCurrentDirectory(MAX_PATH, rootDir);

	CString sFolder = rootDir;
	sFolder += L"\\*.*";

	std::list<std::wstring> folderList;
	WIN32_FIND_DATA wfd;
	HANDLE dh = ::FindFirstFile( sFolder.GetBuffer(), &wfd );
	if( dh != INVALID_HANDLE_VALUE )
	{
		do 
		{
			if ( (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )// Se for um diretório...
			{
				// 폴더리스트
				if(	_wcsicmp(wfd.cFileName, L".") != 0 &&
					_wcsicmp(wfd.cFileName, L"..") != 0 &&
					_wcsicmp(wfd.cFileName, L".svn") != 0 )
				{
					folderList.push_back( wfd.cFileName );					
				}
			}
		} while ( TRUE == ::FindNextFile(dh, &wfd) );
		FindClose(dh);
	}

	int nLastNumber = 0;
	std::wstring findLastFolderName;
	std::list<std::wstring>::iterator it = folderList.begin();
	for( ; it != folderList.end(); ++it )
	{
		int nNumber = GetNumberInString( *it );
		if( nNumber <= nLastNumber )
			continue;
		findLastFolderName = *it;
		nLastNumber = nNumber;
	}

	if( !findLastFolderName.empty() )
		m_PatchNumberInput.SetWindowText( findLastFolderName.c_str() );
	GetDlgItem( IDC_EDIT_PATCH_NUMBER )->SetFocus();
}

void CIntegrityTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_ID, m_IdEdit);
	DDX_Control(pDX, IDC_EDIT_PASSWORD, m_PasswordEdit);
	DDX_Control(pDX, IDC_EDIT_UPLOAD_PORT, m_PortEdit);
	DDX_Control(pDX, IDC_EDIT_UPLOADURL, m_UploadUrlEdit);
	DDX_Control(pDX, IDC_EDIT_UPLOADDIRECTORY, m_UploadDirectoryEdit);
	DDX_Control(pDX, IDC_EDIT_UPLOAD_FILE, m_UploadHashFileNameEdit);
	

	DDX_Control(pDX, IDC_FOLDER, m_IncludeFolder);
	DDX_Control(pDX, IDC_EXTENSION, m_IncludeExt);	
	DDX_Control(pDX, IDC_LIST_INCLUDE_FULL_FILENAME, m_IncludeFileList);

	DDX_Control(pDX, IDC_EDIT_OUTFILENAME, m_HashFileName);		
	DDX_Control(pDX, IDC_EDIT_CURRENTDIR, m_CurrentDirEdit);
	
	DDX_Control(pDX, IDC_LIST_HASHRESULT, m_HashList);		

	DDX_Control(pDX, IDC_EDIT_PATCH_NUMBER, m_PatchNumberInput);

	DDX_Control(pDX, IDC_EDIT_SERVER_ID, m_ServerConnectID);	
	DDX_Control(pDX, IDC_EDIT_SERVER_pw, m_ServerConnectPW);	
	DDX_Control(pDX, IDC_EDIT_SERVER_PORT, m_ServerConnectPort);	
	DDX_Control(pDX, IDC_EDIT_SERVER_URL, m_ServerConnectHost);	

	DDX_Control(pDX, IDC_EDIT_SERVER_HASHFILENAME, m_ServerDirectory);		
	DDX_Control(pDX, IDC_EDIT_SERVER_UPLOAD_FILE, m_ServerHashFileName);	

	DDX_Control(pDX, IDC_EDIT_PATCH_UPLOAD_HOST, m_PatchUploadFTP_Host);		
	DDX_Control(pDX, IDC_EDIT_PATCH_UPLOAD_PORT, m_PatchUploadFTP_Port);		
	DDX_Control(pDX, IDC_EDIT_PATCH_UPLOAD_ID, m_PatchUploadFTP_ID);		
	DDX_Control(pDX, IDC_EDIT_PATCH_UPLOAD_PW, m_PatchUploadFTP_PW);		
	DDX_Control(pDX, IDC_EDIT_PATCH_UPLOAD_PATH, m_PatchUploadFTP_Path);
	DDX_Control(pDX, IDC_EDIT_PATCH_UPLOAD_FILE, m_PatchUploadFTP_File);

	DDX_Control(pDX, IDC_EDIT_USER_CONNECTION_ID, m_UserConnectionID);
	DDX_Control(pDX, IDC_EDIT_USER_CONNECTION_PW, m_UserConnectionPW);

	DDX_Control(pDX, IDC_EDIT_HASHLOG_HOST, m_UserHashLogFTP_Host);		
	DDX_Control(pDX, IDC_EDIT_HASHLOG_PORT, m_UserHashLogFTP_Port);		
	DDX_Control(pDX, IDC_EDIT_HASHLOG_ID, m_UserHashLogFTP_ID);		
	DDX_Control(pDX, IDC_EDIT_HASHLOG_PW, m_UserHashLogFTP_PW);		
	DDX_Control(pDX, IDC_EDIT_HASHLOG_PATH, m_UserHashLogFTP_Path);
	DDX_Control(pDX, IDC_EDIT_HASHLOG_FILENAME, m_UserHashLogFTP_File);	

	DDX_Control(pDX, IDC_EDIT_CLIENTEXE_FILENAME, m_Editbox_ClientExeFileName);
	DDX_Control(pDX, IDC_EDIT_ClientHashResult, m_Editbox_ClientExeHashResult);	

	DDX_Control(pDX, IDC_EDIT_SERVER_REQUEST_PORT, m_ServerRequestPort);		
}

BOOL CIntegrityTool::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		switch( pMsg->wParam )
		{
		case VK_RETURN:
			{
				if( pMsg->hwnd == m_PatchNumberInput )
				{
					if( _OnMakePatchAndHashData() )
						OnBnClickedUpdataServerHash();
				}
			}
			return TRUE;
			break;
		case VK_ESCAPE:
			return TRUE;
		}
	}	

	return CDialog::PreTranslateMessage(pMsg);
}


BEGIN_MESSAGE_MAP(CIntegrityTool, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ChangeDir, &CIntegrityTool::OnBnClickedButtonChangedir)
	ON_BN_CLICKED(IDCANCEL, &CIntegrityTool::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_MAKE_HASH, &CIntegrityTool::OnBnClickedButtonMakeHash)
	ON_BN_CLICKED(IDC_UPDATE_HASHDATA, &CIntegrityTool::OnBnClickedUpdateHashdata)
	ON_BN_CLICKED(IDC_HASHFILE_SAVE, &CIntegrityTool::OnBnClickedHashfileSave)
	ON_LBN_SELCHANGE(IDC_FOLDER, &CIntegrityTool::OnLbnSelchangeFolder)
	ON_BN_CLICKED(IDC_BUTTON_RELOAD_CONFIG, &CIntegrityTool::OnBnClickedButtonReloadConfig)
	ON_BN_CLICKED(IDC_BUTTON_HASHDATA_SAVE, &CIntegrityTool::OnBnClickedButtonHashdataSave)
	ON_BN_CLICKED(IDC_UPDATA_SERVER_HASH, &CIntegrityTool::OnBnClickedUpdataServerHash)
	ON_BN_CLICKED(IDC_UPDATA_PATCH_DATA, &CIntegrityTool::OnBnClickedUpdataPatchData)
	ON_BN_CLICKED(IDC_BUTTON1, &CIntegrityTool::OnBnClickedButton_PatchFileFind)
	ON_EN_CHANGE(IDC_EDIT_UPLOADURL, &CIntegrityTool::OnEnChangeEditUploadurl)
	ON_EN_CHANGE(IDC_EDIT_UPLOAD_PORT, &CIntegrityTool::OnEnChangeEditUploadPort)
	ON_EN_CHANGE(IDC_EDIT_ID, &CIntegrityTool::OnEnChangeEditId)
	ON_EN_CHANGE(IDC_EDIT_PASSWORD, &CIntegrityTool::OnEnChangeEditPassword)
	ON_EN_CHANGE(IDC_EDIT_UPLOADDIRECTORY, &CIntegrityTool::OnEnChangeEditUploaddirectory)
	ON_EN_CHANGE(IDC_EDIT_UPLOAD_FILE, &CIntegrityTool::OnEnChangeEditUploadFile)
	ON_EN_CHANGE(IDC_EDIT_SERVER_URL, &CIntegrityTool::OnEnChangeEditServerUrl)
	ON_EN_CHANGE(IDC_EDIT_SERVER_PORT, &CIntegrityTool::OnEnChangeEditServerPort)
	ON_EN_CHANGE(IDC_EDIT_SERVER_REQUEST_PORT, &CIntegrityTool::OnEnChangeEditServerRequestPort)
	ON_EN_CHANGE(IDC_EDIT_SERVER_ID, &CIntegrityTool::OnEnChangeEditServerId)
	ON_EN_CHANGE(IDC_EDIT_SERVER_pw, &CIntegrityTool::OnEnChangeEditServerpw)
	ON_EN_CHANGE(IDC_EDIT_SERVER_HASHFILENAME, &CIntegrityTool::OnEnChangeEditServerHashfilename)
	ON_EN_CHANGE(IDC_EDIT_SERVER_UPLOAD_FILE, &CIntegrityTool::OnEnChangeEditServerUploadFile)
	ON_EN_CHANGE(IDC_EDIT_PATCH_UPLOAD_HOST, &CIntegrityTool::OnEnChangeEditPatchUploadHost)
	ON_EN_CHANGE(IDC_EDIT_PATCH_UPLOAD_PORT, &CIntegrityTool::OnEnChangeEditPatchUploadPort)
	ON_EN_CHANGE(IDC_EDIT_PATCH_UPLOAD_ID, &CIntegrityTool::OnEnChangeEditPatchUploadId)
	ON_EN_CHANGE(IDC_EDIT_PATCH_UPLOAD_PW, &CIntegrityTool::OnEnChangeEditPatchUploadPw)
	ON_EN_CHANGE(IDC_EDIT_PATCH_UPLOAD_PATH, &CIntegrityTool::OnEnChangeEditPatchUploadPath)
	ON_EN_CHANGE(IDC_EDIT_PATCH_UPLOAD_FILE, &CIntegrityTool::OnEnChangeEditPatchUploadFile)
	ON_BN_CLICKED(IDC_BUTTON2, &CIntegrityTool::OnBnClickedButton2)
	ON_EN_CHANGE(IDC_EDIT_USER_CONNECTION_ID, &CIntegrityTool::OnEnChangeEditUserConnectionId)
	ON_EN_CHANGE(IDC_EDIT_USER_CONNECTION_PW, &CIntegrityTool::OnEnChangeEditUserConnectionPw)
	ON_EN_CHANGE(IDC_EDIT_HASHLOG_HOST, &CIntegrityTool::OnEnChangeEditHashlogHost)
	ON_EN_CHANGE(IDC_EDIT_HASHLOG_PATH, &CIntegrityTool::OnEnChangeEditHashlogPath)
	ON_EN_CHANGE(IDC_EDIT_HASHLOG_PORT, &CIntegrityTool::OnEnChangeEditHashlogPort)
	ON_EN_CHANGE(IDC_EDIT_HASHLOG_ID, &CIntegrityTool::OnEnChangeEditHashlogId)
	ON_EN_CHANGE(IDC_EDIT_HASHLOG_PW, &CIntegrityTool::OnEnChangeEditHashlogPw)
	ON_EN_CHANGE(IDC_EDIT_HASHLOG_FILENAME, &CIntegrityTool::OnEnChangeEditHashlogFilename)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_ServerHashData_Ref, &CIntegrityTool::OnBnClickedServerhashdataRef)
	ON_BN_CLICKED(IDC_BUTTON_RESOURCE_EDIT, &CIntegrityTool::OnBnClickedButtonResourceEdit)
END_MESSAGE_MAP()

// 가로 스크롤 생성하는 함수
void CIntegrityTool::_CreateHorizontalScroll(CListBox & listBox)
{
	CString str; CSize sz; int dx=0;
	CDC *pDC = listBox.GetDC();

	for(int i = 0; i < listBox.GetCount(); i++)
	{
		listBox.GetText(i, str);
		sz = pDC->GetTextExtent(str);

		if (sz.cx > dx)
			dx = sz.cx;
	}
	listBox.ReleaseDC(pDC);

	if (listBox.GetHorizontalExtent() < dx)
	{
		listBox.SetHorizontalExtent(dx);
		ASSERT(listBox.GetHorizontalExtent() == dx);
	}
}

bool CIntegrityTool::_CheckIncludeFile( dm_stringlist const& orgList, dm_stringlist& pResultList )
{
	dm_stringlist::const_iterator orgIT = orgList.begin();
	for( ; orgIT != orgList.end(); ++orgIT )
	{
		if( m_ConfigeData.CheckIncludeFile( *orgIT ) )
		{
			pResultList.push_back( *orgIT );
			continue;
		}

		if( m_ConfigeData.CheckIncludeDirectory( *orgIT ) )
			pResultList.push_back( *orgIT );
	}

	return true;
}

bool CIntegrityTool::_LoadFileList_PackFile( std::string const& sPackFile, dm_stringlist& saveData )
{
	if( !CsFPS::CsFPSystem::Initialize( true, sPackFile.c_str(), false ) )
	{
		CsMessageBoxA( MB_OK, "%s - Nao e possível abrir o arquivo de embalagem.", sPackFile.c_str() );
		return false;
	}

	CsFPS::CsFPSystem::GetFileList(0, saveData);
	CsFPS::CsFPSystem::Destroy();
	return true;
}

// Extraindo valores de dados de hash de arquivos compactados
bool CIntegrityTool::_LoadHashData_FromPackFile( std::string const& sPackFile, MAP_HASH_CONTANER& getHashData )
{
	//getHashData.clear();

	if( !CsFPS::CsFPSystem::Initialize( true, sPackFile.c_str(), false ) )
	{
		CsFPS::CsFPSystem::Destroy();
		CsMessageBoxA( MB_OK, "%s -Nao e possível abrir o arquivo de embalagem.", sPackFile.c_str() );
		return false;
	}

	dm_stringlist orgFiles;
	CsFPS::CsFPSystem::GetFileList(0, orgFiles);

	dm_stringlist::iterator orgit = orgFiles.begin();
	for( ; orgit != orgFiles.end(); ++orgit )
		DmCS::StringFn::Lwr( *orgit );

	dm_stringlist resultFiles;
	if( !_CheckIncludeFile( orgFiles, resultFiles ) )
	{
		CsFPS::CsFPSystem::Destroy();
		return false;
	}

	char* pAlloc = NULL;
	size_t nAllocSize = 0;
	dm_stringlist::iterator it = resultFiles.begin();
	for( ; it != resultFiles.end(); ++it )
	{
		CsFPS::CsFileHash::sINFO* pHashInfo = CsFPS::CsFPSystem::GetHashData(0, (*it).c_str() );
		if( NULL == pHashInfo )
			continue;

		if( nAllocSize < pHashInfo->s_nDataSize )
		{
			nAllocSize = pHashInfo->s_nDataSize;
			free( pAlloc );
			pAlloc = (char*)malloc( nAllocSize );
		}

		CsFPS::CsFPSystem::GetFileData( 0, &pAlloc, pHashInfo->s_nOffset, pHashInfo->s_nDataSize );
		std::string sHash = DmCS::sha256_buffer( (const unsigned char*)pAlloc, pHashInfo->s_nDataSize );

		auto rib = getHashData.insert( std::make_pair((*it), sHash ) );
		if( !rib.second )
			CsMessageBoxA( MB_OK, "Existem arquivos duplicados no arquivo que voce esta tentando colocar no arquivo compactado..\n %s", (*it).c_str() );
	}		

	if( pAlloc != NULL )
	{
		free( pAlloc );
		pAlloc = NULL;
	}
	CsFPS::CsFPSystem::Destroy();
	return true;
}

// Extraia valores de dados de hash de arquivos.
bool CIntegrityTool::_LoadHashData_FromFolderFile( std::string const& rootPath, dm_stringlist const& sPackFile, MAP_HASH_CONTANER& getHashData )
{
	if( sPackFile.empty() )
		return false;

	dm_stringlist::const_iterator it = sPackFile.begin();
	for( ; it != sPackFile.end(); ++it )
	{
		std::string sHash = DmCS::sha256_file( rootPath + (*it) );
		if( sHash.empty() )
			sHash = DmCS::sha256_file( rootPath + "pack01\\" + (*it) );

		if( sHash.empty() )
		{
			CsMessageBoxA( MB_OK, "Arquivo nao encontrado.\n %s", rootPath + (*it).c_str() );
			return false;
		}

		auto rib = getHashData.insert( std::make_pair((*it), sHash ) );
		if( !rib.second )
			CsMessageBoxA( MB_OK, "Existem arquivos duplicados no arquivo que você está tentando colocar no arquivo compactado..\n %s", (*it).c_str() );
	}
	return true;
}

std::string CIntegrityTool::_MakeHashDataToString( MAP_HASH_CONTANER const& saveData )
{
	std::string hashData;
	MAP_HASH_CONTANER_CIT it = saveData.begin();
	for( ; it != saveData.end(); ++it )
		hashData += it->first + TOKKEN + it->second + NEWLINE;

	return hashData;
}

// String dividida em valor de chave e valor de dados
void CIntegrityTool::_MaekCheckData( std::string const& downloadData, MAP_HASH_CONTANER& saveData )
{
	saveData.clear();
	std::string org = downloadData;
	while( !org.empty() )
	{
		std::string line = DmCS::StringFn::tokenize( org, NEWLINE );
		dm_stringvec value;
		DmCS::StringFn::Cut( line, TOKKEN, value );
		if( value.size() < 2 )
			break;
		DmCS::StringFn::Lwr( value[0] );
		saveData.insert( std::make_pair( value[0], value[1] ) );
	}
}

//Atualização de dados adicionados ou alterados recentemente
void CIntegrityTool::_UpdateNewHashData( MAP_HASH_CONTANER const& newData, MAP_HASH_CONTANER& recvData )
{
	MAP_HASH_CONTANER_CIT it = newData.begin();
	for( ; it != newData.end(); ++it )
	{
		MAP_HASH_CONTANER_IT orgIt = recvData.find( it->first );
		if( orgIt == recvData.end() )
			recvData.insert( std::make_pair( it->first, it->second ) );
		else
			orgIt->second = it->second;
	}
}

int CALLBACK BrowseCallbackProc( HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData )
{
	if( uMsg == BFFM_INITIALIZED )
	{
		if( lpData )
			SendMessage(hwnd, BFFM_SETSELECTION, (WPARAM)TRUE, (LPARAM)lpData);
	}
	return 0;
}

void CIntegrityTool::OnBnClickedButtonChangedir()
{
	CString currDir;
	m_CurrentDirEdit.GetWindowText( currDir );

	TCHAR tstrPathName[MAX_PATH] = _T("");
	BROWSEINFO   bi; 
	ZeroMemory(&bi,sizeof(BROWSEINFO));
	bi.hwndOwner   =   this->m_hWnd; 
	bi.pszDisplayName = tstrPathName;  // Salve o nome do diretório selecionado
	bi.lpszTitle = L"Selecione uma pasta"; 
	bi.lParam = (LPARAM)(LPCTSTR)currDir.GetBuffer();
	bi.ulFlags  =  BIF_EDITBOX    // Uma janela que exibe e edita o nome do diretório selecionado
		| BIF_NEWDIALOGSTYLE             // Adicionado um botão para criar um novo diretório
		| BIF_BROWSEINCLUDEFILES; 
	bi.lpfn = BrowseCallbackProc;
	LPITEMIDLIST idl = SHBrowseForFolder(&bi);

	if (idl) { 
		TCHAR changePath[MAX_PATH] = _T("");
		SHGetPathFromIDList(idl, changePath); // Nomes de diretório incluindo PATH completo

		m_CurrentDirEdit.SetWindowText( changePath );
	}
}

void CIntegrityTool::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnCancel();
}

// Extraia o valor de hash do arquivo executável
void CIntegrityTool::_MakeClientExeHash( std::string const& makePath )
{
	m_ClientExeHashData.clear();
	std::string strClientFileName = m_ConfigeData.GetClientFileName();
	if( !strClientFileName.empty() )
		m_ClientExeHashData = DmCS::sha256_file( makePath + strClientFileName );
}

bool CIntegrityTool::_OnMakeHashData(std::wstring const& makePath)
{
	std::wstring fullpath = makePath;

	std::wstring wsIsFileName;
	std::wstring wsIsFileExt;
	DmCS::FileFn::FindFileNameExt( makePath.c_str(), wsIsFileName, wsIsFileExt );
	if( !wsIsFileExt.empty() )
	{
		if( 0 == wsIsFileExt.compare( _T(PACK_EX_NAME) ) || 0 == wsIsFileExt.compare( _T(HASH_EX_NAME) ) )
		{
			std::string path;
			DmCS::StringFn::Replace( fullpath, _T(PACK_EX_NAME), L"" );// pack ex 제거
			DmCS::StringFn::Replace( fullpath, _T(HASH_EX_NAME), L"" );// pack ex 제거
			DmCS::StringFn::ToMB( fullpath, path );

			return _LoadHashData_FromPackFile( path, m_Result );

// 			DmCS::StringFn::Replace( fullpath, wsIsFileName, L"" );// 루트 경로 제거
// 			DmCS::StringFn::Replace( fullpath, wsIsFileExt, L"" );// pack01폴더 제거
// 			DmCS::StringFn::Replace( fullpath, _T(PACK_EX_NAME), L"" );// pack01폴더 제거
// 			DmCS::StringFn::Replace( fullpath, _T(HASH_EX_NAME), L"" );// pack01폴더 제거
		}
	}
	
	std::transform(fullpath.begin(), fullpath.end(), fullpath.begin(), tolower);

	dm_stringlist_t getList;
	DmCS::FileFn::GatherFileList( fullpath.c_str(), NULL, true, true, getList, DmCS::FileFn::eLower );
	fullpath += L"\\";

	dm_stringlist_t::iterator it = getList.begin();
	for( ; it != getList.end(); ++it )
	{
		DmCS::StringFn::Replace( (*it), fullpath, L"" );// 루트 경로 제거
		DmCS::StringFn::Replace( (*it), L"pack01\\", L"" );// pack01폴더 제거
	}

	dm_stringlist strList;
	it = getList.begin();
	for( ; it != getList.end(); ++it )
	{
		std::string path;
		DmCS::StringFn::ToMB( *it, path );
		strList.push_back( path );
	}

	std::string strFullPath;
	DmCS::StringFn::ToMB( fullpath, strFullPath );

	// Extraia o valor de hash do arquivo executável
	_MakeClientExeHash( strFullPath );

	dm_stringlist resultFiles;
	_CheckIncludeFile( strList, resultFiles );
	if( !resultFiles.empty() )
		_LoadHashData_FromFolderFile( strFullPath, resultFiles, m_Result );

	//encontrar o arquivo de embalagem
	dm_stringlist::iterator subIT = strList.begin();
	for( ; subIT != strList.end(); ++subIT )
	{
		std::string fileName;		std::string extName;
		DmCS::FileFn::FindFileNameExt( (*subIT).c_str(), fileName, extName );

		if( !DmCS::StringFn::iCmp( extName , PACK_EX_NAME ) )
			continue;

		DmCS::StringFn::Replace((*subIT),PACK_EX_NAME,"");
		if( !_LoadHashData_FromPackFile( strFullPath + (*subIT), m_Result ) )
			return false;
	}

	return true;
}

// Extraia dados de hash de um diretório.
void CIntegrityTool::OnBnClickedButtonMakeHash()
{
	CString path;
	m_CurrentDirEdit.GetWindowText( path );
	if( path.IsEmpty() )
	{
		CsMessageBoxA( MB_OK, "O diretorio de dados nao existe." );
		return;
	}

	m_Result.clear();

	_OnMakeHashData( path.GetBuffer() );

	m_Editbox_ClientExeHashResult.SetWindowText( L"" );
	std::wstring strExeHashResult;
	DmCS::StringFn::From( strExeHashResult, m_ClientExeHashData );
	m_Editbox_ClientExeHashResult.SetWindowText( strExeHashResult.c_str() );

	m_HashList.ResetContent();
	MAP_HASH_CONTANER_CIT it = m_Result.begin();
	for( ; it != m_Result.end(); ++it )
	{
		std::wstring first;
		DmCS::StringFn::From( first, it->first );

		std::wstring value;
		DmCS::StringFn::From( value, it->second );

		first += _T(TOKKEN) + value;
		m_HashList.AddString( first.c_str() );
	}
	_CreateHorizontalScroll(m_HashList);
}

bool CIntegrityTool::_IsIncludeFolder( std::string const& strFolder, MAP_HASH_CONTANER const& hashData )
{
	MAP_HASH_CONTANER_CIT it = hashData.begin();
	for( ; it != hashData.end(); ++it )
	{
		std::string::size_type pos = it->first.find( strFolder );
		if( 0 != pos )
			continue;

		return true;
	}

	return false;
}

void CIntegrityTool::_RemoveIncludeFolderData( std::string const& strFolder, MAP_HASH_CONTANER & hashData )
{
	MAP_HASH_CONTANER_IT it = hashData.begin();
	for( ; it != hashData.end(); )
	{
		std::string::size_type pos = it->first.find( strFolder );
		if( 0 != pos )
		{
			++it;
			continue;
		}

		hashData.erase(it++);
	}
}

//////////////////////////////////////////////////////////////////////////
// Recebe dados do FTP com dados de hash e os atualiza.
//////////////////////////////////////////////////////////////////////////
bool CIntegrityTool::_SendUpdateHashData()
{
	if( m_Result.empty() )
	{	
		if( !m_ClientExeHashData.empty() )
			return true;

		CsMessageBoxA( MB_OK, "Nao ha dados para salvar." );
		return false;
	}

	sFTPInfo const& hashDataFTPInfo = m_ConfigeData.GetHashDataFTPInfo();
	std::string downloadData;
	// Recebe dados de hash antigos do FTP.
	bool bDown = _FTPFileDownload( hashDataFTPInfo.m_Host, hashDataFTPInfo.m_Port, hashDataFTPInfo.m_ID, hashDataFTPInfo.m_PW, hashDataFTPInfo.m_Directory, hashDataFTPInfo.m_FileName, downloadData );
	if( !bDown )
	{
		int nResult = CsMessageBoxA( MB_OKCANCEL, "Falha ao baixar o arquivo existente. Gostaria de carregar novos dados?" );
		if(nResult != IDOK)
			return false;
	}

	MAP_HASH_CONTANER	RecvHashData;
	// 받은 해쉬 데이터의 압축을 푼다. 
	if( gzip::is_compressed( downloadData.c_str(), downloadData.size() ) )
	{
		std::string strdeCom = gzip::decompress( downloadData.c_str(), downloadData.size() );	
		_MaekCheckData( strdeCom, RecvHashData );
	}
	else
		_MaekCheckData( downloadData, RecvHashData );

	// O arquivo do pacote de dados da tabela é inserido após a remoção do antigo.
	if( _IsIncludeFolder( "data\\bin\\", m_Result ) )
		_RemoveIncludeFolderData( "data\\bin\\", RecvHashData );

	// Atualização de dados de hash
	_UpdateNewHashData( m_Result, RecvHashData );	

	// 갱신한 데이터를 문자열로 변환
	std::string saveData = _MakeHashDataToString( RecvHashData );

// 	// salvar string em arquivo
// 	if( !_SaveFile( saveData, hashDataFTPInfo.m_FileName ) )
// 		return false;

	// Comprimido.
	std::string strCompress = gzip::compress( saveData.c_str(), saveData.size() );	

	// salvar string em arquivo
	if( !_SaveFile( strCompress, hashDataFTPInfo.m_FileName ) )
		return false;

	while(TRUE)
	{
		if( _FTPFileUpload( hashDataFTPInfo.m_Host, hashDataFTPInfo.m_Port, hashDataFTPInfo.m_ID, hashDataFTPInfo.m_PW, hashDataFTPInfo.m_Directory, hashDataFTPInfo.m_FileName ) )
		{
			CsMessageBoxA( MB_OK, "Carregamento de arquivo hash bem-sucedido \n%s\n%s", hashDataFTPInfo.m_FileName.c_str(), hashDataFTPInfo.m_Directory.c_str() );
			break;
		}

		int nResult = CsMessageBoxA( MB_OKCANCEL, "Falha ao fazer upload do arquivo hash. Tente novamente?\n%s\n%s", hashDataFTPInfo.m_FileName.c_str(), hashDataFTPInfo.m_Directory.c_str() );
		if( nResult != IDOK )
			break;
	}

	return true;
}

void CIntegrityTool::OnBnClickedUpdateHashdata()
{
	// TODO: Adicione seu código do manipulador de notificação de controle aqui.
	_SendUpdateHashData();
}

// Extraia o valor de hash do diretório e salve-o como um arquivo.
void CIntegrityTool::OnBnClickedHashfileSave()
{
	if( m_Result.empty() )
	{
		CsMessageBoxA( MB_OK, "Nao ha dados para salvar." );
		return;
	}

	sFTPInfo const& hashDataFTPInfo = m_ConfigeData.GetHashDataFTPInfo();
	std::string resultString = _MakeHashDataToString( m_Result );
	if( _SaveFile( resultString, hashDataFTPInfo.m_FileName, false ) )
		CsMessageBoxA( MB_OK, "%s\nSalvou", hashDataFTPInfo.m_FileName.c_str() );
}

void CIntegrityTool::OnLbnSelchangeFolder()
{
	m_IncludeExt.ResetContent();

	int index = m_IncludeFolder.GetCurSel();
	if( index < 0 )
		return;

	CString Selfolder;
	m_IncludeFolder.GetText(index, Selfolder);

	std::string folder;
	std::string fullpath;
	DmCS::StringFn::ToMB( Selfolder.GetBuffer(), folder );

	std::list<std::string> const * pExt = m_ConfigeData.GetIncludeFilesExt( folder );
	if( NULL == pExt )
		return;

	std::list<std::string>::const_iterator it = pExt->begin();
	for( ; it != pExt->end(); ++it )
	{
		std::wstring val;
		DmCS::StringFn::From( val, (*it) );
		m_IncludeExt.AddString( val.c_str() );			
	}
}

void CIntegrityTool::OnBnClickedButtonReloadConfig()
{
	m_IdEdit.SetWindowText( L"" );
	m_PasswordEdit.SetWindowText( L"" );
	m_PortEdit.SetWindowText( L"" );
	m_UploadUrlEdit.SetWindowText( L""  );
	m_UploadDirectoryEdit.SetWindowText( L""  );
	m_UploadHashFileNameEdit.SetWindowText(L"");
	m_HashFileName.SetWindowText(L"" );

	m_ServerConnectID.SetWindowText(L"" );
	m_ServerConnectPW.SetWindowText(L"" );
	m_ServerConnectPort.SetWindowText(L"" );
	m_ServerRequestPort.SetWindowText(L"" );
	m_ServerConnectHost.SetWindowText(L"" );
	m_ServerDirectory.SetWindowText(L"");
	m_ServerHashFileName.SetWindowText(L"" );

	m_PatchUploadFTP_Host.SetWindowText(L"" );
	m_PatchUploadFTP_Port.SetWindowText(L"" );	
	m_PatchUploadFTP_ID.SetWindowText(L"" );	
	m_PatchUploadFTP_PW.SetWindowText(L"" );
	m_PatchUploadFTP_Path.SetWindowText(L"" );
	m_PatchUploadFTP_File.SetWindowText(L"");

	m_UserConnectionID.SetWindowText(L"");
	m_UserConnectionPW.SetWindowText(L"");

	m_UserHashLogFTP_ID.SetWindowText(L"");
	m_UserHashLogFTP_PW.SetWindowText(L"");
	m_UserHashLogFTP_Host.SetWindowText(L"");
	m_UserHashLogFTP_Port.SetWindowText(L"");
	m_UserHashLogFTP_Path.SetWindowText(L"");
	m_UserHashLogFTP_File.SetWindowText(L"");
	m_Editbox_ClientExeFileName.SetWindowText(L"");
	

	m_IncludeFolder.ResetContent();
	m_IncludeFileList.ResetContent();
	m_IncludeExt.ResetContent();

	m_makeDirFile.clear();
	m_uploadZipFile.clear();
	m_saveHashDirDir.clear();
	
	_LoadConfigeFile();

	_CreateUI();	
}

bool CIntegrityTool::_LoadConfigeFile()
{
	m_ConfigeData.ResetData();
	if( m_ConfigeData.LoadConfigFile(m_ConfigFileName) )
		return true;

	CsMessageBoxA( MB_OK, "%s\nNenhum arquivo de configuracao de ferramenta ou nenhum dado.", m_ConfigFileName.c_str() );
	this->SetWindowText( L"Falha ao recarregar o arquivo de configuração da ferramenta" );
	return false;
}

void CIntegrityTool::_CreateUI()
{
	WCHAR path[MAX_PATH] = {0,};  // O local onde o arquivo está localizado é tratado como o diretório atual.
	GetCurrentDirectory(MAX_PATH, path);

	m_CurrentDirEdit.SetWindowText( path );	

	sFTPInfo const& hashFTPInfo = m_ConfigeData.GetHashDataFTPInfo();
 	std::wstring value = hashFTPInfo.GetID();
 	m_IdEdit.SetWindowText( value.c_str() );

	value = hashFTPInfo.GetPW();
	m_PasswordEdit.SetWindowText( value.c_str() );

	value = hashFTPInfo.GetPort();
	m_PortEdit.SetWindowText( value.c_str() );

	value = hashFTPInfo.GetHost();
	m_UploadUrlEdit.SetWindowText( value.c_str() );

	value = hashFTPInfo.GetDirectory();
	m_UploadDirectoryEdit.SetWindowText( value.c_str() );

	value = hashFTPInfo.GetFileName();
	m_UploadHashFileNameEdit.SetWindowText( value.c_str() );

	value = hashFTPInfo.GetFileName();
	m_HashFileName.SetWindowText( value.c_str() );

	value = m_ConfigeData.GetUserConnectionIDW();
	m_UserConnectionID.SetWindowText( value.c_str() );

	value = m_ConfigeData.GetUserConnectionPWW();
	m_UserConnectionPW.SetWindowText( value.c_str() );

	{
		std::list<std::string> const& nFiles = m_ConfigeData.GetIncludeFiles();
		std::list<std::string>::const_iterator it = nFiles.begin(); 
		for( ; it != nFiles.end(); ++it )
		{
			std::wstring value;
			DmCS::StringFn::From( value, (*it) );
			m_IncludeFileList.AddString( value.c_str() );
		}
	}

	{
		std::list<sIncludeFolderInfo> const& nFolders = m_ConfigeData.GetIncludeFolders();
		std::list<sIncludeFolderInfo>::const_iterator it = nFolders.begin(); 
		for( ; it != nFolders.end(); ++it )
		{
			std::wstring value;
			DmCS::StringFn::From( value, (*it).m_Folder );
			m_IncludeFolder.AddString( value.c_str() );
		}

		m_IncludeFolder.SetCurSel(0);
		it = nFolders.begin();
		if( it != nFolders.end() )
		{
			std::list<std::string>::const_iterator secit = (*it).m_FileExt.begin();
			for( ; secit != (*it).m_FileExt.end(); ++secit )
			{
				std::wstring value;
				DmCS::StringFn::From( value, (*secit) );
				m_IncludeExt.AddString( value.c_str() );
			}
		}

	}

	sFTPInfo const& serverFTPInfo = m_ConfigeData.GetServerUploadFTPInfo();
	value = serverFTPInfo.GetID();
	m_ServerConnectID.SetWindowText( value.c_str() );

	value = serverFTPInfo.GetPW();
	m_ServerConnectPW.SetWindowText( value.c_str() );

	value = serverFTPInfo.GetPort();
	m_ServerConnectPort.SetWindowText( value.c_str() );

	value = serverFTPInfo.GetRequestPort();
	m_ServerRequestPort.SetWindowText( value.c_str() );

	value = serverFTPInfo.GetHost();
	m_ServerConnectHost.SetWindowText( value.c_str() );

	value = serverFTPInfo.GetDirectory();
	m_ServerDirectory.SetWindowText( value.c_str() );

	value = serverFTPInfo.GetFileName();
	m_ServerHashFileName.SetWindowText( value.c_str() );

	sFTPInfo const& patchFTPInfo = m_ConfigeData.GetPatchUploadFTPInfo();
	value = patchFTPInfo.GetHost();
	m_PatchUploadFTP_Host.SetWindowText( value.c_str() );
	value = patchFTPInfo.GetPort();
	m_PatchUploadFTP_Port.SetWindowText( value.c_str() );	
	value = patchFTPInfo.GetID();
	m_PatchUploadFTP_ID.SetWindowText( value.c_str() );	
	value = patchFTPInfo.GetPW();
	m_PatchUploadFTP_PW.SetWindowText( value.c_str() );
	value = patchFTPInfo.GetDirectory();
	m_PatchUploadFTP_Path.SetWindowText( value.c_str() );

	sFTPInfo const& userHashLogFTPInfo = m_ConfigeData.GetUserHashLogFTPInfo();
	value = userHashLogFTPInfo.GetHost();
	m_UserHashLogFTP_Host.SetWindowText( value.c_str() );
	value = userHashLogFTPInfo.GetPort();
	m_UserHashLogFTP_Port.SetWindowText( value.c_str() );	
	value = userHashLogFTPInfo.GetID();
	m_UserHashLogFTP_ID.SetWindowText( value.c_str() );	
	value = userHashLogFTPInfo.GetPW();
	m_UserHashLogFTP_PW.SetWindowText( value.c_str() );
	value = userHashLogFTPInfo.GetDirectory();
	m_UserHashLogFTP_Path.SetWindowText( value.c_str() );
	value = userHashLogFTPInfo.GetFileName();
	m_UserHashLogFTP_File.SetWindowText( value.c_str() );

	value.clear();
	std::string exeFileName = m_ConfigeData.GetClientFileName();
	DmCS::StringFn::From( value, exeFileName );
	m_Editbox_ClientExeFileName.SetWindowText( value.c_str() );

	value = m_ConfigeData.GetTitleName();
	this->SetWindowText( value.c_str() );
}

//////////////////////////////////////////////////////////////////////////
// Se um arquivo de patch for criado e os dados de hash a serem atualizados existirem no arquivo de patch,
// Função para atualizar dados de hash.
//////////////////////////////////////////////////////////////////////////
bool CIntegrityTool::_OnMakePatchAndHashData()
{
	CString findFolder;
	m_PatchNumberInput.GetWindowText(findFolder);

	WCHAR rootDir[MAX_PATH] = {0,};  // O local onde o arquivo está localizado é tratado como o diretório atual.
	GetCurrentDirectory(MAX_PATH, rootDir);
	CString strDir = rootDir;

	// 패치할 폴더를 찾음.
	CString sFindFolder = _FindFolder( strDir, findFolder, false );
	if( sFindFolder.IsEmpty() )
	{
		CsMessageBoxA( MB_OK, "Pasta nao encontrada. : %s", sFindFolder.GetBuffer() );
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	// 찾은 폴더를 압축함.

	std::wstring upload = sFindFolder;
	std::wstring zipFilePath = DmCS::FileFn::FindFilePath(upload.c_str());
//	zipFilePath += L"\\";
	zipFilePath += DmCS::FileFn::FindFileName(upload.c_str());
	zipFilePath += L".zip";

	std::string zipfile;
	DmCS::StringFn::ToUTF8(zipFilePath, zipfile);
	
	std::string zipRootPath;
	DmCS::StringFn::ToUTF8(upload, zipRootPath);

	DmCS::CZipper zipper;
	if (zipper.OpenZip(zipfile.c_str(), zipRootPath.c_str(), false))
	{
		zipRootPath += "\\";
		if (!zipper.AddFolderToZip(zipRootPath.c_str()))
		{
			zipper.CloseZip();
			CsMessageBoxA(MB_OK, "압축 실패 : %s", sFindFolder.GetBuffer());
			return false;
		}			
	}
	zipper.CloseZip();	

	std::wstring zipPath = DmCS::FileFn::GetFileFullName( zipFilePath.c_str() );

	std::string zipFile ;
	DmCS::StringFn::ToMB( zipPath, zipFile);

	sFTPInfo const& UploadFtpInfo = m_ConfigeData.GetPatchUploadFTPInfo();	
	// Carregar arquivo com informações de dados de upload.
	if( DmCS::FileFn::IsAccessFile( zipFile.c_str() ) )
	{
		while( TRUE )
		{
			bool bResult = _FTPFileUpload( UploadFtpInfo.m_Host, UploadFtpInfo.m_Port, UploadFtpInfo.m_ID, UploadFtpInfo.m_PW, UploadFtpInfo.m_Directory, zipFile );
			if( bResult )
			{
				CsMessageBoxA( MB_OK, "Carregado para a Akamai..\n%s\n%s", zipFile.c_str(), UploadFtpInfo.m_Directory.c_str() );
				break;
			}

			int nResult = CsMessageBoxA( MB_OKCANCEL, "Falha ao carregar para a Akamai Deseja tentar novamente??\nUpload File Name : %s", zipFile.c_str() );
			if( nResult != IDOK )
				return false;
		}
	}	
	else
	{
		CsMessageBoxA( MB_OK, "Nao ha arquivos para carregar na Akamai.\n%s", zipFile.c_str() );
		return false;
	}
	//////////////////////////////////////////////////////////////////////////

	m_Result.clear();

	if( !_OnMakeHashData( sFindFolder.GetBuffer() ) )
		return false;

	m_Editbox_ClientExeHashResult.SetWindowText( L"" );
	std::wstring strExeHashResult;
	DmCS::StringFn::From( strExeHashResult, m_ClientExeHashData );
	m_Editbox_ClientExeHashResult.SetWindowText( strExeHashResult.c_str() );

	if( m_Result.empty() )
	{
		if( !m_ClientExeHashData.empty() )
			return true;
		return false;
	}

	sFTPInfo const& hashdataFtpInfo = m_ConfigeData.GetHashDataFTPInfo();
	std::wstring wsFile = DmCS::FileFn::GetFilePath( sFindFolder.GetBuffer() );			
	wsFile += findFolder.GetBuffer();
	std::string SavehashFile ;
	DmCS::StringFn::ToMB( wsFile, SavehashFile);
	std::string saveFileName = DmCS::FileFn::GetFileName(hashdataFtpInfo.m_FileName.c_str());
	std::string saveFileExt = DmCS::FileFn::GetFileExt(hashdataFtpInfo.m_FileName.c_str());
	SavehashFile += "_";
	SavehashFile += hashdataFtpInfo.m_FileName;

	std::string resultString = _MakeHashDataToString( m_Result );
	_SaveFile( resultString, SavehashFile, false );
	_SendUpdateHashData();
	return true;
}

CString CIntegrityTool::_FindFolder( CString const& RootDir, CString const& findFolder, bool bFindDownFolder )
{
	CString sFolder = RootDir;
	sFolder += L"\\*.*";
	std::wstring find = findFolder;
	CString sFindPatchFolderName;

	WIN32_FIND_DATA wfd;
	HANDLE dh = ::FindFirstFile( sFolder.GetBuffer(), &wfd );
	if( dh != INVALID_HANDLE_VALUE )
	{
		do 
		{
			if ( (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )// 디렉토리 이면....
			{
				// 폴더리스트
				if(	_wcsicmp(wfd.cFileName, L".") != 0 &&
					_wcsicmp(wfd.cFileName, L"..") != 0 &&
					_wcsicmp(wfd.cFileName, L".svn") != 0 )
				{
					if( 0 == _wcsicmp( wfd.cFileName, find.c_str() ) )
					{
						sFindPatchFolderName = RootDir;
						sFindPatchFolderName += L"\\";
						sFindPatchFolderName += wfd.cFileName;
						break;
					}

					if( bFindDownFolder )
					{
						CString sNextFolder = RootDir;
						sNextFolder += L"\\";
						sNextFolder += wfd.cFileName;

						CString find = _FindFolder( sNextFolder, findFolder, bFindDownFolder );
						if( !find.IsEmpty() )
						{
							sFindPatchFolderName = find;
							break;
						}
					}
				}
			}
		} while ( TRUE == ::FindNextFile(dh, &wfd) );
		FindClose(dh);
	}

	return sFindPatchFolderName;
}

// Uma função que recebe um arquivo do FTP onde os dados de hash são armazenados e os salva como um arquivo
void CIntegrityTool::OnBnClickedButtonHashdataSave()
{
	sFTPInfo const& hashDataFTPInfo = m_ConfigeData.GetHashDataFTPInfo();
	std::string downloadData;
	if( !_FTPFileDownload( hashDataFTPInfo.m_Host, hashDataFTPInfo.m_Port, hashDataFTPInfo.m_ID, hashDataFTPInfo.m_PW, hashDataFTPInfo.m_Directory, hashDataFTPInfo.m_FileName, downloadData ) )
		return;

	MAP_HASH_CONTANER	RecvHashData;
	if( gzip::is_compressed( downloadData.c_str(), downloadData.size() ) )
	{
		std::string strdeCom = gzip::decompress( downloadData.c_str(), downloadData.size() );	
		_MaekCheckData( strdeCom, RecvHashData );
	}
	else
		_MaekCheckData( downloadData, RecvHashData );
	
	std::string toString = _MakeHashDataToString( RecvHashData );
	_SaveFile( toString, hashDataFTPInfo.m_FileName, false );
}

// Quando os dados de hash são alterados, os dados de hash alterados são enviados para o servidor.
// Função para atualizar informações
void CIntegrityTool::OnBnClickedUpdataServerHash()
{
	sFTPInfo const& hashDataFTPInfo = m_ConfigeData.GetHashDataFTPInfo();

	if( DmCS::StringFn::iCmp( hashDataFTPInfo.m_ID, m_ConfigeData.GetUserConnectionID() ) )
	{
		CsMessageBoxA( MB_OK, "A ID de acesso FTP nao deve ser igual a ID do usuario." );
		return;
	}

	sFTPInfo const& serverFTPInfo = m_ConfigeData.GetServerUploadFTPInfo();
	std::string serverEncyHashData;
	if( !_FTPFileDownload( serverFTPInfo.m_Host, serverFTPInfo.m_Port, serverFTPInfo.m_ID, serverFTPInfo.m_PW, serverFTPInfo.m_Directory, serverFTPInfo.m_FileName, serverEncyHashData ) )
	{
		int nRe = CsMessageBoxA( MB_OKCANCEL, "O servidor FTP nao possui arquivos. Você gostaria de criar um novo?" );
		if( nRe != IDOK )
			return;
	}

	std::string serverHashData;
	if( !serverEncyHashData.empty() )
	{
		if( !SimpleSecurity::SecuritySelf::Decript( serverEncyHashData, serverHashData ) )// 암호해체 실패
		{
			int nRe = CsMessageBoxA( MB_OKCANCEL, "Os dados recebidos do servidor FTP nso sso criptografados. Gostaria de criar um novo?" );
			if( nRe != IDOK )
				return;
		}
	}

	dm_stringvec infos;
	DmCS::StringFn::Cut( serverHashData, ",", infos);

	std::string downloadData;
	if( !_FTPFileDownload( hashDataFTPInfo.m_Host, hashDataFTPInfo.m_Port, hashDataFTPInfo.m_ID, hashDataFTPInfo.m_PW, hashDataFTPInfo.m_Directory, hashDataFTPInfo.m_FileName,downloadData ) )
	{
		CsMessageBoxA( MB_OK, "Falha ao baixar o arquivo de dados de hash FTP" );
		return;
	}

	std::string hashFileHash = DmCS::sha256( downloadData );

	//while( infos.size() < 14 )
	while( infos.size() < 13 )
		infos.push_back("");

	sFTPInfo const& userHashLogFTPInfo = m_ConfigeData.GetUserHashLogFTPInfo();
	dm_stringvec::iterator it = infos.begin();
	for( int n = 0; it != infos.end(); ++it, ++n )
	{
		switch(n)
		{
		case 0:		(*it) = hashFileHash;							break;
		case 1:		(*it) = hashDataFTPInfo.m_Host;					break;
		case 2:		(*it) = hashDataFTPInfo.GetPortA();				break;
		case 3:		(*it) = m_ConfigeData.GetUserConnectionID();	break;
		case 4:		(*it) = m_ConfigeData.GetUserConnectionPW();	break;
		case 5:		(*it) = hashDataFTPInfo.m_Directory;			break;
		case 6:		(*it) = hashDataFTPInfo.m_FileName;				break;
		case 7:		(*it) = userHashLogFTPInfo.m_Host;				break;
		case 8:		(*it) = userHashLogFTPInfo.GetPortA();			break;
		case 9:		(*it) = userHashLogFTPInfo.m_ID;				break;
		case 10:	(*it) = userHashLogFTPInfo.m_PW;				break;
		case 11:	(*it) = userHashLogFTPInfo.m_Directory;			break;
		case 12:	(*it) = userHashLogFTPInfo.m_FileName;			break;
// 		case 13:
// 			{
// 				if( !m_ClientExeHashData.empty() )
// 					(*it) = m_ClientExeHashData;				
// 			}break;			
		}
	}

	std::string saveData;
	it = infos.begin();
	for( int n = 0 ; it != infos.end(); ++it, ++n )
	{
		if( 0 == n )	DmCS::StringFn::Add( saveData, *it );
		else			DmCS::StringFn::Add( saveData, *it, "," );
	}

	{// 암호화 하기전의 파일
		std::string saveFileName = DmCS::FileFn::GetFileName(serverFTPInfo.m_FileName.c_str());
		std::string saveFileExt = DmCS::FileFn::GetFileExt(serverFTPInfo.m_FileName.c_str());
		std::string orgFile = saveFileName + "_org." + saveFileExt;
		if( !_SaveFile( saveData, orgFile, false ) )
			return;

		_FTPFileUpload( serverFTPInfo.m_Host, serverFTPInfo.m_Port, serverFTPInfo.m_ID, serverFTPInfo.m_PW, serverFTPInfo.m_Directory, orgFile );	
	}

	{
		std::string en;
		if( !SimpleSecurity::SecuritySelf::Encript( saveData, en ) )
		{
			CsMessageBoxA( MB_OK, "A criptografia dos dados de hash a serem enviados ao servidor falhou" );	
			return;
		}

		if( !_SaveFile( en, serverFTPInfo.m_FileName, false ) )
			return;

		_FTPFileUpload( serverFTPInfo.m_Host, serverFTPInfo.m_Port, serverFTPInfo.m_ID, serverFTPInfo.m_PW, serverFTPInfo.m_Directory, serverFTPInfo.m_FileName );
	}

	OnBnClickedServerhashdataRef();
}

//////////////////////////////////////////////////////////////////////////
// 파일 명으로 해쉬 데이터 파일 저장
bool CIntegrityTool::_SaveFile(std::string const& saveData, std::string const& saveFileName, bool bCompress)
{
	if( saveFileName.empty() )
	{
		CsMessageBoxA( MB_OK, "Nao ha nome de arquivo para salvar." );
		return false;
	}

	if( saveData.empty()  )
	{
		CsMessageBoxA( MB_OK, "Nao ha dados para salvar." );
		return false;
	}

	FILE *file = NULL;
	fopen_s(&file, saveFileName.c_str(), "wb");
	//파일에 0x0A("\n") 입력시 0x0D 가 저장되는 오류 때문에
	// 파일에 쓸때 파일 저장 형식을 바이너리 형식으로 저장시켜야함

	if( NULL == file )
	{
		CsMessageBoxA( MB_OK, "A criacao do arquivo de saida falhou(%s)", saveFileName.c_str() );
		return false;
	}

	if( bCompress )
	{
		if( !gzip::is_compressed( saveData.c_str(), saveData.size() ) )
		{
			std::string strcompress = gzip::compress( saveData.c_str(), saveData.size() );
			fwrite( strcompress.c_str(), sizeof( char ), strcompress.size(), file );
		}	
		else
			fwrite( saveData.c_str(), sizeof( char ), saveData.size(), file );		
	}
	else
	{
		if( gzip::is_compressed( saveData.c_str(), saveData.size() ) )
		{
			std::string strcompress = gzip::decompress( saveData.c_str(), saveData.size() );
			fwrite( strcompress.c_str(), sizeof( char ), strcompress.size(), file );
		}
		else
			fwrite( saveData.c_str(), sizeof( char ), saveData.size(), file );
	}

	fclose( file );
	return true;
}

struct sFTPErrorLog : public nsFTP::CFTPClient::CNotification
{
public:
	sFTPErrorLog() {};
	virtual ~sFTPErrorLog() {};

	virtual void OnSendCommand(const tstring& strCommand)
	{
		if( strCommand.length()==0 )
			return;

		if( strCommand.length()>4 && strCommand.substr(5)==_T("PASS ") )
			tErrorLog += _T("< PASS **********\r\n");
		else
		{
			tErrorLog +=_T("> ");
			tErrorLog += strCommand;
			tErrorLog +=_T(NEWLINE);
		}
	}

	virtual void OnResponse(const nsFTP::CReply& Reply)
	{
		if( Reply.Value().length()==0 )
			return;

		tErrorLog +=_T("< ");
		tErrorLog += Reply.Value();
		tErrorLog +=_T(NEWLINE);
	}

	std::string GetLog() 
	{
		std::string log;
		log.assign( tErrorLog.begin(), tErrorLog.end() );
		return log;
	}

	void clear()
	{
		tErrorLog.clear();
	}

private:
	tstring tErrorLog;
};

sFTPErrorLog g_FTPLoger;

//////////////////////////////////////////////////////////////////////////
bool CIntegrityTool::_FTPFileDownload( std::string const& sIp,			// 업로드 ftp IP
									  unsigned int const& nPort,		// 업로드 ftp Port
									  std::string const& sID,		// 업로드 ID
									  std::string const& sPW,	// 업로드 PW
									  std::string const& sPath,		// 업로드 폴더
									  std::string const& sDownloadFile,
									  std::string & downloadString)
{
	downloadString.clear();

	tstring wsIP;
	wsIP.assign( sIp.begin(), sIp.end() );

	tstring wsID;
	wsID.assign( sID.begin(), sID.end() );

	tstring wsPW;
	wsPW.assign( sPW.begin(), sPW.end() );

	tstring wsFilePath;
	std::string pathFile = sPath + sDownloadFile;
	wsFilePath.assign( pathFile.begin(), pathFile.end() );


	nsFTP::CFTPClient ftpClientsource;
	nsFTP::CLogonInfo logonInfoSource;
	logonInfoSource.SetHost( wsIP, nPort, wsID, wsPW );
	//ftpClientsource.AttachObserver(&g_FTPLoger);
	if( !ftpClientsource.Login( logonInfoSource ) )
		return false;

	nsFTP::COutputStream outputStream(_T("\r\n"), _T("fileName"));
	// do file operations
	if( !ftpClientsource.DownloadFile(wsFilePath, outputStream, nsFTP::CRepresentation(nsFTP::CType::Image()), true) )
	{
		ftpClientsource.Logout();
		return false;
	}

	//std::string log = g_FTPLoger.GetLog();

	tstring output = outputStream.GetBuffer();

	downloadString.assign(output.begin(), output.end());

	// disconnect
	ftpClientsource.Logout();
	return true;
}

// URL 로부터 해쉬 데이터 파일 받음
bool CIntegrityTool::_FTPFileUpload( std::string const& sIp,			// 업로드 ftp IP
									unsigned int const& nFtpPort,		// 업로드 ftp Port
									std::string const& userId,		// 업로드 ID
									std::string const& userPassword,	// 업로드 PW
									std::string const& ftpPath,		// 업로드 폴더
									std::string const& uploadFile )	// subir arquivo
{
	tstring wsIP;
	DmCS::StringFn::From( wsIP, sIp );
	//wsIP.assign( sIp.begin(), sIp.end() );

	tstring wsID;
	DmCS::StringFn::From( wsID, userId );
	//wsID.assign( userId.begin(), userId.end() );

	tstring wsPW;
	DmCS::StringFn::From( wsPW, userPassword );
	//wsPW.assign( userPassword.begin(), userPassword.end() );

	tstring wsUploadDir;	
	std::string sUloadDir = DmCS::FileFn::FindFilePath( ftpPath.c_str() );
	DmCS::StringFn::From( wsUploadDir, sUloadDir );
	//wsUploadDir.assign( ftpPath.begin(), ftpPath.end() );

	tstring wsUploadFileFullPath;
	DmCS::StringFn::From( wsUploadFileFullPath, uploadFile );
	//wsUploadFileFullPath.assign( uploadFile.begin(), uploadFile.end() );

	tstring wsUploadFile;
	std::string sUloadFile = nsCSFILE::GetFileName( uploadFile.c_str() );
	DmCS::StringFn::From( wsUploadFile, sUloadFile );
	//wsUploadFile.assign( sUloadFile.begin(), sUloadFile.end() );

	nsFTP::CFTPClient ftpClientsource;
	nsFTP::CLogonInfo logonInfoSource;
	logonInfoSource.SetHost( wsIP, nFtpPort, wsID, wsPW );

	if( !ftpClientsource.Login( logonInfoSource ) )
		return false;

	ftpClientsource.Delete( wsUploadDir + wsUploadFile );

	bool bUpResult = ftpClientsource.UploadFile(wsUploadFileFullPath, wsUploadDir + wsUploadFile, false, nsFTP::CRepresentation(nsFTP::CType::Image()), true);

	// disconnect
	ftpClientsource.Logout();

	return bUpResult;
}

void CIntegrityTool::OnBnClickedUpdataPatchData()
{
	sFTPInfo const& UploadFtpInfo = m_ConfigeData.GetPatchUploadFTPInfo();

	// 업로드 데이터 정보로 파일을 업로드 함.
	if( !DmCS::FileFn::IsAccessFile( UploadFtpInfo.m_FileName.c_str() ) )
	{
		CsMessageBoxA( MB_OK, "패치 서버에 업로드할 파일이 없습니다.\n%s", UploadFtpInfo.m_FileName.c_str() );
		return;
	}

	int nRe = CsMessageBoxA( MB_OKCANCEL, "패치 서버에 업로드 하시겠습니까?\nPath:%s\nFile:%s", UploadFtpInfo.m_Directory.c_str(), UploadFtpInfo.m_FileName.c_str() );
	if( nRe == IDOK )
	{
		if( _FTPFileUpload( UploadFtpInfo.m_Host, UploadFtpInfo.m_Port, UploadFtpInfo.m_ID, UploadFtpInfo.m_PW, UploadFtpInfo.m_Directory, UploadFtpInfo.m_FileName ) )
			CsMessageBoxA( MB_OK, "패치 서버에 업로드 되었습니다..\n%s\n%s", UploadFtpInfo.m_Directory.c_str(), UploadFtpInfo.m_FileName.c_str() );
		else
			CsMessageBoxA( MB_OK, "패치 서버에 업로드 실패.\n%s", UploadFtpInfo.m_FileName.c_str() );
	}
}

void CIntegrityTool::OnBnClickedButton_PatchFileFind()
{
	CString currDir;
	m_PatchUploadFTP_File.GetWindowText( currDir );

	if( currDir.IsEmpty() )
	{
		WCHAR path[MAX_PATH] = {0,};  // 파일이 있는 위치를 current directory로 해서 처리한다.	
		GetCurrentDirectory(MAX_PATH, path);
		currDir = path;
	}

	TCHAR tstrPathName[MAX_PATH] = _T("");
	BROWSEINFO   bi; 
	ZeroMemory(&bi,sizeof(BROWSEINFO));
	bi.hwndOwner   =   this->m_hWnd; 
	bi.pszDisplayName = tstrPathName;  // 선택된 디렉토리명 저장 
	bi.lpszTitle = L"폴더를 선택해 주세요"; 
	bi.lParam = (LPARAM)(LPCTSTR)currDir.GetBuffer();
	bi.ulFlags  =  BIF_EDITBOX    // 선택된 디렉토리명을 표시하고 편집할 수 있는 창
		| BIF_NEWDIALOGSTYLE             // 새로운 디렉토리를 만들수 있는 버튼 추가 
		| BIF_BROWSEINCLUDEFILES; 
	bi.lpfn = BrowseCallbackProc;
	LPITEMIDLIST idl = SHBrowseForFolder(&bi);

	if (idl) { 
		TCHAR changePath[MAX_PATH] = _T("");
		SHGetPathFromIDList(idl, changePath); // 전체 PATH을 포함한 디렉토리명

		m_PatchUploadFTP_File.SetWindowText( changePath );

		m_ConfigeData.SetPatchInfo_File( changePath );
	}
}
//////////////////////////////////////////////////////////////////////////
void CIntegrityTool::OnEnChangeEditUploadurl()
{
	CString change;
	m_UploadUrlEdit.GetWindowText( change );
	m_ConfigeData.SetHashFTPInfo_Host( change.GetBuffer() );
}

void CIntegrityTool::OnEnChangeEditUploadPort()
{
	CString change;
	m_PortEdit.GetWindowText( change );
	m_ConfigeData.SetHashFTPInfo_Port( change.GetBuffer() );
}

void CIntegrityTool::OnEnChangeEditId()
{
	CString change;
	m_IdEdit.GetWindowText( change );
	m_ConfigeData.SetHashFTPInfo_ID( change.GetBuffer() );
}

void CIntegrityTool::OnEnChangeEditPassword()
{
	CString change;
	m_PasswordEdit.GetWindowText( change );
	m_ConfigeData.SetHashFTPInfo_PW( change.GetBuffer() );
}

void CIntegrityTool::OnEnChangeEditUploaddirectory()
{
	CString change;
	m_UploadDirectoryEdit.GetWindowText( change );
	m_ConfigeData.SetHashFTPInfo_Directory( change.GetBuffer() );
}

void CIntegrityTool::OnEnChangeEditUploadFile()
{
	CString change;
	m_UploadHashFileNameEdit.GetWindowText( change );
	m_ConfigeData.SetHashFTPInfo_File( change.GetBuffer() );
	m_HashFileName.SetWindowText( change );
}
//////////////////////////////////////////////////////////////////////////
void CIntegrityTool::OnEnChangeEditServerUrl()
{
	CString change;
	m_ServerConnectHost.GetWindowText( change );
	m_ConfigeData.SetServerInfo_Host( change.GetBuffer() );
}

void CIntegrityTool::OnEnChangeEditServerPort()
{
	CString change;
	m_ServerConnectPort.GetWindowText( change );
	m_ConfigeData.SetServerInfo_Port( change.GetBuffer() );
}

void CIntegrityTool::OnEnChangeEditServerRequestPort()
{
	CString change;
	m_ServerRequestPort.GetWindowText( change );
	m_ConfigeData.SetServerInfo_RequestPort( change.GetBuffer() );
}

void CIntegrityTool::OnEnChangeEditServerId()
{
	CString change;
	m_ServerConnectID.GetWindowText( change );
	m_ConfigeData.SetServerInfo_ID( change.GetBuffer() );
}

void CIntegrityTool::OnEnChangeEditServerpw()
{
	CString change;
	m_ServerConnectPW.GetWindowText( change );
	m_ConfigeData.SetServerInfo_PW( change.GetBuffer() );
}

void CIntegrityTool::OnEnChangeEditServerHashfilename()
{
	CString change;
	m_ServerDirectory.GetWindowText( change );
	m_ConfigeData.SetServerInfo_Directory( change.GetBuffer() );
}

void CIntegrityTool::OnEnChangeEditServerUploadFile()
{
	CString change;
	m_ServerHashFileName.GetWindowText( change );
	m_ConfigeData.SetServerInfo_File( change.GetBuffer() );
}
//////////////////////////////////////////////////////////////////////////
void CIntegrityTool::OnEnChangeEditPatchUploadHost()
{
	CString change;
	m_PatchUploadFTP_Host.GetWindowText( change );
	m_ConfigeData.SetPatchInfo_Host( change.GetBuffer() );
}

void CIntegrityTool::OnEnChangeEditPatchUploadPort()
{
	CString change;
	m_PatchUploadFTP_Port.GetWindowText( change );
	m_ConfigeData.SetPatchInfo_Port( change.GetBuffer() );
}

void CIntegrityTool::OnEnChangeEditPatchUploadId()
{
	CString change;
	m_PatchUploadFTP_ID.GetWindowText( change );
	m_ConfigeData.SetPatchInfo_ID( change.GetBuffer() );
}

void CIntegrityTool::OnEnChangeEditPatchUploadPw()
{
	CString change;
	m_PatchUploadFTP_PW.GetWindowText( change );
	m_ConfigeData.SetPatchInfo_PW( change.GetBuffer() );
}

void CIntegrityTool::OnEnChangeEditPatchUploadPath()
{
	CString change;
	m_PatchUploadFTP_Path.GetWindowText( change );
	m_ConfigeData.SetPatchInfo_Directory( change.GetBuffer() );
}

void CIntegrityTool::OnEnChangeEditPatchUploadFile()
{
	CString change;
	m_PatchUploadFTP_File.GetWindowText( change );
	m_ConfigeData.SetPatchInfo_File( change.GetBuffer() );
}
//////////////////////////////////////////////////////////////////////////
void CIntegrityTool::OnEnChangeEditUserConnectionId()
{
	CString change;
	m_UserConnectionID.GetWindowText( change );
	m_ConfigeData.SetUserConnectionID( change.GetBuffer() );
}

void CIntegrityTool::OnEnChangeEditUserConnectionPw()
{
	CString change;
	m_UserConnectionPW.GetWindowText( change );
	m_ConfigeData.SetUserConnectionPW( change.GetBuffer() );
}


void CIntegrityTool::OnBnClickedButton2()
{
	CString currentDir;
	WCHAR path[MAX_PATH] = {0,};
	GetCurrentDirectory(MAX_PATH, path);
	currentDir = path;

	TCHAR tstrPathName[MAX_PATH] = _T("");
	BROWSEINFO   bi; 
	ZeroMemory(&bi,sizeof(BROWSEINFO));
	bi.hwndOwner   =   this->m_hWnd; 
	bi.pszDisplayName = tstrPathName;  // 선택된 디렉토리명 저장 
	bi.lpszTitle = L"폴더를 선택해 주세요"; 
	bi.lParam = (LPARAM)(LPCTSTR)currentDir.GetBuffer();
	bi.ulFlags  =  BIF_EDITBOX    // 선택된 디렉토리명을 표시하고 편집할 수 있는 창
		| BIF_NEWDIALOGSTYLE             // 새로운 디렉토리를 만들수 있는 버튼 추가 
		| BIF_BROWSEINCLUDEFILES; 
	bi.lpfn = BrowseCallbackProc;
	LPITEMIDLIST idl = SHBrowseForFolder(&bi);

	if (idl) { 
		TCHAR changePath[MAX_PATH] = _T("");
		SHGetPathFromIDList(idl, changePath); // 전체 PATH을 포함한 디렉토리명

		DmCS::StringFn::ToMB( changePath, m_ConfigFileName );
		OnBnClickedButtonReloadConfig();
	}
}
//////////////////////////////////////////////////////////////////////////
void CIntegrityTool::OnEnChangeEditHashlogHost()
{
	CString change;
	m_UserHashLogFTP_Host.GetWindowText( change );
	m_ConfigeData.SetUserHashLogFTPInfo_Host( change.GetBuffer() );
}

void CIntegrityTool::OnEnChangeEditHashlogPath()
{
	CString change;
	m_UserHashLogFTP_Path.GetWindowText( change );
	m_ConfigeData.SetUserHashLogFTPInfo_Directory( change.GetBuffer() );
}

void CIntegrityTool::OnEnChangeEditHashlogPort()
{
	CString change;
	m_UserHashLogFTP_Port.GetWindowText( change );
	m_ConfigeData.SetUserHashLogFTPInfo_Port( change.GetBuffer() );
}

void CIntegrityTool::OnEnChangeEditHashlogId()
{
	CString change;
	m_UserHashLogFTP_ID.GetWindowText( change );
	m_ConfigeData.SetUserHashLogFTPInfo_ID( change.GetBuffer() );
}

void CIntegrityTool::OnEnChangeEditHashlogPw()
{
	CString change;
	m_UserHashLogFTP_PW.GetWindowText( change );
	m_ConfigeData.SetUserHashLogFTPInfo_PW( change.GetBuffer() );
}

void CIntegrityTool::OnEnChangeEditHashlogFilename()
{
	CString change;
	m_UserHashLogFTP_File.GetWindowText( change );
	m_ConfigeData.SetUserHashLogFTPInfo_File( change.GetBuffer() );	
}
//////////////////////////////////////////////////////////////////////////
void CIntegrityTool::OnDropFiles(HDROP hDropInfo)
{
	WCHAR szPathName[MAX_PATH]={0,};
	int nFiles = ::DragQueryFile(hDropInfo,0xFFFFFFFF, szPathName, MAX_PATH);
	for( int n = 0; n < nFiles; ++n )
	{
		::DragQueryFile( hDropInfo, n, szPathName, MAX_PATH );
		m_PatchUploadFTP_File.SetWindowText( szPathName );
		break;
	}

	::DragFinish(hDropInfo);
	CDialog::OnDropFiles(hDropInfo);
}

///////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// Uma função que sopra pacotes para atualizar o hash no servidor
//////////////////////////////////////////////////////////////////////////
//void CIntegrityTool::OnBnClickedServerhashdataRef()
//{
//	// TODO: Adicione seu código do manipulador de notificação de controle aqui.
//	class cCliServer : public cClient
//	{
//	public:
//		cCliServer():cClient(nSession::Resource),bOnConnect(false),bSendSuccess(false){};
//		~cCliServer() 
//		{
//			cClient::Stop();
//		};	
//
//	public:
//		bool	OnRun()
//		{
//			if( !Run(1) )
//				return false;
//
//			while(1)
//			{
//				if( bOnConnect )
//					break;
//				Sleep(100);
//			}
//
//			if( !bSendSuccess )
//				return false;
//
//			return true;
//		};
//	protected:
//		virtual void OnConnected(void)
//		{
//			u8 ff = 0x1963a81b82fa9c7d;
//			newp( pSvr::ResourceHashReload );
//			push( ff );
//			endp(pSvr::ResourceHashReload);
//			DoSend();
//
//			bSendSuccess = true;
//			bOnConnect = true;
//		};			// 연결 성공시 호출됨
//		virtual void OnExecute(void)
//		{
//		};			// 패킷 수신시 호출됨
//		virtual void OnDisconnected(char *msg)
//		{
//			bOnConnect = true;
//		};	//Chamado ao sair, não execute xdelete(this) dentro da função.
//
//
//	public:
//		bool bOnConnect;
//		bool bSendSuccess;
//	};
//
//	cCliServer net;
//	bool bResult = net.Bind();
//	if( !bResult )
//	{
//		CsMessageBoxA( MB_OK, "A função Server Bind falhou. Falha.\nEntre em contato com o programador do servidor." );	
//		return;
//	}
//
//	sFTPInfo const& hashDataFTPInfo = m_ConfigeData.GetServerUploadFTPInfo();
//	std::string serverIp = hashDataFTPInfo.m_Host;
//	if( !net.Connect( const_cast<char*>(serverIp.c_str()), hashDataFTPInfo.m_RequestPort ) )
//	{
//		CsMessageBoxA( MB_OK, "Falha na função de conexão do servidor.\nEntre em contato com o programador do servidor.\nIP do servidor : %s, Port : %d", serverIp.c_str(), hashDataFTPInfo.m_RequestPort );	
//		return;
//	}
//
//	if( !net.OnRun() )
//	{
//		CsMessageBoxA( MB_OK, "A solicitação de atualização de hash para o servidor falhou.\nEntre em contato com o programador do servidor");
//		return;
//	}
//	CsMessageBoxA( MB_OK, "Solicitação de atualização de hash para o servidor bem-sucedida");
//}

void CIntegrityTool::OnBnClickedButtonResourceEdit()
{
	// TODO: Adicione seu código do manipulador de notificação de controle aqui.
	sFTPInfo const& hashDataFTPInfo = m_ConfigeData.GetHashDataFTPInfo();
	std::string downloadData;
	// FTP로 부터 기존 해쉬 데이터를 받는다.
	bool bDown = _FTPFileDownload( hashDataFTPInfo.m_Host, hashDataFTPInfo.m_Port, hashDataFTPInfo.m_ID, hashDataFTPInfo.m_PW, hashDataFTPInfo.m_Directory, hashDataFTPInfo.m_FileName, downloadData );
	if( !bDown )
	{
		CsMessageBoxA( MB_OK, "Falha ao baixar o arquivo existente." );
		return;
	}

	MAP_HASH_CONTANER	RecvHashData;
	// Descompacte os dados de hash recebidos.
	if( gzip::is_compressed( downloadData.c_str(), downloadData.size() ) )
	{
		std::string strdeCom = gzip::decompress( downloadData.c_str(), downloadData.size() );	
		_MaekCheckData( strdeCom, RecvHashData );
	}
	else
		_MaekCheckData( downloadData, RecvHashData );

	_DlgResourceView dlg(this);
	dlg.SetHashData(RecvHashData);
	dlg.DoModal();
}

void CIntegrityTool::ChangeHashData(MAP_HASH_CONTANER const& changeHashData )
{
	// Converter dados atualizados em string
	sFTPInfo const& hashDataFTPInfo = m_ConfigeData.GetHashDataFTPInfo();
	std::string saveData = _MakeHashDataToString( changeHashData );

	// Comprimido.
	std::string strCompress = gzip::compress( saveData.c_str(), saveData.size() );	

	// salvar string em arquivo
	if( !_SaveFile( strCompress, hashDataFTPInfo.m_FileName ) )
		return;

	while(TRUE)
	{
		if( _FTPFileUpload( hashDataFTPInfo.m_Host, hashDataFTPInfo.m_Port, hashDataFTPInfo.m_ID, hashDataFTPInfo.m_PW, hashDataFTPInfo.m_Directory, hashDataFTPInfo.m_FileName ) )
		{
			CsMessageBoxA( MB_OK, "Carregamento de arquivo hash bem-sucedido \n%s\n%s", hashDataFTPInfo.m_FileName.c_str(), hashDataFTPInfo.m_Directory.c_str() );
			break;
		}

		int nResult = CsMessageBoxA( MB_OKCANCEL, "Falha ao fazer upload do arquivo hash. Tentar novamente?\n%s\n%s", hashDataFTPInfo.m_FileName.c_str(), hashDataFTPInfo.m_Directory.c_str() );
		if( nResult != IDOK )
			break;
		else
			return;
	}

	OnBnClickedUpdataServerHash();
}