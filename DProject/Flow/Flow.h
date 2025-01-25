//---------------------------------------------------------------------------
//
// 파일명 : Flow.h
// 작성일 : 
// 작성자 : 
// 설  명 : 
//
//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------
#include "FlowState.h"

//---------------------------------------------------------------------------
namespace Flow {
    class CFlow : public IFlowState {
    public:
        typedef enum _FLOW_IDS {
            FLW_NONE = -1,            // No flow
            FLW_INTRO,                // Intro screen
            FLW_LOGO,                 // Logo screen
            FLW_LOGIN,                // Login screen
            FLW_SERVERSEL,            // Server selection screen
            FLW_SECURITY,             // Secondary password screen
            FLW_CHARSEL,              // Character selection screen
            FLW_CHARCREATE,           // Character creation screen
            FLW_DIGIMON_CREATE,       // Digimon creation screen
            FLW_LOADING,              // Loading screen
            FLW_MAINGAME,             // Main game screen
            FLW_DATSCENTER,           // Dats Center
            FLW_DATATRADECENTER,      // Data exchange screen (uses the same data as Dats Center)
            FLW_BATTLE_REGISTER,      // Battle registration page
            FLW_BATTLE,               // Battle
            FLW_BATTLE_RESULT,        // Battle result page
            FLW_SERVER_RELOCATE,      // Server relocation page
            FLW_ID_COUNT              // Number of flow IDs
        } FLOW_IDS;

        CFlow(int p_iID);
        virtual ~CFlow();

        // ************************
        // IFlowState
        // ************************
        void Destroy(int p_iNextFlowID);
        virtual void OnEnter();
        virtual void OnExit(int p_iNextFlowID);
        virtual void OnIdle();
        virtual void OnOverride(int p_iNextFlowID);
        virtual void OnResume(int p_iNextFlowID);
        // ************************

        // ************************
        // OnMsgHandler
        // ************************
        virtual BOOL OnMsgHandler(const MSG& p_kMsg);
        // ************************

        // ************************
        // Lost, Reset Device
        // ************************
        virtual bool LostDevice(void* p_pvData);
        virtual bool ResetDevice(bool p_bBeforeReset, void* p_pvData);
        // ************************

        // 변경될 플로우가 예약 瑛?때 호출함
        virtual void ReservedChangeFlow(int p_iNextFlowID);
        virtual void ReservedPushFlow(int p_iNextFlowID);
        virtual void ReservedPopFlow();

    protected:
        // ************************
        // Init, Term, Loop
        // ************************
        virtual BOOL Initialize();
        virtual void Terminate();
        virtual void MainLoop();
        // ************************

        // ************************
        // Resource
        // ************************
        virtual BOOL LoadResource();
        virtual void ReleaseResource();
        // ************************

        // ************************
        // Update, Cull, Render
        // ************************
        virtual void GlobalUpdate(); // Update(전역)
        virtual BOOL MeasureTime(); // Time
        virtual void InputFrame(); // Input     
        virtual void UpdateFrame(); // Update(지역)
        virtual void CullFrame(); // Cull
        virtual void DrawFrame(); // Draw
        virtual bool BeginFrame(); // BeginScene
        virtual bool BeginRenderTarget(); // BeginTarget
        virtual void RenderSceneFrame(); // 3D Draw
        virtual void RenderScreenFrame(); // 2D Draw
        virtual void RenderBackScreenFrame(); // 2D Screen Draw
        virtual void RenderUIFrame(); // UI Draw
        virtual void EndRenderTarget(); // EndTarget
        virtual void EndFrame(); // EndScene
        virtual void DisplayFrame(); // Present
        // ************************

    public:
        // ************************
        // Flag Get
        // ************************
        BOOL IsInitialized() const;
        BOOL IsCreated() const;
        //BOOL  IsLoaded()      const;
        BOOL IsPaused() const;
        int GetID() const;
        // ************************

        // ************************
        // Flag Set
        // ************************
        void SetInitialized(BOOL p_bOn);
        void SetCreated(BOOL p_bOn);
        void SetLoaded(BOOL p_bOn);
        void SetPaused(BOOL p_bOn);
        void SetID(int p_iID);
        // ************************

        void SetInputLock();
        void SetInputUnLock();

    protected:
        int m_iID;
        BOOL m_bInitialized;
        BOOL m_bCreated;
        BOOL m_bPaused;
        BOOL m_bLoaded;

    private:
        BOOL m_bInputLock;
        bool m_bDestroyed;
        void SafeCleanup(int p_iNextFlowID);
    };
}

#include "Flow.inl"
//---------------------------------------------------------------------------
