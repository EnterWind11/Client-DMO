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

#include "NiLinRotKey.h"

NiImplementAnimationStream(NiLinRotKey,ROTKEY,LINKEY);

//---------------------------------------------------------------------------
void NiLinRotKey::RegisterSupportedFunctions(KeyContent eContent,
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
void NiLinRotKey::FillDerivedVals(NiAnimationKey* pkKeys, 
    unsigned int uiNumKeys, unsigned char ucSize)
{
    NiRotKey::FillDerivedVals(pkKeys,uiNumKeys,ucSize);
}
//---------------------------------------------------------------------------
void NiLinRotKey::Copy(NiAnimationKey* pkNewKey,
    const NiAnimationKey* pkOrigKey)
{
    NiLinRotKey* pkNewRot = (NiLinRotKey*) pkNewKey;
    NiLinRotKey* pkOrigRot = (NiLinRotKey*) pkOrigKey;

    pkNewRot->m_fTime = pkOrigRot->m_fTime;
    pkNewRot->m_quat = pkOrigRot->m_quat;
}
//---------------------------------------------------------------------------
NiAnimationKey* NiLinRotKey::CreateArray(unsigned int uiNumKeys)
{
    return NiNew NiLinRotKey[uiNumKeys];
}
//---------------------------------------------------------------------------
void NiLinRotKey::DeleteArray(NiAnimationKey* pkKeyArray)
{
    NiLinRotKey* pkLinRotKeyArray = (NiLinRotKey*) pkKeyArray;
    NiDelete[] pkLinRotKeyArray;
}
//---------------------------------------------------------------------------
void NiLinRotKey::Interpolate(float fTime, const NiAnimationKey* pKey0,
    const NiAnimationKey* pKey1, void* pResult)
{
    NiRotKey* pRotKey0 = (NiRotKey*)pKey0;
    NiRotKey* pRotKey1 = (NiRotKey*)pKey1;

    *(NiQuaternion*)pResult = NiQuaternion::Slerp(fTime,
        pRotKey0->GetQuaternion(), pRotKey1->GetQuaternion());
}
//---------------------------------------------------------------------------
bool NiLinRotKey::Insert(float fTime, NiAnimationKey*& pkKeys, 
    unsigned int& uiNumKeys)
{
    NiLinRotKey* pkLinRotKeys = (NiLinRotKey*) pkKeys;

    unsigned int uiDestSize;
    unsigned int uiInsertAt;
    if (OkayToInsert(fTime, pkLinRotKeys, uiNumKeys, uiInsertAt,
        sizeof(NiLinRotKey)))
    {
        NiLinRotKey* pkNewKeys = NiNew NiLinRotKey[uiNumKeys + 1];
        uiDestSize = (uiNumKeys + 1) * sizeof(NiLinRotKey);
        NiMemcpy(pkNewKeys, uiDestSize, pkLinRotKeys,
            uiInsertAt * sizeof(NiLinRotKey));
        if (uiNumKeys > uiInsertAt)
        {
            uiDestSize = (uiNumKeys - uiInsertAt) * sizeof(NiLinRotKey);
            NiMemcpy(&pkNewKeys[uiInsertAt + 1], &pkLinRotKeys[uiInsertAt], 
                uiDestSize);
        }

        NiQuaternion kQ = NiRotKey::GenInterpDefault(fTime, pkLinRotKeys,
            LINKEY, uiNumKeys, sizeof(NiLinRotKey));

        NiLinRotKey* pkNewKey = &pkNewKeys[uiInsertAt];
        pkNewKey->m_fTime = fTime;
        pkNewKey->m_quat = kQ;

        uiNumKeys++;
        NiDelete[] pkLinRotKeys;
        pkKeys = pkNewKeys;
        FillDerivedVals(pkKeys, uiNumKeys, sizeof(NiLinRotKey));
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
NiAnimationKey* NiLinRotKey::CreateFromStream(NiStream& stream,
    unsigned int uiNumKeys)
{
    NiLinRotKey* pkLinRotKeys = NiNew NiLinRotKey[uiNumKeys];
    for (unsigned int ui = 0; ui < uiNumKeys; ui++)
    {
        pkLinRotKeys[ui].LoadBinary(stream);
    }
    return pkLinRotKeys;
}
//---------------------------------------------------------------------------
void NiLinRotKey::LoadBinary(NiStream& stream)
{
    NiRotKey::LoadBinary(stream);
}
//---------------------------------------------------------------------------
void NiLinRotKey::SaveToStream(NiStream& stream, NiAnimationKey* pkKeys,
    unsigned int uiNumKeys)
{
    NiLinRotKey* pkLinRotKeys = (NiLinRotKey*) pkKeys;

    for (unsigned int ui = 0; ui < uiNumKeys; ui++)
    {
        SaveBinary(stream, &pkLinRotKeys[ui]);
    }
}
//---------------------------------------------------------------------------
void NiLinRotKey::SaveBinary(NiStream& stream, NiAnimationKey* pkKey)
{
    NiRotKey::SaveBinary(stream, pkKey);
}
//---------------------------------------------------------------------------
