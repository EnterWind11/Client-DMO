//---------------------------------------------------------------------------
// 
// File Name  : GameApp.cpp
// Date       :
// Author     :
// Description: 
// 
//---------------------------------------------------------------------------

#include "StdAfx.h"
#include "GameApp.h"

#include <mutex>

#include "../Flow/FlowMgr.h"
#include "../Flow/Flow.h"

#include "../ContentsSystem/ContentsSystemDef.h"
#include "../ContentsSystem/ContentsSystem.h"

#ifdef DEF_CORE_NPROTECT
#include "../nProtect/Client_nProtect.h"
#define CHECK_NPROTECT_TIME 5.0f
#endif

CsCriticalSection __g_GlobalCS;
int __g_nGlobalState = 0;

void SetGlobalState(int nState) {
    __g_GlobalCS.Lock();
    __g_nGlobalState = nState;
    __g_GlobalCS.Unlock();
}

int GetGlobalState() {
    __g_GlobalCS.Lock();
    int nReturn = __g_nGlobalState;
    __g_GlobalCS.Unlock();
    return nReturn;
}

/**
 * Thread_LoadFileTable()
 * 
 * Description:
 *  - Loads various file tables needed by the application.
 *  - Configures usage flags and language settings based on the country/region.
 */
void Thread_LoadFileTable() {
    // "List of items not to load"
    nsCsMapTable::g_bUseMapStart = false;
    nsCsMapTable::g_bUseMapResurrection = false;
    nsCsMapTable::g_eModeMapMonster = nsCsMapTable::eMode_Client;

    // "Special loading options"
    nsCsFileTable::g_bAddExp = true;
    nsCsFileTable::g_bUseMoveObject = true;
    nsCsFileTable::g_bUseHelp = true;
    nsCsFileTable::g_bUseAchieve = true;
    nsCsFileTable::g_bAddExp = true;
    nsCsFileTable::g_bBuffMng = true;
    nsCsFileTable::g_bSceneDataMng = true;
    
#ifndef BATTLE_MATCH
    nsCsFileTable::g_bUseEvoExtra = false;
    nsCsFileTable::g_bUseEvoBattle = false;
#endif

    nsCsFileTable::g_bUseCashShop = true;
    nsCsFileTable::g_bUseGotcha = true;

    // Sets file table language based on the country/region
    if (GLOBALDATA_ST.IsCountry(GData::eCountry_Aeria))
        g_pResist->m_Global.s_eFTLanguage = nsCsFileTable::ENGLISH_A;
    else if (GLOBALDATA_ST.IsCountry(GData::eCountry_GSP))
        g_pResist->m_Global.s_eFTLanguage = nsCsFileTable::ENGLISH;
    else if (GLOBALDATA_ST.IsCountry(GData::eCountry_Steam))
        g_pResist->m_Global.s_eFTLanguage = nsCsFileTable::ENGLISH;
    else if (GLOBALDATA_ST.IsCountry(GData::eCountry_Kor))
        g_pResist->m_Global.s_eFTLanguage = nsCsFileTable::KOREA_TRANSLATION;
    

#ifdef _DEBUG
    if (g_bUseFilePack)
        nsCsFileTable::g_eFileType = nsCsFileTable::FT_FILEPACK;
    else
        nsCsFileTable::g_eFileType = nsCsFileTable::FT_BIN;
#elif defined _GIVE
    if (g_bUseFilePack)
        nsCsFileTable::g_eFileType = nsCsFileTable::FT_FILEPACK;
    else
        nsCsFileTable::g_eFileType = nsCsFileTable::FT_BIN;
    nsCsFileTable::g_eFileType = nsCsFileTable::FT_BIN;
#elif defined NIRELEASE
    if (g_bUseFilePack)
        nsCsFileTable::g_eFileType = nsCsFileTable::FT_FILEPACK;
    else
        nsCsFileTable::g_eFileType = nsCsFileTable::FT_BIN;
#else
    nsCsFileTable::g_eFileType = nsCsFileTable::FT_BIN;
#endif

#ifdef PC_BANG_SERVICE_TEST // PC방 테스트 관련 알파서버 접속 클라 생성
    // "PC room test-related alpha server client creation"
    nsCsFileTable::g_eFileType = nsCsFileTable::FT_FILEPACK;
#endif

    // Initialize the FileTable Manager
    if (nsCsFileTable::g_FileTableMng.Init(nsCsFileTable::g_eFileType,
                                           g_pResist->m_Global.s_eFTLanguage) == false)
    {
        SetGlobalState(2);
        return;
    }

#ifndef _DEBUG
#ifndef _GIVE
#ifndef NIRELEASE
#ifndef PC_BANG_SERVICE_TEST
    // "Reload data for location finding using bin files"
    // 위치찾기에 필요한 데이터 bin파일로 Reload
    char cPath[MAX_PATH];
    nsCsFileTable::g_FileTableMng.GetLanguagePath(g_pResist->m_Global.s_eFTLanguage, cPath);
    bool bResult = nsCsFileTable::g_pQuestMng->Reload(cPath);
    assert_cs(bResult);
    nsCsMapTable::g_pMapMonsterMng->Reload(cPath);
#endif
#endif
#endif
#endif

    SetGlobalState(1);
}

