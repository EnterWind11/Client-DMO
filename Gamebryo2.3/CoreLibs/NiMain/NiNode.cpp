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
#include "NiMainPCH.h"

#include "NiAlphaProperty.h"
#include "NiAVObject.h"
#include "NiBool.h"
#include "NiCloningProcess.h"
#include "NiLight.h"
#include "NiMainMetrics.h"
#include "NiNode.h"
#include "NiTextureEffect.h"
#include "NiTriBasedGeom.h"
#include "NiTimeController.h"
#include "NiShadowGenerator.h"
#include "NiTNodeTraversal.h" // must be included in some NiMain file, so here.
#include <NiRTLib.h>

NiImplementRTTI(NiNode, NiAVObject);

//---------------------------------------------------------------------------
// construction and destruction
//---------------------------------------------------------------------------
NiNode::NiNode(unsigned int uiNumChildren) : m_kChildren(uiNumChildren)
{
    SetNodeBit();
}
//---------------------------------------------------------------------------
NiNode::~NiNode()
{
    DetachAllEffects();

    for (unsigned int i = 0; i < m_kChildren.GetSize(); i++)
    {
        DetachChildAt(i);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// parent and child manipulation
//---------------------------------------------------------------------------
void NiNode::AttachChild(NiAVObject* pkChild, bool bFirstAvail)
{
    NIASSERT(pkChild);   // for debug mode
    if (pkChild == 0)  // for release mode
        return;

    // Need to hold onto child in case AttachParent winds up replacing the
    // old parent and the only reference to the child was that parent.  This
    // avoids AttachParent deleting pkChild due to zero ref count.
    pkChild->IncRefCount();

    pkChild->AttachParent(this);

    if (bFirstAvail)
        m_kChildren.AddFirstEmpty(pkChild);
    else
        m_kChildren.Add(pkChild);

    NIASSERT(pkChild->GetRefCount() >= 2);
    pkChild->DecRefCount();
}
//---------------------------------------------------------------------------
NiAVObjectPtr NiNode::DetachChildAt(unsigned int i)
{
    if (i < m_kChildren.GetSize())
    {
        NiAVObjectPtr spChild = m_kChildren.GetAt(i);
        if (spChild)
        {
            spChild->DetachParent();
            m_kChildren.RemoveAt(i);
        }
        return spChild;
    }
    else
    {
        return 0;
    }
}
//---------------------------------------------------------------------------
NiAVObjectPtr NiNode::DetachChild(NiAVObject* pkChild)
{
    for (unsigned int i = 0; i < m_kChildren.GetSize(); i++)
    {
        NiAVObjectPtr spChild = m_kChildren.GetAt(i);
        if (spChild && spChild == pkChild)
        {
            spChild->DetachParent();
            m_kChildren.RemoveAt(i);
            return spChild;
        }
    }

    return 0;
}
//---------------------------------------------------------------------------
NiAVObjectPtr NiNode::SetAt(unsigned int i, NiAVObject* pkChild)
{
    if (m_kChildren.GetSize() <= i)
    {
        if (pkChild)
            pkChild->AttachParent(this);
     
        m_kChildren.SetAtGrow(i, pkChild);
        return 0;
    }
    else
    {
        NiAVObjectPtr spFormerChild = m_kChildren.GetAt(i);
        if (spFormerChild)
            spFormerChild->DetachParent();
        
        if (pkChild)
            pkChild->AttachParent(this);

        m_kChildren.SetAt(i, pkChild);
        return spFormerChild;
    }
}
//---------------------------------------------------------------------------
bool NiNode::SameTopology(NiAVObject* pkObj0, NiAVObject* pkObj1)
{
    if (pkObj0 == pkObj1)
        return true;

    if (!pkObj0->IsExactKindOf(pkObj1->GetRTTI()))
        return false;

    NiNode* pkNode0 = NiDynamicCast(NiNode, pkObj0);
    if (pkNode0)
    {
        NiNode* pkNode1 = (NiNode*) pkObj1;
        unsigned int uiCount0 = pkNode0->GetArrayCount();
        unsigned int uiCount1 = pkNode1->GetArrayCount();
        if (uiCount0 != uiCount1)
            return false;

        for (unsigned int uiI = 0; uiI < uiCount0; uiI++)
        {
            NiAVObject* pkChild0 = pkNode0->GetAt(uiI);
            NiAVObject* pkChild1 = pkNode1->GetAt(uiI);
            if ((pkChild0 && !pkChild1) || (!pkChild0 && pkChild1))
                return false;

            if (pkChild0)
            {
                if (!SameTopology(pkChild0, pkChild1))
                    return false;
            }
        }
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// material updates
//---------------------------------------------------------------------------
void NiNode::SetMaterialNeedsUpdate(bool bNeedsUpdate)
{
    for (unsigned int i = 0; i < m_kChildren.GetSize(); i++)
    {
        NiAVObject* pkChild = m_kChildren.GetAt(i);
        if (pkChild)
            pkChild->SetMaterialNeedsUpdate(bNeedsUpdate);
    }
}
//---------------------------------------------------------------------------
void NiNode::SetDefaultMaterialNeedsUpdateFlag(bool bFlag)
{
    for (unsigned int i = 0; i < m_kChildren.GetSize(); i++)
    {
        NiAVObject* pkChild = m_kChildren.GetAt(i);
        if (pkChild)
            pkChild->SetDefaultMaterialNeedsUpdateFlag(bFlag);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// update methods
//---------------------------------------------------------------------------
void NiNode::ApplyTransform(const NiMatrix3& kMat, const NiPoint3& kTrn, 
    bool bOnLeft)
{
    // Allows for a general homogeneous transformation to be applied to a
    // scene subtree.  This creates a technical problem in that nodes only
    // store rotation, uniform scale, and translation.  If the general
    // transform has nonuniform scales or shearing, this information must be
    // propagated down to the leaf geometry to let those objects decide what
    // to do with the transform.  In the process, nodes must retain their
    // rotation, uniform scale, and translation information.  The idea is as
    // follows.  Let <M0, T0> be the general transform (my ASCII notation for
    // a homogeneous transformation).  Let <s*R, T> be the transform for
    // the given node.  An application may want to apply <M0, T0> before or
    // after <s*R, T> is applied.  The flag bOnLeft set to true says to
    // multiply <M0, T0>*<s*R, T>.  If false, <M0, T0> may be directly
    // applied to the children of the node.  In the case of left multiply, 
    // we want to determine matrix kM1 and translation kT1 such that
    //
    //     <M0, T0>*<s*R, T> = <s*R, T>*<kM1, kT1>
    //
    // The node retains its same scale, rotation, and translation and the
    // matrix <kM1, kT1> is applied recursively to the children of the node.
    // The matrix equation can be solved as
    //
    //     <kM1, kT1> = <s*R, T>^{-1} <M0, T0> <s*R, T>
    //             = <R^t*M0*R, (1/s)*R^t*(M0*T+T0-T)>

    unsigned int i;
    NiAVObject* pkChild;

    if (bOnLeft)
    {
        NiMatrix3 kM1 = m_kLocal.m_Rotate.TransposeTimes(kMat
            * m_kLocal.m_Rotate);
        NiPoint3 kT1 = ((kMat*m_kLocal.m_Translate+kTrn-m_kLocal.m_Translate) 
            * m_kLocal.m_Rotate)/m_kLocal.m_fScale;

        for (i = 0; i < m_kChildren.GetSize(); i++)
        {
            pkChild = m_kChildren.GetAt(i);
            if (pkChild)
                pkChild->ApplyTransform(kM1, kT1, true);
        }
    }
    else
    {
        for (i = 0; i < m_kChildren.GetSize(); i++)
        {
            pkChild = m_kChildren.GetAt(i);
            if (pkChild)
                pkChild->ApplyTransform(kMat, kTrn, true);
        }
    }
}
//---------------------------------------------------------------------------
void NiNode::UpdateDownwardPass(float fTime, bool bUpdateControllers)
{
    // NOTE: When changing UpdateDownwardPass, UpdateSelectedDownwardPass,
    // or UpdateRigidDownwardPass, remember to make equivalent changes
    // the all of these functions.

    NIMETRICS_MAIN_INCREMENTUPDATES();

    if (bUpdateControllers)
        UpdateObjectControllers(fTime);

    UpdateWorldData();

    m_kWorldBound.SetRadius(0.0f);

    // To avoid having to call UpdateWorldBound and therefore making another
    // iteration through the node's children, the world bound is calculated
    // during this loop.
    for (unsigned int i = 0; i < m_kChildren.GetSize(); i++)
    {
        NiAVObject* pkChild = m_kChildren.GetAt(i);
        if (pkChild)
        {
            pkChild->UpdateDownwardPass(fTime, bUpdateControllers);

            if (pkChild->IsVisualObject())
            {
                if (m_kWorldBound.GetRadius() == 0.0f)
                {
                    m_kWorldBound = pkChild->GetWorldBound();
                }
                else
                {
                    m_kWorldBound.Merge(&pkChild->GetWorldBound());
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiNode::UpdateSelectedDownwardPass(float fTime)
{
    // NOTE: When changing UpdateDownwardPass, UpdateSelectedDownwardPass,
    // or UpdateRigidDownwardPass, remember to make equivalent changes
    // the all of these functions.

    NIMETRICS_MAIN_INCREMENTUPDATES();

    UpdateObjectControllers(fTime, GetSelectiveUpdatePropertyControllers());

    if (GetSelectiveUpdateTransforms())
        UpdateWorldData();

    m_kWorldBound.SetRadius(0.0f);

    // To avoid having to call UpdateWorldBound and therefore making another
    // iteration through the node's children, the world bound is calculated
    // during this loop.
    for (unsigned int i = 0; i < m_kChildren.GetSize(); i++)
    {
        NiAVObject* pkChild = m_kChildren.GetAt(i);
        if (pkChild)
        {
            pkChild->DoSelectedUpdate(fTime);

            if (pkChild->IsVisualObject())
            {
                if (m_kWorldBound.GetRadius() == 0.0f)
                {
                    m_kWorldBound = pkChild->GetWorldBound();
                }
                else
                {
                    m_kWorldBound.Merge(&pkChild->GetWorldBound());
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiNode::UpdateRigidDownwardPass(float fTime)
{
    // NOTE: When changing UpdateDownwardPass, UpdateSelectedDownwardPass,
    // or UpdateRigidDownwardPass, remember to make equivalent changes
    // the all of these functions.

    NIMETRICS_MAIN_INCREMENTUPDATES();

    UpdateObjectControllers(fTime, GetSelectiveUpdatePropertyControllers());

    if (GetSelectiveUpdateTransforms())
    {
        UpdateWorldData();
        m_kWorldBound.Update(m_kBound, m_kWorld);
    }

    for (unsigned int i = 0; i < m_kChildren.GetSize(); i++)
    {
        NiAVObject* pkChild = m_kChildren.GetAt(i);
        if (pkChild)
        {
            if (pkChild->GetSelectiveUpdate())
                pkChild->UpdateRigidDownwardPass(fTime);
        }
    }
}
//---------------------------------------------------------------------------
void NiNode::UpdateControllers(float fTime)
{
    UpdateObjectControllers(fTime);

    // Call UpdateControllers on all the children of the node.
    for (unsigned int ui = 0; ui < m_kChildren.GetSize(); ui++)
    {
        NiAVObject* pkChild = m_kChildren.GetAt(ui);
        if (pkChild)
            pkChild->UpdateControllers(fTime);
    }
}
//---------------------------------------------------------------------------
void NiNode::UpdateUpwardPass()
{
    NiNode* pParent = GetParent();

    UpdateWorldBound();
    if (pParent)
        pParent->UpdateUpwardPass();
}
//---------------------------------------------------------------------------
void NiNode::UpdateWorldBound()
{
    // This function might be called by derived class implementations of 
    // UpdateDownwardPass, but NiNode::UpdateDownwardPass does not call it.

    m_kWorldBound.SetRadius(0.0f);

    for (unsigned int i = 0; i < m_kChildren.GetSize(); i++)
    {
        NiAVObject* pkChild = m_kChildren.GetAt(i);
        if (pkChild && pkChild->IsVisualObject())
        {
            if (m_kWorldBound.GetRadius() == 0.0f)
            {
                m_kWorldBound = pkChild->GetWorldBound();
            }
            else
            {
                m_kWorldBound.Merge(&pkChild->GetWorldBound());
            }
        }
    }
}
//---------------------------------------------------------------------------
NiPropertyStatePtr NiNode::UpdatePropertiesUpward()
{
    NiPropertyStatePtr spParentState;
    
    // Avoid use of ?: operator here to avoid ee-gcc compile error.
    if (m_pkParent)
        spParentState = m_pkParent->UpdatePropertiesUpward();
    else
        spParentState = NiNew NiPropertyState;

    return PushLocalProperties(spParentState, false);
}
//---------------------------------------------------------------------------
NiDynamicEffectStatePtr NiNode::UpdateEffectsUpward()
{
    NiDynamicEffectStatePtr spParentState = NULL;
    if (m_pkParent) 
        spParentState = m_pkParent->UpdateEffectsUpward();

    return PushLocalEffects(spParentState, false);
}
//---------------------------------------------------------------------------
NiDynamicEffectStatePtr NiNode::PushLocalEffects(
    NiDynamicEffectState* pkParentState, bool bCopyOnChange)
{
    NiDynamicEffectStatePtr spState = pkParentState;

    bool bCopiedAlready = false;
    
    if (pkParentState)
    {
        NiDynEffectStateIter kPos = pkParentState->GetLightHeadPos();
        while (kPos)
        {
            NiDynamicEffect* pkLight = pkParentState->GetNextLight(kPos);
            if (pkLight)
            {
                if (pkLight->IsUnaffectedNode(this))
                {
                    if(!bCopiedAlready)
                    {
                        spState = pkParentState->Copy();
                        bCopiedAlready = true;
                    }
                    spState->RemoveEffect(pkLight);
                }

                if (pkLight->GetShadowGenerator())
                {
                    pkLight->GetShadowGenerator()->SetRenderViewsDirty(true);
                }

            }
            
        }

        kPos = pkParentState->GetProjLightHeadPos();
        while (kPos)
        {
            NiDynamicEffect* pkProjLight = 
                pkParentState->GetNextProjLight(kPos);
            if (pkProjLight && pkProjLight->IsUnaffectedNode(this))
            {
                if(!bCopiedAlready)
                {
                    spState = pkParentState->Copy();
                    bCopiedAlready = true;
                }
                spState->RemoveEffect(pkProjLight);
            }
        }

        kPos = pkParentState->GetProjShadowHeadPos();
        while (kPos)
        {
            NiDynamicEffect* pkProjShadow =
                pkParentState->GetNextProjShadow(kPos);
            if (pkProjShadow && pkProjShadow->IsUnaffectedNode(this))
            {
                if(!bCopiedAlready)
                {
                    spState = pkParentState->Copy();
                    bCopiedAlready = true;
                }
                spState->RemoveEffect(pkProjShadow);
            }
        }

        NiDynamicEffect* pkEnvMap = pkParentState->GetEnvironmentMap();
        NiDynamicEffect* pkFogMap = pkParentState->GetFogMap();
    
        if (pkEnvMap && pkEnvMap->IsUnaffectedNode(this))
        {
            if(!bCopiedAlready)
            {
                spState = pkParentState->Copy();
                bCopiedAlready = true;
            }
            spState->RemoveEffect(pkEnvMap);
        }

        if (pkFogMap && pkFogMap->IsUnaffectedNode(this))
        {
            if(!bCopiedAlready)
            {
                spState = pkParentState->Copy();
                bCopiedAlready = true;
            }
            spState->RemoveEffect(pkFogMap);
        }
    }

    if(GetEffectList().IsEmpty())
    {
        return spState;
    }
    else
    {
        
        if (!pkParentState)
        {
            spState = NiNew NiDynamicEffectState;
        }
        else
        {
            // because there are local effects, we must copt the state before
            // pushing the new properties if the "copy on change" flag is set.
            // otherwise, we are free to change the input state
            if(bCopyOnChange && !bCopiedAlready)
                spState = pkParentState->Copy();
            else if (!bCopyOnChange && !bCopiedAlready)
                spState = pkParentState;
        }

        NiTListIterator kPos = m_kEffectList.GetHeadPos();
        while (kPos)
        {
            spState->AddEffect(m_kEffectList.GetNext(kPos));
        }

        return spState;
    }   
}
//---------------------------------------------------------------------------
void NiNode::UpdatePropertiesDownward(NiPropertyState* pkParentState)
{
    NiPropertyStatePtr spState 
        = PushLocalProperties(pkParentState, true);
    
    for (unsigned int i = 0; i < m_kChildren.GetSize(); i++)
    {
        NiAVObject* pkChild = m_kChildren.GetAt(i);
        if (pkChild)
            pkChild->UpdatePropertiesDownward (spState);
    }
}
//---------------------------------------------------------------------------
void NiNode::UpdateEffectsDownward(NiDynamicEffectState* pkParentState)
{
    NiDynamicEffectStatePtr spState 
        = PushLocalEffects(pkParentState, true);
    
    for (unsigned int i = 0; i < m_kChildren.GetSize(); i++)
    {
        NiAVObject* pkChild = m_kChildren.GetAt(i);
        if (pkChild)
            pkChild->UpdateEffectsDownward (spState);
    }
}
//---------------------------------------------------------------------------
void NiNode::UpdateNodeBound()
{
    // Recursively sets m_kBound for all nodes in the subtree

    for (unsigned int i = 0; i < m_kChildren.GetSize(); i++)
    {
        NiAVObject* pkChild = m_kChildren.GetAt(i);
        if (pkChild)
        {
            pkChild->UpdateNodeBound();
        }
    }

    NiTransform kWorldInverse;
    m_kWorld.Invert(kWorldInverse);
    m_kBound.Update(m_kWorldBound, kWorldInverse);
}
//---------------------------------------------------------------------------
void NiNode::SetSelectiveUpdateFlags(bool& bSelectiveUpdate, 
    bool bSelectiveUpdateTransforms, bool& bRigid)
{
    // Sets the SelectiveUpdate flag for each object in the tree
    // by checking to see if the object is animated or has any animated
    // decendents.

    // Sets the SelectiveUpdateTransforms flag if the object has any
    // ancestors with animated transforms.

    // Sets the SelectiveUpdatePropertyControllers flag if the object has
    // any animated property controllers.

    // Sets the SelectiveUpdateRigid flag to false if the object has
    // any decendents with animated transforms or NiSkinInstances;
    // true otherwise.

    // Returns what the SelectiveUpdate flag was set to for pkObject

    // first test for SelectiveUpdateTransforms since this must be 
    // propagated down the tree

    bool bAnimatedTransform = HasTransformController();
    if (!bSelectiveUpdateTransforms)
    {
        // check for transform controllers to set SelectiveUpdateTransforms 
        // flag
        bSelectiveUpdateTransforms = bAnimatedTransform ||
            GetSelectiveUpdateTransformsOverride();
    }

    // recurse depth first over children
    bSelectiveUpdate = bSelectiveUpdateTransforms;
    bRigid = true;

    for (unsigned int ui = 0; ui < GetArrayCount(); ui++)
    {
        NiAVObject* pkChild = GetAt(ui);
        if (pkChild)
        {
            bool bSelectiveUpdateTemp = false;
            bool bRigidTemp = true;

            pkChild->SetSelectiveUpdateFlags(bSelectiveUpdateTemp, 
                bSelectiveUpdateTransforms, bRigidTemp);

            if (bSelectiveUpdateTemp)
                bSelectiveUpdate = true;
            if (!bRigidTemp)
                bRigid = false;
        }
    }

    // check for any controllers on the properties
    bool bSelectiveUpdatePropertyControllers = HasPropertyController();
    
    // this node needs updating
    if (bSelectiveUpdatePropertyControllers || GetControllers() != NULL)
        bSelectiveUpdate = true;

    SetSelectiveUpdate(bSelectiveUpdate);
    SetSelectiveUpdateTransforms(bSelectiveUpdateTransforms);
    SetSelectiveUpdatePropertyControllers(
        bSelectiveUpdatePropertyControllers);
    SetSelectiveUpdateRigid(bRigid);
    
    if (bAnimatedTransform)
        bRigid = false;

}
//---------------------------------------------------------------------------
void NiNode::OnVisible(NiCullingProcess& kCuller)
{
    // skip the object if it isn't visual
    if (IsVisualObject())
    {
        for (unsigned int i = 0; i < m_kChildren.GetSize(); i++)
        {
            NiAVObject* pkChild = m_kChildren.GetAt(i);
            if (pkChild)
                pkChild->Cull(kCuller);
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// renderer data
//---------------------------------------------------------------------------
void NiNode::PurgeRendererData(NiRenderer* pkRenderer)
{
    NiAVObject::PurgeRendererData(pkRenderer);

    NiTListIterator kEffectIter = m_kEffectList.GetHeadPos();
    while (kEffectIter)
    {
        NiDynamicEffect* pkEffect = m_kEffectList.GetNext(kEffectIter);
        pkRenderer->PurgeEffect(pkEffect);
    }

    for (unsigned int i = 0; i < m_kChildren.GetSize(); i++)
    {
        NiAVObject* pkChild = m_kChildren.GetAt(i);
        if (pkChild)
            pkChild->PurgeRendererData(pkRenderer);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// lights
//---------------------------------------------------------------------------
void NiNode::AttachEffect(NiDynamicEffect* pkEffect)
{
    // ensure that we don't recurse forever
    if (m_kEffectList.FindPos(pkEffect))
        return;

    m_kEffectList.AddHead(pkEffect);
    pkEffect->AttachAffectedNode(this);
}
//---------------------------------------------------------------------------
void NiNode::DetachEffect(NiDynamicEffect* pkEffect)
{
    // ensure that we don't recurse forever
    if (!m_kEffectList.FindPos(pkEffect))
        return;

    m_kEffectList.Remove(pkEffect);
    pkEffect->DetachAffectedNode(this);
}
//---------------------------------------------------------------------------
void NiNode::DetachAllEffects()
{
    while (!m_kEffectList.IsEmpty())
    {
        NiDynamicEffect *pkEffect = m_kEffectList.GetHead();
        m_kEffectList.RemoveHead();
        pkEffect->DetachAffectedNode(this);
    }
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// name search
//---------------------------------------------------------------------------

NiAVObject* NiNode::GetObjectByName(const NiFixedString& kName)
{
    NiAVObject* pkObject = NiAVObject::GetObjectByName(kName);
    if (pkObject)
        return pkObject;

    for (unsigned int i = 0; i < m_kChildren.GetSize(); i++)
    {
        NiAVObject* pkChild = GetAt(i);
        if (pkChild)
        {
            pkObject = pkChild->GetObjectByName(kName);
            if (pkObject)
                return pkObject;
        }
    }

    return 0;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiNode);
//---------------------------------------------------------------------------
void NiNode::CopyMembers(NiNode* pkDest,
    NiCloningProcess& kCloning)
{
    NiAVObject::CopyMembers(pkDest, kCloning);

    for (unsigned int i = 0; i < m_kChildren.GetSize(); i++)
    {
        NiAVObject* pkChild = m_kChildren.GetAt(i);
        if (pkChild)
        {
            NiAVObject* pkClone = (NiAVObject*) pkChild->CreateClone(
                kCloning);
            NIASSERT(pkClone);
            pkDest->SetAt(i, pkClone);
        }
    }

    // The effect list m_kEffectList is not processed in this function.  The 
    // functions below (ProcessClone and CopyEffectListClones) have the 
    // responsibility of cloning the light/node relationships.
}
//---------------------------------------------------------------------------
void NiNode::ProcessClone(NiCloningProcess& kCloning)
{
    NiAVObject::ProcessClone(kCloning);

    NiObject* pkClone = NULL;
#ifdef _DEBUG
    bool bCloned =
#endif 
    kCloning.m_pkCloneMap->GetAt(this, pkClone);
    // If this assert is hit because the RTTI doesn't match, then something
    // is attempting to clone a type of object that does not implement
    // cloning (and thus cannot be cloned), such as a portal system.
    NIASSERT(bCloned && GetRTTI() == pkClone->GetRTTI());

    NiNode* pkNode = (NiNode*) pkClone;

    const NiDynamicEffectList *pkList = &GetEffectList();

    if (!pkList->IsEmpty())
        pkNode->CopyEffectListClones(pkList, kCloning);

    for (unsigned int i = 0; i < m_kChildren.GetSize(); i++)
    {
        NiAVObject *pkChild;
        
        pkChild = m_kChildren.GetAt(i);
        if (pkChild != NULL)
            pkChild->ProcessClone(kCloning);
    }
}
//---------------------------------------------------------------------------
void NiNode::CopyEffectListClones(const NiDynamicEffectList* pkList,
    NiCloningProcess& kCloning)
{
    if (kCloning.m_eAffectedNodeRelationBehavior !=
        NiCloningProcess::CLONE_RELATION_NONE)
    {
        // Iterate in reverse order so cloned lists will be in the same order.

        // Process dynamic effects.
        NiTListIterator kIter = pkList->GetTailPos();
        while (kIter)
        {
            NiDynamicEffect* pkEffect = pkList->GetPrev(kIter);
            NiObject* pkClone;
            if (kCloning.m_pkCloneMap->GetAt(pkEffect, pkClone))
            {
                pkEffect = (NiDynamicEffect*) pkClone;
            }
            else if (kCloning.m_eAffectedNodeRelationBehavior ==
                NiCloningProcess::CLONE_RELATION_CLONEDONLY)
            {
                pkEffect = NULL;
            }

            if (pkEffect)
            {
                AttachEffect(pkEffect);
            }
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiNode);
//---------------------------------------------------------------------------
void NiNode::LoadBinary(NiStream& kStream)
{
    NiAVObject::LoadBinary(kStream);

    // Force the node bit on after streaming in the object.
    SetNodeBit();

    // read children pointers
    kStream.ReadMultipleLinkIDs();  // m_kChildren

    // read effect pointers
    kStream.ReadMultipleLinkIDs();  // m_kEffectList
}
//---------------------------------------------------------------------------
void NiNode::LinkObject(NiStream& kStream)
{
    NiAVObject::LinkObject(kStream);

    // link children
    unsigned int uiSize = kStream.GetNumberOfLinkIDs();
    if (uiSize)
    {
        m_kChildren.SetSize(uiSize);
        for (unsigned int i = 0; i < uiSize; i++)
        {
            NiAVObject* pkChild = 
                (NiAVObject*) kStream.GetObjectFromLinkID();

            SetAt(i, pkChild);
        }
    }

    // link lights in reverse order (for IsEqual to work properly)
    uiSize = kStream.GetNumberOfLinkIDs();
    while (uiSize--)
    {
        NiDynamicEffect* pkEffect = 
            (NiDynamicEffect*) kStream.GetObjectFromLinkID();
        if (pkEffect)
        {
            m_kEffectList.AddHead(pkEffect);
            pkEffect->AttachAffectedNode(this);
        }
    }
}
//---------------------------------------------------------------------------
bool NiNode::RegisterStreamables(NiStream& kStream)
{
    if (!NiAVObject::RegisterStreamables(kStream))
    {
        return false;
    }

    // register children
    for (unsigned int i = 0; i < m_kChildren.GetSize(); i++)
    {
        NiAVObject* pkChild = GetAt(i);
        if (pkChild)
            pkChild->RegisterStreamables(kStream);
    }

    // register lights
    NiTListIterator kPos = m_kEffectList.GetHeadPos();
    while (kPos)
    {
        NiDynamicEffect* pkEffect = m_kEffectList.GetNext(kPos);
        if (pkEffect)
            pkEffect->RegisterStreamables(kStream);
    }

    return true;
}
//---------------------------------------------------------------------------
void NiNode::SaveBinary(NiStream& kStream)
{
    NiAVObject::SaveBinary(kStream);

    // save children
    unsigned int uiChildrenSize = m_kChildren.GetSize();
    NiStreamSaveBinary(kStream, uiChildrenSize);
    for (unsigned int i = 0; i < uiChildrenSize; i++)
    {
        NiAVObject* pkChild = GetAt(i);
        kStream.SaveLinkID(pkChild);
    }

    // save lights
    int iListSize = m_kEffectList.GetSize();
    NiStreamSaveBinary(kStream, iListSize);
    if (iListSize > 0)
    {
        // save in reverse order because lights will link faster that way
        NiTListIterator kPos = m_kEffectList.GetTailPos();
        while (kPos)
        {
            kStream.SaveLinkID(m_kEffectList.GetPrev(kPos));
        }
    }
}
//---------------------------------------------------------------------------
bool NiNode::IsEqual(NiObject* pkObject)
{
    if (!NiAVObject::IsEqual(pkObject))
        return false;

    NiNode* pkNode = (NiNode*) pkObject;

    // children
    unsigned int uiCount0 = m_kChildren.GetSize();
    unsigned int uiCount1 = pkNode->m_kChildren.GetSize();
    if (uiCount0 != uiCount1)
        return false;

    for (unsigned int i = 0; i < uiCount0; i++)
    {
        NiAVObject* pkChild0 = GetAt(i);
        NiAVObject* pkChild1 = pkNode->GetAt(i);
        if ((pkChild0 && !pkChild1) || (!pkChild0 && pkChild1))
            return false;

        if (pkChild0 && !pkChild0->IsEqual(pkChild1))
            return false;
    }

    // lights
    uiCount0 = m_kEffectList.GetSize();
    uiCount1 = pkNode->m_kEffectList.GetSize();
    if (uiCount0 != uiCount1)
        return false;

    if (uiCount0 > 0)
    {
        NiTListIterator kPos0 = m_kEffectList.GetHeadPos();
        NiTListIterator kPos1 = pkNode->m_kEffectList.GetHeadPos();
        while (kPos0)
        {
            NiDynamicEffect* pkEffect0 = m_kEffectList.GetNext(kPos0);
            NiDynamicEffect* pkEffect1 = 
                pkNode->m_kEffectList.GetNext(kPos1);
            if ((pkEffect0 && !pkEffect1) || (!pkEffect0 && pkEffect1))
                return false;

            if (pkEffect0 && !pkEffect0->IsEqual(pkEffect1))
                return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiNode::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiAVObject::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiNode::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("m_bVisual", IsVisualObject()));

    if (!m_kEffectList.IsEmpty())
    {
        int i = 0;
        char acPrefix[64];
        NiTListIterator kPos = m_kEffectList.GetHeadPos();
        while (kPos)
        {
            NiDynamicEffect* pkEffect = m_kEffectList.GetNext(kPos);
            NiSprintf(acPrefix, 64, "effect[%d]", i);
            pkStrings->Add(NiGetViewerString(acPrefix, pkEffect));
            i++;
        }
    }
}
//---------------------------------------------------------------------------
void NiNode::_UpdateSelect( float fTime, bool bUpdate )
{
	if( !bUpdate )
		return;

	UpdateSelected( fTime );
}
//---------------------------------------------------------------------------
