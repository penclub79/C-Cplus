// IPChangeDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include <afxinet.h>
#include "FactoryDefaultDlg.h"
#include "./Network/NetScanVision.h"

IMPLEMENT_DYNAMIC(CFactoryDefaultDlg, CDialog)

CFactoryDefaultDlg::CFactoryDefaultDlg(CWnd* pParent /*=NULL*/)
: CDialog(CFactoryDefaultDlg::IDD, pParent)
, m_strID(_T(""))
, m_strPassword(_T(""))
{
	m_pScanInfo = NULL; // 다이얼로그를 실행하는 위치에서 메모리 할당 및 초기화 한다.
}

CFactoryDefaultDlg::~CFactoryDefaultDlg()
{
	if(m_pScanInfo)
	{
		delete [] m_pScanInfo;
		m_pScanInfo = NULL;
	}
}

void CFactoryDefaultDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_USERID,      m_strID);
	DDX_Text(pDX, IDC_PASSWORD,    m_strPassword);
	DDX_Control(pDX, IDC_SVR_LIST, m_cSvrList);
	DDX_Control(pDX, ID_FACTORY_DEFAULT_BTN, m_ctlButtonApply);
	DDX_Control(pDX, ID_FACTORY_DEFAULT_CANCEL, m_ctlButtonCancel);
}


BEGIN_MESSAGE_MAP(CFactoryDefaultDlg, CDialog)
	ON_BN_CLICKED(ID_FACTORY_DEFAULT_BTN, &CFactoryDefaultDlg::OnBnClickedOk)
	ON_BN_CLICKED(ID_FACTORY_DEFAULT_CANCEL, &CFactoryDefaultDlg::OnBnClickedCancel)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_SVR_LIST, &CFactoryDefaultDlg::OnLvnItemchangedSvrList)
	ON_EN_CHANGE(IDC_USERID, &CFactoryDefaultDlg::OnEnChangeUserid)
	ON_EN_CHANGE(IDC_PASSWORD, &CFactoryDefaultDlg::OnEnChangePassword)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()


// CFactoryDefaultDlg 메시지 처리기입니다.

