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
#include "NiEntityPCH.h"

#include "NiResetUIntDataCommand.h"

//---------------------------------------------------------------------------
NiResetUIntDataCommand::NiResetUIntDataCommand(
    NiEntityPropertyInterface* pkEntityPropertyInterface,
    const NiFixedString& kPropertyName) : NiChangePropertyCommand(
    pkEntityPropertyInterface, kPropertyName, 0), m_puiOldData(NULL)
{
    NiFixedString kPrimitiveType;
    NIVERIFY(m_spEntityPropertyInterface->GetPrimitiveType(
        m_kPropertyName, kPrimitiveType));
    NIASSERT(kPrimitiveType == NiEntityPropertyInterface::PT_UINT);
}
//---------------------------------------------------------------------------
NiResetUIntDataCommand::~NiResetUIntDataCommand()
{
    NiFree(m_puiOldData);
}
//---------------------------------------------------------------------------
// NiChangePropertyCommand overrides.
//---------------------------------------------------------------------------
NiBool NiResetUIntDataCommand::StoreOldData()
{
    NiBool bSuccess = m_spEntityPropertyInterface->GetElementCount(
        m_kPropertyName, m_uiOldDataCount);
    if (!bSuccess)
    {
        return false;
    }

    if (!m_puiOldData)
    {
        m_puiOldData = NiAlloc(unsigned int, m_uiOldDataCount);
    }

    for (unsigned int ui = 0; ui < m_uiOldDataCount; ui++)
    {
        if (!m_spEntityPropertyInterface->GetPropertyData(m_kPropertyName,
            m_puiOldData[ui], ui))
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiResetUIntDataCommand::SetNewData()
{
    return m_spEntityPropertyInterface->ResetProperty(m_kPropertyName);
}
//---------------------------------------------------------------------------
NiBool NiResetUIntDataCommand::SetOldData()
{
    for (unsigned int ui = 0; ui < m_uiOldDataCount; ui++)
    {
        if (!m_spEntityPropertyInterface->SetPropertyData(m_kPropertyName,
            m_puiOldData[ui], ui))
        {
            return false;
        }
    }
    
    return true;
}
//---------------------------------------------------------------------------
