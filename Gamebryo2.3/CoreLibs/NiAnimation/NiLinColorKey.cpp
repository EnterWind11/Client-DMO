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

#include "NiLinColorKey.h"

NiImplementAnimationStream(NiLinColorKey,COLORKEY,LINKEY);

//---------------------------------------------------------------------------
void NiLinColorKey::RegisterSupportedFunctions(KeyContent eContent,
    KeyType eType)
{
    SetCurvatureFunction(eContent, eType, NULL);
    SetInterpFunction(eContent, eType, Interpolate);
    SetInterpD1Function(eContent, eType, NULL);
    SetInterpD2Function(eContent, eType, NULL);
    SetEqualFunction(eContent, eType, Equal);
    SetFillDerivedValsFunction(eContent, eType, NULL);
    SetInsertFunction(eContent, eType, Insert);
}
//---------------------------------------------------------------------------
void NiLinColorKey::Copy(NiAnimationKey* pkNewKey,
    const NiAnimationKey* pkOrigKey)
{
    NiLinColorKey* pkNewColor = (NiLinColorKey*) pkNewKey;
    NiLinColorKey* pkOrigColor = (NiLinColorKey*) pkOrigKey;

    pkNewColor->m_fTime = pkOrigColor->m_fTime;
    pkNewColor->m_Color = pkOrigColor->m_Color;
}
//---------------------------------------------------------------------------
NiAnimationKey* NiLinColorKey::CreateArray(unsigned int uiNumKeys)
{
    return NiNew NiLinColorKey[uiNumKeys];
}
//---------------------------------------------------------------------------
void NiLinColorKey::DeleteArray(NiAnimationKey* pkKeyArray)
{
    NiLinColorKey* pkLinColorKeyArray = (NiLinColorKey*) pkKeyArray;
    NiDelete[] pkLinColorKeyArray;
}
//---------------------------------------------------------------------------
void NiLinColorKey::Interpolate(float fTime, const NiAnimationKey* pkKey0,
    const NiAnimationKey* pkKey1, void* pvResult)
{
    NiColorKey* pkColorKey0 = (NiColorKey*)pkKey0;
    NiColorKey* pkColorKey1 = (NiColorKey*)pkKey1;

    *(NiColorA*)pvResult = fTime*pkColorKey1->GetColor() + 
        (1.0f-fTime)*pkColorKey0->GetColor();
}
//---------------------------------------------------------------------------
bool NiLinColorKey::Insert(float fTime, NiAnimationKey*& pkKeys, 
    unsigned int& uiNumKeys)
{
    NiLinColorKey* pkLinColorKeys = (NiLinColorKey*) pkKeys;

    unsigned int uiDestSize;
    unsigned int uiInsertAt;
    if (OkayToInsert(fTime, pkLinColorKeys, uiNumKeys, uiInsertAt,
        sizeof(NiLinColorKey)))
    {
        NiLinColorKey* pkNewKeys = NiNew NiLinColorKey[uiNumKeys + 1];
        uiDestSize = (uiNumKeys + 1) * sizeof(NiLinColorKey);
        NiMemcpy(pkNewKeys, uiDestSize, pkLinColorKeys,
            uiInsertAt * sizeof(NiLinColorKey));
        if (uiNumKeys > uiInsertAt)
        {
            uiDestSize = (uiNumKeys - uiInsertAt) * sizeof(NiLinColorKey);
            NiMemcpy(&pkNewKeys[uiInsertAt + 1], &pkLinColorKeys[uiInsertAt],
                uiDestSize);
        }

        NiColorA kColor = NiColorKey::GenInterpDefault(fTime, pkLinColorKeys,
                LINKEY, uiNumKeys, sizeof(NiLinColorKey));

        NiLinColorKey* pkNewKey = &pkNewKeys[uiInsertAt];
        pkNewKey->m_fTime = fTime;
        pkNewKey->m_Color = kColor;

        uiNumKeys++;
        NiDelete[] pkLinColorKeys;
        pkKeys = pkNewKeys;
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
NiAnimationKey* NiLinColorKey::CreateFromStream(NiStream& kStream,
    unsigned int uiNumKeys)
{
    NiLinColorKey* pkLinColorKeys = NiNew NiLinColorKey[uiNumKeys];
    for (unsigned int ui = 0; ui < uiNumKeys; ui++)
    {
        pkLinColorKeys[ui].LoadBinary(kStream);
    }
    return pkLinColorKeys;
}
//---------------------------------------------------------------------------
void NiLinColorKey::LoadBinary(NiStream& kStream)
{
    NiColorKey::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiLinColorKey::SaveToStream(NiStream& stream, NiAnimationKey* pkKeys,
    unsigned int uiNumKeys)
{
    NiLinColorKey* pkLinColorKeys = (NiLinColorKey*) pkKeys;

    for (unsigned int ui = 0; ui < uiNumKeys; ui++)
    {
        SaveBinary(stream, &pkLinColorKeys[ui]);
    }
}
//---------------------------------------------------------------------------
void NiLinColorKey::SaveBinary(NiStream& kStream, NiAnimationKey* pkKey)
{
    NiColorKey::SaveBinary(kStream, pkKey);
}
//---------------------------------------------------------------------------

