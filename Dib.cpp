
#include "pch.h"
#include "stdafx.h"
#include "Dib.h"

/* -- Constructor of CDib Class --
	Description :
		새로운 Dib를 만들기 위해 Dib 객체를 만든다
		파일 이름은 noname.bmp으로 한다.
*/

/* -- Constructor of CDib Class --
	Description :
		주어진 파일명을 가지고 클래스를 생성하면서 BMP 파일의 
		이미지를 가지고 있게 한다.
*/
CDib::CDib(CString filename)
{
	  // 파일명을 m_strFileName에 기록한다.
	
	m_strFileName = filename;

	  // 이미지 포인터와 버퍼를 null로 설정한다.
	szDib = NULL;
	lpDib = NULL;
	  // 팔레트 포인터와 버퍼를 null로 설정한다.
	szPal = NULL;
	m_palDIB = NULL;

	m_lpDibBits = NULL;

	//  // BMP 파일을 로드시킨다.
	//if (!bLoadBmpFile())
	//	AfxMessageBox(m_strMessageTemp);
}

/* -- Destructor of CDib Class --
	Description : 
		이미지와 팔레트를 위해 설정된 메모리를 해제시킨다.
*/
CDib::~CDib()
{
	  // 현재 szDib가 메모리에 설정되어 있다면 이것을 해제한다.
	if (szDib)
	{
		::GlobalUnlock(szDib);
		::GlobalFree(szDib);
	}

	  // 현재 팔레트가 메모리에 설정되어 있다면 이것을 해제한다.
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
		BMP 파일을 로드한다.
*/
BOOL CDib::bLoadBmpFile()
{
	  // 파일을 연다. 	
	CFile bmpFile( m_strFileName, CFile::modeRead );
	
	  // bmpHeader 구조체에 BMPHEADER 헤더 정보를 읽어 온다.
	if( bmpFile.Read( (LPSTR)&bmpHeader, sizeof(bmpHeader) ) != sizeof(bmpHeader) )
	{
		sprintf_s(m_strMessageTemp, "%s BMPHEADER를 읽을 수없습니다.", m_strFileName);
		return FALSE;
	}	

	  // 파일이 BMP 인지 아닌지 확인.
	if( bmpHeader.bfType != DIB_HEADER_MARKER )
	{
		sprintf_s(m_strMessageTemp, "%s BMP 파일이 아닙니다.", m_strFileName);
		return FALSE;
	}

		// 파일크기에서 헤더 크기를 빼서 Dib의 크기를 계산
	DWORD fileLength = bmpFile.GetLength();
	DWORD dibSize = fileLength - sizeof( bmpHeader );

	  // 이미지 데이터 크기만큼 메모리를 설정한다.
	szDib = ::GlobalAlloc(GHND, dibSize);

		// 메모리가 할당되어 있지 않다면
	if(szDib == NULL)
	{
		sprintf_s(m_strMessageTemp, "%s 메모리가 부족합니다.", m_strFileName);
		return FALSE;
	}

	  // 이미지 데이터 버퍼를 컨트롤할 수 있도록 lpDib에 설정한다.
	lpDib = (LPSTR) ::GlobalLock(szDib);

 		// 이미지 데이터를 읽는다.
		// 이미지 크기만큼 데이터를 읽지 못한다면 메모리에서 해제한다.
	if( bmpFile.Read( (LPSTR)lpDib, dibSize ) != dibSize )
	{
		::GlobalUnlock(szDib);
		::GlobalFree(szDib);
		szDib = NULL;
		lpDib = NULL;
		
		sprintf_s(m_strMessageTemp, "%s 파일을 읽을 수없습니다.", m_strFileName);
		return FALSE;
	}
	  
	bmpInfoHeader = (LPBITMAPINFOHEADER)lpDib;
	bmpInfo = (LPBITMAPINFO)lpDib;	  

	  // 현재 사용하는 컬러수를 받는다.
	m_nColorNums = bGetColorNums();

	  // 팔레트를 만든다.
	if( !bCreatePalette() )
	{
		::GlobalUnlock(szDib);
		::GlobalFree(szDib);
		szDib = NULL;
		lpDib = NULL;
		
		return FALSE;
	}

	  // 실질 이미지를 분리한다.
	if( m_lpDibBits == NULL )
		m_lpDibBits = bFindDib();

	bmpFile.Close();

	return TRUE;
}

/* -- bCreatePalette()  --
	Description : 
		팔레트를 만든다.
*/
BOOL	CDib::bCreatePalette()
{
	if( GetColorNums() == 0)
		return TRUE;

	  // 팔레트 메모리를 할당한다.
	szPal = ::GlobalAlloc(GHND, sizeof(LOGPALETTE) + sizeof(PALETTEENTRY) * m_nColorNums);
	  // 팔레트 메모리를 할당하지 못했다면 메모리를 해제한다.
	if(szPal == NULL)
	{
		sprintf_s(m_strMessageTemp, "%s palette 메모리가 부족합니다.", m_strFileName);
		return FALSE;
	}

	  // lpPal을 설정한다.
	lpPal = (LPLOGPALETTE) ::GlobalLock((HGLOBAL)szPal);
	lpPal->palVersion = PALVERSION;
	lpPal->palNumEntries = (WORD)m_nColorNums;

	  // 컬러수만큼 팔레트를 팔레트 엔트리에 넣는다.
	for(int i = 0; i < (int)m_nColorNums; i++)
	{
		lpPal->palPalEntry[i].peRed = bmpInfo->bmiColors[i].rgbRed;
		lpPal->palPalEntry[i].peGreen = bmpInfo->bmiColors[i].rgbGreen;
		lpPal->palPalEntry[i].peBlue = bmpInfo->bmiColors[i].rgbBlue;
		lpPal->palPalEntry[i].peFlags = 0;
	}	

	  // CPalette 구조체에 lpPal을 링크시켜서 팔레트를 만든다.
	m_palDIB = new CPalette;
	m_palDIB->CreatePalette(lpPal);

	return TRUE;
}

/* -- bGetColorNums()  --
	Description : 
		현재 비트맵의 컬러수를 찾는다.
		가로 크기와 세로 크기를 m_nWidth, m_nHeight에 기록한다.
*/
int CDib::bGetColorNums()
{
	m_nWidth = bmpInfoHeader->biWidth;
	m_nHeight = bmpInfoHeader->biHeight;

	DWORD dwClrUsed;
	  // 사용된 컬러수를 받아서 0이 아니면 컬러수를 설정한다.
	dwClrUsed = bmpInfoHeader->biClrUsed;
	if(dwClrUsed != 0)
		return (int)dwClrUsed;
	
	WORD wBitCount;
	  // 비트 카운트를 받는다.
	
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
		헤더를 포함한 비트맵 파일에서 실질적인 이미지 데이터의 포인터를 
		찾아서 연결시킨다.
*/
LPSTR CDib::bFindDib()
{
	return ( (LPSTR)lpDib + *( (LPDWORD)lpDib ) + bPaletteSize() );
	//return ( (LPSTR)lpDib + sizeof( BITMAPINFOHEADER ) + bPaletteSize() );
}

/* -- bPaletteSize()  --
	Description : 
		팔레트의 크기를 알아본다.
*/
WORD CDib::bPaletteSize()
{	
	return (WORD)(m_nColorNums * sizeof(RGBQUAD));	
}