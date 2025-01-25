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

// Precompiled Headers
#include "MayaPluginPCH.h"

#include "NiMeshParticleSystem.h"
#include "NiParticleInfo.h"
#include "NiPSysMeshUpdateModifier.h"
#include "NiPSysUpdateCtlr.h"
#include "NiPSysBoxEmitter.h"
#include "NiPSysEmitterCtlr.h"
#include "NiPSysAgeDeathModifier.h"
#include "NiPSysPositionModifier.h"
#include "NiPSysColorModifier.h"
#include "NiPSysGrowFadeModifier.h"
#include "NiPSysRotationModifier.h"
#include "NiPSysGravityModifier.h"
#include "NiPSysColliderManager.h"
#include "NiPSysSphericalCollider.h"
#include "NiPSysPlanarCollider.h"
#include "NiPSysBoundUpdateModifier.h"
#include "NiPSysEmitterSpeedCtlr.h"
#include "NiPSysTurbulenceFieldModifier.h"
#include "NiPSysSpawnModifier.h"
#include "NiPSysDragFieldModifier.h"
#include "NiPSysResetOnLoopCtlr.h"
#include "NiPSysSphereEmitter.h"
#include "NiPSysCylinderEmitter.h"
#include "NiPSysMeshEmitter.h"
#include "NiPsysGravityFieldModifier.h"
#include "NiPSysRadialFieldModifier.h"
#include "NiPSysInitialRotSpeedCtlr.h"
#include "NiPSysInitialRotSpeedVarCtlr.h"
#include "NiPSysInitialRotAngleCtlr.h"
#include "NiPSysInitialRotAngleVarCtlr.h"

#include "MyiParticleSystem.h"
#include "MyiMaterialmanager.h"
#include "MyiTextureManager.h"
#include "MyiTriShape.h" 

