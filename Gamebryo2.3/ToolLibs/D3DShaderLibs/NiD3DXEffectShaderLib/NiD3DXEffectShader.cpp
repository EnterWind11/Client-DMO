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
//---------------------------------------------------------------------------
// Precompiled Header
#include "NiD3DXEffectShaderLibPCH.h"

#include "NiD3DXEffectShader.h"
#include "NiD3DXEffectFactory.h"
#include "NiD3DXEffectFile.h"
#include "NiD3DXEffectParameter.h"
#include "NiD3DXEffectShaderSDM.h"
#include "NiD3DXEffectTechnique.h"
#include <NiD3DDefines.h>
#include <NiD3DRendererHeaders.h>
#include <NiD3DUtility.h>
#include <NiTimeSyncController.h>

#include <NiNode.h>

static NiD3DXEffectShaderSDM NiD3DXEffectShaderSDMObject;

NiImplementRTTI(NiD3DXEffectShader, NiD3DShader);

//---------------------------------------------------------------------------
NiD3DXEffectShader::NiD3DXEffectShader() :
    NiD3DShader(),
    m_pkD3DXEffectTechnique(NULL),
    m_pkD3DXEffect(NULL),
    m_pkTechnique(NULL),
    m_pkD3DXRenderableEffect(NULL),
    m_uiActualImplementation(0),
    m_uiBoneMatrixRegisters(0), 
    m_eBoneCalcMethod(BONECALC_SKIN),
    m_bSoftwareVP(false)
{
    for (unsigned int ui = 0; ui < m_kPasses.GetAllocatedSize(); ui++)
        m_kPasses.SetAt(ui, 0);
}
//---------------------------------------------------------------------------
NiD3DXEffectShader::~NiD3DXEffectShader()
{
    // No passes should have been set
}
//---------------------------------------------------------------------------
bool NiD3DXEffectShader::Initialize()
{
    if (!m_bInitialized)
    {
        if (!NiD3DShader::Initialize())
            return false;

        if (!m_kName.Exists() || m_kName.GetLength() == 0)
            return false;

        RecreateRendererData();
        if (m_pkD3DXRenderableEffect == NULL || m_pkTechnique == NULL)
            return false;
    }

    return m_bInitialized;
}
//---------------------------------------------------------------------------
unsigned int NiD3DXEffectShader::PreProcessPipeline(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, NiGeometryData::RendererData* pkRendererData,
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects,
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    unsigned int uiReturn = NiD3DShader::PreProcessPipeline(pkGeometry, 
        pkSkin, pkRendererData, pkState, pkEffects, kWorld, kWorldBound);

    // Make sure renderer data is up to date
    if (m_pkD3DXRenderableEffect == NULL || m_pkTechnique == NULL)
    {
        RecreateRendererData();
        if (m_pkD3DXEffect == NULL)
        {
            NiD3DRenderer::Error("NiD3DXEffectShader::PreProcessPipeline"
                " - %s - Effect failed to load\n", (const char*)m_kName);
        }
    }

    m_pkD3DXRenderableEffect->SetTechnique(m_pkTechnique);

    return uiReturn;
}
//---------------------------------------------------------------------------
unsigned int NiD3DXEffectShader::UpdatePipeline(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, NiGeometryData::RendererData* pkRendererData,
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects, 
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    // Do nothing; this is handled by the D3DXEffect object
    return 0;
}
//---------------------------------------------------------------------------
unsigned int NiD3DXEffectShader::SetupRenderingPass(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, NiGeometryData::RendererData* pkRendererData,
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects, 
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    // Do nothing; this is handled by the D3DXEffect object
    return 0;
}
//---------------------------------------------------------------------------
unsigned int NiD3DXEffectShader::SetupTransformations(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, 
    const NiSkinPartition::Partition* pkPartition, 
    NiGeometryData::RendererData* pkRendererData, 
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects, 
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    NiGeometryBufferData* pkBuffData = (NiGeometryBufferData*)pkRendererData;
    unsigned int uiReturn = 0;

    m_pkD3DRenderState->SetSoftwareVertexProcessing(m_bSoftwareVP);
    if (m_pkD3DXEffectTechnique->VertexShaderPresent(m_uiCurrentPass))
    {
        if (pkPartition)
        {
            if (m_uiCurrentPass == 0)
            {
                BoneMatrixCalcMethod eMethod = GetBoneCalcMethod();
                m_pkD3DRenderer->CalculateBoneMatrices(pkSkin, kWorld, 
                    true, m_uiBoneMatrixRegisters,
                    (eMethod == BONECALC_NO_SKIN));
            }

            m_pkD3DRenderState->SetBoneCount(
                pkPartition->m_usBonesPerVertex);
        }
        else
        {
            m_pkD3DRenderState->SetBoneCount(0);
        }

        // Force the calculation of the 'world' D3D matrix
        // but indicate it shouldn't be pushed to the device
        m_pkD3DRenderer->SetModelTransform(kWorld, false);
    }
    else
    {
        // Must set up FF pipeline with appropriate transforms
        uiReturn = NiD3DShader::SetupTransformations(pkGeometry, pkSkin, 
            pkPartition, pkBuffData, pkState, pkEffects, kWorld, kWorldBound);
    }

    // Set parameters and textures
    NiD3DXEffectParameter* pkParam = 
        m_pkD3DXEffectTechnique->GetFirstParameter();
    bool bVertexShaderPresent = 
        m_pkD3DXEffectTechnique->VertexShaderPresent(m_uiCurrentPass);
    while (pkParam)
    {
        NiD3DError eResult = pkParam->SetParameter(m_pkD3DXRenderableEffect, 
            pkGeometry, pkSkin, pkPartition, pkBuffData, pkState, 
            pkEffects, kWorld, kWorldBound, m_uiCurrentPass, 
            bVertexShaderPresent);

        if (eResult != NISHADERERR_OK)
        {
            switch (eResult)
            {
                case NISHADERERR_DYNEFFECTNOTFOUND:
                    if (NiShaderErrorSettings::GetAllowDynEffectNotFound())
                    {
                        NiD3DRenderer::Warning(
                            "NiD3DXEffectShader::SetupTransformations - %s - "
                            "Parameter %s on geometry with name \"%s\" "
                            "references a nonexistent NiDynamicEffect "
                            "object. Default values used.\n", 
                            (const char*)m_kName, 
                            (const char*)pkParam->GetName(), 
                            (const char*)pkGeometry->GetName());
                    }
                    break;
                default:
                    NiD3DRenderer::Error(
                        "NiD3DXEffectShader::SetupTransformations - %s - "
                        "Parameter %s failed to be set on geometry with "
                        "name \"%s\"\n", (const char*)m_kName,
                        (const char*)pkParam->GetName(), 
                        (const char*)pkGeometry->GetName());
                    break;
            }
        }

        pkParam = m_pkD3DXEffectTechnique->GetNextParameter();
    }

    return uiReturn;
}
//---------------------------------------------------------------------------
unsigned int NiD3DXEffectShader::SetupShaderPrograms(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, 
    const NiSkinPartition::Partition* pkPartition, 
    NiGeometryData::RendererData* pkRendererData, 
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects, 
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    NiGeometryBufferData* pkBuffData = (NiGeometryBufferData*)pkRendererData;

#if defined(WIN32)
    NiD3DXEffectStateManager* pkStateManager = 
        NiD3DXEffectStateManager::GetInstance(m_pkD3DRenderer);
    
    pkStateManager->ResetLastTextureStage();
#endif  // #if defined(WIN32)
            
    // Handle SDK difference between Summer 2003 and Summer 2004 updates
    // and between different Xenon XDK versions
#if (defined(_XENON) && (_XTL_VER >= 431)) || \
    (D3DX_VERSION >= 0x0902 && D3DX_SDK_VERSION >= 22)

    HRESULT hr = m_pkD3DXRenderableEffect->BeginPass(m_uiCurrentPass);
    if (FAILED(hr))
    {
        NiD3DRenderer::Error("NiD3DXEffectShader::SetupShaderPrograms"
            " - %s - ID3DXEffect::BeginPass failed\n", (const char*)m_kName);
    }

    hr = m_pkD3DXRenderableEffect->CommitChanges();
    if (FAILED(hr))
    {
        NiD3DRenderer::Error("NiD3DXEffectShader::SetupShaderPrograms"
            " - %s - ID3DXEffect::CommitChanges failed\n", 
            (const char*)m_kName);
    }
#else // #if (really long conditional dealing with XDK/DXSDK version...)
    HRESULT hr = m_pkD3DXRenderableEffect->Pass(m_uiCurrentPass);
    if (FAILED(hr))
    {
        NiD3DRenderer::Error("NiD3DXEffectShader::SetupShaderPrograms"
            " - %s - ID3DXEffect::Pass failed\n", m_pszName);
    }
#endif // #if (really long conditional dealing with XDK/DXSDK version...)

#if defined(WIN32)
    // Clear out textures from unused samplers
    for (int i = pkStateManager->GetLastTextureStage() + 1; 
        i < (int)NiD3DPass::ms_uiMaxSamplers; i++)
    {
        m_pkD3DRenderState->SetTexture(i, NULL);
    }
#endif  // #if defined(WIN32)

    if (m_uiCurrentPass == 0)
    {
        // Set the object's FVF in case it isn't handled by the D3DXEffect
        if (pkBuffData == NULL)
        {
            if (pkPartition)
            {
                pkBuffData = (NiGeometryBufferData*)pkPartition->m_pkBuffData;
            }
            else
            {
                pkBuffData = (NiGeometryBufferData*)
                    pkGeometry->GetModelData()->GetRendererData();
            }
            NIASSERT(pkBuffData);
        }

#if !defined(_XENON)
        if (pkBuffData->GetFVF())
        {
            m_pkD3DRenderState->SetFVF(pkBuffData->GetFVF());
        }
        else
        {
            m_pkD3DRenderState->SetDeclaration(
                pkBuffData->GetVertexDeclaration());
        }
#else
        // Xenon does not use FVFs. Every buffer data should have a vertex
        // declaration on it.
        NIASSERT(pkBuffData->GetVertexDeclaration());
        m_pkD3DRenderState->SetDeclaration(
            pkBuffData->GetVertexDeclaration());

#endif
    }

    return 0;
}
//---------------------------------------------------------------------------
unsigned int NiD3DXEffectShader::PostRender(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, 
    const NiSkinPartition::Partition* pkPartition, 
    NiGeometryData::RendererData* pkRendererData, 
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects, 
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    // Handle SDK difference between Summer 2003 and Summer 2004 updates
    // and between different Xenon XDK versions
#if (defined(_XENON) && (_XTL_VER >= 431)) || \
    (D3DX_VERSION >= 0x0902 && D3DX_SDK_VERSION >= 22)

    HRESULT hr = m_pkD3DXRenderableEffect->EndPass();
    if (FAILED(hr))
    {
        NiD3DRenderer::Error("NiD3DXEffectShader::PostRender"
            " - %s - ID3DXEffect::EndPass failed\n", (const char*)m_kName);
    }
#endif // #if (really long conditional dealing with XDK/DXSDK version...)

    return 0;
}
//---------------------------------------------------------------------------
unsigned int NiD3DXEffectShader::PostProcessPipeline(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, NiGeometryData::RendererData* pkRendererData,
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects,
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
#if defined(_XENON)
    // Release texture references if we're on Xenon. If the effect holds on
    // to them, we'll have issues at shutdown.
    NiD3DXEffectParameter* pkParam = 
        m_pkD3DXEffectTechnique->GetFirstParameter();
    while (pkParam)
    {
        pkParam->ClearTextureReferences(m_pkD3DXEffect);
        pkParam = m_pkD3DXEffectTechnique->GetNextParameter();
    }
#endif
    return NiD3DShader::PostProcessPipeline(pkGeometry, pkSkin, pkRendererData,
        pkState, pkEffects, kWorld, kWorldBound);
}
//---------------------------------------------------------------------------
unsigned int NiD3DXEffectShader::FirstPass()
{
    HRESULT hr = m_pkD3DXRenderableEffect->Begin(&m_uiPassCount, 
#if defined(WIN32)
        D3DXFX_DONOTSAVESTATE | D3DXFX_DONOTSAVESHADERSTATE
    #if (D3DX_VERSION >= 0x0902 && D3DX_SDK_VERSION >= 22)
        | D3DXFX_DONOTSAVESAMPLERSTATE
    #endif  // #if (D3DX_VERSION >= 0x0902 && D3DX_SDK_VERSION >= 22)
        );
#elif defined(_XENON)
        // D3DXFX_DONOTSAVESTATE | D3DXFX_DONOTSAVESHADERSTATE);
        0);
