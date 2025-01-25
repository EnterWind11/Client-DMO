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

// Precompiled Header
#include "NiParticlePCH.h"

#include "NiPSysInitialRotAngleVarCtlr.h"
#include "NiPSysRotationModifier.h"

NiImplementRTTI(NiPSysInitialRotAngleVarCtlr, NiPSysModifierFloatCtlr);

//---------------------------------------------------------------------------
NiPSysInitialRotAngleVarCtlr::NiPSysInitialRotAngleVarCtlr(
    const char* pcModifierName) : NiPSysModifierFloatCtlr(pcModifierName)
{
}
//---------------------------------------------------------------------------
NiPSysInitialRotAngleVarCtlr::NiPSysInitialRotAngleVarCtlr() 
{
}
//---------------------------------------------------------------------------
bool NiPSysInitialRotAngleVarCtlr::InterpTargetIsCorrectType(
    NiObjectNET* pkTarget) const
{
    if (!NiPSysModifierCtlr::InterpTargetIsCorrectType(pkTarget))
    {
        return false;
    }

    NiPSysModifier* pkModifier = ((NiParticleSystem*) pkTarget)
        ->GetModifierByName(m_kModifierName);
    NIASSERT(pkModifier);

    return NiIsKindOf(NiPSysRotationModifier, pkModifier);
}
//---------------------------------------------------------------------------
void NiPSysInitialRotAngleVarCtlr::GetTargetFloatValue(float& fValue)
{
    fValue = ((NiPSysRotationModifier*) m_pkModifier)
        ->GetInitialRotAngleVar();
}
//---------------------------------------------------------------------------
void NiPSysInitialRotAngleVarCtlr::SetTargetFloatValue(float fValue)
{
    ((NiPSysRotationModifier*) m_pkModifier)->SetInitialRotAngleVar(fValue);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysInitialRotAngleVarCtlr);
//---------------------------------------------------------------------------
void NiPSysInitialRotAngleVarCtlr::CopyMembers(
    NiPSysInitialRotAngleVarCtlr* pkDest, NiCloningProcess& kCloning)
{
    NiPSysModifierFloatCtlr::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysInitialRotAngleVarCtlr);
//---------------------------------------------------------------------------
void NiPSysInitialRotAngleVarCtlr::LoadBinary(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSysInitialRotAngleVarCtlr::LinkObject(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysInitialRotAngleVarCtlr::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifierFloatCtlr::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysInitialRotAngleVarCtlr::SaveBinary(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysInitialRotAngleVarCtlr::IsEqual(NiObject* pkObject)
{
    return NiPSysModifierFloatCtlr::IsEqual(pkObject);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer Strings
//---------------------------------------------------------------------------
void NiPSysInitialRotAngleVarCtlr::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiPSysModifierFloatCtlr::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSysInitialRotAngleVarCtlr::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
