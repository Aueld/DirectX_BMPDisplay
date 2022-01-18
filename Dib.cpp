
#include "pch.h"
#include "stdafx.h"
#include "Dib.h"

/* -- Constructor of CDib Class --
	Description :
		���ο� Dib�� ����� ���� Dib ��ü�� �����
		���� �̸��� noname.bmp���� �Ѵ�.
*/

/* -- Constructor of CDib Class --
	Description :
		�־��� ���ϸ��� ������ Ŭ������ �����ϸ鼭 BMP ������ 
		�̹����� ������ �ְ� �Ѵ�.
*/
CDib::CDib(CString filename)
{
	  // ���ϸ��� m_strFileName�� ����Ѵ�.
	
	m_strFileName = filename;

	  // �̹��� �����Ϳ� ���۸� null�� �����Ѵ�.
	szDib = NULL;
	lpDib = NULL;
	  // �ȷ�Ʈ �����Ϳ� ���۸� null�� �����Ѵ�.
	szPal = NULL;
	m_palDIB = NULL;

	m_lpDibBits = NULL;

	//  // BMP ������ �ε��Ų��.
	//if (!bLoadBmpFile())
	//	AfxMessageBox(m_strMessageTemp);
}

/* -- Destructor of CDib Class --
	Description : 
		�̹����� �ȷ�Ʈ�� ���� ������ �޸𸮸� ������Ų��.
*/
CDib::~CDib()
{
	  // ���� szDib�� �޸𸮿� �����Ǿ� �ִٸ� �̰��� �����Ѵ�.
	if (szDib)
	{
		::GlobalUnlock(szDib);
		::GlobalFree(szDib);
	}

	  // ���� �ȷ�Ʈ�� �޸𸮿� �����Ǿ� �ִٸ� �̰��� �����Ѵ�.
	if(m_palDIB)
	{
		delete m_palDIB;
		m_palDIB = NULL;
	}
	if(szPal)
	{
		::GlobalUnlock(szPal);
		::GlobalFree(szPal);
	}
}

/* -- bLoadBmpFile()  --
	Description : 
		BMP ������ �ε��Ѵ�.
*/
BOOL CDib::bLoadBmpFile()
{
	  // ������ ����. 	
	CFile bmpFile( m_strFileName, CFile::modeRead );
	
	  // bmpHeader ����ü�� BMPHEADER ��� ������ �о� �´�.
	if( bmpFile.Read( (LPSTR)&bmpHeader, sizeof(bmpHeader) ) != sizeof(bmpHeader) )
	{
		sprintf_s(m_strMessageTemp, "%s BMPHEADER�� ���� �������ϴ�.", m_strFileName);
		return FALSE;
	}	

	  // ������ BMP ���� �ƴ��� Ȯ��.
	if( bmpHeader.bfType != DIB_HEADER_MARKER )
	{
		sprintf_s(m_strMessageTemp, "%s BMP ������ �ƴմϴ�.", m_strFileName);
		return FALSE;
	}

		// ����ũ�⿡�� ��� ũ�⸦ ���� Dib�� ũ�⸦ ���
	DWORD fileLength = bmpFile.GetLength();
	DWORD dibSize = fileLength - sizeof( bmpHeader );

	  // �̹��� ������ ũ�⸸ŭ �޸𸮸� �����Ѵ�.
	szDib = ::GlobalAlloc(GHND, dibSize);

		// �޸𸮰� �Ҵ�Ǿ� ���� �ʴٸ�
	if(szDib == NULL)
	{
		sprintf_s(m_strMessageTemp, "%s �޸𸮰� �����մϴ�.", m_strFileName);
		return FALSE;
	}

	  // �̹��� ������ ���۸� ��Ʈ���� �� �ֵ��� lpDib�� �����Ѵ�.
	lpDib = (LPSTR) ::GlobalLock(szDib);

 		// �̹��� �����͸� �д´�.
		// �̹��� ũ�⸸ŭ �����͸� ���� ���Ѵٸ� �޸𸮿��� �����Ѵ�.
	if( bmpFile.Read( (LPSTR)lpDib, dibSize ) != dibSize )
	{
		::GlobalUnlock(szDib);
		::GlobalFree(szDib);
		szDib = NULL;
		lpDib = NULL;
		
		sprintf_s(m_strMessageTemp, "%s ������ ���� �������ϴ�.", m_strFileName);
		return FALSE;
	}
	  
	bmpInfoHeader = (LPBITMAPINFOHEADER)lpDib;
	bmpInfo = (LPBITMAPINFO)lpDib;	  

	  // ���� ����ϴ� �÷����� �޴´�.
	m_nColorNums = bGetColorNums();

	  // �ȷ�Ʈ�� �����.
	if( !bCreatePalette() )
	{
		::GlobalUnlock(szDib);
		::GlobalFree(szDib);
		szDib = NULL;
		lpDib = NULL;
		
		return FALSE;
	}

	  // ���� �̹����� �и��Ѵ�.
	if( m_lpDibBits == NULL )
		m_lpDibBits = bFindDib();

	bmpFile.Close();

	return TRUE;
}

