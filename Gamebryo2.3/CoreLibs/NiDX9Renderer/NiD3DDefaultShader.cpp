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

#include "NiD3DDefaultShader.h"
#include "NiD3DRendererHeaders.h"
#include <NiDirectionalLight.h>
#include <NiFogProperty.h>
#include <NiPointLight.h>
#include <NiSourceTexture.h>

NiImplementRTTI(NiD3DDefaultShader, NiD3DShader);

//---------------------------------------------------------------------------
NiD3DDefaultShader::NiD3DDefaultShader() :
    NiD3DShader(),
    m_bBlendTextureAlpha(false),
#if defined(D3D_NI42_TEXTURE_PIPELINE)
    m_bQuickDecals(false),
#else //#if defined(D3D_NI42_TEXTURE_PIPELINE)
    m_bQuickDecals(true),
#endif //#if defined(D3D_NI42_TEXTURE_PIPELINE)
    m_eEnvBumpOp(D3DTOP_DISABLE),
    m_peFogPassArray(NULL),
    m_uiFogPassArraySize(0),
    m_pkLastState(NULL)
{
    m_kName = "Default Shader From Gamebryo 2.1";
    SetIsBestImplementation(true);
}
//---------------------------------------------------------------------------
NiD3DDefaultShader::~NiD3DDefaultShader()
{
    ClearProjectedTextures();

    NiFree(m_peFogPassArray);
}
//---------------------------------------------------------------------------
bool NiD3DDefaultShader::Initialize()
{
    // Quick-out if already initialized
    if (m_bInitialized)
        return true;

    if (!NiD3DShader::Initialize())
        return false;

    InitializeDeviceCaps();

    return true;
}
//---------------------------------------------------------------------------
unsigned int NiD3DDefaultShader::PreProcessPipeline(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, NiGeometryData::RendererData* pkRendererData,
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects, 
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    // Update the render state
    m_pkD3DRenderState->UpdateRenderState(pkState);

// Xenon handles lights via shaders and does not have a light manager.
#if !defined(_XENON)
    // Update the light state
    m_pkD3DRenderer->GetLightManager()->SetState(pkEffects, 
        pkState->GetTexturing(), pkState->GetVertexColor());
#endif

    unsigned int uiRet = NiD3DShader::PreProcessPipeline(pkGeometry, pkSkin, 
        pkRendererData, pkState, pkEffects, kWorld, kWorldBound);

    // Save lighting state in case lighting needs to be disabled
#if !defined(_XENON)
    m_uiOldLightingValue = 
        m_pkD3DRenderState->GetRenderState(D3DRS_LIGHTING);
#endif  //#if !defined(_XENON)

    return uiRet;
};
//---------------------------------------------------------------------------
unsigned int NiD3DDefaultShader::UpdatePipeline(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, NiGeometryData::RendererData* pkRendererData,
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects, 
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    // Clear out values from the last run
    ResetPasses();

    NiGeometryBufferData* pkBuffData = (NiGeometryBufferData*)pkRendererData;
    return ConstructPipeline(pkGeometry, pkSkin, pkBuffData, pkState, 
        pkEffects, kWorld, kWorldBound);
}
//---------------------------------------------------------------------------
unsigned int NiD3DDefaultShader::PostProcessPipeline(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, NiGeometryData::RendererData* pkRendererData,
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects, 
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    // Restore lighting state in case lighting needs to be disabled
#if !defined(_XENON)
    m_pkD3DRenderState->SetRenderState(D3DRS_LIGHTING, m_uiOldLightingValue);
#endif  //#if !defined(_XENON)

    return NiD3DShader::PostProcessPipeline(pkGeometry, pkSkin, 
        pkRendererData, pkState, pkEffects, kWorld, kWorldBound);
};
//---------------------------------------------------------------------------
unsigned int NiD3DDefaultShader::ConstructPipeline(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, NiGeometryBufferData* pkBuffData, 
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects, 
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    // Make sure we have the rendering information we need.
    if (pkGeometry == NULL)
    {
        if (pkBuffData == NULL || !pkBuffData->IsVBChipValid() ||
            pkState == NULL)
        {
            return 0xFFFFFFFF;
        }
    }
    else if (pkBuffData == NULL)
        return 0xFFFFFFFF;

    NIASSERT(pkGeometry == NULL || pkGeometry->GetModelData() != NULL);

    // Create and store projected effects
    UpdateProjectedTextures(pkEffects);

    // Query properties and effects for textures
    const NiTexturingProperty* pkTexProp = pkState->GetTexturing();
    const NiTexturingProperty::Map* pkBaseMap = NULL;
    const NiTexturingProperty::Map* pkDetailMap = NULL;
    const NiTexturingProperty::Map* pkDarkMap = NULL;
    const NiTexturingProperty::BumpMap* pkBumpMap = NULL;
    const NiTexturingProperty::Map* pkGlowMap = NULL;
    const NiTexturingProperty::Map* pkGlossMap = NULL;

    unsigned short usNumTextureSets = 0;

    if (pkBuffData == NULL)
    {
        pkBuffData = (NiGeometryBufferData*)
            pkGeometry->GetModelData()->GetRendererData();
    }
    NIASSERT(pkBuffData != NULL);

    if (pkGeometry == NULL || 
        pkGeometry->GetModelData()->GetConsistency() == NiGeometryData::STATIC)
    {
        usNumTextureSets = 
            m_pkD3DRenderer->GetNumUVsFromFlags(pkBuffData->GetFlags());
    }
    else 
    {
        usNumTextureSets = pkGeometry->GetTextureSets();
    }

    if (usNumTextureSets > NiD3DPass::ms_uiMaxTextureBlendStages)
        usNumTextureSets = NiD3DPass::ms_uiMaxTextureBlendStages;

    m_uiMaxTextureIndex = 0;
    if (usNumTextureSets != 0)
    {
        pkBaseMap = pkTexProp->GetBaseMap();
        pkDetailMap = pkTexProp->GetDetailMap();
        pkDarkMap = pkTexProp->GetDarkMap();
        pkBumpMap = pkTexProp->GetBumpMap();
        pkGlowMap = pkTexProp->GetGlowMap();
        pkGlossMap = pkTexProp->GetGlossMap();

        m_uiMaxTextureIndex = usNumTextureSets - 1;
    }

    // Default pipeline only considers one map except for decal maps
    unsigned int uiNumBaseMaps = 
        (pkBaseMap && pkBaseMap->GetTexture() != 0 ? 1 : 0);
    unsigned int uiNumDetailMaps = 
        (pkDetailMap && pkDetailMap->GetTexture() != 0 ? 1 : 0);
    unsigned int uiNumDarkMaps = 
        (pkDarkMap && pkDarkMap->GetTexture() != 0 ? 1 : 0);
    unsigned int uiNumBumpMaps = 
        (pkBumpMap && pkBumpMap->GetTexture() != 0 ? 1 : 0);
    unsigned int uiNumDecalMaps = pkTexProp->GetDecalMapCount();
    unsigned int uiNumGlowMaps = 
        (pkGlowMap && pkGlowMap->GetTexture() != 0 ? 1 : 0);
    unsigned int uiNumGlossMaps = 
        (pkGlossMap && pkGlossMap->GetTexture() != 0 ? 1 : 0);

    // Remove any empty decal maps
    unsigned int uiNumDecalSlots = pkTexProp->GetDecalArrayCount();
    for (unsigned int uiDecal = 0; uiDecal < uiNumDecalSlots; uiDecal++)
    {
        const NiTexturingProperty::Map* pkDecalMap = 
            pkTexProp->GetDecalMap(uiDecal);
        if (pkDecalMap && pkDecalMap->GetTexture() == NULL)
            uiNumDecalMaps--;
    }

    const unsigned int uiProjLights = GetProjectedLightCount();
    const unsigned int uiClipProjLights = GetClippedProjectedLightCount();
    const unsigned int uiProjShadows = GetProjectedShadowCount();
    const unsigned int uiClipProjShadows = GetClippedProjectedShadowCount();
    const unsigned int uiEnvironmentMaps = GetEnvironmentMapCount();
    const unsigned int uiFogMaps = GetFogMapCount();

    bool bEnvMap = (uiEnvironmentMaps > 0);

    // Check for a cube map - can't bump map a cube map.
    if (bEnvMap && uiNumBumpMaps)
    {
        const NiTextureEffect* pkEffect = GetEnvironmentMap(0);
        NIASSERT(pkEffect);
        if (pkEffect->GetTextureCoordGen() != NiTextureEffect::SPHERE_MAP)
            uiNumBumpMaps = false;
    }

    NiTexturingProperty::ApplyMode eApply = pkTexProp->GetApplyMode();
    bool bVertexColors = (eApply != NiTexturingProperty::APPLY_REPLACE);

    // Query alpha property
    // For combining multiple passes, alpha blending is assumed to be
    // SRC*SRCALPHA + DEST*INVSRCALPHA
    const NiAlphaProperty* pkAlphaProp = pkState->GetAlpha();
    bool bAlphaBlending = pkAlphaProp->GetAlphaBlending();
    if (bAlphaBlending)
    {
        // Only consider alpha blending that actually uses alpha values
        NiAlphaProperty::AlphaFunction eSrc = 
            pkAlphaProp->GetSrcBlendMode();
        NiAlphaProperty::AlphaFunction eDest = 
            pkAlphaProp->GetDestBlendMode();
        if (eSrc != NiAlphaProperty::ALPHA_SRCALPHA &&
            eSrc != NiAlphaProperty::ALPHA_INVSRCALPHA &&
            eSrc != NiAlphaProperty::ALPHA_DESTALPHA &&
            eSrc != NiAlphaProperty::ALPHA_INVDESTALPHA &&
            eSrc != NiAlphaProperty::ALPHA_SRCALPHASAT &&
            eDest != NiAlphaProperty::ALPHA_SRCALPHA &&
            eDest != NiAlphaProperty::ALPHA_INVSRCALPHA &&
            eDest != NiAlphaProperty::ALPHA_DESTALPHA &&
            eDest != NiAlphaProperty::ALPHA_INVDESTALPHA &&
            eDest != NiAlphaProperty::ALPHA_SRCALPHASAT)
        {
            bAlphaBlending = false;
        }
    }
    bool bAlphaTesting = pkAlphaProp->GetAlphaTesting();
    bool bAlphaRequiresTexture = false;
    bool bAlphaRequiresVertexColor = false;
    if (bAlphaBlending || bAlphaTesting)
    {
        bAlphaRequiresTexture = (pkBaseMap != NULL) &&
            (eApply != NiTexturingProperty::APPLY_DECAL);
        bAlphaRequiresVertexColor = bVertexColors;
    }

    // Consider dark/projected lights along with base/detail/VC
    unsigned int uiNumLightTextures = uiProjLights + uiClipProjLights * 2 + 
        uiNumDarkMaps;

    // Calculate the number of textures and stages necessary to render the
    // base map, detail map, and vertex colors
    unsigned int uiNumBaseTextures = uiNumBaseMaps + uiNumDetailMaps;

    bool bNeedsVertexColors = bVertexColors;

    unsigned int uiRemainingClipProjLights = uiClipProjLights;
    unsigned int uiRemainingProjLights = uiProjLights;

    while (bNeedsVertexColors || uiNumLightTextures + uiNumBaseTextures > 0)
    {
        // One extra stage may needed for vertex colors
        unsigned int uiNumBaseStages = uiNumBaseTextures;
        if (bNeedsVertexColors && (uiRemainingClipProjLights != 0 ||
            uiNumLightTextures > 1 || uiNumBaseTextures == 0))
        {
            uiNumBaseStages++;
        }

        // Test to see if all lights and base texture fit in one pass
        // Only test against ms_uiMaxSimultaneousTextures because no textures
        // can go in stages beyone the max texture count.
        bool bOnePass = true;
        if (m_uiPassCount > 0)
        {
            // In secondary passes, lights cannot be combined with base maps
            bOnePass = (uiNumLightTextures == 0 && 
                uiNumBaseStages <= NiD3DPass::ms_uiMaxSimultaneousTextures);
        }
        else
        {
            // In the first pass, lights and base maps can all coexist
            bOnePass = (uiClipProjLights <= 1 &&
                ((uiNumLightTextures + uiNumBaseStages) <=
                NiD3DPass::ms_uiMaxSimultaneousTextures));
        }

        // Open a new pass
        bool bTexAlpha = (!bOnePass && bAlphaRequiresTexture);
        bool bVCAlpha = (!bOnePass && bAlphaRequiresVertexColor);
        if (uiNumLightTextures != 0)
        {
            // Add additional lights
            if (bAlphaBlending)
            {   
                // Alpha blending op already applied to dest - 
                //   apply alpha blending op to source
                NiAlphaProperty::AlphaFunction eSrc = 
                    pkAlphaProp->GetSrcBlendMode();
                D3DBLEND eD3DSrc = 
                    (D3DBLEND)m_pkD3DRenderState->GetD3DBlendMode(eSrc);
                OpenNewPass(eD3DSrc, D3DBLEND_ONE, bTexAlpha, bVCAlpha);
            }
            else
            {
                OpenNewPass(D3DBLEND_ONE, D3DBLEND_ONE, bTexAlpha, bVCAlpha);
            }
        }
        else
        {
            // Multiply base colors by sum of lights
            OpenNewPass(D3DBLEND_ZERO, D3DBLEND_SRCCOLOR, bTexAlpha, bVCAlpha);
        }

        // Begin work on this pass
        bool bFirstStageFilled = false;

        // Add clipped projected light if possible
        if (uiRemainingClipProjLights != 0 && 
            m_uiRemainingStages > 0 && m_uiRemainingTextures > 0)
        {
            // Add clipped projected light first
            // (along with VC for alpha if bAlphaRequiresVertexColor)
            const NiTextureEffect* pkClippedLight = 
                GetClippedProjectedLight(uiClipProjLights - 
                uiRemainingClipProjLights);
            NiD3DTextureStage* pkLight = AddTextureStage(pkClippedLight);

            uiNumLightTextures--;
            uiRemainingClipProjLights--;

            // Add clipper (skip if not enough room available)
            if (m_uiRemainingStages > 0 && m_uiRemainingTextures > 0)
            {
                NiD3DTextureStage* pkClipper = AddClipperStage(pkClippedLight);
                pkClipper->SetStageState(D3DTSS_COLOROP, D3DTOP_MODULATE);
                pkClipper->SetStageState(D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
            }
            // Record clipper texture even if skipped
            uiNumLightTextures--;

            bFirstStageFilled = true;
        }
        // Add dark maps
        while (uiNumDarkMaps != 0 && 
            m_uiRemainingStages > 0 && m_uiRemainingTextures > 0)
        {
            NIASSERT(pkDarkMap);
            unsigned short usTextureSet = pkDarkMap->GetTextureIndex();
            if (usTextureSet > m_uiMaxTextureIndex)
                usTextureSet = m_uiMaxTextureIndex;
            NiD3DTextureStage* pkDark = AddTextureStage(pkDarkMap, 
                usTextureSet);
            if (bFirstStageFilled)
            {
                pkDark->SetStageState(D3DTSS_COLOROP, D3DTOP_ADD);
            }
            else
            {
                if (bNeedsVertexColors && 
                    (uiProjLights + uiClipProjLights + uiNumDarkMaps < 2))
                {
                    // Insert color if this is only light map
                    pkDark->SetStageState(D3DTSS_COLOROP, D3DTOP_MODULATE);
                    pkDark->SetStageState(D3DTSS_COLORARG2, D3DTA_DIFFUSE);
#if defined(D3D_NI42_TEXTURE_PIPELINE)
                    pkDark->SetStageState(D3DTSS_ALPHAOP, D3DTOP_MODULATE);
#else //#if defined(D3D_NI42_TEXTURE_PIPELINE)
                    pkDark->SetStageState(D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
#endif //#if defined(D3D_NI42_TEXTURE_PIPELINE)
                    pkDark->SetStageState(D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

                    bNeedsVertexColors = false;
                }
                else
                {
#if defined(D3D_NI42_TEXTURE_PIPELINE)
                    pkDark->SetStageState(D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
#endif //#if defined(D3D_NI42_TEXTURE_PIPELINE)
                }

                bFirstStageFilled = true;
            }

            uiNumDarkMaps--;
            uiNumLightTextures--;
        }

        // Add light maps
        while (uiRemainingProjLights != 0 && 
            m_uiRemainingStages > 0 && m_uiRemainingTextures > 0)
        {
            NiD3DTextureStage* pkLight = AddTextureStage(GetProjectedLight(
                uiProjLights - uiRemainingProjLights));
            if (bFirstStageFilled)
            {
                pkLight->SetStageState(D3DTSS_COLOROP, D3DTOP_ADD);
            }
            else
            {
                if (bNeedsVertexColors && 
                    (uiProjLights + uiClipProjLights + uiNumDarkMaps < 2))
                {
                    // Insert color if this is only light map 
                    pkLight->SetStageState(D3DTSS_COLOROP, D3DTOP_MODULATE);
                    pkLight->SetStageState(D3DTSS_COLORARG2, D3DTA_DIFFUSE);
                    pkLight->SetStageState(D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
                    pkLight->SetStageState(D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

                    bNeedsVertexColors = false;
                }

                bFirstStageFilled = true;
            }

            uiRemainingProjLights--;
            uiNumLightTextures--;
        }

        if (bOnePass)
        {
            // No other lights or dark maps should exist
            NIASSERT(uiNumLightTextures == 0 && uiNumDarkMaps == 0 &&
                uiRemainingProjLights == 0);

            // Must be enough room for base, detail, and/or vertex colors
            NIASSERT(m_uiRemainingStages >= uiNumBaseStages && 
                m_uiRemainingTextures >= uiNumBaseTextures);

            // Add base map, detail map, and vertex colors
            if (uiNumBaseMaps > 0)
            {
                unsigned short usTextureSet = pkBaseMap->GetTextureIndex();
                if (usTextureSet > m_uiMaxTextureIndex)
                    usTextureSet = m_uiMaxTextureIndex;
                NiD3DTextureStage* pkBase = AddTextureStage(pkBaseMap, 
                    usTextureSet);
                NIASSERT(pkBase);
                if (bFirstStageFilled)
                {
                    if (bAlphaRequiresVertexColor && !bNeedsVertexColors)
                    {
                        // VC already in pipeline
                        pkBase->SetStageState(D3DTSS_COLOROP, D3DTOP_MODULATE);
                        pkBase->SetStageState(D3DTSS_ALPHAOP, D3DTOP_MODULATE);
                    }
                    else
                    {
                        pkBase->SetStageState(D3DTSS_COLOROP, D3DTOP_MODULATE);
#if defined(D3D_NI42_TEXTURE_PIPELINE)
                        pkBase->SetStageState(D3DTSS_ALPHAOP, D3DTOP_MODULATE);
#else //#if defined(D3D_NI42_TEXTURE_PIPELINE)
                        pkBase->SetStageState(D3DTSS_ALPHAOP, 
                            D3DTOP_SELECTARG1);
#endif //#if defined(D3D_NI42_TEXTURE_PIPELINE)
                    }
                }
                else
                {
                    if (bNeedsVertexColors)
                    {
                        pkBase->SetStageState(D3DTSS_COLOROP, D3DTOP_MODULATE);
                        pkBase->SetStageState(D3DTSS_COLORARG2, D3DTA_DIFFUSE);
                        pkBase->SetStageState(D3DTSS_ALPHAOP, D3DTOP_MODULATE);
                        pkBase->SetStageState(D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
                        bNeedsVertexColors = false;
                    }
                    else
                    {
                        pkBase->SetStageState(D3DTSS_COLOROP, 
                            D3DTOP_SELECTARG1);
                        pkBase->SetStageState(D3DTSS_ALPHAOP, 
                            D3DTOP_SELECTARG1);
                    }
                }

                uiNumBaseTextures--;
                bFirstStageFilled = true;
            }

            if (uiNumDetailMaps > 0)
            {
                NIASSERT(pkDetailMap);
                unsigned short usTextureSet = pkDetailMap->GetTextureIndex();
                if (usTextureSet > m_uiMaxTextureIndex)
                    usTextureSet = m_uiMaxTextureIndex;
                NiD3DTextureStage* pkDetail = AddTextureStage(pkDetailMap, 
                    usTextureSet);
                // Detail map requires base map or vertex colors
                NIASSERT(bVertexColors || uiNumBaseMaps != 0);
                if (!bFirstStageFilled)
                {
                    NIASSERT(bNeedsVertexColors);
                    pkDetail->SetStageState(D3DTSS_COLORARG2, D3DTA_DIFFUSE);
                    pkDetail->SetStageState(D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
                    pkDetail->SetStageState(D3DTSS_ALPHAOP, D3DTOP_MODULATE2X);
                    bNeedsVertexColors = false;
                    bFirstStageFilled = true;
                }
                pkDetail->SetStageState(D3DTSS_COLOROP, D3DTOP_MODULATE2X);
#if defined(D3D_NI42_TEXTURE_PIPELINE)
                pkDetail->SetStageState(D3DTSS_ALPHAOP, D3DTOP_MODULATE2X);
#endif //#if defined(D3D_NI42_TEXTURE_PIPELINE)

                uiNumBaseTextures--;
            }

            if (bNeedsVertexColors)
            {
                NiD3DTextureStage* pkVC = AddTextureStage(
                    (NiTexturingProperty::Map*)NULL, 0);
                if (bFirstStageFilled && 
                    eApply == NiTexturingProperty::APPLY_MODULATE)
                {
                    pkVC->SetStageState(D3DTSS_COLOROP, D3DTOP_MODULATE);
                    pkVC->SetStageState(D3DTSS_ALPHAOP, D3DTOP_MODULATE);
                }
                else
                {
                    pkVC->SetStageState(D3DTSS_COLOROP, D3DTOP_SELECTARG1);
                    pkVC->SetStageState(D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
                }
                bFirstStageFilled = true;
                bNeedsVertexColors = false;
            }

            // The first stage must have been filled at this point
            NIASSERT(bFirstStageFilled);

            // No more passes should be required
            NIASSERT(!bNeedsVertexColors &&
                uiNumLightTextures + uiNumBaseTextures == 0);
        }
        else
        {
            // Get ready for more passes
            ClosePass(pkBaseMap);
        }
    }

    // Consider decal maps
    if (uiNumDecalMaps != 0)
    {
        unsigned int uiDecalIndex = 0;
        if (m_bBlendTextureAlpha && m_bQuickDecals && m_uiPassCount == 0 && 
            !bAlphaBlending && !bAlphaTesting && !bVertexColors) 
        {
            while (uiNumDecalMaps != 0 &&
                m_uiRemainingStages > 0 && m_uiRemainingTextures > 0)
            {
                const NiTexturingProperty::Map* pkDecalMap = 
                    pkTexProp->GetDecalMap(uiDecalIndex++);
                if (pkDecalMap == NULL || pkDecalMap->GetTexture() == NULL)
                    continue;
                unsigned short usTextureSet = pkDecalMap->GetTextureIndex();
                if (usTextureSet > m_uiMaxTextureIndex)
                    usTextureSet = m_uiMaxTextureIndex;

                NiD3DTextureStage* pkDecal = AddTextureStage(pkDecalMap, 
                    usTextureSet);
                pkDecal->SetStageState(D3DTSS_COLOROP, 
                    D3DTOP_BLENDTEXTUREALPHA);

                uiNumDecalMaps--;
            }

            // Check to see if the decals are done
            if (uiNumDecalMaps != 0)
                ClosePass();
        }
        else
        {
            // Must start new pass for decals
            ClosePass(pkBaseMap);
        }

        // Remaining decal maps
        while (uiNumDecalMaps != 0)
        {
            const NiTexturingProperty::Map* pkDecalMap = 
                pkTexProp->GetDecalMap(uiDecalIndex++);
            if (pkDecalMap == NULL || pkDecalMap->GetTexture() == NULL)
                continue;
            unsigned short usTextureSet = pkDecalMap->GetTextureIndex();
            if (usTextureSet > m_uiMaxTextureIndex)
                usTextureSet = m_uiMaxTextureIndex;

            OpenNewPass(D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);

            NiD3DTextureStage* pkDecal = AddTextureStage(pkDecalMap, 
                usTextureSet);
            if (m_bBlendTextureAlpha && m_bQuickDecals && !bVertexColors)
            {
                // First stage, color = decal*alpha, alpha = alpha
                pkDecal->SetStageState(D3DTSS_COLOROP, D3DTOP_MODULATE);
                pkDecal->SetStageState(D3DTSS_COLORARG2, 
                    D3DTA_TEXTURE | D3DTA_ALPHAREPLICATE);
                pkDecal->SetStageState(D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

                uiNumDecalMaps--;

                // Remaining stages are blended in, alphas are summed
                while (uiNumDecalMaps != 0 && 
                    m_uiRemainingStages > 0 && m_uiRemainingTextures > 0)
                {
                    pkDecalMap = pkTexProp->GetDecalMap(uiDecalIndex++);
                    if (pkDecalMap == NULL || pkDecalMap->GetTexture() == NULL)
                        continue;
                    usTextureSet = pkDecalMap->GetTextureIndex();
                    if (usTextureSet > m_uiMaxTextureIndex)
                        usTextureSet = m_uiMaxTextureIndex;

                    pkDecal = AddTextureStage(pkDecalMap, usTextureSet);
                    pkDecal->SetStageState(D3DTSS_COLOROP, 
                        D3DTOP_BLENDTEXTUREALPHA);
                    pkDecal->SetStageState(D3DTSS_ALPHAOP, D3DTOP_ADD);

                    uiNumDecalMaps--;
                }

                // Check to see if the decals are done
                if (uiNumDecalMaps != 0)
                    ClosePass();
            }
            else
            {
                // Each decal gets its own pass
                if (bVertexColors)
                {
                    pkDecal->SetStageState(D3DTSS_COLOROP, D3DTOP_MODULATE);
                    pkDecal->SetStageState(D3DTSS_ALPHAOP, D3DTOP_MODULATE);
                    pkDecal->SetStageState(D3DTSS_COLORARG2, D3DTA_DIFFUSE);
                    pkDecal->SetStageState(D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
                }
                else
                {
                    pkDecal->SetStageState(D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
                }

                uiNumDecalMaps--;
                ClosePass();
            }
        }
    }

    // Consider projected shadows
    if (uiProjShadows != 0 || uiClipProjShadows != 0)
    {
        unsigned int uiRemainingClipProjShadows = uiClipProjShadows;
        unsigned int uiRemainingProjShadows = uiProjShadows;

        if (uiRemainingClipProjShadows ||
            !IsCurrentCombineMode(D3DBLEND_ZERO, D3DBLEND_SRCCOLOR))
        {
            ClosePass(pkBaseMap);
        }

        unsigned int uiNumShadowTextures = uiClipProjShadows + uiProjShadows;

        while (uiNumShadowTextures != 0)
        {
            bool bFirstStageFilled = true;

            if (m_spCurrentPass == NULL)
            {
                OpenNewPass(D3DBLEND_ZERO, D3DBLEND_SRCCOLOR,
                    bAlphaRequiresTexture, bAlphaRequiresVertexColor);
                bFirstStageFilled = false;
            }

            if (!bFirstStageFilled && uiRemainingClipProjShadows != 0)
            {
                // Add clipped projected shadow first
                // (along with VC for alpha if bAlphaRequiresVertexColor)
                const NiTextureEffect* pkClippedShadow = 
                    GetClippedProjectedShadow(uiClipProjShadows - 
                    uiRemainingClipProjShadows);
                NiD3DTextureStage* pkShadow = AddTextureStage(pkClippedShadow);
 
                uiNumShadowTextures--;
                uiRemainingClipProjShadows--;

                // Add clipper (skip if not enough room available)
                if (m_uiRemainingStages > 0 && m_uiRemainingTextures > 0)
                {
                    NiD3DTextureStage* pkClipper = 
                        AddClipperStage(pkClippedShadow);
                    pkClipper->SetStageState(D3DTSS_COLOROP, D3DTOP_ADD);
                }

                bFirstStageFilled = true;
            }

            // Add shadow maps
            while (uiRemainingProjShadows != 0 && 
                m_uiRemainingStages > 0 && m_uiRemainingTextures > 0)
            {
                NiD3DTextureStage* pkShadow = AddTextureStage(
                    GetProjectedShadow(
                    uiProjShadows - uiRemainingProjShadows));
                if (bFirstStageFilled)
                    pkShadow->SetStageState(D3DTSS_COLOROP, D3DTOP_MODULATE);
                else
                    bFirstStageFilled = true;

                uiRemainingProjShadows--;
                uiNumShadowTextures--;
            }

            if (uiNumShadowTextures != 0)
            {
                ClosePass(pkBaseMap);
            }
        }
    }

    // Consider env/gloss/bump maps
    if (bEnvMap)
    {
        // Disable bump map if not supported.
        if (m_eEnvBumpOp == D3DTOP_DISABLE)
            uiNumBumpMaps = 0;

        // We only apply one env map, anyway...
        unsigned int uiActiveEnvMaps = (uiEnvironmentMaps > 0 ? 1 : 0);

        // Calculate number of stages necessary
        unsigned int uiNumEnvStages = 
            uiActiveEnvMaps + uiNumBumpMaps + uiNumGlossMaps;

        // Be sure to account for the alpha texture stage, if necessary!
        unsigned int uiNumTotalEnvStages = (bAlphaRequiresTexture ? 1 : 0) +
            uiNumEnvStages;

        // Disable gloss maps, then bump maps if there not enough 
        // stages/textures available to fit in one pass
        while (uiNumTotalEnvStages > NiD3DPass::ms_uiMaxTextureBlendStages &&
            uiNumTotalEnvStages > NiD3DPass::ms_uiMaxSimultaneousTextures)
        {
            if (uiNumGlossMaps > 0)
                uiNumGlossMaps--;
            else if (uiNumBumpMaps > 0)
                uiNumBumpMaps--;
            else 
                NIASSERT(false);// GB requires 2-stage hardware at least!

            uiNumTotalEnvStages--;
            uiNumEnvStages--;
        }

        if (uiNumGlossMaps != 0 ||
            !IsCurrentCombineMode(D3DBLEND_ONE, D3DBLEND_ONE) ||
            m_uiRemainingStages < uiNumEnvStages || 
            m_uiRemainingTextures < uiNumEnvStages)
        {
            ClosePass(pkBaseMap);
        }

        bool bFirstStageFilled = true;

        if (m_spCurrentPass == NULL)
        {
            D3DBLEND eD3DSrc = D3DBLEND_ONE;
            if (bAlphaBlending)
            {
                NiAlphaProperty::AlphaFunction eSrc = 
                    pkAlphaProp->GetSrcBlendMode();
                eD3DSrc = (D3DBLEND)m_pkD3DRenderState->GetD3DBlendMode(eSrc);
            }
            OpenNewPass(eD3DSrc, D3DBLEND_ONE,
                bAlphaRequiresTexture, bAlphaRequiresVertexColor);
            bFirstStageFilled = false;
        }

        // Add bump map if there's room for it and the env map
        if (uiNumBumpMaps > 0 && 
            m_uiRemainingStages > 1 && m_uiRemainingTextures > 1)
        {
            unsigned short usTextureSet = pkBumpMap->GetTextureIndex();
            if (usTextureSet > m_uiMaxTextureIndex)
                usTextureSet = m_uiMaxTextureIndex;
            NiD3DTextureStage* pkBump = AddTextureStage(pkBumpMap, 
                usTextureSet);
            // Don't set bFirstStageFilled - env map stage will handle this

            uiNumBumpMaps--;
        }

        // Add env map
        NiD3DTextureStage* pkEnv = AddTextureStage(GetEnvironmentMap(0));
        if (bFirstStageFilled)
        {
            pkEnv->SetStageState(D3DTSS_COLOROP, D3DTOP_ADD);
        }
        else
        {
            bFirstStageFilled = true;
        }

        // Add gloss map
        // NOTE - having the environment map and gloss map together may cause 
        // the base texture to not fit into this pass for the purposes of 
        // providing the alpha channel.  It was decided that the gloss map 
        // provides more visual feedback in this situation than the alpha from
        // the base map.  To ensure that the base alpha is used in this case
        // rather than the gloss map, use the following if statement instead
        // of the current one:
        // if (uiNumGlossMaps > 0 && 
        //     m_uiRemainingStages > 0 && m_uiRemainingTextures > 0)
        if (uiNumGlossMaps > 0)
        {
            unsigned short usTextureSet = pkGlossMap->GetTextureIndex();
            if (usTextureSet > m_uiMaxTextureIndex)
                usTextureSet = m_uiMaxTextureIndex;
            NiD3DTextureStage* pkGloss = AddTextureStage(pkGlossMap, 
                usTextureSet);
            pkGloss->SetStageState(D3DTSS_COLOROP, D3DTOP_MODULATE);
#if defined(D3D_NI42_TEXTURE_PIPELINE)
            pkGloss->SetStageState(D3DTSS_ALPHAOP, D3DTOP_MODULATE);
#endif //#if defined(D3D_NI42_TEXTURE_PIPELINE)

            uiNumGlossMaps--;
        }
    }

    // Consider glow maps
    if (uiNumGlowMaps != 0)
    {
        if (!IsCurrentCombineMode(D3DBLEND_ONE, D3DBLEND_ONE) ||
            m_uiRemainingStages == 0 || m_uiRemainingTextures == 0)
        {
            ClosePass(pkBaseMap);
        }

        D3DTEXTUREOP eCombine = D3DTOP_ADD;

        if (m_spCurrentPass == NULL)
        {
            D3DBLEND eD3DSrc = D3DBLEND_ONE;
            if (bAlphaBlending)
            {
                NiAlphaProperty::AlphaFunction eSrc = 
                    pkAlphaProp->GetSrcBlendMode();
                eD3DSrc = (D3DBLEND)m_pkD3DRenderState->GetD3DBlendMode(eSrc);
            }
            OpenNewPass(eD3DSrc, D3DBLEND_ONE,
                bAlphaRequiresTexture, bAlphaRequiresVertexColor);
            eCombine = D3DTOP_SELECTARG1;
        }

        // Add glow map
        unsigned short usTextureSet = pkGlowMap->GetTextureIndex();
        if (usTextureSet > m_uiMaxTextureIndex)
            usTextureSet = m_uiMaxTextureIndex;
        NiD3DTextureStage* pkGlow = AddTextureStage(pkGlowMap, usTextureSet);
        pkGlow->SetStageState(D3DTSS_COLOROP, eCombine);

        uiNumGlowMaps--;
    }

    // Consider fog maps
    if (uiFogMaps != 0)
    {
        unsigned int uiRemainingFogMaps = uiFogMaps;
        unsigned int uiFogIndex = 0;

        if (m_bBlendTextureAlpha && m_uiPassCount == 0) 
        {
            while (uiRemainingFogMaps != 0 &&
                m_uiRemainingStages > 0 && m_uiRemainingTextures > 0)
            {
                const NiTextureEffect* pkFogMap = GetFogMap(uiFogIndex++);

                NiD3DTextureStage* pkFog = AddTextureStage(pkFogMap);
                pkFog->SetStageState(D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA);

                uiRemainingFogMaps--;
            }

            // Check to see if the fog maps are done
            if (uiRemainingFogMaps != 0)
                ClosePass();
        }
        else
        {
            // May need to start new pass for fog maps
            if (!IsCurrentCombineMode(D3DBLEND_SRCALPHA, 
                D3DBLEND_INVSRCALPHA))
            {
                ClosePass();
            }
        }

        // Remaining fog maps
        while (uiRemainingFogMaps != 0)
        {
            if (m_uiRemainingStages == 0 || m_uiRemainingTextures == 0)
                ClosePass();

            // Each fog gets its own pass
            const NiTextureEffect* pkFogMap = GetFogMap(uiFogIndex++);

            if (m_spCurrentPass == NULL)
            {
                OpenNewPass(D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA,
                    bAlphaRequiresTexture, bAlphaRequiresVertexColor);
            }

            NiD3DTextureStage* pkFog = AddTextureStage(pkFogMap);
            pkFog->SetStageState(D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
            uiRemainingFogMaps--;

            ClosePass();
        }
    }

    // Close current pass
    ClosePass(pkBaseMap);

    // Consider fogging properties
    PostProcessFogProperties(pkState->GetFog(), pkAlphaProp, pkBaseMap, 
        eApply);

    return 0;
}
//---------------------------------------------------------------------------
void NiD3DDefaultShader::OpenNewPass(D3DBLEND eSrcBlend, D3DBLEND eDestBlend,
    bool bReturnBaseTextureAlpha, bool bReturnVertexColorAlpha)
{
    if (m_spCurrentPass != NULL)
        return;

    m_spCurrentPass = NiD3DPass::CreateNewPass();
    m_kPasses.SetAtGrow(m_uiPassCount, m_spCurrentPass);

    if (m_uiPassCount != 0)
    {
        if (eSrcBlend == D3DBLEND_ONE && eDestBlend == D3DBLEND_ZERO)
        {
            m_spCurrentPass->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE, 
                true);
        }
        else
        {
            m_spCurrentPass->SetRenderState(D3DRS_SRCBLEND, eSrcBlend);
            m_spCurrentPass->SetRenderState(D3DRS_DESTBLEND, eDestBlend);
            m_spCurrentPass->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE, 
                true);
        }
    }

    m_bBaseTextureAlphaNeeded = bReturnBaseTextureAlpha;
    m_bVertexColorAlphaNeeded = bReturnVertexColorAlpha;
    m_uiRemainingStages = NiD3DPass::ms_uiMaxTextureBlendStages;
    m_uiRemainingTextures = NiD3DPass::ms_uiMaxSimultaneousTextures;

    if (m_bBaseTextureAlphaNeeded)
    {
        m_uiRemainingStages--;
        m_uiRemainingTextures--;
    }
}
//---------------------------------------------------------------------------
void NiD3DDefaultShader::ClosePass(const NiTexturingProperty::Map* pkBaseMap)
{
    if (m_spCurrentPass == NULL)
        return;

    if (m_bBaseTextureAlphaNeeded && pkBaseMap != NULL)
    {
        unsigned short usTextureSet = pkBaseMap->GetTextureIndex();
        if (usTextureSet > m_uiMaxTextureIndex)
            usTextureSet = m_uiMaxTextureIndex;
        NiD3DTextureStage* pkAlpha = AddTextureStage(pkBaseMap, 
            usTextureSet);

        if (pkAlpha)
        {
            pkAlpha->SetStageState(D3DTSS_COLOROP, D3DTOP_SELECTARG2);
            if (m_bBaseTextureAlphaNeeded)
            {
                pkAlpha->SetStageState(D3DTSS_ALPHAOP, D3DTOP_MODULATE);
                pkAlpha->SetStageState(D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
            }
            else
            {
                pkAlpha->SetStageState(D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
            }
        }
    }

    m_spCurrentPass = NULL;

    m_uiRemainingStages = 0;
    m_uiRemainingTextures = 0;

    m_uiPassCount++;
}
//---------------------------------------------------------------------------
NiD3DTextureStage* NiD3DDefaultShader::AddTextureStage(
    const NiTexturingProperty::Map* pkMap, unsigned int uiIndex)
{
    // Check for room in pass
    if (m_spCurrentPass == NULL ||
        m_spCurrentPass->CheckFreeStages(1, (pkMap == NULL ? 0 : 1)) == false)
    {
        return NULL;
    }

    NiD3DTextureStagePtr spStage = NiD3DTextureStage::CreateNewStage();
    NiD3DTextureStage* pkStage = spStage;
    pkStage->SetStageState(D3DTSS_RESULTARG, D3DTA_CURRENT);

    if (pkMap == NULL || pkMap->GetTexture() == NULL)
    {
        // Use diffuse color for color and alpha
        pkStage->SetStageState(D3DTSS_COLOROP, D3DTOP_SELECTARG1);
        pkStage->SetStageState(D3DTSS_COLORARG1, D3DTA_DIFFUSE);
        pkStage->SetStageState(D3DTSS_COLORARG2, D3DTA_CURRENT);
        if (m_bVertexColorAlphaNeeded)
        {
            pkStage->SetStageState(D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
            m_bVertexColorAlphaNeeded = false;
        }
        else
        {
            pkStage->SetStageState(D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
        }
        pkStage->SetStageState(D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
        pkStage->SetStageState(D3DTSS_ALPHAARG2, D3DTA_CURRENT);

        // Always record a texture use because textures cannot be accessed by
        // stages whose number is greater than the max number of textures.
        // (Not necessary if we're already past that value.)
        if (m_uiRemainingTextures != 0)
            m_uiRemainingTextures--;
    }
    else
    {
        NiTexture* pkTex = pkMap->GetTexture();
        NIASSERT(pkTex);
        pkStage->SetTexture(pkTex);

        pkStage->SetStageState(D3DTSS_TEXCOORDINDEX, uiIndex);

        // Color from texture, alpha from current
        pkStage->SetStageState(D3DTSS_COLOROP, D3DTOP_SELECTARG1);
        pkStage->SetStageState(D3DTSS_COLORARG1, D3DTA_TEXTURE);
        pkStage->SetStageState(D3DTSS_COLORARG2, D3DTA_CURRENT);
        pkStage->SetStageState(D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
        pkStage->SetStageState(D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
        if (m_bVertexColorAlphaNeeded)
        {
            pkStage->SetStageState(D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
            m_bVertexColorAlphaNeeded = false;
        }
        else
        {
            pkStage->SetStageState(D3DTSS_ALPHAARG2, D3DTA_CURRENT);
        }

        pkStage->SetFilterMode(pkMap->GetFilterMode());
        pkStage->SetClampMode(pkMap->GetClampMode());

        NiTextureTransform* pkTexTransform = 
            ((NiTexturingProperty::Map*)pkMap)->GetTextureTransform();
        // Check pointer, not value
        if (pkTexTransform == NULL)
        {
            pkStage->SetStageState(D3DTSS_TEXTURETRANSFORMFLAGS, 
                D3DTTFF_DISABLE);
        }
        else
        {
            PackTextureTransform(*pkTexTransform->GetMatrix(), pkStage);
        }
        m_uiRemainingTextures--;
    }

    m_uiRemainingStages--;

    m_spCurrentPass->AppendStage(pkStage);

    return pkStage;
}
//---------------------------------------------------------------------------
NiD3DTextureStage* NiD3DDefaultShader::AddTextureStage(
    const NiTexturingProperty::BumpMap* pkMap, unsigned int uiIndex)
{
    // Check for room in pass
    if (m_spCurrentPass == NULL || 
        m_spCurrentPass->CheckFreeStages() == false)
    {
        return NULL;
    }

    NiD3DTextureStagePtr spStage = NiD3DTextureStage::CreateNewStage();
    NiD3DTextureStage* pkStage = spStage;
    pkStage->SetStage(m_spCurrentPass->GetCurrentStage());

    NiTexture* pkTex = pkMap->GetTexture();
    pkStage->SetTexture(pkTex);

    pkStage->SetStageState(D3DTSS_TEXCOORDINDEX, uiIndex);

    // Color from bump, alpha from current
    pkStage->SetStageState(D3DTSS_COLOROP, m_eEnvBumpOp);
    pkStage->SetStageState(D3DTSS_COLORARG1, D3DTA_TEXTURE);
    pkStage->SetStageState(D3DTSS_COLORARG2, D3DTA_CURRENT);

    pkStage->SetStageState(D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
    pkStage->SetStageState(D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

    if (m_bVertexColorAlphaNeeded)
    {
        pkStage->SetStageState(D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
        m_bVertexColorAlphaNeeded = false;
    }
    else
    {
        pkStage->SetStageState(D3DTSS_ALPHAARG2, D3DTA_CURRENT);
    }

    if (pkTex)
    {
        pkStage->SetFilterMode(pkMap->GetFilterMode());
        pkStage->SetClampMode(pkMap->GetClampMode());

        NiTextureTransform* pkTexTransform = 
            ((NiTexturingProperty::Map*)pkMap)->GetTextureTransform();
        // Check pointer, not value
        if (pkTexTransform == NULL)
        {
            pkStage->SetStageState(D3DTSS_TEXTURETRANSFORMFLAGS, 
                D3DTTFF_DISABLE);
        }
        else
        {
            PackTextureTransform(*pkTexTransform->GetMatrix(), pkStage);
        }

        pkStage->SetStageState(D3DTSS_BUMPENVMAT00, 
            F2DW(pkMap->GetBumpMat00()));
        pkStage->SetStageState(D3DTSS_BUMPENVMAT01, 
            F2DW(pkMap->GetBumpMat01()));
        pkStage->SetStageState(D3DTSS_BUMPENVMAT10, 
            F2DW(pkMap->GetBumpMat10()));
        pkStage->SetStageState(D3DTSS_BUMPENVMAT11, 
            F2DW(pkMap->GetBumpMat11()));
        pkStage->SetStageState(D3DTSS_BUMPENVLSCALE, 
            F2DW(pkMap->GetLumaScale()));
        pkStage->SetStageState(D3DTSS_BUMPENVLOFFSET, 
            F2DW(pkMap->GetLumaOffset()));
    }

    m_uiRemainingStages--;
    m_uiRemainingTextures--;

    m_spCurrentPass->AppendStage(pkStage);

    return pkStage;
}
//---------------------------------------------------------------------------
NiD3DTextureStage* NiD3DDefaultShader::AddTextureStage(
    const NiTextureEffect* pkEffect)
{
    // Check for room in pass
    if (m_spCurrentPass == NULL || 
        m_spCurrentPass->CheckFreeStages() == false)
    {
        return NULL;
    }

    NiD3DTextureStagePtr spStage = NiD3DTextureStage::CreateNewStage();
    NiD3DTextureStage* pkStage = spStage;
    pkStage->SetStage(m_spCurrentPass->GetCurrentStage());

    NiTexture* pkTex = pkEffect->GetEffectTexture();
    pkStage->SetTexture(pkTex);
    pkStage->SetStageState(D3DTSS_TEXCOORDINDEX, 1);

    // Color from texture, alpha from current
    pkStage->SetStageState(D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    pkStage->SetStageState(D3DTSS_COLORARG1, D3DTA_TEXTURE);
    pkStage->SetStageState(D3DTSS_COLORARG2, D3DTA_CURRENT);
    pkStage->SetStageState(D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
    pkStage->SetStageState(D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

    if (m_bVertexColorAlphaNeeded)
    {
        pkStage->SetStageState(D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
        m_bVertexColorAlphaNeeded = false;
    }
    else
    {
        pkStage->SetStageState(D3DTSS_ALPHAARG2, D3DTA_CURRENT);
    }

    // we never touch the bottom row (as viewed in NiMatrix3 notation), as
    // we never generate more than 3 output coords.  As a result, we leave 
    // the bottom row at its originally initialized 0 0 0 1.
    PackDynamicEffect(pkEffect, pkStage, false, m_pkD3DRenderer);

    pkStage->SetFilterMode(pkEffect->GetTextureFilter());
    pkStage->SetClampMode(pkEffect->GetTextureClamp());

    m_uiRemainingStages--;
    m_uiRemainingTextures--;

    m_spCurrentPass->AppendStage(pkStage);

    return pkStage;
}
//---------------------------------------------------------------------------
NiD3DTextureStage* NiD3DDefaultShader::AddClipperStage(
    const NiTextureEffect* pkEffect)
{
    // Check for room in pass
    if (m_spCurrentPass == NULL || 
        m_spCurrentPass->CheckFreeStages() == false)
    {
        return NULL;
    }

    NiD3DTextureStagePtr spStage = NiD3DTextureStage::CreateNewStage();
    NiD3DTextureStage* pkStage = spStage;
    pkStage->SetStage(m_spCurrentPass->GetCurrentStage());

    pkStage->SetTexture(m_pkD3DRenderer->GetClipperTexture());
    pkStage->SetStageState(D3DTSS_TEXCOORDINDEX, 
        D3DTSS_TCI_CAMERASPACEPOSITION);
    pkStage->SetStageState(D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

    // Color from texture * current, alpha from current
    pkStage->SetStageState(D3DTSS_COLOROP, D3DTOP_MODULATE);
    pkStage->SetStageState(D3DTSS_COLORARG1, D3DTA_TEXTURE);
    pkStage->SetStageState(D3DTSS_COLORARG2, D3DTA_CURRENT);
    pkStage->SetStageState(D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
    pkStage->SetStageState(D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    if (m_bVertexColorAlphaNeeded)
    {
        pkStage->SetStageState(D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
        m_bVertexColorAlphaNeeded = false;
    }
    else
    {
        pkStage->SetStageState(D3DTSS_ALPHAARG2, D3DTA_CURRENT);
    }

    pkStage->SetFilterMode(NiTexturingProperty::FILTER_NEAREST);
    pkStage->SetClampMode(NiTexturingProperty::CLAMP_S_CLAMP_T);

    // This function does not check for values in range, as it is assumed
    // that the calling function will not call this unless a projected,
    // clipped light object could be allocated.  Since there is a 1-1
    // relationship between light clippers and clipped light objects, the
    // availability of one should imply the other.

    const NiPlane& kPlane = pkEffect->GetWorldClippingPlane();

    float fX = kPlane.GetNormal().x;
    float fY = kPlane.GetNormal().y;
    float fZ = kPlane.GetNormal().z;
    float fC = kPlane.GetConstant();

    if (pkEffect->GetTextureType() == NiTextureEffect::PROJECTED_SHADOW)
    {
        fX = -kPlane.GetNormal().x;
        fY = -kPlane.GetNormal().y;
        fZ = -kPlane.GetNormal().z;
        fC = -kPlane.GetConstant();
    }
    else
    {
        NIASSERT(pkEffect->GetTextureType() == 
            NiTextureEffect::PROJECTED_LIGHT);
        fX = kPlane.GetNormal().x;
        fY = kPlane.GetNormal().y;
        fZ = kPlane.GetNormal().z;
        fC = kPlane.GetConstant();
    }

    // we never touch the bottom 3 rows (as viewed in NiMatrix3 notation), 
    // as we never generate more than 1 nonzero output coords for this
    // effect.  As a result, we leave the bottom 3 rows at their originally
    // initialized 0 0 0 0.

    D3DMATRIX& kMat = pkStage->GetTextureTransformation();

    // cam matrix = kWorldMat * kInvMat
    // D3DMatrices are transposed with respect to NiMatrix3.
    const D3DMATRIX& kInvMat = m_pkD3DRenderer->GetInvView();

    kMat._11 = fX * kInvMat._11 +  
        fY * kInvMat._12 + 
        fZ * kInvMat._13;
    kMat._21 = fX * kInvMat._21 +  
        fY * kInvMat._22 + 
        fZ * kInvMat._23;
    kMat._31 = fX * kInvMat._31 +  
        fY * kInvMat._32 + 
        fZ * kInvMat._33;
    kMat._41 = 0.5f + fX * kInvMat._41 + 
        fY * kInvMat._42 + 
        fZ * kInvMat._43 - fC;

    m_uiRemainingStages--;
    m_uiRemainingTextures--;

    m_spCurrentPass->AppendStage(pkStage);

    return pkStage;
}
//---------------------------------------------------------------------------
bool NiD3DDefaultShader::IsCurrentCombineMode(D3DBLEND eSrcBlend, 
    D3DBLEND eDestBlend)
{
    // First pass or first stage of new pass do not have a combine modes
    if (m_uiPassCount == 0 || m_spCurrentPass == NULL)
        return true;

    bool bSave;

    unsigned int uiSrcBlend;
    m_spCurrentPass->GetRenderState(D3DRS_SRCBLEND, uiSrcBlend, bSave);
    if ((D3DBLEND)uiSrcBlend != eSrcBlend)
        return false;

    unsigned int uiDestBlend;
    m_spCurrentPass->GetRenderState(D3DRS_DESTBLEND, uiDestBlend, bSave);
    if ((D3DBLEND)uiDestBlend != eDestBlend)
        return false;
        
    return true;
}
//---------------------------------------------------------------------------
void NiD3DDefaultShader::NextFrame()
{
    ClearProjectedTextures();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Projected texture effects
//---------------------------------------------------------------------------
void NiD3DDefaultShader::UpdateProjectedTextures(
    const NiDynamicEffectState* pkState)
{
    if (pkState == m_pkLastState)
        return;

    ClearProjectedTextures();

    m_pkLastState = pkState;

    if (!pkState)
        return;

    // Projected lights
    NiDynEffectStateIter pkIter = pkState->GetProjLightHeadPos();
    while (pkIter)
    {
        NiTextureEffect* pkEffect = pkState->GetNextProjLight(pkIter);
        
        SaveProjectedTexture(pkEffect, pkEffect->GetClippingPlaneEnable());
    }

    // Projected shadows
    pkIter = pkState->GetProjShadowHeadPos();
    while (pkIter)
    {
        NiTextureEffect* pkEffect = pkState->GetNextProjShadow(pkIter);

        SaveProjectedTexture(pkEffect, pkEffect->GetClippingPlaneEnable());
    }

    NiTextureEffect* pkEffect;

    // Environment map
    if (pkEffect = pkState->GetEnvironmentMap())
        SaveProjectedTexture(pkEffect);

    // Fog Map
    if (pkEffect = pkState->GetFogMap())
        SaveProjectedTexture(pkEffect);
}
//---------------------------------------------------------------------------
void NiD3DDefaultShader::ClearProjectedTextures()
{
    m_kClippedProjectedLights.RemoveAll();
    m_kProjectedLights.RemoveAll();
    m_kClippedProjectedShadows.RemoveAll();
    m_kProjectedShadows.RemoveAll();
    m_kEnvironmentMaps.RemoveAll();
    m_kFogMaps.RemoveAll();

    m_pkLastState = NULL;
}
//---------------------------------------------------------------------------
bool NiD3DDefaultShader::SaveProjectedTexture(
    const NiTextureEffect* pkEffect, bool bClipped)
{
    if (!pkEffect->GetSwitch())
        return false;

    NiTexture* pkIm = (NiTexture*)(pkEffect->GetEffectTexture());
    if (!pkIm)
        return false;

    NIASSERT(m_pkD3DRenderer->GetTextureManager());
    unsigned int uiFmt = 
        m_pkD3DRenderer->GetTextureManager()->GetFormatFromTexture(*pkIm);
    if (uiFmt == NiD3DRenderer::TEX_NUM)
        return false;

    switch(pkEffect->GetTextureType())
    {
    case NiTextureEffect::PROJECTED_LIGHT:
        if (bClipped)
        {
            m_kClippedProjectedLights.Add(pkEffect);
        }
        else
        {
            m_kProjectedLights.Add(pkEffect);
        }
        break;
    case NiTextureEffect::PROJECTED_SHADOW:
        if (bClipped)
        {
            m_kClippedProjectedShadows.Add(pkEffect);
        }
        else
        {
            m_kProjectedShadows.Add(pkEffect);
        }
        break;
    case NiTextureEffect::ENVIRONMENT_MAP:
        NIASSERT(!bClipped);
        m_kEnvironmentMaps.Add(pkEffect);
        break;
    case NiTextureEffect::FOG_MAP:
        NIASSERT(!bClipped);
        m_kFogMaps.Add(pkEffect);
        break;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiD3DDefaultShader::PackTextureTransform(const NiMatrix3& kTexMatrix, 
    NiD3DTextureStage* pkStage)
{
    D3DMATRIX& kMat = pkStage->GetTextureTransformation();

    pkStage->SetStageState(D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
    kMat._11 = kTexMatrix.GetEntry( 0,0 ); 
    kMat._21 = kTexMatrix.GetEntry( 0,1 ); 
    kMat._31 = kTexMatrix.GetEntry( 0,2 ); 
    kMat._41 = 0.0f; 
    kMat._12 = kTexMatrix.GetEntry( 1,0 ); 
    kMat._22 = kTexMatrix.GetEntry( 1,1 ); 
    kMat._32 = kTexMatrix.GetEntry( 1,2 ); 
    kMat._42 = 0.0f; 
    kMat._13 = kMat._23 = kMat._33 = kMat._43 = 0.0f; 
    kMat._14 = kMat._24 = kMat._34 = kMat._44 = 0.0f;
}
//---------------------------------------------------------------------------
const NiTextureEffect* NiD3DDefaultShader::GetClippedProjectedLight(
    unsigned int uiIndex) const
{
    return m_kClippedProjectedLights.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
const NiTextureEffect* NiD3DDefaultShader::GetProjectedLight(
    unsigned int uiIndex) const
{
    return m_kProjectedLights.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
const NiTextureEffect* NiD3DDefaultShader::GetClippedProjectedShadow(
    unsigned int uiIndex) const
{
    return m_kClippedProjectedShadows.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
const NiTextureEffect* NiD3DDefaultShader::GetProjectedShadow(
    unsigned int uiIndex) const
{
    return m_kProjectedShadows.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
const NiTextureEffect* NiD3DDefaultShader::GetEnvironmentMap(
    unsigned int uiIndex) const
{
    return m_kEnvironmentMaps.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
const NiTextureEffect* NiD3DDefaultShader::GetFogMap(unsigned int uiIndex)
    const
{
    return m_kFogMaps.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
unsigned int NiD3DDefaultShader::GetClippedProjectedLightCount() const
{
    return m_kClippedProjectedLights.GetSize();
}
//---------------------------------------------------------------------------
unsigned int NiD3DDefaultShader::GetProjectedLightCount() const
{
    return m_kProjectedLights.GetSize();
}
//---------------------------------------------------------------------------
unsigned int NiD3DDefaultShader::GetClippedProjectedShadowCount() const
{
    return m_kClippedProjectedShadows.GetSize();
}
//---------------------------------------------------------------------------
unsigned int NiD3DDefaultShader::GetProjectedShadowCount() const
{
    return m_kProjectedShadows.GetSize();
}
//---------------------------------------------------------------------------
unsigned int NiD3DDefaultShader::GetEnvironmentMapCount() const
{
    return m_kEnvironmentMaps.GetSize();
}
//---------------------------------------------------------------------------
unsigned int NiD3DDefaultShader::GetFogMapCount() const
{
    return m_kFogMaps.GetSize();
}
//---------------------------------------------------------------------------
void NiD3DDefaultShader::UseQuickDecals(bool bQuickDecals)
{
    m_bQuickDecals = bQuickDecals;
}
//---------------------------------------------------------------------------
void NiD3DDefaultShader::PostProcessFogProperties(
    const NiFogProperty* pkFog, const NiAlphaProperty* pkAlpha, 
    const NiTexturingProperty::Map* pkBaseMap, 
    NiTexturingProperty::ApplyMode eApply)
{
    if (m_uiPassCount < 2 || pkFog->GetFog() == false)
        return;

    bool bAlphaBlending = pkAlpha->GetAlphaBlending();
    bool bAlphaTesting = pkAlpha->GetAlphaTesting();
    bool bAlphaRequiresTexture = false;
    bool bAlphaRequiresVertexColor = false;
    if (bAlphaBlending || bAlphaTesting)
    {
        bAlphaRequiresTexture = (pkBaseMap != NULL) &&
            (eApply != NiTexturingProperty::APPLY_DECAL);
        bAlphaRequiresVertexColor = 
            (eApply != NiTexturingProperty::APPLY_REPLACE);
    }

    if (m_uiFogPassArraySize < m_uiPassCount)
    {
        NiFree(m_peFogPassArray);
        m_peFogPassArray = NiAlloc(PassFogStatus, m_uiPassCount);
        m_uiFogPassArraySize = m_uiPassCount;
    }

    m_peFogPassArray[0] = BLACK_FOG;

    NiD3DPass* pkPrevPass = m_kPasses.GetAt(0);
    NiD3DPass* pkPass = NULL;
    unsigned int i = 1;
    for (; i < m_uiPassCount; i++)
    {
        pkPass = m_kPasses.GetAt(i);

        unsigned int uiSrcBlend;
        unsigned int uiDestBlend;
        bool bSave;

        bool bPresent = pkPass->GetRenderState(D3DRS_SRCBLEND, uiSrcBlend,
            bSave);
        NIASSERT(bPresent);
        bPresent = pkPass->GetRenderState(D3DRS_DESTBLEND, uiDestBlend,
            bSave);
        NIASSERT(bPresent);

        if (uiSrcBlend != D3DBLEND_SRCCOLOR &&
            uiSrcBlend != D3DBLEND_INVSRCCOLOR &&
            uiDestBlend != D3DBLEND_INVSRCCOLOR &&
            uiDestBlend != D3DBLEND_DESTCOLOR)
        {
            if (uiSrcBlend == D3DBLEND_ZERO ||
                uiSrcBlend == D3DBLEND_DESTCOLOR)
            {
                // Method 1
                m_peFogPassArray[i] = NO_FOG;

                pkPrevPass = pkPass;
                continue;
            }
            else if (uiDestBlend == D3DBLEND_ZERO ||
                uiDestBlend == D3DBLEND_SRCCOLOR)
            {
                // Method 2
                for (unsigned int uiIndex = 0; uiIndex < i; uiIndex++)
                    m_peFogPassArray[uiIndex] = NO_FOG;

                m_peFogPassArray[i] = BLACK_FOG;

                pkPrevPass = pkPass;
                continue;
            }
        }

        // if (uiSrcBlend == D3DBLEND_INVDESTCOLOR ||
        //     uiSrcBlend == D3DBLEND_SRCCOLOR ||
        //     uiSrcBlend == D3DBLEND_INVSRCCOLOR ||
        //     uiDestBlend == D3DBLEND_INVSRCCOLOR ||
        //     uiDestBlend == D3DBLEND_DESTCOLOR ||
        //     uiDestBlend == D3DBLEND_INVDESTCOLOR)
        // Too complex to handle correctly - use Method 3
        // Other cases can use this method correctly

        m_peFogPassArray[i] = BLACK_FOG;

        pkPrevPass = pkPass;
    }

#if !defined(_XENON)
    // Set fog states for each pass
    unsigned int uiFogColor = 
        m_pkD3DRenderState->GetRenderState(D3DRS_FOGCOLOR);
#endif  //#if !defined(_XENON)

    for (i = 0; i < m_uiPassCount; i++)
    {
        pkPass = m_kPasses.GetAt(i);
#if !defined(_XENON)
        if (m_peFogPassArray[i] == NO_FOG)
        {
            pkPass->SetRenderState(D3DRS_FOGENABLE, FALSE);
        }
        else // if (m_peFogPassArray[i] == BLACK_FOG)
        {
            pkPass->SetRenderState(D3DRS_FOGENABLE, TRUE);
            pkPass->SetRenderState(D3DRS_FOGCOLOR, 0xff000000);
        }
#endif  //#if !defined(_XENON)
    }

    // Add extra pass for adding the fog color
    NiAlphaProperty::AlphaFunction eSrc = pkAlpha->GetSrcBlendMode();
    D3DBLEND eD3DSrc = (D3DBLEND)m_pkD3DRenderState->GetD3DBlendMode(eSrc);
    OpenNewPass(eD3DSrc, D3DBLEND_ONE);

    unsigned int uiTexCoordIndex = 0;
    if (!bAlphaRequiresTexture)
    {
        pkBaseMap = NULL;
    }
    else if (pkBaseMap)
    {
        uiTexCoordIndex = pkBaseMap->GetTextureIndex();
        if (uiTexCoordIndex > m_uiMaxTextureIndex)
            uiTexCoordIndex = m_uiMaxTextureIndex;
    }

    NiD3DTextureStage* pkFogStage = AddTextureStage(pkBaseMap, 
        uiTexCoordIndex);
    NIASSERT(pkFogStage);

#if !defined(_XENON)
    // Color for this pass is black
    m_spCurrentPass->SetRenderState(D3DRS_TEXTUREFACTOR, 0xff000000);
    pkFogStage->SetStageState(D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    pkFogStage->SetStageState(D3DTSS_COLORARG1, D3DTA_TFACTOR);
#endif  //#if !defined(_XENON)

    // Alpha comes from texture and/or VC.
    if (bAlphaRequiresVertexColor)
    {
        if (bAlphaRequiresTexture)
        {
            pkFogStage->SetStageState(D3DTSS_ALPHAOP, D3DTOP_MODULATE);
            pkFogStage->SetStageState(D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
        }
        else
        {
            pkFogStage->SetStageState(D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
            pkFogStage->SetStageState(D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
        }
    }
    else
    {
        pkFogStage->SetStageState(D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    }

    // Set standard fogging on this new pass
#if !defined(_XENON)
    m_spCurrentPass->SetRenderState(D3DRS_FOGENABLE, TRUE);
    m_spCurrentPass->SetRenderState(D3DRS_FOGCOLOR, uiFogColor);
#endif  //#if !defined(_XENON)

    ClosePass();
}
//---------------------------------------------------------------------------
