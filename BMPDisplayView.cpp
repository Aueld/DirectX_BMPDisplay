// BMPDisplayView.cpp : CBMPDisplayView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "BMPDisplay.h"
#endif

#include "BMPDisplayDoc.h"
#include "BMPDisplayView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CBMPDisplayView

IMPLEMENT_DYNCREATE(CBMPDisplayView, CView)

BEGIN_MESSAGE_MAP(CBMPDisplayView, CView)
// 표준 인쇄 명령입니다.
   ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
   ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
   ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CBMPDisplayView::OnFilePrintPreview)
   ON_WM_CONTEXTMENU()
   ON_WM_RBUTTONUP()
   ON_WM_KEYDOWN()
   ON_WM_SETCURSOR()
   END_MESSAGE_MAP()

// CBMPDisplayView 생성/소멸

CBMPDisplayView::CBMPDisplayView()
{
   // TODO: 여기에 생성 코드를 추가합니다.

}

CBMPDisplayView::~CBMPDisplayView()
{
   if(m_pDib !=NULL)              // Bmp 파일 객체 소거
      delete m_pDib;

   if(m_pDDP !=NULL)             // DirectDrawPalette 해제
      m_pDDP->Release();

   if(m_pPS != NULL)              // Primary Surface 해제
      m_pPS->Release();

   if(m_pDDObj !=NULL)           // DirectDraw 객체 해제
      m_pDDObj->Release();
}

BOOL CBMPDisplayView::PreCreateWindow(CREATESTRUCT& cs)
{
   // TODO: CREATESTRUCT cs를 수정하여 여기에서
   //  Window 클래스 또는 스타일을 수정합니다.

   return CView::PreCreateWindow(cs);
}

// CBMPDisplayView 그리기

void CBMPDisplayView::OnDraw(CDC* /*pDC*/)
{
   CBMPDisplayDoc* pDoc = GetDocument();
   ASSERT_VALID(pDoc);
   if (!pDoc)
      return;

   // TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}

// CBMPDisplayView 인쇄

void CBMPDisplayView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
   AFXPrintPreview(this);
#endif
}

BOOL CBMPDisplayView::OnPreparePrinting(CPrintInfo* pInfo)
{
   // 기본적인 준비
   return DoPreparePrinting(pInfo);
}

void CBMPDisplayView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
   // TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CBMPDisplayView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
   // TODO: 인쇄 후 정리 작업을 추가합니다.
}

void CBMPDisplayView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
   ClientToScreen(&point);
   OnContextMenu(this, point);
}

void CBMPDisplayView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
   theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}

// CBMPDisplayView 진단

#ifdef _DEBUG
void CBMPDisplayView::AssertValid() const
{
   CView::AssertValid();
}

void CBMPDisplayView::Dump(CDumpContext& dc) const
{
   CView::Dump(dc);
}

CBMPDisplayDoc* CBMPDisplayView::GetDocument() const
{
   ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CBMPDisplayDoc)));
   return (CBMPDisplayDoc*)m_pDocument;
}
#endif //_DEBUG

// CBMPDisplayView 메시지 처리기

void CBMPDisplayView::OnInitialUpdate()
{
   CView::OnInitialUpdate();

   HRESULT result;          // Direct Draw 함수의 결과를 return 받음(성공: DD_OK)
   BOOL surface;            // CreateSurfaces 함수의 실행 성공을 확인

   m_DDrawOK = FALSE;       // DD 상태변수
   m_pPS = NULL;            // primary surface
   m_pBB = NULL;            // back buffer
   m_pDDP = NULL;           // 팔레트
   m_pDib = NULL;
   m_CountBmp = 1;

   // DirectDraw7 객체 생성
   result = DirectDrawCreateEx(NULL, (LPVOID*)&m_pDDObj, IID_IDirectDraw7, NULL);
   if (result != DD_OK)
      return;

   // 윈도우를 독점모드와 전체 화면모드로 설정
   HWND hWnd = AfxGetMainWnd()->m_hWnd;
   result = m_pDDObj->SetCooperativeLevel(hWnd, DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN);
   if (result != DD_OK )
      return;

   // 화면의 해상도를 1280 x 1024으로, 색상수는 256색으로 설정
   result = m_pDDObj->SetDisplayMode(1280, 1024, 8, 0, 0);
   if (result != DD_OK)
      return;

   // surface 생성
   surface = CreateSurfaces();
   if (!surface)
      return;

   m_DDrawOK = ClearSurface( m_pBB );       // Back Buffer를 clear(검은 화면으로 초기화)
   UpdateSurface();                         // 그림을 불러옴

   // flipping : backbuffer의 내용 화면출력
   BOOL exitDo = FALSE;
   do
   {
      result = m_pPS->Flip(NULL, DDFLIP_WAIT);
      if (result == DDERR_SURFACELOST)
         m_pPS->Restore();
      else if (result != DDERR_WASSTILLDRAWING)
         exitDo = TRUE;
   }while(!exitDo);
}