BOOL CFactoryDefaultDlg::OnInitDialog()
{
	CString str = L"";

	CDialog::OnInitDialog();

	ASSERT(m_pScanInfo != NULL); // 다이얼로그 실행 전에 배열 형태로 초기화 해야한다.
	//////////////////////////////////////////////////////////////////////////
	// server list column initialize
	// ----------------------------------------------------------------------
	// 0    1             2       3                  4             5
	// IP | MAC Address | Model | Firmware version | Stream Port | Http Port 
	// ----------------------------------------------------------------------
	// --> column update
	//enum {
	//	SUBITEM_IP               = 0,
	//	SUBITEM_MAC              = 1,
	//	SUBITEM_ID               = 2,
	//	SUBITEM_PASS             = 3,
	//	SUBITEM_MODEL            = 4,
	//	SUBITEM_FIRMWARE_VERSION = 5,
	//	SUBITEM_STREAM_PORT      = 6,
	//	SUBITEM_HTTP_PORT        = 7,
	//	SUBITEM_UPGRADE_PORT     = 8,
	//	SUBITEM_SERVER_NAME      = 9,
	//	SUBITEM_RESOLUTION       = 10,
	//	SUBITEM_VIDEO_FORMAT     = 11,
	//	SUBITEM_ALARMIN_CNT      = 12,
	//	SUBITEM_ALARMOUT_CNT     = 13,
	//	SUBITEM_AUDIOIN_CNT      = 14,
	//	SUBITEM_AUDIOOUT_CNT     = 15
	//};
	m_cSvrList.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

	CString strItem; str.LoadString(IDS_ADDRESS);
	strItem = L"IP " + str;
	m_cSvrList.InsertColumn(SUBITEM_IP,               strItem, LVCFMT_CENTER, 120, 0 );
	strItem = L"MAC " + str;
	m_cSvrList.InsertColumn(SUBITEM_MAC,              strItem, LVCFMT_CENTER, 120, 0 );
	strItem = L"ID";
	m_cSvrList.InsertColumn(SUBITEM_ID,               strItem, LVCFMT_CENTER, 50,  0 );
	strItem = L"PW";
	m_cSvrList.InsertColumn(SUBITEM_PASS,             strItem, LVCFMT_CENTER, 50,  0 );
	str.LoadString(IDS_PORT); 
	strItem.LoadString(IDS_STREAM);
	strItem += L" "; strItem += str;
	m_cSvrList.InsertColumn(SUBITEM_STREAM_PORT,      strItem, LVCFMT_CENTER, 100, 0 );
	strItem = L"HTTP "; strItem += str;
	m_cSvrList.InsertColumn(SUBITEM_HTTP_PORT,        strItem, LVCFMT_CENTER, 100, 0 );
	strItem = L"Upgrade "; strItem += str;
	m_cSvrList.InsertColumn(SUBITEM_UPGRADE_PORT,     strItem, LVCFMT_CENTER, 150, 0 );

	strItem.LoadString(IDS_SERVER_NAME);
	m_cSvrList.InsertColumn(SUBITEM_SERVER_NAME,      strItem, LVCFMT_CENTER, 160, 0 );
	strItem.LoadString(IDS_MODEL);
	m_cSvrList.InsertColumn(SUBITEM_MODEL,            strItem, LVCFMT_CENTER, 140, 0 );
	strItem.LoadString(IDS_FIRMWARE_VERSION);
	m_cSvrList.InsertColumn(SUBITEM_FIRMWARE_VERSION, strItem, LVCFMT_CENTER, 160, 0 );
	strItem.LoadString(IDS_RESOLUTIONS);
	m_cSvrList.InsertColumn(SUBITEM_RESOLUTION,       strItem, LVCFMT_CENTER, 180, 0 );
	strItem.LoadString(IDS_VIDEO_FORMAT);
	m_cSvrList.InsertColumn(SUBITEM_VIDEO_FORMAT,     strItem, LVCFMT_CENTER, 100, 0 );
	//strItem.LoadString(IDS_ALARM_IN_COUNT);
	//m_cSvrList.InsertColumn(SUBITEM_ALARMIN_CNT,      strItem, LVCFMT_CENTER, 120, 0 );
	//strItem.LoadString(IDS_ALARM_OUT_COUNT);
	//m_cSvrList.InsertColumn(SUBITEM_ALARMOUT_CNT,     strItem, LVCFMT_CENTER, 120, 0 );
	//strItem.LoadString( IDS_AUDIO_IN_COUNT);
	//m_cSvrList.InsertColumn(SUBITEM_AUDIOIN_CNT,      strItem, LVCFMT_CENTER, 120, 0 );
	//strItem.LoadString(IDS_AUDIO_OUT_COUNT);
	//m_cSvrList.InsertColumn(SUBITEM_AUDIOOUT_CNT,     strItem, LVCFMT_CENTER, 120, 0 );

	AddData();
#ifdef _DEFAULT_ID_PASSWORD_
	m_strID       = L"admin";
	m_strPassword = L"1234";

	SetDlgItemText( IDC_USERID,   m_strID );
	SetDlgItemText( IDC_PASSWORD, m_strPassword );
	//}}
#endif _DEFAULT_ID_PASSWORD_

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CFactoryDefaultDlg::AddData()
{
	CString strTemp;

	//UpgradeList에 데이터 입력
	for(int i=0;i<m_nSelectedCnt;i++)
	{
		// 이미 복사 되어 있으므로 메모리 복사하지 않
		//memcpy((char*)m_pScanInfo, m_pBufScanInfo+sizeof(SCAN_INFO)*i,sizeof(SCAN_INFO));

		// add information into UI
		LV_ITEM item;
		memset(&item, 0, sizeof(item));
		item.mask = LVIF_TEXT | LVIF_PARAM;
		item.iItem = i;
		item.iSubItem = SUBITEM_IP;
		item.pszText = m_pScanInfo[i].szAddr;
		item.lParam = (LPARAM)&m_pScanInfo[i];
		m_cSvrList.InsertItem(&item);


		item.mask = LVIF_TEXT;
		item.iItem = i;
		item.iSubItem = SUBITEM_MAC;
		item.pszText = m_pScanInfo[i].szMAC;
		m_cSvrList.SetItem(&item);

		//{{ ID, Password 항목 추가
		item.mask = LVIF_TEXT;
		item.iItem = i;
		item.iSubItem = SUBITEM_ID;
		item.pszText = L"";
		m_cSvrList.SetItem(&item);
		m_cSvrList.CreateEdit(i, SUBITEM_ID, FALSE );

		item.mask = LVIF_TEXT;
		item.iItem = i;
		item.iSubItem = SUBITEM_PASS;
		item.pszText = L"";
		m_cSvrList.SetItem(&item);
		m_cSvrList.CreateEdit(i, SUBITEM_PASS, TRUE );
		//}}

		//strTemp.Format(_T("%d"), m_pScanInfo[i].nStreamPort);
		//item.mask = LVIF_TEXT;
		//item.iItem = i;
		//item.iSubItem = SUBITEM_STREAM_PORT;
		//item.pszText = (LPTSTR)(LPCTSTR)strTemp;
		//m_cSvrList.SetItem(&item);

		strTemp.Format(_T("%d"), m_pScanInfo[i].nHTTPPort);
		item.mask = LVIF_TEXT;
		item.iItem = i;
		item.iSubItem = SUBITEM_HTTP_PORT;
		item.pszText = (LPTSTR)(LPCTSTR)strTemp;
		m_cSvrList.SetItem(&item);


		// extended infomation printout
		if(m_pScanInfo[i].nExtraFieldCount)
		{
			//strTemp = m_pScanInfo[i]._ReadValue(L"Upgrade Port");
			//item.mask = LVIF_TEXT;
			//item.iItem = i;
			//item.iSubItem = SUBITEM_UPGRADE_PORT; // indexing bug fix
			//item.pszText = (LPTSTR)(LPCTSTR)strTemp;
			//m_cSvrList.SetItem(&item);

			strTemp = m_pScanInfo[i]._ReadValue(L"System Name");
			item.mask = LVIF_TEXT;
			item.iItem = i;
			item.iSubItem = SUBITEM_SERVER_NAME;
			item.pszText = (LPTSTR)(LPCTSTR)strTemp;
			m_cSvrList.SetItem(&item);

			strTemp = m_pScanInfo[i]._ReadValue(L"Model Type");	
			item.mask = LVIF_TEXT;
			item.iItem = i;
			item.iSubItem = SUBITEM_MODEL;
			item.pszText = (LPTSTR)(LPCTSTR)strTemp;
			m_cSvrList.SetItem(&item);

			strTemp = m_pScanInfo[i]._ReadValue(L"Firmware Version");
			item.mask = LVIF_TEXT;
			item.iItem = i;
			item.iSubItem = SUBITEM_FIRMWARE_VERSION;
			item.pszText = (LPTSTR)(LPCTSTR)strTemp;
			m_cSvrList.SetItem(&item);

			strTemp = m_pScanInfo[i]._ReadValue(L"Support Resolution");
			item.mask = LVIF_TEXT;
			item.iItem = i;
			item.iSubItem = SUBITEM_RESOLUTION;
			item.pszText = (LPTSTR)(LPCTSTR)strTemp;
			m_cSvrList.SetItem(&item);

			strTemp = m_pScanInfo[i]._ReadValue(L"Video Format");
			item.mask = LVIF_TEXT;
			item.iItem = i;
			item.iSubItem = SUBITEM_VIDEO_FORMAT;
			item.pszText = (LPTSTR)(LPCTSTR)strTemp;
			m_cSvrList.SetItem(&item);

			strTemp = m_pScanInfo[i]._ReadValue(L"Alarm In Count");
			item.mask = LVIF_TEXT;
			item.iItem = i;
			item.iSubItem = SUBITEM_ALARMIN_CNT;
			item.pszText = (LPTSTR)(LPCTSTR)strTemp;
			m_cSvrList.SetItem(&item);

			strTemp = m_pScanInfo[i]._ReadValue(L"Alarm Out Count");
			item.mask = LVIF_TEXT;
			item.iItem = i;
			item.iSubItem = SUBITEM_ALARMOUT_CNT;
			item.pszText = (LPTSTR)(LPCTSTR)strTemp;
			m_cSvrList.SetItem(&item);

			strTemp = m_pScanInfo[i]._ReadValue(L"Audio In Count");
			item.mask = LVIF_TEXT;
			item.iItem = i;
			item.iSubItem = SUBITEM_AUDIOIN_CNT;
			item.pszText = (LPTSTR)(LPCTSTR)strTemp;
			m_cSvrList.SetItem(&item);

			strTemp = m_pScanInfo[i]._ReadValue(L"Audio Out Count");
			item.mask = LVIF_TEXT;
			item.iItem = i;
			item.iSubItem = SUBITEM_AUDIOOUT_CNT;
			item.pszText = (LPTSTR)(LPCTSTR)strTemp;
			m_cSvrList.SetItem(&item);
		}
	}

	return;
}

void CFactoryDefaultDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
}

