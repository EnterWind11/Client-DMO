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

#include "NiMainPCH.h"

#include "NiDepthStencilBuffer.h"
#include "NiRenderer.h"

NiImplementRTTI(NiDepthStencilBuffer, Ni2DBuffer);
//---------------------------------------------------------------------------
NiDepthStencilBuffer::NiDepthStencilBuffer()
{
}
//---------------------------------------------------------------------------
NiDepthStencilBuffer::~NiDepthStencilBuffer()
{
}
//---------------------------------------------------------------------------
NiDepthStencilBuffer* NiDepthStencilBuffer::Create(unsigned int uiWidth, 
    unsigned int uiHeight, NiRenderer* pkRenderer, 
    const NiPixelFormat& kFormat, Ni2DBuffer::MultiSamplePreference eMSAAPref)
{
    if (kFormat.GetFormat() != NiPixelFormat::FORMAT_DEPTH_STENCIL)
        return NULL;

    NiDepthStencilBuffer* pkBuffer = NiNew NiDepthStencilBuffer();
    pkBuffer->m_uiWidth = uiWidth;
    pkBuffer->m_uiHeight = uiHeight;
    
    if (!pkBuffer->CreateRendererData(kFormat,eMSAAPref))
    {
        NiDelete pkBuffer;
        return NULL;
    }
    return pkBuffer;
}
//---------------------------------------------------------------------------
NiDepthStencilBuffer* NiDepthStencilBuffer::CreateCompatible(
    Ni2DBuffer* pkBuffer, NiRenderer* pkRenderer)
{
    if (!pkBuffer || !pkRenderer)
        return NULL;

    const NiPixelFormat* pkFormat = 
        pkRenderer->FindClosestDepthStencilFormat(pkBuffer->GetPixelFormat());

    if (pkFormat)
    {
        NiDepthStencilBuffer* pkDSBuffer = Create(
            pkBuffer->GetWidth(), 
            pkBuffer->GetHeight(),
            pkRenderer, *pkFormat,
            pkBuffer->GetMSAAPref());
        return pkDSBuffer;
    }
    return NULL;
}
//---------------------------------------------------------------------------
NiDepthStencilBuffer* NiDepthStencilBuffer::Create(unsigned int uiWidth, 
    unsigned int uiHeight, Ni2DBuffer::RendererData* pkData)
{
    NiDepthStencilBuffer* pkBuffer = NiNew NiDepthStencilBuffer();
    pkBuffer->m_uiWidth = uiWidth;
    pkBuffer->m_uiHeight = uiHeight;
    pkBuffer->SetRendererData(pkData);
    return pkBuffer;
}
//---------------------------------------------------------------------------
bool NiDepthStencilBuffer::CreateRendererData(const NiPixelFormat& kFormat, 
    Ni2DBuffer::MultiSamplePreference eMSAAPref) 
{
    if (m_spRendererData)
        return true;

    NiRenderer* pkRenderer = NiRenderer::GetRenderer();

    if (pkRenderer && !pkRenderer->CreateDepthStencilRendererData(this, 
        &kFormat, eMSAAPref))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