BOOL CBMPDisplayView::CreateSurfaces(void)
{
   // 변수선언 및 초기화
   DDSURFACEDESC2 ddSD;
   DDSCAPS2 ddsC;
   BOOL surface = FALSE;
   HRESULT result;

   // DDSURFACEDESC2 구조 초기화
   memset(&ddSD, 0, sizeof(DDSURFACEDESC2));
   ddSD.dwSize = sizeof(DDSURFACEDESC2);
   ddSD.dwFlags = DDSD_CAPS|DDSD_BACKBUFFERCOUNT;
   ddSD.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE|DDSCAPS_FLIP|DDSCAPS_COMPLEX;
   ddSD.dwBackBufferCount = 1;

   // primary surface와 backbuffer 생성
   result = m_pDDObj->CreateSurface( &ddSD, &m_pPS, NULL );
   if (result == DD_OK)
   {
      ZeroMemory(&ddsC, sizeof(ddsC));

      ddsC.dwCaps = DDSCAPS_BACKBUFFER;
      result = m_pPS->GetAttachedSurface(&ddsC, &m_pBB);
      if (result == DD_OK)
         surface = TRUE;
   }
   return surface;
}

BOOL CBMPDisplayView::ClearSurface(LPDIRECTDRAWSURFACE7 pSurface)
{
   // 변수선언 및 초기화
   DDSURFACEDESC2 ddSD;
   BOOL surface = FALSE;
   BOOL exitDo = FALSE;
   HRESULT result;

   memset( &ddSD, 0, sizeof(DDSURFACEDESC2));
   ddSD.dwSize = sizeof(DDSURFACEDESC2);

   // 지울 surface 메모리 영역을 잠금
   do
   {
      result=pSurface->Lock(NULL, &ddSD, DDLOCK_SURFACEMEMORYPTR, NULL);
      if (result == DDERR_SURFACELOST)
         m_pPS->Restore();
      else if(result != DDERR_WASSTILLDRAWING)
         exitDo = TRUE;
   }while (!exitDo);

   // 잠근 면 삭제 및 unlock()을 이용한 잠금 해제
   if(result == DD_OK)
   {
      UINT S_width, S_height;

      S_width = ddSD.lPitch;
      S_height = ddSD.dwHeight;

      char* buf = (char*)ddSD.lpSurface;

      memset(buf, 0, S_width * S_height );

      pSurface->Unlock(NULL);
      surface = TRUE;
   }
   return surface;
}

void CBMPDisplayView::UpdateSurface(void)
{
   // backbuffer clear
   DDBLTFX ddbltfx;                     // 화면 배경의 색상 지정

   ddbltfx.dwSize = sizeof(ddbltfx);
   ddbltfx.dwFillColor = 0;             // 화면 배경을 채울 색상 결정 (검정)

   // 화면을 검게 만듦
   m_pBB->Blt(NULL,NULL,NULL,DDBLT_COLORFILL|DDBLT_WAIT, &ddbltfx);

   if ( m_CountBmp > 0 )
   {
      CString temp;
      temp.Format ("test%d.bmp",m_CountBmp);

      m_pDib = new CDib(temp);                          // CDib 객체 생성
      m_pDDP = CreateDDPalette( m_pDDObj, m_pDib );     // 팔레트 생성
      if ( m_pDDP != NULL )
      {
         HRESULT result;
         result= m_pPS->SetPalette( m_pDDP );           // 팔레트 설정
         if ( result == DD_OK )
            m_DDrawOK = TRUE;
      }
      LoadBMP( m_pBB, m_pDib );             // 로드 된 이미지를 backbuffer에 전송
   }
   m_CountBmp = m_CountBmp%3 + 1;           // 다음 그림을 위한 변수 증가
}

