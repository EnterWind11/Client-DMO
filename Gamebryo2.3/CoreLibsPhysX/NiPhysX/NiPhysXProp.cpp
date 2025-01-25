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
#include "NiPhysXPCH.h"

#include "NiPhysX.h"

NiImplementRTTI(NiPhysXProp, NiObjectNET);    
        
//---------------------------------------------------------------------------
NiPhysXProp::NiPhysXProp()
{
    m_pkScene = 0;

    m_kXform.id();
    m_fScalePToW = 1.0f;
    
    m_bKeepMeshes = false;
    m_spSnapshot = 0;
}
//---------------------------------------------------------------------------
NiPhysXProp::~NiPhysXProp()
{
    // Delete all the sources and destinations
    m_kSources.RemoveAll();
    m_kDestinations.RemoveAll();
        
    // Delete the snapshot
    m_spSnapshot = 0;
}
//---------------------------------------------------------------------------
void NiPhysXProp::AttachSceneCallback(NiPhysXScene* pkScene)
{
    m_pkScene = pkScene;
}
//---------------------------------------------------------------------------
void NiPhysXProp::DetachSceneCallback(NiPhysXScene* pkScene)
{
    m_pkScene = 0;
}
//---------------------------------------------------------------------------
void NiPhysXProp::SetAllSrcInterp(const bool bActive)
{
    for (unsigned int i = 0; i < m_kSources.GetSize(); i++)
        m_kSources.GetAt(i)->SetInterpolate(bActive);   
}
//---------------------------------------------------------------------------
void NiPhysXProp::SetAllDestInterp(const bool bActive)
{
    for (unsigned int i = 0; i < m_kDestinations.GetSize(); i++)
        m_kDestinations.GetAt(i)->SetInterpolate(bActive);   
}
//---------------------------------------------------------------------------
unsigned int NiPhysXProp::AddSource(NiPhysXSrc* pkSrc)
{
    if (m_pkScene)
        m_pkScene->AddSource(pkSrc);
    return m_kSources.AddFirstEmpty(pkSrc);
}
//---------------------------------------------------------------------------
bool NiPhysXProp::DeleteSource(NiPhysXSrc* pkTarget)
{
    if (m_pkScene)
        m_pkScene->DeleteSource(pkTarget);
    for (unsigned int i = 0; i < m_kSources.GetSize(); i++)
        if (m_kSources.GetAt(i) == pkTarget)
        {
            m_kSources.RemoveAtAndFill(i);
            return true;
        }
        
    return false;
}
//---------------------------------------------------------------------------
void NiPhysXProp::DeleteSourceAt(const unsigned int uiIndex)   
{
    NiPhysXSrcPtr spSrc = m_kSources.RemoveAtAndFill(uiIndex);
    NIASSERT(spSrc != 0);
    if (m_pkScene)
        m_pkScene->DeleteSource(spSrc);
    spSrc = 0;
}
//---------------------------------------------------------------------------
unsigned int NiPhysXProp::AddDestination(NiPhysXDest* pkDest)
{
    if (m_pkScene)
        m_pkScene->AddDestination(pkDest);
    return m_kDestinations.AddFirstEmpty(pkDest);
}
//---------------------------------------------------------------------------
bool NiPhysXProp::DeleteDestination(NiPhysXDest* pkTarget)
{
    if (m_pkScene)
        m_pkScene->DeleteDestination(pkTarget);

    for (unsigned int i = 0; i < m_kDestinations.GetSize(); i++)
        if (m_kDestinations.GetAt(i) == pkTarget)
        {
            m_kDestinations.RemoveAtAndFill(i);
            return true;
        }
        
    return false;
}
//---------------------------------------------------------------------------
void NiPhysXProp::DeleteDestinationAt(const unsigned int uiIndex) 
{
    NiPhysXDestPtr spDest = m_kDestinations.RemoveAtAndFill(uiIndex);
    NIASSERT(spDest != 0);
    if (m_pkScene)
        m_pkScene->DeleteDestination(spDest);
    spDest = 0;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning Functions
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXProp);
//---------------------------------------------------------------------------
void NiPhysXProp::CopyMembers(NiPhysXProp* pkDest,
    NiCloningProcess& kCloning)
{
    NiObjectNET::CopyMembers(pkDest, kCloning);
    
    pkDest->m_pkScene = 0;
    
    pkDest->m_kXform = m_kXform;
    pkDest->m_fScalePToW = m_fScalePToW;

    pkDest->m_kSources.SetSize(m_kSources.GetSize());
    for (unsigned int ui = 0; ui < m_kSources.GetSize(); ui++)
    {
        NiPhysXSrc* pkSrcClone = (NiPhysXSrc*)
            m_kSources.GetAt(ui)->CreateSharedClone(kCloning);
        pkDest->m_kSources.SetAt(ui, pkSrcClone);
    }
    pkDest->m_kDestinations.SetSize(m_kDestinations.GetSize());
    for (unsigned int ui = 0; ui < m_kDestinations.GetSize(); ui++)
    {
        NiPhysXDest* pkDestClone = (NiPhysXDest*)
            m_kDestinations.GetAt(ui)->CreateSharedClone(kCloning);
        pkDest->m_kDestinations.SetAt(ui, pkDestClone);
    }

    // Snapshots
    pkDest->m_bKeepMeshes = m_bKeepMeshes;
    if (m_spSnapshot)
    {
        pkDest->m_spSnapshot =
            (NiPhysXPropDesc*)m_spSnapshot->CreateSharedClone(kCloning);
    }
    else
    {
        pkDest->m_spSnapshot = 0;
    }
}
//---------------------------------------------------------------------------
void NiPhysXProp::ProcessClone(NiCloningProcess& kCloning)
{
    NiObjectNET::ProcessClone(kCloning);

    bool bCloned;
    NiObject* pkCloneObj = 0;
    bCloned = kCloning.m_pkCloneMap->GetAt(this, pkCloneObj);
    if (!bCloned)
        return;
    
    for (unsigned int ui = 0; ui < m_kSources.GetSize(); ui++)
    {
        bool bJunk;
        NiPhysXSrc* pkSrc = m_kSources.GetAt(ui);
        if (!kCloning.m_pkProcessMap->GetAt(pkSrc, bJunk))
            pkSrc->ProcessClone(kCloning);
    }
    for (unsigned int ui = 0; ui < m_kDestinations.GetSize(); ui++)
    {
        bool bJunk;
        NiPhysXDest* pkDest = m_kDestinations.GetAt(ui);
        if (!kCloning.m_pkProcessMap->GetAt(pkDest, bJunk))
            pkDest->ProcessClone(kCloning);
    }
    if (m_spSnapshot)
    {
        bool bJunk;
        if (!kCloning.m_pkProcessMap->GetAt(m_spSnapshot, bJunk))
            m_spSnapshot->ProcessClone(kCloning);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXProp::RegisterStreamables(NiStream& kStream)
{
    if (!NiObjectNET::RegisterStreamables(kStream))
        return false;
        
    // Sources and destinations
    for (unsigned int i = 0; i < m_kSources.GetSize(); i++)
    {
        NiPhysXSrcPtr spSrc = m_kSources.GetAt(i);
        NIASSERT(spSrc != 0);
        spSrc->RegisterStreamables(kStream);
    }
    for (unsigned int i = 0; i < m_kDestinations.GetSize(); i++)
    {
        NiPhysXDestPtr spDest = m_kDestinations.GetAt(i);
        NIASSERT(spDest != 0);
        spDest->RegisterStreamables(kStream);
    }
       
    // Make sure we have at least one snapshot for streaming
    if (m_spSnapshot)
        m_spSnapshot->RegisterStreamables(kStream);
    
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXProp::SaveBinary(NiStream& kStream)
{
    NiObjectNET::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_fScalePToW);
    
    // Sources and Destinations
    NiStreamSaveBinary(kStream, m_kSources.GetSize());
    for (unsigned int i = 0; i < m_kSources.GetSize(); i++)
    {
        kStream.SaveLinkID(m_kSources.GetAt(i));
    }
    NiStreamSaveBinary(kStream, m_kDestinations.GetSize());
    for (unsigned int i = 0; i < m_kDestinations.GetSize(); i++)
    {
        kStream.SaveLinkID(m_kDestinations.GetAt(i));
    }
    
    NiStreamSaveBinary(kStream, NiBool(m_bKeepMeshes));

    kStream.SaveLinkID(m_spSnapshot);
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXProp);
//---------------------------------------------------------------------------
void NiPhysXProp::LoadBinary(NiStream& kStream)
{
    NiObjectNET::LoadBinary(kStream);
    
    NiStreamLoadBinary(kStream, m_fScalePToW);

    unsigned int uiSize;

    // Sources and Destinations
    NiStreamLoadBinary(kStream, uiSize);
    m_kSources.SetSize(uiSize);
    for (unsigned int i = 0; i < uiSize; i++)
    {
        m_kSources.SetAt(i, (NiPhysXSrc*)kStream.ResolveLinkID());
    }
    NiStreamLoadBinary(kStream, uiSize);
    m_kDestinations.SetSize(uiSize);
    for (unsigned int i = 0; i < uiSize; i++)
    {
        m_kDestinations.SetAt(i, (NiPhysXDest*)kStream.ResolveLinkID());
    }
    
    NiBool bVal;
    NiStreamLoadBinary(kStream, bVal);
    m_bKeepMeshes = bVal ? true : false;

    m_spSnapshot = (NiPhysXPropDesc*)kStream.ResolveLinkID();
}
//---------------------------------------------------------------------------
void NiPhysXProp::LinkObject(NiStream& kStream)
{
    NiObjectNET::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXProp::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXProp, pkObject));
    if(!NiObjectNET::IsEqual(pkObject))
        return false;
        
    NiPhysXProp* pkNewObject = (NiPhysXProp*)pkObject;
        
    NiMatrix3 kNewM;
    NiPoint3 kNewP;
    NiMatrix3 kThisM;
    NiPoint3 kThisP;
    NiPhysXTypes::NxMat34ToNiTransform(pkNewObject->m_kXform, kNewM, kNewP);
    NiPhysXTypes::NxMat34ToNiTransform(m_kXform, kThisM, kThisP);
    if (kNewM != kThisM || kNewP != kThisP)
        return false;

    if (m_kSources.GetSize() != pkNewObject->m_kSources.GetSize())
        return false;
    for (unsigned int i = 0; i < m_kSources.GetSize(); i++)
    {
        NiPhysXSrc* pkSrc = m_kSources.GetAt(i);
        NiPhysXSrc* pkNewSrc = pkNewObject->m_kSources.GetAt(i);
        if (!pkSrc->IsEqual(pkNewSrc))
            return false;
    }
    if (m_kDestinations.GetSize() != pkNewObject->m_kDestinations.GetSize())
        return false;
    for (unsigned int i = 0; i < m_kDestinations.GetSize(); i++)
    {
        NiPhysXDest* pkDest = m_kDestinations.GetAt(i);
        NiPhysXDest* pkNewDest = pkNewObject->m_kDestinations.GetAt(i);
        if (!pkDest->IsEqual(pkNewDest))
            return false;
    }

    if (m_bKeepMeshes != pkNewObject->m_bKeepMeshes)
    {
        return false;
    }
    
    if (m_spSnapshot)
    {
        if (!pkNewObject->m_spSnapshot)
            return false;
        if (!m_spSnapshot->IsEqual(pkNewObject->m_spSnapshot))
            return false;
    }
    else
    {
        if (pkNewObject->m_spSnapshot)
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