/**
 * Checks if the given file name references a UI texture.
 * @return true if it's a UI texture, false otherwise.
 */
bool IsUiTexture(const char* pName) {
    assert_cs(pName[0] != '\\');

    bool bUiTex = false;

    if (pName[0] == '.')
        bUiTex = (strnicmp(&pName[2], "data\\Interface", 14) == 0);
    else
        bUiTex = (strnicmp(pName, "data\\Interface", 14) == 0);

    if (bUiTex) {
        // _access_s checks file existence
        return (_access_s(pName, 0) == 0);
    }
    return false;
}


/**
 * Custom file creation function for file pack usage.
 * This allows the game engine to load data from memory or from a real file 
 * depending on whether the file is present in the pack.
 */
NiFile* CsFilePackFileCreateFunc(const char* pcName, 
                                 NiFile::OpenMode eMode, 
                                 unsigned int uiBufferSize) 
{
#ifdef SDM_USER_UI_SKIN_CHANGE_20160331
    if (IsUiTexture(pcName)) {
        return NiNew NiFile(pcName, eMode, uiBufferSize);
    }
#endif

    CsFPS::CsFileHash::sINFO* pHashInfo = CsFPS::CsFPSystem::GetHashData(0, pcName);
    if (pHashInfo == NULL) {
        return NiNew NiFile(pcName, eMode, uiBufferSize);
    }

    char* pBuffer = NiAlloc(char, pHashInfo->s_nDataSize);
    if (pBuffer == NULL) {
        // If the allocation fails, return NULL
        return nullptr;
    }

    CsFPS::CsFPSystem::GetFileData(0, &pBuffer, pHashInfo->s_nOffset, pHashInfo->s_nDataSize);
    return NiNew NiMemFile(pBuffer, NiFile::READ_ONLY, pHashInfo->s_nDataSize);
}

/**
 * Determines whether the engine should treat the file name as a packed file.
 * Only allows "data\\" paths.
 */
bool CsFilePackFileAccessFunc(const char* pcName, NiFile::OpenMode /* eMode */) {
    assert_cs(pcName[0] != '\\');

    if (pcName[0] == '.')
        return (strnicmp(&pcName[2], "data\\", 5) == 0);

    return (strnicmp(pcName, "data\\", 5) == 0);
}

