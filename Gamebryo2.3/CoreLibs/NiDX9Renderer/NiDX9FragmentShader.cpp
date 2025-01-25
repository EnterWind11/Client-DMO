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

#include "NiDX9FragmentShader.h"
#include "NiD3DShaderProgramFactory.h"
#include <NiLight.h>

NiImplementRTTI(NiDX9FragmentShader,NiD3DShader);
//---------------------------------------------------------------------------
NiDX9FragmentShader::NiDX9FragmentShader(NiMaterialDescriptor* pkDesc) 
{
    NIASSERT(pkDesc != NULL);
    m_kDescriptor.m_kMatDesc = *pkDesc;
}
//---------------------------------------------------------------------------
NiDX9FragmentShader::~NiDX9FragmentShader()
{
}
//---------------------------------------------------------------------------
bool NiDX9FragmentShader::IsGenericallyConfigurable()
{
    return true;
}
//---------------------------------------------------------------------------
NiShaderDeclaration* NiDX9FragmentShader::CreateShaderDeclaration(
    unsigned int uiMaxStreamEntryCount, unsigned int uiStreamCount)
{
    NiD3DShaderDeclaration* pkDecl = NiDX9ShaderDeclaration::Create(
        NiRenderer::GetRenderer(), uiMaxStreamEntryCount, uiStreamCount);

    SetShaderDecl(pkDecl);
    return pkDecl;
}
//---------------------------------------------------------------------------
bool NiDX9FragmentShader::AppendRenderPass(unsigned int& uiPassId)
{
    NiD3DPassPtr spPass = NiD3DPass::CreateNewPass();
    NIASSERT(spPass);
    m_kPasses.SetAtGrow(m_uiPassCount, spPass);
    uiPassId = m_uiPassCount;
    m_uiPassCount++;
    return true;
}
//---------------------------------------------------------------------------
bool NiDX9FragmentShader::SetAlphaOverride(unsigned int uiPassId, 
    bool bAlphaBlend, bool bUsePreviousSrcBlendMode, 
    NiAlphaProperty::AlphaFunction eSrcBlendMode, 
    bool bUsePreviousDestBlendMode, 
    NiAlphaProperty::AlphaFunction eDestBlendMode)
{
    if (uiPassId >= m_uiPassCount)
        return false;

    NiD3DPass* pkPass = m_kPasses.GetAt(uiPassId);
    if (!pkPass)
        return false;

    if (bAlphaBlend)
    {
        pkPass->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
        NiD3DRenderState* pkRS = pkPass->GetD3DRenderState();
        if (!bUsePreviousSrcBlendMode)
        {
            pkPass->SetRenderState(D3DRS_SRCBLEND, 
                pkRS->GetD3DBlendMode(eSrcBlendMode));
        }
        else
        {
            pkPass->RemoveRenderState(D3DRS_SRCBLEND);
        }

        if (!bUsePreviousDestBlendMode)
        {
            pkPass->SetRenderState(D3DRS_DESTBLEND,
                pkRS->GetD3DBlendMode(eDestBlendMode));
        }
        else
        {
            pkPass->RemoveRenderState(D3DRS_DESTBLEND);
        }
    }
    else
    {
        pkPass->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiDX9FragmentShader::SetGPUProgram(unsigned int uiPassId, 
    NiGPUProgram* pkProgram, NiGPUProgram::ProgramType& eProgramType)
{
    if (uiPassId >= m_uiPassCount)
        return false;

    NiD3DPass* pkPass = m_kPasses.GetAt(uiPassId);
    if (!pkPass)
        return false;

    switch (pkProgram->GetProgramType())
    {
        case NiGPUProgram::PROGRAM_VERTEX:
            pkPass->SetVertexShader((NiD3DVertexShader*) pkProgram);
            eProgramType = NiGPUProgram::PROGRAM_VERTEX;
            break;
        case NiGPUProgram::PROGRAM_PIXEL:
            pkPass->SetPixelShader((NiD3DPixelShader*) pkProgram);
            eProgramType = NiGPUProgram::PROGRAM_PIXEL;
            break;
    }
    return true;
}
//---------------------------------------------------------------------------
NiShaderConstantMap* NiDX9FragmentShader::CreateShaderConstantMap(
    unsigned int uiPassId, NiGPUProgram::ProgramType eProgramType,
    unsigned int uiMapIndex)
{
    if (uiPassId >= m_uiPassCount)
        return false;

    NiD3DPass* pkPass = m_kPasses.GetAt(uiPassId);
    if (!pkPass)
        return false;

    NiD3DShaderConstantMap* pkSCM = NULL;
    if (eProgramType == NiGPUProgram::PROGRAM_VERTEX) // Vertex program
    {
        pkSCM = pkPass->GetVertexConstantMap();

        if (!pkSCM)
        {
            pkSCM = NiNew NiD3DShaderConstantMap(NiD3DRenderer::GetRenderer(),
                NiGPUProgram::PROGRAM_VERTEX);
            pkPass->SetVertexConstantMap(pkSCM);
        }
    }
    else if (eProgramType == NiGPUProgram::PROGRAM_PIXEL) // Pixel program
    {
        pkSCM = pkPass->GetPixelConstantMap();
        if (!pkSCM)
        {
            pkSCM = NiNew NiD3DShaderConstantMap(NiD3DRenderer::GetRenderer(),
                NiGPUProgram::PROGRAM_PIXEL);
            pkPass->SetPixelConstantMap(pkSCM);
        }
    }

    return pkSCM;
}
//---------------------------------------------------------------------------
bool NiDX9FragmentShader::AppendTextureSampler(unsigned int uiPassId, 
    unsigned int& uiSamplerId, const NiFixedString& kSemantic,
    const NiFixedString& kVariableName, unsigned int uiInstance)
{
    if (uiPassId >= m_uiPassCount)
        return false;

    NiD3DPass* pkPass = m_kPasses.GetAt(uiPassId);
    if (!pkPass)
        return false;

    NiD3DTextureStagePtr spStage = NiD3DTextureStage::CreateNewStage();
    if (NISHADERERR_OK != pkPass->AppendStage(spStage))
        return false;

    uiSamplerId = pkPass->GetStageCount() - 1;

    unsigned int uiTextureID = 0;
    if (NiTexturingProperty::GetMapIDFromName(kSemantic, uiTextureID))
    {
        switch(uiTextureID)
        {
            case NiTexturingProperty::BASE_INDEX:
                spStage->SetNDLMap(NiD3DTextureStage::TSTF_NDL_BASE);
                break;
            case NiTexturingProperty::DARK_INDEX:
                spStage->SetNDLMap(NiD3DTextureStage::TSTF_NDL_DARK);
                break;
            case NiTexturingProperty::DETAIL_INDEX:
                spStage->SetNDLMap(NiD3DTextureStage::TSTF_NDL_DETAIL);
                break;
            case NiTexturingProperty::GLOSS_INDEX:
                spStage->SetNDLMap(NiD3DTextureStage::TSTF_NDL_GLOSS);
                break;
            case NiTexturingProperty::GLOW_INDEX:
                spStage->SetNDLMap(NiD3DTextureStage::TSTF_NDL_GLOW);
                break;
            case NiTexturingProperty::BUMP_INDEX:
                spStage->SetNDLMap(NiD3DTextureStage::TSTF_NDL_BUMP);
                break;
            case NiTexturingProperty::NORMAL_INDEX:
                spStage->SetNDLMap(NiD3DTextureStage::TSTF_NDL_NORMAL);
                break;
            case NiTexturingProperty::PARALLAX_INDEX:
                spStage->SetNDLMap(NiD3DTextureStage::TSTF_NDL_PARALLAX);
                break;
            case NiTexturingProperty::DECAL_BASE:
                spStage->SetDecalMap(uiInstance);
                break;
            case NiTexturingProperty::SHADER_BASE:
                spStage->SetShaderMap(uiInstance);
                break;
            default:
                NIASSERT(!"Unknown map entry!");
                return false;
                break;
        }
    }
    else if (NiTextureEffect::GetTypeIDFromName(kSemantic, uiTextureID))
    {
        unsigned short usObjTextureFlags = 0;
        switch((NiTextureEffect::TextureType) uiTextureID)
        {
            case NiTextureEffect::PROJECTED_LIGHT:
                usObjTextureFlags = 
                    (usObjTextureFlags & ~NiD3DTextureStage::TSOTF_TYPE_MASK) |
                    (NiShaderAttributeDesc::OT_EFFECT_PROJECTEDLIGHTMAP << 
                    NiD3DTextureStage::TSOTF_TYPE_SHIFT);
                break;
            case NiTextureEffect::PROJECTED_SHADOW:
                usObjTextureFlags = 
                    (usObjTextureFlags & ~NiD3DTextureStage::TSOTF_TYPE_MASK) |
                    (NiShaderAttributeDesc::OT_EFFECT_PROJECTEDSHADOWMAP << 
                    NiD3DTextureStage::TSOTF_TYPE_SHIFT);
                break;
            case NiTextureEffect::ENVIRONMENT_MAP:
                usObjTextureFlags = 
                    (usObjTextureFlags & ~NiD3DTextureStage::TSOTF_TYPE_MASK) |
                    (NiShaderAttributeDesc::OT_EFFECT_ENVIRONMENTMAP << 
                    NiD3DTextureStage::TSOTF_TYPE_SHIFT);
                break;
            case NiTextureEffect::FOG_MAP:
                usObjTextureFlags = 
                    (usObjTextureFlags & ~NiD3DTextureStage::TSOTF_TYPE_MASK) |
                    (NiShaderAttributeDesc::OT_EFFECT_FOGMAP << 
                    NiD3DTextureStage::TSOTF_TYPE_SHIFT);
                break;
            default:
                NIASSERT(!"Unknown texture effect type");
                break;
        }

        usObjTextureFlags = 
            (usObjTextureFlags & ~NiD3DTextureStage::TSOTF_INDEX_MASK) | 
            uiInstance;
        
        spStage->SetObjTextureFlags(usObjTextureFlags);
    }
    else if (NiShadowMap::GetLightTypeFromName(kSemantic, uiTextureID))
    {
        unsigned short usObjTextureFlags = 0;

        switch ((NiStandardMaterial::LightType)uiTextureID)
        {
        case NiStandardMaterial::LIGHT_DIR:
            usObjTextureFlags = 
                (usObjTextureFlags & ~NiD3DTextureStage::TSOTF_TYPE_MASK) |
                (NiShaderAttributeDesc::OT_EFFECT_DIRSHADOWMAP << 
                NiD3DTextureStage::TSOTF_TYPE_SHIFT);
            break;
        case NiStandardMaterial::LIGHT_SPOT:
            usObjTextureFlags = 
                (usObjTextureFlags & ~NiD3DTextureStage::TSOTF_TYPE_MASK) |
                (NiShaderAttributeDesc::OT_EFFECT_SPOTSHADOWMAP << 
                NiD3DTextureStage::TSOTF_TYPE_SHIFT);
            break;
        case NiStandardMaterial::LIGHT_POINT:
            usObjTextureFlags = 
                (usObjTextureFlags & ~NiD3DTextureStage::TSOTF_TYPE_MASK) |
                (NiShaderAttributeDesc::OT_EFFECT_POINTSHADOWMAP << 
                NiD3DTextureStage::TSOTF_TYPE_SHIFT);
            break;
        default:
            NIASSERT(!"Unknown light type");
            break;
        };

        usObjTextureFlags = 
            (usObjTextureFlags & ~NiD3DTextureStage::TSOTF_INDEX_MASK) | 
            uiInstance;

        spStage->SetObjTextureFlags(usObjTextureFlags);
    }

    

    return true;
}
//---------------------------------------------------------------------------
bool NiDX9FragmentShader::Initialize(NiD3DRenderer* pkRenderer)
{
    if (m_bInitialized)
        return true;

    if (!NiD3DShader::Initialize())
        return false;
   
    return true;
}
//---------------------------------------------------------------------------
unsigned int NiDX9FragmentShader::PreProcessPipeline(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, 
    NiGeometryData::RendererData* pkRendererData, 
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects,
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    return NiD3DShader::PreProcessPipeline(pkGeometry, pkSkin,
        pkRendererData, pkState, pkEffects, kWorld, kWorldBound);
}
//---------------------------------------------------------------------------
unsigned int NiDX9FragmentShader::UpdatePipeline(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, 
    NiGeometryData::RendererData* pkRendererData, 
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects,
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    for (unsigned int uiPass = 0; uiPass < m_kPasses.GetSize(); uiPass++)
    {
        NiD3DPass* pkCurPass = m_kPasses.GetAt(uiPass);
        if (!pkCurPass)
        {
            continue;
        }
        for (unsigned int uiStage = 0; uiStage < pkCurPass->GetStageCount();
            uiStage++)
        {
            NiD3DTextureStage* pkCurStage = pkCurPass->GetStage(uiStage);
            if (!pkCurStage)
            {
                continue;
            }
            const NiTexturingProperty* pkTexProp = pkState->GetTexturing();
            PrepareTextureStage(pkGeometry, pkCurStage, pkTexProp, 
                pkEffects, uiPass);
        }
    }

    return NiD3DShader::UpdatePipeline(pkGeometry, pkSkin,
        pkRendererData, pkState, pkEffects, kWorld, kWorldBound);
}
//---------------------------------------------------------------------------
unsigned int NiDX9FragmentShader::SetupRenderingPass(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, 
    NiGeometryData::RendererData* pkRendererData, 
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects,
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    
    return NiD3DShader::SetupRenderingPass(pkGeometry, pkSkin,
        pkRendererData, pkState, pkEffects, kWorld, kWorldBound);
}
//---------------------------------------------------------------------------
unsigned int NiDX9FragmentShader::SetupTransformations(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, 
    const NiSkinPartition::Partition* pkPartition, 
    NiGeometryData::RendererData* pkRendererData, 
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects, 
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    // We need to set the bone matrices ourselves.
    if (pkPartition)
    {
        if (m_uiCurrentPass == 0)
        {
            m_pkD3DRenderer->CalculateBoneMatrices(pkSkin, kWorld, true, 3, 
                true);
        }

        m_pkD3DRenderState->SetBoneCount(
            pkPartition->m_usBonesPerVertex);

        // Force the calculation of the 'world' D3D matrix
        // but indicate it shouldn't be pushed to the device
        m_pkD3DRenderer->SetModelTransform(kWorld, false);
        return 0;
    }

    return NiD3DShader::SetupTransformations(pkGeometry, pkSkin, pkPartition,
        pkRendererData, pkState, pkEffects, kWorld, kWorldBound);
}
//---------------------------------------------------------------------------
unsigned int NiDX9FragmentShader::SetupShaderPrograms(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, 
    const NiSkinPartition::Partition* pkPartition, 
    NiGeometryData::RendererData* pkRendererData, 
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects, 
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
   
    return NiD3DShader::SetupShaderPrograms(pkGeometry, pkSkin, pkPartition,
        pkRendererData, pkState, pkEffects, kWorld, kWorldBound);
}
//---------------------------------------------------------------------------
NiGeometryData::RendererData* NiDX9FragmentShader::PrepareGeometryForRendering(
    NiGeometry* pkGeometry, const NiSkinPartition::Partition* pkPartition,
    NiGeometryData::RendererData* pkRendererData, 
    const NiPropertyState* pkState)
{
    return NiD3DShader::PrepareGeometryForRendering(pkGeometry,  pkPartition,
        pkRendererData, pkState);
}
//---------------------------------------------------------------------------
unsigned int NiDX9FragmentShader::PostRender(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, 
    const NiSkinPartition::Partition* pkPartition, 
    NiGeometryData::RendererData* pkRendererData, 
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects, 
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    return NiD3DShader::PostRender(pkGeometry, pkSkin, pkPartition,
        pkRendererData, pkState, pkEffects, kWorld, kWorldBound);
}
//---------------------------------------------------------------------------
unsigned int NiDX9FragmentShader::PostProcessPipeline(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, 
    NiGeometryData::RendererData* pkRendererData, 
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects,
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    
    return NiD3DShader::PostProcessPipeline(pkGeometry, pkSkin,
        pkRendererData, pkState, pkEffects, kWorld, kWorldBound);
}
//---------------------------------------------------------------------------
unsigned int NiDX9FragmentShader::FirstPass()
{
    return NiD3DShader::FirstPass();
}
//---------------------------------------------------------------------------
unsigned int NiDX9FragmentShader::NextPass()
{
    return NiD3DShader::NextPass();
}
//---------------------------------------------------------------------------
void NiDX9FragmentShader::DestroyRendererData()
{
    
    NiD3DShader::DestroyRendererData();
}
//---------------------------------------------------------------------------
void NiDX9FragmentShader::RecreateRendererData()
{
    NiD3DShader::RecreateRendererData();
}
//---------------------------------------------------------------------------
unsigned int NiDX9FragmentShader::GetSamplerValue(
    NiD3DRenderer* pkD3DRenderer,
    unsigned int uiState,
    unsigned int uiValue, 
    NiTexturingProperty::ClampMode eClampMode,
    NiTexturingProperty::FilterMode eFilterMode)
{    
    switch (uiState)
    {
        case NiD3DRenderState::NISAMP_ADDRESSU:
            return pkD3DRenderer->GetD3DClampMode(eClampMode).m_eU;
        case NiD3DRenderState::NISAMP_ADDRESSV:
            return pkD3DRenderer->GetD3DClampMode(eClampMode).m_eV;
        case NiD3DRenderState::NISAMP_MAGFILTER:
            return pkD3DRenderer->GetD3DFilterMode(eFilterMode).m_eMag;
        case NiD3DRenderState::NISAMP_MINFILTER:
            return pkD3DRenderer->GetD3DFilterMode(eFilterMode).m_eMin;
        case NiD3DRenderState::NISAMP_MIPFILTER:
            return pkD3DRenderer->GetD3DFilterMode(eFilterMode).m_eMip;
        default:
            // Invalid sampler state.
            NIASSERT(false);
            return uiValue;
    }
}
//---------------------------------------------------------------------------
void NiDX9FragmentShader::PrepareTextureStage(NiGeometry* pkGeometry,
    NiD3DTextureStage* pkStage,
    const NiTexturingProperty* pkTexProp,
    const NiDynamicEffectState* pkEffects,
    unsigned int uiPass)
{
    NiTexture* pkTexture = NULL;
    NiTexturingProperty::ClampMode eClampMode =
        NiTexturingProperty::CLAMP_S_CLAMP_T;
    NiTexturingProperty::FilterMode eFilterMode =
        NiTexturingProperty::FILTER_NEAREST;

    unsigned int uiTextureFlags = pkStage->GetTextureFlags();
    unsigned short usObjTextureFlags = pkStage->GetObjTextureFlags();

    if (usObjTextureFlags == NiD3DTextureStage::TSOTF_IGNORE)
    {
        if (uiTextureFlags != NiD3DTextureStage::TSTF_IGNORE)
        {
            const NiTexturingProperty::Map* pkMap = NULL;
            NIASSERT(pkTexProp);
            if ((uiTextureFlags & NiD3DTextureStage::TSTF_MAP_MASK) == 0)
            {
                switch (uiTextureFlags & NiD3DTextureStage::TSTF_NDL_TYPEMASK)
                {
                case NiD3DTextureStage::TSTF_NONE:
                    break;
                case NiD3DTextureStage::TSTF_NDL_BASE:
                    pkMap = pkTexProp->GetBaseMap();
                    break;
                case NiD3DTextureStage::TSTF_NDL_DARK:
                    pkMap = pkTexProp->GetDarkMap();
                    break;
                case NiD3DTextureStage::TSTF_NDL_DETAIL:
                    pkMap = pkTexProp->GetDetailMap();
                    break;
                case NiD3DTextureStage::TSTF_NDL_GLOSS:
                    pkMap = pkTexProp->GetGlossMap();
                    break;
                case NiD3DTextureStage::TSTF_NDL_GLOW:
                    pkMap = pkTexProp->GetGlowMap();
                    break;
                case NiD3DTextureStage::TSTF_NDL_BUMP:
                    pkMap = pkTexProp->GetBumpMap();
                    break;
                case NiD3DTextureStage::TSTF_NDL_NORMAL:
                    pkMap = pkTexProp->GetNormalMap();
                    break;
                case NiD3DTextureStage::TSTF_NDL_PARALLAX:
                    pkMap = pkTexProp->GetParallaxMap();
                    break;
                
                }
            }
            else if ((uiTextureFlags & NiD3DTextureStage::TSTF_MAP_MASK) ==
                NiD3DTextureStage::TSTF_MAP_DECAL)
            {
                unsigned int uiIndex = uiTextureFlags &
                    NiD3DTextureStage::TSTF_INDEX_MASK;
                if (uiIndex < pkTexProp->GetDecalArrayCount())
                {
                    pkMap = pkTexProp->GetDecalMap(uiIndex);
                }
            }
            else if ((uiTextureFlags & NiD3DTextureStage::TSTF_MAP_MASK) ==
                NiD3DTextureStage::TSTF_MAP_SHADER)
            {
                unsigned int uiIndex = uiTextureFlags &
                    NiD3DTextureStage::TSTF_INDEX_MASK;
                if (uiIndex < pkTexProp->GetShaderArrayCount())
                {
                    pkMap = pkTexProp->GetShaderMap(uiIndex);
                }
            }

            if (pkMap)
            {
                pkTexture = pkMap->GetTexture();
                eClampMode = pkMap->GetClampMode();
                eFilterMode = pkMap->GetFilterMode();
                // What else is needed?

                // If we are to use the index from the map, set that now
                // as well
                if ((uiTextureFlags & NiD3DTextureStage::TSTF_MAP_USE_MASK) == 
                    NiD3DTextureStage::TSTF_MAP_USE_INDEX)
                {
                    unsigned int uiTexCoordIndex = pkMap->GetTextureIndex();

                    // We need to take the transformation info into account
                    uiTexCoordIndex |= pkStage->GetTexCoordIndex() &
                        ~NiD3DTextureStage::TSTF_INDEX_MASK;
                    
                    pkStage->SetStageState(D3DTSS_TEXCOORDINDEX,
                        uiTexCoordIndex, pkStage->GetSaveTexCoordIndex());
                }
            }
        }
    }
    else if (pkEffects)
    {
        NiTextureEffect* pkTextureEffect = NULL;
        unsigned short usObjectIndex = usObjTextureFlags &
            NiD3DTextureStage::TSOTF_INDEX_MASK;
        switch ((usObjTextureFlags & NiD3DTextureStage::TSOTF_TYPE_MASK) >>
            NiD3DTextureStage::TSOTF_TYPE_SHIFT)
        {
            case NiShaderAttributeDesc::OT_EFFECT_DIRSHADOWMAP:
                {
                    NiLight* pkLight  = (NiLight*)
                        NiShaderConstantMap::GetDynamicEffectForObject(
                        pkEffects, NiShaderAttributeDesc::
                        OT_EFFECT_SHADOWDIRECTIONALLIGHT, usObjectIndex);
                    NIASSERT(pkLight);

                    NiShadowGenerator* pkGenerator = 
                        pkLight->GetShadowGenerator();
                    NIASSERT(pkGenerator);

                    NiShadowMap* pkShadowMap = 
                        pkGenerator->RetrieveShadowMap(
                        NiShadowGenerator::AUTO_DETERMINE_SM_INDEX, 
                        pkGeometry);
                    NIASSERT(pkShadowMap);

                    pkTexture = pkShadowMap->GetTexture();
                    eClampMode = (NiTexturingProperty::ClampMode)
                        pkShadowMap->GetClampMode();
                    eFilterMode = (NiTexturingProperty::FilterMode)
                        pkShadowMap->GetFilterMode();
                    break;
                }

            case NiShaderAttributeDesc::OT_EFFECT_POINTSHADOWMAP:
                {
                    NiLight* pkLight  = (NiLight*)
                        NiShaderConstantMap::GetDynamicEffectForObject(
                        pkEffects, 
                        NiShaderAttributeDesc::OT_EFFECT_SHADOWPOINTLIGHT, 
                        usObjectIndex);
                    NIASSERT(pkLight);

                    NiShadowGenerator* pkGenerator = 
                        pkLight->GetShadowGenerator();
                    NIASSERT(pkGenerator);

                    NiShadowMap* pkShadowMap = 
                        pkGenerator->RetrieveShadowMap(
                        NiShadowGenerator::AUTO_DETERMINE_SM_INDEX, 
                        pkGeometry);
                    NIASSERT(pkShadowMap);

                    pkTexture = pkShadowMap->GetTexture();
                    eClampMode = (NiTexturingProperty::ClampMode)
                        pkShadowMap->GetClampMode();
                    eFilterMode = (NiTexturingProperty::FilterMode)
                        pkShadowMap->GetFilterMode();
                    break;
                }

            case NiShaderAttributeDesc::OT_EFFECT_SPOTSHADOWMAP:
                {
                    NiLight* pkLight  = (NiLight*)
                        NiShaderConstantMap::GetDynamicEffectForObject(
                        pkEffects, 
                        NiShaderAttributeDesc::OT_EFFECT_SHADOWSPOTLIGHT, 
                        usObjectIndex);
                    NIASSERT(pkLight);

                    NiShadowGenerator* pkGenerator = 
                        pkLight->GetShadowGenerator();
                    NIASSERT(pkGenerator);

                    NiShadowMap* pkShadowMap = 
                        pkGenerator->RetrieveShadowMap(
                        NiShadowGenerator::AUTO_DETERMINE_SM_INDEX, 
                        pkGeometry);
                    NIASSERT(pkShadowMap);

                    pkTexture = pkShadowMap->GetTexture();
                    eClampMode = (NiTexturingProperty::ClampMode)
                        pkShadowMap->GetClampMode();
                    eFilterMode = (NiTexturingProperty::FilterMode)
                        pkShadowMap->GetFilterMode();
                    break;
                }

            case NiShaderAttributeDesc::OT_EFFECT_ENVIRONMENTMAP:
                pkTextureEffect = pkEffects->GetEnvironmentMap();
                if (pkTextureEffect && !pkTextureEffect->GetSwitch())
                    pkTextureEffect = NULL;
                break;
            case NiShaderAttributeDesc::OT_EFFECT_PROJECTEDSHADOWMAP:
            {
                unsigned short usIndex = 0;
                NiDynEffectStateIter kIter =
                    pkEffects->GetProjShadowHeadPos();
                while (kIter)
                {
                    NiTextureEffect* pkProjShadow =
                        pkEffects->GetNextProjShadow(kIter);
                    if (pkProjShadow && pkProjShadow->GetSwitch() == true)
                    {
                        if (usIndex++ == usObjectIndex)
                        {
                            pkTextureEffect = pkProjShadow;
                            break;
                        }
                    }
                }
                break;
            }
            case NiShaderAttributeDesc::OT_EFFECT_PROJECTEDLIGHTMAP:
            {
                unsigned short usIndex = 0;
                NiDynEffectStateIter kIter =
                    pkEffects->GetProjLightHeadPos();
                while (kIter)
                {
                    NiTextureEffect* pkProjLight =
                        pkEffects->GetNextProjLight(kIter);
                    if (pkProjLight && pkProjLight->GetSwitch() == true)
                    {
                        if (usIndex++ == usObjectIndex)
                        {
                            pkTextureEffect = pkProjLight;
                            break;
                        }
                    }
                }
                break;
            }
            case NiShaderAttributeDesc::OT_EFFECT_FOGMAP:
                pkTextureEffect = pkEffects->GetFogMap();
                if (pkTextureEffect && !pkTextureEffect->GetSwitch())
                    pkTextureEffect = NULL;
                break;
            default:
                // This assertion is hit when the object type is not one
                // that has a texture.
                NIASSERT(false);
                break;
        }

        if (pkTextureEffect)
        {
            pkTexture = pkTextureEffect->GetEffectTexture();
            eClampMode = pkTextureEffect->GetTextureClamp();
            eFilterMode = pkTextureEffect->GetTextureFilter();

            // If UseMapIndex is specified, grab and set the tex coord gen
            // mode and 
            if ((uiTextureFlags & NiD3DTextureStage::TSTF_MAP_USE_MASK) == 
                NiD3DTextureStage::TSTF_MAP_USE_INDEX)
            {
                PackDynamicEffect(pkTextureEffect, pkStage,
                    pkStage->GetSaveTexCoordIndex(), m_pkD3DRenderer);
            }
        }
    }

    // Even if the texture is NULL, we need to set that here to prevent a 
    // stale texture from getting used by accident.
    pkStage->SetTexture(pkTexture);
    pkStage->SetClampMode(eClampMode);
    pkStage->SetFilterMode(eFilterMode);
}
//---------------------------------------------------------------------------
const NiShader::NiShaderInstanceDescriptor* 
    NiDX9FragmentShader::GetShaderInstanceDesc() const
{
    return &m_kDescriptor;
}
//---------------------------------------------------------------------------
