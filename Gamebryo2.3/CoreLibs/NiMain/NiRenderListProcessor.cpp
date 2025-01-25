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
#include "NiMainPCH.h"

#include "NiRenderListProcessor.h"
#include "NiRenderer.h"
#include "NiGeometry.h"

NiImplementRootRTTI(NiRenderListProcessor);

//---------------------------------------------------------------------------
void NiRenderListProcessor::PreRenderProcessList(const NiVisibleArray* pkInput,
    NiVisibleArray& kOutput, void* pvExtraData)
{
    // If the input array pointer is null, do nothing.
    if (!pkInput)
    {
        return;
    }

    // Get renderer pointer.
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    NIASSERT(pkRenderer);

    // Render all input geometry objects immediately.
    const unsigned int uiInputCount = pkInput->GetCount();
    for (unsigned int ui = 0; ui < uiInputCount; ui++)
    {
        NiGeometry& kGeometry = pkInput->GetAt(ui);
        kGeometry.RenderImmediate(pkRenderer);
    }

    // Don't add any geometry objects to output array.
}
//---------------------------------------------------------------------------
void NiRenderListProcessor::PostRenderProcessList(
    NiVisibleArray& kPreviousOutput, void* pvExtraData)
{
    // Do nothing.
}
//---------------------------------------------------------------------------