namespace App {
    //---------------------------------------------------------------------------
    CGameApp::CGameApp(): m_bInitialStarting(false), m_fCheckGameGuardTimer(0.0f) {}
    //---------------------------------------------------------------------------
    CGameApp::~CGameApp() {}
    //---------------------------------------------------------------------------
    BOOL CGameApp::OnInitialize() {
        CREATE_SINGLETON(GameEventMng);

        CREATE_SINGLETON(Flow::CFlowMgr);
        if (!FLOWMGR_ST.Create()) {
            assert(!"Created FlowMgr Failed!");
            return FALSE;
        }

        CREATE_SINGLETON(COptionMng); // 파일 매니저

        if (OPTIONMNG_PTR)
            OPTIONMNG_PTR->LoadMachineOption();
        CREATE_SINGLETON(ContentsSystem);
        SAFE_POINTER_RETVAL(CONTENTSSYSTEM_PTR, FALSE);

        if (CONTENTSSYSTEM_PTR) {
            CONTENTSSYSTEM_PTR->BuildContents();
            CONTENTSSYSTEM_PTR->IntraConnection();
        }

        CREATE_SINGLETON(ResourceMng);
        if (RESOURCEMGR_STPTR)
            RESOURCEMGR_ST.Init();

        CREATE_SINGLETON(cGlobalInput);

        CREATE_SINGLETON(CCursor);
        if (CURSOR_STPTR)
            CURSOR_ST.Init(m_hInstance);

        // 엔진의 렌더러 생성
        if (CEngine::Init() == false)
            return FALSE;

        DxResolutionInfo::GlobalInit();

        if (g_pEngine->Create() == false)
            return FALSE;

        g_pResist->m_Global.CheckResolution();
        CalculateSize(g_pResist->m_Global.s_nResolutionWidth, g_pResist->m_Global.s_nResolutionHeight,
                      g_pResist->m_Global.s_bFullScreen);

        ReSize(GetWidth(), GetHeight(), GetFullMode(), true);

        g_nScreenWidth = GetWidth();
        g_nScreenHeight = GetHeight();

        g_pEngine->ChangeResolutionMode(GetFullMode(), GetWidth(), GetHeight());

        //CREATE_SINGLETON( CClock )
        //if(!CLOCK_ST.Create( NiNew CTimeSourceNi ) )
        //{
        //	assert(!"Created Clock Failed!");
        //	return FALSE;
        //}

        ////------------------------------------------------
         //// Init Setup
        //CLOCK_ST.SetMaxFrameRate(0.0f);
        //CLOCK_ST.SetCheckFps(TRUE);

        if (g_bUseFilePack) {
            // 파일패킹 콜백, 엔진 생성 이후에
            //CsMessageBox(MB_OK, _T("Loading File packs"));
            NiFile::SetFileCreateFunc(CsFilePackFileCreateFunc);
            NiFile::SetFileAccessFunc(CsFilePackFileAccessFunc);

            /*AIL_set_file_callbacks( Sound_file_open_callback,
            Sound_file_close_callback,
            Sound_file_seek_callback,
            Sound_file_read_callback );*/
        }

        CMngCollector::ShotInit();

        // 쓰레드로 파일테이블 로드
        sTCUnit* pUnit = sTCUnit::NewInstance(sTCUnit::LoadFileTable);
        pUnit->s_pLoadedObject = NULL;
        g_pThread->LoadChar(pUnit);

        bool bLoad = true;
        while (bLoad) {
            switch (GetGlobalState()) {
            case 0: Sleep(100);
                break;
            case 1: bLoad = false;
                break;
            case 2:
                bLoad = false;
                CsMessageBox(MB_OK, _LAN("파일테이블이 잘못 되었습니다"));
                return false;
            }
        }

        if (GAME_EVENT_STPTR)
            GAME_EVENT_STPTR->OnEvent(EVENT_CODE::TABLE_LOAD_SUCCESS);

        cIconMng::GlobalInit();
        cDataMng::GlobalInit();

        // 레지스트 설정된거 적용	
        nsCsGBTerrain::g_eTexFilter = (NiTexturingProperty::FilterMode)g_pResist->m_Global.s_eTexFilter;
        nsCsGBTerrain::g_bShadowRender = (g_pResist->m_Global.s_nShadowType == cResist::sGLOBAL::SHADOW_ON);
        nsCsGBTerrain::g_bCharOutLine = g_pResist->m_Global.s_bCharOutLine;
        nsCsGBTerrain::g_bSpeedCellRender = g_pResist->m_Global.s_bCell;
        g_pWeather->SetPerformance(g_pResist->m_Global.s_nWeather);
        CsC_AvObject::g_bEnableVoice = g_pResist->m_Global.s_bEnableVoice;


        CREATE_SINGLETON(CToolTipMng);
        if (TOOLTIPMNG_STPTR)
            TOOLTIPMNG_ST.Init();

        net::start();

#ifndef SKIP_LOGO
		FLOWMGR_ST.StartFlow(Flow::CFlow::FLW_LOGO);
#else
        FLOWMGR_ST.StartFlow(Flow::CFlow::FLW_LOGIN);
#endif

#ifdef DEF_CORE_NPROTECT
		m_fCheckGameGuardTimer = CHECK_NPROTECT_TIME;
#endif
        // 		if( GAME_EVENT_STPTR )
        // 			GAME_EVENT_STPTR->OnEvent(EVENT_CODE::START_RESOURCECHECKER);

        return TRUE;
    }

