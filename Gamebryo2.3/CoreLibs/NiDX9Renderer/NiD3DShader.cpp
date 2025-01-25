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
#include "NiD3DRendererPCH.h"

#include "NiD3DShader.h"
#include "NiD3DPass.h"
#include "NiD3DShaderDeclaration.h"
#include "NiD3DRenderStateGroup.h"
#include "NiD3DRendererHeaders.h"
#include "NiD3DError.h"
#include "NiVBChip.h"
#include "NiD3DShaderProgramFactory.h"
#include "NiD3DShaderFactory.h"

NiFixedString NiD3DShader::ms_kNDLShaderMapName = NULL;

NiD3DShader::DynamicEffectPacker NiD3DShader::ms_apfnDynEffectPackers[
    NiTextureEffect::NUM_COORD_GEN];

NiImplementRTTI(NiD3DShader, NiD3DShaderInterface);
//---------------------------------------------------------------------------
void NiD3DShader::_SDMInit()
{
    ms_kNDLShaderMapName = "__NDL_SCMData";
    ms_apfnDynEffectPackers[NiTextureEffect::WORLD_PARALLEL] = 
        &PackWorldParallelEffect;
    ms_apfnDynEffectPackers[NiTextureEffect::WORLD_PERSPECTIVE] = 
        &PackWorldPerspectiveEffect;
    ms_apfnDynEffectPackers[NiTextureEffect::SPHERE_MAP] = 
        &PackWorldSphereEffect;
    ms_apfnDynEffectPackers[NiTextureEffect::SPECULAR_CUBE_MAP] = 
        &PackSpecularCubeEffect;
    ms_apfnDynEffectPackers[NiTextureEffect::DIFFUSE_CUBE_MAP] = 
        &PackDiffuseCubeEffect;
}
//---------------------------------------------------------------------------
void NiD3DShader::_SDMShutdown()
{
    ms_kNDLShaderMapName = NULL;
}
//---------------------------------------------------------------------------
NiD3DShader::~NiD3DShader()
{
    if (m_kName.Exists() && m_kName.GetLength() != 0)
    {
        NiD3DShaderFactory* pkShaderFactory = 
            NiD3DShaderFactory::GetD3DShaderFactory();

        // Attempt to remove the shader wherever it has been added
        if (this == 
            pkShaderFactory->FindShader(m_kName, GetImplementation()))
        {
            pkShaderFactory->RemoveShader(m_kName, GetImplementation());
        }
        if (GetIsBestImplementation() && (this ==
            pkShaderFactory->FindShader(m_kName, 
            NiShader::DEFAULT_IMPLEMENTATION)))
        {
            pkShaderFactory->RemoveShader(m_kName, 
                NiShader::DEFAULT_IMPLEMENTATION);
        }
    }
    if (m_pkD3DRenderer)
        m_pkD3DRenderer->ReleaseD3DShader(this);

    m_bInitialized = false;
    NiD3DRenderStateGroup::ReleaseRenderStateGroup(m_pkRenderStateGroup);
    m_uiCurrentPass = 0;
    m_uiPassCount = 0;
    for (unsigned int ui = 0; ui < m_kPasses.GetSize(); ui++)
    {
        m_kPasses.SetAt(ui, 0);
    }

    m_kPasses.RemoveAll();
}
//---------------------------------------------------------------------------
bool NiD3DShader::Initialize()
{
    // By default, we don't do anything in this call. (At the current moment)
    // A derived class would want to load any shader files at this point,
    // as well as any other 'miscellaneous' data it requires. This call is
    // provided to allow for shaders to be instantiated during streaming,
    // but not initialized. This prevents the shader from interfering with
    // load times by requiring a random seek during the loading of a nif 
    // file.

    return NiD3DShaderInterface::Initialize();
}
//---------------------------------------------------------------------------
unsigned int NiD3DShader::FirstPass()
{
    // Open first pass
    m_uiCurrentPass = 0;
    m_spCurrentPass = m_kPasses.GetAt(m_uiCurrentPass);

    return m_uiPassCount;
}
//---------------------------------------------------------------------------
unsigned int NiD3DShader::NextPass()
{
    // Close out the current pass
    if (m_spCurrentPass != NULL)
        m_spCurrentPass->PostProcessRenderingPass(m_uiCurrentPass);

    // Advance to the next pass
    m_uiCurrentPass++;
    if (m_uiCurrentPass == m_uiPassCount)
        return 0;

    m_spCurrentPass = m_kPasses.GetAt(m_uiCurrentPass);

    return (m_uiPassCount - m_uiCurrentPass);
}
//---------------------------------------------------------------------------
unsigned int NiD3DShader::PreProcessPipeline(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, NiGeometryData::RendererData* pkRendererData,
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects, 
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    // Safety catch - fail if the shader hasn't been initialized
    if (!m_bInitialized)
    {
        return 0xFFFFFFFF;
    }

    if (m_bUsesNiRenderState)
    {
        // UPdate the render state
        m_pkD3DRenderState->UpdateRenderState(pkState);
    }

// Xenon handles lights via shaders and does not have a light manager.
#if !defined(_XENON)
    if (m_bUsesNiLightState)
    {
        // Update the light state
        if (m_pkD3DRenderer->GetLightManager())
        {
            m_pkD3DRenderer->GetLightManager()->SetState(pkEffects, 
                pkState->GetTexturing(), pkState->GetVertexColor());
        }
    }
#endif

    // Set the 'global' rendering states
    if (m_pkRenderStateGroup != NULL)
    {
        NiD3DError kErr = m_pkRenderStateGroup->SetRenderStates();
    }

    return 0;
}
//---------------------------------------------------------------------------
unsigned int NiD3DShader::UpdatePipeline(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, NiGeometryData::RendererData* pkRendererData,
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects, 
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    // By default, nothing to do in UpdatePipline

    return 0;
}
//---------------------------------------------------------------------------
unsigned int NiD3DShader::SetupRenderingPass(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, NiGeometryData::RendererData* pkRendererData,
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects, 
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    NiGeometryBufferData* pkBuffData = (NiGeometryBufferData*)pkRendererData;

    unsigned int uiRet = 0;

    // Setup the current pass
    uiRet = m_spCurrentPass->SetupRenderingPass(pkGeometry, pkSkin, 
        pkBuffData, pkState, pkEffects, kWorld, kWorldBound);

    return uiRet;
}
//---------------------------------------------------------------------------
unsigned int NiD3DShader::SetupTransformations(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, 
    const NiSkinPartition::Partition* pkPartition, 
    NiGeometryData::RendererData* pkRendererData, 
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects, 
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    if (pkPartition)
    {
        if (m_uiCurrentPass == 0)
            m_pkD3DRenderer->CalculateBoneMatrices(pkSkin, kWorld);

        m_pkD3DRenderState->SetBoneCount(pkPartition->m_usBonesPerVertex);
        m_pkD3DRenderer->SetSkinnedModelTransforms(pkSkin, pkPartition, 
            kWorld);
    }
    else if (m_uiCurrentPass == 0 || m_pkD3DRenderer->GetBatchRendering())
    {
        m_pkD3DRenderState->SetBoneCount(0);
        m_pkD3DRenderer->SetModelTransform(kWorld);
    }

    return 0;
}
//---------------------------------------------------------------------------
unsigned int NiD3DShader::SetupShaderPrograms(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, 
    const NiSkinPartition::Partition* pkPartition, 
    NiGeometryData::RendererData* pkRendererData, 
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects, 
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    NiGeometryBufferData* pkBuffData = (NiGeometryBufferData*)pkRendererData;

    unsigned int uiRet = 0;

    // If there is an NiSCMExtraData object on this geometry, we want to 
    // reset the iterator to 0 so we can hit that cache when we set 
    // attribute constants.
    ResetSCMExtraData(pkGeometry);

    // Setup the shader programs and constants for the current pass.
    // If this is the first pass, then also set the 'global' constants
    // and render states.
    // Set the shader constants
    uiRet = m_spCurrentPass->SetupShaderPrograms(pkGeometry, pkSkin, 
        pkPartition, pkBuffData, pkState, pkEffects, kWorld, kWorldBound, 
        m_uiCurrentPass);

    // Now that the shaders have been set, if there are any 'global' 
    // mappings, set them now. This has to occur now since the pixel
    // shader has to be set prior to setting pixel shader constants.
    // This must be done every pass, since using a different shader on
    // a different pass may require constant remapping.
    NiD3DError eErr = NISHADERERR_OK;

    // Pixel shader mappings
    if (m_spPixelConstantMap && m_spCurrentPass->GetPixelShader())
    {
        eErr = m_spPixelConstantMap->SetShaderConstants(
            m_spCurrentPass->GetPixelShader(), pkGeometry, pkSkin, 
            pkPartition, pkBuffData, pkState, pkEffects, kWorld, kWorldBound, 
            m_uiCurrentPass, true);
    }

    // Vertex shader mappings
    if (m_spVertexConstantMap && m_spCurrentPass->GetVertexShader())
    {
        eErr = m_spVertexConstantMap->SetShaderConstants(
            m_spCurrentPass->GetVertexShader(), pkGeometry, pkSkin, 
            pkPartition, pkBuffData, pkState, pkEffects, kWorld, kWorldBound, 
            m_uiCurrentPass, true);
    }

    // On the FIRST pass only:
    if (m_uiCurrentPass == 0 && pkGeometry)
    {
        NiD3DShaderDeclaration* pkDecl = 
            (NiD3DShaderDeclaration*)(pkGeometry->GetShaderDecl());
        if (pkDecl)
        {
            m_pkD3DRenderState->SetDeclaration(
                pkDecl->GetD3DDeclaration());
        }
    }

    return uiRet;
}
//---------------------------------------------------------------------------
NiGeometryData::RendererData* NiD3DShader::PrepareGeometryForRendering(
    NiGeometry* pkGeometry, const NiSkinPartition::Partition* pkPartition,
    NiGeometryData::RendererData* pkRendererData, 
    const NiPropertyState* pkState)
{
    NiGeometryBufferData* pkBuffData = (NiGeometryBufferData*)pkRendererData;
    NIASSERT(pkBuffData);
    // On the first pass, the geometry should be packed. This call is a 
    // safety net to catch geometry that was not precached.
    // If the geometry has already been packed, this part of the function 
    // will quick-out.
    // 
    // Special case: volatile HW-skinned geometry must be repacked each pass
    //
    // Each pass, the stream sources and index buffers (if they apply) should
    // be set in this function.
    if (pkGeometry != NULL && (m_uiCurrentPass == 0 || 
        (pkGeometry->GetConsistency() == NiGeometryData::VOLATILE &&
        pkPartition != NULL)))
    {
        NiGeometryData* pkData = pkGeometry->GetModelData();
        NiSkinInstance* pkSkinInstance = pkGeometry->GetSkinInstance();

        NiD3DShaderDeclaration* pkDecl = 
            (NiD3DShaderDeclaration*)(pkGeometry->GetShaderDecl());
        if (!pkDecl)
        {
            NIVERIFY(NiShaderDeclaration::CreateDeclForGeometry(pkGeometry));
            pkDecl = (NiD3DShaderDeclaration*)(pkGeometry->GetShaderDecl());
        }

        if (pkPartition)
        {
            NiSkinPartition::Partition* pkSkinPartition = 
                (NiSkinPartition::Partition*)pkPartition;

            pkBuffData = (NiGeometryBufferData*)pkPartition->m_pkBuffData;

            m_pkD3DRenderer->PackSkinnedGeometryBuffer(pkBuffData,
                pkData, pkSkinInstance, pkSkinPartition,
                pkDecl);
        }
        else
        {
            pkBuffData = (NiGeometryBufferData*)pkData->GetRendererData();

            m_pkD3DRenderer->PackGeometryBuffer(pkBuffData, pkData, 
                pkSkinInstance, pkDecl);
        }
    }

    if (pkBuffData)
    {
        // Set the streams
        for (unsigned int i = 0; i < pkBuffData->GetStreamCount(); i++)
        {
            m_pkD3DDevice->SetStreamSource(i, 
                pkBuffData->GetVBChip(i)->GetVB(), 0, 
                pkBuffData->GetVertexStride(i));
        }
        m_pkD3DDevice->SetIndices(pkBuffData->GetIB());
        NIMETRICS_DX9RENDERER_AGGREGATEVALUE(VERTEX_BUFFER_CHANGES, 
            pkBuffData->GetStreamCount());
    }

    return pkBuffData;
}
//---------------------------------------------------------------------------
unsigned int NiD3DShader::PostProcessPipeline(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, NiGeometryData::RendererData* pkRendererData,
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects, 
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    // Restore the 'global' rendering states
    if (m_pkRenderStateGroup != NULL)
    {
        NiD3DError kErr = m_pkRenderStateGroup->RestoreRenderStates();
    }

    return 0;
}
//---------------------------------------------------------------------------
bool NiD3DShader::GetVertexInputSemantics(
    NiShaderDeclaration::ShaderRegisterEntry* pakSemantics)
{
    // Note that due to limitations on the GetShaderDecl function at the time
    // of authoring, this function could not be promoted into the base class.
    // If you modify this code, please insure that all platform renderers
    // are updated appropriately.
    NiShaderDeclaration* pkDecl = GetShaderDecl();
    if (!pkDecl)
    {
        pakSemantics[0].m_uiUsageIndex = NiMaterial::VS_INPUTS_USE_GEOMETRY;
        return false;
    }

    unsigned int uiStreamCount = pkDecl->GetStreamCount();
    unsigned int uiEntryCount = 0;
    unsigned int uiSemanticCount = 0;
    const NiShaderDeclaration::ShaderRegisterEntry* pkEntry = NULL;

    for (unsigned int ui = 0; ui < uiStreamCount; ++ui)
    {
        uiEntryCount = pkDecl->GetEntryCount(ui);
        for (unsigned int uj = 0; uj < uiEntryCount; ++uj)
        {
            // uj counts entries, and GetEntry takes the entry arg first.
            pkEntry = pkDecl->GetEntry(uj, ui);
            if (!pkEntry)
                continue;

            pakSemantics[uiSemanticCount].m_uiPreferredStream = ui;
            pakSemantics[uiSemanticCount].m_eInput = pkEntry->m_eInput;
            pakSemantics[uiSemanticCount].m_eType = pkEntry->m_eType;
            pakSemantics[uiSemanticCount].m_kUsage = pkEntry->m_kUsage;
            pakSemantics[uiSemanticCount].m_uiUsageIndex = 
                pkEntry->m_uiUsageIndex; 
            pakSemantics[uiSemanticCount].m_uiExtraData = 
                pkEntry->m_uiExtraData;
            ++uiSemanticCount;

            if (uiSemanticCount == NiMaterial::VS_INPUTS_MAX_NUM - 1)
                break;
        }
        if (uiSemanticCount == NiMaterial::VS_INPUTS_MAX_NUM - 1)
            break;
    }

    // Terminate the array.
    pakSemantics[uiSemanticCount].m_uiUsageIndex = 
        NiMaterial::VS_INPUTS_TERMINATE_ARRAY;
    
    return true;
}
//---------------------------------------------------------------------------
NiD3DShaderDeclaration* NiD3DShader::GetShaderDecl() const
{
    return m_spShaderDecl;
}
//---------------------------------------------------------------------------
void NiD3DShader::SetShaderDecl(NiD3DShaderDeclaration* pkShaderDecl)
{
    m_spShaderDecl = pkShaderDecl;
}
//---------------------------------------------------------------------------
void NiD3DShader::ResetPasses()
{
    for (unsigned int i = 0; i < m_uiPassCount; i++)
        m_kPasses.SetAt(i, 0);

    m_spCurrentPass = NULL;
    m_uiCurrentPass = 0;
    m_uiPassCount = 0;
}
//---------------------------------------------------------------------------
void NiD3DShader::InitializePools()
{
    NiD3DPass::InitializePools();
    NiD3DTextureStage::InitializePools();
}
//---------------------------------------------------------------------------
void NiD3DShader::ShutdownPools()
{
    // Needs to be called by Ni***RendererSDM::Shutdown so all passes and
    // stages are destroyed before the pools.
    NiD3DPass::ShutdownPools();
    NiD3DTextureStage::ShutdownPools();
}
//---------------------------------------------------------------------------
bool NiD3DShader::SetupGeometry(NiGeometry* pkGeometry)
{
    // Fill this in with the geometry getting assigned 'default' extra
    // data instances the shader expects to see. By default, NiD3DShader
    // simply sets up the NiSCMExtraData so that the engine does not have
    // to call strcmp too many times when rendering.
    SetupSCMExtraData(this, pkGeometry);

    return true;
}
//---------------------------------------------------------------------------
void NiD3DShader::DestroyRendererData()
{
    for (unsigned int i = 0; i < m_uiPassCount; i++)
    {
        NiD3DPass* pkPass = m_kPasses.GetAt(i);
        if (pkPass)
        {
            NiD3DPixelShader* pkPS = pkPass->GetPixelShader();
            if (pkPS)
                pkPS->DestroyRendererData();

            NiD3DVertexShader* pkVS = pkPass->GetVertexShader();
            if (pkVS)
                pkVS->DestroyRendererData();
        }
    }
    m_bInitialized = false;
}
//---------------------------------------------------------------------------
void NiD3DShader::RecreateRendererData()
{
    if (!m_pkD3DRenderer)
    {
        NIASSERT(!"NiD3DShader::RecreateRendererData> Invalid renderer!");
        return;
    }

    for (unsigned int i = 0; i < m_uiPassCount; i++)
    {
        NiD3DPass* pkPass = m_kPasses.GetAt(i);
        if (pkPass)
        {
            NiD3DPixelShader* pkPS = pkPass->GetPixelShader();
            if (pkPS)
                pkPS->RecreateRendererData();

            NiD3DVertexShader* pkVS = pkPass->GetVertexShader();
            if (pkVS)
                pkVS->RecreateRendererData();
        }
    }
    m_bInitialized = true;
}
//---------------------------------------------------------------------------
bool NiD3DShader::GetVSPresentAllPasses() const
{
    for (unsigned int i = 0; i < m_kPasses.GetSize(); i++)
    {
        NiD3DPass* pkPass = m_kPasses.GetAt(i);
        if (pkPass->GetVertexShader() == NULL)
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiD3DShader::GetVSPresentAnyPass() const
{
    for (unsigned int i = 0; i < m_kPasses.GetSize(); i++)
    {
        NiD3DPass* pkPass = m_kPasses.GetAt(i);
        if (pkPass->GetVertexShader())
            return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiD3DShader::GetPSPresentAllPasses() const
{
    for (unsigned int i = 0; i < m_kPasses.GetSize(); i++)
    {
        NiD3DPass* pkPass = m_kPasses.GetAt(i);
        if (pkPass->GetPixelShader() == NULL)
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiD3DShader::GetPSPresentAnyPass() const
{
    for (unsigned int i = 0; i < m_kPasses.GetSize(); i++)
    {
        NiD3DPass* pkPass = m_kPasses.GetAt(i);
        if (pkPass->GetPixelShader())
            return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void NiD3DShader::SetupSCMExtraData(NiD3DShader* pkD3DShader,
    NiGeometry* pkGeometry)
{
    // Remove any previous instances of the extra data cache.
    pkGeometry->RemoveExtraData(ms_kNDLShaderMapName);

    // Determine the number of entries and allocate a new instance so that
    // we get a tightly packed array of entries.
    unsigned int uiNumVertexEntries = 0;
    unsigned int uiNumPixelEntries = 0;
    unsigned int uiCount = 0;

    // Establish the number of entries in the global constant map times the
    // number of passes.

    // Vertex Shader Constants
    if (pkD3DShader->m_spVertexConstantMap != 0)
    {
        unsigned int uiCount = 
            pkD3DShader->m_spVertexConstantMap->GetEntryCount();
        for (unsigned int ui = 0; ui < uiCount; ui++)
        {
            NiShaderConstantMapEntry* pkEntry = 
                pkD3DShader->m_spVertexConstantMap->GetEntryAtIndex(ui);
            if (pkEntry && pkEntry->IsAttribute())
            {
                ++uiNumVertexEntries;
            }
        }
        // If the passes array is not tightly packed, this could cause some
        // waste in the arrays, but that is not likely.
        uiNumVertexEntries *= pkD3DShader->m_kPasses.GetSize();
    }

    // Pixel Shader Constants
    if (pkD3DShader->m_spPixelConstantMap != 0)
    {
        unsigned int uiCount = 
            pkD3DShader->m_spPixelConstantMap->GetEntryCount();
        for (unsigned int ui = 0; ui < uiCount; ui++)
        {
            NiShaderConstantMapEntry* pkEntry = 
                pkD3DShader->m_spPixelConstantMap->GetEntryAtIndex(ui);
            if (pkEntry && pkEntry->IsAttribute())
            {
                ++uiNumPixelEntries;
            }
        }
        // If the passes array is not tightly packed, this could cause some
        // waste in the arrays, but that is not likely.
        uiNumPixelEntries *= pkD3DShader->m_kPasses.GetSize();
    }
    
    unsigned int uiPasses;
    unsigned int uiNumPasses = pkD3DShader->m_kPasses.GetSize();
    for (uiPasses = 0; uiPasses < uiNumPasses; uiPasses++)
    {
        NiD3DPass* pkPass = pkD3DShader->m_kPasses.GetAt(uiPasses);
        if (pkPass)
        {
            // Check each pass for constant maps and then increment the entry
            // count if that entry is an attribute type entry.
            NiD3DShaderConstantMap* pkMap = pkPass->GetPixelConstantMap();
            if (pkMap)
            {
                
                unsigned int uiCount = pkMap->GetEntryCount();
                for (unsigned int ui = 0; ui < uiCount; ui++)
                {
                    NiShaderConstantMapEntry* pkEntry = 
                        pkMap->GetEntryAtIndex(ui);
                    if (pkEntry && pkEntry->IsAttribute())
                    {
                        ++uiNumPixelEntries;
                    }
                }
            }
            
            pkMap = pkPass->GetVertexConstantMap();
            if (pkMap)
            {
                unsigned int uiCount = pkMap->GetEntryCount();
                for (unsigned int ui = 0; ui < uiCount; ui++)
                {
                    NiShaderConstantMapEntry* pkEntry = 
                        pkMap->GetEntryAtIndex(ui);
                    if (pkEntry && pkEntry->IsAttribute())
                    {
                        ++uiNumVertexEntries;
                    }
                }
            }
        }
    }

    // Create the extra data cache table if necessary. We don't want to waste
    // time if there are no attribute type constant map entries.
    NiSCMExtraData* pkShaderExtraData = 0;
    if (uiNumVertexEntries > 0 || uiNumPixelEntries > 0)
    {
        pkShaderExtraData = NiNew NiSCMExtraData(ms_kNDLShaderMapName, 
            uiNumVertexEntries, 0, uiNumPixelEntries);
    }
    else
    {
        return;
    }

    // Populate the extra data with pointers. Again, we only insert entries
    // into our cache when the entry in the constant map is of the attribute
    // type.
    for (uiPasses = 0; uiPasses < uiNumPasses; uiPasses++)
    {
        NiD3DPass* pkPass = pkD3DShader->m_kPasses.GetAt(uiPasses);
        if (pkPass)
        {
            // Add global constant maps per pass so that we hit them
            // multiple times when iterating through the lists.
            uiCount = 0;
            if (pkD3DShader->m_spPixelConstantMap != 0)
                uiCount = pkD3DShader->m_spPixelConstantMap->GetEntryCount();
            unsigned int uiEntry;
            for (uiEntry = 0; uiEntry < uiCount; uiEntry++)
            {
                NiShaderConstantMapEntry* pkEntry = pkD3DShader->
                    m_spPixelConstantMap->GetEntryAtIndex(uiEntry);
                if (pkEntry && pkEntry->IsAttribute())
                {
                    NiExtraData* pkExtra = 
                        pkGeometry->GetExtraData(pkEntry->GetKey());
                    if (pkExtra)
                    {
                        pkShaderExtraData->AddEntry(
                            pkEntry->GetShaderRegister(), 0, 
                            NiGPUProgram::PROGRAM_PIXEL, pkExtra, true);
                    }
                }
            }

            uiCount = 0;
            if (pkD3DShader->m_spVertexConstantMap != 0)
                uiCount = pkD3DShader->m_spVertexConstantMap->GetEntryCount();
            for (uiEntry = 0; uiEntry < uiCount; uiEntry++)
            {
                NiShaderConstantMapEntry* pkEntry = pkD3DShader->
                    m_spVertexConstantMap->GetEntryAtIndex(uiEntry);
                if (pkEntry && pkEntry->IsAttribute())
                {
                    NiExtraData* pkExtra = 
                        pkGeometry->GetExtraData(pkEntry->GetKey());
                    if (pkExtra)
                    {
                        pkShaderExtraData->AddEntry(
                            pkEntry->GetShaderRegister(),0, 
                            NiGPUProgram::PROGRAM_VERTEX, pkExtra, true);
                    }
                }
            }

            // Add per pass pixel shader constants.
            NiD3DShaderConstantMap* pkPixelMap = pkPass->GetPixelConstantMap();
            if (pkPixelMap)
            {
                uiCount = pkPixelMap->GetEntryCount();
                for (unsigned int ui = 0; ui < uiCount; ui++)
                {
                    NiShaderConstantMapEntry* pkEntry = 
                        pkPixelMap->GetEntryAtIndex(ui);
                    if (pkEntry && pkEntry->IsAttribute())
                    {
                        NiExtraData* pkExtra = 
                            pkGeometry->GetExtraData(pkEntry->GetKey());
                        if (pkExtra)
                        {
                            pkShaderExtraData->AddEntry(
                                pkEntry->GetShaderRegister(),
                                0, NiGPUProgram::PROGRAM_PIXEL,
                                pkExtra, false);
                        }
                    }
                }
            }

            // Add per pass vertex shader constants.
            NiD3DShaderConstantMap* pkVertexMap = 
                pkPass->GetVertexConstantMap();
            if (pkVertexMap)
            {
                unsigned int uiCount = pkVertexMap->GetEntryCount();
                for (unsigned int ui = 0; ui < uiCount; ui++)
                {
                    NiShaderConstantMapEntry* pkEntry = 
                        pkVertexMap->GetEntryAtIndex(ui);
                    if (pkEntry && pkEntry->IsAttribute())
                    {
                        NiExtraData* pkExtra = 
                            pkGeometry->GetExtraData(pkEntry->GetKey());
                        if (pkExtra)
                        {
                            pkShaderExtraData->AddEntry(
                                pkEntry->GetShaderRegister(),
                                0, NiGPUProgram::PROGRAM_VERTEX,
                                pkExtra, false);
                        }
                    }
                }
            }
        }
    }

    // Attach the NiSCMExtraData object which holds our cached values.
    pkGeometry->AddExtraData(pkShaderExtraData);
}
//---------------------------------------------------------------------------
void NiD3DShader::PackWorldParallelEffect(const NiMatrix3& kWorldMat,
    const NiPoint3& kWorldTrans, NiD3DTextureStage* pkStage, bool bSave,
    NiD3DRenderer* pkD3DRenderer)
{
    pkStage->SetStageState(D3DTSS_TEXCOORDINDEX, 
        D3DTSS_TCI_CAMERASPACEPOSITION, bSave);
    pkStage->SetStageState(D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2,
        bSave);

    D3DMATRIX& kMat = pkStage->GetTextureTransformation();
    
    // kMat._14, kMat._24, kMat._34 and kMat._44 are always 0.0  

    // cam matrix = kWorldMat * m_invView
    // D3DMatrices are transposed with respect to NiMatrix3.
    const D3DMATRIX& kInvMat = pkD3DRenderer->GetInvView();

    // cam matrix = kWorldMat * m_invView
    kMat._11 = kWorldMat.GetEntry( 0,0 ) * kInvMat._11 +  
        kWorldMat.GetEntry( 0,1 ) * kInvMat._12 +  
        kWorldMat.GetEntry( 0,2 ) * kInvMat._13; 
    kMat._21 = kWorldMat.GetEntry( 0,0 ) * kInvMat._21 +  
        kWorldMat.GetEntry( 0,1 ) * kInvMat._22 +  
        kWorldMat.GetEntry( 0,2 ) * kInvMat._23; 
    kMat._31 = kWorldMat.GetEntry( 0,0 ) * kInvMat._31 +  
        kWorldMat.GetEntry( 0,1 ) * kInvMat._32 +  
        kWorldMat.GetEntry( 0,2 ) * kInvMat._33; 
    kMat._12 = kWorldMat.GetEntry( 1,0 ) * kInvMat._11 +  
        kWorldMat.GetEntry( 1,1 ) * kInvMat._12 +  
        kWorldMat.GetEntry( 1,2 ) * kInvMat._13; 
    kMat._22 = kWorldMat.GetEntry( 1,0 ) * kInvMat._21 +  
        kWorldMat.GetEntry( 1,1 ) * kInvMat._22 +  
        kWorldMat.GetEntry( 1,2 ) * kInvMat._23; 
    kMat._32 = kWorldMat.GetEntry( 1,0 ) * kInvMat._31 +  
        kWorldMat.GetEntry( 1,1 ) * kInvMat._32 +  
        kWorldMat.GetEntry( 1,2 ) * kInvMat._33; 
    kMat._13 = kWorldMat.GetEntry( 2,0 ) * kInvMat._11 +  
        kWorldMat.GetEntry( 2,1 ) * kInvMat._12 +  
        kWorldMat.GetEntry( 2,2 ) * kInvMat._13; 
    kMat._23 = kWorldMat.GetEntry( 2,0 ) * kInvMat._21 +  
        kWorldMat.GetEntry( 2,1 ) * kInvMat._22 +  
        kWorldMat.GetEntry( 2,2 ) * kInvMat._23; 
    kMat._33 = kWorldMat.GetEntry( 2,0 ) * kInvMat._31 +  
        kWorldMat.GetEntry( 2,1 ) * kInvMat._32 +  
        kWorldMat.GetEntry( 2,2 ) * kInvMat._33; 

    // cam trans = kWorldMat * invViewTrans + kWorldTrans
    kMat._41 = kWorldMat.GetEntry( 0,0 ) * kInvMat._41 +  
        kWorldMat.GetEntry( 0,1 ) * kInvMat._42 +  
        kWorldMat.GetEntry( 0,2 ) * kInvMat._43 + kWorldTrans.x; 
    kMat._42 = kWorldMat.GetEntry( 1,0 ) * kInvMat._41 +  
        kWorldMat.GetEntry( 1,1 ) * kInvMat._42 +  
        kWorldMat.GetEntry( 1,2 ) * kInvMat._43 + kWorldTrans.y; 
    kMat._43 = kWorldMat.GetEntry( 2,0 ) * kInvMat._41 +  
        kWorldMat.GetEntry( 2,1 ) * kInvMat._42 +  
        kWorldMat.GetEntry( 2,2 ) * kInvMat._43 + kWorldTrans.z; 

    kMat._14 = kMat._24 = kMat._34 = kMat._44 = 0.0f;
}
//---------------------------------------------------------------------------
void NiD3DShader::PackWorldPerspectiveEffect(const NiMatrix3& kWorldMat,
    const NiPoint3& kWorldTrans, NiD3DTextureStage* pkStage, bool bSave,
    NiD3DRenderer* pkD3DRenderer)
{
    pkStage->SetStageState(D3DTSS_TEXCOORDINDEX, 
        D3DTSS_TCI_CAMERASPACEPOSITION, bSave);
    pkStage->SetStageState(D3DTSS_TEXTURETRANSFORMFLAGS, 
        pkD3DRenderer->GetProjectedTextureFlags(), bSave);

    D3DMATRIX& kMat = pkStage->GetTextureTransformation();

    // kMat._14, kMat._24, kMat._34 and kMat._44 are always 0.0  

    // cam matrix = kWorldMat * kInvMat
    // D3DMatrices are transposed with respect to NiMatrix3.
    const D3DMATRIX& kInvMat = pkD3DRenderer->GetInvView();

    kMat._11 = kWorldMat.GetEntry( 0,0 ) * kInvMat._11 +  
        kWorldMat.GetEntry( 0,1 ) * kInvMat._12 +  
        kWorldMat.GetEntry( 0,2 ) * kInvMat._13; 
    kMat._21 = kWorldMat.GetEntry( 0,0 ) * kInvMat._21 +  
        kWorldMat.GetEntry( 0,1 ) * kInvMat._22 +  
        kWorldMat.GetEntry( 0,2 ) * kInvMat._23; 
    kMat._31 = kWorldMat.GetEntry( 0,0 ) * kInvMat._31 +  
        kWorldMat.GetEntry( 0,1 ) * kInvMat._32 +  
        kWorldMat.GetEntry( 0,2 ) * kInvMat._33; 
    kMat._12 = kWorldMat.GetEntry( 1,0 ) * kInvMat._11 +  
        kWorldMat.GetEntry( 1,1 ) * kInvMat._12 +  
        kWorldMat.GetEntry( 1,2 ) * kInvMat._13; 
    kMat._22 = kWorldMat.GetEntry( 1,0 ) * kInvMat._21 +  
        kWorldMat.GetEntry( 1,1 ) * kInvMat._22 +  
        kWorldMat.GetEntry( 1,2 ) * kInvMat._23; 
    kMat._32 = kWorldMat.GetEntry( 1,0 ) * kInvMat._31 +  
        kWorldMat.GetEntry( 1,1 ) * kInvMat._32 +  
        kWorldMat.GetEntry( 1,2 ) * kInvMat._33; 
    kMat._13 = kWorldMat.GetEntry( 2,0 ) * kInvMat._11 +  
        kWorldMat.GetEntry( 2,1 ) * kInvMat._12 +  
        kWorldMat.GetEntry( 2,2 ) * kInvMat._13; 
    kMat._23 = kWorldMat.GetEntry( 2,0 ) * kInvMat._21 +  
        kWorldMat.GetEntry( 2,1 ) * kInvMat._22 +  
        kWorldMat.GetEntry( 2,2 ) * kInvMat._23; 
    kMat._33 = kWorldMat.GetEntry( 2,0 ) * kInvMat._31 +  
        kWorldMat.GetEntry( 2,1 ) * kInvMat._32 +  
        kWorldMat.GetEntry( 2,2 ) * kInvMat._33; 

    // cam trans = kWorldMat * pkInvViewTrans + kWorldTrans

    kMat._41 = kWorldMat.GetEntry( 0,0 ) * kInvMat._41 +  
        kWorldMat.GetEntry( 0,1 ) * kInvMat._42 +  
        kWorldMat.GetEntry( 0,2 ) * kInvMat._43 + kWorldTrans.x; 
    kMat._42 = kWorldMat.GetEntry( 1,0 ) * kInvMat._41 +  
        kWorldMat.GetEntry( 1,1 ) * kInvMat._42 +  
        kWorldMat.GetEntry( 1,2 ) * kInvMat._43 + kWorldTrans.y; 
    kMat._43 = kWorldMat.GetEntry( 2,0 ) * kInvMat._41 +  
        kWorldMat.GetEntry( 2,1 ) * kInvMat._42 +  
        kWorldMat.GetEntry( 2,2 ) * kInvMat._43 + kWorldTrans.z; 

    kMat._14 = kMat._24 = kMat._34 = kMat._44 = 0.0f;
}
//---------------------------------------------------------------------------
void NiD3DShader::PackWorldSphereEffect(const NiMatrix3& kWorldMat,
    const NiPoint3& kWorldTrans, NiD3DTextureStage* pkStage, bool bSave,
    NiD3DRenderer* pkD3DRenderer)
{
    pkStage->SetStageState(D3DTSS_TEXCOORDINDEX, 
        D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR, bSave);
    pkStage->SetStageState(D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2,
        bSave);

    D3DMATRIX& kMat = pkStage->GetTextureTransformation();

    // kMat._14, kMat._24, kMat._34 and kMat._44 are always 0.0  

    // cam matrix = kWorldMat * kInvMat
    // D3DMatrices are transposed with respect to NiMatrix3.
    const D3DMATRIX& kInvMat = pkD3DRenderer->GetInvView();

    kMat._11 = kWorldMat.GetEntry( 0,0 ) * kInvMat._11 +  
        kWorldMat.GetEntry( 0,1 ) * kInvMat._12 +  
        kWorldMat.GetEntry( 0,2 ) * kInvMat._13; 
    kMat._21 = kWorldMat.GetEntry( 0,0 ) * kInvMat._21 +  
        kWorldMat.GetEntry( 0,1 ) * kInvMat._22 +  
        kWorldMat.GetEntry( 0,2 ) * kInvMat._23; 
    kMat._31 = kWorldMat.GetEntry( 0,0 ) * kInvMat._31 +  
        kWorldMat.GetEntry( 0,1 ) * kInvMat._32 +  
        kWorldMat.GetEntry( 0,2 ) * kInvMat._33; 
    kMat._12 = kWorldMat.GetEntry( 1,0 ) * kInvMat._11 +  
        kWorldMat.GetEntry( 1,1 ) * kInvMat._12 +  
        kWorldMat.GetEntry( 1,2 ) * kInvMat._13; 
    kMat._22 = kWorldMat.GetEntry( 1,0 ) * kInvMat._21 +  
        kWorldMat.GetEntry( 1,1 ) * kInvMat._22 +  
        kWorldMat.GetEntry( 1,2 ) * kInvMat._23; 
    kMat._32 = kWorldMat.GetEntry( 1,0 ) * kInvMat._31 +  
        kWorldMat.GetEntry( 1,1 ) * kInvMat._32 +  
        kWorldMat.GetEntry( 1,2 ) * kInvMat._33; 
    kMat._13 = kWorldMat.GetEntry( 2,0 ) * kInvMat._11 +  
        kWorldMat.GetEntry( 2,1 ) * kInvMat._12 +  
        kWorldMat.GetEntry( 2,2 ) * kInvMat._13; 
    kMat._23 = kWorldMat.GetEntry( 2,0 ) * kInvMat._21 +  
        kWorldMat.GetEntry( 2,1 ) * kInvMat._22 +  
        kWorldMat.GetEntry( 2,2 ) * kInvMat._23; 
    kMat._33 = kWorldMat.GetEntry( 2,0 ) * kInvMat._31 +  
        kWorldMat.GetEntry( 2,1 ) * kInvMat._32 +  
        kWorldMat.GetEntry( 2,2 ) * kInvMat._33; 

    // cam trans is used directly

    kMat._41 = kWorldTrans.x; 
    kMat._42 = kWorldTrans.y; 
    kMat._43 = kWorldTrans.z; 

    kMat._14 = kMat._24 = kMat._34 = kMat._44 = 0.0f;
}
//---------------------------------------------------------------------------
void NiD3DShader::PackCameraSphereEffect(const NiMatrix3& kWorldMat,
    const NiPoint3& kWorldTrans, NiD3DTextureStage* pkStage, bool bSave,
    NiD3DRenderer* pkD3DRenderer)
{
    pkStage->SetStageState(D3DTSS_TEXCOORDINDEX, 
        D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR, bSave);
    pkStage->SetStageState(D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2,
        bSave);

    D3DMATRIX& kMat = pkStage->GetTextureTransformation();

    // This is always the same matrix in camera space
    kMat._11 = kMat._12 = kMat._13 = kMat._21 = kMat._23 
        = kMat._32 = kMat._33 = 0.0f;
    kMat._22 = 0.5f;
    kMat._31 = 0.5f;
    
    kMat._41 = 0.5f;
    kMat._42 = 0.5f;
    kMat._43 = 0.0f;

    kMat._14 = kMat._24 = kMat._34 = kMat._44 = 0.0f;
}
//---------------------------------------------------------------------------
void NiD3DShader::PackSpecularCubeEffect(const NiMatrix3& kWorldMat,
    const NiPoint3& kWorldTrans, NiD3DTextureStage* pkStage, bool bSave,
    NiD3DRenderer* pkD3DRenderer)
{
    pkStage->SetStageState(D3DTSS_TEXCOORDINDEX, 
        D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR, bSave);
    pkStage->SetStageState(D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3,
        bSave);

    D3DMATRIX& kMat = pkStage->GetTextureTransformation();

    // kMat._14, kMat._24, kMat._34 and kMat._44 are always 0.0  

    // cam matrix = kWorldMat * kInvMat
    // D3DMatrices are transposed with respect to NiMatrix3.
    const D3DMATRIX& kInvMat = pkD3DRenderer->GetInvView();

    kMat._11 = kWorldMat.GetEntry( 0,0 ) * kInvMat._11 +  
        kWorldMat.GetEntry( 0,1 ) * kInvMat._12 +  
        kWorldMat.GetEntry( 0,2 ) * kInvMat._13; 
    kMat._21 = kWorldMat.GetEntry( 0,0 ) * kInvMat._21 +  
        kWorldMat.GetEntry( 0,1 ) * kInvMat._22 +  
        kWorldMat.GetEntry( 0,2 ) * kInvMat._23; 
    kMat._31 = kWorldMat.GetEntry( 0,0 ) * kInvMat._31 +  
        kWorldMat.GetEntry( 0,1 ) * kInvMat._32 +  
        kWorldMat.GetEntry( 0,2 ) * kInvMat._33; 
    kMat._12 = kWorldMat.GetEntry( 1,0 ) * kInvMat._11 +  
        kWorldMat.GetEntry( 1,1 ) * kInvMat._12 +  
        kWorldMat.GetEntry( 1,2 ) * kInvMat._13; 
    kMat._22 = kWorldMat.GetEntry( 1,0 ) * kInvMat._21 +  
        kWorldMat.GetEntry( 1,1 ) * kInvMat._22 +  
        kWorldMat.GetEntry( 1,2 ) * kInvMat._23; 
    kMat._32 = kWorldMat.GetEntry( 1,0 ) * kInvMat._31 +  
        kWorldMat.GetEntry( 1,1 ) * kInvMat._32 +  
        kWorldMat.GetEntry( 1,2 ) * kInvMat._33; 
    kMat._13 = kWorldMat.GetEntry( 2,0 ) * kInvMat._11 +  
        kWorldMat.GetEntry( 2,1 ) * kInvMat._12 +  
        kWorldMat.GetEntry( 2,2 ) * kInvMat._13; 
    kMat._23 = kWorldMat.GetEntry( 2,0 ) * kInvMat._21 +  
        kWorldMat.GetEntry( 2,1 ) * kInvMat._22 +  
        kWorldMat.GetEntry( 2,2 ) * kInvMat._23; 
    kMat._33 = kWorldMat.GetEntry( 2,0 ) * kInvMat._31 +  
        kWorldMat.GetEntry( 2,1 ) * kInvMat._32 +  
        kWorldMat.GetEntry( 2,2 ) * kInvMat._33; 

    // cam trans is used directly

    kMat._41 = kWorldTrans.x; 
    kMat._42 = kWorldTrans.y; 
    kMat._43 = kWorldTrans.z; 

    kMat._14 = kMat._24 = kMat._34 = kMat._44 = 0.0f;
}
//---------------------------------------------------------------------------
void NiD3DShader::PackDiffuseCubeEffect(const NiMatrix3& kWorldMat,
    const NiPoint3& kWorldTrans, NiD3DTextureStage* pkStage, bool bSave,
    NiD3DRenderer* pkD3DRenderer)
{
    pkStage->SetStageState(D3DTSS_TEXCOORDINDEX, 
        D3DTSS_TCI_CAMERASPACENORMAL, bSave);
    pkStage->SetStageState(D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3,
        bSave);

    D3DMATRIX& kMat = pkStage->GetTextureTransformation();

    // kMat._14, kMat._24, kMat._34 and kMat._44 are always 0.0  

    // cam matrix = kWorldMat * kInvMat
    // D3DMatrices are transposed with respect to NiMatrix3.
    const D3DMATRIX& kInvMat = pkD3DRenderer->GetInvView();

    kMat._11 = kWorldMat.GetEntry( 0,0 ) * kInvMat._11 +  
        kWorldMat.GetEntry( 0,1 ) * kInvMat._12 +  
        kWorldMat.GetEntry( 0,2 ) * kInvMat._13; 
    kMat._21 = kWorldMat.GetEntry( 0,0 ) * kInvMat._21 +  
        kWorldMat.GetEntry( 0,1 ) * kInvMat._22 +  
        kWorldMat.GetEntry( 0,2 ) * kInvMat._23; 
    kMat._31 = kWorldMat.GetEntry( 0,0 ) * kInvMat._31 +  
        kWorldMat.GetEntry( 0,1 ) * kInvMat._32 +  
        kWorldMat.GetEntry( 0,2 ) * kInvMat._33; 
    kMat._12 = kWorldMat.GetEntry( 1,0 ) * kInvMat._11 +  
        kWorldMat.GetEntry( 1,1 ) * kInvMat._12 +  
        kWorldMat.GetEntry( 1,2 ) * kInvMat._13; 
    kMat._22 = kWorldMat.GetEntry( 1,0 ) * kInvMat._21 +  
        kWorldMat.GetEntry( 1,1 ) * kInvMat._22 +  
        kWorldMat.GetEntry( 1,2 ) * kInvMat._23; 
    kMat._32 = kWorldMat.GetEntry( 1,0 ) * kInvMat._31 +  
        kWorldMat.GetEntry( 1,1 ) * kInvMat._32 +  
        kWorldMat.GetEntry( 1,2 ) * kInvMat._33; 
    kMat._13 = kWorldMat.GetEntry( 2,0 ) * kInvMat._11 +  
        kWorldMat.GetEntry( 2,1 ) * kInvMat._12 +  
        kWorldMat.GetEntry( 2,2 ) * kInvMat._13; 
    kMat._23 = kWorldMat.GetEntry( 2,0 ) * kInvMat._21 +  
        kWorldMat.GetEntry( 2,1 ) * kInvMat._22 +  
        kWorldMat.GetEntry( 2,2 ) * kInvMat._23; 
    kMat._33 = kWorldMat.GetEntry( 2,0 ) * kInvMat._31 +  
        kWorldMat.GetEntry( 2,1 ) * kInvMat._32 +  
        kWorldMat.GetEntry( 2,2 ) * kInvMat._33; 

    // cam trans is used directly

    kMat._41 = kWorldTrans.x; 
    kMat._42 = kWorldTrans.y; 
    kMat._43 = kWorldTrans.z; 

    kMat._14 = kMat._24 = kMat._34 = kMat._44 = 0.0f;
}
//---------------------------------------------------------------------------
