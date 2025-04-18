:: EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
:: 
:: This software is supplied under the terms of a license agreement or
:: nondisclosure agreement with Emergent Game Technologies and may not 
:: be copied or disclosed except in accordance with the terms of that 
:: agreement.
:: 
::      Copyright (c) 1996-2007 Emergent Game Technologies.
::      All Rights Reserved.
:: 
:: Emergent Game Technologies, Chapel Hill, North Carolina 27517
:: http://www.emergent.net
:: 
:: ------------------------------------------------------------------------
:: 
:: Batch file to generate the files NiStandardMaterialNodeLibrary.h
:: and NiStandardMaterialNodeLibrary.cpp from the XML file
:: StandardMaterialNodeLibraryFragments.xml.
:: 
:: This batch script expects to run from the directory containing these files.
:: 
:: ------------------------------------------------------------------------
@echo off

SETLOCAL

:: Attempt to use a release build of the application
set VC71EXE=..\..\..\Tools\DeveloperTools\NiMaterialNodeXMLLibraryParser\Win32\VC71\NiMaterialNodeXMLLibraryParser.exe
set VC80EXE=..\..\..\Tools\DeveloperTools\NiMaterialNodeXMLLibraryParser\Win32\VC80\NiMaterialNodeXMLLibraryParser.exe

:: First, try to use VC80 version
if exist %VC80EXE% goto :UseVC80

:: Then, try to use VC71 version
if exist %VC71EXE% goto :UseVC71

::  Else, error
echo.
echo No built release version of the tool NiMaterialNodeXMLLibraryParser.exe
echo   was found!
echo Be sure this tool is built.

goto :End

:: ------------------------------------------------------------------------
:UseVC80
echo.
echo Running VC80 version of NiMaterialNodeXMLLibraryParser.exe:
echo.
set EXE=%VC80EXE%
goto :Execute

:: ------------------------------------------------------------------------
:UseVC71

echo.
echo Running VC71 version of NiMaterialNodeXMLLibraryParser.exe:
echo.
set EXE=%VC71EXE%
goto :Execute

:: ------------------------------------------------------------------------
:Execute
%EXE% -in VSMBlurMaterialNodeLibraryFragments.xml -class NiVSMBlurMaterialNodeLibrary -modifier NIMAIN_ENTRY -pch NiMainPCH.h
if errorlevel 1 goto ErrorInExe
goto :Success

:: ------------------------------------------------------------------------
:Success
echo.
echo SUCCESS!
echo.
goto :End

:: ------------------------------------------------------------------------
:Failure
echo.
echo NO FILES PARSED!
echo.
goto :End

:: ------------------------------------------------------------------------
:ErrorInExe
echo.
echo ERROR!
echo.
goto :End

:: ------------------------------------------------------------------------
:End
PAUSE

ENDLOCAL
