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

#ifndef NIPHYSXEXTRACTORPLUGIN_H
#define NIPHYSXEXTRACTORPLUGIN_H

class NiPhysXNxVec3HashFunctor : public NiMemObject
{
public:
    static unsigned int KeyToHashIndex(NxVec3* key, unsigned int uiTableSize);
};

class NiPhysXNxVec3EqualsFunctor : public NiMemObject
{
public:
    static bool IsKeysEqual(NxVec3* key1, NxVec3* key2);
};


class NiPhysXExtractorPlugin : public NiPlugin
{
public:
    /// RTTI declaration macro.
    NiDeclareRTTI;

    /// Default constructor.
    NiPhysXExtractorPlugin();
    
    // Destructor
    ~NiPhysXExtractorPlugin();

    //-----------------------------------------------------------------------
    // Required virtual function overrides from NiPlugin.
    //-----------------------------------------------------------------------
    /// Returns a plug-in script with default parameters.
    virtual NiPluginInfo* GetDefaultPluginInfo(); 

    /// Does this plug-in have editable options?
    virtual bool HasManagementDialog();
    
    /// Bring up the options dialog and handle user interaction.
    virtual bool DoManagementDialog(NiPluginInfo* pkInitialInfo,
        NiWindowRef hWndParent);

    /// Determines whether or not this plug-in can handle this plug-in info
    /// object.
    virtual bool CanExecute(NiPluginInfo* pkInfo);
    
    /// Execute the NiPluginInfo script.
    virtual NiExecutionResultPtr Execute(const NiPluginInfo* pkInfo);

    // Compute a hash key for vertices
    static unsigned int KeyToHashIndex(NxVec3 key, unsigned int uiTableSize);

protected:
    // Scene creation
    NiPhysXProp* CreateRBProp(const NiPluginInfo* pkInfo, NiNode* pkSceneRoot);

    // Rigid bodies
    bool ExtractRigidBodiesAndCloth(NiPhysXProp* pkProp,
        NiPhysXPropDesc* pkPropDesc, NiAVObject* pkAVObj,
        NxCookingInterface* pkCookInterface, float fSkinWidth,
        bool bHardware, bool bInflate, bool bUncompressed, bool bFluid);
    bool ExtractRigidBody(NiPhysXProp* pkProp, NiPhysXPropDesc* pkPropDesc,
        NiAVObject* pkAVObj, int iActorType,
        NxCookingInterface* pkCookInterface, float fSkinWidth, bool bHardware,
        bool bInflate, bool bUncompressed, bool bFluid);
    NiPhysXActorDesc* ExtractActor(NiPhysXPropDesc* pkPropDesc,
        NiAVObject* pkAVObj, int iActorType,
        NxCookingInterface* pkCookInterface, float fInvScale, float fSkinWidth,
        bool bHardware, bool bInflate, bool bUncompressed, bool bFluid);
    bool ExtractShape(NiPhysXPropDesc* pkPropDesc, NiAVObject* pkAVObj,
        NiTObjectArray<NiPhysXShapeDescPtr>& kShapesArray,
        bool bCompoundPiece, NiTransform& kGlobalXform,
        NxMaterialDesc &kMatDesc, NxCookingInterface* pkCookInterface,
        float fInvScale, float fSkinWidth, bool bHardware, bool bInflate,
        bool bUncompressed, bool bFluid, bool bStatic);
    NiPhysXShapeDesc* ExtractGeometry(NiAVObject* pkAVObj,
        NxCookingInterface* pkCookInterface, float fInvScale, bool bHardware,
        bool bInflate, bool bUncompressed, bool bFluid, bool bStatic);

    // Cloth
    bool ExtractCloth(NiPhysXProp* pkProp, NiPhysXPropDesc* pkPropDesc,
        NiAVObject* pkAVObj, NxCookingInterface* pkCookInterface,
        bool bHardware);
    bool ExtractClothDesc(NiPhysXClothDesc*& pkClothDescReturn,
        NiTriShape*& pkTriShapeReturn, NiAVObject* pkAVObj,
        NxCookingInterface* pkCookInterface, bool bHardware, float fInvScale);
    bool ExtractClothMesh(NiPhysXClothDesc* pkClothDesc,
        NiTriShape*& pkTriShapeReturn, NiAVObject* pkAVObj,
        NxCookingInterface* pkCookInterface, float fInvScale, bool bHardware);
    void ProcessAttachments(NiPhysXClothDesc* pkClothDesc, NxVec3& kWorldPt,
        unsigned short us);
    bool CheckClothAttachments(NiPhysXClothDesc* pkClothDesc);

