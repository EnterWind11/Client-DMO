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
#include "NiCommonMaterialLibPCH.h"

#include "NiCommonMaterialLibrary.h"
#include "NiCommonMaterialSDM.h"

NiImplementSDMConstructor(NiCommonMaterial);

//---------------------------------------------------------------------------
void NiCommonMaterialSDM::Init()
{
    NiImplementSDMInitCheck();

    NiCommonMaterialLibrary::_SDMInit();
}
//---------------------------------------------------------------------------
void NiCommonMaterialSDM::Shutdown()
{
    NiImplementSDMShutdownCheck();

    NiCommonMaterialLibrary::_SDMShutdown();

}
//---------------------------------------------------------------------------
