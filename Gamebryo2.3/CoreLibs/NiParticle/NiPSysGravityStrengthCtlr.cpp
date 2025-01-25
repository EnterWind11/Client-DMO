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

#include "NiPSysGravityStrengthCtlr.h"
#include "NiPSysGravityModifier.h"

NiImplementRTTI(NiPSysGravityStrengthCtlr, NiPSysModifierFloatCtlr);

//---------------------------------------------------------------------------
NiPSysGravityStrengthCtlr::NiPSysGravityStrengthCtlr(
    const char* pcModifierName) : NiPSysModifierFloatCtlr(pcModifierName)
{
}
//---------------------------------------------------------------------------
NiPSysGravityStrengthCtlr::NiPSysGravityStrengthCtlr() 
{
}
//---------------------------------------------------------------------------
bool NiPSysGravityStrengthCtlr::InterpTargetIsCorrectType(
    NiObjectNET* pkTarget) const
{
    if (!NiPSysModifierCtlr::InterpTargetIsCorrectType(pkTarget))
    {
        return false;
    }

    NiPSysModifier* pkModifier = ((NiParticleSystem*) pkTarget)
        ->GetModifierByName(m_kModifierName);
    NIASSERT(pkModifier);

    return NiIsKindOf(NiPSysGravityModifier, pkModifier);
}
//---------------------------------------------------------------------------
void NiPSysGravityStrengthCtlr::GetTargetFloatValue(float& fValue)
{
    fValue = ((NiPSysGravityModifier*) m_pkModifier)->GetStrength();
}
//---------------------------------------------------------------------------
void NiPSysGravityStrengthCtlr::SetTargetFloatValue(float fValue)
{
    ((NiPSysGravityModifier*) m_pkModifier)->SetStrength(fValue);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysGravityStrengthCtlr);
//---------------------------------------------------------------------------
void NiPSysGravityStrengthCtlr::CopyMembers(NiPSysGravityStrengthCtlr* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSysModifierFloatCtlr::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysGravityStrengthCtlr);
//---------------------------------------------------------------------------
void NiPSysGravityStrengthCtlr::LoadBinary(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSysGravityStrengthCtlr::LinkObject(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysGravityStrengthCtlr::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifierFloatCtlr::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysGravityStrengthCtlr::SaveBinary(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysGravityStrengthCtlr::IsEqual(NiObject* pkObject)
{
    return NiPSysModifierFloatCtlr::IsEqual(pkObject);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer Strings
//---------------------------------------------------------------------------
void NiPSysGravityStrengthCtlr::GetViewerStrings(NiViewerStringsArray* 
    pkStrings)
{
    NiPSysModifierFloatCtlr::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSysGravityStrengthCtlr::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
