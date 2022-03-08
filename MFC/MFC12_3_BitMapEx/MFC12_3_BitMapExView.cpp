
// MFC12_3_BitMapExView.cpp : CMFC12_3_BitMapExView Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "MFC12_3_BitMapEx.h"
#endif

#include "MFC12_3_BitMapExDoc.h"
#include "MFC12_3_BitMapExView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFC12_3_BitMapExView

IMPLEMENT_DYNCREATE(CMFC12_3_BitMapExView, CView)

BEGIN_MESSAGE_MAP(CMFC12_3_BitMapExView, CView)
	// ǥ�� �μ� �����Դϴ�.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_COMMAND(ID_IMAGE_ROAD, &CMFC12_3_BitMapExView::OnImageRoad)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// CMFC12_3_BitMapExView ����/�Ҹ�

CMFC12_3_BitMapExView::CMFC12_3_BitMapExView()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	for (int i = 0; i < 2; i++)
	{
		m_apBuffer[i] = NULL;
	}
	m_pBuffer = NULL;

}

CMFC12_3_BitMapExView::~CMFC12_3_BitMapExView()
{
	for (int i = 0; i < 2; i++)
	{
		if (m_apBuffer[i])
		{
			free(m_apBuffer[i]);
			m_apBuffer[i] = NULL;
		}
	}
	
	if (m_pBuffer)
	{
		free(m_pBuffer);
		m_pBuffer = NULL;
	}
}

BOOL CMFC12_3_BitMapExView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CView::PreCreateWindow(cs);
}

// CMFC12_3_BitMapExView �׸���

void CMFC12_3_BitMapExView::OnDraw(CDC* _pDC)
{
	CMFC12_3_BitMapExDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.
	BITMAPFILEHEADER *pfh = NULL;
	BITMAPINFOHEADER *pih = NULL;
	BYTE *pRaster = NULL;
	int bx = 0, by = 0;
	
	HDC hdc = NULL;

	// ���ۿ� �����Ͱ� �ִ��� üũ �ִٸ�= ���, ���ٸ�= ���
	//for (int i = 2; i < 3; i++)

	if (m_pBuffer)
	{
		pfh = (BITMAPFILEHEADER *)m_pBuffer;
		pih = (BITMAPINFOHEADER *)((PBYTE)m_pBuffer + sizeof(BITMAPFILEHEADER));
		pRaster = (PBYTE)m_pBuffer + pfh->bfOffBits;
		
		bx = pih->biWidth;
		by = pih->biHeight;
		//m_ibx = pih->biWidth;
		//m_iby = pih->biHeight;
		
		::SetDIBitsToDevice(
			_pDC->GetSafeHdc(), 
			10, 50, bx, by, 
			0, 0, 0, by, 
			pRaster, 
			(BITMAPINFO *)pih, DIB_RGB_COLORS);
	}

}


// CMFC12_3_BitMapExView �μ�

BOOL CMFC12_3_BitMapExView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// �⺻���� �غ�
	return DoPreparePrinting(pInfo);
}

void CMFC12_3_BitMapExView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ��ϱ� ���� �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
}

void CMFC12_3_BitMapExView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ� �� ���� �۾��� �߰��մϴ�.
}


// CMFC12_3_BitMapExView ����

#ifdef _DEBUG
void CMFC12_3_BitMapExView::AssertValid() const
{
	CView::AssertValid();
}

void CMFC12_3_BitMapExView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMFC12_3_BitMapExDoc* CMFC12_3_BitMapExView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFC12_3_BitMapExDoc)));
	return (CMFC12_3_BitMapExDoc*)m_pDocument;
}
#endif //_DEBUG


// CMFC12_3_BitMapExView �޽��� ó����


