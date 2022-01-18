// BMPDisplayView.cpp : CBMPDisplayView Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ����
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
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
// ǥ�� �μ� ����Դϴ�.
   ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
   ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
   ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CBMPDisplayView::OnFilePrintPreview)
   ON_WM_CONTEXTMENU()
   ON_WM_RBUTTONUP()
   ON_WM_KEYDOWN()
   ON_WM_SETCURSOR()
   END_MESSAGE_MAP()

// CBMPDisplayView ����/�Ҹ�

CBMPDisplayView::CBMPDisplayView()
{
   // TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

}

CBMPDisplayView::~CBMPDisplayView()
{
   if(m_pDib !=NULL)              // Bmp ���� ��ü �Ұ�
      delete m_pDib;

   if(m_pDDP !=NULL)             // DirectDrawPalette ����
      m_pDDP->Release();

   if(m_pPS != NULL)              // Primary Surface ����
      m_pPS->Release();

   if(m_pDDObj !=NULL)           // DirectDraw ��ü ����
      m_pDDObj->Release();
}

BOOL CBMPDisplayView::PreCreateWindow(CREATESTRUCT& cs)
{
   // TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
   //  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

   return CView::PreCreateWindow(cs);
}

// CBMPDisplayView �׸���

void CBMPDisplayView::OnDraw(CDC* /*pDC*/)
{
   CBMPDisplayDoc* pDoc = GetDocument();
   ASSERT_VALID(pDoc);
   if (!pDoc)
      return;

   // TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.
}

// CBMPDisplayView �μ�

void CBMPDisplayView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
   AFXPrintPreview(this);
#endif
}

BOOL CBMPDisplayView::OnPreparePrinting(CPrintInfo* pInfo)
{
   // �⺻���� �غ�
   return DoPreparePrinting(pInfo);
}

void CBMPDisplayView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
   // TODO: �μ��ϱ� ���� �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
}

void CBMPDisplayView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
   // TODO: �μ� �� ���� �۾��� �߰��մϴ�.
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

// CBMPDisplayView ����

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

// CBMPDisplayView �޽��� ó����

void CBMPDisplayView::OnInitialUpdate()
{
   CView::OnInitialUpdate();

   HRESULT result;          // Direct Draw �Լ��� ����� return ����(����: DD_OK)
   BOOL surface;            // CreateSurfaces �Լ��� ���� ������ Ȯ��

   m_DDrawOK = FALSE;       // DD ���º���
   m_pPS = NULL;            // primary surface
   m_pBB = NULL;            // back buffer
   m_pDDP = NULL;           // �ȷ�Ʈ
   m_pDib = NULL;
   m_CountBmp = 1;

   // DirectDraw7 ��ü ����
   result = DirectDrawCreateEx(NULL, (LPVOID*)&m_pDDObj, IID_IDirectDraw7, NULL);
   if (result != DD_OK)
      return;

   // �����츦 �������� ��ü ȭ����� ����
   HWND hWnd = AfxGetMainWnd()->m_hWnd;
   result = m_pDDObj->SetCooperativeLevel(hWnd, DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN);
   if (result != DD_OK )
      return;

   // ȭ���� �ػ󵵸� 1280 x 1024����, ������� 256������ ����
   result = m_pDDObj->SetDisplayMode(1280, 1024, 8, 0, 0);
   if (result != DD_OK)
      return;

   // surface ����
   surface = CreateSurfaces();
   if (!surface)
      return;

   m_DDrawOK = ClearSurface( m_pBB );       // Back Buffer�� clear(���� ȭ������ �ʱ�ȭ)
   UpdateSurface();                         // �׸��� �ҷ���

   // flipping : backbuffer�� ���� ȭ�����
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
   // �������� �� �ʱ�ȭ
   DDSURFACEDESC2 ddSD;
   DDSCAPS2 ddsC;
   BOOL surface = FALSE;
   HRESULT result;

   // DDSURFACEDESC2 ���� �ʱ�ȭ
   memset(&ddSD, 0, sizeof(DDSURFACEDESC2));
   ddSD.dwSize = sizeof(DDSURFACEDESC2);
   ddSD.dwFlags = DDSD_CAPS|DDSD_BACKBUFFERCOUNT;
   ddSD.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE|DDSCAPS_FLIP|DDSCAPS_COMPLEX;
   ddSD.dwBackBufferCount = 1;

   // primary surface�� backbuffer ����
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
   // �������� �� �ʱ�ȭ
   DDSURFACEDESC2 ddSD;
   BOOL surface = FALSE;
   BOOL exitDo = FALSE;
   HRESULT result;

   memset( &ddSD, 0, sizeof(DDSURFACEDESC2));
   ddSD.dwSize = sizeof(DDSURFACEDESC2);

   // ���� surface �޸� ������ ���
   do
   {
      result=pSurface->Lock(NULL, &ddSD, DDLOCK_SURFACEMEMORYPTR, NULL);
      if (result == DDERR_SURFACELOST)
         m_pPS->Restore();
      else if(result != DDERR_WASSTILLDRAWING)
         exitDo = TRUE;
   }while (!exitDo);

   // ��� �� ���� �� unlock()�� �̿��� ��� ����
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
   DDBLTFX ddbltfx;                     // ȭ�� ����� ���� ����

   ddbltfx.dwSize = sizeof(ddbltfx);
   ddbltfx.dwFillColor = 0;             // ȭ�� ����� ä�� ���� ���� (����)

   // ȭ���� �˰� ����
   m_pBB->Blt(NULL,NULL,NULL,DDBLT_COLORFILL|DDBLT_WAIT, &ddbltfx);

   if ( m_CountBmp > 0 )
   {
      CString temp;
      temp.Format ("test%d.bmp",m_CountBmp);

      m_pDib = new CDib(temp);                          // CDib ��ü ����
      m_pDDP = CreateDDPalette( m_pDDObj, m_pDib );     // �ȷ�Ʈ ����
      if ( m_pDDP != NULL )
      {
         HRESULT result;
         result= m_pPS->SetPalette( m_pDDP );           // �ȷ�Ʈ ����
         if ( result == DD_OK )
            m_DDrawOK = TRUE;
      }
      LoadBMP( m_pBB, m_pDib );             // �ε� �� �̹����� backbuffer�� ����
   }
   m_CountBmp = m_CountBmp%3 + 1;           // ���� �׸��� ���� ���� ����
}

