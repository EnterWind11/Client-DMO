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
#include "NiAnimationPCH.h"

#include <NiBool.h>
#include <NiPoint3.h>
#include "NiMorphData.h"
#include "NiFloatInterpolator.h"

NiImplementRTTI(NiMorphData,NiObject);

//---------------------------------------------------------------------------
// NiMorphData::MorphTarget
//---------------------------------------------------------------------------
NiMorphData::MorphTarget::MorphTarget() : m_aTargetVerts(0),
    m_fLegacyWeight(0.0f)
{
}
//---------------------------------------------------------------------------
NiMorphData::MorphTarget::~MorphTarget()
{
    NiDelete[] m_aTargetVerts;
}
//---------------------------------------------------------------------------
void NiMorphData::MorphTarget::ReplaceTargetVerts(NiPoint3* aTargetVerts)
{
    NiDelete[] m_aTargetVerts;
    m_aTargetVerts = aTargetVerts;
}
//---------------------------------------------------------------------------
const NiFixedString&  NiMorphData::MorphTarget::GetName()
{
    return m_kName;
}
//---------------------------------------------------------------------------
void NiMorphData::MorphTarget::SetName(const NiFixedString& kName)
{
    m_kName = kName;
}
//---------------------------------------------------------------------------
void NiMorphData::MorphTarget::LoadBinary(NiStream& kStream, 
    unsigned int uiNumVerts)
{
    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 1, 0, 1))
    {
        kStream.LoadCStringAsFixedString(m_kName);
    }
    else
    {
        kStream.LoadFixedString(m_kName);
    }

    if (kStream.GetFileVersion() >= NiStream::GetVersion(10, 1, 0, 104))
    {
        if (kStream.GetFileVersion() < NiStream::GetVersion(20, 1, 0, 3))
        {
            NiStreamLoadBinary(kStream, m_fLegacyWeight);
        }
    }

    m_aTargetVerts = NiNew NiPoint3[uiNumVerts];
    NiPoint3::LoadBinary(kStream, m_aTargetVerts, uiNumVerts);
}
//---------------------------------------------------------------------------
void NiMorphData::MorphTarget::SaveBinary(NiStream& kStream, 
    unsigned int uiNumVerts)
{
    kStream.SaveFixedString(m_kName);
    if (uiNumVerts)
        NiPoint3::SaveBinary(kStream, m_aTargetVerts, uiNumVerts);
}
//---------------------------------------------------------------------------
void NiMorphData::MorphTarget::RegisterStreamables(NiStream& kStream)
{
    kStream.RegisterFixedString(m_kName);
}
//---------------------------------------------------------------------------
bool NiMorphData::MorphTarget::IsEqual(NiMorphData::MorphTarget* pkOther, 
    unsigned int uiNumVerts)
{
    for (unsigned int uiJ = 0; uiJ < uiNumVerts; uiJ++)
    {
        if (GetTargetVert(uiJ) != pkOther->GetTargetVert(uiJ))
            return false;
    }

    if (m_kName != pkOther->m_kName)
        return false;

    return true;
}
//---------------------------------------------------------------------------
NiInterpolator* NiMorphData::MorphTarget::GetLegacyInterpolator()
{
    return m_spLegacyInterpolator;
}
//---------------------------------------------------------------------------
void NiMorphData::MorphTarget::SetLegacyInterpolator(NiInterpolator* pkInterp)
{
    m_spLegacyInterpolator = pkInterp;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// NiMorphData
//---------------------------------------------------------------------------
NiMorphData::NiMorphData()
{    
    m_uiNumTargets = 0;
    m_uiNumVertsPerTarget = 0;
    m_aMorphTargets = 0;
    m_bRelativeTargets = false;
}
//---------------------------------------------------------------------------
NiMorphData::~NiMorphData ()
{
    NiDelete[] m_aMorphTargets;
}
//---------------------------------------------------------------------------
void NiMorphData::ReplaceTargets(NiMorphData::MorphTarget* aMorphTargets, 
    unsigned int uiNumTargets, unsigned int uiNumVertsPerTarget)
{
    NiDelete[] m_aMorphTargets;
    m_aMorphTargets = aMorphTargets;
    m_uiNumTargets = uiNumTargets;
    m_uiNumVertsPerTarget = uiNumVertsPerTarget;
}
//---------------------------------------------------------------------------
void NiMorphData::SetAndAdjustRelativeTargets(bool bRelativeTargets)
{
    if (m_bRelativeTargets == bRelativeTargets)
        return;

    SetRelativeTargets(bRelativeTargets);

    if (m_aMorphTargets == 0)
        return;

    NiPoint3* aBaseVerts = m_aMorphTargets[0].GetTargetVerts();

    if (m_bRelativeTargets)
    {
        for (unsigned int uiI = 1; uiI < m_uiNumTargets; uiI++)
        {
            NiPoint3* aTargetVerts = m_aMorphTargets[uiI].GetTargetVerts();
            for (unsigned int uiJ = 0; uiJ < m_uiNumVertsPerTarget; uiJ++)
            {
                aTargetVerts[uiJ] -= aBaseVerts[uiJ];
            }
        }
    }
    else
    {
        for (unsigned int uiI = 1; uiI < m_uiNumTargets; uiI++)
        {
            NiPoint3* aTargetVerts = m_aMorphTargets[uiI].GetTargetVerts();
            for (unsigned int uiJ = 0; uiJ < m_uiNumVertsPerTarget; uiJ++)
            {
                aTargetVerts[uiJ] += aBaseVerts[uiJ];
            }
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiMorphData);
//---------------------------------------------------------------------------
void NiMorphData::LoadBinary(NiStream& stream)
{
    NiObject::LoadBinary(stream);

    NiStreamLoadBinary(stream, m_uiNumTargets);
    NiStreamLoadBinary(stream, m_uiNumVertsPerTarget);

    NiBool bRelativeTargets;
    NiStreamLoadBinary(stream, bRelativeTargets);
    m_bRelativeTargets = (bRelativeTargets == 1);

    NIASSERT(m_uiNumTargets && m_uiNumVertsPerTarget);
    m_aMorphTargets = NiNew MorphTarget[m_uiNumTargets];
    
    for (unsigned int uiI = 0; uiI < m_uiNumTargets; uiI++)
    {
        if (stream.GetFileVersion() < NiStream::GetVersion(10, 1, 0, 104))
        {
            char acString[256];
            unsigned int uiNumKeys;
            NiFloatKey::KeyType eType;
            NiStreamLoadBinary(stream, uiNumKeys);
            NiStreamLoadEnum(stream, eType);
            
            if (uiNumKeys > 0)
            {
                unsigned char ucSize = NiFloatKey::GetKeySize(eType);

                NiFloatKey::CreateFunction cf =
                    NiFloatKey::GetCreateFunction(eType);
     
                NiFloatKey* pkMorphKeys = (NiFloatKey*) cf(stream, uiNumKeys);

                NiFloatKey::FillDerivedValsFunction derived =
                    NiFloatKey::GetFillDerivedFunction(eType);
                derived((NiAnimationKey*) pkMorphKeys, uiNumKeys, ucSize);

                NiFloatInterpolator* pkInterp = NiNew NiFloatInterpolator();
                pkInterp->ReplaceKeys(pkMorphKeys, uiNumKeys, eType);
                m_aMorphTargets[uiI].SetLegacyInterpolator(pkInterp);
                pkInterp->Collapse();
            }

            NiSprintf(acString, 256, "MT %d", uiI);
            m_aMorphTargets[uiI].SetName(acString);

            NiPoint3* aTargetVerts = NiNew NiPoint3[m_uiNumVertsPerTarget];
            for (unsigned int uiJ = 0; uiJ < m_uiNumVertsPerTarget; uiJ++)
                aTargetVerts[uiJ].LoadBinary(stream);

            m_aMorphTargets[uiI].ReplaceTargetVerts(aTargetVerts);
            continue;
        }

        m_aMorphTargets[uiI].LoadBinary(stream, m_uiNumVertsPerTarget);
    }
}
//---------------------------------------------------------------------------
void NiMorphData::LinkObject(NiStream& stream)
{
    NiObject::LinkObject(stream);
}
//---------------------------------------------------------------------------
bool NiMorphData::RegisterStreamables(NiStream& stream)
{
    if (!NiObject::RegisterStreamables(stream))
        return false;

    for (unsigned int ui = 0; ui < m_uiNumTargets; ui++)
    {
        NiMorphData::MorphTarget* pkTarget = GetTarget(ui);
        pkTarget->RegisterStreamables(stream);
    }

    return true;
}
//---------------------------------------------------------------------------
void NiMorphData::SaveBinary(NiStream& stream)
{
    NiObject::SaveBinary(stream);

    NiStreamSaveBinary(stream, m_uiNumTargets);
    NiStreamSaveBinary(stream, m_uiNumVertsPerTarget);

    NiBool bRelativeTargets = m_bRelativeTargets;
    NiStreamSaveBinary(stream, bRelativeTargets);

    for (unsigned int uiI = 0; uiI < m_uiNumTargets; uiI++)
    {
        m_aMorphTargets[uiI].SaveBinary(stream, m_uiNumVertsPerTarget);
    }

}
//---------------------------------------------------------------------------
bool NiMorphData::IsEqual(NiObject* pObject)
{
    if (!NiObject::IsEqual(pObject))
        return false;

    NiMorphData* pData = (NiMorphData*) pObject;

    if ( m_uiNumTargets != pData->m_uiNumTargets
    ||   m_uiNumVertsPerTarget != pData->m_uiNumVertsPerTarget
    ||   m_bRelativeTargets != pData->m_bRelativeTargets)
    {
        return false;
    }

    for (unsigned int uiI = 0; uiI < m_uiNumTargets; uiI++)
    {
        if (!GetTarget(uiI)->IsEqual(pData->GetTarget(uiI), 
            m_uiNumVertsPerTarget))
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiMorphData::GetViewerStrings(NiViewerStringsArray* pStrings)
{
    NiObject::GetViewerStrings(pStrings);

    pStrings->Add(NiGetViewerString(NiMorphData::ms_RTTI.GetName()));

    pStrings->Add(NiGetViewerString("m_uiNumTargets",m_uiNumTargets));
    pStrings->Add(NiGetViewerString("m_uiNumVertsPerTarget",
        m_uiNumVertsPerTarget));

    pStrings->Add(NiGetViewerString("MorphTargets"));
    for (unsigned int ui = 0; ui < m_uiNumTargets; ui++)
    {
        NiMorphData::MorphTarget* pkTarget = GetTarget(ui);
        pStrings->Add(NiGetViewerString("m_pcName", 
            (const char*)pkTarget->GetName()));
    }

}
//---------------------------------------------------------------------------