/* -- bCreatePalette()  --
	Description : 
		�ȷ�Ʈ�� �����.
*/
BOOL	CDib::bCreatePalette()
{
	if( GetColorNums() == 0)
		return TRUE;

	  // �ȷ�Ʈ �޸𸮸� �Ҵ��Ѵ�.
	szPal = ::GlobalAlloc(GHND, sizeof(LOGPALETTE) + sizeof(PALETTEENTRY) * m_nColorNums);
	  // �ȷ�Ʈ �޸𸮸� �Ҵ����� ���ߴٸ� �޸𸮸� �����Ѵ�.
	if(szPal == NULL)
	{
		sprintf_s(m_strMessageTemp, "%s palette �޸𸮰� �����մϴ�.", m_strFileName);
		return FALSE;
	}

	  // lpPal�� �����Ѵ�.
	lpPal = (LPLOGPALETTE) ::GlobalLock((HGLOBAL)szPal);
	lpPal->palVersion = PALVERSION;
	lpPal->palNumEntries = (WORD)m_nColorNums;

	  // �÷�����ŭ �ȷ�Ʈ�� �ȷ�Ʈ ��Ʈ���� �ִ´�.
	for(int i = 0; i < (int)m_nColorNums; i++)
	{
		lpPal->palPalEntry[i].peRed = bmpInfo->bmiColors[i].rgbRed;
		lpPal->palPalEntry[i].peGreen = bmpInfo->bmiColors[i].rgbGreen;
		lpPal->palPalEntry[i].peBlue = bmpInfo->bmiColors[i].rgbBlue;
		lpPal->palPalEntry[i].peFlags = 0;
	}	

	  // CPalette ����ü�� lpPal�� ��ũ���Ѽ� �ȷ�Ʈ�� �����.
	m_palDIB = new CPalette;
	m_palDIB->CreatePalette(lpPal);

	return TRUE;
}

/* -- bGetColorNums()  --
	Description : 
		���� ��Ʈ���� �÷����� ã�´�.
		���� ũ��� ���� ũ�⸦ m_nWidth, m_nHeight�� ����Ѵ�.
*/
int CDib::bGetColorNums()
{
	m_nWidth = bmpInfoHeader->biWidth;
	m_nHeight = bmpInfoHeader->biHeight;

	DWORD dwClrUsed;
	  // ���� �÷����� �޾Ƽ� 0�� �ƴϸ� �÷����� �����Ѵ�.
	dwClrUsed = bmpInfoHeader->biClrUsed;
	if(dwClrUsed != 0)
		return (int)dwClrUsed;
	
	WORD wBitCount;
	  // ��Ʈ ī��Ʈ�� �޴´�.
	
	wBitCount = bmpInfoHeader->biBitCount;
	
	switch(wBitCount)
	{
	case 1:
		return 2;		// mono
	case 4:
		return 16;		// 16 color
	case 8:
		return 256;		// 256 color
	default:
		return 0;		// true color
	}
}

/* -- bFindDib()  --
	Description : 
		����� ������ ��Ʈ�� ���Ͽ��� �������� �̹��� �������� �����͸� 
		ã�Ƽ� �����Ų��.
*/
LPSTR CDib::bFindDib()
{
	return ( (LPSTR)lpDib + *( (LPDWORD)lpDib ) + bPaletteSize() );
	//return ( (LPSTR)lpDib + sizeof( BITMAPINFOHEADER ) + bPaletteSize() );
}

/* -- bPaletteSize()  --
	Description : 
		�ȷ�Ʈ�� ũ�⸦ �˾ƺ���.
*/
WORD CDib::bPaletteSize()
{	
	return (WORD)(m_nColorNums * sizeof(RGBQUAD));	
}