HBRUSH CFactoryDefaultDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	return hbr;
}

LRESULT CFactoryDefaultDlg::OnMoveWnd(WPARAM wParam, LPARAM lParam)
{
	CRect rc; GetWindowRect(&rc);
	MoveWindow(rc.left - (LONG)wParam, rc.top - (LONG)lParam, rc.Width(),rc.Height());
	return 0L;
}

void CFactoryDefaultDlg::OnBnClickedOk()
{
	CString receive = L"";
	CString strTemp=L"";
	CString strID;
	CString strPass;
	int     i = 0;
	CString msg;
	CString str;

	GetDlgItemText(IDC_USERID,  m_strID);
	GetDlgItemText(IDC_PASSWORD,m_strPassword);

	if(m_strID.IsEmpty())
	{
		str.LoadString(IDS_ENTER_LOGIN_INFO);
		AfxMessageBox(str, MB_ICONWARNING);
		GetDlgItem(IDC_USERID)->SetFocus();
		return;
	}
	if(m_strPassword.IsEmpty())
	{
		str.LoadString(IDS_ENTER_PASSWORD_INFO);
		AfxMessageBox(str, MB_ICONWARNING);
		GetDlgItem(IDC_PASSWORD)->SetFocus();
		return;
	}

	//{{ ID, Password field check
	for(i = 0;i < m_nSelectedCnt;i++)
	{
		strID	= m_cSvrList.GetEditText(i, SUBITEM_ID);
		strPass = m_cSvrList.GetEditText(i, SUBITEM_PASS);
		if(strID.IsEmpty())
		{
			str.LoadString(IDS_ENTER_LOGIN_INFO);
			AfxMessageBox(str, MB_ICONWARNING);
			m_cSvrList.SetFocusEdit(i, SUBITEM_ID);
			return;
		}
		if(strPass.IsEmpty())
		{
			str.LoadString(IDS_ENTER_PASSWORD_INFO);
			AfxMessageBox(str, MB_ICONWARNING);
			m_cSvrList.SetFocusEdit(i, SUBITEM_PASS);
			return;
		}
	}
	//}}
	msg.Format(_T("[Warning] Program is processing factory default..."));
	SetDlgItemText(IDC_STC_PROCESSING, msg);

	for(i = 0;i < m_nSelectedCnt;i++)
	{
//		TRACE(L"\nRequest Factory Default!, Index = %d, IPAddr = %s, HttpPort  =%d\n",i,m_pScanInfo[i].szAddr,m_pScanInfo[i].nHTTPPort);
		strID	= m_cSvrList.GetEditText(i, SUBITEM_ID);
		strPass = m_cSvrList.GetEditText(i, SUBITEM_PASS);
		if(ConnectionCheck(m_pScanInfo[i].szAddr, m_pScanInfo[i].nHTTPPort))
		{
//			TRACE(L"\nConnectionCheck OK!, Index = %d, IPAddr = %s, HttpPort  =%d\n",i,m_pScanInfo[i].szAddr,m_pScanInfo[i].nHTTPPort);
			strTemp=L"";
			strTemp.Format(L"cgi-bin/auth.cgi?mode=login&id=%s&pass=%s", strID, strPass);
			CallHttpRequest(m_pScanInfo[i].szAddr, m_pScanInfo[i].nHTTPPort, strTemp, receive);

//			TRACE(L"\nIndex = %d, IPAddr = %s, HttpPort  =%d\n",i,m_pScanInfo[i].szAddr,m_pScanInfo[i].nHTTPPort);
//			TRACE(L"\nResponse = %s\n\n",receive);

			if(receive.Find(L"OK") != -1)
			{
				strTemp=L"";
				strTemp.Format(L"cgi-bin/factorydefault.cgi"); 
				CallHttpRequest(m_pScanInfo[i].szAddr, m_pScanInfo[i].nHTTPPort, strTemp, receive);
			}
			else
			{
//				TRACE(L"\nLogin Fail!, Index = %d, IPAddr = %s, HttpPort  =%d\n",i,m_pScanInfo[i].szAddr,m_pScanInfo[i].nHTTPPort);
			}

		}
		else
		{
//			TRACE(L"\nConnection Fail!, Index = %d, IPAddr = %s, HttpPort  =%d\n",i,m_pScanInfo[i].szAddr,m_pScanInfo[i].nHTTPPort);
		}

		Sleep(100);
	}

	OnOK();
}

