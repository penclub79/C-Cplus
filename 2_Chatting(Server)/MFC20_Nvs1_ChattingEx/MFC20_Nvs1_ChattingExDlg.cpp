
// MFC20_Nvs1_ChattingExDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "MFC20_Nvs1_ChattingEx.h"
#include "MFC20_Nvs1_ChattingExDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFC20_Nvs1_ChattingExDlg 대화 상자


CMFC20_Nvs1_ChattingExDlg::CMFC20_Nvs1_ChattingExDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMFC20_Nvs1_ChattingExDlg::IDD, pParent)
	, m_strMyIP(_T(""))
	, m_strOtherIP(_T(""))
	, m_strInitLoc(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFC20_Nvs1_ChattingExDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CHAT, m_listChat);
	DDX_Control(pDX, IDC_IPADDRESS_SERVER, m_IPAddress);
}

BEGIN_MESSAGE_MAP(CMFC20_Nvs1_ChattingExDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CMFC20_Nvs1_ChattingExDlg::OnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CMFC20_Nvs1_ChattingExDlg::OnClickedButtonSend)
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CMFC20_Nvs1_ChattingExDlg::OnClickedButtonClose)
END_MESSAGE_MAP()


// CMFC20_Nvs1_ChattingExDlg 메시지 처리기

BOOL CMFC20_Nvs1_ChattingExDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	// IP주소 가져오기
	char szName[255];
	PHOSTENT stHostinfo;
	if (gethostname(szName, sizeof(szName)) == 0)
	{
		if ((stHostinfo = gethostbyname(szName)) != NULL)
		{
			m_strMyIP = inet_ntoa (*(struct in_addr *)*stHostinfo->h_addr_list);
		}
	}

	// 컨트롤 초기화
	m_IPAddress.SetWindowTextW(m_strMyIP);	// ipaddress 상자에 ip 값을 넣어준다.
	//m_IPAddress.EnableWindow(FALSE);  // ipaddress 상자를 비활성화 시킨다.
	SetDlgItemText(IDC_BUTTON_CONNECT, _T("Open"));

	// Text Static ServerIP초기 좌표가져오기
	GetDlgItem(IDC_STATIC_SERVER_IP)->GetWindowRect(&m_strInitLoc);
	// 다이얼로그 내에 상대적 좌표로 변환
	ScreenToClient(&m_strInitLoc);

	// 다이얼로그 초기화
	RECT stDlgLoc = { 0 };
	this->GetClientRect(&stDlgLoc);
	ResizeControl(stDlgLoc.right - stDlgLoc.left, stDlgLoc.bottom - stDlgLoc.top);

	::GetClientRect(::GetDlgItem(this->GetSafeHwnd(), IDC_EDIT_SEND), &stDlgLoc);

	//// SetWindowPos() 함수
	//::SetWindowPos(this->GetSafeHwnd(), HWND_TOPMOST, 0, 0, 900, 900, SWP_NOZORDER | SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);

	//SetWindowPos(&CWnd::wndBottom, )


	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CMFC20_Nvs1_ChattingExDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
		SendMessage(1000);
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMFC20_Nvs1_ChattingExDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMFC20_Nvs1_ChattingExDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMFC20_Nvs1_ChattingExDlg::OnClickedButtonConnect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();
	
	((CMFC20_Nvs1_ChattingExApp*)AfxGetApp())->InitServer();

	GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(FALSE);
	
	CString strIP;
	GetDlgItemText(IDC_IPADDRESS_SERVER, strIP);
	
}


void CMFC20_Nvs1_ChattingExDlg::OnClickedButtonSend()
{
	
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strSend, strInsert;
	
	strInsert.Format(_T("클라이언트[%s]:%s"), m_strMyIP, strSend);

	int sel = m_listChat.InsertString(-1, strInsert);
	m_listChat.SetCurSel(sel);
	
	SetDlgItemText(IDC_EDIT_SEND, _T(""));
	
}
// 사용자가 채팅방에 들어왔을 때
void CMFC20_Nvs1_ChattingExDlg::EntryClient(CAcceptSock* pAccept)
{
	CString strInsert;
	CString strDisplayUserID;

	// 클라이언트 ID를 받아온다.
	CString strUserID = pAccept->GetUserID();
	// 채팅방에 들어온 사용자 변수 데이터 가공
	strDisplayUserID.Format(_T("[%s] 입장"), strUserID);
	// 채팅방 리스트 다이얼로그에 insert
	m_listChat.InsertString(-1, strDisplayUserID);
}