    //---------------------------------------------------------------------------
    void CGameApp::OnIdleExtern() {
#ifdef VERSION_STEAM
		STEAM_ST.SteamCallbackUpdate();
#endif
        // 네트웍 업데이트
        net::execute();

#ifdef DEF_CORE_NPROTECT
		m_fCheckGameGuardTimer -= g_fDeltaTime;
		if( m_fCheckGameGuardTimer < 0.0f )
		{
			m_fCheckGameGuardTimer = CHECK_NPROTECT_TIME;
			nProtect_Check();
		}
#endif
    }

    //---------------------------------------------------------------------------
    void CGameApp::OnIdle() {
        // Flow Idle
        if (FLOWMGR_STPTR)
            FLOWMGR_ST.OnIdle();
    }

    //---------------------------------------------------------------------------
    void CGameApp::OnTerminate() {
        //	SAFE_NIDELETE( g_pLoading );
        if (TOOLTIPMNG_STPTR)
            DESTROY_SINGLETON(TOOLTIPMNG_STPTR);

        BHPRT("TOOLTIPMNG_STPTR");

        if (CONTENTSSYSTEM_PTR) {
            CONTENTSSYSTEM_PTR->ClearWorldData();
            CONTENTSSYSTEM_PTR->ClearMainActorData();
            BHPRT("CONTENTSSYSTEM_PTR");
        }

        if (FLOWMGR_STPTR) {
            FLOWMGR_STPTR->LockFlow();
            FLOWMGR_STPTR->Destroy();
            DESTROY_SINGLETON(FLOWMGR_STPTR);
            BHPRT("FLOWMGR_STPTR");
        }

        g_Sorting.Delete();
        BHPRT("g_Sorting.Delete()");

        SAFE_NIDELETE(g_pGameIF);

        BHPRT("g_pGameIF");
        if (CONTENTSSYSTEM_PTR) {
            CONTENTSSYSTEM_PTR->PrepareDestroy();
            CONTENTSSYSTEM_PTR->RemoveAll();
            DESTROY_SINGLETON(CONTENTSSYSTEM_PTR);
            BHPRT("CONTENTSSYSTEM_PTR");
        }

        if (OPTIONMNG_PTR)
            DESTROY_SINGLETON(OPTIONMNG_PTR);
        BHPRT("OPTIONMNG_PTR");

        if (CURSOR_STPTR)
            DESTROY_SINGLETON(CURSOR_STPTR);
        BHPRT("CURSOR_STPTR");

        cIconMng::GlobalShotdown();
        BHPRT("cIconMng::GlobalShotdown()");
        CMngCollector::ShotDown();
        BHPRT("CMngCollector::ShotDown()");
        DxResolutionInfo::GlobalShotDown();
        BHPRT("DxResolutionInfo::GlobalShotDown()");
        nsDIRECTSHOW::GlobalShotDown();
        BHPRT("nsDIRECTSHOW::GlobalShotDown()");

        cGameInterface::GlobalShotDown();
        BHPRT("cGameInterface::GlobalShotDown()");
        nsCsFileTable::g_FileTableMng.Delete();
        BHPRT("nsCsFileTable::g_FileTableMng.Delete()");

        if (CURSOR_STPTR)
            DESTROY_SINGLETON(CURSOR_STPTR);
        BHPRT("CURSOR_STPTR");

        if (GLOBALINPUT_STPTR)
            DESTROY_SINGLETON(GLOBALINPUT_STPTR);
        BHPRT("GLOBALINPUT_STPTR");

        // 		// Clock Destroy
        // 		if(CLOCK_STPTR)
        // 			CLOCK_ST.Destroy();
        // 		DESTROY_SINGLETON(CLOCK_STPTR);

        if (GAME_EVENT_STPTR)
            DESTROY_SINGLETON(GAME_EVENT_STPTR);
        BHPRT("GAME_EVENT_STPTR");

        if (RESOURCEMGR_STPTR) {
            RESOURCEMGR_STPTR->End();
            DESTROY_SINGLETON(RESOURCEMGR_STPTR);
        }
    }

