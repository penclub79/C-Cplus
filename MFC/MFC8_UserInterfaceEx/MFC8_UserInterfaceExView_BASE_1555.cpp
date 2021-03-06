
// MFC8_UserInterfaceExView.cpp : CMFC8_UserInterfaceExView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "MFC8_UserInterfaceEx.h"
#endif

#include "MFC8_UserInterfaceExDoc.h"
#include "MFC8_UserInterfaceExView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFC8_UserInterfaceExView

IMPLEMENT_DYNCREATE(CMFC8_UserInterfaceExView, CView)

BEGIN_MESSAGE_MAP(CMFC8_UserInterfaceExView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMFC8_UserInterfaceExView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_LINE, &CMFC8_UserInterfaceExView::OnLine)
	ON_COMMAND(ID_ELLIPSE, &CMFC8_UserInterfaceExView::OnEllipse)
	ON_COMMAND(ID_POLYGON, &CMFC8_UserInterfaceExView::OnPolygon)
	ON_COMMAND(ID_LINE_COLOR, &CMFC8_UserInterfaceExView::OnLineColor)
	ON_COMMAND(ID_FACE_COLOR, &CMFC8_UserInterfaceExView::OnFaceColor)
	ON_COMMAND(ID_BDIAGONAL, &CMFC8_UserInterfaceExView::OnBdiagonal)
	ON_COMMAND(ID_CROSS, &CMFC8_UserInterfaceExView::OnCross)
	ON_COMMAND(ID_VERTICAL, &CMFC8_UserInterfaceExView::OnVertical)
END_MESSAGE_MAP()

// CMFC8_UserInterfaceExView 생성/소멸

CMFC8_UserInterfaceExView::CMFC8_UserInterfaceExView()
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CMFC8_UserInterfaceExView::~CMFC8_UserInterfaceExView()
{
}

BOOL CMFC8_UserInterfaceExView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CMFC8_UserInterfaceExView 그리기

void CMFC8_UserInterfaceExView::OnDraw(CDC* /*pDC*/)
{
	CMFC8_UserInterfaceExDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// CMFC8_UserInterfaceExView 인쇄


void CMFC8_UserInterfaceExView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CMFC8_UserInterfaceExView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CMFC8_UserInterfaceExView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CMFC8_UserInterfaceExView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}

void CMFC8_UserInterfaceExView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CMFC8_UserInterfaceExView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CMFC8_UserInterfaceExView 진단

#ifdef _DEBUG
void CMFC8_UserInterfaceExView::AssertValid() const
{
	CView::AssertValid();
}

void CMFC8_UserInterfaceExView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMFC8_UserInterfaceExDoc* CMFC8_UserInterfaceExView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFC8_UserInterfaceExDoc)));
	return (CMFC8_UserInterfaceExDoc*)m_pDocument;
}
#endif //_DEBUG


// CMFC8_UserInterfaceExView 메시지 처리기


void CMFC8_UserInterfaceExView::OnLine()
{	
	// TODO: 여기에 명령 처리기 코드를 추가합니다.

	// 직선 메뉴를 선택했을 때 메시지 출력
	AfxMessageBox(_T("직선을 그립니다."));
}


void CMFC8_UserInterfaceExView::OnEllipse()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.

	// 원 메뉴를 선택했을 때 메시지 출력
	AfxMessageBox(_T("원을 그립니다."));
}


void CMFC8_UserInterfaceExView::OnPolygon()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.

	// 다각형 메뉴를 선택했을 때 메시지 출력
	AfxMessageBox(_T("다각형을 그립니다."));
}


void CMFC8_UserInterfaceExView::OnLineColor()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.

	// 선 색상 메뉴를 선택했을 때 메시지 출력
	AfxMessageBox(_T("선의 색상을 변경합니다."));
}


void CMFC8_UserInterfaceExView::OnFaceColor()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.

	// 면 색상 메뉴를 선택했을 때 메시지 출력
	AfxMessageBox(_T("면의 색상을 변경합니다."));
}


void CMFC8_UserInterfaceExView::OnBdiagonal()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.

	// 브러시 스타일을 왼쪽 45도 메뉴를 선택했을 때 메시지 출력
	AfxMessageBox(_T("왼쪽 45도 패턴으로 변경합니다."));
}


void CMFC8_UserInterfaceExView::OnCross()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.

	// 브러시 스타일을 십자가 메뉴를 선택했을 때 메시지 출력
	AfxMessageBox(_T("십자가 패턴으로 변경합니다."));
}


void CMFC8_UserInterfaceExView::OnVertical()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.

	// 브러시 스타일을 수집 메뉴를 선택했을 때 메시지 출력
	AfxMessageBox(_T("수직 패턴으로 변경합니다."));
}