// 사용자가 채팅방을 나갔을 때
void CMFC20_Nvs1_ChattingExDlg::onClose(CAcceptSock* pAccept)
{
	CString strInsert;
	CString strDisplayUserID;

	// 클라이언트 ID를 받아온다.
	CString strUserID = pAccept->GetUserID();
	strDisplayUserID.Format(_T("[%s] 퇴장"), strUserID);
	// 채팅방 리스트 다이얼로그에 insert
	m_listChat.InsertString(-1, strDisplayUserID);
}

// 연결된 클라이언트 IP받기
void CMFC20_Nvs1_ChattingExDlg::Accept(CString strSock)
{
	m_strOtherIP = strSock;
}

void CMFC20_Nvs1_ChattingExDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	ResizeControl(cx, cy);
}


void CMFC20_Nvs1_ChattingExDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialogEx::OnSizing(fwSide, pRect);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CMFC20_Nvs1_ChattingExDlg::ResizeControl(int cx, int cy)
{
	if (NULL == this->GetSafeHwnd())
		return;

	/* 위치 정보 가져오기(컨트롤) - START */
	// Server IP STATIC
	CWnd* pstrCtl = GetDlgItem(IDC_STATIC_SERVER_IP);
	//TRACE(_T("left : %d, right : %d, top : %d, bottom : %d,", m_rectDlg.left, m_rectDlg.right, m_rectDlg.top, m_rectDlg.bottom));
	if (NULL == pstrCtl)
		return;

	int iMarginX = 20;
	int iMarginY = 10;
	int iHeight = 40;

	/* 위치 정보 적용(컨트롤) - START */
	// Server IP STATIC
	pstrCtl->MoveWindow(iMarginX, iMarginY, 100, iHeight, TRUE);

	// IPADDRESS EDIT
	pstrCtl = GetDlgItem(IDC_IPADDRESS_SERVER);
	pstrCtl->MoveWindow(iMarginX + 100 + iMarginX, iMarginY, cx - (cx/2 + iMarginX + 100 + iMarginX), iHeight, TRUE);

	// CONNECT BUTTON
	pstrCtl = GetDlgItem(IDC_BUTTON_CONNECT);
	pstrCtl->MoveWindow(cx - (iMarginX + iMarginX + iMarginX + 150) + iMarginX + iMarginX, iMarginY, 150, iHeight, TRUE);

	// LISTBOX 
	pstrCtl = GetDlgItem(IDC_LIST_CHAT);
	pstrCtl->MoveWindow(iMarginX, iMarginY + iHeight + iMarginY, (cx - iMarginX * 2), cy - (iMarginY + 39 + 10 + iHeight + 300));

	// SEND EDIT
	pstrCtl = GetDlgItem(IDC_EDIT_SEND);
	pstrCtl->MoveWindow(iMarginX, cy - (iMarginY + 39 + 10 + iHeight + 300) + iMarginY + iHeight + iMarginY + iMarginY, cx - (iMarginX + iMarginX + iMarginX + 150), iHeight, TRUE);

	// SEND BUTTON
	pstrCtl = GetDlgItem(IDC_BUTTON_SEND);
	pstrCtl->MoveWindow(cx - (iMarginX + iMarginX + iMarginX + 150) + iMarginX + iMarginX, cy - (iMarginY + 39 + 10 + iHeight + 300) + iMarginY + iHeight + iMarginY + iMarginY, 150, iHeight, TRUE);

	// CLOSE BUTTON
	pstrCtl = GetDlgItem(IDC_BUTTON_CLOSE);
	pstrCtl->MoveWindow((cx / 2) - (150 / 2), cy - (iMarginY + 39 + 10 + iHeight + 300) + iMarginY + iHeight + iMarginY + iMarginY + iHeight + iMarginX, 150, iHeight, TRUE);

	/* 위치 정보 적용(컨트롤) - END */
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

void CMFC20_Nvs1_ChattingExDlg::OnClickedButtonClose()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	PostMessage(WM_CLOSE);
}


LRESULT CMFC20_Nvs1_ChattingExDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	switch (message)
	{
	case WM_CLOSE:
		((CMFC20_Nvs1_ChattingExApp*)AfxGetApp())->CleanUp();
		break;

	default:
		break;
	}

	return CDialogEx::WindowProc(message, wParam, lParam);
}

