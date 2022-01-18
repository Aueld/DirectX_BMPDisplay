#ifndef _DIB_H
#define _DIB_H

#define DEFAULTLEN 80
#define DIB_HEADER_MARKER ((WORD)('M'<<8 | 'B'))
#define PALVERSION	0x300
#define	IS_WIN30_DIB(lpbi) ((*(LPDWORD)(lpbi) == sizeof(BITMAPINFOHEADER)))

class CDib 
{
public:	
	CDib(CString filename);
	~CDib();
	
	// BMP 파일의 가로 크기를 얻는다.
	int   GetWidth() { return m_nWidth; };
	  // BMP 파일의 세로 크기를 얻는다.
	int   GetHeight() { return m_nHeight; };
	  // BMP 파일의 색상수를 얻는다.
	int   GetColorNums() { return m_nColorNums; };
	  // DIB의 이미지 크기를 얻는다.
	int   GetImageSize() { return ( m_nWidth * m_nHeight ); };
	  // 이미지 데이터에 대한 포인터를 얻는다.
	LPSTR GetDibBits() { return m_lpDibBits; };
	  // DIB의 BITMAPINFOHEADER에 대한 포인터를 얻는다.
	LPSTR GetDibInfoHeader() { return ( (LPSTR)bmpInfoHeader ); };
	  // DIB의 BITMAPINFO에 대한 포인터를 얻는다.
	LPSTR GetDibInfo() { return ( (LPSTR)bmpInfo ); };
	  // DIB의 Palette에 대한 포인터를 얻는다.
	LPSTR GetDibRGBTable()
	{	return ( (LPSTR)(lpDib + sizeof( BITMAPINFOHEADER )) ); };
	
	// -- Attribute --

protected:
	int  m_nWidth;								// BMP 의 가로 크기
	int  m_nHeight;								// BMP 의 세로 크기
	int  m_nColorNums;							// 현재 BMP의 컬러수	
	char m_strMessageTemp[DEFAULTLEN];			// 에러메시지를 닫는 buffer
	CPalette* m_palDIB;							// 팔레트
	CString m_strFileName;						// BMP 파일명
	LPSTR	m_lpDibBits;						// 실제 이미지에 대한 포인터	
	
	BITMAPFILEHEADER bmpHeader;					// BMP 파일의 헤더
	LPBITMAPINFOHEADER bmpInfoHeader;			// BMP 정보 헤더	
	LPBITMAPINFO bmpInfo;	
 
	HANDLE szPal;		// 팔레트를 메모리에 올리기 위한 메모리 핸들
	  
	LPLOGPALETTE lpPal; // 팔레트 구조체
	  
	HANDLE szDib;		// BMP 데이터를 저장할 메모리 핸들
	  
	LPSTR lpDib;		// BMP 데이터
	
	// -- Member function --  	
	int   bLoadBmpFile();						// BMP 파일을 읽는다.
	BOOL  bCreatePalette();						// 팔레트를 만든다.
	int   bGetColorNums();						// 컬러수를 얻는다.
	LPSTR bFindDib();							// 데이터에서 비트맵 데이터만 찾는다.
	WORD  bPaletteSize();						// 팔레트 크기를 얻는다.
};

// CDib 클래스

#endif