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
#include "NiPhysXFluidPCH.h"

#include "NiPhysXMeshFluidData.h"

NiImplementRTTI(NiPhysXMeshFluidData, NiMeshPSysData);

//---------------------------------------------------------------------------
NiPhysXMeshFluidData::NiPhysXMeshFluidData()
{
    m_kFluidData.setToDefault();
}
//---------------------------------------------------------------------------
NiPhysXMeshFluidData::NiPhysXMeshFluidData(
    NxParticleData& kFluidData, unsigned short usMaxNumParticles, 
    bool bHasColors, bool bHasRotations, unsigned int uiPoolSize,
    bool bFillPoolsOnLoad) :
    NiMeshPSysData(usMaxNumParticles, bHasColors, bHasRotations, uiPoolSize,
    1, bFillPoolsOnLoad)
{
    NIASSERT(usMaxNumParticles >= kFluidData.maxParticles);

    m_kFluidData = kFluidData;
}
//---------------------------------------------------------------------------
NiPhysXMeshFluidData::NiPhysXMeshFluidData(NiMeshPSysData* pkPSysData) : 
    NiMeshPSysData(pkPSysData->GetMaxNumParticles(),
    pkPSysData->GetColors() != NULL, pkPSysData->GetRotationAngles() != NULL,
    pkPSysData->GetDefaultPoolsSize(), pkPSysData->GetNumGenerations(),
    pkPSysData->GetFillPoolsOnLoad())
{
    m_kFluidData.maxParticles = pkPSysData->GetMaxNumParticles();
    m_kFluidData.numParticlesPtr = NiAlloc(NxU32, 1);
    *(m_kFluidData.numParticlesPtr) = pkPSysData->GetNumParticles();
    m_kFluidData.bufferPosByteStride = sizeof(NxF32) * 3;
    m_kFluidData.bufferPos = NiAlloc(NxF32, m_kFluidData.maxParticles * 3);
}
//---------------------------------------------------------------------------
NiPhysXMeshFluidData::~NiPhysXMeshFluidData()
{
    NiFree(m_kFluidData.numParticlesPtr);
    NiFree(m_kFluidData.bufferPos);
    NiFree(m_kFluidData.bufferVel);
    NiFree(m_kFluidData.bufferLife);
    NiFree(m_kFluidData.bufferDensity);
}
//---------------------------------------------------------------------------
void NiPhysXMeshFluidData::SetFluidData(NxParticleData& kFluidData)
{
    NIASSERT(kFluidData.maxParticles <= m_usVertices);
    
    // Delete all old data
    NiFree(m_kFluidData.numParticlesPtr);
    NiFree(m_kFluidData.bufferPos);
    NiFree(m_kFluidData.bufferVel);
    NiFree(m_kFluidData.bufferLife);
    NiFree(m_kFluidData.bufferDensity);
   
    m_kFluidData = kFluidData;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning Functions
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXMeshFluidData);
//---------------------------------------------------------------------------
void NiPhysXMeshFluidData::CopyMembers(NiPhysXMeshFluidData* pkDest,
    NiCloningProcess& kCloning)
{
    NiMeshPSysData::CopyMembers(pkDest, kCloning);

    pkDest->m_kFluidData.maxParticles = m_kFluidData.maxParticles;
    if (m_kFluidData.numParticlesPtr)
    {
        pkDest->m_kFluidData.numParticlesPtr = NiAlloc(NxU32, 1);
        *(pkDest->m_kFluidData.numParticlesPtr) =
            *(m_kFluidData.numParticlesPtr);
    }
    if (m_kFluidData.bufferPos)
    {
        pkDest->m_kFluidData.bufferPos =
            NiAlloc(NxF32, m_kFluidData.maxParticles *
            m_kFluidData.bufferPosByteStride / sizeof(NxF32));
        pkDest->m_kFluidData.bufferPosByteStride =
            m_kFluidData.bufferPosByteStride;
        NiMemcpy(pkDest->m_kFluidData.bufferPos, m_kFluidData.bufferPos,
            m_kFluidData.maxParticles * m_kFluidData.bufferPosByteStride);
    }
    if (m_kFluidData.bufferVel)
    {
        pkDest->m_kFluidData.bufferVel =
            NiAlloc(NxF32, m_kFluidData.maxParticles *
            m_kFluidData.bufferVelByteStride / sizeof(NxF32));
        pkDest->m_kFluidData.bufferVelByteStride =
            m_kFluidData.bufferVelByteStride;
        NiMemcpy(pkDest->m_kFluidData.bufferVel, m_kFluidData.bufferVel,
            m_kFluidData.maxParticles * m_kFluidData.bufferVelByteStride);
    }
    if (m_kFluidData.bufferLife)
    {
        pkDest->m_kFluidData.bufferLife =
            NiAlloc(NxF32, m_kFluidData.maxParticles *
            m_kFluidData.bufferLifeByteStride / sizeof(NxF32));
        pkDest->m_kFluidData.bufferLifeByteStride =
            m_kFluidData.bufferLifeByteStride;
        NiMemcpy(pkDest->m_kFluidData.bufferLife, m_kFluidData.bufferLife,
            m_kFluidData.maxParticles * m_kFluidData.bufferLifeByteStride);
    }
    if (m_kFluidData.bufferDensity)
    {
        pkDest->m_kFluidData.bufferDensity =
            NiAlloc(NxF32, m_kFluidData.maxParticles *
            m_kFluidData.bufferDensityByteStride / sizeof(NxF32));
        pkDest->m_kFluidData.bufferDensityByteStride =
            m_kFluidData.bufferDensityByteStride;
        NiMemcpy(pkDest->m_kFluidData.bufferDensity, 
            m_kFluidData.bufferDensity,
            m_kFluidData.maxParticles * m_kFluidData.bufferDensityByteStride);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXMeshFluidData);
//---------------------------------------------------------------------------
bool NiPhysXMeshFluidData::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXMeshFluidData::LoadBinary(NiStream& kStream)
{
    NiMeshPSysData::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_kFluidData.maxParticles);
    m_kFluidData.numParticlesPtr = NiAlloc(NxU32, 1);
    NiStreamLoadBinary(kStream, *(m_kFluidData.numParticlesPtr));
    
    unsigned int uiSize;
    NiStreamLoadBinary(kStream, uiSize);
    if (uiSize)
    {
        NiStreamLoadBinary(kStream, m_kFluidData.bufferPosByteStride);
        NIASSERT(uiSize == m_kFluidData.maxParticles *
            m_kFluidData.bufferPosByteStride / sizeof(NxF32));
        m_kFluidData.bufferPos = NiAlloc(NxF32, uiSize);
        NiStreamLoadBinary(kStream, m_kFluidData.bufferPos, uiSize);
    }

    NiStreamLoadBinary(kStream, uiSize);
    if (uiSize)
    {
        NiStreamLoadBinary(kStream, m_kFluidData.bufferVelByteStride);
        NIASSERT(uiSize == m_kFluidData.maxParticles *
            m_kFluidData.bufferVelByteStride / sizeof(NxF32));
        m_kFluidData.bufferVel = NiAlloc(NxF32, uiSize);
        NiStreamLoadBinary(kStream, m_kFluidData.bufferVel, uiSize);
    }

    NiStreamLoadBinary(kStream, uiSize);
    if (uiSize)
    {
        NiStreamLoadBinary(kStream, m_kFluidData.bufferLifeByteStride);
        NIASSERT(uiSize == m_kFluidData.maxParticles *
            m_kFluidData.bufferLifeByteStride / sizeof(NxF32));
        m_kFluidData.bufferLife = NiAlloc(NxF32, uiSize);
        NiStreamLoadBinary(kStream, m_kFluidData.bufferLife, uiSize);
    }

    NiStreamLoadBinary(kStream, uiSize);
    if (uiSize)
    {
        NiStreamLoadBinary(kStream, m_kFluidData.bufferDensityByteStride);
        NIASSERT(uiSize == m_kFluidData.maxParticles *
            m_kFluidData.bufferDensityByteStride / sizeof(NxF32));
        m_kFluidData.bufferDensity = NiAlloc(NxF32, uiSize);
        NiStreamLoadBinary(kStream, m_kFluidData.bufferDensity, uiSize);
    }

    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 3, 0, 1))
    {
        // Deal with flags buffer
        NiStreamLoadBinary(kStream, uiSize);
        if (uiSize)
        {
            NxU32 uiBufferFlagsByteStride;
            NiStreamLoadBinary(kStream, uiBufferFlagsByteStride);
            NIASSERT(uiSize == m_kFluidData.maxParticles *
                uiBufferFlagsByteStride / sizeof(NxU32));

            NxU32* puiBufferFlags = NiAlloc(NxU32, uiSize);
            NiStreamLoadBinary(kStream, puiBufferFlags, uiSize);
            
            NiFree(puiBufferFlags);
        }
    }
}
//---------------------------------------------------------------------------
void NiPhysXMeshFluidData::LinkObject(NiStream& kStream)
{
    NiMeshPSysData::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXMeshFluidData::RegisterStreamables(NiStream& kStream)
{
    if (!NiMeshPSysData::RegisterStreamables(kStream))
        return false;
    
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXMeshFluidData::SaveBinary(NiStream& kStream)
{
    NiMeshPSysData::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_kFluidData.maxParticles);
    NiStreamSaveBinary(kStream, *(m_kFluidData.numParticlesPtr));
        
    unsigned int uiSize;
    
    if (m_kFluidData.bufferPos)
    {
        uiSize = m_kFluidData.maxParticles * m_kFluidData.bufferPosByteStride
            / sizeof(NxF32);
        NiStreamSaveBinary(kStream, uiSize);
        NiStreamSaveBinary(kStream, m_kFluidData.bufferPosByteStride);
        NiStreamSaveBinary(kStream, m_kFluidData.bufferPos, uiSize);
    }
    else
    {
        uiSize = 0;
        NiStreamSaveBinary(kStream, uiSize);
    }
    
    if (m_kFluidData.bufferVel)
    {
        uiSize = m_kFluidData.maxParticles * m_kFluidData.bufferVelByteStride
            / sizeof(NxF32);
        NiStreamSaveBinary(kStream, uiSize);
        NiStreamSaveBinary(kStream, m_kFluidData.bufferVelByteStride);
        NiStreamSaveBinary(kStream, m_kFluidData.bufferVel, uiSize);
    }
    else
    {
        uiSize = 0;
        NiStreamSaveBinary(kStream, uiSize);
    }
    
    if (m_kFluidData.bufferLife)
    {
        uiSize = 
            m_kFluidData.maxParticles * m_kFluidData.bufferLifeByteStride
            / sizeof(NxF32);
        NiStreamSaveBinary(kStream, uiSize);
        NiStreamSaveBinary(kStream, m_kFluidData.bufferLifeByteStride);
        NiStreamSaveBinary(kStream, m_kFluidData.bufferLife, uiSize);
    }
    else
    {
        uiSize = 0;
        NiStreamSaveBinary(kStream, uiSize);
    }
    
    if (m_kFluidData.bufferDensity)
    {
        uiSize = 
            m_kFluidData.maxParticles * m_kFluidData.bufferDensityByteStride
            / sizeof(NxF32);
        NiStreamSaveBinary(kStream, uiSize);
        NiStreamSaveBinary(kStream, m_kFluidData.bufferDensityByteStride);
        NiStreamSaveBinary(kStream, m_kFluidData.bufferDensity, uiSize);
    }
    else
    {
        uiSize = 0;
        NiStreamSaveBinary(kStream, uiSize);
    }
}
//---------------------------------------------------------------------------
bool NiPhysXMeshFluidData::IsEqual(NiObject* pkObject)
{
    if (!NiMeshPSysData::IsEqual(pkObject))
    {
        return false;
    }

    NiPhysXMeshFluidData* pkDest = (NiPhysXMeshFluidData*) pkObject;

    if (m_kFluidData.maxParticles != pkDest->m_kFluidData.maxParticles)
        return false;

    if (*(m_kFluidData.numParticlesPtr) != 
        *(pkDest->m_kFluidData.numParticlesPtr))
        return false;

    if (m_kFluidData.bufferPos)
    {
        if (!pkDest->m_kFluidData.bufferPos)
            return false;
        for (unsigned int ui = 0;
            ui < *(m_kFluidData.numParticlesPtr) * 3;
            ui++)
        {
            if (m_kFluidData.bufferPos[ui] !=
                pkDest->m_kFluidData.bufferPos[ui])
                return false;
        }
    }
    else
    {
        if (pkDest->m_kFluidData.bufferPos)
            return false;
    }

    if (m_kFluidData.bufferVel)
    {
        if (!pkDest->m_kFluidData.bufferVel)
            return false;
        for (unsigned int ui = 0;
            ui < *(m_kFluidData.numParticlesPtr) * 3;
            ui++)
        {
            if (m_kFluidData.bufferVel[ui] !=
                pkDest->m_kFluidData.bufferVel[ui])
                return false;
        }
    }
    else
    {
        if (pkDest->m_kFluidData.bufferVel)
            return false;
    }

    if (m_kFluidData.bufferLife)
    {
        if (!pkDest->m_kFluidData.bufferLife)
            return false;
        for (unsigned int ui = 0; ui < *(m_kFluidData.numParticlesPtr);
            ui++)
        {
            if (m_kFluidData.bufferLife[ui] !=
                pkDest->m_kFluidData.bufferLife[ui])
                return false;
        }
    }
    else
    {
        if (pkDest->m_kFluidData.bufferLife)
            return false;
    }

    if (m_kFluidData.bufferDensity)
    {
        if (!pkDest->m_kFluidData.bufferDensity)
            return false;
        for (unsigned int ui = 0; ui < *(m_kFluidData.numParticlesPtr);
            ui++)
        {
            if (m_kFluidData.bufferDensity[ui] !=
                pkDest->m_kFluidData.bufferDensity[ui])
                return false;
        }
    }
    else
    {
        if (pkDest->m_kFluidData.bufferDensity)
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
