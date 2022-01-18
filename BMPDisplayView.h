// BMPDisplayView.h : CBMPDisplayView Ŭ������ �������̽�
//

#pragma once

#include <ddraw.h>
#include "Dib.h"

class CBMPDisplayView : public CView
{
protected: // serialization������ ��������ϴ�.
   CBMPDisplayView();
   DECLARE_DYNCREATE(CBMPDisplayView)

// Ư���Դϴ�.
public:
   CBMPDisplayDoc* GetDocument() const;   
   LPDIRECTDRAW7 m_pDDObj;
   LPDIRECTDRAWSURFACE7 m_pPS;
   LPDIRECTDRAWSURFACE7 m_pBB;
   BOOL m_DDrawOK;
   CDib* m_pDib;
   LPDIRECTDRAWPALETTE m_pDDP;
   int m_CountBmp;

// �۾��Դϴ�.
public:
   BOOL CreateSurfaces(void);
   BOOL ClearSurface(LPDIRECTDRAWSURFACE7 pSurface);
   void UpdateSurface(void);
   LPDIRECTDRAWPALETTE CreateDDPalette(LPDIRECTDRAW7 pDDObj, CDib* cdib);
   void LoadBMP(LPDIRECTDRAWSURFACE7 pSurface, CDib* cdib);

// �������Դϴ�.
public:
   virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
   virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
   virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
   virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
   virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// �����Դϴ�.
public:
   virtual ~CBMPDisplayView();
#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
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

#ifndef _DEBUG  // BMPDisplayView.cpp�� ����� ����
inline CBMPDisplayDoc* CBMPDisplayView::GetDocument() const
   { return reinterpret_cast<CBMPDisplayDoc*>(m_pDocument); }
#endif