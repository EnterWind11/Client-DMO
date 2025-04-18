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


#include "NiCollisionPCH.h" // Precompiled header.

#include <NiSwitchNode.h>
#include <NiTriBasedGeom.h>
#include "NiCollisionMetrics.h"
#include "NiCollisionTraversals.h"
#include "NiCollisionUtils.h"
#include "NiPick.h"
#include "NiSphereBV.h"
#include "NiTriShape.h"
#include "NiTriStrips.h"

#if defined (_PS3) && defined(ENABLE_PARALLEL_PICK)
    #include "NiCollisionWorkflow.h"
    #include "NiStreamProcessor.h"
#endif

namespace NiCollisionTraversals
{

//---------------------------------------------------------------------------
// Collision detection scene graph traversal routines.
//---------------------------------------------------------------------------
bool CheckForCollisionData(NiAVObject* pkRoot, bool bRecursive)
{
    NiCollisionData* pkData = NiGetCollisionData(pkRoot);

    if (pkData)
        return true;

    if (bRecursive == false)
        return false;

    if (pkRoot && pkRoot->IsNode())
    {
        NiNode* pkNode = (NiNode*)pkRoot;
        const unsigned int uiSize = pkNode->GetArrayCount();

        for (unsigned int uiI = 0; uiI < uiSize; uiI++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiI);
            if (pkChild)
            {
                bool bResult = CheckForCollisionData(pkChild, bRecursive);

                if (bResult)
                    return true;
            }
        }
    }   

