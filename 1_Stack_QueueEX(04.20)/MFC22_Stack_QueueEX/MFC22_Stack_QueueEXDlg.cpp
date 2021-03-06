
// MFC22_Stack_QueueEXDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "MFC22_Stack_QueueEX.h"
#include "MFC22_Stack_QueueEXDlg.h"
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


// CMFC22_Stack_QueueEXDlg 대화 상자



CMFC22_Stack_QueueEXDlg::CMFC22_Stack_QueueEXDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMFC22_Stack_QueueEXDlg::IDD, pParent)
	
{
	m_hIcon				= AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pStack			= NULL;
	m_pQueue			= NULL;
	m_pstrCheckRadio	= NULL;
	m_bIsStack			= TRUE;
}

CMFC22_Stack_QueueEXDlg::~CMFC22_Stack_QueueEXDlg()
{
	if (NULL != m_pStack)
	{
		delete m_pStack;
		m_pStack = NULL;
	}

	if (NULL != m_pQueue)
	{
		delete m_pQueue;
		m_pQueue = NULL;
	}
}

void CMFC22_Stack_QueueEXDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_OUTPUT, m_ctlListBox);
}

BEGIN_MESSAGE_MAP(CMFC22_Stack_QueueEXDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(IDC_RADIO_STACK, &CMFC22_Stack_QueueEXDlg::OnRadioStack)
	ON_COMMAND(IDC_RADIO_QUEUE, &CMFC22_Stack_QueueEXDlg::OnRadioQueue)
	ON_BN_CLICKED(IDC_BUTTON_PUSH, &CMFC22_Stack_QueueEXDlg::OnClickedButtonPush)
	ON_BN_CLICKED(IDC_BUTTON_POP, &CMFC22_Stack_QueueEXDlg::OnClickedButtonPop)
END_MESSAGE_MAP()


// CMFC22_Stack_QueueEXDlg 메시지 처리기

BOOL CMFC22_Stack_QueueEXDlg::OnInitDialog()
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
	m_pstrCheckRadio = (CButton*)GetDlgItem(IDC_RADIO_STACK);
	m_pstrCheckRadio->SetCheck(TRUE);
	m_bIsStack	= TRUE;

	m_pStack = new CStack(5);
	m_pQueue = new CQueue(10);


	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CMFC22_Stack_QueueEXDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMFC22_Stack_QueueEXDlg::OnPaint()
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
HCURSOR CMFC22_Stack_QueueEXDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// Stack 라디오 버튼 활성화
void CMFC22_Stack_QueueEXDlg::OnRadioStack()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	m_pstrCheckRadio = (CButton*)GetDlgItem(IDC_RADIO_STACK);
	m_pstrCheckRadio->SetCheck(TRUE);
	m_bIsStack = TRUE;
	m_ctlListBox.ResetContent();

	// Stack 생성 Queue 해제

}

// Queue 라디오 버튼 활성화
void CMFC22_Stack_QueueEXDlg::OnRadioQueue()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	m_pstrCheckRadio = (CButton*)GetDlgItem(IDC_RADIO_QUEUE);
	m_pstrCheckRadio->SetCheck(TRUE);
	m_bIsStack = FALSE;
	m_ctlListBox.ResetContent();

	// Queue 생성 Stack 해제
}

// 값 입력
void CMFC22_Stack_QueueEXDlg::OnClickedButtonPush()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strValue;
	int		iValue		= 0;
	int		iIndex		= 0;
	int		iStrLen		= 0;
	WCHAR*	pwszBuff	= NULL;

	GetDlgItemText(IDC_EDIT_INPUT, strValue);

	if (TRUE == m_bIsStack)										
	{
		if (NULL != m_pStack)									
		{
			iStrLen = strValue.GetLength();
			pwszBuff = strValue.GetBuffer(sizeof(WCHAR) * iStrLen + 1);

			m_pStack->Push((char*)pwszBuff, sizeof(WCHAR) * iStrLen);

			strValue.ReleaseBuffer();

			ReDrawList();
		}
	}
	else
	{	
		if (NULL != m_pQueue)
		{
			iStrLen = strValue.GetLength();
			pwszBuff = strValue.GetBuffer(sizeof(WCHAR) * iStrLen + 1);

			m_pQueue->EnQueue((char*)pwszBuff, sizeof(WCHAR) * iStrLen);

			strValue.ReleaseBuffer();

			ReDrawList();
		}
	}
}


