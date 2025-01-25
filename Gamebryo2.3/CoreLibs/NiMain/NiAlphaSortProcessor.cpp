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

#include "NiAlphaSortProcessor.h"

NiImplementRTTI(NiAlphaSortProcessor, NiBackToFrontSortProcessor);

//---------------------------------------------------------------------------
void NiAlphaSortProcessor::PreRenderProcessList(const NiVisibleArray* pkInput,
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

    // Get camera data from renderer.
    NiPoint3 kWorldLoc, kWorldDir, kWorldUp, kWorldRight;
    NiFrustum kFrustum;
    NiRect<float> kViewport;
    pkRenderer->GetCameraData(kWorldLoc, kWorldDir, kWorldUp, kWorldRight,
        kFrustum, kViewport);

    const unsigned int uiInputCount = pkInput->GetCount();

    // Initialize size of object depths array.
    if (m_uiAllocatedDepths < uiInputCount)
    {
        NiFree(m_pfDepths);
        m_pfDepths = NiAlloc(float, uiInputCount);
        m_uiAllocatedDepths = uiInputCount;
    }

    // Iterate over input geometry array.
    unsigned int uiDepthIndex = 0;
    for (unsigned int ui = 0; ui < uiInputCount; ui++)
    {
        NiGeometry& kGeometry = pkInput->GetAt(ui);

        if (IsTransparent(kGeometry))
        {
            // Add geometry to output array.
            kOutput.Add(kGeometry);

            // Compute and store depth from camera.
            m_pfDepths[uiDepthIndex++] = ComputeDepth(kGeometry, kWorldDir);
        }
        else
        {
            // Render opaque geometry immediately.
            kGeometry.RenderImmediate(pkRenderer);
        }
    }

    // Sort output array by depth.
    SortObjectsByDepth(kOutput, 0, kOutput.GetCount() - 1);
}
//---------------------------------------------------------------------------
