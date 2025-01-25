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
#include "NiD3D10RendererPCH.h"

#include "NiD3D10PixelFormat.h"
#include "NiD3D10Renderer.h"
#include "NiD3D10ResourceManager.h"
#include "NiD3D10SourceTextureData.h"

#include <NiImageConverter.h>
#include <NiSourceCubeMap.h>
#include <NiSourceTexture.h>

unsigned short NiD3D10SourceTextureData::ms_usSkipLevels = 0;

//---------------------------------------------------------------------------
NiD3D10SourceTextureData::NiD3D10SourceTextureData(NiSourceTexture* pkTexture)
    : NiD3D10TextureData(pkTexture),
    m_bReplacementData(false),
    m_bMipmap(false),
    m_uiSourceRevID(0),
    m_uiFormattedSize(0),
    m_usLevelsSkipped(0),
    m_usNumTextures(0),
    m_uiPaletteRevID(0)
{
    m_usTextureType |= TEXTURETYPE_SOURCE;
    if (NiIsKindOf(NiSourceCubeMap, pkTexture))
        m_usTextureType |= TEXTURETYPE_CUBE;
}
//---------------------------------------------------------------------------
NiD3D10SourceTextureData::~NiD3D10SourceTextureData()
{
}
//---------------------------------------------------------------------------
NiD3D10SourceTextureData* NiD3D10SourceTextureData::Create(
    NiSourceTexture* pkTexture)
{
    NiD3D10SourceTextureData* pkThis = 
        NiNew NiD3D10SourceTextureData(pkTexture);

    bool bSuccess = pkThis->LoadTexture();

    NIASSERT(bSuccess == false || pkTexture->GetRendererData() == pkThis);

    if (bSuccess)
        return pkThis;
    else
        return NULL;
}
//---------------------------------------------------------------------------
bool NiD3D10SourceTextureData::LoadTexture()
{
    NIASSERT(NiIsKindOf(NiSourceTexture, m_pkTexture));
    NiSourceTexture* pkTexture = (NiSourceTexture*)m_pkTexture;

    const NiTexture::FormatPrefs& kPrefs = m_pkTexture->GetFormatPreferences();
    bool bCube = IsCubeMap();

    // It is safe to use regular pointers, since the image converter functions
    // never assign incoming or outgoing data to smart pointers.
    NiPixelData* pkSrcPixels = NULL;

    m_bReplacementData = false;

    // If there's NiD3D10PersistentSrcTextureRendererData, use it.
    {
        pkSrcPixels = pkTexture->GetSourcePixelData();

        if (!pkSrcPixels)
        {
            // Import file into pixel data object
            const char* pcFilename = pkTexture->GetPlatformSpecificFilename();
            if (pcFilename)
            {
                pkSrcPixels = NiImageConverter::GetImageConverter()->
                    ReadImageFile(pcFilename, NULL);
            }
        }
    }

    // This smart pointer exists to allow pkSrcPixels to be deleted if it
    // needs to be.
    NiPixelDataPtr spSrcPixels = pkSrcPixels;

    const NiPixelFormat* pkDestFmt = NULL;

    if (!pkSrcPixels)
    {
        pkSrcPixels = GetReplacementData(pkTexture, 
            bCube ? CUBEMAPFILENOTFOUND : FILENOTFOUND);

        m_bReplacementData = true;
    }
    else
    {
        NIASSERT(pkSrcPixels);
        const NiPixelFormat& kSrcFmt = pkSrcPixels->GetPixelFormat();

        if (bCube && (pkSrcPixels->GetWidth() != pkSrcPixels->GetHeight()))
        {   // Check width == height
            pkSrcPixels = GetReplacementData(pkTexture, CUBEMAPBADDIMENSIONS);
            m_bReplacementData = true;
        }
        else
        {
            pkDestFmt = FindMatchingPixelFormat(kSrcFmt, kPrefs, bCube ? 
                D3D10_FORMAT_SUPPORT_TEXTURECUBE : 
                D3D10_FORMAT_SUPPORT_TEXTURE2D);

            if (pkDestFmt == NULL)
            {
                pkSrcPixels = GetReplacementData(pkTexture, bCube ? 
                    CUBEMAPFAILEDCONVERT : FAILEDCONVERT);
                m_bReplacementData = true;
            }
        }
    }

    if (m_bReplacementData)
    {
        NIASSERT(pkSrcPixels);
        pkDestFmt = &pkSrcPixels->GetPixelFormat();
        NIASSERT(pkDestFmt->GetRendererHint() == DXGI_FORMAT_R8G8B8A8_UNORM);
    }

    {
        m_bMipmap = 
            (!m_bReplacementData &&
            ((kPrefs.m_eMipMapped == NiTexture::FormatPrefs::YES) ||
            ((kPrefs.m_eMipMapped == NiTexture::FormatPrefs::MIP_DEFAULT) && 
            NiTexture::GetMipmapByDefault())));

        NiPixelData* pkFormatted = NULL;
        if (m_bReplacementData)
        {
            pkFormatted = pkSrcPixels;  // Do not convert replacement data.
        }
        else
        {
            // Convert data format
            pkFormatted = 
                NiImageConverter::GetImageConverter()->ConvertPixelData(
                *pkSrcPixels, *pkDestFmt, pkSrcPixels, m_bMipmap);
        }
        NIASSERT(pkFormatted);

        m_uiSourceRevID = pkSrcPixels->GetRevisionID();
        m_kPixelFormat = *pkDestFmt;
        m_uiFormattedSize = pkFormatted->GetTotalSizeInBytes();

        // Allocate and fill texture
        NIASSERT(m_pkD3D10Texture == NULL);
        InitializeTexture(pkFormatted, bCube, pkTexture->GetStatic());

        // Attach palette - no need to check result because palette was just
        // used to un-palettize the data.
        UpdatePalette(pkSrcPixels->GetPalette());

        // Some of the objects used locally may have zero refcounts. Smart
        // pointers have been avoided in the function for speed. Here, at the
        // end of the function, the pkFormatted pointer is assigned to a smart
        // pointer to force destruction, as needed.
        NiPixelDataPtr spDestructor = pkFormatted;
    }

    pkTexture->SetRendererData(this);

    if (NiSourceTexture::GetDestroyAppDataFlag())
        pkTexture->DestroyAppPixelData();

    return true;
}
//---------------------------------------------------------------------------
void NiD3D10SourceTextureData::Update()
{
    // Textures using pixel replacement data needs not be updated
    if (m_bReplacementData)
        return;

    // Can't update static textures
    if (((NiSourceTexture*)m_pkTexture)->GetStatic())
        return;

    NiPixelData* pkSrcPixels = 
        ((NiSourceTexture*)m_pkTexture)->GetSourcePixelData();

    if (pkSrcPixels)
    {
        // Check to see if palette has changed
        bool bPaletteExpand = UpdatePalette(pkSrcPixels->GetPalette());

        // The most common reason for this case below is a change to the
        // pixels in the pixel data object. We may also have to enter this 
        // case when a palette changes and the destination format has no
        // palette (in which case the palette must be re-expanded into the
        // RGB(A) texture data, and expensive operation)
        if (pkSrcPixels->GetRevisionID() != m_uiSourceRevID || bPaletteExpand)
        {
            // Convert data format
            NiPixelData* pkFormatted = 
                NiImageConverter::GetImageConverter()->ConvertPixelData(
                *pkSrcPixels, m_kPixelFormat, pkSrcPixels, m_bMipmap);

            NIASSERT(pkFormatted);

            m_uiSourceRevID = pkSrcPixels->GetRevisionID();

            // Copy to D3D10 texture
            if (m_pkD3D10Texture)
                UpdateTexture(pkFormatted);

            // Some of the objects used locally may have zero refcounts. 
            // We have avoided using smart pointers in the function for speed.
            // Here, at the end of the function, we assign them to smart 
            // pointers to force destruction as needed
            NiPixelDataPtr spDestructor = pkFormatted;
        }
    }
}
//---------------------------------------------------------------------------
void NiD3D10SourceTextureData::SetMipmapSkipLevel(unsigned short usSkip)
{
    ms_usSkipLevels = usSkip;
}
//---------------------------------------------------------------------------
unsigned short NiD3D10SourceTextureData::GetMipmapSkipLevel()
{
    return ms_usSkipLevels;
}
//---------------------------------------------------------------------------
bool NiD3D10SourceTextureData::InitializeTexture(const NiPixelData* pkPixels,
    bool bCube, bool bStatic)
{
    NIASSERT(pkPixels);

    DXGI_FORMAT eFormat = 
        NiD3D10PixelFormat::DetermineDXGIFormat(m_kPixelFormat);

    m_usLevels = (unsigned short)pkPixels->GetNumMipmapLevels();

    m_usLevelsSkipped = ms_usSkipLevels;
    if (m_usLevelsSkipped > m_usLevels - 1)
        m_usLevelsSkipped = m_usLevels - 1;
    m_usLevels -= m_usLevelsSkipped;

    m_uiWidth = pkPixels->GetWidth(m_usLevelsSkipped);
    m_uiHeight = pkPixels->GetHeight(m_usLevelsSkipped);
    m_usNumTextures = pkPixels->GetNumFaces();

    NIASSERT(ms_usSkipLevels == 0 || m_usLevels != 0);

    unsigned int uiMSAACount = 1;
    unsigned int uiMSAAQuality = 0;

    D3D10_USAGE eUsage = 
        (bStatic ? D3D10_USAGE_IMMUTABLE : D3D10_USAGE_DEFAULT);
    unsigned int uiCPUAccessFlags = 0;

    unsigned int uiBindFlags = D3D10_BIND_SHADER_RESOURCE;

    NIASSERT(m_usLevels != 0 && 
        m_usLevels < D3D10_MAX_TEXTURE_DIMENSION_2_TO_EXP);

    unsigned int uiMiscFlags = (bCube ? D3D10_RESOURCE_MISC_TEXTURECUBE : 0);

    const unsigned short usMaxTextureCount = 8;
    NIASSERT(m_usNumTextures < usMaxTextureCount)

    D3D10_SUBRESOURCE_DATA akInitialData
        [usMaxTextureCount * D3D10_MAX_TEXTURE_DIMENSION_2_TO_EXP];
    unsigned int uiCompressedBitsPerBlock = 16;
    bool bCompressed = m_kPixelFormat.GetCompressed();
    if (bCompressed &&
        (m_kPixelFormat.GetRendererHint() == DXGI_FORMAT_BC1_TYPELESS ||
        m_kPixelFormat.GetRendererHint() == DXGI_FORMAT_BC1_UNORM ||
        m_kPixelFormat.GetRendererHint() == DXGI_FORMAT_BC1_UNORM_SRGB ||
        m_kPixelFormat.GetRendererHint() == DXGI_FORMAT_BC4_TYPELESS ||
        m_kPixelFormat.GetRendererHint() == DXGI_FORMAT_BC4_UNORM ||
        m_kPixelFormat.GetRendererHint() == DXGI_FORMAT_BC4_SNORM ||
        m_kPixelFormat.GetRendererHint() == DXGI_FORMAT_BC5_TYPELESS ||
        m_kPixelFormat.GetRendererHint() == DXGI_FORMAT_BC5_UNORM ||
        m_kPixelFormat.GetRendererHint() == DXGI_FORMAT_BC5_SNORM))
    {
        uiCompressedBitsPerBlock = 8;
    }
    unsigned int uiEntry = 0;
    for (unsigned short i = 0; i < m_usNumTextures; i++)
    {
        for (unsigned short j = 0; j < m_usLevels; j++)
        {
            unsigned short usLevel = j + m_usLevelsSkipped;
            unsigned int uiWidth = pkPixels->GetWidth(usLevel);
            unsigned int uiPitch = uiWidth * pkPixels->GetPixelStride();
            if (bCompressed)
            {
                // pkPixels->GetPixelStride = 0 for compressed formats; need
                // to recalculate and account for non-divisible-by-four pixel 
                // counts, such as smaller mipmap levels.
                uiPitch = (uiWidth + 3) & 0xFFFFFFC;
                if (uiCompressedBitsPerBlock == 8)
                    uiPitch /= 2;
            }

            // Texture dimensions must match.
            NIASSERT(pkPixels->GetWidth(usLevel, i) == 
                pkPixels->GetWidth(usLevel, 0));
            NIASSERT(pkPixels->GetHeight(usLevel, i) ==
                pkPixels->GetHeight(usLevel, 0));
            akInitialData[uiEntry].pSysMem = pkPixels->GetPixels(usLevel, i);
            akInitialData[uiEntry].SysMemPitch = uiPitch;

            // MS recommends this, but it's not strictly necessary.
            akInitialData[uiEntry].SysMemSlicePitch = 
                pkPixels->GetSizeInBytes(usLevel, i);

            uiEntry++;
        }
    }

    NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();
    NIASSERT(pkRenderer && pkRenderer->GetResourceManager() &&
        pkRenderer->GetD3D10Device());

    ID3D10Texture2D* pkD3D10Texture = 
        pkRenderer->GetResourceManager()->CreateTexture2D(
        m_uiWidth, m_uiHeight, m_usLevels, m_usNumTextures, eFormat, 
        uiMSAACount, uiMSAAQuality, eUsage, uiBindFlags, uiCPUAccessFlags, 
        uiMiscFlags, akInitialData);

    if (pkD3D10Texture == NULL)
    {
        return false;
    }

    D3D10_SHADER_RESOURCE_VIEW_DESC kDesc;
    kDesc.Format = eFormat;
    if (bCube)
    {
        NIASSERT(m_usNumTextures == 6);
        kDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURECUBE;
        kDesc.TextureCube.MostDetailedMip = 0;
        kDesc.TextureCube.MipLevels = m_usLevels;
    }
    else
    {
        if (m_usNumTextures == 1)
        {
            kDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
            kDesc.Texture2D.MostDetailedMip = 0;
            kDesc.Texture2D.MipLevels = m_usLevels;
        }
        else
        {
            kDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2DARRAY;
            kDesc.Texture2DArray.MostDetailedMip = 0;
            kDesc.Texture2DArray.MipLevels = m_usLevels;
            kDesc.Texture2DArray.FirstArraySlice = 0;
            kDesc.Texture2DArray.ArraySize = m_usNumTextures;
        }
    }

    ID3D10ShaderResourceView* pkResourceView = NULL;
    HRESULT hr = pkRenderer->GetD3D10Device()->CreateShaderResourceView(
        pkD3D10Texture, &kDesc, &pkResourceView);
    if (FAILED(hr) || pkResourceView == NULL)
    {

        if (pkResourceView)
        {
            pkResourceView->Release();
        }
        return false;
    }

    return InitializeFromD3D10Resource(pkD3D10Texture, pkResourceView);
}
//---------------------------------------------------------------------------
bool NiD3D10SourceTextureData::UpdateTexture(const NiPixelData* pkPixels)
{
    NIASSERT(pkPixels);

    DXGI_FORMAT eFormat = 
        NiD3D10PixelFormat::DetermineDXGIFormat(m_kPixelFormat);

    m_uiWidth = pkPixels->GetWidth();
    m_uiHeight = pkPixels->GetHeight();
    m_usLevels = (unsigned short)pkPixels->GetNumMipmapLevels();

    unsigned short usSkipLevels = ms_usSkipLevels;
    if (usSkipLevels > m_usLevels - 1)
        usSkipLevels = m_usLevels - 1;

    // Adjust (possibly non-power-of-two) dimensions by uiSkipLevels.
    unsigned int i = 0;
    for (; i < usSkipLevels; i++)
    {
        if (m_uiWidth & 0x1)    // If odd, add 1 so shifted value rounds up.
            m_uiWidth++;
        m_uiWidth >>= 1;
        if (m_uiHeight & 0x1)   // If odd, add 1 so shifted value rounds up.
            m_uiHeight++;
        m_uiHeight >>= 1;
    }

    m_usLevelsSkipped = usSkipLevels;

    NIASSERT(ms_usSkipLevels == 0 || m_usLevels != 0);

    NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();
    NIASSERT(pkRenderer && pkRenderer->GetResourceManager() &&
        pkRenderer->GetD3D10Device());

    NIASSERT(m_usLevels != 0 && 
        m_usLevels < D3D10_MAX_TEXTURE_DIMENSION_2_TO_EXP);

    unsigned int uiCompressedBitsPerBlock = 16;
    if (m_kPixelFormat.GetRendererHint() == DXGI_FORMAT_BC1_TYPELESS ||
        m_kPixelFormat.GetRendererHint() == DXGI_FORMAT_BC1_UNORM ||
        m_kPixelFormat.GetRendererHint() == DXGI_FORMAT_BC1_UNORM_SRGB ||
        m_kPixelFormat.GetRendererHint() == DXGI_FORMAT_BC4_TYPELESS ||
        m_kPixelFormat.GetRendererHint() == DXGI_FORMAT_BC4_UNORM ||
        m_kPixelFormat.GetRendererHint() == DXGI_FORMAT_BC4_SNORM ||
        m_kPixelFormat.GetRendererHint() == DXGI_FORMAT_BC5_TYPELESS ||
        m_kPixelFormat.GetRendererHint() == DXGI_FORMAT_BC5_UNORM ||
        m_kPixelFormat.GetRendererHint() == DXGI_FORMAT_BC5_SNORM)
    {
        uiCompressedBitsPerBlock = 8;
    }

    NIASSERT(m_pkD3D10Texture);
    for (i = 0; i < m_usLevels; i++)
    {
        unsigned int uiPitch = pkPixels->GetWidth(i) * 
            pkPixels->GetPixelStride();
        if (m_kPixelFormat.GetCompressed())
        {
            // pkPixels->GetPixelStride = 0 for compressed formats; need to
            // recalculate and account for non-divisible-by-four pixel counts
            uiPitch = (pkPixels->GetWidth(i) + 3) & 0xFFFFFFC;
            if (uiCompressedBitsPerBlock == 8)
                uiPitch /= 2;
        }

        pkRenderer->GetD3D10Device()->UpdateSubresource(m_pkD3D10Texture,
            i, NULL, pkPixels->GetPixels(i), uiPitch, 0);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10SourceTextureData::UpdatePalette(NiPalette* pkNewPalette)
{
    bool bPaletteExpand = false;

    // If the Pixel Data has a Palette, We need to check for:
    // 1) New palette attached to data
    // 2) Palette elements changed
    if (pkNewPalette)
    {
        unsigned int uiPaletteRevID = pkNewPalette->GetRevisionID();

        // We know that the dest is not palettized, so if the palette
        // entries have changed, then we need to re-expand the data
        if (m_uiPaletteRevID != uiPaletteRevID)
            bPaletteExpand = true;

        m_uiPaletteRevID = uiPaletteRevID;

        // We know that the dest is not palettized, so if the palette
        // itself has changed, then we need to re-expand the data
        if (m_spPalette != pkNewPalette)
        {
            m_spPalette = pkNewPalette;
            bPaletteExpand = true;
        }
    }

    return bPaletteExpand;
}
//---------------------------------------------------------------------------