    bool PostProcessActors();

    // Joints
    bool ExtractJoints(NiPhysXPropDesc* pkPropDesc, float fInvScale);
    bool  ExtractMaxJoint(NiPhysXPropDesc* pkPropDesc, float fInvScale,
        NiAVObject* pkChildNode, unsigned int uiChildIndex,
        NiAVObject* pkParentNode, unsigned int uiParentIndex,
        NxD6JointDesc& kJointDesc);
    bool ExtractD6Joint(NiPhysXPropDesc* pkPropDesc, float fInvScale,
        NiAVObject* pkChildNode, unsigned int uiChildIndex,
        NiTransform& kParentXform, NxD6JointDesc& kJointDesc);
    
    // Particle system conversion
    bool FindAndConvertPSys(NiPhysXProp* pkRBProp,
        NiPhysXPSysProp*& pkProp, NiPhysXMeshPSysProp*& pkMeshProp,
        NiAVObject* pkAVObj, bool bToFluid,
        NiTPointerMap<NiAVObject*, NiAVObject*>& kMap,
        bool bPhysXSpace, bool bPhysXRotates);
    NiPhysXParticleSystemPtr ConvertPSysToPSys(NiPhysXPSysProp* pkProp,
        NiParticleSystem* pkPSys, bool bPhysXSpace);
    NiPhysXMeshParticleSystemPtr ConvertPSysToMeshPSys(
        NiPhysXMeshPSysProp* pkProp, NiMeshParticleSystem* pkPSys,
        bool bPhysXSpace, bool bPhysXRotates);
    void EnactNodeSwitch(NiAVObject* pkObject,
        NiTPointerMap<NiAVObject*, NiAVObject*>& kMap);
        
    // Set up shape maps
    void CreateMaps(NiAVObject* pkAVObj);
    
    // Clear everything ready for another run
    void Reset();
    
    // Determine if a parent really is a parent.
    bool ParentIsSceneGraphParent(NiAVObject* pkParentNode,
        NiAVObject* pkChildNode);
        
    // Stash version info in cooked meshes
    void StashMeshVersion(unsigned char* pucData);

    // Prime numbers for hash table sizing
    static const int NUM_PRIMES;
    static const unsigned int PRIMES[18];
    static unsigned int NextPrime(const unsigned int target);

    // Some of the maps hold arrays of objects
    typedef NiTObjectArray<NiAVObjectPtr> NiAVObjectArray;
    
    // All nodes that are actors
    NiTMap<NiAVObject*, NiPhysXActorDescPtr> m_kActorMap;
    
    // All nodes that are cloth
    NiTMap<NiAVObject*, NiPhysXClothDescPtr> m_kClothMap;

    // All objects defined as the source of a proxy shape
    NiTMap<unsigned int, NiAVObject*> m_kProxyMap;
    
    // All the objects making up a compound shape
    NiTMap<unsigned int, NiAVObjectArray*> m_kShapeGroupMap;
    
    // The basic shapes in the scene
    NiTMap<unsigned int, NiAVObject*> m_kBasicShapeMap;
    
    // All the nodes making up some part of a MultiMaterial mesh
    NiTMap<unsigned int, NiAVObjectArray*> m_kMeshGroupMap;
    
    // All nodes acting as particle system actors
    NiTStringMap<NiPhysXActorDesc*> m_kParticleMap;
    
    // All the shared collision mesh data
    NiTMap<unsigned int, NiPhysXShapeDesc*> m_kMeshesMap;
    NiTMap<unsigned int, NiPhysXMeshDesc*> m_kClothMeshesMap;

    // All the attachments for cloth to shapes
    NiTMap<unsigned int, NiPhysXClothDesc::ClothAttachment*> m_kAttachmentMap;

    // Joints
    typedef struct {
        NiAVObject* pkParentNode;
        unsigned int uiParentIndex;
        NiAVObject* pkChildNode;
        unsigned int uiChildIndex;
    } JointData;

    NiTMap<unsigned int, JointData*> m_kJointsMap;
    
    // Material index counter.
    NxMaterialIndex m_usNextMaterial;
    
    // Error logger
    NiXMLLogger* m_pkLogger;
};

#include "NiPhysXExtractorPlugin.inl"

#endif  // #ifndef NIPHYSXEXTRACTORPLUGIN_H