LPDIRECTDRAWPALETTE CBMPDisplayView::CreateDDPalette(LPDIRECTDRAW7 pDDObj, CDib* cdib)
{
   // 변수 선언
   LPDIRECTDRAWPALETTE pDDP;                // DD에서 사용되는 팔레트의 포인터
   PALETTEENTRY pe[256];                    // 색상정보의 최소 단위를 나타내는 구조체
   HRESULT result;

   if ( cdib->GetColorNums() != 256 )       // 이미지의 컬러 수 확인
   {
      pDDP = NULL;
      return pDDP;
   }

   LPRGBQUAD pBmpColor = (LPRGBQUAD) cdib->GetDibRGBTable();        // 팔레트 얻기
   for ( int i = 0; i < 256 ; i++ )
   {
      pe[i].peRed  = pBmpColor[i].rgbRed ;
      pe[i].peGreen = pBmpColor[i].rgbGreen ;
      pe[i].peBlue = pBmpColor[i].rgbBlue ;
   }

   result = pDDObj->CreatePalette(DDPCAPS_8BIT, pe, &pDDP, NULL);   // 팔레트 생성
   if ( result != DD_OK )
      pDDP = NULL;

   return pDDP;
}

void CBMPDisplayView::LoadBMP(LPDIRECTDRAWSURFACE7 pSurface, CDib* cdib)
{
   DDSURFACEDESC2 ddSD;    // surface의 정보를 알 수 있는 구조체 변수
   LPSTR pSB;
   LPSTR pDB;
   BOOL exitDo = FALSE;
   HRESULT result;
      
   ddSD.dwSize = sizeof(DDSURFACEDESC2);

   do                      // surface를 잠금
   {
      result=pSurface->Lock(NULL,&ddSD, DDLOCK_SURFACEMEMORYPTR, NULL);
      if (result == DDERR_SURFACELOST)
      {
         m_pPS->Restore();
         ClearSurface( m_pBB );
      }
      else if(result != DDERR_WASSTILLDRAWING)
         exitDo = TRUE;

   }while (!exitDo);

   if(result == DD_OK)
   {
      UINT Bmp_width, Bmp_height;

      pSB = (LPSTR) ddSD.lpSurface;     // backbuffer의 실제 물리적인 위치
      pDB = cdib->GetDibBits();         // bitmap 파일의 이미지 데이터 부분을 가리킴

      Bmp_width = cdib->GetWidth();
      Bmp_height = cdib->GetHeight();

      if ( (Bmp_width % 4) != 0 )               // bmp 파일의 width는 항상 4의 배수
         Bmp_width += 4 - ( Bmp_width % 4 );    // 4의 배수가 아니면 4의 배수로 맞춤

      pDB += (Bmp_height - 1 ) * Bmp_width;
      // 그림의 위치를 가운데에 위치시킴
	    
      //pSB += (ddSD.dwWidth-Bmp_width)/2 + (ddSD.dwWidth)*((ddSD.dwHeight-Bmp_height)/2);
	  pSB += (ddSD.dwWidth-Bmp_width)/2 + (ddSD.lPitch)*((ddSD.dwHeight-Bmp_height)/2);
	 
      for ( UINT i = 0 ; i < Bmp_height ; i++ ) // 이미지 로드
      {
         memcpy( pSB, pDB, Bmp_width);
         //pSB += (ddSD.dwWidth);
		 pSB += (ddSD.lPitch);
         pDB -= Bmp_width;
      }      
      pSurface->Unlock(NULL);                   // surface 잠금 해제
   }
}

void CBMPDisplayView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   // TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

   if ((nChar == VK_SPACE) && (m_DDrawOK))
   {
      HRESULT result;
      BOOL exitDo = FALSE;

      UpdateSurface();
      do
      {
         result = m_pPS->Flip(NULL, DDFLIP_WAIT);
         if (result == DDERR_SURFACELOST)
            m_pPS->Restore();
         else if (result != DDERR_WASSTILLDRAWING)
            exitDo = TRUE;
      }while(!exitDo);
   }
   else if ( nChar == VK_ESCAPE )
   {
      GetParentFrame()->PostMessage(WM_CLOSE);
   }

   CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CBMPDisplayView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
   // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

   SetCursor(NULL);
   return TRUE;
   //CView::OnSetCursor(pWnd, nHitTest, message);
}