void CFactoryDefaultDlg::OnBnClickedCancel()
{
	OnCancel();
}

void CFactoryDefaultDlg::OnDestroy()
{
	CDialog::OnDestroy();
	// FIX ME : not entered here why?
}

void CFactoryDefaultDlg::OnLvnItemchangedSvrList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}

int CFactoryDefaultDlg::CallHttpRequest(CString ipaddr, int port, CString query, CString &response)
{
	CInternetSession*	pSession		= NULL;
	CHttpConnection*	pHttpConnect	= NULL;
	CHttpFile*			pHttpFile		= NULL;
	CString				url				= query;
	DWORD				HttpRequestFlags;

	try{

		HttpRequestFlags = INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE;

		// HTTP 메소드별로 사용 가능
		pSession = new CInternetSession;

		pSession->SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 5000); 
		pSession->SetOption(INTERNET_OPTION_CONNECT_RETRIES,1); 
		pSession->SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, 5000);
		pSession->SetOption(INTERNET_OPTION_SEND_TIMEOUT, 5000);

		pHttpConnect = pSession->GetHttpConnection(ipaddr, (INTERNET_PORT)port );
#if 0
		pHttpFile = pHttpConnect->OpenRequest(CHttpConnection::HTTP_VERB_GET, url);
#else
		pHttpFile = pHttpConnect->OpenRequest(CHttpConnection::HTTP_VERB_GET,
			url.GetBuffer(1024), NULL, 1, NULL, (LPCTSTR)"1.0", HttpRequestFlags);url.ReleaseBuffer();
