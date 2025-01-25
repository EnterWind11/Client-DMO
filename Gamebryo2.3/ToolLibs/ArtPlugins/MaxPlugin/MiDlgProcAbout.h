// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not
// be copied or disclosed except in accordance with the terms of that
// agreement.
//
//      Copyright (c) 1996-2007 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

#ifndef MIDLGPROCABOUT_H
#define MIDLGPROCABOUT_H

#include "MiViewerPlugin.h"
//---------------------------------------------------------------------------
class MiDlgProcAbout : public ParamMapUserDlgProc 
{
    public:
        MiDlgProcAbout(MiViewerPlugin *pUtil) {m_pUtil = pUtil;}
        BOOL DlgProc(TimeValue t,IParamMap *map,
            HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
        void DeleteThis() { NiDelete this; }
    private:
        MiViewerPlugin *m_pUtil;
};
//---------------------------------------------------------------------------

#endif


