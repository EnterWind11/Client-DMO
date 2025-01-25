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

#include "NiBezRotKey.h"

NiImplementAnimationStream(NiBezRotKey,ROTKEY,BEZKEY);

//---------------------------------------------------------------------------
void NiBezRotKey::RegisterSupportedFunctions(KeyContent eContent,
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
bool NiBezRotKey::Equal(const NiAnimationKey& key0, 
    const NiAnimationKey& key1)
{
    if ( !NiRotKey::Equal(key0,key1) )
        return false;

    const NiBezRotKey& bez0 = (const NiBezRotKey&) key0;
    const NiBezRotKey& bez1 = (const NiBezRotKey&) key1;

    return bez0.m_IntQuat == bez1.m_IntQuat;
}
//---------------------------------------------------------------------------
void NiBezRotKey::Copy(NiAnimationKey* pkNewKey,
    const NiAnimationKey* pkOrigKey)
{
    NiBezRotKey* pkNewBez = (NiBezRotKey*) pkNewKey;
    NiBezRotKey* pkOrigBez = (NiBezRotKey*) pkOrigKey;

    pkNewBez->m_fTime = pkOrigBez->m_fTime;
    pkNewBez->m_quat = pkOrigBez->m_quat;
    pkNewBez->m_IntQuat = pkOrigBez->m_IntQuat;
}
//---------------------------------------------------------------------------
NiAnimationKey* NiBezRotKey::CreateArray(unsigned int uiNumKeys)
{
    return NiNew NiBezRotKey[uiNumKeys];
}
//---------------------------------------------------------------------------
void NiBezRotKey::DeleteArray(NiAnimationKey* pkKeyArray)
{
    NiBezRotKey* pkBezRotKeyArray = (NiBezRotKey*) pkKeyArray;
    NiDelete[] pkBezRotKeyArray;
}
//---------------------------------------------------------------------------
void NiBezRotKey::Interpolate(float fTime, const NiAnimationKey* pKey0,
    const NiAnimationKey* pKey1, void* pResult)
{
    NiBezRotKey* pBez0 = (NiBezRotKey*) pKey0;
    NiBezRotKey* pBez1 = (NiBezRotKey*) pKey1;

    *(NiQuaternion*)pResult = NiQuaternion::Squad(fTime,pBez0->m_quat,
        pBez0->m_IntQuat,pBez1->m_IntQuat,pBez1->m_quat);
}
//---------------------------------------------------------------------------
void NiBezRotKey::FillDerivedVals(NiAnimationKey* pkKeys, 
    unsigned int uiNumKeys, unsigned char ucSize)
{
    if ( uiNumKeys >= 2 )
    {
        // consecutive quaternions are set to have an acute angle
        NiRotKey::FillDerivedVals(pkKeys, uiNumKeys, ucSize);

        // calculate the intermediate quaternions and angles
        NiBezRotKey* pkBezRotKeys = (NiBezRotKey*) pkKeys;

        pkBezRotKeys[0].m_IntQuat = NiQuaternion::Intermediate(
            pkBezRotKeys[0].m_quat, pkBezRotKeys[0].m_quat,
            pkBezRotKeys[1].m_quat);

        unsigned int uiNumKeysM1 = uiNumKeys - 1;
        for (unsigned int uiI = 1; uiI < uiNumKeysM1; uiI++)
        {
            pkBezRotKeys[uiI].m_IntQuat = NiQuaternion::Intermediate(
                pkBezRotKeys[uiI-1].m_quat, pkBezRotKeys[uiI].m_quat,
                pkBezRotKeys[uiI+1].m_quat);
        }

        pkBezRotKeys[uiNumKeysM1].m_IntQuat = NiQuaternion::Intermediate(
            pkBezRotKeys[uiNumKeys-2].m_quat,
            pkBezRotKeys[uiNumKeysM1].m_quat,
            pkBezRotKeys[uiNumKeysM1].m_quat);
    }
}
//---------------------------------------------------------------------------
bool NiBezRotKey::Insert(float fTime, NiAnimationKey*& pkKeys, 
    unsigned int& uiNumKeys)
{
    NiBezRotKey* pkBezRotKeys = (NiBezRotKey*) pkKeys;

    unsigned int uiDestSize;
    unsigned int uiInsertAt;
    if (OkayToInsert(fTime, pkBezRotKeys, uiNumKeys, uiInsertAt,
        sizeof(NiBezRotKey)))
    {
        NiBezRotKey* pkNewKeys = NiNew NiBezRotKey[uiNumKeys + 1];
        uiDestSize = (uiNumKeys + 1) * sizeof(NiBezRotKey);
        NiMemcpy(pkNewKeys, uiDestSize, pkBezRotKeys,
            uiInsertAt * sizeof(NiBezRotKey));
        if (uiNumKeys > uiInsertAt)
        {
            uiDestSize = (uiNumKeys - uiInsertAt) * sizeof(NiBezRotKey);
            NiMemcpy(&pkNewKeys[uiInsertAt + 1], &pkBezRotKeys[uiInsertAt],
                uiDestSize);
        }

        NiQuaternion kQ = NiRotKey::GenInterpDefault(fTime, pkBezRotKeys,
            BEZKEY, uiNumKeys, sizeof(NiBezRotKey));

        NiBezRotKey* pkNewKey = &pkNewKeys[uiInsertAt];
        pkNewKey->m_fTime = fTime;
        pkNewKey->m_quat = kQ;

        uiNumKeys++;
        NiDelete[] pkBezRotKeys;
        pkKeys = pkNewKeys;
        FillDerivedVals(pkKeys, uiNumKeys, sizeof(NiBezRotKey));
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
NiAnimationKey* NiBezRotKey::CreateFromStream(NiStream& stream,
    unsigned int uiNumKeys)
{
    NiBezRotKey* pkBezRotKeys = NiNew NiBezRotKey[uiNumKeys];
    for (unsigned int ui = 0; ui < uiNumKeys; ui++)
    {
        pkBezRotKeys[ui].LoadBinary(stream);
    }
    return pkBezRotKeys;
}
//---------------------------------------------------------------------------
void NiBezRotKey::LoadBinary(NiStream& stream)
{
    NiRotKey::LoadBinary(stream);
}
//---------------------------------------------------------------------------
void NiBezRotKey::SaveToStream(NiStream& stream, NiAnimationKey* pkKeys,
    unsigned int uiNumKeys)
{
    NiBezRotKey* pkBezRotKeys = (NiBezRotKey*) pkKeys;

    for (unsigned int ui = 0; ui < uiNumKeys; ui++)
    {
        SaveBinary(stream, &pkBezRotKeys[ui]);
    }
}
//---------------------------------------------------------------------------
void NiBezRotKey::SaveBinary(NiStream& stream, NiAnimationKey* pkKey)
{
    NiRotKey::SaveBinary(stream, pkKey);
}
//---------------------------------------------------------------------------
