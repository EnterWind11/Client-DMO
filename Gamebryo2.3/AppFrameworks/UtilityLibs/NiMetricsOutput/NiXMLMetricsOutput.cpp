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

#include "NiMetricsOutputPCH.h"
#include "NiXMLMetricsOutput.h"
#include "NiMetricsTimer.h"

//---------------------------------------------------------------------------
NiXMLMetricsOutput::NiXMLMetricsOutput(unsigned int uiFramePeriod,
    const char* pcNiFileName, unsigned int uiNiFileBufferSize,
    unsigned int uiHashSize) :
    NiCalculatingOutput(uiFramePeriod, uiHashSize),
    m_kFile(pcNiFileName, NiFile::WRITE_ONLY, uiNiFileBufferSize)
{
    if (m_kFile)
        WriteHeader();
}
//---------------------------------------------------------------------------
bool NiXMLMetricsOutput::GetFileValid() const
{
    return (bool)m_kFile;
}
//---------------------------------------------------------------------------
NiXMLMetricsOutput::~NiXMLMetricsOutput()
{
    // Need to increment frame count here to not be "off by one" on the
    // final number of frames, since the count starts at zero.  This is
    // analogous to what gets done in Update().
    ++m_uiFrameCount;

    WriteBuffer();
    WriteFooter();
}
//---------------------------------------------------------------------------
void NiXMLMetricsOutput::AddValue(const char* pcName, float fValue)
{
    MetricContainer* pkCont = FindOrRegisterMetric(pcName);
    pkCont->AddValue(fValue);
}
//---------------------------------------------------------------------------
void NiXMLMetricsOutput::EndTimer(NiMetricsTimer& kTimer)
{
    AddValue(kTimer.GetName(), kTimer.GetElapsed());
}
//---------------------------------------------------------------------------
void NiXMLMetricsOutput::AddImmediateEvent(const char* pcName, float fValue)
{
    AddValue(pcName, fValue);
}
//---------------------------------------------------------------------------
void NiXMLMetricsOutput::Update()
{
    // Explicitly handle frame count since we will write the buffer after
    // the frame period.

    if (++m_uiFrameCount >= m_uiFramePeriod)
    {
        // Write this to the log
        WriteBuffer();

        m_uiFrameCount = 0;
    }
}
//---------------------------------------------------------------------------
#define XMLHEADER "xmlmetricsoutput"
#define XMLFRAME "period"
#define XMLMETRIC "metric"
#define XMLTAB "    "
#define XMLEOL "\r\n"
//---------------------------------------------------------------------------
void NiXMLMetricsOutput::WriteHeader()
{
    NIASSERT(m_kFile);

    const char acHeader[] =
        //"<!xml namespace etc.../>\n"
        "<" XMLHEADER ">" XMLEOL
        ;
    m_kFile.Write(acHeader, sizeof(acHeader) - 1);
}
//---------------------------------------------------------------------------
void NiXMLMetricsOutput::WriteBuffer()
{
    NIASSERT(m_kFile);

    // Calculate values before writing...
    NiCalculatingOutput::CalculateValues();

    // Write frame header
    char acBuffer[512];

    int iLen = NiSprintf(acBuffer, sizeof(acBuffer),
        XMLTAB "<" XMLFRAME " frames=\"%u\">" XMLEOL, m_uiFrameCount);
    NIASSERT(iLen > 0);
    m_kFile.Write(acBuffer, (unsigned int)iLen);

    NiTMapIterator kIter = m_kMetrics.GetFirstPos();
    while(kIter)
    {
        const char* pcName;
        MetricContainer* pkMetric;
        m_kMetrics.GetNext(kIter, pcName, pkMetric);

        // Skip printing metrics that don't have a count for this particular
        // frame.
        float fCount = pkMetric->GetValue(FUNC_COUNT);
        if (fCount > 0.0f)
        {
            int iLen = NiSprintf(acBuffer, sizeof(acBuffer), 
                XMLTAB XMLTAB "<" XMLMETRIC " name=\"%s\" %s=\"%e\" %s=\"%e\" "
                "%s=\"%e\" %s=\"%e\" %s=\"%e\"/>" XMLEOL,
                pkMetric->GetName(),     
                GetName(FUNC_COUNT), fCount,
                GetName(FUNC_MEAN), pkMetric->GetValue(FUNC_MEAN),
                GetName(FUNC_SUM), pkMetric->GetValue(FUNC_SUM),
                GetName(FUNC_MAX), pkMetric->GetValue(FUNC_MAX),
                GetName(FUNC_MIN), pkMetric->GetValue(FUNC_MIN));

            NIASSERT(iLen > 0);
            m_kFile.Write(acBuffer, (unsigned int)iLen);
        }
    }

    // Write frame footer
    const char acFooter[] = XMLTAB "</" XMLFRAME ">" XMLEOL;
    m_kFile.Write(acFooter, sizeof(acFooter) - 1);
}
//---------------------------------------------------------------------------
void NiXMLMetricsOutput::WriteFooter()
{
    NIASSERT(m_kFile);

    const char acFooter[] = "</" XMLHEADER ">" XMLEOL;
    m_kFile.Write(acFooter, sizeof(acFooter) - 1);
}
//---------------------------------------------------------------------------
