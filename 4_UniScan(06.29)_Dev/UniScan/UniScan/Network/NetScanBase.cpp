#include "../stdafx.h"
#include "NetScanBase.h"


NetScanBase::NetScanBase()
{
	m_hScanThread		= NULL;
	m_hCloseMsgRecvWnd	= NULL;
	m_dwScanThreadID	= NULL;
	m_hNotifyWnd		= NULL;
	m_pReceive_buffer	= NULL;
	m_bUserCancel		= FALSE;
	m_lNotifyMsg		= 0;
	m_ulBindAddress		= 0;
	m_iRevPort			= 0;
	m_lCloseMsg			= 0;
	m_hReceiveSock		= 0;
	m_bIsSendPass = FALSE;
}

NetScanBase::~NetScanBase()
{
	DelBuff();
}

void NetScanBase::DelBuff()
{
	if (NULL != m_pReceive_buffer)
	{
		delete[] m_pReceive_buffer;
		m_pReceive_buffer = NULL;
	}
}

BOOL NetScanBase::StartScanF(LPTHREAD_START_ROUTINE _pThreadFunc)
{
	m_bUserCancel = FALSE;

	if (NULL != m_hScanThread)
		return TRUE;
	
	m_hScanThread = ::CreateThread(NULL, 0, _pThreadFunc, this, 0, &m_dwScanThreadID);

	if (NULL == m_hScanThread)
	{
		TRACE("Thread create failed\n");
		return FALSE;
	}

	return TRUE;
}

// Dlg에서 사용하는 Stop함수
BOOL NetScanBase::StopScan(int iType)
{
	MSG msg;

	m_bUserCancel = TRUE;

	if (m_hReceiveSock)
	{
		closesocket(m_hReceiveSock);
		m_hReceiveSock = NULL;
	}

	if (m_hScanThread)
	{
		TRACE("WaitForSingleObject\n");

		//switch (MsgWaitForMultipleObjects(1, &m_hScanThread, FALSE, INFINITE, QS_ALLINPUT))
		//{
		//case WAIT_OBJECT_0:
		//	break;

		//case WAIT_TIMEOUT:
		//	TerminateThread(m_hScanThread, 0xffffffff);
		//	break;

		//default:
		//	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		//	{
		//		TranslateMessage(&msg);
		//		DispatchMessage(&msg);
		//	}
		//}

		if (WAIT_TIMEOUT == WaitForSingleObject(m_hScanThread, INFINITE))
		{
			TerminateThread(m_hScanThread, 0xffffffff);
		}

		m_dwScanThreadID = 0;
		TRACE("WaitForSingleObject  -- PASS --\n");
		CloseHandle(m_hScanThread);
		m_hScanThread = NULL;
		DelBuff();
		
		
	}
	return TRUE;
}

void NetScanBase::_Lock()
{
	// Critical Section에 진입.
	EnterCriticalSection(&m_mt);
}

void NetScanBase::_Unlock()
{
	// Critical Section에서 빠져나옴
	LeaveCriticalSection(&m_mt);
}

BOOL NetScanBase::SocketBind()
{
	BOOL bEnable = FALSE;

	m_hReceiveSock = socket(AF_INET, SOCK_DGRAM, 0);

	// broadcast
	if (SOCKET_ERROR == setsockopt(m_hReceiveSock, SOL_SOCKET, SO_BROADCAST, (char*)&bEnable, sizeof(bEnable)))
	{
		TRACE("2.setsocketopt error = %d\n", WSAGetLastError());
		if (m_hNotifyWnd)
			::SendMessage(m_hNotifyWnd, m_lNotifyMsg, 0, SCAN_ERR_SOCKET_OPT);
		
		//ThreadExit();
		return FALSE;
	}

	SOCKADDR_IN		stSockAddr;

	stSockAddr.sin_family = AF_INET;
	stSockAddr.sin_port = htons(m_iRevPort);
	stSockAddr.sin_addr.s_addr = m_ulBindAddress;

	if (SOCKET_ERROR == bind(m_hReceiveSock, (SOCKADDR*)&stSockAddr, sizeof(SOCKADDR)))
	{
		//TRACE("Bind Error = %d\n", WSAGetLastError());
		if (m_hNotifyWnd)
		{
			::SendMessage(m_hNotifyWnd, m_lNotifyMsg, 0, SCAN_ERR_BIND);
		}

		//ThreadExit();
		return FALSE;
	}

	return TRUE;
}

// Bind Address Set
void NetScanBase::SetBindAddress(ULONG _ulBindAddress)
{
	m_ulBindAddress = _ulBindAddress;
}

// Window Message Set
void NetScanBase::SetNotifyWindow(HWND _hWnd, LONG _msg)
{
	m_hNotifyWnd = _hWnd;
	m_lNotifyMsg = _msg;
}

// Close Message Set
void NetScanBase::SetCloseMsgRecvWindow(HWND _hWnd, LONG _msg/* = WM_CLOSE*/)
{
	m_hCloseMsgRecvWnd = _hWnd;
	m_lCloseMsg = _msg;
}

void NetScanBase::SetUserInfo(char* _pszUserName, char* _pszPassword)
{
	if (0 < strlen(_pszUserName) && 0 < strlen(_pszPassword))
	{
		strcpy(m_aszUserName, _pszUserName);
		strcpy(m_aszPassword, _pszPassword);
	}
}


// char -> WCHAR 복사 공통
void NetScanBase::WideCopyStringFromAnsi(WCHAR* _pwszString, int _iMaxBufferLen, char* _pszString)
{
	USES_CONVERSION;
	wcscpy_s(_pwszString, _iMaxBufferLen, A2W(_pszString));
}

// receive 함수에 쓰임
//void NetScanBase::ThreadExit()
//{
//	closesocket(m_hReceiveSock);
//	m_hReceiveSock = NULL;
//
//	if (m_bUserCancel && m_hCloseMsgRecvWnd && ::IsWindow(m_hCloseMsgRecvWnd))
//	{
//		//::SendMessage(m_hCloseMsgRecvWnd, m_lCloseMsg, 0, 0);
//		::SendMessage(m_hCloseMsgRecvWnd, WM_CLOSE, NULL, NULL);
//		TRACE("Thread Exit\n");
//		m_bUserCancel = FALSE;
//	}
//}

void NetScanBase::SetScanIP(SCAN_INFO* _pstScanInfo)
{
	SCAN_INFO* pScanInfo = (SCAN_INFO*)_pstScanInfo;

	if (0 != m_dwScanThreadID)
	{
		if (this->m_hNotifyWnd)
			::SendMessage(this->m_hNotifyWnd, this->m_lNotifyMsg, (WPARAM)pScanInfo, 0);
	}
}