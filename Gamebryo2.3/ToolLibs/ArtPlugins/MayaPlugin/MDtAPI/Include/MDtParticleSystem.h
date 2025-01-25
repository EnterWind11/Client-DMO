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

// MDtParticleSystem.h

#ifndef MDTPARTICLESYSTEM_H
#define MDTPARTICLESYSTEM_H

#include <MDt.h>
#include <MDtExt.h>



class kMDtEmitter
{
public:
    kMDtEmitter();
    ~kMDtEmitter();


    bool Load(MObject PSystem, MObject Parent);


    long        m_lShapeIndex;
    char*       m_szName;
    MObject     m_MObjectEmitter;
    MObject     m_MObjectParent;
    MObject     m_MObjectParticles;
    MObject     m_MObjectEmitterSurface;

    // Transform Attributes
    float       m_fTranslateX;
    float       m_fTranslateY;
    float       m_fTranslateZ;
    float       m_fRotateX;
    float       m_fRotateY;
    float       m_fRotateZ;
    float       m_fScaleX;
    float       m_fScaleY;
    float       m_fScaleZ;

    // Basic Emitter Attributes
    long        m_lEmitterType;
    float       m_fParticlesPerSecond;
    bool        m_bScaleRateByObjectSize;
    bool        m_bUsePointPerRates;
    bool        m_bNeedParentUV;            // (Nurbs Surfaces Only)
    long        m_lCycleEmissionType;
    float       m_fCycleInterval;
    float       m_fDeltaTimeCycle;

    // Distance / Direction Attributes
    float       m_fMinDistance;
    float       m_fMaxDistance;
    float       m_fDirectionX;
    float       m_fDirectionY;
    float       m_fDirectionZ;
    float       m_fSpread;

    // Basic Emission Speed Attributes
    float       m_fSpeed;
    float       m_fSpeedRandom;
    float       m_fTangentSpeed;
    float       m_fNormalSpeed;

    // Volume Emitter Attributes
    long        m_lVolumeShapeType;
    float       m_fVolumeOffsetX;
    float       m_fVolumeOffsetY;
    float       m_fVolumeOffsetZ;
    float       m_fVolumeSweep;
    float       m_fSectionRadius;

    // Volume Speed Attributes
    float       m_fAwayFromCenter;
    float       m_fAwayFromAxis;
    float       m_fAlongAxis;
    float       m_fAroundAxis;
    float       m_fRandomDirection;
    float       m_fDirectionalSpeed;
    bool        m_bScaleSpeedBySize;
    bool        m_bDisplaySpeed;

    // Texture Emission Attributes (Nurbs Surfaces Only)
    //          m_ParticleColor;
    MObject     m_MObjectTextureName;
    char*       m_szParticleTextureFileName;
    int         m_iTextureID;

    float       m_fParticleColorR;
    float       m_fParticleColorG;
    float       m_fParticleColorB;
    bool        m_bInheritColor;
    bool        m_bInheritOpacity;
    bool        m_bUseLuminance;
    bool        m_bInvertOpacity;

    //          m_TextureRate
    float       m_fTextureRateR;
    float       m_fTextureRateG;
    float       m_fTextureRateB;
    bool        m_bEnableTextureRate;
    bool        m_bEmitFromDark;

    
    enum 
    {
        DIRECTIONAL = 0,
        OMNI,
        SURFACE,
        CURVE,
        VOLUME
    };

    enum
    {
        CUBE = 0,
        SPHERE,
        CYLINDER,
        CONE,
        TORUS
    };
};


class kMDtParticles
{
public:

    kMDtParticles();
    ~kMDtParticles();

    void Load(MObject PSystem, MObject Parent, MDagPath shapePath);

        // Fields which affect the Particle 
    int GetNumFields();
    MObject GetField(int iFieldNum);

    int GetNumColliders();

    void SetShader();
    void SetMaterialID();

    MObject m_MObjectParticles;
    MDagPath m_ShapePath;
    MObject m_MObjectParent;
    MObject m_MObjectEmitter;

    MObject m_MObjectShader;
    int m_iMaterialID;
    int m_iTextureID;


    char* m_szName;

    long    m_lRenderType;
/*
renderVolume
visibleFraction
motionBlur
*/
    bool    m_bVisibleInReflections;
    bool    m_bVisibleInRefractions;
    bool    m_bCastsShadows;
/*
maxVisibilitySamplesOverride
maxVisibilitySamples
geometryAntialiasingOverride
antialiasingLevel
shadingSamplesOverride
shadingSamples
maxShadingSamples
volumeSamplesOverride
volumeSamples
*/
    bool    m_bPrimaryVisibility;
/*
referenceObject
compInstObjGroups
compObjectGroups
compObjectGrpCompList
compObjectGroupId
position
rampPosition
centroid
lastPosition
velocity
rampVelocity
lastVelocity
acceleration
rampAcceleration
force
inputForce
worldPosition
worldCentroid
lastWorldPosition
worldVelocity
worldVelocityInObjectSpace
lastWorldVelocity
lastWorldMatrix
position0
velocity0
acceleration0
emitterId0
useStartupCache
startupCachePath
startupCacheFrame
cachedPosition
lastCachedPosition
cachedWorldPosition
cachedWorldCentroid
cachedVelocity
cachedWorldVelocity
count
computingCount
mass
mass0
massCache
particleId
particleId0
idCache
idMapping
nextId
nextId0
birthTime
birthTime0
birthTimeCache
age
age0
ageCache
emission
emitterId
dieOnEmissionVolumeExit
isFull
NiNewParticles
collisionEvents
death
*/
    long    m_lLifespanMode;
    float   m_fLifespanRandom;
//finalLifespanPP
    float   m_fGeneralSeed;
/*
randState
randStateX
randStateY
randStateZ
*/
    bool    m_bExpressionsAfterDynamics;
/*
executeCreationExpression
executeRuntimeExpression
input
output
time
frame
internalRuntimeExpression
internalCreationExpression
currentParticle
diedLastTime
netEmittedLastTime
startEmittedIndex
*/
    bool    m_bIsDynamic;
    float   m_fDynamicsWeight;
    bool    m_bForcesInWorld;
    float   m_fConserve;

