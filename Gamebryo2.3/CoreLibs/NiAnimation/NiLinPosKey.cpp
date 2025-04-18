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

#include "NiLinPosKey.h"

NiImplementAnimationStream(NiLinPosKey,POSKEY,LINKEY);

//---------------------------------------------------------------------------
void NiLinPosKey::RegisterSupportedFunctions(KeyContent eContent,
    KeyType eType)
{
    SetCurvatureFunction(eContent, eType, ComputeMaxCurvature);
    SetInterpFunction(eContent, eType, Interpolate);
    SetInterpD1Function(eContent, eType, InterpolateD1);
    SetInterpD2Function(eContent, eType, InterpolateD2);
    SetEqualFunction(eContent, eType, Equal);
    SetFillDerivedValsFunction(eContent, eType, FillDerivedVals);
    SetInsertFunction(eContent, eType, Insert);
}
//---------------------------------------------------------------------------
float NiLinPosKey::ComputeMaxCurvature(NiAnimationKey*, unsigned int)
{
    // curvature for a line segment is identically zero
    return 0.0f;
}
//---------------------------------------------------------------------------
void NiLinPosKey::Copy(NiAnimationKey* pkNewKey,
    const NiAnimationKey* pkOrigKey)
{
    NiLinPosKey* pkNewPos = (NiLinPosKey*) pkNewKey;
    NiLinPosKey* pkOrigPos = (NiLinPosKey*) pkOrigKey;

    pkNewPos->m_fTime = pkOrigPos->m_fTime;
    pkNewPos->m_Pos = pkOrigPos->m_Pos;
}
//---------------------------------------------------------------------------
NiAnimationKey* NiLinPosKey::CreateArray(unsigned int uiNumKeys)
{
    return NiNew NiLinPosKey[uiNumKeys];
}
//---------------------------------------------------------------------------
void NiLinPosKey::DeleteArray(NiAnimationKey* pkKeyArray)
{
    NiLinPosKey* pkLinPosKeyArray = (NiLinPosKey*) pkKeyArray;
    NiDelete[] pkLinPosKeyArray;
}
//---------------------------------------------------------------------------
void NiLinPosKey::Interpolate(float fTime, const NiAnimationKey* pKey0,
    const NiAnimationKey* pKey1, void* pResult)
{
    NiPosKey* pPosKey0 = (NiPosKey*)pKey0;
    NiPosKey* pPosKey1 = (NiPosKey*)pKey1;

    // interpolate positions
    *(NiPoint3*)pResult = fTime*pPosKey1->GetPos() + 
        (1.0f-fTime)*pPosKey0->GetPos();
}
//---------------------------------------------------------------------------
void NiLinPosKey::InterpolateD1(float fTime, const NiAnimationKey* pKey0,
    const NiAnimationKey* pKey1, void* pResult)
{
    NiPosKey* pPosKey0 = (NiPosKey*)pKey0;
    NiPosKey* pPosKey1 = (NiPosKey*)pKey1;

    // interpolate first derivative
    *(NiPoint3*)pResult = pPosKey1->GetPos() - pPosKey0->GetPos();
}
//---------------------------------------------------------------------------
void NiLinPosKey::InterpolateD2(float fTime, const NiAnimationKey* pKey0,
    const NiAnimationKey* pKey1, void* pResult)
{
    // interpolate second derivative
    *(NiPoint3*)pResult = NiPoint3::ZERO;
}
//---------------------------------------------------------------------------
bool NiLinPosKey::Insert(float fTime, NiAnimationKey*& pkKeys, 
    unsigned int& uiNumKeys)
{
    NiLinPosKey* pkLinPosKeys = (NiLinPosKey*) pkKeys;

    unsigned int uiDestSize;
    unsigned int uiInsertAt;
    if (OkayToInsert(fTime, pkLinPosKeys, uiNumKeys, uiInsertAt,
        sizeof(NiLinPosKey)))
    {
        NiLinPosKey* pkNewKeys = NiNew NiLinPosKey[uiNumKeys + 1];
        uiDestSize = (uiNumKeys + 1) * sizeof(NiLinPosKey);
        NiMemcpy(pkNewKeys, uiDestSize, pkLinPosKeys, 
            uiInsertAt * sizeof(NiLinPosKey));
        if (uiNumKeys > uiInsertAt)
        {
            uiDestSize = (uiNumKeys - uiInsertAt) * sizeof(NiLinPosKey);
            NiMemcpy(&pkNewKeys[uiInsertAt + 1], &pkLinPosKeys[uiInsertAt],
                uiDestSize);
        }

        NiPoint3 kPt = NiPosKey::GenInterpDefault(fTime, pkLinPosKeys,
                LINKEY, uiNumKeys, sizeof(NiLinPosKey));

        NiLinPosKey* pkNewKey = &pkNewKeys[uiInsertAt];
        pkNewKey->m_fTime = fTime;
        pkNewKey->m_Pos = kPt;

        uiNumKeys++;
        NiDelete[] pkLinPosKeys;
        pkKeys = pkNewKeys;
        FillDerivedVals(pkKeys, uiNumKeys, sizeof(NiLinPosKey));
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
NiAnimationKey* NiLinPosKey::CreateFromStream(NiStream& stream,
    unsigned int uiNumKeys)
{
    NiLinPosKey* pkLinPosKeys = NiNew NiLinPosKey[uiNumKeys];
    for (unsigned int ui = 0; ui < uiNumKeys; ui++)
    {
        pkLinPosKeys[ui].LoadBinary(stream);
    }
    return pkLinPosKeys;
}
//---------------------------------------------------------------------------
void NiLinPosKey::LoadBinary(NiStream& stream)
{
    NiPosKey::LoadBinary(stream);
}
//---------------------------------------------------------------------------
void NiLinPosKey::SaveToStream(NiStream& stream, NiAnimationKey* pkKeys,
    unsigned int uiNumKeys)
{
    NiLinPosKey* pkLinPosKeys = (NiLinPosKey*) pkKeys;

    for (unsigned int ui = 0; ui < uiNumKeys; ui++)
    {
        SaveBinary(stream, &pkLinPosKeys[ui]);
    }
}
//---------------------------------------------------------------------------
void NiLinPosKey::SaveBinary(NiStream& stream, NiAnimationKey* pkKey)
{
    NiPosKey::SaveBinary(stream, pkKey);
}
//---------------------------------------------------------------------------
