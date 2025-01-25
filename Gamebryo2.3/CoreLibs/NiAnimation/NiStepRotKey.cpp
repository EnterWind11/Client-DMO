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

#include "NiStepRotKey.h"

NiImplementAnimationStream(NiStepRotKey,ROTKEY,STEPKEY);

//---------------------------------------------------------------------------
void NiStepRotKey::RegisterSupportedFunctions(KeyContent eContent,
    KeyType eType)
{
    SetCurvatureFunction(eContent, eType, NULL);
    SetInterpFunction(eContent, eType, Interpolate);
    SetInterpD1Function(eContent, eType, NULL);
    SetInterpD2Function(eContent, eType, NULL);
    SetEqualFunction(eContent, eType, Equal);
    SetFillDerivedValsFunction(eContent, eType, FillDerivedVals);
    SetInsertFunction(eContent, eType, Insert);
}
//---------------------------------------------------------------------------
void NiStepRotKey::FillDerivedVals(NiAnimationKey* pkKeys, 
    unsigned int uiNumKeys, unsigned char ucSize)
{
    NiRotKey::FillDerivedVals(pkKeys,uiNumKeys,ucSize);
}
//---------------------------------------------------------------------------
void NiStepRotKey::Copy(NiAnimationKey* pkNewKey,
    const NiAnimationKey* pkOrigKey)
{
    NiStepRotKey* pkNewRot = (NiStepRotKey*) pkNewKey;
    NiStepRotKey* pkOrigRot = (NiStepRotKey*) pkOrigKey;

    pkNewRot->m_fTime = pkOrigRot->m_fTime;
    pkNewRot->m_quat = pkOrigRot->m_quat;
}
//---------------------------------------------------------------------------
NiAnimationKey* NiStepRotKey::CreateArray(unsigned int uiNumKeys)
{
    return NiNew NiStepRotKey[uiNumKeys];
}
//---------------------------------------------------------------------------
void NiStepRotKey::DeleteArray(NiAnimationKey* pkKeyArray)
{
    NiStepRotKey* pkStepRotKeyArray = (NiStepRotKey*) pkKeyArray;
    NiDelete[] pkStepRotKeyArray;
}
//---------------------------------------------------------------------------
void NiStepRotKey::Interpolate(float fTime, const NiAnimationKey* pkKey0,
    const NiAnimationKey* pkKey1, void* pvResult)
{
    NiRotKey* pkRotKey0 = (NiRotKey*)pkKey0;
    NiRotKey* pkRotKey1 = (NiRotKey*)pkKey1;
    if (fTime < 1.0f)
        *(NiQuaternion*)pvResult = pkRotKey0->GetQuaternion();
    else
        *(NiQuaternion*)pvResult = pkRotKey1->GetQuaternion();
}
//---------------------------------------------------------------------------
bool NiStepRotKey::Insert(float fTime, NiAnimationKey*& pkKeys, 
    unsigned int& uiNumKeys)
{
    NiStepRotKey* pkStepRotKeys = (NiStepRotKey*) pkKeys;

    unsigned int uiDestSize;
    unsigned int uiInsertAt;
    if (OkayToInsert(fTime, pkStepRotKeys, uiNumKeys, uiInsertAt,
        sizeof(NiStepRotKey)))
    {
        NiStepRotKey* pkNewKeys = NiNew NiStepRotKey[uiNumKeys + 1];
        uiDestSize = (uiNumKeys + 1) * sizeof(NiStepRotKey);
        NiMemcpy(pkNewKeys, uiDestSize, pkStepRotKeys,
            uiInsertAt * sizeof(NiStepRotKey));
        if (uiNumKeys > uiInsertAt)
        {
            uiDestSize = (uiNumKeys - uiInsertAt) * sizeof(NiStepRotKey);
            NiMemcpy(&pkNewKeys[uiInsertAt + 1], &pkStepRotKeys[uiInsertAt],
                uiDestSize);
        }

        NiQuaternion kQ = NiRotKey::GenInterpDefault(fTime, pkStepRotKeys,
            STEPKEY, uiNumKeys, sizeof(NiStepRotKey));

        NiStepRotKey* pkNewKey = &pkNewKeys[uiInsertAt];
        pkNewKey->m_fTime = fTime;
        pkNewKey->m_quat = kQ;

        uiNumKeys++;
        NiDelete[] pkStepRotKeys;
        pkKeys = pkNewKeys;
        FillDerivedVals(pkKeys, uiNumKeys, sizeof(NiStepRotKey));
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiAnimationKey* NiStepRotKey::CreateFromStream(NiStream& stream,
    unsigned int uiNumKeys)
{
    NiStepRotKey* pkStepRotKeys = NiNew NiStepRotKey[uiNumKeys];
    for (unsigned int ui = 0; ui < uiNumKeys; ui++)
    {
        pkStepRotKeys[ui].LoadBinary(stream);
    }
    return pkStepRotKeys;
}
//---------------------------------------------------------------------------
void NiStepRotKey::LoadBinary(NiStream& stream)
{
    NiRotKey::LoadBinary(stream);
}
//---------------------------------------------------------------------------
void NiStepRotKey::SaveToStream(NiStream& stream, NiAnimationKey* pkKeys,
    unsigned int uiNumKeys)
{
    NiStepRotKey* pkStepRotKeys = (NiStepRotKey*) pkKeys;

    for (unsigned int ui = 0; ui < uiNumKeys; ui++)
    {
        SaveBinary(stream, &pkStepRotKeys[ui]);
    }
}
//---------------------------------------------------------------------------
void NiStepRotKey::SaveBinary(NiStream& stream, NiAnimationKey* pkKey)
{
    NiRotKey::SaveBinary(stream, pkKey);
}
//---------------------------------------------------------------------------