    bool    m_bEmissionInWorld;
    float   m_fMaxCount;
    float   m_fLevelOfDetail;
    float   m_fInheritFactor;

    float   m_fSeed;
    
//fieldData
//emitterData
//forceDynamics

    float   m_fCurrentTime;
/*  
currentTimeSave
evaluationTime
currentSceneTime
lastTimeEvaluated
lastSceneTime
cachedTime
timeStepSize
sceneTimeStepSize
*/

    float   m_fStartFrame;
    float   m_fStartTime;
/*
inputGeometry
inputGeometryPoints
inputGeometrySpace
enforceCountFromHistory
targetGeometry
targetGeometryWorldMatrix
targetGeometrySpace
*/
    float   m_fGoalSmoothness;
/*
goalGeometry
goalWeight
goalActive
cacheData
cacheWidth
*/
    bool    m_bCollisions;
    float   m_fTraceDepth;
/*
collisionData
collisionGeometry
collisionResilience
collisionFriction
collisionRecords

totalEventCount
eventTest
lastTotalEventCount
eventSeed
eventRandState
eventRandStateX
eventRandStateY
eventRandStateZ
eventTarget
eventName
eventValid
eventCount
eventEmit
eventSplit
eventDie
eventRandom
eventSpread
eventProc

instanceData
instanceAttributeMapping
instancePointData
debugDraw
numberOfEvents
eventNameCount
fieldConnections
collisionConnections

connectionsToMe
auxiliariesOwned
emitterConnections
inheritColor
shapeNameMsg
doDynamics
doEmission
forceEmission
doAge
agesLastDone
timeLastComputed
parentMatrixDirty
NiNewFileFormat
*/
    bool    m_bDepthSort;
    long    m_lParticleRenderType;
/*
disableCloudAxis
normalizeVelocity
samplerPerParticleData
centroidX
centroidY
centroidZ
worldCentroidX
worldCentroidY
worldCentroidZ
cachedWorldCentroidX
cachedWorldCentroidY
cachedWorldCentroidZ
sortedId
idIndex
fieldDataPosition
fieldDataVelocity
fieldDataMass
fieldDataDeltaTime
emitterDataPosition
emitterDataVelocity
emitterDataDeltaTime
*/
    float   m_fLifespanPP;
    float   m_fLifespanPP0;
    float   m_fLifespan;

        // Points Particle Render Type
    bool    m_bColorAccum;
    bool    m_bUseLighting;
    bool    m_lPointSize;
    bool    m_lNormalDir;

        // Sprites Particle Render Type
    float   m_fSpriteTwist;
    float   m_fSpriteScaleX;
    float   m_fSpriteScaleY;
    float   m_fSpriteNum;

    enum
    {
        LIVE_FOREVER = 0,
        CONSTANT,
        RANDOM_RANGE,
        LIFESPAN_PP_ONLY
    };

    enum
    {
        MULTIPOINT = 0,
        MULTISTREAK,
        NUMERIC,
        POINTS,
        SPHERES,
        SPRITES,
        STREAK,
        BLOBBY_SURFACE,
        CLOUD,
        TUBE
    };
};



//---------------------------------------------------------------------------

class kMDtParticleSystemManager
{
public:

    kMDtParticleSystemManager();
    ~kMDtParticleSystemManager();

    void Reset();
    void ResetEmitters();
    void ResetParticles();

    int GetNumEmitters();

    int AddEmitter(kMDtEmitter* pNewEmitter);
    kMDtEmitter* GetEmitter( int iIndex );
    int GetEmitter( MObject mobj );     


    int GetNumParticles();

    int AddParticles(kMDtParticles* pNewParticles);
    kMDtParticles* GetParticles( int iIndex );
    int GetParticles( MObject mobj ); 

    void SetParticleMaterials();

protected:

    int             m_iNumEmitters;
    kMDtEmitter**   m_ppEmitters;

    int             m_iNumParticles;
    kMDtParticles** m_ppParticles;

};


//---------------------------------------------------------------------------

int addTransformEmitter(MObject transformNode,  MObject shapeNode);

int addTransformParticles(MObject transformNode, MObject shapeNode, 
    MDagPath shapePath);

//---------------------------------------------------------------------------

void gParticleSystemNew();  // INITIALIZE AT THE START OF THE EXPORTER

//---------------------------------------------------------------------------

// Declare a global Joint Manager.
extern kMDtParticleSystemManager gParticleSystemManager;

#endif