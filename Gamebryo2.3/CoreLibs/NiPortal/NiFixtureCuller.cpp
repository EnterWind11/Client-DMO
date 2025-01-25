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

#include "NiPortalPCH.h"

#include "NiFixtureCuller.h"
#include "NiPortalMetrics.h"

//---------------------------------------------------------------------------
NiFixtureCuller::NiFixtureCuller()
    :
    NiCullingProcess(0, true) // the bool "true" signifies that we have
                              // declared our own AppendVirtual in this class
                              // and need it to be called by the base class
                              // Append function.
{
}
//---------------------------------------------------------------------------
NiFixtureCuller::~NiFixtureCuller()
{
}
//---------------------------------------------------------------------------
void NiFixtureCuller::Process(const NiCamera* pkCamera, NiAVObject* pkScene,
    NiVisibleArray* pkVisibleSet)
{
    NIMETRICS_PORTAL_SCOPETIMER(FIXTURECULLER_PROCESS);

    m_kFixtureMap.RemoveAll();
    NiCullingProcess::Process(pkCamera, pkScene, pkVisibleSet);
}
//---------------------------------------------------------------------------
void NiFixtureCuller::AppendVirtual(NiGeometry& kVisible)
{
    int iDummy;
    if (!m_kFixtureMap.GetAt(&kVisible, iDummy))
    {
        m_kFixtureMap.SetAt(&kVisible, 0);
        m_pkVisibleSet->Add(kVisible);
    }
}
//---------------------------------------------------------------------------
