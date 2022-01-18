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
	
	// BMP ������ ���� ũ�⸦ ��´�.
	int   GetWidth() { return m_nWidth; };
	  // BMP ������ ���� ũ�⸦ ��´�.
	int   GetHeight() { return m_nHeight; };
	  // BMP ������ ������� ��´�.
	int   GetColorNums() { return m_nColorNums; };
	  // DIB�� �̹��� ũ�⸦ ��´�.
	int   GetImageSize() { return ( m_nWidth * m_nHeight ); };
	  // �̹��� �����Ϳ� ���� �����͸� ��´�.
	LPSTR GetDibBits() { return m_lpDibBits; };
	  // DIB�� BITMAPINFOHEADER�� ���� �����͸� ��´�.
	LPSTR GetDibInfoHeader() { return ( (LPSTR)bmpInfoHeader ); };
	  // DIB�� BITMAPINFO�� ���� �����͸� ��´�.
	LPSTR GetDibInfo() { return ( (LPSTR)bmpInfo ); };
	  // DIB�� Palette�� ���� �����͸� ��´�.
	LPSTR GetDibRGBTable()
	{	return ( (LPSTR)(lpDib + sizeof( BITMAPINFOHEADER )) ); };
	
	// -- Attribute --

protected:
	int  m_nWidth;								// BMP �� ���� ũ��
	int  m_nHeight;								// BMP �� ���� ũ��
	int  m_nColorNums;							// ���� BMP�� �÷���	
	char m_strMessageTemp[DEFAULTLEN];			// �����޽����� �ݴ� buffer
	CPalette* m_palDIB;							// �ȷ�Ʈ
	CString m_strFileName;						// BMP ���ϸ�
	LPSTR	m_lpDibBits;						// ���� �̹����� ���� ������	
	
	BITMAPFILEHEADER bmpHeader;					// BMP ������ ���
	LPBITMAPINFOHEADER bmpInfoHeader;			// BMP ���� ���	
	LPBITMAPINFO bmpInfo;	
 
	HANDLE szPal;		// �ȷ�Ʈ�� �޸𸮿� �ø��� ���� �޸� �ڵ�
	  
	LPLOGPALETTE lpPal; // �ȷ�Ʈ ����ü
	  
	HANDLE szDib;		// BMP �����͸� ������ �޸� �ڵ�
	  
	LPSTR lpDib;		// BMP ������
	
	// -- Member function --  	
	int   bLoadBmpFile();						// BMP ������ �д´�.
	BOOL  bCreatePalette();						// �ȷ�Ʈ�� �����.
	int   bGetColorNums();						// �÷����� ��´�.
	LPSTR bFindDib();							// �����Ϳ��� ��Ʈ�� �����͸� ã�´�.
	WORD  bPaletteSize();						// �ȷ�Ʈ ũ�⸦ ��´�.
};

// CDib Ŭ����

#endif