    //---------------------------------------------------------------------------
    BOOL CGameApp::OnMsgHandler(const MSG& p_kMsg) {
        switch (p_kMsg.message) {
        case WM_CTLCOLOREDIT: {
            if (g_pGameIF) {
                if (g_pGameIF->IsActiveWindow(cBaseWindow::WT_CHAT_MAIN_WINDOW)) {
                    ChatContents* pChatCon = (ChatContents*)CONTENTSSYSTEM_PTR->GetContents(
                        E_CT_CHATTING_STANDARDIZATION);
                    if (pChatCon && (HWND)p_kMsg.lParam == pChatCon->_GetEditHwnd()) {
                        return pChatCon->_ApplyEditColor((HDC)p_kMsg.wParam);
                    }
                }

                if (g_pMoneySeparate) {
                    LRESULT hr = g_pMoneySeparate->ApplyEditColor((HWND)p_kMsg.lParam, (HDC)p_kMsg.wParam);
                    if (hr != NULL) {
                        return hr;
                    }
                }

                if (g_pItemSeparate) {
                    LRESULT hr = g_pItemSeparate->ApplyEditColor((HWND)p_kMsg.lParam, (HDC)p_kMsg.wParam);
                    if (hr != NULL) {
                        return hr;
                    }
                }
                if (g_pGameIF->IsActiveWindow(cBaseWindow::WT_MASTERS_MATCHING)) {
                    LRESULT hr = g_pGameIF->GetMastersMatching()->ApplyEditColor(
                        (HWND)p_kMsg.lParam, (HDC)p_kMsg.wParam);
                    if (hr != NULL) {
                        return hr;
                    }
                }
            }
        }
        break;
        case WM_DISPLAYCHANGE: {
            if (!m_bFullMode)
                ::SystemParametersInfo(SPI_GETWORKAREA, 0, &m_uiWindowModeWorkSpaceRect, 0);
        }
        break;
        }

        if (FLOWMGR_STPTR && FLOWMGR_ST.OnMsgHandler(p_kMsg))
            return TRUE;

        return FALSE;
    }

    //---------------------------------------------------------------------------
    bool CGameApp::LostDevice(void* p_pvData) {
        // Flow LostDevice
        if (FLOWMGR_STPTR && !FLOWMGR_ST.LostDevice(p_pvData))
            return false;

        return true;
    }

    //---------------------------------------------------------------------------
    bool CGameApp::ResetDevice(bool p_bBeforeReset, void* p_pvData) {
        // Flow ResetDevice
        if (FLOWMGR_STPTR && !FLOWMGR_ST.ResetDevice(p_bBeforeReset, p_pvData))
            return false;

        return true;
    }

    //---------------------------------------------------------------------------
    bool CGameApp::RecreateDevice(WORD wWidth, WORD wHeight, BYTE byBit, BYTE RefRate, bool bFullMode) {
        bool bScreenModeChanged = (static_cast<bool>(m_bFullMode) != bFullMode);
        CalculateSize(wWidth, wHeight, bFullMode);
        ReSize(GetWidth(), GetHeight(), GetFullMode(), bScreenModeChanged);
        return true;
    }
}

//---------------------------------------------------------------------------
