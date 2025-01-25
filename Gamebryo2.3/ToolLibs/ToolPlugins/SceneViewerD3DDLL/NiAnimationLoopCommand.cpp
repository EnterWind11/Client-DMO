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
#include "stdafx.h"  
#include "NiAnimationLoopCommand.h"

NiCommandID NiAnimationLoopCommand::ms_kID = 0;
                  
//---------------------------------------------------------------------------
NiAnimationLoopCommand::NiAnimationLoopCommand()
{
}
//---------------------------------------------------------------------------
bool NiAnimationLoopCommand::Apply(NiSceneViewer* pkViewer)
{
    pkViewer->SetAnimationLooping(!pkViewer->GetAnimationLooping());
    return true;
}
//---------------------------------------------------------------------------
NiCommandID NiAnimationLoopCommand::GetClassID()
{
    return ms_kID;
}
//---------------------------------------------------------------------------
NiAnimationLoopCommand::~NiAnimationLoopCommand()
{

}
//---------------------------------------------------------------------------
NiSceneCommand* NiAnimationLoopCommand::Create(NiSceneCommandInfo* pkInfo)
{
    return NiNew NiAnimationLoopCommand();
}
//---------------------------------------------------------------------------
NiTPrimitiveArray<NiSceneCommandInfo*>* 
    NiAnimationLoopCommand::GetDefaultCommandParamaterizations()
{
    NiSceneCommandInfo* pkCommandInfo = NiNew NiSceneCommandInfo();
    pkCommandInfo->m_uiClassID = GetClassID();
    pkCommandInfo->m_eDeviceState = NiDevice::PRESSED;
    pkCommandInfo->m_iPriority = 0;
    pkCommandInfo->SetName("Toggle Animation Looping");
    
    NiTPrimitiveArray<NiSceneCommandInfo*>* pkArray = 
        NiNew NiTPrimitiveArray<NiSceneCommandInfo*>();
    pkArray->Add(pkCommandInfo);
    return pkArray;
}
//---------------------------------------------------------------------------
void NiAnimationLoopCommand::Register()
{
    ms_kID = NiSceneCommandRegistry::RegisterCommand("ANIMATION_LOOP",
        NiAnimationLoopCommand::Create);

}
//---------------------------------------------------------------------------
