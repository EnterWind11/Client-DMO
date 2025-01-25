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

#include "NiPhysXPCH.h"

#include "NiPhysX.h"

#include <NxPhysics.h>

NiImplementRTTI(NiPhysXActorDesc, NiObject);

//---------------------------------------------------------------------------
NiPhysXActorDesc::NiPhysXActorDesc() : m_kActorName(NULL), m_kActorShapes(0,1)
{
    NxActorDesc kActorDesc;
    kActorDesc.setToDefault();
    
    m_fDensity = kActorDesc.density;
    m_uiActorFlags = kActorDesc.flags;
    m_uiActorGroup = kActorDesc.group;

    m_spBodyDesc = 0;
    m_uiNumPoses = 0;
    m_pkPoses = 0;
    
    m_pkBodyDesc = 0;
    
    m_pkActor = 0;
    m_pkActorParent = 0;
    m_spSource = 0;
    m_spDest = 0;
}
//---------------------------------------------------------------------------
NiPhysXActorDesc::~NiPhysXActorDesc()
{
    m_spBodyDesc = 0;
    
    NiFree(m_pkPoses);
    
    m_pkActorParent = 0;
    m_spSource = 0;
    m_spDest = 0;

    m_kActorShapes.RemoveAll();
    NiExternalDelete m_pkBodyDesc;
}
//---------------------------------------------------------------------------
void NiPhysXActorDesc::SetConstants(const NiFixedString& kActorName,
    const NxReal fDensity, const NxU32 uiActorFlags,
    const NxActorGroup uiActorGroup, NiPhysXBodyDesc* pkBodyDesc,
    const NiTObjectArray<NiPhysXShapeDescPtr>& kActorShapes)
{
    m_pkActor = 0;

    m_kActorName = kActorName;
    m_fDensity = fDensity;
    m_uiActorFlags = uiActorFlags;
    m_uiActorGroup = uiActorGroup;
    
    m_spBodyDesc = pkBodyDesc;

    NxMat34 kPose;
    SetPose(kPose, 0);

    m_kActorShapes.RemoveAll();
    for (unsigned int ui = 0; ui < kActorShapes.GetSize(); ui++)
        m_kActorShapes.Add(kActorShapes.GetAt(ui));
}
//---------------------------------------------------------------------------
void NiPhysXActorDesc::ToActorDesc(NxActorDesc& kActorDesc,
    NxCompartment* pkCompartment, const unsigned int uiIndex,
    const NxMat34& kXform, const bool bKeepMeshes, const bool bCheckPlatform)
{
    kActorDesc.name = m_kActorName;    
    kActorDesc.globalPose.multiply(kXform, m_pkPoses[uiIndex]);
    if (m_spBodyDesc)
    {
        NiExternalDelete m_pkBodyDesc;
        m_pkBodyDesc = NiExternalNew NxBodyDesc;
        m_spBodyDesc->ToBodyDesc(*m_pkBodyDesc, uiIndex, kXform.M);
        kActorDesc.body = m_pkBodyDesc;
        if (m_pkBodyDesc->mass > 0.0f)
        {
            kActorDesc.density = 0.0f;
        }
        else
        {
            kActorDesc.density = m_fDensity;
        }
    }
    else
    {
        kActorDesc.body = 0;
        kActorDesc.density = m_fDensity;
    }
    kActorDesc.flags = m_uiActorFlags;
    kActorDesc.group = m_uiActorGroup;

    kActorDesc.compartment = pkCompartment;

    for (unsigned int i = 0; i < m_kActorShapes.GetSize(); i++)
    {
        NiPhysXShapeDescPtr spShape = m_kActorShapes.GetAt(i);
        if (spShape)
        {
            switch (spShape->GetType())
            {
                case NX_SHAPE_PLANE: {
                    NxPlaneShapeDesc* kShape = new NxPlaneShapeDesc;
                    spShape->ToPlaneDesc(*kShape);
                    kActorDesc.shapes.pushBack(kShape);
                    } break;

                case NX_SHAPE_SPHERE: {
                    NxSphereShapeDesc* kShape = new NxSphereShapeDesc;
                    spShape->ToSphereDesc(*kShape);
                    kActorDesc.shapes.pushBack(kShape);
                    } break;

                case NX_SHAPE_CAPSULE: {
                    NxCapsuleShapeDesc* kShape = new NxCapsuleShapeDesc;
                    spShape->ToCapsuleDesc(*kShape);
                    kActorDesc.shapes.pushBack(kShape);
                    } break;

                case NX_SHAPE_BOX: {
                    NxBoxShapeDesc* kShape = new NxBoxShapeDesc;
                    spShape->ToBoxDesc(*kShape);
                    kActorDesc.shapes.pushBack(kShape);
                    } break;
                
                case NX_SHAPE_CONVEX: {
                    NxConvexShapeDesc* kShape = new NxConvexShapeDesc;
                    spShape->ToConvexDesc(*kShape, bKeepMeshes,
                        bCheckPlatform);
                    if (kShape->meshData)
                        kActorDesc.shapes.pushBack(kShape);
                    } break;
                    
                case NX_SHAPE_MESH: {
                    NxTriangleMeshShapeDesc* kShape =
                        new NxTriangleMeshShapeDesc;
                    spShape->ToTriMeshDesc(
                        *kShape, bKeepMeshes, bCheckPlatform);
                    if (kShape->meshData)
                        kActorDesc.shapes.pushBack(kShape);
                    } break;
                    
                default:;
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiPhysXActorDesc::SetPose(const NxMat34& kPose, 
    const unsigned int uiIndex)
{
    if (m_uiNumPoses <= uiIndex)
    {
        NxMat34* pkNewArray = NiAlloc(NxMat34, uiIndex + 1);
        for (unsigned int ui = 0; ui < m_uiNumPoses; ui++)
        {
            pkNewArray[ui] = m_pkPoses[ui];
        }
        NiFree(m_pkPoses);
        m_pkPoses = pkNewArray;
        m_uiNumPoses = uiIndex + 1;
    }

    m_pkPoses[uiIndex] = kPose;
}
//---------------------------------------------------------------------------
NxActor* NiPhysXActorDesc::CreateActor(NxScene* pkScene,
    NxCompartment* pkCompartment, const unsigned int uiIndex,
    const NxMat34& kXform, const bool bKeepMeshes)
{
    NxActorDesc kActorDesc;
    ToActorDesc(kActorDesc, pkCompartment, uiIndex, kXform, bKeepMeshes);
            
    m_pkActor = pkScene->createActor(kActorDesc);
    if (!m_pkActor)
    {
#ifdef _DEBUG
        NiOutputDebugString(
            "NiPhysXActorDesc::CreateActor: Actor creation failed.\n");
        NiOutputDebugString(
            "NiPhysXActorDesc::CreateActor: Look for a message from the "
            "PhysX SDK as to why this might have occurred.\n");
#endif
    }
    else
    {    
        // Set shape pointers
        unsigned int uiCount = m_kActorShapes.GetEffectiveSize();
        NIASSERT(m_pkActor->getNbShapes() == uiCount);
        for (unsigned int ui = 0, uj = 0; ui < uiCount; ui++)
        {
            NiPhysXShapeDesc* pkShape = m_kActorShapes.GetAt(ui);
            if (pkShape)
            {
                pkShape->SetShape((m_pkActor->getShapes())[uj]);
                uj++;
            }
        }
    }
    
    // Clean up allocated memory
    NiExternalDelete m_pkBodyDesc;
    m_pkBodyDesc = 0;
    for (unsigned int i = 0; i < kActorDesc.shapes.size(); i++)
        delete kActorDesc.shapes[i];
    kActorDesc.shapes.clear();
    
    return m_pkActor;
}
//---------------------------------------------------------------------------
bool NiPhysXActorDesc::IsValid()
{
    bool bResult;
    
    // Do this with keep meshes true. If the mesh already exists, won't be
    // an issue at all. If it doesn't, we might create it but we won't
    // throw the data away. That makes this a suitable function to call
    // from an exporter. If the mesh is to be loaded from file, it will have
    // to have been saved already. Also tell it not to check the platform,
    // because this function might be called from export code to test a
    // different target.
    NxActorDesc kActorDesc;
    ToActorDesc(kActorDesc, 0, 0, NiPhysXTypes::NXMAT34_ID, true, false);

    bResult = kActorDesc.isValid();

    NiExternalDelete m_pkBodyDesc;
    m_pkBodyDesc = 0;
    for (unsigned int i = 0; i < kActorDesc.shapes.size(); i++)
        delete kActorDesc.shapes[i];
    kActorDesc.shapes.clear();
    
    return bResult;
}
//---------------------------------------------------------------------------
void NiPhysXActorDesc::RemoveActor(NxScene* pkScene)
{
    if (m_pkActor)
    {
        pkScene->releaseActor(*m_pkActor);
        m_pkActor = 0;
    }
    
    // De-activate any sources or destinations.
    if (m_spSource)
    {
        m_spSource->SetTarget(0);
    }
    if (m_spDest)
    {
        m_spDest->SetActor(0);
        if (NiIsKindOf(NiPhysXRigidBodyDest, m_spDest))
            m_spDest->SetActorParent(0);
    }
}
//---------------------------------------------------------------------------
unsigned int NiPhysXActorDesc::AddState(const NxMat34& kXform)
{
    if (!m_pkActor)
        return 0;
    
    NxMat34 kPose = kXform * m_pkActor->getGlobalPose();
    unsigned int uiIndex = m_uiNumPoses;
    SetPose(kPose, uiIndex);
    if (m_spBodyDesc)
    {
        NIVERIFY(m_spBodyDesc->AddState(m_pkActor, kXform.M) == uiIndex);
    }
    
    return uiIndex;
}
//---------------------------------------------------------------------------
void NiPhysXActorDesc::UpdateState(const unsigned int uiIndex,
    const NxMat34& kXform)
{
    NIASSERT(uiIndex < m_uiNumPoses);
    
    m_pkPoses[uiIndex] = kXform * m_pkActor->getGlobalPose();
    if (m_spBodyDesc)
        m_spBodyDesc->UpdateState(m_pkActor, uiIndex, kXform.M);
}
//---------------------------------------------------------------------------
void NiPhysXActorDesc::RestoreState(const unsigned int uiIndex,
    const NxMat34& kXform)
{
    // PhysX samples imply that this might not be deterministic. But their
    // solution, releasing the SDK, is not possible for us.
    if (!m_pkActor)
        return;

    NIASSERT(uiIndex < m_uiNumPoses);

    if (!m_pkActor->isDynamic())
        return;
    
    NIASSERT(m_spBodyDesc && "NiPhysXActorDesc::RestoreActor: No body desc");
    
    NxBodyDesc kBodyDesc;
    NxMat34 kPose = kXform * m_pkPoses[uiIndex];
    m_pkActor->setGlobalPose(kPose);
    m_spBodyDesc->RestoreState(m_pkActor, uiIndex, kXform.M);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning Functions
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXActorDesc);
//---------------------------------------------------------------------------
void NiPhysXActorDesc::CopyMembers(NiPhysXActorDesc* pkDest,
    NiCloningProcess& kCloning)
{
    NiObject::CopyMembers(pkDest, kCloning);

    // copy object names
    if (kCloning.m_eCopyType == NiObjectNET::COPY_EXACT)
    {
        pkDest->m_kActorName = m_kActorName;
    }
    else if (kCloning.m_eCopyType == NiObjectNET::COPY_UNIQUE)
    {
        if (m_kActorName)
        {
            int iQuantity = strlen(m_kActorName) + 2;
            char* pcNamePlus = NiAlloc(char, iQuantity);
            NIASSERT(pcNamePlus);
            NiStrcpy(pcNamePlus, iQuantity, m_kActorName);
            pcNamePlus[iQuantity - 2] = kCloning.m_cAppendChar;
            pcNamePlus[iQuantity - 1] = 0;
            pkDest->m_kActorName = pcNamePlus;
            NiFree(pcNamePlus);
        }
    }

    pkDest->m_uiNumPoses = m_uiNumPoses;
    pkDest->m_pkPoses = NiAlloc(NxMat34, m_uiNumPoses);
    for (unsigned int ui = 0; ui < m_uiNumPoses; ui++)
        pkDest->m_pkPoses[ui] = m_pkPoses[ui];

    if (m_spBodyDesc)
    {
        pkDest->m_spBodyDesc =
            (NiPhysXBodyDesc*)m_spBodyDesc->CreateClone(kCloning);
    }
    else
    {
        pkDest->m_spBodyDesc = 0;
    }
    pkDest->m_fDensity = m_fDensity;
    pkDest->m_uiActorFlags = m_uiActorFlags;
    pkDest->m_uiActorGroup = m_uiActorGroup;
    
    pkDest->m_kActorShapes.SetSize(m_kActorShapes.GetSize());
    for (unsigned int i = 0; i < m_kActorShapes.GetSize(); i++)
    {
        NiPhysXShapeDescPtr spShape = m_kActorShapes.GetAt(i);
        if (spShape)
        {
            pkDest->m_kActorShapes.SetAt(i,
                (NiPhysXShapeDesc*)spShape->CreateClone(kCloning));
        }
        else
        {
            pkDest->m_kActorShapes.SetAt(i, 0);
        }
    }
    
    pkDest->m_pkBodyDesc = 0;
    pkDest->m_pkActor = 0;
    pkDest->m_pkActorParent = m_pkActorParent;
    pkDest->m_spSource = m_spSource;
    pkDest->m_spDest = m_spDest;
}
//---------------------------------------------------------------------------
void NiPhysXActorDesc::ProcessClone(NiCloningProcess& kCloning)
{
    NiObject::ProcessClone(kCloning);

    bool bCloned;
    NiObject* pkCloneObj = 0;
    bCloned = kCloning.m_pkCloneMap->GetAt(this, pkCloneObj);
    if (!bCloned)
        return;
    NiPhysXActorDesc* pkClone = (NiPhysXActorDesc*)pkCloneObj;

    // Process clone on objects to which we have pointers and created
    // unique clones
    if (m_spBodyDesc)
    {
        m_spBodyDesc->ProcessClone(kCloning);
    }
    unsigned int uiCount = m_kActorShapes.GetSize();
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        NiPhysXShapeDescPtr spShape = m_kActorShapes.GetAt(ui);
        if (spShape)
        {
            spShape->ProcessClone(kCloning);
        }
    }

    // Check for clones of other objects.
    if (m_pkActorParent)
    {
        NiObject* pkClonedParentObj = 0;
        bCloned = kCloning.m_pkCloneMap->
            GetAt(m_pkActorParent, pkClonedParentObj);        
        if (bCloned)
        {
            pkClone->m_pkActorParent = (NiPhysXActorDesc*)pkClonedParentObj;
        }
    }
    
    if (m_spSource)
    {
        NiObject* pkClonedSourceObj = 0;
        bCloned = kCloning.m_pkCloneMap->
            GetAt(m_spSource, pkClonedSourceObj);        
        if (bCloned)
        {
            pkClone->m_spSource = (NiPhysXRigidBodySrc*)pkClonedSourceObj;
        }
    }
    
    if (m_spDest)
    {
        NiObject* pkClonedDestObj = 0;
        bCloned = kCloning.m_pkCloneMap->GetAt(m_spDest, pkClonedDestObj);
        if (bCloned)
        {
            pkClone->m_spDest = (NiPhysXRigidBodyDest*)pkClonedDestObj;
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXActorDesc::RegisterStreamables(NiStream& kStream)
{
    if (!NiObject::RegisterStreamables(kStream))
        return false;

    if (m_spBodyDesc)
        m_spBodyDesc->RegisterStreamables(kStream);
        
    for (unsigned int i = 0; i < m_kActorShapes.GetSize(); i++)
    {
        NiPhysXShapeDescPtr spShape = m_kActorShapes.GetAt(i);
        if (spShape)
            spShape->RegisterStreamables(kStream);
    }
    
    kStream.RegisterFixedString(m_kActorName);

    if (m_pkActorParent)
        m_pkActorParent->RegisterStreamables(kStream);
    if (m_spSource)
        m_spSource->RegisterStreamables(kStream);
    if (m_spDest)
        m_spDest->RegisterStreamables(kStream);

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXActorDesc::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);
    
    kStream.SaveFixedString(m_kActorName);
    
    NiStreamSaveBinary(kStream, m_uiNumPoses);
    for (unsigned int ui = 0; ui < m_uiNumPoses; ui++)
    {
        NiPhysXTypes::NxMat34SaveBinary(kStream, m_pkPoses[ui]);
    }
    
    kStream.SaveLinkID(m_spBodyDesc);
    
    NiStreamSaveBinary(kStream, m_fDensity);
    NiStreamSaveBinary(kStream, m_uiActorFlags);
    NiStreamSaveBinary(kStream, m_uiActorGroup);

    NiStreamSaveBinary(kStream, m_kActorShapes.GetEffectiveSize());
    for (unsigned int i = 0; i < m_kActorShapes.GetSize(); i++)
    {
        NiPhysXShapeDescPtr spShape = m_kActorShapes.GetAt(i);
        kStream.SaveLinkID(spShape);
    }

    kStream.SaveLinkID(m_pkActorParent);
    kStream.SaveLinkID(m_spSource);
    kStream.SaveLinkID(m_spDest);
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXActorDesc);
//---------------------------------------------------------------------------
void NiPhysXActorDesc::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    kStream.LoadFixedString(m_kActorName);
    
    NiStreamLoadBinary(kStream, m_uiNumPoses);
    m_pkPoses = NiAlloc(NxMat34, m_uiNumPoses);
    for (unsigned int ui = 0; ui < m_uiNumPoses; ui++)
    {
        NiPhysXTypes::NxMat34LoadBinary(kStream, m_pkPoses[ui]);
    }
    
    m_spBodyDesc = (NiPhysXBodyDesc*)kStream.ResolveLinkID();

    NiStreamLoadBinary(kStream, m_fDensity);
    NiStreamLoadBinary(kStream, m_uiActorFlags);
    NiStreamLoadBinary(kStream, m_uiActorGroup);
    if (kStream.GetFileVersion() > NiStream::GetVersion(20, 3, 0, 0) &&
        kStream.GetFileVersion() < NiStream::GetVersion(20, 3, 0, 6))
    {
        NxU32 uiDummy;
        NiStreamLoadBinary(kStream, uiDummy);
    }

    unsigned int uiShapeSize;
    NiStreamLoadBinary(kStream, uiShapeSize);
    m_kActorShapes.SetSize(uiShapeSize);
    for (unsigned int i = 0; i < uiShapeSize; i++)
    {
        m_kActorShapes.SetAt(i, (NiPhysXShapeDesc*)kStream.ResolveLinkID());
    }

    m_pkActorParent = (NiPhysXActorDesc*)kStream.ResolveLinkID();
    m_spSource = (NiPhysXRigidBodySrc*)kStream.ResolveLinkID();
    m_spDest = (NiPhysXRigidBodyDest*)kStream.ResolveLinkID();
}
//---------------------------------------------------------------------------
void NiPhysXActorDesc::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXActorDesc::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXActorDesc, pkObject));
    if(!NiObject::IsEqual(pkObject))
        return false;
        
    NiPhysXActorDesc* pkNewObject = (NiPhysXActorDesc*)pkObject;
    
    if (m_kActorName != pkNewObject->m_kActorName)
        return false;
    
    if (m_uiNumPoses != pkNewObject->m_uiNumPoses)
        return false;
        
    NiMatrix3 kNewM;
    NiPoint3 kNewP;
    NiMatrix3 kThisM;
    NiPoint3 kThisP;
    for (unsigned int ui = 0; ui < m_uiNumPoses; ui++)
    {
        NxVec3 kDiffT = pkNewObject->m_pkPoses[ui].t - m_pkPoses[ui].t;
        if (kDiffT.magnitudeSquared() > 1.0e-8f)
            return false;
        NxMat33 kDiffM = pkNewObject->m_pkPoses[ui].M - m_pkPoses[ui].M;
        if (NiAbs(kDiffM.determinant() > 0.0001f))
            return false;
    }
    
    if ((m_spBodyDesc && !pkNewObject->m_spBodyDesc) ||
        (!m_spBodyDesc && pkNewObject->m_spBodyDesc))
        return false;
        
    if (m_spBodyDesc && (!m_spBodyDesc->IsEqual(pkNewObject->m_spBodyDesc)))
        return false;

    if (pkNewObject->m_fDensity != m_fDensity  ||
        pkNewObject->m_uiActorFlags != m_uiActorFlags ||
        pkNewObject->m_uiActorGroup != m_uiActorGroup)
        return false;

    if (m_kActorShapes.GetSize() != pkNewObject->m_kActorShapes.GetSize())
        return false;
        
    for (unsigned int i = 0; i < m_kActorShapes.GetSize(); i++)
    {
        NiPhysXShapeDescPtr spShape = m_kActorShapes.GetAt(i);
        NiPhysXShapeDescPtr spNewShape = pkNewObject->m_kActorShapes.GetAt(i);
        if ((spShape && !spNewShape) || (!spShape && spNewShape))
            return false;
        
        if (spShape && (!spShape->IsEqual(spNewShape)))
            return false;
    }

    if (m_pkActorParent)
    {
        if (!pkNewObject->m_pkActorParent ||
            !m_pkActorParent->IsEqual(pkNewObject->m_pkActorParent))
        return false;
    }
    else
    {
        if (pkNewObject->m_pkActorParent)
            return false;
    }
    
    if (m_spSource)
    {
        if (!pkNewObject->m_spSource ||
            !m_spSource->IsEqual(pkNewObject->m_spSource))
        return false;
    }
    else
    {
        if (pkNewObject->m_spSource)
            return false;
    }
    
    if (m_spDest)
    {
        if (!pkNewObject->m_spDest ||
            !m_spDest->IsEqual(pkNewObject->m_spDest))
        return false;
    }
    else
    {
        if (pkNewObject->m_spDest)
            return false;
    }
    
    return true;
}
//---------------------------------------------------------------------------

