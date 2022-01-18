// BMPDisplayView.h : CBMPDisplayView 클래스의 인터페이스
//

#pragma once

#include <ddraw.h>
#include "Dib.h"

class CBMPDisplayView : public CView
{
protected: // serialization에서만 만들어집니다.
   CBMPDisplayView();
   DECLARE_DYNCREATE(CBMPDisplayView)

// 특성입니다.
public:
   CBMPDisplayDoc* GetDocument() const;   
   LPDIRECTDRAW7 m_pDDObj;
   LPDIRECTDRAWSURFACE7 m_pPS;
   LPDIRECTDRAWSURFACE7 m_pBB;
   BOOL m_DDrawOK;
   CDib* m_pDib;
   LPDIRECTDRAWPALETTE m_pDDP;
   int m_CountBmp;

// 작업입니다.
public:
   BOOL CreateSurfaces(void);
   BOOL ClearSurface(LPDIRECTDRAWSURFACE7 pSurface);
   void UpdateSurface(void);
   LPDIRECTDRAWPALETTE CreateDDPalette(LPDIRECTDRAW7 pDDObj, CDib* cdib);
   void LoadBMP(LPDIRECTDRAWSURFACE7 pSurface, CDib* cdib);

// 재정의입니다.
public:
   virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
   virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
   virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
   virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
   virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 구현입니다.
public:
   virtual ~CBMPDisplayView();
#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생생된 메시지 맵 함수
protected:
   afx_msg void OnFilePrintPreview();
   afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
   afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
   DECLARE_MESSAGE_MAP()
public:
   virtual void OnInitialUpdate();
   afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
   afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};

#ifndef _DEBUG  // BMPDisplayView.cpp의 디버그 버전
inline CBMPDisplayDoc* CBMPDisplayView::GetDocument() const
   { return reinterpret_cast<CBMPDisplayDoc*>(m_pDocument); }
#endif