//---------------------------------------------------------------------------
//
// 파일명 : LoginFlow.h
// 작성일 : 
// 작성자 : 
// 설  명 : 
//
//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------
#include "Flow.h"

class CLogin;
class CFade;

//---------------------------------------------------------------------------
namespace Flow {
    class CLoginFlow : public CFlow {
    public:
        CLoginFlow(int p_iID);
        virtual ~CLoginFlow();
        
        // ************************
        // Lost, Reset Device
        // ************************
        virtual bool LostDevice(void* p_pvData);
        virtual bool ResetDevice(bool p_bBeforeReset, void* p_pvData);
        // ************************

        virtual void ReservedChangeFlow(int p_iNextFlowID);
        virtual void InputFrame();
        virtual void OnEnter();
        virtual void OnExit(int p_iNextFlowID);

        virtual BOOL OnMsgHandler(const MSG& p_kMsg); //2016-01-13-nova 윈도우Msg로 처리

    protected:
        // ************************
        // Init, Term
        // ************************
        virtual BOOL Initialize();
        // ************************

        // ************************
        // Update, Cull, Render
        // ************************
        virtual void UpdateFrame();
        virtual void RenderUIFrame();
        virtual void RenderSceneFrame();
        virtual void RenderBackScreenFrame(); // 2D Screen Draw
        // ************************

    private:
        bool m_bBgmPlay;

        CLogin* m_pLoginUI;
        CFade* m_pFadeUI;
    };
}

//---------------------------------------------------------------------------
