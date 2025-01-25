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

#ifndef MYIPLUGIN_H
#define MYIPLUGIN_H

#include <maya/MPxFileTranslator.h>

#include <MDt.h>
#include <MDtExt.h>

//---------------------------------------------------------------------------
// CLASS DECLARATIONS
//---------------------------------------------------------------------------
class SceneViewerInfo;
class MyImmerse;
class NiShaderAttributeDesc;

class NifExportPlugin : public MPxFileTranslator 
{
private:
    static MString  magic;

public:
    NifExportPlugin ();
    virtual ~NifExportPlugin ();
    static void *creator ();

    MStatus reader( const MFileObject& file, const MString&  optionsString,
        MPxFileTranslator::FileAccessMode mode);

    MStatus writer ( const MFileObject& file, const MString& optionsString,
        MPxFileTranslator::FileAccessMode mode);

    bool haveReadMethod  () const;
    bool haveWriteMethod () const;
    bool canBeOpened () const;
    MString defaultExtension () const;
    MFileKind identifyFile ( const MFileObject& fileName, const char *buffer,
        short size) const;

    static bool inBatchMode();
    static void setBatchMode(bool isOn);

protected:

    bool DestinationFileReadOnly(const MFileObject& file);
    void LoadPixelShaderData();
    char* CreateAttributeString(const NiShaderAttributeDesc* pkDesc);
    void CreateArrayAttributeString(const NiShaderAttributeDesc* pkDesc,
        char* pcBuffer);
    void SetAvaiableViewers();

private:
    static bool ms_bInBatchMode;


};

#endif