void CMFC12_3_BitMapExView::OnImageRoad()
{
	// TODO: ���⿡ ���� ó���� �ڵ带 �߰��մϴ�.
	HANDLE ahFile[2] = { INVALID_HANDLE_VALUE };
	DWORD adwFileSize[2] = { 0 };
	DWORD adwRead[2] = { 0 };
	TCHAR aszFileName[2][MAX_PATH] = { 0 };
	int iBigImgID = 0;
	int iSmallImgID = 0;
	int isx = 0, isy = 0;

	// DIB���
	BITMAPFILEHEADER *apfh[2] = { NULL };
	BITMAPINFOHEADER *apih[2] = { NULL };
	BYTE *apRaster[2] = { NULL };
	BYTE *pRasterResult	= NULL;



	wsprintf(aszFileName[0], L"D:\\C\\MFC\\MFC12_2_BitMapEx\\background.bmp");
	wsprintf(aszFileName[1], L"D:\\C\\MFC\\MFC12_2_BitMapEx\\insert.bmp");

	////////////////////////////////////////////////////////////////////////////////
	// ���Ͽ��� ���۷� �����͸� ��� �� /////////////////////////////////////////////

	for (int i = 0; i < 2; i++)
	{
		ahFile[i] = CreateFile(aszFileName[i], GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		
		if (ahFile[i] == INVALID_HANDLE_VALUE)
		{
			break;
		}

		adwFileSize[i] = GetFileSize(ahFile[i], NULL);

		// ���ۿ� ���� �ִ��� üũ
		if (m_apBuffer[i])
		{
			free(m_apBuffer[i]);
			m_apBuffer[i] = NULL;
		}

		// ���� �޸� �Ҵ�
		m_apBuffer[i] = (PBYTE)malloc(adwFileSize[i]);

		// ���� �о����
		ReadFile(ahFile[i], m_apBuffer[i], adwFileSize[i], &adwRead[i], NULL);

		// �޸� ����
		CloseHandle(ahFile[i]);

		// ���� �ڵ� �ݱ�
		ahFile[i] = INVALID_HANDLE_VALUE;
	}

	// ���Ͽ��� ���۷� �����͸� ��� �� /////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////
	// ���� ���ۿ��� �ʿ��� �����͸� m_pBuffer�� ���� �ϴ� �� ///////////////////////
	for (int i = 0; i < 2; i++)
	{
		// �� ���ۿ��� ���� ���� �����´�. 
		if (m_apBuffer[i])
		{
			apfh[i] = (BITMAPFILEHEADER *)m_apBuffer[i];
			apih[i] = (BITMAPINFOHEADER *)((PBYTE)m_apBuffer[i] + sizeof(BITMAPFILEHEADER));
			apRaster[i] = (PBYTE)m_apBuffer[i] + apfh[i]->bfOffBits;
		}	
	}


	// ū �̹��� ������ ��ŭ m_pBuffer ���� �Ҵ�
	if (adwFileSize[0] > adwFileSize[1])
	{
		iBigImgID = 0;
		iSmallImgID = 1;
	}
	else
	{
		iBigImgID = 1;
		iSmallImgID = 0;
	}

	// m_pBuffer ���ۿ� �̹��� Merge �۾� 
	m_pBuffer = (PBYTE)malloc(adwFileSize[iBigImgID]);

	// ū �������� �̹��� ������ ��� ������ ����� ����� �����Ѵ�.
	memcpy(m_pBuffer, m_apBuffer[iBigImgID], adwFileSize[iBigImgID]);
	pRasterResult = m_pBuffer+apfh[iBigImgID]->bfOffBits;

	m_ibx = apih[iSmallImgID]->biWidth; // �׸�1 ���� * �׸�2 ���� = �׸�3 ����
	m_iby = apih[iSmallImgID]->biHeight; // �׸�1 ���� & �׸�2 ���� = �׸�3 ����

	// ��� ũ�� �׸�1 + �׸�2

	int iIndexSrc = 0;
	int iIndexDst = 0;
	int iWidthBitSrc = (apih[iSmallImgID]->biWidth * (apih[iSmallImgID]->biBitCount / 8) + 3) & ~3;
	int iWidthBitDst = (apih[iBigImgID]->biWidth * (apih[iBigImgID]->biBitCount / 8) + 3) & ~3;

	for (int ih = 0; ih < apih[iSmallImgID]->biHeight; ih++)
	{
		for (int iw = 0; iw < iWidthBitSrc/3; iw++)
		{
			iIndexSrc = iw * 3 + 0 + (iWidthBitSrc)*ih;
			iIndexDst = iw * 3 + 0 + (iWidthBitDst)*ih;

			pRasterResult[iIndexDst] = (apRaster[iBigImgID][iIndexDst] + apRaster[iSmallImgID][iIndexSrc]) / 2;	// R
			//pRasterResult[iIndexDst] = (apRaster[iSmallImgID][iIndexSrc]);	// R

			iIndexSrc = iw * 3 + 1 + (iWidthBitSrc)*ih;
			iIndexDst = iw * 3 + 1 + (iWidthBitDst)*ih;

			pRasterResult[iIndexDst] = (apRaster[iBigImgID][iIndexDst] + apRaster[iSmallImgID][iIndexSrc]) / 2; // G
			//pRasterResult[iIndexDst] = (apRaster[iSmallImgID][iIndexSrc]); // G

			iIndexSrc = iw * 3 + 2 + (iWidthBitSrc)*ih;
			iIndexDst = iw * 3 + 2 + (iWidthBitDst)*ih;

			pRasterResult[iIndexDst] = (apRaster[iBigImgID][iIndexDst] + apRaster[iSmallImgID][iIndexSrc]) / 2; // B
			//pRasterResult[iIndexDst] = (apRaster[iSmallImgID][iIndexSrc]); // B
		}
	}

	
	// ���� ���ۿ��� �ʿ��� �����͸� m_pBuffer�� ���� �ϴ� �� ///////////////////////
	////////////////////////////////////////////////////////////////////////////////

	InvalidateRect(NULL);
}


void CMFC12_3_BitMapExView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	CView::OnLButtonDown(nFlags, point);
}


void CMFC12_3_BitMapExView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	CView::OnLButtonUp(nFlags, point);
}


void CMFC12_3_BitMapExView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	CView::OnMouseMove(nFlags, point);
}