    return false;
}
//---------------------------------------------------------------------------
void CreateCollisionData(NiAVObject* pkRoot,
    NiCollisionData::CollisionMode eCollision)
{
    NiCollisionData* pkData = NiGetCollisionData(pkRoot);

    // Create data only for NiTriBasedGeom.
    if (NiIsKindOf(NiTriBasedGeom, pkRoot))
    {
        if (!pkData)
        {
            pkData = NiNew NiCollisionData(pkRoot);
            NIASSERT(pkData);
            pkData->SetPropagationMode(NiCollisionData::PROPAGATE_NEVER);
            pkData->SetCollisionMode(eCollision);
            pkRoot->SetCollisionObject(pkData);
        }
    }

    if (pkRoot && pkRoot->IsNode())
    {
        NiNode* pkNode = (NiNode*)pkRoot;
        const unsigned int uiSize = pkNode->GetArrayCount();

        for (unsigned int uiI = 0; uiI < uiSize; uiI++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiI);
            if (pkChild)
                CreateCollisionData(pkChild, eCollision);
        }
    }
}
//---------------------------------------------------------------------------
void ClearWorldVelocities(NiAVObject* pkRoot, NiPoint3& kValue)
{
    NiCollisionData* pkData = NiGetCollisionData(pkRoot);

    if (pkData)
    {
        pkData->SetLocalVelocity(kValue);
        pkData->SetWorldVelocity(kValue);
    }

    if (pkRoot && pkRoot->IsNode())
    {
        NiNode* pkNode = (NiNode*)pkRoot;
        const unsigned int uiSize = pkNode->GetArrayCount();

        for (unsigned int uiI = 0; uiI < uiSize; uiI++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiI);
            if (pkChild)
                ClearWorldVelocities(pkChild, kValue);
        }
    }
}
//---------------------------------------------------------------------------
// World vertices and normals.
//---------------------------------------------------------------------------
void CreateWorldVertices(NiAVObject* pkObj)
{
    if (pkObj == NULL)
        return; 

    NiCollisionData* pkColData = NiGetCollisionData(pkObj);

    if (pkColData)
    {
        pkColData->CreateWorldVertices();
    }
    else
    {
        // If this object is geometry, assign collision data so that the
        // vertices may be created.
        if (NiIsKindOf(NiGeometry, pkObj))
        {
            pkColData = NiNew NiCollisionData(pkObj);
            pkColData->CreateWorldVertices();
        }
    }

    if (pkObj && pkObj->IsNode())
    {
        NiNode* pkNode = (NiNode*)pkObj;
        for (unsigned int uiI = 0; uiI < pkNode->GetArrayCount(); uiI++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiI);
            if (pkChild)
                CreateWorldVertices(pkChild);
        }
    }
}
//---------------------------------------------------------------------------
void UpdateWorldVertices(NiAVObject* pkObj)
{
    if (pkObj == NULL)
        return;

    NiCollisionData* pkColData = NiGetCollisionData(pkObj);

    if (pkColData)
        pkColData->UpdateWorldVertices();

    if (pkObj && pkObj->IsNode())
    {
        NiNode* pkNode = (NiNode*)pkObj;
        for (unsigned int uiI = 0; uiI < pkNode->GetArrayCount(); uiI++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiI);
            if (pkChild)
                UpdateWorldVertices(pkChild);
        }
    }
}
//---------------------------------------------------------------------------
void DestroyWorldVertices(NiAVObject* pkObj)
{
    if (pkObj == NULL)
        return;

    NiCollisionData* pkColData = NiGetCollisionData(pkObj);

    if (pkColData)
        pkColData->DestroyWorldVertices();

    if (pkObj && pkObj->IsNode())
    {
        NiNode* pkNode = (NiNode*)pkObj;  
        for (unsigned int uiI = 0; uiI < pkNode->GetArrayCount(); uiI++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiI);
            if (pkChild)
                DestroyWorldVertices(pkChild);
        }
    }
}
//---------------------------------------------------------------------------
void CreateWorldNormals(NiAVObject* pkObj)
{
    if (pkObj == NULL)
        return;

    NiCollisionData* pkColData = NiGetCollisionData(pkObj);

    if (pkColData)
    {
        pkColData->CreateWorldNormals();
    }
    else
    {
        // If this object is geometry, assign collision data so that the 
        // vertices may be created.
        if (NiIsKindOf(NiGeometry,pkObj))
        {
            pkColData = NiNew NiCollisionData(pkObj);
            pkColData->CreateWorldNormals();
        }

    }

    if (pkObj && pkObj->IsNode())
    {
        NiNode* pkNode = (NiNode*)pkObj;
        for (unsigned int uiI = 0; uiI < pkNode->GetArrayCount(); uiI++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiI);
            if (pkChild)
                CreateWorldNormals(pkChild);
        }
    }
}
//---------------------------------------------------------------------------
void UpdateWorldNormals(NiAVObject* pkObj)
{
    if (pkObj == NULL)
        return;

    NiCollisionData* pkColData = NiGetCollisionData(pkObj);

    if (pkColData)
        pkColData->UpdateWorldNormals();

    if (pkObj && pkObj->IsNode())
    {
        NiNode* pkNode = (NiNode*)pkObj;
        for (unsigned int uiI = 0; uiI < pkNode->GetArrayCount(); uiI++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiI);
            if (pkChild)
                UpdateWorldNormals(pkChild);
        }
    }
}
//---------------------------------------------------------------------------
void DestroyWorldNormals(NiAVObject* pkObj)
{
    if (pkObj == NULL)
        return;

    NiCollisionData* pkColData = NiGetCollisionData(pkObj);

    if (pkColData)
        pkColData->DestroyWorldNormals();

    if (pkObj && pkObj->IsNode())
    {
        NiNode* pkNode = (NiNode*)pkObj;
        for (unsigned int uiI = 0; uiI < pkNode->GetArrayCount(); uiI++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiI);
            if (pkChild)
                DestroyWorldNormals(pkChild);
        }
    }
}
//---------------------------------------------------------------------------
bool TestCollisions(float fDeltaTime, NiCollisionGroup::Record& kRecord0,
    NiCollisionGroup::Record& kRecord1)
{
    bool bCollision = false;
    TestCollisions(fDeltaTime, kRecord0, kRecord1, bCollision);

    return bCollision;
}
//---------------------------------------------------------------------------
int BoundsTestCheck(float fDeltaTime, NiAVObject* pkObj1, NiAVObject* pkObj2, 
    NiCollisionData::CollisionTest eCollisionTest)    
{
    NiCollisionData* pkCD1 = NiGetCollisionData(pkObj1);
    NiCollisionData* pkCD2 = NiGetCollisionData(pkObj2);
    NiCollisionData::CollisionMode CollisionMode1 = pkCD1->GetCollisionMode();
    NiCollisionData::CollisionMode CollisionMode2 = pkCD2->GetCollisionMode();
    int iNumOfABVs = 0;

    // Create or update ABV Sphere for TRI_NIBOUND case only.
    if (eCollisionTest == NiCollisionData::TRI_NIBOUND)
    {
        NiTransform kWorldInverse;
        if (CollisionMode1 == NiCollisionData::USE_NIBOUND)
        {
            NiBound kBound = pkObj1->GetWorldBound();
            NiTransform kWorldTrans = pkObj1->GetWorldTransform();
            kWorldTrans.Invert(kWorldInverse);

            kBound.SetCenter(kWorldInverse.m_fScale * (kWorldInverse.m_Rotate
                * kBound.GetCenter()) + kWorldInverse.m_Translate);
            kBound.SetRadius(kWorldInverse.m_fScale * kBound.GetRadius());

            NiBoundingVolume* pkSphABV = pkCD1->GetModelSpaceABV();
            if (pkSphABV)   // Update pre-existing ABV Sphere.
            {
                NiSphereBV& kSphABV = (NiSphereBV&)(*pkSphABV);
                kSphABV.SetCenter(kBound.GetCenter());
                kSphABV.SetRadius(kBound.GetRadius());
                iNumOfABVs++;
            }
            else    // Create new ABV Sphere.
            {
                pkSphABV = NiBoundingVolume::ConvertToSphereBV_Safe(kBound);
                if (pkSphABV)
                {
                    pkCD1->SetModelSpaceABV(pkSphABV);
                    iNumOfABVs++;
                }
            }
        }
        else if (CollisionMode2 == NiCollisionData::USE_NIBOUND)
        {
            NiBound kBound = pkObj2->GetWorldBound();
            NiTransform kWorldTrans = pkObj2->GetWorldTransform();
            kWorldTrans.Invert(kWorldInverse);

            kBound.SetCenter(kWorldInverse.m_fScale * (kWorldInverse.m_Rotate
                * kBound.GetCenter()) + kWorldInverse.m_Translate);
            kBound.SetRadius(kWorldInverse.m_fScale * kBound.GetRadius());

            NiBoundingVolume* pkSphABV = pkCD2->GetModelSpaceABV();
            if (pkSphABV)   // Update pre-existing ABV Sphere.
            {
                NiSphereBV& kSphABV = (NiSphereBV&)(*pkSphABV);
                kSphABV.SetCenter(kBound.GetCenter());
                kSphABV.SetRadius(kBound.GetRadius());
                iNumOfABVs++;
            }
            else    // Create new ABV Sphere.
            {
                pkSphABV = NiBoundingVolume::ConvertToSphereBV_Safe(kBound);
                if (pkSphABV)
                {
                    pkCD2->SetModelSpaceABV(pkSphABV);
                    iNumOfABVs++;
                }
            }
        }
    }

    // Create an ABV Box for selected cases, and perform a quick out test if
    // appropriate.
    switch(eCollisionTest)
    {
        case NiCollisionData::ABV_ABV:
            return true;    // Simply let collision testing of ABVs handle it.
            break;
        case NiCollisionData::NOTEST_NOTEST:
            return false;
            break;
        case NiCollisionData::TRI_NIBOUND:
        case NiCollisionData::ABV_NIBOUND:
        case NiCollisionData::OBB_NIBOUND:
        case NiCollisionData::NIBOUND_NIBOUND:
        // Still a chance that we may benefit by creating an ABV for
        // collisions with TRIs or OBBs.
        case NiCollisionData::OBB_ABV:       
        case NiCollisionData::TRI_ABV:      
        case NiCollisionData::TRI_TRI:
        case NiCollisionData::OBB_OBB:
        case NiCollisionData::OBB_TRI:
        {
            // Geometry can have lots of triangles and each would be tested
            // against the ABV.  Instead of testing against the world bounds
            // first, create a BoxABV for "quick out" tests, since it is a
            // better fit and enables more efficient culling.
            if (CollisionMode1 == NiCollisionData::USE_TRI ||
                CollisionMode1 == NiCollisionData::USE_OBB)
            {
                if (!pkCD1->GetModelSpaceABV())
                {
                    // Assert if this object, which has collision data set to
                    // USE_TRI or USE_OBB, is not triangle-based geometry.
                    // Such a case represents an error in how this object was
                    // constructed, since USE_TRI or USE_OBB collision data
                    // should be applied directly to triangle-based geometry in
                    // the scene graph.  Also, to avoid other downstream issues
                    // in Release or Shipping builds, this "garbage in" case is
                    // treated as a non-collision.
                    NIASSERT(NiIsKindOf(NiTriBasedGeom, pkObj1));
                    if (!NiIsKindOf(NiTriBasedGeom, pkObj1))
                        return false;

                    NiTriBasedGeom* pkTri = (NiTriBasedGeom*)pkObj1;

                    // Skip creating ABV Box for low vertex counts.
                    if (pkTri->GetVertexCount() > 25)
                    {
                        NiBoundingVolume* pkBoxBV = 
                            NiBoundingVolume::ConvertToBoxBV( 
                            pkTri->GetVertexCount(), pkTri->GetVertices());

                        if (pkBoxBV)
                        {
                            pkCD1->SetModelSpaceABV(pkBoxBV);
                            iNumOfABVs++;
                        }
                    }
                }
                else
                {
                    iNumOfABVs++;
                }
            }
            else if (CollisionMode1 == NiCollisionData::USE_ABV)
            {
                // Should have an ABV then.
                NIASSERT(pkCD1->GetModelSpaceABV());
                if (pkCD1->GetModelSpaceABV())
                    iNumOfABVs++;               
            }

            if (CollisionMode2 == NiCollisionData::USE_TRI ||
                CollisionMode2 == NiCollisionData::USE_OBB)
            {
                if (!pkCD2->GetModelSpaceABV())
                {
                    // Assert if this object, which has collision data set to
                    // USE_TRI or USE_OBB, is not triangle-based geometry.
                    // Such a case represents an error in how this object was
                    // constructed, since USE_TRI or USE_OBB collision data
                    // should be applied directly to triangle-based geometry in
                    // the scene graph.  Also, to avoid other downstream issues
                    // in Release or Shipping builds, this "garbage in" case is
                    // treated as a non-collision.
                    NIASSERT(NiIsKindOf(NiTriBasedGeom, pkObj2));
                    if (!NiIsKindOf(NiTriBasedGeom, pkObj2))
                        return false;

                    NiTriBasedGeom* pkTri = (NiTriBasedGeom*)pkObj2;

                    // Skip creating ABV Box for low vertex counts.
                    if (pkTri->GetVertexCount() > 25)
                    {
                        NiBoundingVolume* pkBoxBV =
                            NiBoundingVolume::ConvertToBoxBV(
                            pkTri->GetVertexCount(), pkTri->GetVertices());

                        if (pkBoxBV)
                        {
                            pkCD2->SetModelSpaceABV(pkBoxBV);
                            iNumOfABVs++;
                        }
                    }
                }
                else
                {
                    iNumOfABVs++;
                }
            }
            else if (CollisionMode2 == NiCollisionData::USE_ABV)
            {
                // Should have an ABV then.
                NIASSERT(pkCD2->GetModelSpaceABV());
                if (pkCD2->GetModelSpaceABV())
                    iNumOfABVs++;
            }

            // If tighter fitting ABVs exist, they're used in a 1st pass
            // "quick out" test.
            if (iNumOfABVs == 2)
            {
                return NiCollisionUtils::BoundingVolumeTestIntersect(
                    fDeltaTime, pkObj1, pkObj2);
            }

            break;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
int TestCollisions(float fDeltaTime, NiCollisionGroup::Record& kRecord0,
    NiCollisionGroup::Record& kRecord1, bool& bCollision)
{
    NiAVObject* pkObj1 = kRecord0.GetAVObject();
    NiAVObject* pkObj2 = kRecord1.GetAVObject();

    // Look for collision data.
    NiCollisionData* pkCD1 = NiGetCollisionData(pkObj1);
    NiCollisionData* pkCD2 = NiGetCollisionData(pkObj2);

    // In cases were we don't have collision data, we attempt to
    // go deeper into the scene graph to find it. This is to support
    // not having to have collision data at every node.
    if (pkCD1 == NULL || pkCD2 == NULL)
    {
        return TestCollisionDataAndCollisions(fDeltaTime, kRecord0, kRecord1,
            bCollision);
    }

    NiCollisionData::CollisionMode eCollisionMode1
        = pkCD1->GetCollisionMode();
    NiCollisionData::CollisionMode eCollisionMode2
        = pkCD2->GetCollisionMode();

    bCollision = false;

    // NOTEST cases go directly to propagation handling.
    // And only NOTEST with PROPAGATE_ALWAYS will result
    // in possible collisions.

    if (eCollisionMode1 != NiCollisionData::NOTEST &&
        eCollisionMode2 != NiCollisionData::NOTEST)
    {
        if (TestHandleCollisions(fDeltaTime, kRecord0, kRecord1,
            bCollision) == NiCollisionGroup::TERMINATE_COLLISIONS)
        {
            return NiCollisionGroup::TERMINATE_COLLISIONS;
        }
    }
    else
    {
        if (eCollisionMode1 == NiCollisionData::NOTEST ||
            eCollisionMode2 == NiCollisionData::NOTEST)
            bCollision = true;
    }

    return TestPropagateCollisions(fDeltaTime, kRecord0, kRecord1,
        bCollision);
}
//---------------------------------------------------------------------------
int TestCollisionDataAndCollisions(float fDeltaTime,
    NiCollisionGroup::Record& kRecord0, NiCollisionGroup::Record& kRecord1,
    bool& bCollision)
{
    // If pkObj1 is a node, traverse the tree and call FindCollision on
    // everything in the scene graph that has non-NULL collision data,
    // depending on propagation flags.
    NiAVObject* pkObj1 = kRecord0.GetAVObject();
    NiCollisionData* pkCD1 = NiGetCollisionData(pkObj1);

    if ((pkCD1 == NULL) && pkObj1 && pkObj1->IsNode())
    {
        NiNode* pkNode1 = (NiNode*)pkObj1;

        unsigned int uiStart = 0;
        unsigned int uiTotalCnt = pkNode1->GetArrayCount();

        if (NiIsKindOf(NiSwitchNode, pkObj1))   // Special case switch nodes.
        {
            uiStart = ((NiSwitchNode*)pkObj1)->GetIndex();
            uiTotalCnt = uiStart + 1;
        }

        for (unsigned int uiCnt = 0; uiCnt < uiTotalCnt; uiCnt++)
        {
            NiAVObject* pkChild = pkNode1->GetAt(uiCnt);

            if (pkChild == NULL)
                continue;

            NiCollisionGroup::Record kNewRecord0(kRecord0.GetRoot(),
                pkChild, kRecord0.GetMaxDepth(), kRecord0.GetBinSize());

            if (TestCollisions(fDeltaTime, kNewRecord0, kRecord1,bCollision) 
                == NiCollisionGroup::TERMINATE_COLLISIONS)
            {
                return NiCollisionGroup::TERMINATE_COLLISIONS;
            }
        }
        return NiCollisionGroup::CONTINUE_COLLISIONS;
    }

    // If pkObj2 is a node, traverse the tree and call FindCollision on
    // everything in the scene graph that has non-NULL collision data,
    // depending on propagation flags.
    NiAVObject* pkObj2 = kRecord1.GetAVObject();
    NiCollisionData* pkCD2 = NiGetCollisionData(pkObj2);

    if ((pkCD2 == NULL) && pkObj2 && pkObj2->IsNode())
    {
        NiNode* pkNode2 = (NiNode*)pkObj2;

        unsigned int uiStart = 0;
        unsigned int uiTotalCnt = pkNode2->GetArrayCount();

        if (NiIsKindOf(NiSwitchNode, pkObj2))   // Special case switch nodes.  
        {
            uiStart = ((NiSwitchNode*)pkObj2)->GetIndex();
            uiTotalCnt = uiStart + 1;
        }

        for (unsigned int uiCnt = 0; uiCnt < uiTotalCnt; uiCnt++)
        {
            NiAVObject* pkChild = pkNode2->GetAt(uiCnt);

            if (pkChild == NULL)
                continue;

            NiCollisionGroup::Record kNewRecord1(kRecord1.GetRoot(),
                pkChild, kRecord1.GetMaxDepth(), kRecord1.GetBinSize());

            if (TestCollisions(fDeltaTime, kRecord0, kNewRecord1,bCollision)
                == NiCollisionGroup::TERMINATE_COLLISIONS)
            {
                return NiCollisionGroup::TERMINATE_COLLISIONS;
            }
        }
        return NiCollisionGroup::CONTINUE_COLLISIONS;
    }

    return NiCollisionGroup::CONTINUE_COLLISIONS;
}
//---------------------------------------------------------------------------
int FindCollisionDataAndCollisions(float fDeltaTime,
    NiCollisionGroup::Record& kRecord0, NiCollisionGroup::Record& kRecord1)
{
    // If pkObj1 is a node, traverse the tree and call FindCollision on
    // everything in the scene graph that has non-NULL collision data,
    // depending on propagation flags.
    NiAVObject* pkObj1 = kRecord0.GetAVObject();
    NiCollisionData* pkCD1 = NiGetCollisionData(pkObj1);

    if ((pkCD1 == NULL) && pkObj1 && pkObj1->IsNode())
    {
        NiNode* pkNode1 = (NiNode*)pkObj1;

        unsigned int uiStart = 0;
        unsigned int uiTotalCnt = pkNode1->GetArrayCount();

        if (NiIsKindOf(NiSwitchNode, pkObj1))   // Special case switch nodes.
        {
            uiStart = ((NiSwitchNode*)pkObj1)->GetIndex();
            uiTotalCnt = uiStart + 1;
        }

        for (unsigned int uiCnt = uiStart; uiCnt < uiTotalCnt; uiCnt++)
        {
            NiAVObject* pkChild = pkNode1->GetAt(uiCnt);

            if (pkChild == NULL)
                continue;

            NiCollisionGroup::Record kNewRecord0(kRecord0.GetRoot(),
                pkChild, kRecord0.GetMaxDepth(), kRecord0.GetBinSize());

            if (FindCollisions(fDeltaTime, kNewRecord0, kRecord1) ==
                NiCollisionGroup::TERMINATE_COLLISIONS)
            {
                return NiCollisionGroup::TERMINATE_COLLISIONS;
            }
        }
        
        return NiCollisionGroup::CONTINUE_COLLISIONS;
    }

    // If pkObj2 is a node, traverse the tree and call FindCollision on
    // everything in the scene graph that has non-NULL collision data,
    // depending on propagation flags.
    NiAVObject* pkObj2 = kRecord1.GetAVObject();
    NiCollisionData* pkCD2 = NiGetCollisionData(pkObj2);

    if ((pkCD2 == NULL) && pkObj2 && pkObj2->IsNode())
    {
        NiNode* pkNode2 = (NiNode*)pkObj2;

        unsigned int uiStart = 0;
        unsigned int uiTotalCnt = pkNode2->GetArrayCount();

        if (NiIsKindOf(NiSwitchNode, pkObj2))   // Special case switch nodes.
        {
            uiStart = ((NiSwitchNode*)pkObj2)->GetIndex();
            uiTotalCnt = uiStart + 1;
        }

        for (unsigned int uiCnt = uiStart; uiCnt < uiTotalCnt; uiCnt++)
        {
            NiAVObject* pkChild = pkNode2->GetAt(uiCnt);

            if (pkChild == NULL)
                continue;

            NiCollisionGroup::Record kNewRecord1(kRecord1.GetRoot(),
                pkChild, kRecord1.GetMaxDepth(), kRecord1.GetBinSize());

            if (FindCollisions(fDeltaTime, kRecord0, kNewRecord1) ==
                NiCollisionGroup::TERMINATE_COLLISIONS)
            {
                return NiCollisionGroup::TERMINATE_COLLISIONS;
            }
        }
        return NiCollisionGroup::CONTINUE_COLLISIONS;
    }

    return NiCollisionGroup::CONTINUE_COLLISIONS;
}
//---------------------------------------------------------------------------
int FindNotNOTESTAndCollisions(float fDeltaTime,
    NiCollisionGroup::Record& kRecord0, NiCollisionGroup::Record& kRecord1,
    bool bNoTest1, bool bNoTest2)
{
    // If pkObj1 is set up as NOTEST and PROPAGATE_ALWAYS, traverse the tree
    // and call FindCollision on everything in the scene graph that does not
    // have NOTEST and PROPAGATE_ALWAYS, depending on propagation flags.

    NiAVObject* pkObj1 = kRecord0.GetAVObject();

    if (bNoTest1 && pkObj1 && pkObj1->IsNode())
    {
        NiNode* pkNode1 = (NiNode*)pkObj1;
        unsigned int uiStart = 0;
        unsigned int uiTotalCnt = pkNode1->GetArrayCount();

        if (NiIsKindOf(NiSwitchNode, pkObj1))   // Special case switch nodes.
        {
            uiStart = ((NiSwitchNode*)pkObj1)->GetIndex();
            uiTotalCnt = uiStart + 1;
        }

        for (unsigned int uiCnt = uiStart; uiCnt < uiTotalCnt; uiCnt++)
        {
            NiAVObject* pkChild = pkNode1->GetAt(uiCnt);

            if (pkChild == NULL)
                continue;

            NiCollisionGroup::Record kNewRecord0(kRecord0.GetRoot(),
                pkChild, kRecord0.GetMaxDepth(), kRecord0.GetBinSize());

            if (FindCollisions(fDeltaTime, kNewRecord0, kRecord1) ==
                NiCollisionGroup::TERMINATE_COLLISIONS)
            {
                return NiCollisionGroup::TERMINATE_COLLISIONS;
            }
        }

        return NiCollisionGroup::CONTINUE_COLLISIONS;
    }

    // If pkObj2 is set up as NOTEST and PROPAGATE_ALWAYS, traverse the tree
    // and call FindCollision on everything in the scene graph that does not
    // have NOTEST and PROPAGATE_ALWAYS, depending on propagation flags.

    NiAVObject* pkObj2 = kRecord1.GetAVObject();

    if (bNoTest2 && pkObj2 && pkObj2->IsNode())
    {
        NiNode* pkNode2 = (NiNode*)pkObj2;
        unsigned int uiStart = 0;
        unsigned int uiTotalCnt = pkNode2->GetArrayCount();

        if (NiIsKindOf(NiSwitchNode, pkObj2))   // Special case switch nodes.
        {
            uiStart = ((NiSwitchNode*)pkObj2)->GetIndex();
            uiTotalCnt = uiStart + 1;
        }

        for (unsigned int uiCnt = uiStart; uiCnt < uiTotalCnt; uiCnt++)
        {
            NiAVObject* pkChild = pkNode2->GetAt(uiCnt);

            if (pkChild == NULL)
                continue;

            NiCollisionGroup::Record kNewRecord1(kRecord1.GetRoot(),
                pkChild, kRecord1.GetMaxDepth(), kRecord1.GetBinSize());

            if (FindCollisions(fDeltaTime, kRecord0, kNewRecord1) ==
                NiCollisionGroup::TERMINATE_COLLISIONS)
            {
                return NiCollisionGroup::TERMINATE_COLLISIONS;
            }
        }
        return NiCollisionGroup::CONTINUE_COLLISIONS;
    }

    return NiCollisionGroup::CONTINUE_COLLISIONS;
}
//---------------------------------------------------------------------------
int TestCollisionsForRecord0(NiNode* pkNode, float fDeltaTime,
    NiCollisionGroup::Record& kRecord0, NiCollisionGroup::Record& kRecord1,
    bool& bCollisionSuccess)
{
    unsigned int uiTotalCnt = pkNode->GetArrayCount();

    // Propagate to children.
    for (unsigned int uiCnt = 0; uiCnt < uiTotalCnt; uiCnt++)
    {
        NiAVObject* pkChild = pkNode->GetAt(uiCnt);

        if (pkChild == NULL)
            continue;

        NiCollisionGroup::Record kNewRecord0(kRecord0.GetRoot(),
            pkChild, kRecord0.GetMaxDepth(), kRecord0.GetBinSize());

        if (TestCollisions(fDeltaTime, kNewRecord0, kRecord1,
            bCollisionSuccess) == NiCollisionGroup::TERMINATE_COLLISIONS)
        {
            return NiCollisionGroup::TERMINATE_COLLISIONS;
        }
    }
    return NiCollisionGroup::CONTINUE_COLLISIONS;
}
//---------------------------------------------------------------------------
int TestCollisionsForRecord1(NiNode* pkNode, float fDeltaTime,
    NiCollisionGroup::Record& kRecord0, NiCollisionGroup::Record& kRecord1,
    bool& bCollisionSuccess)
{
    unsigned int uiTotalCnt = pkNode->GetArrayCount();

    // Propagate to children.
    for (unsigned int uiCnt = 0; uiCnt < uiTotalCnt; uiCnt++)
    {
        NiAVObject* pkChild = pkNode->GetAt(uiCnt);

        if (pkChild == NULL)
            continue;

        NiCollisionGroup::Record kNewRecord1(kRecord1.GetRoot(),
            pkChild, kRecord1.GetMaxDepth(), kRecord1.GetBinSize());

        if (TestCollisions(fDeltaTime, kRecord0, kNewRecord1,
            bCollisionSuccess) == NiCollisionGroup::TERMINATE_COLLISIONS)
        {
            return NiCollisionGroup::TERMINATE_COLLISIONS;
        }
    }
    return NiCollisionGroup::CONTINUE_COLLISIONS;
}
//---------------------------------------------------------------------------
int TestPropagateCollisions(float fDeltaTime, 
    NiCollisionGroup::Record& kRecord0, NiCollisionGroup::Record& kRecord1,
    bool& bCollisionSuccess)
{
    if (!kRecord0.m_bLocked)
    {
        NiAVObject* pkObj1 = kRecord0.GetAVObject();
        NiCollisionData* pkCD1 = NiGetCollisionData(pkObj1);

        if (pkCD1)
        {
            // Essentially we never lock down conditionals, but instead pass
            // along to their immediate children.
             if ((pkCD1->GetPropagationMode() == NiCollisionData::
                 PROPAGATE_ON_SUCCESS && bCollisionSuccess) ||
                 (pkCD1->GetPropagationMode() == NiCollisionData::
                 PROPAGATE_ON_FAILURE && !bCollisionSuccess))
             {
                NIASSERT(NiIsKindOf(NiNode,pkObj1));

                NiNode* pkNode = (NiNode*)pkObj1;
                unsigned int uiTotalCnt = pkNode->GetArrayCount();

                // Propagate to children.
                for (unsigned int uiCnt = 0; uiCnt < uiTotalCnt; uiCnt++)
                {
                    NiAVObject* pkChild = pkNode->GetAt(uiCnt);

                    if (pkChild == NULL)
                        continue;

                    NiCollisionGroup::Record kNewRecord0(kRecord0.GetRoot(),
                        pkChild, kRecord0.GetMaxDepth(),
                        kRecord0.GetBinSize());

                    if (TestCollisions(fDeltaTime, kNewRecord0, kRecord1,
                        bCollisionSuccess)
                        == NiCollisionGroup::TERMINATE_COLLISIONS)
                    {
                        return NiCollisionGroup::TERMINATE_COLLISIONS;
                    }
                }
                return NiCollisionGroup::CONTINUE_COLLISIONS;
             }
        }

        // Begin the lock down.
        kRecord0.m_bLocked = true;
        if (TestPropagateCollisions(fDeltaTime, kRecord0, kRecord1,
            bCollisionSuccess) == NiCollisionGroup::TERMINATE_COLLISIONS)
        {
            kRecord0.m_bLocked = false;
            return NiCollisionGroup::TERMINATE_COLLISIONS;
        }
        kRecord0.m_bLocked = false;
    }

    if (!kRecord0.m_bLocked)
    {
        NiAVObject* pkObj1 = kRecord0.GetAVObject();
        if (!pkObj1 || pkObj1->IsLeaf())
            return NiCollisionGroup::CONTINUE_COLLISIONS;

        bool bPropagate;

        NiCollisionData* pkCD1 = NiGetCollisionData(pkObj1);

        if (pkCD1)
        {
            NiCollisionData::PropagationMode ePropagation1 =
                pkCD1->GetPropagationMode();

            if (pkCD1->GetCollisionMode() == NiCollisionData::NOTEST)
            {
                if (ePropagation1 != NiCollisionData::PROPAGATE_ALWAYS)
                {
                    // Short circuit this case because NOTEST without
                    // propagation will not do anything.
                    return NiCollisionGroup::CONTINUE_COLLISIONS;
                }
                else
                {
                    bPropagate = true;
                }
            }
            else
            {
                bPropagate = ShouldPropagationOccur(ePropagation1, 
                   bCollisionSuccess);
            }
        }
        else
        {
            bPropagate = true;
        }

        if (bPropagate)
        {
            return TestCollisionsForRecord0((NiNode*)pkObj1, fDeltaTime,
                kRecord0, kRecord1, bCollisionSuccess);
        }
    }
    else 
    {
        bool bPropagate;

        NiAVObject* pkObj2 = kRecord1.GetAVObject();
        NiCollisionData* pkCD2 = NiGetCollisionData(pkObj2);

        if (pkCD2)
        {
            NiCollisionData::PropagationMode ePropagation2 =
                pkCD2->GetPropagationMode();
        
            if (pkCD2->GetCollisionMode() == NiCollisionData::NOTEST)
            {
                if (ePropagation2 != NiCollisionData::PROPAGATE_ALWAYS)
                {
                    // Short circuit this case because NOTEST without
                    // propagation will not do anything.
                    return NiCollisionGroup::CONTINUE_COLLISIONS;
                }
                else
                {
                    bPropagate = true;
                }
            }
            else
            {
                bPropagate = ShouldPropagationOccur(ePropagation2, 
                    bCollisionSuccess);
            }
        }
        else
        {
            bPropagate = true;
        }

        if (bPropagate)
        {
            return TestCollisionsForRecord1((NiNode*)pkObj2, fDeltaTime,
                kRecord0, kRecord1, bCollisionSuccess);
        }
    }

    return NiCollisionGroup::CONTINUE_COLLISIONS;  // Didn't need propagation.
}
//---------------------------------------------------------------------------
bool ShouldPropagationOccur(NiCollisionData::PropagationMode ePropagation,
    bool bCollisionSuccess)
{
    switch(ePropagation)
    {
        case NiCollisionData::PROPAGATE_ALWAYS:
            return true;
        case NiCollisionData::PROPAGATE_ON_SUCCESS:
            if ( bCollisionSuccess )
                return true;
            break;
        case NiCollisionData::PROPAGATE_ON_FAILURE:
            if ( !bCollisionSuccess )
                return true;
            break;
        default:
            break;
    }

    return false;
}
//---------------------------------------------------------------------------
int FindCollisionsForRecord0(NiNode* pkNode, float fDeltaTime,
    NiCollisionGroup::Record& kRecord0, NiCollisionGroup::Record& kRecord1)
{
    unsigned int uiTotalCnt = pkNode->GetArrayCount();

    // Propagate to children.
    for (unsigned int uiCnt = 0; uiCnt < uiTotalCnt; uiCnt++)
    {
        NiAVObject* pkChild = pkNode->GetAt(uiCnt);

        if (pkChild == NULL)
            continue;

        NiCollisionGroup::Record kNewRecord0(kRecord0.GetRoot(),
            pkChild, kRecord0.GetMaxDepth(), kRecord0.GetBinSize());

        if (FindCollisions(fDeltaTime, kNewRecord0, kRecord1) ==
            NiCollisionGroup::TERMINATE_COLLISIONS)
        {
            return NiCollisionGroup::TERMINATE_COLLISIONS;
        }
    }
    return NiCollisionGroup::CONTINUE_COLLISIONS;
}
//---------------------------------------------------------------------------
int FindCollisionsForRecord1(NiNode* pkNode, float fDeltaTime,
    NiCollisionGroup::Record& kRecord0, NiCollisionGroup::Record& kRecord1)
{
    unsigned int uiTotalCnt = pkNode->GetArrayCount();

    // Propagate to children.
    for (unsigned int uiCnt = 0; uiCnt < uiTotalCnt; uiCnt++)
    {
        NiAVObject* pkChild = pkNode->GetAt(uiCnt);

        if (pkChild == NULL)
            continue;

        NiCollisionGroup::Record kNewRecord1(kRecord1.GetRoot(),
            pkChild, kRecord1.GetMaxDepth(), kRecord1.GetBinSize());

        if (FindCollisions(fDeltaTime, kRecord0, kNewRecord1) ==
            NiCollisionGroup::TERMINATE_COLLISIONS)
        {
            return NiCollisionGroup::TERMINATE_COLLISIONS;
        }
    }
    return NiCollisionGroup::CONTINUE_COLLISIONS;
}
//---------------------------------------------------------------------------
int PropagateCollisions(float fDeltaTime, NiCollisionGroup::Record& kRecord0,
    NiCollisionGroup::Record& kRecord1, bool bCollisionSuccess)
{
    if (!kRecord0.m_bLocked)
    {
        NiAVObject* pkObj1 = kRecord0.GetAVObject();
        NiCollisionData* pkCD1 = NiGetCollisionData(pkObj1);

        if (pkCD1)
        {
            // Essentially we never lock down conditionals, but instead pass
            // along to their immediate children.
             if ((pkCD1->GetPropagationMode() == NiCollisionData::
                 PROPAGATE_ON_SUCCESS && bCollisionSuccess) ||
                 (pkCD1->GetPropagationMode() == NiCollisionData::
                 PROPAGATE_ON_FAILURE && !bCollisionSuccess))
             {
                NIASSERT(NiIsKindOf(NiNode,pkObj1));

                NiNode* pkNode = (NiNode*)pkObj1;

                unsigned int uiTotalCnt = pkNode->GetArrayCount();

                // Propagate to children.
                for (unsigned int uiCnt = 0; uiCnt < uiTotalCnt; uiCnt++)
                {
                    NiAVObject* pkChild = pkNode->GetAt(uiCnt);

                    if (pkChild == NULL)
                        continue;

                    NiCollisionGroup::Record kNewRecord0(kRecord0.GetRoot(),
                        pkChild, kRecord0.GetMaxDepth(),
                        kRecord0.GetBinSize());

                    if (FindCollisions(fDeltaTime, kNewRecord0, kRecord1) ==
                        NiCollisionGroup::TERMINATE_COLLISIONS)
                    {
                        return NiCollisionGroup::TERMINATE_COLLISIONS;
                    }
                }

                return NiCollisionGroup::CONTINUE_COLLISIONS;
             }
        }

        // Begin the lock down.
        kRecord0.m_bLocked = true;
        if (PropagateCollisions(fDeltaTime, kRecord0, kRecord1,
            bCollisionSuccess) == NiCollisionGroup::TERMINATE_COLLISIONS)
        {
            kRecord0.m_bLocked = false;
            return NiCollisionGroup::TERMINATE_COLLISIONS;
        }
        kRecord0.m_bLocked = false;
    }

    if (!kRecord0.m_bLocked)
    {
        NiAVObject* pkObj1 = kRecord0.GetAVObject();
        if (!pkObj1 || pkObj1->IsLeaf())
            return NiCollisionGroup::CONTINUE_COLLISIONS;
        
        bool bPropagate;

        NiCollisionData* pkCD1 = NiGetCollisionData(pkObj1);
 
        if (pkCD1)
        {
            NiCollisionData::PropagationMode ePropagation1 =
                pkCD1->GetPropagationMode();

            if (pkCD1->GetCollisionMode() == NiCollisionData::NOTEST)
            {
                if (ePropagation1 != NiCollisionData::PROPAGATE_ALWAYS)
                {
                    // Short circuit this case because NOTEST without
                    // propagation will not do anything.
                    return NiCollisionGroup::CONTINUE_COLLISIONS;
                }
                else bPropagate = true;
            }
            else
            {
                bPropagate = ShouldPropagationOccur(ePropagation1, 
                   bCollisionSuccess);
            }
        }
        else bPropagate = true;

        if (bPropagate)
            return FindCollisionsForRecord0((NiNode*)pkObj1, fDeltaTime,
                kRecord0, kRecord1);
    }
    else 
    {
        NiAVObject* pkObj2 = kRecord1.GetAVObject();

        bool bPropagate;
 
        NiCollisionData* pkCD2 = NiGetCollisionData(pkObj2);

        if (pkCD2)
        {
            NiCollisionData::PropagationMode ePropagation2 =
                pkCD2->GetPropagationMode();

            if (!pkObj2 || pkObj2->IsLeaf())
                bPropagate = false;
            else if (pkCD2->GetCollisionMode() == NiCollisionData::NOTEST)
            {
                if (ePropagation2 != NiCollisionData::PROPAGATE_ALWAYS)
                {
                    // Short circuit this case because NOTEST without
                    // propagation will not do anything.
                    return NiCollisionGroup::CONTINUE_COLLISIONS;
                }
                else bPropagate = true;
            }
            else
            {
                bPropagate = ShouldPropagationOccur(ePropagation2, 
                    bCollisionSuccess);
            }
        }
        else bPropagate = true;

        if (bPropagate)
            return FindCollisionsForRecord1((NiNode*)pkObj2, fDeltaTime,
                kRecord0, kRecord1);
    }

    return NiCollisionGroup::CONTINUE_COLLISIONS;  // Didn't need propagation.
}
//---------------------------------------------------------------------------
int TestHandleCollisions(float fDeltaTime, NiCollisionGroup::Record& kRecord0,
    NiCollisionGroup::Record& kRecord1, bool& bCollision)
{
    NiAVObject* pkObj1 = kRecord0.GetAVObject();
    NiAVObject* pkObj2 = kRecord1.GetAVObject();

    NiCollisionData::CollisionTest eCollisionTest
        = NiCollisionData::GetCollisionTestType(pkObj1, pkObj2);

    if (BoundsTestCheck(fDeltaTime, pkObj1, pkObj2, eCollisionTest) == false)
        return NiCollisionGroup::CONTINUE_COLLISIONS;

    switch(eCollisionTest)
    {
        case NiCollisionData::OBB_NIBOUND:
        case NiCollisionData::ABV_NIBOUND:
        case NiCollisionData::NIBOUND_NIBOUND:
            NIMETRICS_COLLISION_ADDVALUE(TEST_NIBOUND_NIBOUND, 1);
            return Test_NIBOUNDvsNIBOUND(
                fDeltaTime, pkObj1, pkObj2, bCollision);

        case NiCollisionData::OBB_OBB:
        {
            NIMETRICS_COLLISION_ADDVALUE(TEST_OBB_OBB, 1);
            bool bResult = Test_OBBvsOBB(fDeltaTime, pkObj1, pkObj2, kRecord0, 
                kRecord1);

            if (bResult)
                bCollision = true;

            return NiCollisionGroup::TERMINATE_COLLISIONS;
        }

        case NiCollisionData::OBB_TRI: 
        {
            NIMETRICS_COLLISION_ADDVALUE(TEST_OBB_TRI, 1);
            bool bResult = Test_OBBvsTRI(fDeltaTime, pkObj1, pkObj2, kRecord0,
                kRecord1);
            
            if (bResult)
                bCollision = true;

            return NiCollisionGroup::TERMINATE_COLLISIONS;
        }

        case NiCollisionData::OBB_ABV:
            NIMETRICS_COLLISION_ADDVALUE(TEST_OBB_ABV, 1);
            return Test_OBBvsABV(fDeltaTime, pkObj1, pkObj2, kRecord0,
                kRecord1, bCollision);

        case NiCollisionData::TRI_TRI:
            NIMETRICS_COLLISION_ADDVALUE(TEST_TRI_TRI, 1);
            return Test_TRIvsTRI(fDeltaTime, pkObj1, pkObj2,bCollision);
        
        case NiCollisionData::TRI_ABV:      
        case NiCollisionData::TRI_NIBOUND:
            NIMETRICS_COLLISION_ADDVALUE(TEST_TRI_ABV, 1);
            return Test_TRIvsABV(fDeltaTime, pkObj1, pkObj2, bCollision);
        
        case NiCollisionData::ABV_ABV:
            NIMETRICS_COLLISION_ADDVALUE(TEST_ABV_ABV, 1);
            return Test_ABVvsABV(fDeltaTime, pkObj1, pkObj2, bCollision);

        default:
            break;
    }

    return NiCollisionGroup::CONTINUE_COLLISIONS;
}
//---------------------------------------------------------------------------
int HandleCollisions(float fDeltaTime, NiCollisionGroup::Record& kRecord0,
    NiCollisionGroup::Record& kRecord1, NiCollisionGroup::Intersect& kIntr,
    bool& bCollision)
{
    bCollision = false;
    bool bCalcNormals = true;

    NiAVObject* pkObj1 = kRecord0.GetAVObject();
    NiAVObject* pkObj2 = kRecord1.GetAVObject();

    kIntr.pkRoot0 = kRecord0.GetRoot();
    kIntr.pkObj0 = kRecord0.GetAVObject();
    kIntr.pkRoot1 = kRecord1.GetRoot();
    kIntr.pkObj1 = kRecord1.GetAVObject();

    NiCollisionData::CollisionTest eCollisionTest =
        NiCollisionData::GetCollisionTestType(pkObj1, pkObj2);

    if (BoundsTestCheck(fDeltaTime, pkObj1, pkObj2, eCollisionTest) == false)
        return NiCollisionGroup::CONTINUE_COLLISIONS;

    switch(eCollisionTest)
    {
        case NiCollisionData::OBB_NIBOUND:
        case NiCollisionData::ABV_NIBOUND:
        case NiCollisionData::NIBOUND_NIBOUND:
            NIMETRICS_COLLISION_ADDVALUE(FIND_NIBOUND_NIBOUND, 1);
            return Find_NIBOUNDvsNIBOUND(fDeltaTime, pkObj1, pkObj2, 
                bCalcNormals, kIntr, bCollision);

        case NiCollisionData::OBB_OBB:
            NIMETRICS_COLLISION_ADDVALUE(FIND_OBB_OBB, 1);
            return Find_OBBvsOBB(fDeltaTime, pkObj1, pkObj2, kRecord0, 
                kRecord1, bCollision);

        case NiCollisionData::OBB_TRI: 
            NIMETRICS_COLLISION_ADDVALUE(FIND_OBB_TRI, 1);
            return Find_OBBvsTRI(fDeltaTime, pkObj1, pkObj2, kRecord0, 
                kRecord1, bCollision);

        case NiCollisionData::OBB_ABV:
            NIMETRICS_COLLISION_ADDVALUE(FIND_OBB_ABV, 1);
            return Find_OBBvsABV(fDeltaTime, pkObj1, pkObj2, kRecord0,
                kRecord1, true, kIntr, bCollision);

        case NiCollisionData::TRI_TRI:
            NIMETRICS_COLLISION_ADDVALUE(FIND_TRI_TRI, 1);
            return Find_TRIvsTRI(fDeltaTime, pkObj1, pkObj2, true, kIntr, 
                bCollision);
        
        case NiCollisionData::TRI_ABV:      
        case NiCollisionData::TRI_NIBOUND:
            NIMETRICS_COLLISION_ADDVALUE(FIND_TRI_ABV, 1);
            return Find_TRIvsABV(fDeltaTime, pkObj1, pkObj2, true, kIntr,
                bCollision);
        
        case NiCollisionData::ABV_ABV:
            NIMETRICS_COLLISION_ADDVALUE(FIND_ABV_ABV, 1);
            return Find_ABVvsABV(fDeltaTime, pkObj1, pkObj2, bCalcNormals,
                kIntr, bCollision);

        default:
            break;
    }

    return NiCollisionGroup::CONTINUE_COLLISIONS;
}
//---------------------------------------------------------------------------
int FindCollisions(float fDeltaTime, NiCollisionGroup::Record& kRecord0,
    NiCollisionGroup::Record& kRecord1)
{
    NiAVObject* pkObj1 = kRecord0.GetAVObject();
    NiAVObject* pkObj2 = kRecord1.GetAVObject();

    // Look for collision data.
    NiCollisionData* pkCD1 = NiGetCollisionData(pkObj1);
    NiCollisionData* pkCD2 = NiGetCollisionData(pkObj2);

    // In cases were there is no collision data, attempt to go deeper into the
    // scene graph to find it.  This approach is to support not requiring
    // collision data at every node.
    if (pkCD1 == NULL || pkCD2 == NULL)
        return FindCollisionDataAndCollisions(fDeltaTime, kRecord0, kRecord1);

    // NOTEST Optimization.
    bool bNoTest1 =
        pkCD1->GetPropagationMode() == NiCollisionData::PROPAGATE_ALWAYS &&
        pkCD1->GetCollisionMode() == NiCollisionData::NOTEST;
    bool bNoTest2 =
        pkCD2->GetPropagationMode() == NiCollisionData::PROPAGATE_ALWAYS &&
        pkCD2->GetCollisionMode() == NiCollisionData::NOTEST;
    if (bNoTest1 || bNoTest2)
    {
        return FindNotNOTESTAndCollisions(fDeltaTime, kRecord0, kRecord1,
            bNoTest1, bNoTest2);
    }

    // Steps to insure consistency for PROPAGATE_ON_FAILURE.  Note that
    // PROPAGATE_ON_FAILURE has been deprecated.
    if ((!(pkCD1->GetPropagationMode() ==
        NiCollisionData::PROPAGATE_ON_FAILURE)) && pkCD2->GetPropagationMode()
        == NiCollisionData::PROPAGATE_ON_FAILURE)
    {
        // Swap records.
        return FindCollisions(fDeltaTime, kRecord1, kRecord0);
    }

    NiCollisionData::CollisionMode eCollisionMode1 = 
        pkCD1->GetCollisionMode();
    NiCollisionData::CollisionMode eCollisionMode2 = 
        pkCD2->GetCollisionMode();

    bool bCollision = false;

    // NOTEST cases go directly to propagation handling, and only NOTEST with
    // PROPAGATE_ALWAYS will result in possible collisions.
    if (eCollisionMode1 != NiCollisionData::NOTEST &&
        eCollisionMode2 != NiCollisionData::NOTEST)
    {
        NiCollisionGroup::Intersect kIntr;

        kIntr.fTime = 0.0f; // Initialize.

        if (HandleCollisions(fDeltaTime, kRecord0, kRecord1, kIntr,
            bCollision) == NiCollisionGroup::TERMINATE_COLLISIONS)
        {
            return NiCollisionGroup::TERMINATE_COLLISIONS;
        }    
    }
    else
    {
        if (eCollisionMode1 == NiCollisionData::NOTEST ||
            eCollisionMode2 == NiCollisionData::NOTEST)
            bCollision = true;
    }

    return PropagateCollisions(fDeltaTime, kRecord0, kRecord1, bCollision);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// OBB creation via scene graph traversal
//---------------------------------------------------------------------------
void CreateOBB(NiAVObject* pkRoot, int iBinSize)
{
    if (!pkRoot)
        return;

    if (pkRoot && pkRoot->IsNode())
    {
        NiNode* pkNode = (NiNode*)pkRoot;
        const unsigned int uiSize = pkNode->GetArrayCount();

        for (unsigned int i = 0; i < uiSize; i++)
        {
            NiAVObject* pkChild = pkNode->GetAt(i);

            if (pkChild)
                CreateOBB(pkChild, iBinSize);
        }
    }
    else if (NiIsKindOf(NiTriBasedGeom, pkRoot))
    {
        NiCollisionData* pkData = NiGetCollisionData(pkRoot);

        if (!pkData)
        {
            pkData = NiNew NiCollisionData(pkRoot);
            NIASSERT(pkData);
        }

        pkData->CreateOBB(iBinSize);
    }
}
//---------------------------------------------------------------------------
void DestroyOBB(NiAVObject* pkRoot)
{
    if (pkRoot && pkRoot->IsNode())
    {
        NiNode* pkNode = (NiNode*)pkRoot;
        const unsigned int uiSize = pkNode->GetArrayCount();

        for (unsigned int i = 0; i < uiSize; i++)
        {
            NiAVObject* pkChild = pkNode->GetAt(i);
            if (pkChild)
                DestroyOBB(pkChild);
        }
    }
    else if (NiIsKindOf(NiTriBasedGeom, pkRoot))
    {
        NiCollisionData* pkData = NiGetCollisionData(pkRoot);

        if (pkData)
           pkData->DestroyOBB();
    }
}
//---------------------------------------------------------------------------
// ABV updates via scene graph traversal
//---------------------------------------------------------------------------
void UpdateWorldData(NiAVObject* pkRoot)
{
    if (pkRoot)
    {
        NiCollisionData* pkData = NiGetCollisionData(pkRoot);

        if (pkData)
            pkData->UpdateWorldData();

        if (pkRoot && pkRoot->IsNode())
        {
            NiNode* pkNode = NiStaticCast(NiNode, pkRoot);

            for (unsigned int i = 0; i < pkNode->GetArrayCount(); i++)
            {
                NiAVObject* pkChild = pkNode->GetAt(i);

                if (!pkChild)
                    continue;

                NiCollisionTraversals::UpdateWorldData(pkChild);
            }
        }
    }
}
//---------------------------------------------------------------------------
// Picking scene graph traversal routine.
//---------------------------------------------------------------------------
bool FindIntersections(const NiPoint3& kOrigin, const NiPoint3& kDir,
    NiPick& kPick, NiAVObject* pkRoot)
{
    if (!pkRoot)
        return false;

    if (kPick.GetObserveAppCullFlag() && pkRoot->GetAppCulled())
        return false;

    if (!NiCollisionUtils::FindBoundIntersect(pkRoot, kOrigin, kDir))
        return false;
    
    if (pkRoot && pkRoot->IsNode())
    {
        return FindIntersectionsNode(kOrigin, kDir, kPick, (NiNode*)pkRoot);
    }
    else if (NiIsKindOf(NiTriBasedGeom, pkRoot))
    {
        return FindIntersectionsTriBasedGeom(kOrigin, kDir, kPick, 
            (NiTriBasedGeom*)pkRoot);
    }

    return false;
}
//---------------------------------------------------------------------------
bool FindIntersectionsSwitchNode(const NiPoint3& kOrigin,
    const NiPoint3& kDir, NiPick& kPick, NiSwitchNode* pkSwitch)
{
    // Pick only on the active child.
    bool bFound = false;

    if (pkSwitch->GetIndex() >= 0)
    {
        if (kPick.GetPickType() == NiPick::FIND_FIRST &&
            kPick.GetSortType() == NiPick::NO_SORT && kPick.GetSize())
        {
           return true;
        }

        NiAVObject* pkChild = pkSwitch->GetActiveChild();

        if (pkChild)
        {
            if (FindIntersections(kOrigin, kDir, kPick, pkChild))
                bFound = true;
        }
    }

    return bFound;
}
//---------------------------------------------------------------------------
bool FindIntersectionsNode(const NiPoint3& kOrigin, const NiPoint3& kDir,
    NiPick& kPick, NiNode* pkNode)
{
#if NIMETRICS
    kPick.m_uiNodeComparisons++;
#endif

    // Return if bound intersect and find first.
    if (kPick.GetIntersectType() == NiPick::BOUND_INTERSECT && 
        kPick.GetPickType() == NiPick::FIND_FIRST)
    {
        kPick.Add(pkNode);
        return true;
    }

    if (NiIsKindOf(NiSwitchNode, pkNode))
    {
        return FindIntersectionsSwitchNode(kOrigin, kDir, kPick, 
            (NiSwitchNode*)pkNode);
    }

    bool bFound = false;
    for (unsigned int i = 0; i < pkNode->GetArrayCount(); i++)
    {
        if (kPick.GetPickType() == NiPick::FIND_FIRST &&
            kPick.GetSortType() == NiPick::NO_SORT && kPick.GetSize())
        {
           return true;
        }

        NiAVObject* pkChild = pkNode->GetAt(i);

        if (!pkChild)
            continue;

        if (FindIntersections(kOrigin, kDir, kPick, pkChild))
            bFound = true;
    }

    return bFound;
}
//---------------------------------------------------------------------------
bool FindIntersectionsTriBasedGeom(const NiPoint3& kOrigin,
    const NiPoint3& kDir, NiPick& kPick, NiTriBasedGeom* pkTBG)
{
    // Return if bound intersect.
    if (kPick.GetIntersectType() == NiPick::BOUND_INTERSECT)
    {
        kPick.Add(pkTBG);
    }

    if (NiIsKindOf(NiTriStrips, pkTBG))
    {
        return NiCollisionTraversals::FindIntersections_TriStripFastPath(
            kOrigin, kDir, kPick, (NiTriStrips*)pkTBG);
    }
    else if (NiIsKindOf(NiTriShape, pkTBG))
    {
        return NiCollisionTraversals::FindIntersections_TriShapeFastPath(
            kOrigin, kDir, kPick, (NiTriShape*)pkTBG);
    }
    
    // Find ray's model space kOrigin and kDir.
    const NiTransform& kWorld = pkTBG->GetWorldTransform();
    NiPoint3 kDiff = kOrigin - kWorld.m_Translate;
    float fInvWorldScale = 1.0f / kWorld.m_fScale;
    NiPoint3 kModelOrigin = (kDiff * kWorld.m_Rotate) * fInvWorldScale;
    NiPoint3 kModelDir = (kDir * kWorld.m_Rotate);

    bool bFound = false;
    unsigned short usTris = pkTBG->GetActiveTriangleCount();

    NiPoint3* pVertex = pkTBG->GetVertices();
    if (!pVertex)
    {
        NiOutputDebugString(
            "Warning:  Geometry for NiPick operation is missing.\n"
            "  May need to:\n"
            "  a) Use KEEP flags with NiGeometryData::SetKeepFlags()\n"
            "     to keep the generic NiGeometryData attributes needed\n"
            "     for picking,\n"
            "       or\n"
            "  b) Export the data to keep the NiGeometryData members in\n"
            "     the Nif file.\n");
        return false;
    }

    for (unsigned short i = 0; i < usTris; i++)
    {
#if NIMETRICS
        kPick.m_uiTriComparisons++;
#endif
        unsigned short i0, i1, i2;
        pkTBG->GetTriangleIndices(i, i0, i1, i2);
        
        NiPoint3 kIntersect;
        float fLineParam, fTriParam1, fTriParam2;
        
        bool bIntersects = NiCollisionUtils::IntersectTriangle(kModelOrigin,
            kModelDir, pVertex[i0], pVertex[i1], pVertex[i2],
            kPick.GetFrontOnly(), kIntersect, fLineParam, fTriParam1,
            fTriParam2);
        
        if (bIntersects)
        {
            bFound = true;
            UpdatePickRecord(kPick, pkTBG, kWorld, kIntersect, fLineParam,
                fTriParam1, fTriParam2, i);
            
            if (kPick.GetPickType() == NiPick::FIND_FIRST &&
                kPick.GetSortType() == NiPick::NO_SORT)
            {
                return true;
            }
        }
    }

    return bFound;
}
//---------------------------------------------------------------------------
bool FindIntersections_TriStripFastPath(const NiPoint3& kOrigin,
    const NiPoint3& kDir, NiPick& kPick, NiTriStrips* pkTBG)
{
    // Return if bound intersect.
    if (kPick.GetIntersectType() == NiPick::BOUND_INTERSECT)
    {
        kPick.Add(pkTBG);
    }

    // Find ray's model space kOrigin and kDir.
    const NiTransform& kWorld = pkTBG->GetWorldTransform();
    NiPoint3 kDiff = kOrigin - kWorld.m_Translate;
    float fInvWorldScale = 1.0f / kWorld.m_fScale;
    NiPoint3 kModelOrigin = (kDiff * kWorld.m_Rotate) * fInvWorldScale;
    NiPoint3 kModelDir = (kDir * kWorld.m_Rotate);

    bool bFound = false;
    unsigned short usTris = pkTBG->GetActiveTriangleCount();

    NiPoint3* pVertex = pkTBG->GetVertices();
    if (!pVertex)
    {
        NiOutputDebugString(
            "Warning:  Geometry for NiPick operation is missing.\n"
            "  May need to:\n"
            "  a) Use KEEP flags with NiGeometryData::SetKeepFlags()\n"
            "     to keep the generic NiGeometryData attributes needed\n"
            "     for picking,\n"
            "       or\n"
            "  b) Export the data to keep the NiGeometryData members in\n"
            "     the Nif file.\n");
        return false;
    }

    NiTriStripsData* pkData = (NiTriStripsData*)pkTBG->GetModelData();

    for (unsigned short i = 0; i < usTris; i++)
    {
#if NIMETRICS
        kPick.m_uiTriComparisons++;
#endif
        unsigned short i0, i1, i2;
        pkData->GetTriangleIndices_Inline(i, i0, i1, i2);

        // Skip degenerates. We usually have one strip.
        if (i0 == i1 || i1 == i2)
            continue;
        
        NiPoint3 kIntersect;
        float fLineParam, fTriParam1, fTriParam2;
        
        bool bIntersects = NiCollisionUtils::IntersectTriangle(kModelOrigin,
            kModelDir, pVertex[i0], pVertex[i1], pVertex[i2],
            kPick.GetFrontOnly(), kIntersect, fLineParam, fTriParam1,
            fTriParam2);
        
        if (bIntersects)
        {
            bFound = true;
            UpdatePickRecord(kPick, pkTBG, kWorld, kIntersect, fLineParam,
                fTriParam1, fTriParam2, i);
            
            if (kPick.GetPickType() == NiPick::FIND_FIRST &&
                kPick.GetSortType() == NiPick::NO_SORT)
            {
                return true;
            }
        }
    }

    return bFound;
}
//---------------------------------------------------------------------------
bool FindIntersections_TriShapeFastPath(const NiPoint3& kOrigin,
    const NiPoint3& kDir, NiPick& kPick, NiTriShape* pkTBG)
{
    // Find ray's model space kOrigin and kDir.
    const NiTransform& kWorld = pkTBG->GetWorldTransform();
    NiPoint3 kDiff = kOrigin - kWorld.m_Translate;
    float fInvWorldScale = 1.0f / kWorld.m_fScale;
    NiPoint3 kModelOrigin = (kDiff * kWorld.m_Rotate) * fInvWorldScale;
    NiPoint3 kModelDir = (kDir * kWorld.m_Rotate);

    bool bFound = false;
    unsigned short usTris = pkTBG->GetActiveTriangleCount();

    NiPoint3* pVertex = pkTBG->GetVertices();
    if (!pVertex)
    {
        NiOutputDebugString(
            "Warning:  Geometry for NiPick operation is missing.\n"
            "  May need to:\n"
            "  a) Use KEEP flags with NiGeometryData::SetKeepFlags()\n"
            "     to keep the generic NiGeometryData attributes needed\n"
            "     for picking,\n"
            "       or\n"
            "  b) Export the data to keep the NiGeometryData members in\n"
            "     the Nif file.\n");
        return false;
    }

    NiTriShapeData* pkData = (NiTriShapeData*)pkTBG->GetModelData();

#if defined (_PS3) && defined(ENABLE_PARALLEL_PICK)

    bool bFirstHitOnly = 
        kPick.GetPickType() == NiPick::FIND_FIRST && 
        kPick.GetSortType() == NiPick::NO_SORT;

    NiCollisionWorkflow& kWorkflow = NiCollisionWorkflow::GetCurrentWorkflow();
    kWorkflow.Initialize(kModelOrigin, kModelDir, 
        kPick.GetFrontOnly(), bFirstHitOnly,
        pkData->GetTriList(), usTris, 
        (NiUInt64)pVertex, pkTBG, &kPick);

    NiStreamProcessor::Get()->Submit(&kWorkflow, NiStreamProcessor::MEDIUM);
#else
    for (unsigned short i = 0; i < usTris; i++)
    {
#if NIMETRICS
        kPick.m_uiTriComparisons++;
#endif
        unsigned short i0, i1, i2;
        pkData->GetTriangleIndices_Inline(i, i0, i1, i2);
        
        NiPoint3 kIntersect;
        float fLineParam, fTriParam1, fTriParam2;
        
        bool bIntersects = NiCollisionUtils::IntersectTriangle(
            kModelOrigin, kModelDir, pVertex[i0], pVertex[i1], pVertex[i2],
            kPick.GetFrontOnly(), kIntersect, fLineParam, fTriParam1,
            fTriParam2);
        
        if (bIntersects)
        {
            bFound = true;
            UpdatePickRecord(kPick, pkTBG, kWorld, kIntersect, fLineParam,
                fTriParam1, fTriParam2, i);
            
            if (kPick.GetPickType() == NiPick::FIND_FIRST &&
                kPick.GetSortType() == NiPick::NO_SORT)
            {
                return true;
            }
        }
    }
#endif

    return bFound;
}
//---------------------------------------------------------------------------
// On PS3, this method will be called each time a hit is found.
#if defined (_PS3) && defined(ENABLE_PARALLEL_PICK)
bool FindIntersections_TriShapeFastPathEnd(NiCollisionWorkflow& kFlow, 
    NiUInt32 i)
{
    NiPoint3 kModelDir(kFlow.m_kModelDir_v[0], 
                kFlow.m_kModelDir_v[1], kFlow.m_kModelDir_v[2]);
    NiPoint3  kModelOrigin(kFlow.m_kModelOrigin_v[0], 
                kFlow.m_kModelOrigin_v[1], kFlow.m_kModelOrigin_v[2]);
    
    NiPick*     pkPick = kFlow.m_pkPick;
    NiTriShape* pkTBG = kFlow.m_pkTriShape;
        
    // If we have the regular vertex buffer then use it, 
    // otherwise use the unified memory.
    NiPoint3* pkVertex = pkTBG->GetVertices();
    if (!pkVertex)
    {
        return false;
    }
    
    NiTriShapeData* pkData = (NiTriShapeData*)pkTBG->GetModelData();
    NIASSERT(pkData);
    
    unsigned short i0, i1, i2;
    pkData->GetTriangleIndices_Inline(i, i0, i1, i2);
    
    NiPoint3 kVert0 = pkVertex[i0];
    NiPoint3 kVert1 = pkVertex[i1];
    NiPoint3 kVert2 = pkVertex[i2];

    NiPoint3 kIntersect;
    float fLineParam, fTriParam1, fTriParam2;

    bool bIntersects = NiCollisionUtils::IntersectTriangle(
        kModelOrigin, kModelDir, 
        kVert0, kVert1, kVert2,
        pkPick->GetFrontOnly(), 
        kIntersect, 
        fLineParam, fTriParam1, fTriParam2);
    if (bIntersects)
    {
        UpdatePickRecord(*pkPick, pkTBG, pkTBG->GetWorldTransform(), 
            kIntersect, fLineParam, fTriParam1, fTriParam2, i);
    }
    return true;
}
#endif

//---------------------------------------------------------------------------
void UpdatePickRecord(NiPick& kPick, NiTriBasedGeom* pkTBG, 
    const NiTransform& kWorld, NiPoint3& kIntersect, const float fLineParam, 
    const float fTriParam1, const float fTriParam2, 
    unsigned int uiTriangleIndex)
{
    NiPick::Record* pkRecord = kPick.Add(pkTBG);

    NiPoint3* pVertex = pkTBG->GetVertices();
    
    if (kPick.GetCoordinateType() == NiPick::WORLD_COORDINATES)
    {
        kIntersect = kWorld.m_fScale
            * (kWorld.m_Rotate * kIntersect) + kWorld.m_Translate;
    }
    pkRecord->SetIntersection(kIntersect);
    
    pkRecord->SetDistance(fLineParam * kWorld.m_fScale);
    pkRecord->SetTriangleIndex(uiTriangleIndex);
    unsigned short i0, i1, i2;
    pkTBG->GetTriangleIndices(uiTriangleIndex, i0, i1, i2);
    pkRecord->SetVertexIndices(i0, i1, i2);
    
    float fTriParam0 = 1.0f - (fTriParam1 + fTriParam2);
    
    if (kPick.GetReturnTexture())
    {
        NiPoint2* pkTexture = pkTBG->GetTextures();
        if (pkTexture)
        {
            NiPoint2 kTexture =
                fTriParam0 * pkTexture[i0] +
                fTriParam1 * pkTexture[i1] +
                fTriParam2 * pkTexture[i2];
            pkRecord->SetTexture(kTexture);
        }
        else
        {
            pkRecord->SetTexture(NiPoint2::ZERO);
        }
    }
    else
    {
        pkRecord->SetTexture(NiPoint2::ZERO);
    }
    
    if (kPick.GetReturnNormal())
    {
        NiPoint3 kNormal;
        NiPoint3* pkNormal = pkTBG->GetNormals();
        
        if (kPick.GetReturnSmoothNormal() && pkNormal)
        {
            kNormal =
                fTriParam0 * pkNormal[i0] +
                fTriParam1 * pkNormal[i1] +
                fTriParam2 * pkNormal[i2];
        }
        else
        {
            NiPoint3 v0, kV1;
            
            v0 = pVertex[i1] - pVertex[i0];
            kV1 = pVertex[i2] - pVertex[i0];
            kNormal = v0.Cross(kV1);
        }
        kNormal.Unitize();
        
        if (kPick.GetCoordinateType() == NiPick::WORLD_COORDINATES)
        {
            kNormal = kWorld.m_Rotate * kNormal;
        }
        
        pkRecord->SetNormal(kNormal);
    }
    else
    {
        pkRecord->SetNormal(NiPoint3::ZERO);
    }
    
    if (kPick.GetReturnColor())
    {
        NiColorA* pkColor = pkTBG->GetColors();
        if (pkColor)
        {
            NiColorA color =
                fTriParam0 * pkColor[i0] +
                fTriParam1 * pkColor[i1] +
                fTriParam2 * pkColor[i2];
            pkRecord->SetColor(color);
        }
        else
        {
            pkRecord->SetColor(NiColorA::WHITE);
        }
    }
    else
    {
        pkRecord->SetColor(NiColorA::WHITE);
    }
}
//---------------------------------------------------------------------------
int Find_NIBOUNDvsNIBOUND(float fDeltaTime, NiAVObject* pkObj1,
    NiAVObject* pkObj2, bool bCalcNormals, NiCollisionGroup::Intersect& kIntr,
    bool& bCollision)
{
    NiCollisionData* pkCD1 = NiGetCollisionData(pkObj1);
    NiCollisionData* pkCD2 = NiGetCollisionData(pkObj2);

   // Bounding volume will be converted to a sphereABV if possible.
    bool bResult = NiCollisionUtils::BoundingVolumeFindIntersect(fDeltaTime, 
        pkObj1, pkObj2, kIntr.fTime, kIntr.kPoint, bCalcNormals,
        kIntr.kNormal0, kIntr.kNormal1);

    if (bResult)
    {
        bCollision = true;

        // The rule of callbacks:  Since each object has its own callback, the
        // rule is that an object's callback is called only if the other
        // object's propagation flag is NEVER are ALWAYS.  I.e., it is not
        // conditional.
        return pkCD1->FindCollisionProcessing(kIntr,
            pkCD1->GetPropagationMode(), pkCD2->GetPropagationMode()); 
    }

    return NiCollisionGroup::CONTINUE_COLLISIONS;
}
//---------------------------------------------------------------------------
int Find_OBBvsOBB(float fDeltaTime, NiAVObject* pkObj1, NiAVObject* pkObj2,
    NiCollisionGroup::Record& kRecord0, NiCollisionGroup::Record& kRecord1,
    bool& bCollision)
{
    NiCollisionData* pkCD1 = NiGetCollisionData(pkObj1);
    NiCollisionData* pkCD2 = NiGetCollisionData(pkObj2);

    if (pkCD1->GetWorldVertices() == NULL)
        pkCD1->CreateWorldVertices();

    if (pkCD2->GetWorldVertices() == NULL)
        pkCD2->CreateWorldVertices();

    pkCD1->UpdateWorldVertices();
    pkCD2->UpdateWorldVertices();

    // Create OBB trees just in time.
    pkCD1->CreateOBB(kRecord0.GetBinSize());
    pkCD2->CreateOBB(kRecord1.GetBinSize());         

    return (pkCD1->FindOBBCollisions(fDeltaTime, pkCD2, kRecord0.GetRoot(),
        kRecord1.GetRoot(), kRecord0.GetAVObject(), kRecord1.GetAVObject(), 
        kRecord0.GetMaxDepth(), kRecord1.GetMaxDepth(), bCollision));
}
//---------------------------------------------------------------------------
int Find_OBBvsTRI(float fDeltaTime, NiAVObject* pkObj1, NiAVObject* pkObj2,
    NiCollisionGroup::Record& kRecord0, NiCollisionGroup::Record& kRecord1,
    bool& bCollision)
{
    NiCollisionData* pkCD1 = NiGetCollisionData(pkObj1);
    NiCollisionData* pkCD2 = NiGetCollisionData(pkObj2);

    if (pkCD1->GetWorldVertices() == NULL)
        pkCD1->CreateWorldVertices();

    if (pkCD2->GetWorldVertices() == NULL)
        pkCD2->CreateWorldVertices();

    pkCD1->UpdateWorldVertices();
    pkCD2->UpdateWorldVertices();

    // Create OBB trees just in time.
    pkCD1->CreateOBB(kRecord0.GetBinSize());
    pkCD2->CreateOBB(kRecord1.GetBinSize());         

    return (pkCD1->FindOBBCollisions(fDeltaTime, pkCD2, kRecord0.GetRoot(),
        kRecord1.GetRoot(), kRecord0.GetAVObject(), kRecord1.GetAVObject(), 
        kRecord0.GetMaxDepth(), kRecord1.GetMaxDepth(), bCollision));
}
//---------------------------------------------------------------------------
// There is no code to handle OBB vs. ABV collisions directly.  The strategy
// employed is:  if OBBs can easily be obtained for both, use them; otherwise
// use the triangles directly against the ABV.
int Find_OBBvsABV(float fDeltaTime, NiAVObject* pkObj1, NiAVObject* pkObj2,
    NiCollisionGroup::Record& kRecord0, NiCollisionGroup::Record& kRecord1,
    bool bNormals, NiCollisionGroup::Intersect& kIntr, bool &bCollision)
{
    NiCollisionData* pkCD1 = NiGetCollisionData(pkObj1);
    NiCollisionData* pkCD2 = NiGetCollisionData(pkObj2);

    if (pkCD1->GetWorldVertices() && pkCD2->GetWorldVertices())
    {
        pkCD1->UpdateWorldVertices();
        pkCD2->UpdateWorldVertices();

        // Create OBB trees just in time.
        pkCD1->CreateOBB(kRecord0.GetBinSize());
        pkCD2->CreateOBB(kRecord1.GetBinSize());         

        return (pkCD1->FindOBBCollisions(fDeltaTime, pkCD2,
            kRecord0.GetRoot(), kRecord1.GetRoot(), kRecord0.GetAVObject(),
            kRecord1.GetAVObject(), kRecord0.GetMaxDepth(),
            kRecord1.GetMaxDepth(), bCollision));
    }

    return NiCollisionUtils::TriToBndVolFindIntersect(fDeltaTime, pkObj1,
        pkObj2, true, kIntr, bCollision);
}
//---------------------------------------------------------------------------
int Find_TRIvsTRI(float fDeltaTime, NiAVObject* pkObj1, NiAVObject* pkObj2,
    bool bNormals, NiCollisionGroup::Intersect& kIntr, bool& bCollision)
{
    NiCollisionData* pkCD1 = NiGetCollisionData(pkObj1);
    NiCollisionData* pkCD2 = NiGetCollisionData(pkObj2);

    if (pkCD1->GetWorldVertices() == NULL)
    {
        pkCD1->CreateWorldVertices();
    }

    if (pkCD2->GetWorldVertices() == NULL)
    {
        pkCD2->CreateWorldVertices();
    }

    pkCD1->UpdateWorldVertices();
    pkCD2->UpdateWorldVertices();

    return NiCollisionUtils::TriTriFindIntersect(fDeltaTime, pkObj1, pkObj2,
        true, kIntr, bCollision);
}
//---------------------------------------------------------------------------
int Find_ABVvsABV(float fDeltaTime, NiAVObject* pkObj1, NiAVObject* pkObj2,
    bool bCalcNormals, NiCollisionGroup::Intersect& kIntr, bool& bCollision)
{
    NiCollisionData* pkCD1 = NiGetCollisionData(pkObj1);
    NiCollisionData* pkCD2 = NiGetCollisionData(pkObj2);

    bool bResult = NiCollisionUtils::BoundingVolumeFindIntersect(fDeltaTime, 
        pkObj1, pkObj2, kIntr.fTime, kIntr.kPoint, bCalcNormals,
        kIntr.kNormal0, kIntr.kNormal1);

    if (bResult)
    {
        bCollision = true;

        // The rule of callbacks:  Since each object has its own callback, the
        // rule is that an object's callback is called only if the other
        // object's propagation flag is NEVER are ALWAYS.  I.e., it is not
        // conditional.
        return pkCD1->FindCollisionProcessing(kIntr,
            pkCD1->GetPropagationMode(), pkCD2->GetPropagationMode()); 
    }

    return NiCollisionGroup::CONTINUE_COLLISIONS;
}
//---------------------------------------------------------------------------
int Find_TRIvsABV(float fDeltaTime, NiAVObject* pkObj1, NiAVObject* pkObj2,
    bool bCalcNormals, NiCollisionGroup::Intersect& kIntr, bool &bCollision)
{
    // Because we may want to exit on the first triangle collision detected, 
    // we enable this function to do callbacks
    return NiCollisionUtils::TriToBndVolFindIntersect(fDeltaTime, pkObj1,
        pkObj2, true, kIntr, bCollision);
}
//---------------------------------------------------------------------------
int Test_NIBOUNDvsNIBOUND(float fDeltaTime, NiAVObject* pkObj1,
    NiAVObject* pkObj2, bool& bCollision)
{
   // Bounding volume will be converted to a SphereABV if possible.
   bool bResult = NiCollisionUtils::BoundingVolumeTestIntersect(fDeltaTime,
       pkObj1, pkObj2); 

    if (bResult)
    {
        bCollision = true;

        NiCollisionData* pkCD1 = NiGetCollisionData(pkObj1);
        NiCollisionData* pkCD2 = NiGetCollisionData(pkObj2);

        NIASSERT(pkCD1);
        NIASSERT(pkCD2);

        NiCollisionData::PropagationMode ePropagationMode1
            = pkCD1->GetPropagationMode();
        NiCollisionData::PropagationMode ePropagationMode2
            = pkCD2->GetPropagationMode();

        if (pkCD1->TestCollisionProcessing(ePropagationMode1,
            ePropagationMode2) == NiCollisionGroup::TERMINATE_COLLISIONS)
        {
            return NiCollisionGroup::TERMINATE_COLLISIONS;
        }
    }

    return NiCollisionGroup::CONTINUE_COLLISIONS;
}
//---------------------------------------------------------------------------
bool Test_OBBvsOBB(float fDeltaTime, NiAVObject* pkObj1, NiAVObject* pkObj2, 
    NiCollisionGroup::Record& kRecord0, NiCollisionGroup::Record& kRecord1)
{
    NiCollisionData* pkCD1 = NiGetCollisionData(pkObj1);
    NiCollisionData* pkCD2 = NiGetCollisionData(pkObj2);

    if (pkCD1->GetWorldVertices() == NULL)
        pkCD1->CreateWorldVertices();

    if (pkCD2->GetWorldVertices() == NULL)
        pkCD2->CreateWorldVertices();

    pkCD1->UpdateWorldVertices();
    pkCD2->UpdateWorldVertices();

    // Create OBB trees just in time.
    pkCD1->CreateOBB(kRecord0.GetBinSize());
    pkCD2->CreateOBB(kRecord1.GetBinSize());         

    return (pkCD1->TestOBBCollisions(fDeltaTime, pkCD2,
        kRecord0.GetAVObject(), kRecord1.GetAVObject(),
        kRecord0.GetMaxDepth(), kRecord1.GetMaxDepth()));
}
//---------------------------------------------------------------------------
bool Test_OBBvsTRI(float fDeltaTime, NiAVObject* pkObj1, NiAVObject* pkObj2, 
    NiCollisionGroup::Record& kRecord0, NiCollisionGroup::Record& kRecord1)
{
    NiCollisionData* pkCD1 = NiGetCollisionData(pkObj1);
    NiCollisionData* pkCD2 = NiGetCollisionData(pkObj2);

    if (pkCD1->GetWorldVertices() == NULL)
        pkCD1->CreateWorldVertices();

    if (pkCD2->GetWorldVertices() == NULL)
        pkCD2->CreateWorldVertices();

    pkCD1->UpdateWorldVertices();
    pkCD2->UpdateWorldVertices();

    pkCD1->CreateOBB(kRecord0.GetBinSize()); // Create OBB trees just in time.
    pkCD2->CreateOBB(kRecord1.GetBinSize());         

    return (pkCD1->TestOBBCollisions(fDeltaTime, pkCD2, 
        kRecord0.GetAVObject(), kRecord1.GetAVObject(), 
        kRecord0.GetMaxDepth(), kRecord1.GetMaxDepth()));
}
//---------------------------------------------------------------------------
// There is no code to handle OBB vs. ABV collisions directly.  The strategy
// employed is:  if OBBs can easily be obtained for both, use them; otherwise
// use the triangles directly against the ABV.
int Test_OBBvsABV(float fDeltaTime, NiAVObject* pkObj1, NiAVObject* pkObj2,
    NiCollisionGroup::Record& kRecord0, NiCollisionGroup::Record& kRecord1,
    bool& bCollision)
{
    NiCollisionData* pkCD1 = NiGetCollisionData(pkObj1);
    NiCollisionData* pkCD2 = NiGetCollisionData(pkObj2);

    if (pkCD1->GetWorldVertices() && pkCD2->GetWorldVertices())
    {
        pkCD1->UpdateWorldVertices();
        pkCD2->UpdateWorldVertices();

        // Create OBB trees just in time.
        pkCD1->CreateOBB(kRecord0.GetBinSize());
        pkCD2->CreateOBB(kRecord1.GetBinSize());         

        return (pkCD1->TestOBBCollisions(fDeltaTime, pkCD2, 
            kRecord0.GetAVObject(), kRecord1.GetAVObject(), 
            kRecord0.GetMaxDepth(), kRecord1.GetMaxDepth()));
    }

    return NiCollisionUtils::TriToBndVolTestIntersect(fDeltaTime, pkObj1,
        pkObj2, bCollision); 
}
//---------------------------------------------------------------------------
int Test_TRIvsTRI(float fDeltaTime, NiAVObject* pkObj1, NiAVObject* pkObj2,
    bool& bCollision)
{
    NiCollisionData* pkCD1 = NiGetCollisionData(pkObj1);
    NiCollisionData* pkCD2 = NiGetCollisionData(pkObj2);

    if (pkCD1->GetWorldVertices() == NULL)
    {
        pkCD1->CreateWorldVertices();
    }

    if (pkCD2->GetWorldVertices() == NULL)
    {
        pkCD2->CreateWorldVertices();
    }

    pkCD1->UpdateWorldVertices();
    pkCD2->UpdateWorldVertices();

    return NiCollisionUtils::TriTriTestIntersect(fDeltaTime, pkObj1, pkObj2,
        bCollision);
}
//---------------------------------------------------------------------------
int Test_TRIvsABV(float fDeltaTime, NiAVObject* pkObj1, NiAVObject* pkObj2,
    bool &bCollision)
{
    return NiCollisionUtils::TriToBndVolTestIntersect(fDeltaTime, pkObj1,
        pkObj2, bCollision);
}
//---------------------------------------------------------------------------
int Test_ABVvsABV(float fDeltaTime, NiAVObject* pkObj1, NiAVObject* pkObj2,
    bool& bCollision)                   
{
    bool bResult = NiCollisionUtils::BoundingVolumeTestIntersect(fDeltaTime,
        pkObj1, pkObj2);    

    if (bResult)
    {
        bCollision = true;

        NiCollisionData* pkCD1 = NiGetCollisionData(pkObj1);
        NiCollisionData* pkCD2 = NiGetCollisionData(pkObj2);

        NIASSERT(pkCD1);
        NIASSERT(pkCD2);

        NiCollisionData::PropagationMode ePropagationMode1
            = pkCD1->GetPropagationMode();
        NiCollisionData::PropagationMode ePropagationMode2 
            = pkCD2->GetPropagationMode();

        if (pkCD1->TestCollisionProcessing(ePropagationMode1,
            ePropagationMode2) == NiCollisionGroup::TERMINATE_COLLISIONS)
        {
            return NiCollisionGroup::TERMINATE_COLLISIONS;
        }
    }

    return NiCollisionGroup::CONTINUE_COLLISIONS;
}
//---------------------------------------------------------------------------
} // namespace
