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

#include "NiQuaternionInterpolator.h"
#include "NiEulerRotKey.h"

NiImplementRTTI(NiQuaternionInterpolator, NiKeyBasedInterpolator);

//---------------------------------------------------------------------------
NiQuaternionInterpolator::NiQuaternionInterpolator(
    NiRotData* pkQuaternionData) : m_kQuaternionValue(INVALID_QUATERNION),
    m_spQuaternionData(pkQuaternionData), m_uiLastIdx(0)
{
}
//---------------------------------------------------------------------------
NiQuaternionInterpolator::NiQuaternionInterpolator(
    NiQuaternion kPoseValue) : m_kQuaternionValue(kPoseValue),
    m_spQuaternionData(NULL), m_uiLastIdx(0)
{
}
//---------------------------------------------------------------------------
bool NiQuaternionInterpolator::IsQuaternionValueSupported() const
{
    return true;
}
//---------------------------------------------------------------------------
unsigned short NiQuaternionInterpolator::GetKeyChannelCount() const
{
    return 1;
}
//---------------------------------------------------------------------------
unsigned int NiQuaternionInterpolator::GetKeyCount(unsigned short usChannel)
    const
{
    if (!m_spQuaternionData)
        return 0;
    else
        return m_spQuaternionData->GetNumKeys();
}
//---------------------------------------------------------------------------
NiAnimationKey::KeyType NiQuaternionInterpolator::GetKeyType(
    unsigned short usChannel) const
{
    if (!m_spQuaternionData)
        return NiAnimationKey::NOINTERP;

    unsigned int uiNumKeys;
    NiRotKey::KeyType eType;
    unsigned char ucSize;
    m_spQuaternionData->GetAnim(uiNumKeys, eType, ucSize);
    return eType; 
}
//---------------------------------------------------------------------------
NiAnimationKey::KeyContent NiQuaternionInterpolator::GetKeyContent(
    unsigned short usChannel) const
{
    return NiAnimationKey::ROTKEY;
}
//---------------------------------------------------------------------------
NiAnimationKey* NiQuaternionInterpolator::GetKeyArray(
    unsigned short usChannel) const
{
    if (!m_spQuaternionData)
        return NULL;

    unsigned int uiNumKeys;
    NiRotKey::KeyType eType;
    unsigned char ucSize;
    NiRotKey * pkKeys = m_spQuaternionData->GetAnim(uiNumKeys, eType, 
        ucSize);
    return pkKeys; 
}
//---------------------------------------------------------------------------
unsigned char NiQuaternionInterpolator::GetKeyStride(
    unsigned short usChannel) const
{
    if (!m_spQuaternionData)
        return 0x00;

    unsigned int uiNumKeys;
    NiRotKey::KeyType eType;
    unsigned char ucSize;
    m_spQuaternionData->GetAnim(uiNumKeys, eType, ucSize);
    return ucSize; 
}
//---------------------------------------------------------------------------
void NiQuaternionInterpolator::GetActiveTimeRange(float& fBeginKeyTime,
    float& fEndKeyTime) const
{
    unsigned int uiNumKeys;
    NiRotKey::KeyType eType;
    unsigned char ucSize;
    NiRotKey* pkKeys = GetKeys(uiNumKeys, eType, ucSize);
    bool bKeys = false;
    if (uiNumKeys > 0)
    {
        if (eType == NiRotKey::EULERKEY)
        {
            NiEulerRotKey* pkRotKey = (NiEulerRotKey*) 
                pkKeys->GetKeyAt(0, ucSize);

            float fTempBeginKeyTime = NI_INFINITY;
            float fTempEndKeyTime = -NI_INFINITY;
            for (unsigned char uc = 0; uc < 3; uc++)
            {
                unsigned int uiNumFloatKeys = pkRotKey->GetNumKeys(uc);
                unsigned char ucFloatSize = pkRotKey->GetKeySize(uc);
                if (uiNumKeys > 0)
                {
                    NiFloatKey* pkFloatKeys = pkRotKey->GetKeys(uc);
                    float fKeyTime = pkFloatKeys->GetKeyAt(0,ucFloatSize)->
                        GetTime();
                    if (fKeyTime < fTempBeginKeyTime)
                    {
                        fTempBeginKeyTime = fKeyTime;
                    }
                    fKeyTime = pkFloatKeys->GetKeyAt(uiNumFloatKeys - 1,
                        ucFloatSize)->GetTime();
                    if (fKeyTime > fTempEndKeyTime)
                    {
                        fTempEndKeyTime = fKeyTime;
                    }
                    bKeys = true;
                }
            }
            if (bKeys)
            {
                fBeginKeyTime = fTempBeginKeyTime;
                fEndKeyTime = fTempEndKeyTime;
            }
        }
        else
        {
            fBeginKeyTime = pkKeys->GetKeyAt(0, ucSize)->GetTime();
            fEndKeyTime = pkKeys->GetKeyAt(uiNumKeys - 1, ucSize)->GetTime();
            bKeys = true;
        }
    }

    if (!bKeys)
    {
        fBeginKeyTime = 0.0f;
        fEndKeyTime = 0.0f;
    }
}
//---------------------------------------------------------------------------
bool NiQuaternionInterpolator::GetChannelPosed(unsigned short usChannel) const
{
    if (m_spQuaternionData)
        return false;
    if (m_kQuaternionValue == INVALID_QUATERNION)
        return false;
    return true;
}
//---------------------------------------------------------------------------
void NiQuaternionInterpolator::Collapse()
{
    if (m_spQuaternionData)
    {
        unsigned int uiNumKeys;
        NiAnimationKey::KeyType eType;
        unsigned char ucSize;
        NiRotKey* pkKeys = m_spQuaternionData->GetAnim(uiNumKeys, eType,
            ucSize);
        if (uiNumKeys == 0)
        {
            m_spQuaternionData = NULL;
            m_kQuaternionValue = INVALID_QUATERNION;
        }
        else
        {
            NiQuaternion kValue = pkKeys->GetKeyAt(0, ucSize)
                ->GetQuaternion();
            bool bCollapse = false;
            if (uiNumKeys == 1 && eType != NiAnimationKey::EULERKEY)
            {
                bCollapse = true;
            }
            else if (eType == NiAnimationKey::LINKEY ||
                eType == NiAnimationKey::STEPKEY)
            {
                bCollapse = true;
                for (unsigned int ui = 1; bCollapse && ui < uiNumKeys; ui++)
                {
                    if (pkKeys->GetKeyAt(ui, ucSize)->GetQuaternion() !=
                        kValue)
                    {
                        bCollapse = false;
                    }
                }
            }

            if (bCollapse)
            {
                m_spQuaternionData = NULL;
                m_kQuaternionValue = kValue;
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiQuaternionInterpolator::SetPoseValue(NiQuaternion kPoseValue)
{
    m_spQuaternionData = NULL;
    m_kQuaternionValue = kPoseValue;
}
//---------------------------------------------------------------------------
void NiQuaternionInterpolator::GuaranteeTimeRange(float fStartTime, 
    float fEndTime)
{
    if (m_spQuaternionData)
    {
        m_spQuaternionData->GuaranteeKeysAtStartAndEnd(fStartTime, 
            fEndTime);
    }
}
//---------------------------------------------------------------------------
NiInterpolator* NiQuaternionInterpolator::GetSequenceInterpolator(
    float fStartTime, float fEndTime)
{
    NiQuaternionInterpolator* pkSeqInterp = (NiQuaternionInterpolator*)
        NiKeyBasedInterpolator::GetSequenceInterpolator(fStartTime, fEndTime);
    if (m_spQuaternionData)
    {
        NiRotDataPtr spNewQuaternionData = m_spQuaternionData
            ->GetSequenceData(fStartTime, fEndTime);
        pkSeqInterp->SetQuaternionData(spNewQuaternionData);
        pkSeqInterp->m_uiLastIdx = 0;
    }

    return pkSeqInterp;
}
//---------------------------------------------------------------------------
bool NiQuaternionInterpolator::Update(float fTime,
    NiObjectNET* pkInterpTarget, NiQuaternion& kValue)
{
    if (!TimeHasChanged(fTime))
    {
        kValue = m_kQuaternionValue;
        if (m_kQuaternionValue == INVALID_QUATERNION)
            return false;
        return true;
    }

    unsigned int uiNumKeys;
    NiRotKey::KeyType eType;
    unsigned char ucSize;
    NiRotKey* pkKeys = GetKeys(uiNumKeys, eType,ucSize);
    if (uiNumKeys > 0)
    {
        m_kQuaternionValue = NiRotKey::GenInterp(fTime, pkKeys, eType,
            uiNumKeys, m_uiLastIdx, ucSize);
    }
    
    if (m_kQuaternionValue == INVALID_QUATERNION)
    {
        return false;
    }

    kValue = m_kQuaternionValue;
    m_fLastTime = fTime;

    return true;

}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiQuaternionInterpolator);
//---------------------------------------------------------------------------
void NiQuaternionInterpolator::CopyMembers(NiQuaternionInterpolator* pkDest,
    NiCloningProcess& kCloning)
{
    NiKeyBasedInterpolator::CopyMembers(pkDest, kCloning);

    pkDest->m_kQuaternionValue = m_kQuaternionValue;
    pkDest->m_spQuaternionData = m_spQuaternionData;
    pkDest->m_uiLastIdx = m_uiLastIdx;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiQuaternionInterpolator);
//---------------------------------------------------------------------------
void NiQuaternionInterpolator::LoadBinary(NiStream& kStream)
{
    NiKeyBasedInterpolator::LoadBinary(kStream);

    m_kQuaternionValue.LoadBinary(kStream);
    m_spQuaternionData = (NiRotData*) kStream.ResolveLinkID();
}
//---------------------------------------------------------------------------
void NiQuaternionInterpolator::LinkObject(NiStream& kStream)
{
    NiKeyBasedInterpolator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiQuaternionInterpolator::RegisterStreamables(NiStream& kStream)
{
    if (!NiKeyBasedInterpolator::RegisterStreamables(kStream))
    {
        return false;
    }

    if (m_spQuaternionData)
        m_spQuaternionData->RegisterStreamables(kStream);

    return true;
}
//---------------------------------------------------------------------------
void NiQuaternionInterpolator::SaveBinary(NiStream& kStream)
{
    NiKeyBasedInterpolator::SaveBinary(kStream);

    m_kQuaternionValue.SaveBinary(kStream);
    kStream.SaveLinkID(m_spQuaternionData);
}
//---------------------------------------------------------------------------
bool NiQuaternionInterpolator::IsEqual(NiObject* pkObject)
{
    if (!NiKeyBasedInterpolator::IsEqual(pkObject))
    {
        return false;
    }

    NiQuaternionInterpolator* pkDest = (NiQuaternionInterpolator*) pkObject;

    if (m_kQuaternionValue != pkDest->m_kQuaternionValue)
    {
        return false;
    }
        
    if ((m_spQuaternionData && !pkDest->m_spQuaternionData) ||
        (!m_spQuaternionData && pkDest->m_spQuaternionData) ||
        (m_spQuaternionData && !m_spQuaternionData->IsEqual(
            pkDest->m_spQuaternionData)))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiQuaternionInterpolator::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiKeyBasedInterpolator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiQuaternionInterpolator::ms_RTTI
        .GetName()));

    pkStrings->Add(m_kQuaternionValue.GetViewerString("m_kQuaternionValue"));
    pkStrings->Add(NiGetViewerString("m_spQuaternionData",
        m_spQuaternionData));
}
//---------------------------------------------------------------------------
