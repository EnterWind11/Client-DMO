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
#include "NiFloodgatePCH.h"
#include <NiMath.h>

#include "NiSPAnalyzer.h"
#include "NiSPTask.h"
#include "NiSPWorkflow.h"
#include "NiSPStream.h"
#include "NiSPAlgorithms.h"
//---------------------------------------------------------------------------
NiSPAnalyzer::NiSPAnalyzer() :
    m_usRecursionCount(0)
{
}
//---------------------------------------------------------------------------
NiSPAnalyzer::~NiSPAnalyzer()
{
}
//---------------------------------------------------------------------------
void NiSPAnalyzer::Analyze(NiSPWorkflow* pkWorkflow)
{
    // Reset recursion count
    m_usRecursionCount = 0;

    // Determine which stage each task should be in
    NiUInt32 uiTaskCount = pkWorkflow->GetSize();
    for (NiUInt32 uiIndex = 0; uiIndex < uiTaskCount; ++uiIndex)
    {
        NiSPTask* pTask = pkWorkflow->GetAt(uiIndex);
        if (pTask->IsCached())
            continue;
        Analyze(pTask);
        pTask->SetIsCached(true);
    }

    // Sort the tasks by stage
    NiSPAlgorithms::QuickSort<NiSPWorkflow, NiSPTask>(
        pkWorkflow, 0, pkWorkflow->GetSize() - 1);
}
//---------------------------------------------------------------------------
void NiSPAnalyzer::Analyze(NiSPTask* pkTask)
{
    // In debug builds, assert if we detect a potential infinite loop
    NIASSERT(m_usRecursionCount++ < MAX_RECURSIONS && 
        "Error: Possible Infinite Loop");

    // Don't process nodes that are already marked
    if (pkTask->IsMarked())
        return;
    
    // Don't process nodes that are already marked
    if (pkTask->IsSync())
    {
        pkTask->SetStage(MAX_STAGE);
        pkTask->SetIsMarked(true);
        return;
    }

    // End recursion at a root
    if (pkTask->IsRoot())
    {
        pkTask->SetStage(0);
        pkTask->SetIsMarked(true);
        return;
    }

    // Climb up the hierarchy until a root or marked node is reached.
    NiUInt32 uiInputs = pkTask->GetInputCount();
    for (NiUInt32 uiStreamIndex = 0; uiStreamIndex < uiInputs; ++uiStreamIndex)
    {
        NiSPStream* pStream = pkTask->GetInputAt(uiStreamIndex);
        NiUInt32 uiTasks = pStream->GetOutputSize();
        for (NiUInt32 uiTaskIndex = 0; uiTaskIndex < uiTasks; ++uiTaskIndex)
        {
            // recurse up through the tasks to a root task
            NiSPTask* pkParentTask = pStream->GetOutputAt(uiTaskIndex);
            Analyze(pkParentTask);

            // Ignore root tasks as they are set to stage 0
            if (!pkTask->IsRoot())
            {
                // Our stage = Max(our current stage, parent stage + 1);
                pkTask->SetStage(
                    NiMax(pkTask->GetStage(), pkParentTask->GetStage() + 1));
            }
        }
    }
                    
    // Mark node as processed
    pkTask->SetIsMarked(true);

}