LPDIRECTDRAWPALETTE CBMPDisplayView::CreateDDPalette(LPDIRECTDRAW7 pDDObj, CDib* cdib)
{
   // ���� ����
   LPDIRECTDRAWPALETTE pDDP;                // DD���� ���Ǵ� �ȷ�Ʈ�� ������
   PALETTEENTRY pe[256];                    // ���������� �ּ� ������ ��Ÿ���� ����ü
   HRESULT result;

   if ( cdib->GetColorNums() != 256 )       // �̹����� �÷� �� Ȯ��
   {
      pDDP = NULL;
      return pDDP;
   }

   LPRGBQUAD pBmpColor = (LPRGBQUAD) cdib->GetDibRGBTable();        // �ȷ�Ʈ ���
   for ( int i = 0; i < 256 ; i++ )
   {
      pe[i].peRed  = pBmpColor[i].rgbRed ;
      pe[i].peGreen = pBmpColor[i].rgbGreen ;
      pe[i].peBlue = pBmpColor[i].rgbBlue ;
   }

   result = pDDObj->CreatePalette(DDPCAPS_8BIT, pe, &pDDP, NULL);   // �ȷ�Ʈ ����
   if ( result != DD_OK )
      pDDP = NULL;

   return pDDP;
}

void CBMPDisplayView::LoadBMP(LPDIRECTDRAWSURFACE7 pSurface, CDib* cdib)
{
   DDSURFACEDESC2 ddSD;    // surface�� ������ �� �� �ִ� ����ü ����
   LPSTR pSB;
   LPSTR pDB;
   BOOL exitDo = FALSE;
   HRESULT result;
      
   ddSD.dwSize = sizeof(DDSURFACEDESC2);

   do                      // surface�� ���
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

      pSB = (LPSTR) ddSD.lpSurface;     // backbuffer�� ���� �������� ��ġ
      pDB = cdib->GetDibBits();         // bitmap ������ �̹��� ������ �κ��� ����Ŵ

      Bmp_width = cdib->GetWidth();
      Bmp_height = cdib->GetHeight();

      if ( (Bmp_width % 4) != 0 )               // bmp ������ width�� �׻� 4�� ���
         Bmp_width += 4 - ( Bmp_width % 4 );    // 4�� ����� �ƴϸ� 4�� ����� ����

      pDB += (Bmp_height - 1 ) * Bmp_width;
      // �׸��� ��ġ�� ����� ��ġ��Ŵ
	    
      //pSB += (ddSD.dwWidth-Bmp_width)/2 + (ddSD.dwWidth)*((ddSD.dwHeight-Bmp_height)/2);
	  pSB += (ddSD.dwWidth-Bmp_width)/2 + (ddSD.lPitch)*((ddSD.dwHeight-Bmp_height)/2);
	 
      for ( UINT i = 0 ; i < Bmp_height ; i++ ) // �̹��� �ε�
      {
         memcpy( pSB, pDB, Bmp_width);
         //pSB += (ddSD.dwWidth);
		 pSB += (ddSD.lPitch);
         pDB -= Bmp_width;
      }      
      pSurface->Unlock(NULL);                   // surface ��� ����
   }
}

void CBMPDisplayView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   // TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

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
   // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

   SetCursor(NULL);
   return TRUE;
   //CView::OnSetCursor(pWnd, nHitTest, message);
}