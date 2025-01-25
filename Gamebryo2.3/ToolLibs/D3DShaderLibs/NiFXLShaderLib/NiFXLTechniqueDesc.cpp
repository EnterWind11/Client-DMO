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
#include "NiFXLShaderLibPCH.h"

#include "NiFXLTechniqueDesc.h"
#include <NiRTLib.h>

NiFXLTechniqueDesc NiFXLTechniqueDesc::ms_kTechniqueDesc;

//---------------------------------------------------------------------------
NiFXLTechniqueDesc* NiFXLTechniqueDesc::GetDesc()
{
    memset(&ms_kTechniqueDesc, 0, sizeof(ms_kTechniqueDesc));
    return &ms_kTechniqueDesc;
}
//---------------------------------------------------------------------------
