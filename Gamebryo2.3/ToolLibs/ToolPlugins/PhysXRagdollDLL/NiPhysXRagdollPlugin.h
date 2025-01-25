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

#ifndef NIPHYSXRAGDOLLPLUGIN_H
#define NIPHYSXRAGDOLLPLUGIN_H

/// This is our sample process plug-in. It currently has no functionality
/// and is meant to be a template from which customers can create their own
/// process plug-ins.
class NiPhysXRagdollPlugin : public NiPlugin
{
public:
    /// RTTI declaration macro.
    NiDeclareRTTI;

    /// Default constructor.
    NiPhysXRagdollPlugin();
    
    // Destructor
    ~NiPhysXRagdollPlugin();

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

protected:
    // Do the work
    bool AttachRagdoll(NiNode* pkSkeleton, NiAVObject* pkRagdoll,
         NiString& kPrefix, NiString& kPostfix, const float fInvScale);
    void SwitchTarget(NiPhysXActorDesc* pkActorDesc, NiAVObject* pkNewTarget,
        const float fInvScale);
    void SetJointParents();

    void RemoveExcessNodes();
    
    // Set up actor -> node maps
    void CreateMaps(NiPhysXPropDesc* pkPropDesc);
    
    // Clear everything ready for another run
    void Reset();
    
    // All actor descriptors indexed by their target
    NiTMap<NiAVObject*, NiPhysXActorDesc*> m_kActorMap;
    
    // All joints, indexed by actor descriptor
    typedef NiTPrimitiveArray<NiPhysXJointDesc*> JointArray;
    NiTMap<NiPhysXActorDesc*, JointArray*> m_kJointMap;
    
    // The nodes that have been switched, and can hence be removed
    NiTPrimitiveArray<NiAVObject*> m_kRemappedNodes;

    // Error logger
    NiXMLLogger* m_pkLogger;
};

#endif  // #ifndef NIPHYSXRAGDOLLPLUGIN_H
