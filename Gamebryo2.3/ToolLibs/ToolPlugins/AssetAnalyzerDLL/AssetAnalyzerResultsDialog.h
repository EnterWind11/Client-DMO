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

#ifndef ASSETANALYZERRESULTSDIALOG_H
#define ASSETANALYZERRESULTSDIALOG_H

#include "NiTPointerList.h"

/// This class implements the error results detected by the Asset Analyzer
class AssetAnalyzerResultsDialog : public NiDialog
{
public:
    /// Construction for the dialog box.
    AssetAnalyzerResultsDialog(NiModuleRef hInstance, NiWindowRef hWndParent,
        NiTPointerList<NiString*>* m_pkErrors);

    virtual ~AssetAnalyzerResultsDialog();

    /// Required virtual function overrides from NiDialog.
    virtual void InitDialog();
    virtual BOOL OnCommand(int iWParamLow, int iWParamHigh, long lParam);

protected:

    NiTPointerList<NiString*>* m_pkErrors;
};

#endif  // #ifndef ASSETANALYZERRESULTSDIALOG_H