#endif

    if (FAILED(hr))
    {
        NiD3DRenderer::Error("NiD3DXEffectShader::FirstPass"
            " - %s - ID3DXEffect::Begin failed\n", (const char*)m_kName);
    }

    m_uiCurrentPass = 0;
    if (m_uiPassCount == 0)
        m_pkD3DXRenderableEffect->End();

    return m_uiPassCount;
}
//---------------------------------------------------------------------------
unsigned int NiD3DXEffectShader::NextPass()
{
    m_uiCurrentPass++;
    unsigned int uiRemainingPasses = m_uiPassCount - m_uiCurrentPass;

    if (uiRemainingPasses == 0)
        m_pkD3DXRenderableEffect->End();

    return uiRemainingPasses;
}
//---------------------------------------------------------------------------
bool NiD3DXEffectShader::SetupGeometry(NiGeometry* pkGeometry)
{
    if (!pkGeometry)
        return false;

    // Attach time parameter
    bool bTimeRequired = m_pkD3DXEffectTechnique->GetTimeParameterPresent();

    if (bTimeRequired)
    {
        NiTimeSyncController* pkTimeCtrlr = NiNew NiTimeSyncController();
        NIASSERT(pkTimeCtrlr);
        pkTimeCtrlr->SetTarget(pkGeometry);

        // Ensure object will be updated by UpdateSelected.
        NiAVObject* pkObject = pkGeometry;
        while (pkObject != NULL)
        {
            pkObject->SetSelectiveUpdate(true);
            pkObject = pkObject->GetParent();
        }
    }

    CreateShaderDeclaration();

    return NiD3DShader::SetupGeometry(pkGeometry);
}
//---------------------------------------------------------------------------
void NiD3DXEffectShader::HandleLostDevice()
{
    // Mark all effects as lost
    NiD3DXEffectFactory::GetInstance()->HandleLostDevice(
        m_pkD3DRenderer->GetResetCounter());
}
//---------------------------------------------------------------------------
void NiD3DXEffectShader::HandleResetDevice()
{
    // Mark all effects as reset
    NiD3DXEffectFactory::GetInstance()->HandleResetDevice(
        m_pkD3DRenderer->GetResetCounter());
}
//---------------------------------------------------------------------------
void NiD3DXEffectShader::DestroyRendererData()
{
    m_pkD3DXEffectTechnique->GetFile()->DestroyRendererData();
    
    if (m_pkD3DXEffect)
    {
        m_pkD3DXEffect->Release();
        m_pkD3DXEffect = NULL;
        if (m_pkD3DXRenderableEffect)
            m_pkD3DXRenderableEffect = NULL;
    }

    m_pkTechnique = NULL;

    m_spShaderDecl = 0;

    m_bInitialized = false;
}
//---------------------------------------------------------------------------
void NiD3DXEffectShader::RecreateRendererData()
{
    NiD3DXEffectFile* pkFile = m_pkD3DXEffectTechnique->GetFile();
    LPD3DXBASEEFFECT pkEffect = pkFile->GetEffect();
    if (pkEffect == NULL)
    {
        pkFile->LoadEffect(m_pkD3DRenderer);
        pkEffect = pkFile->GetEffect();
        if (pkEffect == NULL)
            return;
    }
    m_pkD3DXEffect = pkEffect;

#ifdef _XENON
    m_pkD3DXRenderableEffect = (LPD3DXEFFECT)m_pkD3DXEffect;
    m_pkD3DXRenderableEffect->AddRef();
#else
    bool bRenderable = 
        SUCCEEDED(m_pkD3DXEffect->QueryInterface(IID_ID3DXEffect, 
        (LPVOID*)&m_pkD3DXRenderableEffect));
    NIASSERT(bRenderable || m_pkD3DXRenderableEffect == NULL);
#endif

    NiD3DXEffectTechniqueSet* pkTechniqueSet = pkFile->GetTechniqueSet(
        (const char*)m_kName);
    NIASSERT(pkTechniqueSet);

    NiD3DXEffectTechnique* pkTechnique = pkTechniqueSet->GetTechnique(
        m_uiActualImplementation);

    NIASSERT(pkTechnique && pkTechnique == m_pkD3DXEffectTechnique);

    m_pkTechnique = pkTechnique->GetTechniquePtr();

    CreateShaderDeclaration();
}
//---------------------------------------------------------------------------
void NiD3DXEffectShader::CreateShaderDeclaration()
{
    if (m_spShaderDecl == NULL)
    {
        // Get pixel/vertex shader annotations
        D3DXTECHNIQUE_DESC kTechniqueDesc;
        m_pkD3DXEffect->GetTechniqueDesc(m_pkTechnique, &kTechniqueDesc);

        D3DXPASS_DESC* pkPassDescs = NiExternalNew 
            D3DXPASS_DESC[kTechniqueDesc.Passes];

        unsigned int i = 0;
        unsigned int uiTotalSemanticCount = 0;
        for (; i < kTechniqueDesc.Passes; i++)
        {
            D3DXHANDLE hPass = m_pkD3DXEffect->GetPass(m_pkTechnique, i);
            m_pkD3DXEffect->GetPassDesc(hPass, pkPassDescs + i);

            if (pkPassDescs[i].pVertexShaderFunction != NULL)
            {
                unsigned int uiSemanticCount = 0;
                D3DXGetShaderInputSemantics(
                    pkPassDescs[i].pVertexShaderFunction, NULL, 
                    &uiSemanticCount);

                uiTotalSemanticCount += uiSemanticCount;
            }
        }

        if (uiTotalSemanticCount == 0)
        {
            NiExternalDelete[] pkPassDescs;
            return;
        }

        unsigned int uiFilledSemanticCount = 0;
        D3DXSEMANTIC* pkSemantics = NiExternalNew 
            D3DXSEMANTIC[uiTotalSemanticCount];
        for (i = 0; i < kTechniqueDesc.Passes; i++)
        {
            if (pkPassDescs[i].pVertexShaderFunction != NULL)
            {
                unsigned int uiSemanticCount = 0;
                D3DXSEMANTIC* pkBase = pkSemantics + uiFilledSemanticCount;

                D3DXGetShaderInputSemantics(
                    pkPassDescs[i].pVertexShaderFunction, pkBase, 
                    &uiSemanticCount);

                if (i > 0)
                {
                    // Eliminate redundant semantics
                    D3DXSEMANTIC* pkNew = pkBase;
                    for (unsigned int j = 0; j < uiSemanticCount; j++)
                    {
                        D3DXSEMANTIC* pkExisting = pkSemantics;
                        while (pkExisting < pkBase)
                        {
                            if (pkExisting->Usage == pkNew->Usage &&
                                pkExisting->UsageIndex == pkNew->UsageIndex)
                            {
                                pkNew->Usage = 0xFFFFFFFF;
                                pkNew->UsageIndex = 0xFFFFFFFF;
                                uiTotalSemanticCount--;
                                break;
                            }

                            pkExisting++;
                        }
                        pkNew++;
                    }
                }

                uiFilledSemanticCount += uiSemanticCount;
            }
        }
        NIASSERT(uiFilledSemanticCount >= uiTotalSemanticCount);

        NiExternalDelete[] pkPassDescs;

        if (uiFilledSemanticCount == 0)
            return;

        // Assume only one stream
#if defined(WIN32)
        NiD3DShaderDeclaration* pkShaderDecl = 
            NiDX9ShaderDeclaration::Create(
#elif defined(_XENON)
        NiD3DShaderDeclaration* pkShaderDecl =
            NiXenonShaderDeclaration::Create(
#endif  // #if defined(WIN32)
                m_pkD3DRenderer, uiTotalSemanticCount, 1);

        unsigned int uiEntry = 0;
        for (i = 0; i < uiFilledSemanticCount; i++)
        {
            if (pkSemantics[i].Usage != 0xFFFFFFFF &&
                pkSemantics[i].UsageIndex != 0xFFFFFFFF)
            {
                NiD3DShaderDeclaration::ShaderParameter eInput;
                NiD3DShaderDeclaration::ShaderParameterType eType;
                NiD3DShaderDeclaration::ShaderParameterUsage eUsage;
                NiD3DShaderDeclaration::ShaderParameterTesselator eTess;

                bool bSuccess = 
#if defined(WIN32)
                    NiDX9ShaderDeclaration::GetShaderParameterValues(
#elif defined(_XENON)
                    NiXenonShaderDeclaration::GetShaderParameterValues(
#endif  // #if defined(WIN32)
                        (D3DDECLUSAGE)pkSemantics[i].Usage, 
                        pkSemantics[i].UsageIndex, eInput, eType, 
                        eUsage, eTess);

                if (bSuccess)
                {
                    pkShaderDecl->SetEntry(0, uiEntry, eInput, eType, 
                        NiShaderDeclaration::UsageToString(eUsage), 
                        pkSemantics[i].UsageIndex, eTess);
                    uiEntry++;
                }
            }
        }
        NiExternalDelete[] pkSemantics;

        // Check for software vertex processing
        pkShaderDecl->SetSoftwareVertexProcessing(m_bSoftwareVP);

        SetShaderDecl(pkShaderDecl);
    }
}
//---------------------------------------------------------------------------
bool NiD3DXEffectShader::GetVSPresentAllPasses() const
{
    if (m_pkD3DXEffectTechnique)
        return m_pkD3DXEffectTechnique->GetVSPresentAllPasses();
    else
        return false;
}
//---------------------------------------------------------------------------
bool NiD3DXEffectShader::GetVSPresentAnyPass() const
{
    if (m_pkD3DXEffectTechnique)
        return m_pkD3DXEffectTechnique->GetVSPresentAnyPass();
    else
        return false;
}
//---------------------------------------------------------------------------
bool NiD3DXEffectShader::GetPSPresentAllPasses() const
{
    if (m_pkD3DXEffectTechnique)
        return m_pkD3DXEffectTechnique->GetPSPresentAllPasses();
    else
        return false;
}
//---------------------------------------------------------------------------
bool NiD3DXEffectShader::GetPSPresentAnyPass() const
{
    if (m_pkD3DXEffectTechnique)
        return m_pkD3DXEffectTechnique->GetPSPresentAnyPass();
    else
        return false;
}
//---------------------------------------------------------------------------