void CMFC22_Stack_QueueEXDlg::OnClickedButtonPop()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int iCount = 0;
	Link_Data	stLinkData = { 0 };

	BOOL bResult = FALSE;

	if (TRUE == m_bIsStack)
	{
		if (NULL != m_pStack)
		{
			iCount = m_pStack->GetCount();

			if (iCount > 0)
			{
				stLinkData.pszBuffer = new unsigned char[MAX_PATH];
				memset(stLinkData.pszBuffer, 0, sizeof(unsigned char) * MAX_PATH);

				bResult = m_pStack->Pop(&stLinkData);

				// stLinkData.pszBuffer 데이터 사용

				if (NULL != stLinkData.pszBuffer)
				{
					delete[] stLinkData.pszBuffer;
					stLinkData.pszBuffer = NULL;
				}

				if (TRUE == bResult)
					ReDrawList();
			}
			else
			{
				AfxMessageBox(_T("값이 없습니다."));
			}
		}
	}
	else
	{
		iCount = m_pQueue->GetCount();
		if (0 < iCount)
		{
			if (NULL != m_pQueue)
			{
				stLinkData.pszBuffer = new unsigned char[MAX_PATH];
				memset(stLinkData.pszBuffer, 0, sizeof(unsigned char)*MAX_PATH);

				bResult = m_pQueue->DeQueue(&stLinkData);
				
				if (TRUE == bResult)
					ReDrawList();

				if (NULL != stLinkData.pszBuffer)
				{
					delete[] stLinkData.pszBuffer;
					stLinkData.pszBuffer = NULL;
				}
			}
		}
		else
		{
			AfxMessageBox(_T("값이 없습니다."));
		}
	}
}


void CMFC22_Stack_QueueEXDlg::ReDrawList()
{
	m_ctlListBox.ResetContent();

	int			iVal		= 0;
	int			iCount		= 0;
	BOOL		bPos		= FALSE;
	Link_Data	stLinkData	= { 0 };
	CString		strValue;
	CString		strString;

	// 스택
	if (TRUE == m_bIsStack)
	{
		iCount = m_pStack->GetCount();
		
		if (0 < iCount)
		{
			/* 
			1. 매우 중요 ★★★
			구조체 버퍼에 메모리 할당하는데 다른 클래스에서 값을 넣기 때문에
			꼭 해당 클래스에서 해제를 해야한다. 다른 클래스에서 할당하게 되면 
			해당 클래스에서 해제를 해야하므로 여기서 할당하고 -> 값을 받아와서 해제까지 한다.
			문제점은 구조체 문자로 받을 char 버퍼의 사이즈를 문자 길이만큼 받아오지 못하기 
			때문에 MAX_PATH만큼 받아온다.
			*/
			stLinkData.pszBuffer = new unsigned char[MAX_PATH];
			memset(stLinkData.pszBuffer, 0, sizeof(unsigned char)*MAX_PATH);

			for (int i = 0; i < iCount; i++)
			{
				if (TRUE == m_pStack->GetAt(i, &stLinkData))
				{
					strString = stLinkData.pszBuffer;
					strValue.Format(_T("인덱스:%d, 값:%s"), i, strString);
					
					m_ctlListBox.InsertString(i, strValue);
				}
			}

			if (NULL != stLinkData.pszBuffer)
			{
				delete[] stLinkData.pszBuffer;
				stLinkData.pszBuffer = NULL;
			}
		}
	}
	// 큐
	else
	{
		iCount = m_pQueue->GetCount();
		if (0 < iCount)
		{
			stLinkData.pszBuffer = new unsigned char[MAX_PATH];

			for (int i = 0; i < iCount; i++)
			{
				stLinkData.iItemLength = 0;
				memset(stLinkData.pszBuffer, 0, sizeof(unsigned char) * MAX_PATH);

				if (TRUE == m_pQueue->GetAt(i, &stLinkData))
				{
					strString = stLinkData.pszBuffer;
					strValue.Format(_T("인덱스:%d, 값:%s"), i, strString);
					
					m_ctlListBox.InsertString(i, strValue);
				}
			}

			if (NULL != stLinkData.pszBuffer)
			{
				delete[] stLinkData.pszBuffer;
				stLinkData.pszBuffer = NULL;

			}
		}
	}
	
}
