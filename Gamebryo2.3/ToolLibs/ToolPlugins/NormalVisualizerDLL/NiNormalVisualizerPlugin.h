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

#ifndef NINORMALVISUALIZERPLUGIN_H
#define NINORMALVISUALIZERPLUGIN_H

class NiNormalVisualizerPlugin : public NiPlugin
{
public:
    NiDeclareRTTI;

    /// Default constructor.
    NiNormalVisualizerPlugin();

    /// Required virtual function overrides from NiPlugin.
    virtual bool CanExecute(NiPluginInfo* pkInfo);
    virtual NiExecutionResultPtr Execute(const NiPluginInfo* pkInfo);
    virtual NiPluginInfo* GetDefaultPluginInfo(); 
    virtual bool HasManagementDialog();
    virtual bool DoManagementDialog(NiPluginInfo* pkInitialInfo,
        NiWindowRef hWndParent);

protected:
    static void FindGeometry(NiAVObject* pkRoot, 
        NiTList<NiTriBasedGeom*>& kGeoms);

    static float ms_fNormalScale;
    static float ms_fBoundScale;
};

#endif  // #ifndef NINORMALVISUALIZERPLUGIN_H
      