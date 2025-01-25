//---------------------------------------------------------------------------
//
// Filename : Flow.cpp
// Date     : 
// Author   : 
// Description : Implementation of the Flow namespace for game logic management.
//
//---------------------------------------------------------------------------

#include "StdAfx.h"
#include "Flow.h"

//---------------------------------------------------------------------------
namespace Flow {
    CFlow::CFlow(int p_iID)
        : m_iID(p_iID)
          , m_bInitialized(FALSE)
          , m_bCreated(FALSE)
          , m_bPaused(FALSE)
          , m_bLoaded(FALSE)
          , m_bInputLock(FALSE) {
        SetCreated(TRUE);
    }

    //---------------------------------------------------------------------------
    CFlow::~CFlow() {
        if (!m_bDestroyed) {
            SafeCleanup(-1);
        }
        SetCreated(FALSE);
    }

    //---------------------------------------------------------------------------

    void CFlow::Destroy(int p_iNextFlowID) {
        if (!m_bDestroyed) {
            OnExit(p_iNextFlowID);
            m_bDestroyed = true;
        }
    }

    //---------------------------------------------------------------------------
    void CFlow::OnEnter() {
        if (!IsInitialized()) {
            if (Initialize()) {
                if (LoadResource()) {
                    SetLoaded(TRUE);
                }
                SetInitialized(TRUE);
            }
            else {
                assert(!"Flow Initialize Failed!");
            }
        }
    }

    //---------------------------------------------------------------------------
    void CFlow::OnIdle() {
        MainLoop();
    }

    //---------------------------------------------------------------------------
    void CFlow::OnExit(int p_iNextFlowID) {
        if (IsInitialized()) {
            ReleaseResource();
            SetLoaded(FALSE);
            Terminate();
            SetInitialized(FALSE);
        }
    }

    void CFlow::SafeCleanup(int p_iNextFlowID) {
        if (IsInitialized()) {
            ReleaseResource();
            SetLoaded(FALSE);
            Terminate();
            SetInitialized(FALSE);
        }
    }

    // Placeholder for overriding logic
    void CFlow::OnOverride(int p_iNextFlowID) {}

    // Placeholder for resuming logic
    void CFlow::OnResume(int p_iNextFlowID) {}

    // Message handler
    BOOL CFlow::OnMsgHandler(const MSG& p_kMsg) {
        return FALSE;
    }

    //---------------------------------------------------------------------------
    // Handle lost device event
    bool CFlow::LostDevice(void* p_pvData) {
#ifdef DEBUG_FLOW
                DBG("Lost Device\n");
#endif
        return true;
    }

    // Handle device reset event
    bool CFlow::ResetDevice(bool p_bBeforeReset, void* p_pvData) {
#ifdef DEBUG_FLOW
                DBG("Reset Device\n");
#endif
        return true;
    }

    //---------------------------------------------------------------------------
    // Initialize the flow
    BOOL CFlow::Initialize() {
        NiDX9Renderer::GetRenderer()->SetStencilClear(0);
        return TRUE;
    }

    // Terminate the flow
    void CFlow::Terminate() {}

    // Main loop logic
    void CFlow::MainLoop() {
        if (!MeasureTime())
            return;

        GlobalUpdate();

        if (!m_bInputLock)
            InputFrame();

        CullFrame();
        DrawFrame();
    }

    //---------------------------------------------------------------------------
    // Load necessary resources
    BOOL CFlow::LoadResource() {
        return TRUE;
    }

    // Release loaded resources
    void CFlow::ReleaseResource() {}

    // Perform global updates
    void CFlow::GlobalUpdate() {
        g_pEngine->Update();
        CAMERA_ST._UpdateCamera();

        if (g_pSoundMgr)
            g_pSoundMgr->Update(static_cast<float>(g_fAccumTime));

        cEditBox::UpdateFocusEdit();
    }

    // Measure frame time
    BOOL CFlow::MeasureTime() {
        return g_pEngine->m_Frame.UpdateFrame();
    }

    // ____________________________________________________________________________________

    // Process input frame
    void CFlow::InputFrame() {}

    // Update frame logic
    void CFlow::UpdateFrame() {}

    // Perform culling
    void CFlow::CullFrame() {}

    // Render frame logic
    void CFlow::DrawFrame() {
        UpdateFrame();

        if (BeginFrame()) {
            if (BeginRenderTarget()) {
                RenderBackScreenFrame();
                RenderSceneFrame();
                RenderScreenFrame();
                RenderUIFrame();
                EndRenderTarget();
                EndFrame();
                DisplayFrame();
            }
        }
    }

    // ____________________________________________________________________________________
    // Render 3D scene
    void CFlow::RenderSceneFrame() {}

    // Render 2D screen
    void CFlow::RenderScreenFrame() {}

    // Render 2D background UI
    void CFlow::RenderBackScreenFrame() {}

    // Render UI elements
    void CFlow::RenderUIFrame() {}

    // ____________________________________________________________________________________
    // Begin frame rendering
    bool CFlow::BeginFrame() {
        return g_pEngine->BeginFrame();
    }

    // Begin rendering to the target
    bool CFlow::BeginRenderTarget() {
        return g_pEngine->BeginUsingCurrentRenderTargetGroup(NiRenderer::CLEAR_ALL);
    }

    // End rendering to the target
    void CFlow::EndRenderTarget() {
        g_pEngine->EndRenderTarget();
    }

    // End frame rendering
    void CFlow::EndFrame() {
        g_pEngine->EndFrame();
    }

    // Display rendered frame
    void CFlow::DisplayFrame() {
        g_pEngine->DisplayFrame();
    }


    void CFlow::ReservedChangeFlow(int p_iNextFlowID) {}
    void CFlow::ReservedPushFlow(int p_iNextFlowID) {}
    void CFlow::ReservedPopFlow() {}

    // Lock and unlock input
    void CFlow::SetInputLock() {
        m_bInputLock = TRUE;
    }

    void CFlow::SetInputUnLock() {
        m_bInputLock = FALSE;
    }
}

// ____________________________________________________________________________________