#endif

		// Use direct write to posting field!
		CString strHeaders = L"Accept: text/*\r\n";
		strHeaders += L"User-Agent: HttpCall\r\n";
		strHeaders += L"Accept-Language: UFT-8\r\n";

		pHttpFile->AddRequestHeaders(strHeaders);

		pHttpFile->SendRequest();
	}
	catch(CInternetException *m_pEx)
	{
//		m_pEx->ReportError();
		m_pEx->Delete();
		return 0;
	}

	// result값 확인
	DWORD m_dwStatusCode;
	pHttpFile->QueryInfoStatusCode(m_dwStatusCode);

	// Read Data
	CString strRetBufLen;
	pHttpFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH, strRetBufLen);

	CString szResult = L"";
	char c[1024*64];
	while(pHttpFile->ReadString((LPTSTR)c,1024)!=FALSE)
	{
		response += c;
	}

	pHttpFile->Flush();

	// 객체 delete 부분
	if(pHttpFile)
	{
		pHttpFile->Close();
		delete pHttpFile;
		pHttpFile = NULL;
	}
	if(pHttpConnect)
	{
		pHttpConnect->Close();
		delete pHttpConnect;
		pHttpConnect = NULL;
	}
	if(pSession)
	{
		pSession->Close();
		delete pSession;
		pSession = NULL;
	}

	return 0;
}

