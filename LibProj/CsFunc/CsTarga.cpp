
#include "stdafx.h"
#include "CsTarga.h"

void CsTarga::DeleteTgaData( sCS_TARGADATA* pData )
{
	assert_cs( pData != NULL );
	memset( &pData->s_Header, 0, sizeof( TARGAFILEHEADER ) );
	SAFE_DELETE_ARRAY( pData->s_pData );
}

bool CsTarga::GetSize( sCS_TARGADATA* pOutData, LPCTSTR szPath )
{
	HANDLE hFile = CreateFile( szPath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if( hFile == INVALID_HANDLE_VALUE )
		return false;

	DWORD dwFileSize = GetFileSize( hFile, NULL );
	DWORD dwRead;
	BYTE* pFileData = csnew BYTE[ dwFileSize ];
	ReadFile( hFile, pFileData, dwFileSize, &dwRead, NULL );
	CloseHandle( hFile );

	DWORD dwOffSet = 0;
	memcpy( &pOutData->s_Header, &pFileData[ dwOffSet ], sizeof( TARGAFILEHEADER ) );
	return true;
}

bool CsTarga::CreateTgaData( sCS_TARGADATA* pOutData, int nWidth, int nHeight, int nBitCount, DWORD dwInitColor )
{
	if( ( nWidth <= 0 )||( nHeight <= 0 ) )
	{
		assert_csm( false, _T( "Um lado do TGA deve ser maior que zero." ) );
		return false;
	}
	/*if( ( nWidth % 4 != 0 )||( nHeight % 4 != 0 ) )
	{
		assert_csm( false, _T( "Tga의 한변은 4의 배수여야만 합니다." ) );
		return false;
	}*/
	if( ( nBitCount != 24 )&&( nBitCount != 32 ) )
	{
		assert_csm( false, _T( "A contagem de bits da TGA deve ter 24 ou 32." ) );
		return false;
	}
	if( pOutData == NULL )
	{
		assert_csm( false, _T( "O conteúdo de Poutdata deve existir." ) );
		return false;
	}
	if( pOutData->s_pData != NULL )
	{
		assert_csm( false, _T( "Já existe um conteúdo de dados do Out Out." ) );
		return false;
	}

	int nByte = nBitCount/8;
	// Alocação de memória de dados
	pOutData->s_pData = csnew BYTE[ nWidth*nHeight*nByte ];
	
	// Taga cabeçalho
	memset( &pOutData->s_Header, 0, sizeof( TARGAFILEHEADER ) );
	pOutData->s_Header.imageTypeCode = 2;
	pOutData->s_Header.imageWidth = nWidth;
	pOutData->s_Header.imageHeight = nHeight;
	pOutData->s_Header.bitCount = nBitCount;
	if( pOutData->s_Header.bitCount == 32 )
	{
		pOutData->s_Header.imageDescriptor = 0x08;
	}

	BYTE alpha = CS_GetAValue( dwInitColor );
	BYTE red = CS_GetRValue( dwInitColor );
	BYTE green = CS_GetGValue( dwInitColor );
	BYTE blue = CS_GetBValue( dwInitColor );

	// Configuração de valor inicial de dados
	for( int y=0; y<nHeight; ++y )
	{
		for( int x=0; x<nWidth; ++x )
		{
			pOutData->s_pData[ ( y*nWidth + x )*nByte + 0 ] = blue;			// b
			pOutData->s_pData[ ( y*nWidth + x )*nByte + 1 ] = green;		// g
			pOutData->s_pData[ ( y*nWidth + x )*nByte + 2 ] = red;			// r
			if( pOutData->s_Header.bitCount == 32 )
			{
				pOutData->s_pData[ ( y*nWidth + x )*nByte + 3 ] = alpha;	// a
			}
		}
	}
	return true;
}

bool CsTarga::CreateTgaData( sCS_TARGADATA* pOutData, TCHAR* szFileName )
{
	HANDLE hFile = CreateFile( szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if( hFile == NULL )
		return false;

	DWORD dwFileSize = GetFileSize( hFile, NULL );
	DWORD dwRead;
	BYTE* pFileData = csnew BYTE[ dwFileSize ];
	ReadFile( hFile, pFileData, dwFileSize, &dwRead, NULL );
	CloseHandle( hFile );

	DWORD dwOffSet = 0;
	memcpy( &pOutData->s_Header, &pFileData[ dwOffSet ], sizeof( TARGAFILEHEADER ) );
	dwOffSet += sizeof( TARGAFILEHEADER );

	// Alocação de memória de dados
	DWORD dwSize = pOutData->s_Header.imageWidth*pOutData->s_Header.imageHeight*(pOutData->s_Header.bitCount/8);
	pOutData->s_pData = csnew BYTE[ dwSize ];
	memcpy( pOutData->s_pData, &pFileData[ dwOffSet ], sizeof( BYTE )*dwSize );

	return true;
}

void CsTarga::CreateFileFromTgaData( LPCTSTR szFilePath, sCS_TARGADATA* pData )
{
	assert_cs( pData != NULL );

	FILE* fp = NULL;
	_tfopen_s( &fp, szFilePath, _T("wb") );
	fwrite( &pData->s_Header, sizeof( TARGAFILEHEADER ), 1, fp );
	fwrite( pData->s_pData, sizeof( BYTE ), pData->s_Header.imageWidth*pData->s_Header.imageHeight*(pData->s_Header.bitCount/8), fp );
	fclose( fp );
}