//---------------------------------------------------------------------------
MyiParticleSystem::MyiParticleSystem(int iParticlesID, 
        MyiMaterialManager* pkMaterialManager, 
        MyiTextureManager* pkTextureManager)
{
    m_pMDtEmitter = NULL;
    m_pMDtParticles = NULL;
    m_pkCollisionSpawn = NULL;
    m_pkCollisionParticleSystem = NULL;
    m_bDieAtCollision = false;
    m_iNumEmittedParticles = 0;
    m_iNumSplitParticles = 0;
    m_fCollisionSpread = 0.0f;


    // Get the MDt ID's for particle system and emitter
    m_iPSystemParticlesID = iParticlesID;
    m_pMDtParticles = gParticleSystemManager.GetParticles(
        m_iPSystemParticlesID);
    
    m_iPSystemEmitterID = gParticleSystemManager.GetEmitter(
        m_pMDtParticles->m_MObjectEmitter);
    
    // Check to see if we can find an emitter
    if (m_iPSystemEmitterID >= 0)
    {
        m_pMDtEmitter = gParticleSystemManager.GetEmitter( 
            m_iPSystemEmitterID );
    }


    //
    // Construct the particles.
    //
    int iNumParticles = GetNumParticles();


    // Create the Particles
    if (!CreateGeometryParticles(iNumParticles))
    {
        if (!CreateAutoNormalParticles(iNumParticles))
        {
            m_bFail = true;
            return;
        }
    }

    CreateUpdateController();


    CreateZBufferProperties();

        // Create the Material and Texture used by the particles
    CreateMaterialAndTexture(pkMaterialManager, pkTextureManager);

    CreateEmitter();

    CreateEmitterController();

    CreateResetOnLoop();

    CreateEmitterSpeedController();

    CreatePositionModifier();

    CreateColorModifier();

    CreateGrowFadeModifier();

    CreateRotationModifier();

    CreateInitialRotationSpeedCtlr();

    CreateInitialRotationSpeedVarCtlr();

    CreateRotationAngleCtlr();

    CreateRotationAngleVarCtlr();

    CreateBoundUpdateModifier();

    CreateForces();
}
//---------------------------------------------------------------------------
MyiParticleSystem::~MyiParticleSystem()
{

}
//---------------------------------------------------------------------------
NiParticleSystem* MyiParticleSystem::GetParticleSystem()
{
    return m_pkParticleSystem;
}
//---------------------------------------------------------------------------
void MyiParticleSystem::PostProcess(
    NiTList<MyiParticleSystem*>& pkParticleSystems)
{
    // These need to be constructed after the scene graph is complete
    CreateAgeDeathModifier(pkParticleSystems);

    CreateColliders(pkParticleSystems);

    ConvertGravityDirectionToWorldSpace();

    ConvertDragDirectionToWorldSpace();

    SetSphereColliderNodePosistionAndScale();

    SetPlanarColliderWidthAndHeightFromGeometry();

    DeleteCollisionGeometry();

    CloneMeshParticles();
}
//---------------------------------------------------------------------------
bool MyiParticleSystem::MatchesParticleShapeName(MString kShapeName)
{
    MFnDagNode kShape(m_pMDtParticles->m_MObjectParticles);
    
    return (kShape.name() == kShapeName);
}
//---------------------------------------------------------------------------
bool MyiParticleSystem::MatchesParticleName(MString kName)
{
    MFnDagNode kParticle(m_pMDtParticles->m_MObjectParent);
    
    return (kParticle.name() == kName);
}
//---------------------------------------------------------------------------
void MyiParticleSystem::GetLifeSpan(float& fLifeSpan, float& fLifeSpanVar)
{

    switch (m_pMDtParticles->m_lLifespanMode)
    {
    case kMDtParticles::LIVE_FOREVER:
        fLifeSpan = NI_INFINITY;
        break;

    case kMDtParticles::CONSTANT:
        fLifeSpan = m_pMDtParticles->m_fLifespan;
        break;

    case kMDtParticles::RANDOM_RANGE:
        fLifeSpan = m_pMDtParticles->m_fLifespan;
        fLifeSpanVar = m_pMDtParticles->m_fLifespanRandom;
        break;

    case kMDtParticles::LIFESPAN_PP_ONLY:
        DtExt_Err("Error:: Particles %s has Lifespan Mode Lifespan PP Only "
            "is unsupported\n", m_pMDtParticles->m_szName );

        fLifeSpan = NI_INFINITY;
        break;

    default:
        DtExt_Err("Error:: Particles %s has an invalid Lifespan Mode\n", 
            m_pMDtParticles->m_szName );

        fLifeSpan = NI_INFINITY;
        break;
    }
}
//---------------------------------------------------------------------------
int MyiParticleSystem::GetNumParticles()
{
    int iNumParticles = (int)m_pMDtParticles->m_fMaxCount;

        // CHECK FOR INVALID NUMBER OF PARTICLES
    if (iNumParticles < 0)
    {
        DtExt_Err("Error:: Particles:%s has an invalid Max Count of %d using "
            "a default of 100\n", m_pMDtParticles->m_szName, 
            m_pMDtParticles->m_fMaxCount);

        iNumParticles = 100;
    }

    return iNumParticles;
}
//---------------------------------------------------------------------------
bool MyiParticleSystem::CreateGeometryParticles(int iNumParticles)
{

       // Get the Particle Shape
    MStatus kStatus;
    MObject MParticleShape = m_pMDtParticles->m_MObjectParticles;

    MObjectArray kParticleMeshObjects;


    // Look for an Instancer attached to the Particles
    MFnDependencyNode kShape(MParticleShape, &kStatus);
    NIASSERT(kStatus == MS::kSuccess);

    MPlug kInstanceData = kShape.findPlug("instanceData", &kStatus);
    if (kStatus != MS::kSuccess) 
        return false;

    unsigned int uiNumInstancers = kInstanceData.numElements(&kStatus);
    NIASSERT(kStatus == MS::kSuccess);

    // Loop through the Instance Data connections
    unsigned int uiLoop;
    for (uiLoop = 0; uiLoop < uiNumInstancers; uiLoop++)
    {
        MPlug kInstancePlug = kInstanceData.elementByLogicalIndex(uiLoop, 
            &kStatus);

        NIASSERT(kStatus == MS::kSuccess);

        // Look in the point data plug to find the instancers it is 
        // connected with.
        MPlug kInstancePointData = kInstancePlug.child(1, &kStatus);
        NIASSERT(kStatus == MS::kSuccess);

        MPlugArray kConnections;
        
        kInstancePointData.connectedTo(kConnections, false, true, &kStatus);
        NIASSERT(kStatus == MS::kSuccess);

        // Follow the connections to the Instancer nodes
        unsigned int uiConnection;
        for (uiConnection = 0; uiConnection < kConnections.length(); 
            uiConnection++)
        {
            MObject kInstancerObject = 
                kConnections[uiConnection].node(&kStatus);

            NIASSERT(kStatus == MS::kSuccess);

            MFnDependencyNode kInstancer(kInstancerObject, &kStatus);
            NIASSERT(kStatus == MS::kSuccess);

            MPlug kInputHierarchyArray = kInstancer.findPlug(
                MString("inputHierarchy"), &kStatus);

            NIASSERT(kStatus == MS::kSuccess);
            NIASSERT(kInputHierarchyArray.isArray(&kStatus) == true);
            
            unsigned int uiPLoop;
            for (uiPLoop = 0; 
                uiPLoop < kInputHierarchyArray.numElements(&kStatus); 
                uiPLoop++)
            {
                MPlug kInputHierarchy = 
                    kInputHierarchyArray.elementByLogicalIndex(uiPLoop, 
                        &kStatus);

                NIASSERT(kStatus == MS::kSuccess);
                
                MPlugArray kConnectedObjects;
                
                kInputHierarchy.connectedTo(kConnectedObjects, true, false, 
                    &kStatus);

                NIASSERT(kStatus == MS::kSuccess);
                
                // Follow the connections to the Instancer nodes
                unsigned int uiOLoop;
                for (uiOLoop = 0; uiOLoop < kConnectedObjects.length(); 
                    uiOLoop++)
                {
                    MObject kParticleMeshObject = 
                        kConnectedObjects[uiOLoop].node(&kStatus);

                    NIASSERT(kStatus == MS::kSuccess);
                    
                    // Append this object to the list of particle objects
                    kParticleMeshObjects.append(kParticleMeshObject);
                }
            }
        }
    }


    if (kParticleMeshObjects.length() == 0)
        return false;


    // Create the Particle Meshes

    // Check for the world space tag
    bool bWorldSpace = false;

    MFnDependencyNode kParticles(m_pMDtParticles->m_MObjectParticles, 
        &kStatus);

    GetExtraAttribute(kParticles, "Ni_ParticlesInWorldSpace", false, 
        bWorldSpace);

    int iPoolSize = 01;
    GetExtraAttribute(kParticles, "Ni_ParticleMeshDefaultPoolSize", false, 
        iPoolSize);

    bool bFillPoolsOnLoad = false;
    GetExtraAttribute(kParticles, "Ni_ParticleMeshFillPoolsOnLoad", false, 
        bFillPoolsOnLoad);

    NiMeshParticleSystem* pkMeshParticleSystem = NiNew NiMeshParticleSystem(
        iNumParticles, HasColors(), HasRotation(), bWorldSpace, 
        (unsigned int)iPoolSize, 1, bFillPoolsOnLoad);


    // Create the Particle Mesh Modifier
    NiPSysMeshUpdateModifier* pkModifier = NiNew NiPSysMeshUpdateModifier
        ("Mesh Update Modifier");
    pkModifier->SetSystemPointer(pkMeshParticleSystem);

    for (uiLoop = 0; uiLoop < kParticleMeshObjects.length(); uiLoop++)
    {
        // Set the Mest
        pkModifier->SetMeshAt(uiLoop, gUserData.GetNode(
            kParticleMeshObjects[uiLoop]));
    }

    pkMeshParticleSystem->AddModifier(pkModifier);

    m_pkParticleSystem = (NiParticleSystem*)pkMeshParticleSystem;

    // Set the Name
    m_pkParticleSystem->SetName(m_pMDtParticles->m_szName);

    return true;
}
//---------------------------------------------------------------------------
bool MyiParticleSystem::CreateAutoNormalParticles(int iNumParticles)
{
    // CREATE THE PARTICLES

    // Check for the world space tag
    bool bWorldSpace = false;
    MStatus kStatus;
    MFnDependencyNode kParticles(m_pMDtParticles->m_MObjectParticles, 
        &kStatus);

    NIASSERT(kStatus == MS::kSuccess);
    GetExtraAttribute(kParticles, "Ni_ParticlesInWorldSpace", false, 
        bWorldSpace);

    m_pkParticleSystem = NiNew NiParticleSystem(iNumParticles, HasColors(), 
        HasRotation(), bWorldSpace);

    // Set the Name
    m_pkParticleSystem->SetName(m_pMDtParticles->m_szName);
    return true;
}
//---------------------------------------------------------------------------
void MyiParticleSystem::CreateUpdateController()
{
    NiPSysUpdateCtlr* pkUpdate = NiNew NiPSysUpdateCtlr;

    pkUpdate->SetTarget(m_pkParticleSystem);
}
//---------------------------------------------------------------------------
void MyiParticleSystem::CreateEmitter()
{
    // Check for No Emitter
    if (m_pMDtEmitter == NULL)
        return;

    // Particle Life Span
    float fLifeSpan = 0.0f;
    float fLifeSpanVar = 0.0f;

    GetLifeSpan(fLifeSpan, fLifeSpanVar);

    //
    // Initialize the emitter's diminsions
    //
    float fEmitterWidth = 0.0f;
    float fEmitterHeight = 0.0f;
    float fEmitterDepth = 0.0f;

    float fSpeed = 0.0f;
    float fSpeedVar = 0.0f;
    float fDeclination = 0.0f;
    float fDeclinationVar = 0.0f;
    float fPlanarAngle = 0.0f;
    float fPlanarAngleVar = 0.0f;

    const char* pcName = "Emitter";
    NiAVObject* pkEmitterObj = gUserData.GetNode(
        m_pMDtParticles->m_MObjectEmitter);


    NiColorA kInitialColor = NiColorA::WHITE;
    float fInitialRadius = GetInitialSize();

    NiPSysEmitter* pkEmitter;

    switch (m_pMDtEmitter->m_lEmitterType)
    {
    case kMDtEmitter::DIRECTIONAL:
        pkEmitter = CreateDirectional();
        break;
    case kMDtEmitter::OMNI:
        pkEmitter = CreateOmni();
        break;
    case kMDtEmitter::CURVE:
        pkEmitter = CreateCurve();
        break;
    case kMDtEmitter::SURFACE:
        pkEmitter = CreateSurface();
        break;
    case kMDtEmitter::VOLUME:
        pkEmitter = CreateVolume();
        break;
    default:
        DtExt_Err("Error:: Emitter %s has an invalid Emitter Type\n", 
            m_pMDtEmitter->m_szName );
        return;
    }

    // Add the modifier to the particle system
    m_pkParticleSystem->AddModifier(pkEmitter);
}
//---------------------------------------------------------------------------
float MyiParticleSystem::GetParticleSizeVariance()
{
    MObject MParticleShape = m_pMDtParticles->m_MObjectParticles;

    MStatus kStatus;

    MFnDependencyNode kShape(MParticleShape, &kStatus);
    NIASSERT(kStatus == MS::kSuccess);

    MPlug kVariance = kShape.findPlug("Ni_ParticleSizeVariance", 
        &kStatus);

    if (kStatus != MS::kSuccess) 
        return 0.0f;

    float fVariance;
    if (kVariance.getValue(fVariance) != MS::kSuccess)
        return 0.0f;
    
    return fVariance;
}
//---------------------------------------------------------------------------
NiPSysEmitter* MyiParticleSystem::CreateDirectional()
{
    float fEmitterWidth = 0.0f;
    float fEmitterHeight = 0.0f;
    float fEmitterDepth = 0.0f;

    float fSpeed = 0.0f;
    float fSpeedVar = 0.0f;
    float fDeclination = 0.0f;
    float fDeclinationVar = 0.0f;
    float fPlanarAngle = 0.0f;
    float fPlanarAngleVar = 0.0f;

    float fSizeVariance = GetParticleSizeVariance();

    // Particle Life Span
    float fLifeSpan = 0.0f;
    float fLifeSpanVar = 0.0f;

    GetLifeSpan(fLifeSpan, fLifeSpanVar);

    // Set Particle Declination
    NiPoint3 Direction(m_pMDtEmitter->m_fDirectionX,
                       m_pMDtEmitter->m_fDirectionY,
                       m_pMDtEmitter->m_fDirectionZ);


    Direction.Unitize();

    fDeclination = NiACos(Direction.Dot(NiPoint3(0.0f, 0.0f, 1.0f)));

    fDeclinationVar = m_pMDtEmitter->m_fSpread * NI_HALF_PI;

    // Set the Particle Planar Angle
    NiPoint3 XYProjection(Direction.x, Direction.y, 0.0f);

    XYProjection.Unitize();

    fPlanarAngle = NiACos(XYProjection.Dot(NiPoint3(1.0f, 0.0f, 0.0f)));

    if (XYProjection.y < 0.0)
        fPlanarAngle = -fPlanarAngle;


    fPlanarAngleVar = m_pMDtEmitter->m_fSpread * NI_HALF_PI;

    // Set the particles speed
    fSpeed = m_pMDtEmitter->m_fSpeed * gExport.m_fLinearUnitMultiplier;

    fSpeedVar = m_pMDtEmitter->m_fSpeedRandom * 
        gExport.m_fLinearUnitMultiplier;


    NiAVObject* pkEmitterObj = gUserData.GetNode(
        m_pMDtParticles->m_MObjectEmitter);

    NiColorA kInitialColor = NiColorA::WHITE;
    float fInitialRadius = GetInitialSize();

            // Create the emitter modifier
    return NiNew NiPSysBoxEmitter("Emitter", 
            pkEmitterObj, fEmitterWidth, fEmitterHeight, fEmitterDepth, 
            fSpeed, fSpeedVar, fDeclination, fDeclinationVar, fPlanarAngle,
            fPlanarAngleVar, kInitialColor, fInitialRadius, fLifeSpan, 
            fLifeSpanVar, fSizeVariance);
}
//---------------------------------------------------------------------------
NiPSysEmitter* MyiParticleSystem::CreateOmni()
{
    float fSpeed = 0.0f;
    float fSpeedVar = 0.0f;
    float fDeclination = 0.0f;
    float fDeclinationVar = 0.0f;
    float fPlanarAngle = 0.0f;
    float fPlanarAngleVar = 0.0f;

    float fSizeVariance = GetParticleSizeVariance();

    // Particle Life Span
    float fLifeSpan = 0.0f;
    float fLifeSpanVar = 0.0f;

    GetLifeSpan(fLifeSpan, fLifeSpanVar);


    // Set Particle direction
    fDeclination = 0.0f;
    fDeclinationVar = NI_TWO_PI;

    fPlanarAngle = 0.0f;
    fPlanarAngleVar = NI_TWO_PI;


    // Set the particles speed
    fSpeed = m_pMDtEmitter->m_fSpeed * gExport.m_fLinearUnitMultiplier;

    fSpeedVar = m_pMDtEmitter->m_fSpeedRandom * 
        gExport.m_fLinearUnitMultiplier;

    NiAVObject* pkEmitterObj = gUserData.GetNode(
        m_pMDtParticles->m_MObjectEmitter);

    NiColorA kInitialColor = NiColorA::WHITE;
    float fInitialRadius = GetInitialSize();

            // Create the emitter modifier
    return NiNew NiPSysBoxEmitter("Emitter", 
            pkEmitterObj, 0.0f, 0.0f, 0.0f, fSpeed, 
            fSpeedVar, fDeclination, fDeclinationVar, fPlanarAngle, 
            fPlanarAngleVar, kInitialColor, fInitialRadius, fLifeSpan, 
            fLifeSpanVar, fSizeVariance);

}
//---------------------------------------------------------------------------
NiPSysEmitter*  MyiParticleSystem::CreateSurface()
{
    if (m_pMDtEmitter->m_MObjectEmitterSurface.isNull())
    {
        char acString[1024];
        const char *pcName = NULL;
        pcName = m_pMDtEmitter->m_szName;
        NiSprintf(acString, 1024,"WARNING: Emitter [%s]" \
            " is set to use a surface emitter, but no surface" \
            " is specified. Defaulting to Omni emitter.\n", pcName);
        DtExt_Err(acString);
        return CreateOmni();
    }

    float fEmitterWidth = 0.0f;
    float fEmitterHeight = 0.0f;
    float fEmitterDepth = 0.0f;

    float fSpeed = 0.0f;
    float fSpeedVar = 0.0f;
    float fDeclination = 0.0f;
    float fDeclinationVar = 0.0f;
    float fPlanarAngle = 0.0f;
    float fPlanarAngleVar = 0.0f;

    float fSizeVariance = GetParticleSizeVariance();

    // Particle Life Span
    float fLifeSpan = 0.0f;
    float fLifeSpanVar = 0.0f;

    GetLifeSpan(fLifeSpan, fLifeSpanVar);

    // Set Particle Declination
    NiPoint3 Direction(m_pMDtEmitter->m_fDirectionX,
                       m_pMDtEmitter->m_fDirectionY,
                       m_pMDtEmitter->m_fDirectionZ);


    Direction.Unitize();

    fDeclination = NiACos(Direction.Dot(NiPoint3(0.0f, 0.0f, 1.0f)));

    fDeclinationVar = m_pMDtEmitter->m_fSpread * NI_HALF_PI;

    // Set the Particle Planar Angle
    NiPoint3 XYProjection(Direction.x, Direction.y, 0.0f);

    XYProjection.Unitize();

    fPlanarAngle = NiACos(XYProjection.Dot(NiPoint3(1.0f, 0.0f, 0.0f)));

    if (XYProjection.y < 0.0)
        fPlanarAngle = -fPlanarAngle;


    fPlanarAngleVar = m_pMDtEmitter->m_fSpread * NI_HALF_PI;

    // Set the particles speed
    fSpeed = m_pMDtEmitter->m_fSpeed * gExport.m_fLinearUnitMultiplier;

    fSpeedVar = m_pMDtEmitter->m_fSpeedRandom * 
        gExport.m_fLinearUnitMultiplier;


    NiAVObject* pkEmitterObj = gUserData.GetNode(
        m_pMDtParticles->m_MObjectEmitter);

    NiColorA kInitialColor = NiColorA::WHITE;
    float fInitialRadius = GetInitialSize();

    // Read in the Surface Attributes
    MFnDagNode dgNode(m_pMDtParticles->m_MObjectEmitter);

    int iEmitLocation = 0;
    GetExtraAttribute(dgNode, "Ni_SurfaceEmitLocation", true, iEmitLocation);

    int iEmitDirection = 0;
    GetExtraAttribute(dgNode, "Ni_SurfaceEmitDirection", true, iEmitDirection);

    NiPoint3 kEmitAxis(1.0f, 0.0f, 0.0f);
    GetExtraAttribute(dgNode, "Ni_EmitterSurfaceEmitAxisX", true, kEmitAxis.x);
    GetExtraAttribute(dgNode, "Ni_EmitterSurfaceEmitAxisY", true, kEmitAxis.y);
    GetExtraAttribute(dgNode, "Ni_EmitterSurfaceEmitAxisZ", true, kEmitAxis.z);


    MStatus kStatus;
    MFnDagNode kShape(m_pMDtEmitter->m_MObjectEmitterSurface, &kStatus);
    NIASSERT(kStatus == MS::kSuccess);

    MObject kParentNode = kShape.parent(0, &kStatus);
    NIASSERT(kStatus == MS::kSuccess);

    NiAVObject* pkSurface = gUserData.GetNode(kParentNode);
    NiTObjectArray<NiGeometryPtr> kSurfaceGeometry;

    FindSurfaceGeometry(pkSurface, kSurfaceGeometry);

            // Create the emitter modifier
    NiPSysMeshEmitter* pkEmitter =  NiNew NiPSysMeshEmitter("Emitter", 
        NULL, (NiPSysMeshEmitter::MeshEmissionType)iEmitLocation, 
        (NiPSysMeshEmitter::InitialVelocityType)iEmitDirection, 
        kEmitAxis, fSpeed, fSpeedVar, fDeclination, fDeclinationVar, 
        fPlanarAngle, fPlanarAngleVar, kInitialColor, fInitialRadius,
        fLifeSpan, fLifeSpanVar, fSizeVariance);


    // Add the Geometry and set the SGO Keep.
    unsigned int uiLoop;
    for (uiLoop = 0; uiLoop < kSurfaceGeometry.GetSize(); uiLoop++)
    {
        pkEmitter->AddGeometryEmitter(kSurfaceGeometry.GetAt(uiLoop));

        // Add the sgoKeep Tag so our Geometry won't be combined with 
        // anything.
        NiStringExtraData* pDontCull = NiNew NiStringExtraData("sgoKeep");
        kSurfaceGeometry.GetAt(uiLoop)->AddExtraData("sgoKeep", pDontCull);
    }

    return pkEmitter;
}
//---------------------------------------------------------------------------
void MyiParticleSystem::FindSurfaceGeometry(NiAVObject* pkObject, 
    NiTObjectArray<NiGeometryPtr>& kSurfaceGeometry)
{
    // Add any geometry to the list
    if (NiIsKindOf(NiGeometry, pkObject))
    {
        kSurfaceGeometry.Add((NiGeometry*)pkObject);
        return;
    }

    // Search the children as well.
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        unsigned int uiLoop;
        for (uiLoop = 0; uiLoop < pkNode->GetArrayCount(); uiLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiLoop);

            if (pkChild)
            {
                FindSurfaceGeometry(pkChild, kSurfaceGeometry);
            }
        }
    }
}
//---------------------------------------------------------------------------
NiPSysEmitter* MyiParticleSystem::CreateCurve()
{
    DtExt_Err("Error:: Emitter %s has an unsupported Emitter Type: CURVE\n", 
        m_pMDtEmitter->m_szName );


    // Particle Life Span
    float fLifeSpan = 0.0f;
    float fLifeSpanVar = 0.0f;

    GetLifeSpan(fLifeSpan, fLifeSpanVar);

    NiAVObject* pkEmitterObj = gUserData.GetNode(
        m_pMDtParticles->m_MObjectEmitter);

    float fInitialRadius = GetInitialSize();
    float fSizeVariance = GetParticleSizeVariance();

    // Create a default emitter modifier
    return NiNew NiPSysBoxEmitter("Emitter", 
        pkEmitterObj, 0.0f, 0.0f, 0.0f, 0.0f, 
        0.0f, 0.0f, 0.0f, 0.0f, 
        0.0f, NiColorA::WHITE, fInitialRadius, fLifeSpan, fLifeSpanVar,
        fSizeVariance);
}
//---------------------------------------------------------------------------
NiPSysEmitter* MyiParticleSystem::CreateVolume()
{

    // Particle Life Span
    float fLifeSpan = 0.0f;
    float fLifeSpanVar = 0.0f;

    GetLifeSpan(fLifeSpan, fLifeSpanVar);

    //
    // Initialize the emitter's diminsions
    //
    float fEmitterWidth = 0.0f;
    float fEmitterHeight = 0.0f;
    float fEmitterDepth = 0.0f;

    float fSpeed = 0.0f;
    float fSpeedVar = 0.0f;
    float fDeclination = 0.0f;
    float fDeclinationVar = 0.0f;
    float fPlanarAngle = 0.0f;
    float fPlanarAngleVar = 0.0f;
    float fSizeVariance = GetParticleSizeVariance();

    const char* pcName = "Emitter";
    NiAVObject* pkEmitterObj = gUserData.GetNode(
        m_pMDtParticles->m_MObjectEmitter);


    NiColorA kInitialColor = NiColorA::WHITE;
    float fInitialRadius = GetInitialSize();

    //
    // Initialize the emitter's diminsions
    //
    fEmitterHeight = m_pMDtEmitter->m_fScaleY * 2.0f * 
        gExport.m_fLinearUnitMultiplier; 

    fEmitterWidth = m_pMDtEmitter->m_fScaleX * 2.0f * 
        gExport.m_fLinearUnitMultiplier; 

    fEmitterDepth = m_pMDtEmitter->m_fScaleZ * 2.0f * 
        gExport.m_fLinearUnitMultiplier; 


    // Set Particle Declination
    NiPoint3 Direction(m_pMDtEmitter->m_fDirectionX,
                       m_pMDtEmitter->m_fDirectionY,
                       m_pMDtEmitter->m_fDirectionZ);


    Direction.Unitize();

    // The declination will be the Angular difference between the Direction
    // vector and the Z axis

    fDeclination = NiACos(Direction.Dot(NiPoint3(0.0f, 0.0f, 1.0f)));

        // Set the Declination Variation Making sure we don't divide by zero
    if ((m_pMDtEmitter->m_fDirectionalSpeed <= 0.0f) ||
        (m_pMDtEmitter->m_fRandomDirection / 
            m_pMDtEmitter->m_fDirectionalSpeed) >= NI_PI)
    {
        fDeclinationVar = NI_PI;
    }
    else
    {
        fDeclinationVar = m_pMDtEmitter->m_fRandomDirection / 
            m_pMDtEmitter->m_fDirectionalSpeed;
    }

    // Set the Particle Planar Angle
    NiPoint3 XYProjection(Direction.x, Direction.y, 0.0f);

    XYProjection.Unitize();

    // The planar angle will be the difference between the Direction vector 
    // projectected on the XY plane and X axis

    fPlanarAngle = NiACos(XYProjection.Dot(NiPoint3(1.0f, 0.0f, 0.0f)));

    // Since the planar calculation only indicates the angle and not it's 
    // direction we need to see if the rotation is positive or negative.  
    // When the direction vector is below the XZ plane the rotation will 
    // be negative.

    if (XYProjection.y < 0.0)
        fPlanarAngle = -fPlanarAngle;


        // Set the Planar Angle Variation Making sure we don't divide by zero
    if ((m_pMDtEmitter->m_fDirectionalSpeed <= 0.0f) ||
        (m_pMDtEmitter->m_fRandomDirection / 
            m_pMDtEmitter->m_fDirectionalSpeed) >= NI_PI)
    {
        fPlanarAngleVar = NI_PI;
    }
    else
    {
        fPlanarAngleVar = m_pMDtEmitter->m_fRandomDirection / 
            m_pMDtEmitter->m_fDirectionalSpeed;
    }

    // Set the particles speed
    fSpeed = m_pMDtEmitter->m_fDirectionalSpeed * 
        gExport.m_fLinearUnitMultiplier;

    fSpeedVar = m_pMDtEmitter->m_fSpeedRandom * 
        gExport.m_fLinearUnitMultiplier;


    switch (m_pMDtEmitter->m_lVolumeShapeType)
    {
    case kMDtEmitter::CUBE:
        return NiNew NiPSysBoxEmitter(pcName, 
            pkEmitterObj, fEmitterWidth, fEmitterHeight, fEmitterDepth, 
            fSpeed, fSpeedVar, fDeclination, fDeclinationVar, fPlanarAngle, 
            fPlanarAngleVar, kInitialColor, fInitialRadius, fLifeSpan, 
            fLifeSpanVar, fSizeVariance);
        break;
    case kMDtEmitter::SPHERE:
        return NiNew NiPSysSphereEmitter(pcName, 
            pkEmitterObj, 
            NiMin(fEmitterWidth, NiMin(fEmitterHeight, fEmitterDepth)) / 2.0f,
            fSpeed, fSpeedVar, fDeclination, fDeclinationVar, 
            fPlanarAngle, fPlanarAngleVar, kInitialColor, 
            fInitialRadius, fLifeSpan, fLifeSpanVar, fSizeVariance);
        break;

    case kMDtEmitter::CYLINDER:
        {
            // There is an 90 degree X rotation between the cylinder of Maya 
            // and Gamebryo So we need to add a node to accomplish that
            // rotation.
            NiMatrix3 kMatrix;
            NiNodePtr spRotationNode = NiNew NiNode;
            spRotationNode->SetName("CylinderRotationNode");
            kMatrix.MakeXRotation(NI_HALF_PI);
            spRotationNode->SetRotate(kMatrix);

            NIASSERT(NiIsKindOf(NiNode, pkEmitterObj));

            ((NiNode*)pkEmitterObj)->AttachChild(spRotationNode);

            return NiNew NiPSysCylinderEmitter(pcName, 
                spRotationNode, NiMin(fEmitterWidth, fEmitterDepth) / 2.0f, 
                fEmitterHeight, fSpeed, fSpeedVar, fDeclination, 
                fDeclinationVar, fPlanarAngle, fPlanarAngleVar, 
                kInitialColor, fInitialRadius, fLifeSpan, fLifeSpanVar, 
                fSizeVariance);
        }
        break;
    case kMDtEmitter::CONE:
        DtExt_Err("Error:: Emitter %s has an Unsupported Emitter Volume "
            "Type::CONE\n", m_pMDtEmitter->m_szName );

        return NiNew NiPSysBoxEmitter(pcName, 
            pkEmitterObj, fEmitterWidth, fEmitterHeight, fEmitterDepth, 
            fSpeed, fSpeedVar, fDeclination, fDeclinationVar, 
            fPlanarAngle, fPlanarAngleVar, kInitialColor, 
            fInitialRadius, fLifeSpan, fLifeSpanVar, fSizeVariance);
        break;

    case kMDtEmitter::TORUS:
        DtExt_Err("Error:: Emitter %s has an Unsupported Emitter Volume "
            "Type::TORUS\n", m_pMDtEmitter->m_szName );
        return NiNew NiPSysBoxEmitter(pcName, 
            pkEmitterObj, fEmitterWidth, fEmitterHeight, fEmitterDepth, 
            fSpeed, fSpeedVar, fDeclination, fDeclinationVar, 
            fPlanarAngle, fPlanarAngleVar, kInitialColor, 
            fInitialRadius, fLifeSpan, fLifeSpanVar, fSizeVariance);
        break;

    }

    NIASSERT(false);
    return NULL;
}
//---------------------------------------------------------------------------
void MyiParticleSystem::CreateEmitterSpeedController()
{
    if (m_pMDtEmitter == NULL)
        return;


    NiFloatInterpolator* pkInterp;

    if (!CreateFloatInterpolator(m_pMDtParticles->m_MObjectEmitter, 
        "speed", pkInterp))
        return;

    // Create the Controller
    NiPSysEmitterSpeedCtlr* pkController = NiNew NiPSysEmitterSpeedCtlr(
        "Emitter");

    // Set the interpolator and reset the extrema
    pkController->SetInterpolator(pkInterp);
    pkController->ResetTimeExtrema();

    // Attach the Controller to the Particle System
    pkController->SetTarget(m_pkParticleSystem);
}
//---------------------------------------------------------------------------
float MyiParticleSystem::GetInitialSize()
{
    float fInitialSize = 1.0f;

    // 
    // Set particle size
    //
    switch (m_pMDtParticles->m_lParticleRenderType)
    {
    case kMDtParticles::SPRITES:

        if ((m_pMDtParticles->m_fSpriteScaleX == 0.0f) ||
           (m_pMDtParticles->m_fSpriteScaleY == 0.0f))
        {
            DtExt_Err("Error:: Particles %s has a Zero Sprite Scale. X:%d "
                "Y:%d  Sprite Scale is being defaulted to 1.0.\n",
                m_pMDtParticles->m_szName, m_pMDtParticles->m_fSpriteScaleX, 
                m_pMDtParticles->m_fSpriteScaleY );
    
            m_pMDtParticles->m_fSpriteScaleX = 1.0f;
            m_pMDtParticles->m_fSpriteScaleY = 1.0f;
        }

        fInitialSize = m_pMDtParticles->m_fSpriteScaleX * 
            gExport.m_fLinearUnitMultiplier;


        if (m_pMDtParticles->m_fSpriteScaleX != 
                m_pMDtParticles->m_fSpriteScaleY)
        {
            DtExt_Err("Error:: Particles %s has different 'Sprite Scale X' "
                "and 'Sprite Scale Y'. Gamebryo only supports uniform "
                "particle size.\n", m_pMDtParticles->m_szName );
        }
        break;
    case kMDtParticles::POINTS:
    case kMDtParticles::MULTIPOINT:
    case kMDtParticles::MULTISTREAK:
    case kMDtParticles::NUMERIC:
    case kMDtParticles::SPHERES:
    case kMDtParticles::STREAK:
    case kMDtParticles::BLOBBY_SURFACE:
    case kMDtParticles::CLOUD:
    case kMDtParticles::TUBE:
        DtExt_Err("Error:: Particles %s has an Unsupported Particle Render "
            "Type within Gamebryo. Only 'Sprites' are supported\n", 
            m_pMDtParticles->m_szName );
        break;
    default:
        DtExt_Err("Error:: Particles %s has an invalid Render Type\n", 
            m_pMDtParticles->m_szName );
        break;
    }

    return fInitialSize;
}
//---------------------------------------------------------------------------
void MyiParticleSystem::CreateEmitterController()
{
    // Check for No Emitter
    if (m_pMDtEmitter == NULL)
        return;


    unsigned int uiNumBirthRateKeys = 0;
    NiFloatKey* pkBirthRateKeys = NULL;
    NiFloatKey::KeyType eBirthRateKeyType = NiAnimationKey::LINKEY;
    unsigned int uiNumEmitterActiveKeys;
    NiStepBoolKey* pkEmitterActiveKeys;


    MFnDependencyNode kEmitter(m_pMDtEmitter->m_MObjectEmitter);

    // Read the Plug
    MStatus kStatus;
    MPlug kMPlug = kEmitter.findPlug(MString("rate"), &kStatus);

    // Create any animation
    if (!kMPlug.isConnected() ||
       (ConvertFloatAnim(kMPlug, uiNumBirthRateKeys, pkBirthRateKeys, 
            eBirthRateKeyType) != MS::kSuccess))
    {

        // Create Some Default Keys if we the rate isn't animated
        uiNumBirthRateKeys = 1;
        pkBirthRateKeys = NiNew NiLinFloatKey[uiNumBirthRateKeys];
        NiFloatKey::KeyType eBirthRateKeyType = NiAnimationKey::LINKEY;
        
        
        // Set the birth rate
        pkBirthRateKeys[0].SetTime(0.0f);
        pkBirthRateKeys[0].SetValue(m_pMDtEmitter->m_fParticlesPerSecond);

        // Read the begin and end keys attributes to create
        // the start and stop keys
        SetEmitterControllerStartAndStop(uiNumEmitterActiveKeys, 
            pkEmitterActiveKeys);
    }
    else
    {
        SetEmitterControllerStartAndStop( uiNumEmitterActiveKeys, 
            pkEmitterActiveKeys, uiNumBirthRateKeys, pkBirthRateKeys, 
            eBirthRateKeyType);
    }

    // Create the Rate Data and Interpolator
    NiFloatData* pkRateData = NiNew NiFloatData();
    pkRateData->ReplaceAnim(pkBirthRateKeys, uiNumBirthRateKeys, 
        eBirthRateKeyType);

    NiFloatInterpolator *pkRateInterp = NiNew NiFloatInterpolator(pkRateData);
    pkRateInterp->Collapse();

    // Create the Active Data and Interpolator
    NiBoolData* pkActiveData = NiNew NiBoolData();
    pkActiveData->ReplaceAnim(pkEmitterActiveKeys, uiNumEmitterActiveKeys, 
        NiAnimationKey::STEPKEY);

    NiBoolInterpolator *pkActiveInterp = NiNew NiBoolInterpolator(
        pkActiveData);

    pkActiveInterp->Collapse();

    // Create the Controller and add the Interpolators
    NiPSysEmitterCtlr* pkEmitterController = NiNew NiPSysEmitterCtlr(
        "Emitter");

    pkEmitterController->SetBirthRateInterpolator(pkRateInterp);
    pkEmitterController->SetEmitterActiveInterpolator(pkActiveInterp);
    pkEmitterController->ResetTimeExtrema();   

    // Check the Looping Type
    pkEmitterController->SetCycleType(NiTimeController::LOOP);


    MFnDagNode dgNode(m_pMDtParticles->m_MObjectEmitter);
    int iLoopingType;
    if (GetExtraAttribute(dgNode, "Ni_EmitterLoopingType", true, iLoopingType))
    {
        switch (iLoopingType)
        {
        case 0:
            pkEmitterController->SetCycleType(NiTimeController::LOOP);
            break;
        case 1:
            pkEmitterController->SetCycleType(NiTimeController::CLAMP);
            break;
        }
    }

    // Attach the Controller to the Particle System
    pkEmitterController->SetTarget(m_pkParticleSystem);
}
//---------------------------------------------------------------------------
void MyiParticleSystem::SetEmitterControllerStartAndStop(
    unsigned int& uiNumEmitterActiveKeys, NiStepBoolKey*& pkEmitterActiveKeys)
{
    MFnDagNode dgNode(m_pMDtParticles->m_MObjectEmitter);

    float fBeginKey;
    float fEndKey;
    float fEmitterStart;
    float fEmitterStop;

    if (GetExtraAttribute(dgNode, "Ni_EmitterBeginKeyFrame", true, 
            fBeginKey) &&
        GetExtraAttribute(dgNode, "Ni_EmitterEndKeyFrame", true, fEndKey) &&
        GetExtraAttribute(dgNode, "Ni_EmitterEmitterStartFrame", true, 
            fEmitterStart) &&
        GetExtraAttribute(dgNode, "Ni_EmitterEmitterStopFrame", true, 
            fEmitterStop))
    {
        fBeginKey /= gExport.m_fFramesPerSecond;
        fEndKey /= gExport.m_fFramesPerSecond;
        fEmitterStart /= gExport.m_fFramesPerSecond;
        fEmitterStop /= gExport.m_fFramesPerSecond;

        uiNumEmitterActiveKeys = 4;

        if (fBeginKey == fEmitterStart)
            uiNumEmitterActiveKeys--;

        if (fEndKey == fEmitterStop)
            uiNumEmitterActiveKeys--;

        pkEmitterActiveKeys = NiNew NiStepBoolKey[uiNumEmitterActiveKeys];

        int iCurrentKey = 0;

        pkEmitterActiveKeys->GetKeyAt(iCurrentKey, 
            sizeof(NiStepBoolKey))->SetTime(fBeginKey);

        pkEmitterActiveKeys[iCurrentKey].SetBool(fBeginKey == fEmitterStart);

        if (fBeginKey != fEmitterStart)
        {
            pkEmitterActiveKeys[++iCurrentKey].SetTime(fEmitterStart);
            pkEmitterActiveKeys[iCurrentKey].SetBool(true);
        }

        pkEmitterActiveKeys[++iCurrentKey].SetTime(fEmitterStop);
        pkEmitterActiveKeys[iCurrentKey].SetBool(false);

        if (fEndKey != fEmitterStop)
        {
            pkEmitterActiveKeys[++iCurrentKey].SetTime(fEndKey);
            pkEmitterActiveKeys[iCurrentKey].SetBool(false);
        }
    }
    else
    {
        uiNumEmitterActiveKeys = 2;
        pkEmitterActiveKeys = NiNew NiStepBoolKey[uiNumEmitterActiveKeys];
        
        pkEmitterActiveKeys[0].SetTime(m_pMDtParticles->m_fStartTime);
        pkEmitterActiveKeys[0].SetBool(true);
        pkEmitterActiveKeys[1].SetTime((float)gAnimControlGetMaxTime());
        pkEmitterActiveKeys[1].SetBool(false);
    }
}
//---------------------------------------------------------------------------
void MyiParticleSystem::SetEmitterControllerStartAndStop(
    unsigned int& uiNumEmitterActiveKeys, NiStepBoolKey*& pkEmitterActiveKeys,
    unsigned int uiNumBirthRateKeys, NiFloatKey* pkBirthRateKeys,
    NiFloatKey::KeyType eBirthRateKeyType)
{

    double dTime;
    double dStep = 1.0f / (gAnimControlGetFramesPerSecond() * 4.0);
    double dStartTime = 0.0;
    double dEndTime = 0.0;

    unsigned char ucRateSize = NiFloatKey::GetKeySize(eBirthRateKeyType);
   
    // Determine the Start and End Keys
    switch (eBirthRateKeyType)
    {
    case NiAnimationKey::BEZKEY:
        dStartTime = ((NiBezFloatKey*)pkBirthRateKeys)[0].GetTime();
        dEndTime = ((NiBezFloatKey*)pkBirthRateKeys)[uiNumBirthRateKeys-1].
            GetTime();
        break;
    case NiAnimationKey::LINKEY:
        dStartTime = ((NiLinFloatKey*)pkBirthRateKeys)[0].GetTime();
        dEndTime = ((NiLinFloatKey*)pkBirthRateKeys)[uiNumBirthRateKeys-1].
            GetTime();
        break;
    default:
        NIASSERT(false);
    }


    // Count the number of necessary keys
    uiNumEmitterActiveKeys = 0;

    float fRate = 0.0f;
    float fLastRate = 0.0f;
    unsigned int uiLastIdx = 0;

    // Super Sample the Keys looking for where keys become positive
    for (dTime = dStartTime; dTime <= (dEndTime + (dStep / 2.0)); 
        dTime += dStep)
    {
        if (dTime <= dEndTime)
        {
            fRate = NiFloatKey::GenInterp((float)dTime, pkBirthRateKeys, 
                eBirthRateKeyType, uiNumBirthRateKeys, uiLastIdx, ucRateSize);
        }
        else
        {
            fRate = pkBirthRateKeys->GetKeyAt(uiNumBirthRateKeys - 1, 
                ucRateSize)->GetValue();
        }


        // First Key
        if (NiOptimize::CloseTo((float)dTime, (float)dStartTime, 
            (float)(dStep / 10.0)))
        {
            uiNumEmitterActiveKeys++;
        }
        else
        {
            // Starting Positive
            if ((fRate > 0.0f) && (fLastRate <= 0.0f))
            {
                uiNumEmitterActiveKeys++;
            }
        }


        // Last Key
        if (NiOptimize::CloseTo((float)dTime, (float)dEndTime, 
            (float)(dStep / 10.0)))
        {
            uiNumEmitterActiveKeys++;
        }
        else
        {
            // Starting Negative
            if ((fRate <= 0.0f) && (fLastRate > 0.0f))
            {
                uiNumEmitterActiveKeys++;
            }
        }

        fLastRate = fRate;
    }

    // Allocate the necessary Keys
    pkEmitterActiveKeys = NiNew NiStepBoolKey[uiNumEmitterActiveKeys];
    unsigned char ucActiveSize = sizeof(NiStepBoolKey);


    // Create the Keys
    fRate = 0.0f;
    fLastRate = 0.0f;
    uiLastIdx = 0;
    unsigned int uiCurrentKey = 0;

    // Super Sample the Keys looking for where keys become positive
    for (dTime = dStartTime; dTime <= (dEndTime + (dStep / 2.0)); 
        dTime += dStep)
    {
        if (dTime <= dEndTime)
        {
            fRate = NiFloatKey::GenInterp((float)dTime, pkBirthRateKeys, 
                eBirthRateKeyType, uiNumBirthRateKeys, uiLastIdx, ucRateSize);
        }
        else
        {
            fRate = pkBirthRateKeys->GetKeyAt(uiNumBirthRateKeys - 1, 
                ucRateSize)->GetValue();
        }



        // First Key
        if (NiOptimize::CloseTo((float)dTime, (float)dStartTime, 
            (float)(dStep / 10.0)))
        {
            // Create a begin key at the curren time
            pkEmitterActiveKeys->GetKeyAt(uiCurrentKey, ucActiveSize)
                ->SetTime((float)dTime);
            pkEmitterActiveKeys->GetKeyAt(uiCurrentKey++, ucActiveSize)
                ->SetBool(fRate > 0.0f);
        }
        else
        {
            // Starting Positive
            if ((fRate > 0.0f) && (fLastRate <= 0.0f))
            {

                // First Key
                if (NiOptimize::CloseTo((float)(dTime - dStep), 
                    (float)dStartTime, (float)(dStep / 10.0)))
                {
                    // Create a key at the last time Plus a little bit to
                    // avoid duplicate key times
                    pkEmitterActiveKeys->GetKeyAt(uiCurrentKey, ucActiveSize)
                        ->SetTime((float)(dTime - dStep + 0.0001));
                }
                else
                {
                    // Create a key at the last time
                    pkEmitterActiveKeys->GetKeyAt(uiCurrentKey, ucActiveSize)
                        ->SetTime((float)(dTime - dStep));
                }

                pkEmitterActiveKeys->GetKeyAt(uiCurrentKey++, ucActiveSize)
                    ->SetBool(true);
            }
        }

        // At the beginning
        if (NiOptimize::CloseTo((float)dTime, (float)dEndTime, 
            (float)(dStep / 10.0)))
        {
            // Create a begin key at the curren time
            pkEmitterActiveKeys->GetKeyAt(uiCurrentKey, ucActiveSize)
                ->SetTime((float)dTime);
            pkEmitterActiveKeys->GetKeyAt(uiCurrentKey++, ucActiveSize)
                ->SetBool(false);
        }
        else
        {
            // Starting Negative
            if ((fRate <= 0.0f) && (fLastRate > 0.0f))
            {
                // Create a key at the last time
                pkEmitterActiveKeys->GetKeyAt(uiCurrentKey, ucActiveSize)
                    ->SetTime((float)(dTime - dStep));
                pkEmitterActiveKeys->GetKeyAt(uiCurrentKey++, ucActiveSize)
                    ->SetBool(false);
            }
        }


        fLastRate = fRate;
    }

}
//---------------------------------------------------------------------------
void MyiParticleSystem::CreatePositionModifier()
{
    NiPSysPositionModifier* pkModifier = NiNew NiPSysPositionModifier(
        "Position");

    m_pkParticleSystem->AddModifier(pkModifier);
}
//---------------------------------------------------------------------------
void MyiParticleSystem::CreateAgeDeathModifier(
    NiTList<MyiParticleSystem*>& pkParticleSystems)
{
    NiPSysAgeDeathModifier* pkModifier = NULL;
    NiParticleSystem* pkSpawnParticles = NULL;
    bool bSpawnOnDeath = false;
    int iNumGenerations;
    int iMinNumToSpawn;
    int iMaxNumToSpawn;
    float fPercentSpawned;
    float fSpread;
    float fSpawnSpeedChaos;
    float fLifeSpan = 0.0f;
    float fLifeSpanVar = 0.0f;
    MString kTargetParticle;
    MStatus kStatus;

    MFnDependencyNode kParticles(m_pMDtParticles->m_MObjectParticles, 
        &kStatus);

    NIASSERT(kStatus == MS::kSuccess);

    if (GetExtraAttribute(kParticles, "Ni_ParticleSpawnOnDeath", false, 
            bSpawnOnDeath) &&
        GetExtraAttribute(kParticles, "Ni_ParticleNumGenerations", false, 
            iNumGenerations) &&
        GetExtraAttribute(kParticles, "Ni_ParticleMinNumToSpawn", false, 
            iMinNumToSpawn) &&
        GetExtraAttribute(kParticles, "Ni_ParticleMaxNumToSpawn", false, 
            iMaxNumToSpawn) &&
        GetExtraAttribute(kParticles, "Ni_ParticlePercentageSpawned", false, 
            fPercentSpawned) &&
        GetExtraAttribute(kParticles, "Ni_ParticleSpawnedSpread", false, 
            fSpread) &&
        GetExtraAttribute(kParticles, "Ni_ParticleSpawnedSpeedChaos", false, 
            fSpawnSpeedChaos) &&
        GetExtraAttribute(kParticles, "Ni_ParticleTargetParticle", false, 
            kTargetParticle))
    {

        // Search for the Target Particle Shape
        NiTListIterator kIter = pkParticleSystems.GetHeadPos();
        
        while (kIter != NULL)
        {
            if (pkParticleSystems.Get(kIter)->MatchesParticleName(
                kTargetParticle))
            {
                pkSpawnParticles = 
                    pkParticleSystems.Get(kIter)->GetParticleSystem();
                
                pkParticleSystems.Get(kIter)->GetLifeSpan(fLifeSpan, 
                    fLifeSpanVar);
                break;
            }
            
            kIter = pkParticleSystems.GetNextPos(kIter);
        }
    }

    if (!bSpawnOnDeath || (pkSpawnParticles == NULL))
    {
        pkModifier = NiNew NiPSysAgeDeathModifier("AgeDeath", false);
    }
    else
    {

        NiPSysSpawnModifier* pkSpawn = NiNew NiPSysSpawnModifier(
            "Spawn on Death", iNumGenerations, 1.0f, iMinNumToSpawn, 
            iMaxNumToSpawn, fSpawnSpeedChaos, fSpread, fLifeSpan, 
            fLifeSpanVar);

        // Add the emitter to the spawning sparticle system
        pkSpawnParticles->AddModifier(pkSpawn);

        pkModifier = NiNew NiPSysAgeDeathModifier("AgeDeath", bSpawnOnDeath,
            pkSpawn);
        
    }

    m_pkParticleSystem->AddModifier(pkModifier);
}
//---------------------------------------------------------------------------
void MyiParticleSystem::CreateColorModifier()
{
    MObject MParticleShape = m_pMDtParticles->m_MObjectParticles;

    MStatus kStatus;

    // get place2dTexture object for this texture
    MFnDependencyNode kShape(MParticleShape, &kStatus);
    NIASSERT(kStatus == MS::kSuccess);

    // examine animatable parameters to see if they're animated
    MPlug kPlugColorR = kShape.findPlug("Ni_ParticleVertexColorR", &kStatus);
    if (kStatus != MS::kSuccess) return;

    MPlug kPlugColorG = kShape.findPlug("Ni_ParticleVertexColorG", &kStatus);
    if (kStatus != MS::kSuccess) return;

    MPlug kPlugColorB = kShape.findPlug("Ni_ParticleVertexColorB", &kStatus);
    if (kStatus != MS::kSuccess) return;

    MPlug kPlugColorA = kShape.findPlug("Ni_ParticleVertexColorAlpha", 
        &kStatus);
    if (kStatus != MS::kSuccess) return;

    if (!kPlugColorR.isConnected() && 
        !kPlugColorG.isConnected() && 
        !kPlugColorB.isConnected() && 
        !kPlugColorA.isConnected() )
    {
        return;
    }

    unsigned int uiNumKeys;
    NiColorKey* pkKeys;
    NiAnimationKey::KeyType eType;
    if (ConvertColorAlphaAnim(kPlugColorR, kPlugColorG, kPlugColorB, 
        kPlugColorA, uiNumKeys, pkKeys, eType) == MS::kFailure)
    {
        return;
    }


    // Normalize the Keys to run from 0-1
    float fKeyTimeOffset = 0.0f;
    float fKeyTimeScale = 1.0f;

    switch (eType)
    {
    case NiAnimationKey::LINKEY:
        fKeyTimeOffset = ((NiLinColorKey*)pkKeys)[0].GetTime();
        fKeyTimeScale = ((NiLinColorKey*)pkKeys)[uiNumKeys-1].GetTime() - 
                ((NiLinColorKey*)pkKeys)[0].GetTime();
        break;
    default:
        NIASSERT(0);
    }

    unsigned int uiKLoop=0;

    switch (eType)
    {
    case NiAnimationKey::LINKEY:
        for (uiKLoop = 0; uiKLoop < uiNumKeys; uiKLoop++)
        {
            float fTime = (((NiLinColorKey*)pkKeys)[uiKLoop].GetTime() - 
                fKeyTimeOffset) / fKeyTimeScale;

            ((NiLinColorKey*)pkKeys)[uiKLoop].SetTime(fTime);

            // Clamp all values to [0,1.0]
            NiColorA kColor = ((NiLinColorKey*)pkKeys)[uiKLoop].GetColor();
            kColor.r = NiMin(1.0f, kColor.r);
            kColor.r = NiMax(0.0f, kColor.r);
            kColor.g = NiMin(1.0f, kColor.g);
            kColor.g = NiMax(0.0f, kColor.g);
            kColor.b = NiMin(1.0f, kColor.b);
            kColor.b = NiMax(0.0f, kColor.b);
            kColor.a = NiMin(1.0f, kColor.a);
            kColor.a = NiMax(0.0f, kColor.a);
            
            ((NiLinColorKey*)pkKeys)[uiKLoop].SetColor(kColor);
        }
        break;
    default:
        NIASSERT(0);
    }

    NiColorData* pkData = NiNew NiColorData;
    pkData->ReplaceAnim(pkKeys, uiNumKeys, eType);

    NiPSysColorModifier* pkModifier = 
        NiNew NiPSysColorModifier("Color Modifier", pkData);

    m_pkParticleSystem->AddModifier(pkModifier);

    // Check for and Alpha Property
    if (m_pkParticleSystem->GetProperty(NiProperty::ALPHA) == NULL)
    {
        NiAlphaProperty* pAlpha = NiNew NiAlphaProperty;
        pAlpha->SetSrcBlendMode( NiAlphaProperty::ALPHA_SRCALPHA);
        pAlpha->SetDestBlendMode(NiAlphaProperty::ALPHA_INVSRCALPHA);
        pAlpha->SetAlphaBlending(true);
        m_pkParticleSystem->AttachProperty(pAlpha);
    }

}
//---------------------------------------------------------------------------
bool MyiParticleSystem::HasColors()
{

    MObject MParticleShape = m_pMDtParticles->m_MObjectParticles;

    MStatus kStatus;

    // get place2dTexture object for this texture
    MFnDependencyNode kShape(MParticleShape, &kStatus);
    NIASSERT(kStatus == MS::kSuccess);

    NiColorA kColor = NiColorA::WHITE;

    // examine animatable parameters to see if they're animated
    MPlug kPlugColorR = kShape.findPlug("Ni_ParticleVertexColorR", &kStatus);
    if (kStatus != MS::kSuccess) 
        return false;
    else
        kPlugColorR.getValue(kColor.r);


    MPlug kPlugColorG = kShape.findPlug("Ni_ParticleVertexColorG", &kStatus);
    if (kStatus != MS::kSuccess) 
        return false;
    else
       kPlugColorG.getValue(kColor.g);

    MPlug kPlugColorB = kShape.findPlug("Ni_ParticleVertexColorB", &kStatus);
    if (kStatus != MS::kSuccess) 
        return false;
    else
        kPlugColorB.getValue(kColor.b);

    MPlug kPlugColorA = kShape.findPlug("Ni_ParticleVertexColorAlpha", 
        &kStatus);
    if (kStatus != MS::kSuccess) 
        return false;
    else
        kPlugColorA.getValue(kColor.a);

    if (!kPlugColorR.isConnected() && 
        !kPlugColorG.isConnected() && 
        !kPlugColorB.isConnected() && 
        !kPlugColorA.isConnected() &&
        kColor == NiColorA::WHITE)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void MyiParticleSystem::CreateGrowFadeModifier()
{
    MObject MParticleShape = m_pMDtParticles->m_MObjectParticles;

    MStatus kStatus;

    MFnDependencyNode kShape(MParticleShape, &kStatus);
    NIASSERT(kStatus == MS::kSuccess);

    // examine animatable parameters to see if they're animated
    MPlug kGrow = kShape.findPlug("Ni_ParticleGrowFrames", &kStatus);
    if (kStatus != MS::kSuccess) return;

    MPlug kFade = kShape.findPlug("Ni_ParticleShrinkFrames", &kStatus);
    if (kStatus != MS::kSuccess) return;

    float fGrow;
    float fFade;
    
    if (kGrow.getValue(fGrow) != MS::kSuccess)
        return;

    if (kFade.getValue(fFade) != MS::kSuccess)
        return;

    // Check for Zero Grow and Fade
    if (NiOptimize::CloseTo(fGrow, 0.0f, 0.001f) &&
        NiOptimize::CloseTo(fFade, 0.0f, 0.001f))
        return;

    // Create the Controller
    NiPSysGrowFadeModifier* pkModifier = NiNew NiPSysGrowFadeModifier(
        "Grow Fade",
        fGrow / (float)gAnimControlGetFramesPerSecond(), 0,
        fFade / (float)gAnimControlGetFramesPerSecond(), 0);

    m_pkParticleSystem->AddModifier(pkModifier);
}
//---------------------------------------------------------------------------
void MyiParticleSystem::CreateRotationModifier()
{
    if (!HasRotation())
        return;

    MObject MParticleShape = m_pMDtParticles->m_MObjectParticles;

    MStatus kStatus;

    MFnDependencyNode kShape(MParticleShape, &kStatus);
    NIASSERT(kStatus == MS::kSuccess);

    // examine animatable parameters to see if they're animated
    MPlug kRandomAxis = kShape.findPlug("Ni_ParticleRotateRandomAxis", 
        &kStatus);

    if (kStatus != MS::kSuccess) return;

    MPlug kAxisX = kShape.findPlug("Ni_ParticleRotateAxisX", &kStatus);
    if (kStatus != MS::kSuccess) return;

    MPlug kAxisY = kShape.findPlug("Ni_ParticleRotateAxisY", &kStatus);
    if (kStatus != MS::kSuccess) return;

    MPlug kAxisZ = kShape.findPlug("Ni_ParticleRotateAxisZ", &kStatus);
    if (kStatus != MS::kSuccess) return;

    MPlug kRotationSpeed = kShape.findPlug("Ni_ParticleRotateSpeedDegrees", 
        &kStatus);
    if (kStatus != MS::kSuccess) return;

    MPlug kRotationSpeedVar = kShape.findPlug(
        "Ni_ParticleRotationSpeedVarDegrees", &kStatus);
    if (kStatus != MS::kSuccess) return;

    MPlug kRotationIntitialAngle = kShape.findPlug(
        "Ni_ParticleRotationInitialAngle", &kStatus);
    if (kStatus != MS::kSuccess) return;

    MPlug kRotationIntitialAngleVar = kShape.findPlug(
        "Ni_ParticleRotationInitialAngleVar", &kStatus);
    if (kStatus != MS::kSuccess) return;

    MPlug kBiDirectionalRotation = kShape.findPlug(
        "Ni_ParticleBiDirectionalRotation", &kStatus);
    if (kStatus != MS::kSuccess) return;

    bool bRandomAxis;
    float fAxisX;
    float fAxisY;
    float fAxisZ;
    float fRotationSpeed = 0.0f;
    float fRotationSpeedVar = 0.0f;
    float fInitialAngle = 0.0f;
    float fInitialAngleVar = 0.0f;
    bool bBiDirectionalRotation = false;

    if (kRandomAxis.getValue(bRandomAxis) != MS::kSuccess)
        return;

    if (kAxisX.getValue(fAxisX) != MS::kSuccess)
        return;

    if (kAxisY.getValue(fAxisY) != MS::kSuccess)
        return;

    if (kAxisZ.getValue(fAxisZ) != MS::kSuccess)
        return;

    kRotationSpeed.getValue(fRotationSpeed);

    kRotationSpeedVar.getValue(fRotationSpeedVar);

    kRotationIntitialAngle.getValue(fInitialAngle);

    kRotationIntitialAngleVar.getValue(fInitialAngleVar);

    GetExtraAttribute(kShape, "Ni_ParticleBiDirectionalRotation", false, 
            bBiDirectionalRotation);

    // Create the RotationAxis Vector
    NiPoint3 kRotateAxis(fAxisX, fAxisY, fAxisZ);

    // Check for an Invalid Rotation Axis
    if (NiOptimize::Point3CloseTo(kRotateAxis, NiPoint3::ZERO, 0.001f))
    {
        kRotateAxis = NiPoint3::UNIT_X;
        bRandomAxis = true;
    }

    // Unitize the Rotate Axis
    kRotateAxis.Unitize();

    // Create the Controller
    NiPSysRotationModifier* pkModifier = NiNew NiPSysRotationModifier(
        "Rotation Modifier", (fRotationSpeed * NI_PI) / 180.0f, 
        (fRotationSpeedVar * NI_PI) / 180.0f, bBiDirectionalRotation, 
        (fInitialAngle * NI_PI) / 180.0f, 
        (fInitialAngleVar * NI_PI) / 180.0f, bRandomAxis, kRotateAxis);

    m_pkParticleSystem->AddModifier(pkModifier);
}
//---------------------------------------------------------------------------
bool MyiParticleSystem::HasRotation()
{
    MObject MParticleShape = m_pMDtParticles->m_MObjectParticles;

    MStatus kStatus;

    MFnDependencyNode kShape(MParticleShape, &kStatus);
    NIASSERT(kStatus == MS::kSuccess);


    float fRotationSpeed = 0.0f;
    float fRotationSpeedVar = 0.0f;
    float fInitialAngle = 0.0f;
    float fInitialAngleVar = 0.0f;
    bool bAnimatedSpeed = false;
    bool bAnimatedSpeedVar = false;
    bool bAnimatedInitialAngle = false;
    bool bAnimatedInitialAngleVar = false;

    MPlug kRotationSpeed = kShape.findPlug("Ni_ParticleRotateSpeedDegrees", 
        &kStatus);

    if (kStatus == MS::kSuccess) 
    {
        kRotationSpeed.getValue(fRotationSpeed);
        bAnimatedSpeed = kRotationSpeed.isConnected();
    }

    MPlug kRotationSpeedVar = kShape.findPlug(
        "Ni_ParticleRotationSpeedVarDegrees", &kStatus);

    if (kStatus == MS::kSuccess)
    {
        kRotationSpeedVar.getValue(fRotationSpeedVar);
        bAnimatedSpeedVar = kRotationSpeedVar.isConnected();
    }

    MPlug kRotationIntitialAngle = kShape.findPlug(
        "Ni_ParticleRotationInitialAngle", &kStatus);

    if (kStatus == MS::kSuccess)
    {
        kRotationIntitialAngle.getValue(fInitialAngle);
        bAnimatedInitialAngle = kRotationIntitialAngle.isConnected();
    }

    MPlug kRotationIntitialAngleVar = kShape.findPlug(
        "Ni_ParticleRotationInitialAngleVar", &kStatus);

    if (kStatus == MS::kSuccess)
    {
        kRotationIntitialAngleVar.getValue(fInitialAngleVar);
        bAnimatedInitialAngleVar = kRotationIntitialAngleVar.isConnected();
    }

    // Check for Rotations near Zero
    if (NiOptimize::CloseTo(fRotationSpeed, 0.0f, 0.001f) &&
        NiOptimize::CloseTo(fRotationSpeedVar, 0.0f, 0.001f) &&
        NiOptimize::CloseTo(fInitialAngle, 0.0f, 0.001f) &&
        NiOptimize::CloseTo(fInitialAngleVar, 0.0f, 0.001f) &&
        !bAnimatedSpeed && !bAnimatedSpeedVar && !bAnimatedInitialAngle &&
        !bAnimatedInitialAngleVar)       
        return false;

    return true;
}
//---------------------------------------------------------------------------
void MyiParticleSystem::CreateInitialRotationSpeedCtlr()
{
    NiFloatInterpolator* pkInterp;

    if (!CreateFloatInterpolator(m_pMDtParticles->m_MObjectParticles, 
        "Ni_ParticleRotateSpeedDegrees", pkInterp, NI_PI / 180.0f))
        return;

    // Create the Controller
    NiPSysInitialRotSpeedCtlr* pkController = NiNew NiPSysInitialRotSpeedCtlr(
        "Rotation Modifier");

    // Set the interpolator and reset the extrema
    pkController->SetInterpolator(pkInterp);
    pkController->ResetTimeExtrema();
    pkController->SetCycleType(NiTimeController::LOOP);

    // Attach the Controller to the Particle System
    pkController->SetTarget(m_pkParticleSystem);
}
//---------------------------------------------------------------------------
void MyiParticleSystem::CreateInitialRotationSpeedVarCtlr()
{
    NiFloatInterpolator* pkInterp;

    if (!CreateFloatInterpolator(m_pMDtParticles->m_MObjectParticles, 
        "Ni_ParticleRotationSpeedVarDegrees", pkInterp, NI_PI / 180.0f))
        return;

    // Create the Controller
    NiPSysInitialRotSpeedVarCtlr* pkController = 
        NiNew NiPSysInitialRotSpeedVarCtlr("Rotation Modifier");

    // Set the interpolator and reset the extrema
    pkController->SetInterpolator(pkInterp);
    pkController->ResetTimeExtrema();
    pkController->SetCycleType(NiTimeController::LOOP);

    // Attach the Controller to the Particle System
    pkController->SetTarget(m_pkParticleSystem);
}
//---------------------------------------------------------------------------
void MyiParticleSystem::CreateRotationAngleCtlr()
{
    NiFloatInterpolator* pkInterp;

    if (!CreateFloatInterpolator(m_pMDtParticles->m_MObjectParticles, 
        "Ni_ParticleRotationInitialAngle", pkInterp, NI_PI / 180.0f))
        return;

    // Create the Controller
    NiPSysInitialRotAngleCtlr* pkController = NiNew NiPSysInitialRotAngleCtlr(
        "Rotation Modifier");

    // Set the interpolator and reset the extrema
    pkController->SetInterpolator(pkInterp);
    pkController->ResetTimeExtrema();
    pkController->SetCycleType(NiTimeController::LOOP);

    // Attach the Controller to the Particle System
    pkController->SetTarget(m_pkParticleSystem);
}
//---------------------------------------------------------------------------
void MyiParticleSystem::CreateRotationAngleVarCtlr()
{
    NiFloatInterpolator* pkInterp;

    if (!CreateFloatInterpolator(m_pMDtParticles->m_MObjectParticles, 
        "Ni_ParticleRotationInitialAngleVar", pkInterp, NI_PI / 180.0f))
        return;

    // Create the Controller
    NiPSysInitialRotAngleVarCtlr* pkController = 
        NiNew NiPSysInitialRotAngleVarCtlr("Rotation Modifier");

    // Set the interpolator and reset the extrema
    pkController->SetInterpolator(pkInterp);
    pkController->ResetTimeExtrema();
    pkController->SetCycleType(NiTimeController::LOOP);

    // Attach the Controller to the Particle System
    pkController->SetTarget(m_pkParticleSystem);
}
//---------------------------------------------------------------------------
void MyiParticleSystem::CreateBoundUpdateModifier()
{
    int iBoundSkip = 0;
    MStatus kStatus;

    MFnDependencyNode kParticles(m_pMDtParticles->m_MObjectParticles, 
        &kStatus);

    NIASSERT(kStatus == MS::kSuccess);

    GetExtraAttribute(kParticles, "Ni_ParticleSkipBoundUpdate", false, 
        iBoundSkip);

    NiPSysBoundUpdateModifier* pkModifier = NiNew NiPSysBoundUpdateModifier(
        "Bound Update", iBoundSkip);

    m_pkParticleSystem->AddModifier(pkModifier);
}
//---------------------------------------------------------------------------
void MyiParticleSystem::CreateForces()
{

    for (int iLoop = 0; iLoop < m_pMDtParticles->GetNumFields(); iLoop++)
    {
        // Determine what kind of field it is
        kMDtField Field;
        Field.Load(m_pMDtParticles->GetField(iLoop));

        switch (Field.m_lFieldType)
        {
        case kMDtField::GravityType:
            CreateGravityField(iLoop);
            break;
        case kMDtField::RadialType:
            CreateRadialField(iLoop);
            break;
        case kMDtField::DragType:
            CreateDragField(iLoop);
            break;
        case kMDtField::TurbulenceType:
            CreateTurbulenceField(iLoop);
            break;
        case kMDtField::VortexType:
            CreateVortexField(iLoop);
            break;
        case kMDtField::AirType:
            CreateAirField(iLoop);
            break;
        default:
            printf("Error:: Cannot Create Force for Field: %s the field is "
                "an unsupported type.\n", Field.m_szName );
        }
    }
}
//---------------------------------------------------------------------------
void MyiParticleSystem::CreateGravityField(int iField)
{
    kMDtGravityField GravityField;
    GravityField.Load(m_pMDtParticles->GetField(iField));
    
    GravityField.ScaleForLinearUnits(gExport.m_fLinearUnitMultiplier);
    
    NiNode* pkNode = gUserData.GetNode(m_pMDtParticles->GetField(iField));

    NiPoint3 kDirection( GravityField.m_fDirectionX, 
        GravityField.m_fDirectionY, GravityField.m_fDirectionZ);
    
    // Check for Zero
    if (NiOptimize::CloseTo(kDirection.x, 0.0f, 0.0000001f))
        kDirection.x = 0.0f;
    
    if (NiOptimize::CloseTo(kDirection.y, 0.0f, 0.0000001f))
        kDirection.y = 0.0f;
    
    if (NiOptimize::CloseTo(kDirection.z, 0.0f, 0.0000001f))
        kDirection.z = 0.0f;
    
    // Unitize the direction vector.
    kDirection.Unitize();
    
    NiPSysModifier* pkModifier;

    if (gExport.m_bUseOldParticleSystemFields)
    {
        pkModifier = (NiPSysModifier*)NiNew NiPSysGravityModifier(
            GravityField.m_szName, pkNode, kDirection, 
            GravityField.m_fAttenuation, GravityField.m_fMagnitude, 
            NiPSysGravityModifier::FORCE_PLANAR, 0.0f, 1.0f);
    }
    else
    {
        pkModifier = (NiPSysModifier*)NiNew NiPSysGravityFieldModifier(
            GravityField.m_szName, pkNode, GravityField.m_fMagnitude, 
            GravityField.m_fAttenuation, GravityField.m_bUseMaxDistance, 
            GravityField.m_fMaxDistance, kDirection);
    }

    m_pkParticleSystem->AddModifier(pkModifier);


    // Create Any Animated Field Controllers
    if (!gExport.m_bUseOldParticleSystemFields)
    {
        CreateFieldMagnitudeController(GravityField.m_MObjectField, 
            (NiPSysFieldModifier*)pkModifier, GravityField.m_szName);

        CreateFieldAttenuationController(GravityField.m_MObjectField, 
            (NiPSysFieldModifier*)pkModifier, GravityField.m_szName);

        CreateFieldMaxDistanceController(GravityField.m_MObjectField, 
            (NiPSysFieldModifier*)pkModifier, GravityField.m_szName);
    }
}
//---------------------------------------------------------------------------
void MyiParticleSystem::CreateRadialField(int iField)
{
    kMDtRadialField RadialField;
    RadialField.Load(m_pMDtParticles->GetField(iField));
    
    RadialField.ScaleForLinearUnits(gExport.m_fLinearUnitMultiplier);
    
    NiNode* pkNode = gUserData.GetNode(m_pMDtParticles->GetField(iField));

    NiPSysModifier* pkModifier;

    if (gExport.m_bUseOldParticleSystemFields)
    {
        // Radial Magnitude points out and we are oppisite
        pkModifier = (NiPSysModifier*)NiNew NiPSysGravityModifier(
            RadialField.m_szName, pkNode,
            NiPoint3::UNIT_X, RadialField.m_fAttenuation, 
            -RadialField.m_fMagnitude, 
            NiPSysGravityModifier::FORCE_SPHERICAL, 0.0f, 1.0f);
    }
    else
    {
        pkModifier = pkModifier = 
            (NiPSysModifier*)NiNew NiPSysRadialFieldModifier(
            RadialField.m_szName, pkNode, RadialField.m_fMagnitude, 
            RadialField.m_fAttenuation, RadialField.m_bUseMaxDistance, 
            RadialField.m_fMaxDistance, RadialField.m_fRadialType);
    }

    m_pkParticleSystem->AddModifier(pkModifier);

    // Create Any Animated Field Controllers
    if (!gExport.m_bUseOldParticleSystemFields)
    {
        CreateFieldMagnitudeController(RadialField.m_MObjectField, 
            (NiPSysFieldModifier*)pkModifier, RadialField.m_szName);

        CreateFieldAttenuationController(RadialField.m_MObjectField, 
            (NiPSysFieldModifier*)pkModifier, RadialField.m_szName);

        CreateFieldMaxDistanceController(RadialField.m_MObjectField, 
            (NiPSysFieldModifier*)pkModifier, RadialField.m_szName);
    }
}
//---------------------------------------------------------------------------
void MyiParticleSystem::CreateDragField(int iField)
{
    kMDtDragField DragField;
    DragField.Load(m_pMDtParticles->GetField(iField));
    DragField.ScaleForLinearUnits(gExport.m_fLinearUnitMultiplier);
    
    NiNode* pkNode = gUserData.GetNode(m_pMDtParticles->GetField(iField));
    
    NiPoint3 kDirection( DragField.m_fDirectionX, 
        DragField.m_fDirectionY, DragField.m_fDirectionZ);
    
    // Check for Zero
    if (NiOptimize::CloseTo(kDirection.x, 0.0f, 0.0000001f))
        kDirection.x = 0.0f;
    
    if (NiOptimize::CloseTo(kDirection.y, 0.0f, 0.0000001f))
        kDirection.y = 0.0f;
    
    if (NiOptimize::CloseTo(kDirection.z, 0.0f, 0.0000001f))
        kDirection.z = 0.0f;
    
    // Unitize the direction vector.
    kDirection.Unitize();
    
    NiPSysDragFieldModifier* pkModifier = NiNew NiPSysDragFieldModifier(
        DragField.m_szName, pkNode, DragField.m_fMagnitude, 
        DragField.m_fAttenuation, DragField.m_bUseMaxDistance,
        DragField.m_fMaxDistance, DragField.m_bUseDirection,
        kDirection);
   
    m_pkParticleSystem->AddModifier(pkModifier);


    // Create Any Animated Field Controllers
    CreateFieldMagnitudeController(DragField.m_MObjectField, 
        (NiPSysFieldModifier*)pkModifier, DragField.m_szName);

    CreateFieldAttenuationController(DragField.m_MObjectField, 
        (NiPSysFieldModifier*)pkModifier, DragField.m_szName);

    CreateFieldMaxDistanceController(DragField.m_MObjectField, 
        (NiPSysFieldModifier*)pkModifier, DragField.m_szName);

}
//---------------------------------------------------------------------------
void MyiParticleSystem::CreateTurbulenceField(int iField)
{
    kMDtTurbulenceField kTurbulenceField;
    kTurbulenceField.Load(m_pMDtParticles->GetField(iField));
    kTurbulenceField.ScaleForLinearUnits(gExport.m_fLinearUnitMultiplier);

    NiNode* pkNode = gUserData.GetNode(m_pMDtParticles->GetField(iField));


    // Scale the Magnitude for conversion from Maya. This is a fudge factor
    // that appears right.
    kTurbulenceField.m_fMagnitude /= 10.0f;


    NiPSysTurbulenceFieldModifier* pkModifier = 
        NiNew NiPSysTurbulenceFieldModifier(kTurbulenceField.m_szName, pkNode, 
        kTurbulenceField.m_fMagnitude, kTurbulenceField.m_fAttenuation, 
        kTurbulenceField.m_bUseMaxDistance, kTurbulenceField.m_fMaxDistance,
        kTurbulenceField.m_fFrequency);

    m_pkParticleSystem->AddModifier(pkModifier);

    // Create Any Animated Field Controllers
    CreateFieldMagnitudeController(kTurbulenceField.m_MObjectField, 
        (NiPSysFieldModifier*)pkModifier, kTurbulenceField.m_szName);

    CreateFieldAttenuationController(kTurbulenceField.m_MObjectField, 
        (NiPSysFieldModifier*)pkModifier, kTurbulenceField.m_szName);

    CreateFieldMaxDistanceController(kTurbulenceField.m_MObjectField, 
        (NiPSysFieldModifier*)pkModifier, kTurbulenceField.m_szName);

}
//---------------------------------------------------------------------------
void MyiParticleSystem::CreateVortexField(int iField)
{
    kMDtVortexField kVortexField;
    kVortexField.Load(m_pMDtParticles->GetField(iField));
    kVortexField.ScaleForLinearUnits(gExport.m_fLinearUnitMultiplier);

    NiNode* pkNode = gUserData.GetNode(m_pMDtParticles->GetField(iField));


    NiPoint3 kDirection( kVortexField.m_fAxisX, 
        kVortexField.m_fAxisY, kVortexField.m_fAxisZ);
    
    // Check for Zero
    if (NiOptimize::CloseTo(kDirection.x, 0.0f, 0.0000001f))
        kDirection.x = 0.0f;
    
    if (NiOptimize::CloseTo(kDirection.y, 0.0f, 0.0000001f))
        kDirection.y = 0.0f;
    
    if (NiOptimize::CloseTo(kDirection.z, 0.0f, 0.0000001f))
        kDirection.z = 0.0f;
    
    // Unitize the direction vector.
    kDirection.Unitize();


    NiPSysVortexFieldModifier* pkModifier = 
        NiNew NiPSysVortexFieldModifier(kVortexField.m_szName, pkNode, 
        kVortexField.m_fMagnitude, kVortexField.m_fAttenuation, 
        kVortexField.m_bUseMaxDistance, kVortexField.m_fMaxDistance,
        kDirection);

    m_pkParticleSystem->AddModifier(pkModifier);

    // Create Any Animated Field Controllers
    CreateFieldMagnitudeController(kVortexField.m_MObjectField, 
        (NiPSysFieldModifier*)pkModifier, kVortexField.m_szName);

    CreateFieldAttenuationController(kVortexField.m_MObjectField, 
        (NiPSysFieldModifier*)pkModifier, kVortexField.m_szName);

    CreateFieldMaxDistanceController(kVortexField.m_MObjectField, 
        (NiPSysFieldModifier*)pkModifier, kVortexField.m_szName);

}
//---------------------------------------------------------------------------
void MyiParticleSystem::CreateAirField(int iField)
{
    kMDtAirField kAirField;
    kAirField.Load(m_pMDtParticles->GetField(iField));
    kAirField.ScaleForLinearUnits(gExport.m_fLinearUnitMultiplier);

    NiNode* pkNode = gUserData.GetNode(m_pMDtParticles->GetField(iField));


    NiPoint3 kDirection( kAirField.m_fDirectionX, 
        kAirField.m_fDirectionY, kAirField.m_fDirectionZ);
    
    // Check for Zero
    if (NiOptimize::CloseTo(kDirection.x, 0.0f, 0.0000001f))
        kDirection.x = 0.0f;
    
    if (NiOptimize::CloseTo(kDirection.y, 0.0f, 0.0000001f))
        kDirection.y = 0.0f;
    
    if (NiOptimize::CloseTo(kDirection.z, 0.0f, 0.0000001f))
        kDirection.z = 0.0f;
    
    // Unitize the direction vector.
    kDirection.Unitize();


    NiPSysAirFieldModifier* pkModifier = 
        NiNew NiPSysAirFieldModifier(kAirField.m_szName, pkNode, 
        kAirField.m_fMagnitude, kAirField.m_fAttenuation, 
        kAirField.m_bUseMaxDistance, kAirField.m_fMaxDistance,
        kDirection, kAirField.m_fSpeed, kAirField.m_fInheritVelocity,
        kAirField.m_bInheritRotation, kAirField.m_bComponentOnly,
        kAirField.m_bEnableSpread, kAirField.m_fSpread);

    m_pkParticleSystem->AddModifier(pkModifier);

    // Create Any Animated Field Controllers
    CreateFieldMagnitudeController(kAirField.m_MObjectField, 
        (NiPSysFieldModifier*)pkModifier, kAirField.m_szName);

    CreateFieldAttenuationController(kAirField.m_MObjectField, 
        (NiPSysFieldModifier*)pkModifier, kAirField.m_szName);

    CreateFieldMaxDistanceController(kAirField.m_MObjectField, 
        (NiPSysFieldModifier*)pkModifier, kAirField.m_szName);

    CreateAirFieldAirFrictionController(kAirField.m_MObjectField, 
        (NiPSysFieldModifier*)pkModifier, kAirField.m_szName);

    CreateAirFieldInheritVelocityController(kAirField.m_MObjectField, 
        (NiPSysFieldModifier*)pkModifier, kAirField.m_szName);

    CreateAirFieldSpreadController(kAirField.m_MObjectField, 
        (NiPSysFieldModifier*)pkModifier, kAirField.m_szName);
}
//---------------------------------------------------------------------------
void MyiParticleSystem::CreateFieldMagnitudeController(MObject MField, 
    NiPSysFieldModifier* pkModifier, const char* pcFieldName)
{
    NiFloatInterpolator* pkInterp;

    if (!CreateFloatInterpolator(MField, "magnitude", pkInterp))
        return;

    // Create the Controller
    NiPSysFieldMagnitudeCtlr* pkController = NiNew NiPSysFieldMagnitudeCtlr(
        pcFieldName);

    // Set the interpolator and reset the extrema
    pkController->SetInterpolator(pkInterp);
    pkController->ResetTimeExtrema();
    pkController->SetCycleType(NiTimeController::LOOP);

    // Attach the Controller to the Particle System
    pkController->SetTarget(m_pkParticleSystem);

}
//---------------------------------------------------------------------------
 void MyiParticleSystem::CreateFieldAttenuationController(MObject MField, 
    NiPSysFieldModifier* pkModifier, const char* pcFieldName)
{
    NiFloatInterpolator* pkInterp;

    if (!CreateFloatInterpolator(MField, "attenuation", pkInterp))
        return;

    // Create the Controller
    NiPSysFieldAttenuationCtlr* pkController = 
        NiNew NiPSysFieldAttenuationCtlr(pcFieldName);

    // Set the interpolator and reset the extrema
    pkController->SetInterpolator(pkInterp);
    pkController->ResetTimeExtrema();
    pkController->SetCycleType(NiTimeController::LOOP);

    // Attach the Controller to the Particle System
    pkController->SetTarget(m_pkParticleSystem);
}
//---------------------------------------------------------------------------
 void MyiParticleSystem::CreateFieldMaxDistanceController(MObject MField, 
    NiPSysFieldModifier* pkModifier, const char* pcFieldName)
{
    NiFloatInterpolator* pkInterp;

    if (!CreateFloatInterpolator(MField, "maxDistance", pkInterp))
        return;

    // Create the Controller
    NiPSysFieldMaxDistanceCtlr* pkController = 
        NiNew NiPSysFieldMaxDistanceCtlr(pcFieldName);

    // Set the interpolator and reset the extrema
    pkController->SetInterpolator(pkInterp);
    pkController->ResetTimeExtrema();
    pkController->SetCycleType(NiTimeController::LOOP);

    // Attach the Controller to the Particle System
    pkController->SetTarget(m_pkParticleSystem);
}
//---------------------------------------------------------------------------
void MyiParticleSystem::CreateAirFieldAirFrictionController(MObject MField, 
        NiPSysFieldModifier* pkModifier, const char* pcFieldName)
{
    NiFloatInterpolator* pkInterp;

    if (!CreateFloatInterpolator(MField, "speed", pkInterp))
        return;

    // Create the Controller
    NiPSysAirFieldAirFrictionCtlr* pkController = 
        NiNew NiPSysAirFieldAirFrictionCtlr(pcFieldName);

    // Set the interpolator and reset the extrema
    pkController->SetInterpolator(pkInterp);
    pkController->ResetTimeExtrema();
    pkController->SetCycleType(NiTimeController::LOOP);

    // Attach the Controller to the Particle System
    pkController->SetTarget(m_pkParticleSystem);
}
//---------------------------------------------------------------------------
void MyiParticleSystem::CreateAirFieldInheritVelocityController(
    MObject MField, NiPSysFieldModifier* pkModifier, const char* pcFieldName)
{
    NiFloatInterpolator* pkInterp;

    if (!CreateFloatInterpolator(MField, "inheritVelocity", pkInterp))
        return;

    // Create the Controller
    NiPSysAirFieldInheritVelocityCtlr* pkController = 
        NiNew NiPSysAirFieldInheritVelocityCtlr(pcFieldName);

    // Set the interpolator and reset the extrema
    pkController->SetInterpolator(pkInterp);
    pkController->ResetTimeExtrema();
    pkController->SetCycleType(NiTimeController::LOOP);

    // Attach the Controller to the Particle System
    pkController->SetTarget(m_pkParticleSystem);
}
//---------------------------------------------------------------------------
void MyiParticleSystem::CreateAirFieldSpreadController(MObject MField, 
        NiPSysFieldModifier* pkModifier, const char* pcFieldName)
{
    NiFloatInterpolator* pkInterp;

    if (!CreateFloatInterpolator(MField, "spread", pkInterp))
        return;

    // Create the Controller
    NiPSysAirFieldSpreadCtlr* pkController = 
        NiNew NiPSysAirFieldSpreadCtlr(pcFieldName);

    // Set the interpolator and reset the extrema
    pkController->SetInterpolator(pkInterp);
    pkController->ResetTimeExtrema();
    pkController->SetCycleType(NiTimeController::LOOP);

    // Attach the Controller to the Particle System
    pkController->SetTarget(m_pkParticleSystem);
}
//---------------------------------------------------------------------------
void MyiParticleSystem::CreateColliders(
    NiTList<MyiParticleSystem*>& pkParticleSystems)
{
    
    CheckEvents(pkParticleSystems);


    if (m_pMDtParticles->GetNumColliders() == 0)
        return;


    NiPSysColliderManager* pkManager = NiNew NiPSysColliderManager(
        "Collider Manager");

    int iLoop;
    for (iLoop = 0; iLoop < m_pMDtParticles->GetNumColliders(); iLoop++)
    {
        kMDtCollider MDtCollider;

        if (MDtCollider.Load(m_pMDtParticles->m_MObjectParticles, iLoop) )
        {
            MDtCollider.ScaleForLinearUnits(gExport.m_fLinearUnitMultiplier);

            MFnDagNode dgNode(MDtCollider.m_MObjectColliderTransform);


            if (CheckForExtraAttribute(dgNode, "Ni_SphericalCollider", 
                false))
            {
                CreateSphericalCollider(&MDtCollider, pkManager);
            }
            else if (CheckForExtraAttribute(dgNode, "Ni_PlanarCollider", 
                false))
            {
                CreatePlanarCollider(&MDtCollider, pkManager);
            }
            else if (MDtCollider.m_fRadius == 0.0f)
            {
                CreatePlanarCollider(&MDtCollider, pkManager);
            }
            else
            {
                CreateSphericalCollider(&MDtCollider, pkManager);
            }
        }
    }

    m_pkParticleSystem->AddModifier(pkManager);
}
//---------------------------------------------------------------------------
void MyiParticleSystem::CreateSphericalCollider(kMDtCollider* pkCollider,
    NiPSysColliderManager* pkManager)
{
    int iNumParticles = NiMax(m_iNumEmittedParticles, m_iNumSplitParticles);
    NiNode* pkNode = gUserData.GetNode(
        pkCollider->m_MObjectColliderTransform);

    //create the bound node.
    NiNode* pkSphereNode = NiNew NiNode;
    pkSphereNode->SetTranslate(NiPoint3::ZERO);
    pkSphereNode->SetRotate(NiMatrix3::IDENTITY);
    pkSphereNode->SetScale(1.0f);

    char acBuff[256];
    NiSprintf(acBuff, 256, "%s::SphereCollider", pkNode->GetName());
    pkSphereNode->SetName(acBuff);

    pkNode->AttachChild(pkSphereNode);

    // Create the Collider
    NiPSysSphericalCollider* pkSphereCollider = NiNew NiPSysSphericalCollider(
        pkCollider->m_fResilience, (m_pkCollisionSpawn != NULL), 
        m_bDieAtCollision, m_pkCollisionSpawn, pkSphereNode);

    pkManager->AddCollider(pkSphereCollider);

}
//---------------------------------------------------------------------------
void MyiParticleSystem::SetSphereColliderNodePosistionAndScale()
{
    // Find the Collider Manager
    NiPSysModifier* pkModifier = 
        m_pkParticleSystem->GetModifierByName("Collider Manager");

    // Check for no colliders
    if (!pkModifier)
        return;

    NiPSysColliderManager* pkColliderManager = NiDynamicCast(
        NiPSysColliderManager, pkModifier);

    if (!pkColliderManager)
        return;

    NiPSysCollider* pkCurrent = pkColliderManager->GetColliders();

    while (pkCurrent)
    {
        NiPSysSphericalCollider* pkSphereCollider = NiDynamicCast(
            NiPSysSphericalCollider, pkCurrent);

        if (pkSphereCollider)
        {
            // Update the Sphere Parent to time zero
            NiNode* pkSphere = (NiNode*)pkSphereCollider->GetColliderObj();
            NiNode* pkParent = pkSphere->GetParent();

            pkParent->Update(0.0f);

            // Set the scale of the sphere to be the difference between
            // the world bound radius and world scale
            float fWorldBoundRadius = pkParent->GetWorldBound().GetRadius();
            float fWorldScale = pkParent->GetWorldScale();

            pkSphere->SetScale(fWorldBoundRadius / fWorldScale);
            pkSphere->SetTranslate(pkParent->GetWorldBound().GetCenter() - 
                pkParent->GetWorldTranslate());
            pkSphere->Update(0.0f);
        }
        pkCurrent = pkCurrent->GetNext();
    }
}
//---------------------------------------------------------------------------
void MyiParticleSystem::CreatePlanarCollider(kMDtCollider* pkCollider,
    NiPSysColliderManager* pkManager)
{
    int iNumParticles = NiMax(m_iNumEmittedParticles, m_iNumSplitParticles);
    NiNode* pkNode = gUserData.GetNode(
        pkCollider->m_MObjectColliderTransform);

    // Check for Non-Uniform Scale
    float fSX, fSY, fSZ;
    fSX = fSY = fSZ = 1.0f;

    DtShapeGetFinalNonUniformNonAnimatedScale(
        pkCollider->m_MObjectColliderTransform, &fSX, &fSY, &fSZ);


    NiPSysPlanarCollider* pkPlanarCollider = NiNew NiPSysPlanarCollider(
        pkCollider->m_fResilience, (m_pkCollisionSpawn != NULL), 
        m_bDieAtCollision, m_pkCollisionSpawn, pkNode,
        pkCollider->m_fWidth * fSX, pkCollider->m_fHeight * fSZ, 
        NiPoint3::UNIT_X, NiPoint3::UNIT_Z);

    pkManager->AddCollider(pkPlanarCollider);

}
//---------------------------------------------------------------------------
void MyiParticleSystem::SetPlanarColliderWidthAndHeightFromGeometry()
{
    // Find the Collider Manager
    NiPSysModifier* pkModifier = 
        m_pkParticleSystem->GetModifierByName("Collider Manager");

    // Check for no colliders
    if (!pkModifier)
        return;

    NiPSysColliderManager* pkColliderManager = NiDynamicCast(
        NiPSysColliderManager, pkModifier);

    if (!pkColliderManager)
        return;

    NiPSysCollider* pkCurrent = pkColliderManager->GetColliders();

    while (pkCurrent)
    {
        NiPSysPlanarCollider* pkPlanarCollider = NiDynamicCast(
            NiPSysPlanarCollider, pkCurrent);

        // Check for recomputing the width and height by 
        if (pkPlanarCollider && (pkPlanarCollider->GetWidth() == 0.0f) &&
            (pkPlanarCollider->GetHeight() == 0.0f))
        {
            NiPoint3 kMin(NI_INFINITY, NI_INFINITY, NI_INFINITY);
            NiPoint3 kMax(-NI_INFINITY, -NI_INFINITY, -NI_INFINITY);

            NiAVObject* pkColliderObj = pkPlanarCollider->GetColliderObj();

            pkColliderObj->Update(0.0f);

            GetAxisAlignedBoundingBox(pkColliderObj, kMin, kMax);

            // Translate back to Object space from world 
            NiTransform kInvert;

            pkColliderObj->GetWorldTransform().Invert(kInvert);

            NiPoint3 kLocalMin = kInvert * kMin;
            NiPoint3 kLocalMax = kInvert * kMax;

            // Find the Width and Height
            pkPlanarCollider->SetHeight(kLocalMax.z - kLocalMin.z);

            pkPlanarCollider->SetWidth(kLocalMax.x - kLocalMin.x);
        }
        pkCurrent = pkCurrent->GetNext();
    }
}
//---------------------------------------------------------------------------
void MyiParticleSystem::GetAxisAlignedBoundingBox(NiAVObject* pkObject, 
    NiPoint3& kMin, NiPoint3& kMax)
{

    NiGeometry* pkGeometry = NiDynamicCast(NiGeometry, pkObject);

    if (pkGeometry)
    {
        NiPoint3* pkVerts = pkGeometry->GetVertices();
        NiPoint3 kWorldVert;
        const NiTransform& kWorld = pkGeometry->GetWorldTransform();

        // Transform each vert and determine if it extends the min and Max
        unsigned short usCurrent;
        for (usCurrent = 0; usCurrent < pkGeometry->GetVertexCount(); 
            usCurrent++)
        {
            kWorldVert = kWorld * pkVerts[usCurrent];

            kMin.x = NiMin(kMin.x, kWorldVert.x);
            kMax.x = NiMax(kMax.x, kWorldVert.x);
            kMin.y = NiMin(kMin.y, kWorldVert.y);
            kMax.y = NiMax(kMax.y, kWorldVert.y);
            kMin.z = NiMin(kMin.z, kWorldVert.z);
            kMax.z = NiMax(kMax.z, kWorldVert.z);
        }
    }


    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode)
    {
        int iLoop;
        for (iLoop = 0; iLoop < (int)pkNode->GetArrayCount(); iLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(iLoop);
            if (pkChild != NULL) 
            {
                GetAxisAlignedBoundingBox(pkChild, kMin, kMax);
            }
        }
    }
}
//---------------------------------------------------------------------------
void MyiParticleSystem::CheckEvents(
    NiTList<MyiParticleSystem*>& pkParticleSystems)
{
    MString   command;
    MStringArray result;
    MStatus stat = MS::kSuccess;
    int iValue = 0;

    MFnDagNode kDagShape(m_pMDtParticles->m_MObjectParticles);
    MFnDagNode kDagTransform(m_pMDtParticles->m_MObjectParent);


    command = MString("event -ls ") + kDagShape.name();

    stat = MGlobal::executeCommand( command, result );

    if ( stat != MS::kSuccess ) 
        return;

    if (result.length() == 0)
        return;

    NIASSERT(result.length() == 1);

    // Get Die on collide
    command = MString("event -name ") + result[0] + MString(" -q -die ") + 
        kDagTransform.name();

    stat = MGlobal::executeCommand(command, iValue);
    NIASSERT(stat == MS::kSuccess);

    m_bDieAtCollision = iValue > 0;

    // Get the number of emitted particles
    command = MString("event -name ") + result[0] + MString(" -q -emit ") +
        kDagTransform.name();

    stat = MGlobal::executeCommand(command, m_iNumEmittedParticles);
    NIASSERT(stat == MS::kSuccess);

    // Get the number of split particles
    command = MString("event -name ") + result[0] + MString(" -q -split ") +
        kDagTransform.name();

    stat = MGlobal::executeCommand(command, m_iNumSplitParticles);
    NIASSERT(stat == MS::kSuccess);

    // Get the Random Flag
    command = MString("event -name ") + result[0] + MString(" -q -random ") +
        kDagTransform.name();

    stat = MGlobal::executeCommand(command, iValue);
    NIASSERT(stat == MS::kSuccess);

    bool bRandomEmittedParticles = iValue > 0;

    // Get the Spread Flag
    double dSpread;
    command = MString("event -name ") + result[0] + MString(" -q -spread ") +
        kDagTransform.name();

    stat = MGlobal::executeCommand(command, dSpread);
    NIASSERT(stat == MS::kSuccess);

    // Get the Target Particle System Name
    command = MString("event -name ") + result[0] + MString(" -q -target ") +
        kDagTransform.name();

    MString kTarget;
    stat = MGlobal::executeCommand(command, kTarget);
    NIASSERT(stat == MS::kSuccess);

    // Search for the Target Particle Shape
    NiTListIterator kIter = pkParticleSystems.GetHeadPos();
    float fLifeSpan = 0.0f;
    float fLifeSpanVar = 0.0f;

    while (kIter != NULL)
    {
        if (pkParticleSystems.Get(kIter)->MatchesParticleShapeName(kTarget))
        {
            m_pkCollisionParticleSystem = 
                pkParticleSystems.Get(kIter)->GetParticleSystem();

            pkParticleSystems.Get(kIter)->GetLifeSpan(fLifeSpan, 
                fLifeSpanVar);
            break;
        }

        kIter = pkParticleSystems.GetNextPos(kIter);
    }

    if (kIter == NULL)
        return;

    unsigned short usMin = m_iNumEmittedParticles;
    unsigned short usMax = m_iNumEmittedParticles;

    // In Maya Random means make 1 to N particles
    if (bRandomEmittedParticles)
    {
        usMin = 1;
    }


    // Create the necessary emitter
    m_pkCollisionSpawn = NiNew NiPSysSpawnModifier( "Collision Spawn", 1,
        1.0f, usMin, usMax, 0.0f, (float)dSpread, fLifeSpan, 
        fLifeSpanVar);

    m_pkCollisionParticleSystem->AddModifier(m_pkCollisionSpawn);
}
//---------------------------------------------------------------------------
void MyiParticleSystem::CreateResetOnLoop()
{
    MFnDagNode dgNode(m_pMDtParticles->m_MObjectEmitter);

    bool bResetOnLoop = true;

    GetExtraAttribute(dgNode, "Ni_ResetOnLoop", true, bResetOnLoop);

    if (bResetOnLoop)
    {
        // Find the emitter controller
        NiTimeController* pkEmitterCtrl = m_pkParticleSystem->GetController(
            &NiPSysEmitterCtlr::ms_RTTI);

        if (pkEmitterCtrl)
        {
            NiPSysResetOnLoopCtlr* pkController = 
                NiNew NiPSysResetOnLoopCtlr();
            
            pkController->SetBeginKeyTime( pkEmitterCtrl->GetBeginKeyTime());
            pkController->SetEndKeyTime( pkEmitterCtrl->GetEndKeyTime());
            
            pkController->SetTarget(m_pkParticleSystem);
        }
    }
}
//---------------------------------------------------------------------------
void MyiParticleSystem::ConvertGravityDirectionToWorldSpace()
{
    // Find Each Gravity Modifier
    unsigned int uiModifier;
    for (uiModifier = 0; uiModifier < m_pkParticleSystem->GetModifierCount(); 
        uiModifier++)
    {
        NiPSysModifier* pkModifier = m_pkParticleSystem->GetModifierAt(
            uiModifier);
            
        NiPSysGravityModifier* pkGravity = NiDynamicCast(
            NiPSysGravityModifier, pkModifier);

        if (pkGravity)
        {
            NiPoint3 kAxis = pkGravity->GetGravityAxis();

            // Convert to World Space because Maya direction are in world 
            // space
            NIASSERT(pkGravity->GetGravityObj());
            
            // Update Gravity Object so we can convert to world space
            pkGravity->GetGravityObj()->Update(0.0f);

            NiPoint3 kWorldAxis = 
                pkGravity->GetGravityObj()->GetWorldRotate() * kAxis;

            pkGravity->SetGravityAxis(kWorldAxis);

        }
    }
}
//---------------------------------------------------------------------------
void MyiParticleSystem::ConvertDragDirectionToWorldSpace()
{
    // Find Each Gravity Modifier
    unsigned int uiModifier;
    for (uiModifier = 0; uiModifier < m_pkParticleSystem->GetModifierCount(); 
        uiModifier++)
    {
        NiPSysModifier* pkModifier = m_pkParticleSystem->GetModifierAt(
            uiModifier);
            
        NiPSysDragFieldModifier* pkDrag = NiDynamicCast(
            NiPSysDragFieldModifier, pkModifier);

        if (pkDrag)
        {
            NiPoint3 kAxis = pkDrag->GetDirection();

            // Convert to World Space because Maya direction are in world 
            // space
            NIASSERT(pkDrag->GetFieldObj());
            
            // Update Drag Object so we can convert to world space
            pkDrag->GetFieldObj()->Update(0.0f);

            NiPoint3 kWorldAxis = 
                pkDrag->GetFieldObj()->GetWorldRotate() * kAxis;

            pkDrag->SetDirection(kWorldAxis);

        }
    }
}
//---------------------------------------------------------------------------
void MyiParticleSystem::DeleteCollisionGeometry()
{
    // Find the Collider Manager
    NiPSysModifier* pkModifier = 
        m_pkParticleSystem->GetModifierByName("Collider Manager");

    // Check for no colliders
    if (!pkModifier)
        return;

    NiPSysColliderManager* pkColliderManager = NiDynamicCast(
        NiPSysColliderManager, pkModifier);

    if (!pkColliderManager)
        return;

    NiPSysCollider* pkCurrent = pkColliderManager->GetColliders();

    while (pkCurrent)
    {
        NiNode* pkColliderNode = NULL;
        NiNode* pkDeleteNode = NULL;

        // Check for A sphere collider
        NiPSysSphericalCollider* pkSphereCollider = NiDynamicCast(
            NiPSysSphericalCollider, pkCurrent);

        if (pkSphereCollider)
        {
            pkColliderNode = (NiNode*)pkSphereCollider->GetColliderObj();
            pkDeleteNode = pkColliderNode->GetParent();
        }

        // Check for A Planar Collider
        NiPSysPlanarCollider* pkPlanarCollider = NiDynamicCast(
            NiPSysPlanarCollider, pkCurrent);

        if (pkPlanarCollider)
        {
            pkColliderNode = (NiNode*)pkPlanarCollider->GetColliderObj();
            pkDeleteNode = pkColliderNode;
        }

        // Check for the Delete Attribute
        int iShape = gUserData.GetShapeNum(pkDeleteNode);
        MObject kMObject;
        gMDtObjectGetTransform(iShape, kMObject );

        MFnDagNode kDagNode(kMObject);

        bool bDeleteObject = false;
        if (GetExtraAttribute(kDagNode, "Ni_ColliderDeleteOnExport", false,
            bDeleteObject) && bDeleteObject)
        {
            // Remove all children from the delete node except spheres node
            for (int iLoop = 0; iLoop < (int)pkDeleteNode->GetArrayCount(); 
                iLoop++)
            {
                NiAVObject* pkChild = pkDeleteNode->GetAt(iLoop);
                if ((pkChild != NULL) && (pkChild != pkColliderNode)) 
                {
                    pkDeleteNode->DetachChildAt(iLoop);
                }
            }
        }

        // Increment to the next 
        pkCurrent = pkCurrent->GetNext();
    }
}
//---------------------------------------------------------------------------
void MyiParticleSystem::CloneMeshParticles()
{
    // Find the Collider Manager
    NiPSysModifier* pkModifier = 
        m_pkParticleSystem->GetModifierByName("Mesh Update Modifier");

    // Check for no colliders
    if (!pkModifier)
        return;

    // Create the Particle Mesh Modifier
    NiPSysMeshUpdateModifier* pkMeshModifier = 
        NiDynamicCast(NiPSysMeshUpdateModifier, pkModifier);

    if (!pkMeshModifier)
        return;

    // Clone each mesh
    unsigned int uiLoop;
    for (uiLoop = 0; uiLoop < pkMeshModifier->GetMeshCount(); uiLoop++)
    {
        // Clone the object
        NiNode* pkClone = (NiNode*)pkMeshModifier->GetMeshAt(uiLoop)->Clone();

        // Guarantee you can see the object
        pkClone->SetAppCulled(false);

        pkMeshModifier->SetMeshAt(uiLoop, pkClone);
    }
}
//---------------------------------------------------------------------------
void MyiParticleSystem::CreateMaterialAndTexture(
    MyiMaterialManager* pkMaterialManager, 
    MyiTextureManager* pkTextureManager)
{

    NiMaterialProperty *pMaterial = NULL;
    NiSpecularProperty *pSpecular = NULL;
    NiVertexColorProperty *pVertexColor = NULL;
    NiAlphaProperty *pAlpha = NULL;
    NiTexturingProperty* pTexturingProperty = NULL;
    NiZBufferProperty* pZBufferProperty = NULL;

        // If there is no Texture get the particle color
    if (m_pMDtEmitter &&
        !m_pMDtEmitter->m_MObjectTextureName.isNull())
    {

        // Create the Texturing Property
        pTexturingProperty = NiNew NiTexturingProperty;
        NiTexturingProperty::Map* pNewMap = NiNew NiTexturingProperty::Map;

        NiTexturePtr pTexture = pkTextureManager->GetTexture(
            m_pMDtEmitter->m_iTextureID);

        pNewMap->SetTexture(pTexture);
        pTexturingProperty->SetBaseMap(pNewMap);


        // Check for an alpha property
        if (pkTextureManager->NeedsAlphaProperty(m_pMDtEmitter->m_iTextureID))
        {
            pAlpha = NiNew NiAlphaProperty;
            pAlpha->SetSrcBlendMode( NiAlphaProperty::ALPHA_SRCALPHA);
            pAlpha->SetDestBlendMode(NiAlphaProperty::ALPHA_INVSRCALPHA);
            pAlpha->SetAlphaBlending(true);


            // Only Set the ZBuffer of NON Mesh Particles
            if (!NiIsKindOf(NiMeshParticleSystem, m_pkParticleSystem))
            {
                // Don't add a ZBuffer property if on is aleady there
                if (!m_pkParticleSystem->GetProperty(NiProperty::ZBUFFER))
                {
                    // Create a ZBuffer Property for the Particles so they 
                    // Draw correctly The ZBuffer property is only needed 
                    // if the particles have Alpha and a Alpha accumulator 
                    // will take care of sorting objects
                    pZBufferProperty = NiNew NiZBufferProperty();

                    pZBufferProperty->SetZBufferTest(true);
                    pZBufferProperty->SetZBufferWrite(false);
                }
            }
        }


        // Attach the properties
        if (pMaterial)
        {
            m_pkParticleSystem->AttachProperty(pMaterial);
        }
        
        
        if (pVertexColor)
        {
            m_pkParticleSystem->AttachProperty(pVertexColor);
        }
        
        if (pAlpha)
        {
            m_pkParticleSystem->AttachProperty(pAlpha);
        }
        
        if (pTexturingProperty)
        {
            m_pkParticleSystem->AttachProperty(pTexturingProperty);
        }
        
        if (pSpecular)
        {
            m_pkParticleSystem->AttachProperty((NiProperty*)pSpecular);
        }
        
        if (pZBufferProperty)
        {
            m_pkParticleSystem->AttachProperty(pZBufferProperty);
        }
    }
    else
    {
        // There is no Shader attached to the Particles
        if (m_pMDtParticles->m_iMaterialID == -1)
            return;

        bool bHasAlpha = false;

        MyiTriShape::CreateMultiTexture( m_pMDtParticles->m_iMaterialID, 
            pkTextureManager, pTexturingProperty, bHasAlpha);

        // Create the Material Properties
        MyiTriShape::GetMaterialProperties( -1, -1, 
            m_pMDtParticles->m_iMaterialID, pkMaterialManager, 
            pTexturingProperty, bHasAlpha, true, 
            (NiAVObject*)m_pkParticleSystem);

        // Assign the Texture Property
        if (pTexturingProperty != NULL)
        {
            // Make sure we don't have Maya's default texture transform applied
            NiTexturingProperty::Map *pkBase = 
                pTexturingProperty->GetBaseMap();
            if (pkBase)
            {
                if (pkBase->GetTextureTransform())
                {
                    NiDelete pkBase->GetTextureTransform();
                    pkBase->SetTextureTransform(NULL);
                }
            }
            m_pkParticleSystem->AttachProperty(pTexturingProperty);
        }


        unsigned int uiNBTUVSource = 
            NiShaderDesc::BINORMALTANGENTUVSOURCEDEFAULT;
        pkMaterialManager->AddPixelShaderAndAttributes(
            m_pMDtParticles->m_iMaterialID, m_pkParticleSystem, uiNBTUVSource);


        // Only Set the ZBuffer of NON Mesh Particles
        if (!NiIsKindOf(NiMeshParticleSystem, m_pkParticleSystem))
        {
            // Don't add a ZBuffer property if on is aleady there
            if (!m_pkParticleSystem->GetProperty(NiProperty::ZBUFFER))
            {
                // Create a ZBuffer Property for the Particles so they 
                // Draw correctly The ZBuffer property is only needed if 
                // the particles have Alpha and a Alpha accumulator will 
                // take care of sorting objects
                pZBufferProperty = NiNew NiZBufferProperty();

                pZBufferProperty->SetZBufferTest(true);
                pZBufferProperty->SetZBufferWrite(false);

                m_pkParticleSystem->AttachProperty(pZBufferProperty);
            }
        }
    }
}
//---------------------------------------------------------------------------
void MyiParticleSystem::CreateZBufferProperties()
{
    MFnDagNode dgNode(m_pMDtParticles->m_MObjectParticles);

    MyiNode::AddZBufferProperties(m_pkParticleSystem, dgNode);
}
//---------------------------------------------------------------------------