BOOL CFactoryDefaultDlg::ConnectionCheck(CString ipaddr, int port)
{
	BOOL bConnect = TRUE;
	int nPort = port;

	int on = 1;
	struct sockaddr_in toaddr;
	int status = 0;
	int nRet=0;
	int                flags=0, n=0, error=0;
	fd_set            rset, wset;

	USES_CONVERSION; 
	LPCSTR  ip = T2CA(ipaddr); 

	// Client
	int sockfd = 0;
	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	int nonblocking =1;
	ioctlsocket(sockfd, FIONBIO, (unsigned long*) &nonblocking);

	if ( sockfd < 0 )
	{
		bConnect = FALSE;
	}
	memset(&toaddr,0x00, sizeof(toaddr));
	toaddr.sin_family = AF_INET;
	toaddr.sin_port = htons(nPort);
	toaddr.sin_addr.s_addr = inet_addr(ip);
	int conResult=0;

	//int errno

	if (connect(sockfd, (struct sockaddr *)&toaddr, sizeof(toaddr)) < 0) {
		conResult = WSAGetLastError();

		if (conResult != WSAEINPROGRESS && conResult!= WSAEWOULDBLOCK)
		{
//			TRACE("%d \n", errno);
		}
		else
		{
//			TRACE("%d \n", errno);
		}
	}
	else
	{
		bConnect = TRUE;
	}

	FD_ZERO(&rset);
	FD_SET(sockfd, &rset);
	wset = rset;

	struct timeval tval;
	tval.tv_sec = 1;
	tval.tv_usec = 0;

	if ( (n = select(sockfd+1, &rset, &wset, NULL,
		((tval.tv_sec>0) || (tval.tv_usec>0))? &tval : NULL)) == 0) 
	{
		closesocket(sockfd);        /* timeout */
		errno = WSAETIMEDOUT;

		bConnect = FALSE;
	}

	if (!(FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset))) 
	{	
		bConnect = FALSE;
	}
	nonblocking =0;
	ioctlsocket(sockfd, FIONBIO, (unsigned long*) &nonblocking);

	if (error) {
		closesocket(sockfd);        /* just in case */
		errno = error;
		bConnect = FALSE;
	}

	CString kk = L"";

	if(bConnect)
	{
		bConnect = TRUE;
	}
	else
	{
		//kk.Format(L"연결실패 !!! 다시 연결해주세요");
		//AfxMessageBox(kk, MB_OK);
		bConnect = FALSE;
	}

	closesocket(sockfd);

	return bConnect;
}

void CFactoryDefaultDlg::OnEnChangeUserid()
{
	GetDlgItemText( IDC_USERID, m_strID );

	for(int i = 0;i < m_nSelectedCnt;i++)
	{
		m_cSvrList.SetEditText( i, SUBITEM_ID, m_strID );
	}
}

void CFactoryDefaultDlg::OnEnChangePassword()
{
	GetDlgItemText( IDC_PASSWORD, m_strPassword );

	for(int i = 0;i < m_nSelectedCnt; i++)
	{
		m_cSvrList.SetEditText( i, SUBITEM_PASS, m_strPassword );
	}
}

void CFactoryDefaultDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	if( GetSafeHwnd() && m_cSvrList.GetSafeHwnd() && m_ctlButtonApply.GetSafeHwnd() && m_ctlButtonCancel.GetSafeHwnd() )
	{
		m_cSvrList.MoveWindow( 10, 69, cx-20, cy-10-69-30 );
		m_ctlButtonApply.MoveWindow( cx/2-105, cy-21-10, 102, 21 );
		m_ctlButtonCancel.MoveWindow( cx/2+3, cy-21-10, 102, 21 );
	}

}

void CFactoryDefaultDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	lpMMI->ptMinTrackSize.x = 740;
	lpMMI->ptMinTrackSize.y = 328;

	CDialog::OnGetMinMaxInfo(lpMMI);
}
