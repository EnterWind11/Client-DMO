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

#ifndef NIANIMATIONCOMPRESSION_H
#define NIANIMATIONCOMPRESSION_H

#include "NiAnimationCompressionLibType.h"

class NiAVObject;

#include <NiAnimation.h>

#define NI_COMPRESS_KEY                 "NI_COMPRESS"
#define NI_COMPRESSION_RATIO_KEY        "NI_COMPRESSION_RATIO"
#define NI_COMPACT_CONTROL_POINTS_KEY   "NI_COMPACT_CONTROL_POINTS"

typedef NiTPointerList<NiBSplineBasisData*> NiBasisDataList;

class NIANIMATIONCOMPRESSION_ENTRY NiAnimationCompression
{
public:
    static void CompressInterpolators(NiControllerSequence* pkSequence,
        NiAVObject* pkCharacterRoot, float fCompressionRatio,
        bool bAlwaysCompress, bool bCompactControlPoints);
    static NiInterpolator* CompressInterpolator(NiInterpolator* pkInterp, 
        NiInterpController* pkController, float fCompressionRatio, 
        bool bAlwaysCompress, NiBSplineData* pkBSplineData, 
        bool bCompactControlPoints, NiBasisDataList& kBasisList,
        float fTimeStep);

protected:
    static unsigned int EstimateCompressedSize(NiKeyBasedInterpolator* 
        pkInterpolator, unsigned int uiChannel, bool& bCannotCompress,
        float fSampleTimeStep, float fCompressionRatio, 
        bool bCompactControlPoints);
    static NiInterpolator* CompressTransformInterpolator(NiInterpolator*
        pkInterpolator, float fCompressionRatio, float fTimeStep,
        NiBSplineData* pkBSplineData, bool bCompactControlPoints,
        NiBasisDataList& kBasisList);
    static NiInterpolator* CompressColorInterpolator(NiInterpolator*
        pkInterpolator, float fCompressionRatio, float fTimeStep, 
        NiBSplineData* pkBSplineData, bool bCompactControlPoints,
        NiBasisDataList& kBasisList);
    static NiInterpolator* CompressFloatInterpolator(NiInterpolator*
        pkInterpolator, float fCompressionRatio, float fTimeStep,
        NiBSplineData* pkBSplineData, bool bCompactControlPoints,
        NiBasisDataList& kBasisList);
    static NiInterpolator* CompressPoint3Interpolator(NiInterpolator*
        pkInterpolator, float fCompressionRatio, float fTimeStep,  
        NiBSplineData* pkBSplineData, bool bCompactControlPoints,
        NiBasisDataList& kBasisList);

    static void ComputePerObjectOverrides(NiAVObject* pkObj, bool& bCompress,
        float& fCompressionRatio, bool& bCompactControlPoints);
    static bool IgnoreInterpolator(NiInterpolator* pkInterp,
        const char* pcAVObjectName,
        const char* pcPropertyType,
        const char* pcCtlrType,
        const char* pcCtlrID,
        const char* pcInterpolatorID);

    static NiBSplineBasisData* InsertBasis(NiBSplineInterpolator* pkInterp, 
        NiBasisDataList& kBasisList, const NiBSplineBasis<float, 3>* pkBasis);
};

#endif  // #ifndef NIANIMATIONCOMPRESSION_H
