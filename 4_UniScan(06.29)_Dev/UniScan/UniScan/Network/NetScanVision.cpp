#include "..\StdAfx.h"

#include "NetScanVision.h"
#include <atlconv.h>


CNetScanVision::CNetScanVision()
{
}

CNetScanVision::~CNetScanVision(void)
{

}

void tagSCAN_STRUCT::SetReceiveTime()
{
	tReceiveTime	= GetCurrentTime();
}

int   tagSCAN_STRUCT::_PrintValues()
{
	int i = 0;
	CString str;

	for(i = 0 ; i < nExtraFieldCount; i++)
	{
		str.Format(L"Caption : %s  Value : %s\n", pExtScanInfos[i].aszCaption, pExtScanInfos[i].lpszValue);
		OutputDebugString(str);
	}

	return 0;
}

// 캡션 NAME 달기
CString tagSCAN_STRUCT::_ReadValue(WCHAR* aszCaption)
{
	CString str;
	int i;
	if(aszCaption == NULL)
		return str;

	for(i = 0 ; i < nExtraFieldCount; i++)
	{
		if(wcscmp(pExtScanInfos[i].aszCaption, aszCaption) == 0)
		{
			str = pExtScanInfos[i].lpszValue;
			break;
		}
	}
	return str;
}

CString tagSCAN_STRUCT::_ReadValues() // 모든 값을 읽여서 Caption : Value 값으로 리턴해준다
{
	CString str;
	int i;

	str.AppendFormat(L"%s\n", szAddr);
	for(i = 0 ; i < nExtraFieldCount; i++)
	{
		str.AppendFormat(L"%s : %s\n", pExtScanInfos[i].aszCaption, pExtScanInfos[i].lpszValue);
	}
	return str;
}

// ----------------------------------------------------------------------------------------------------
//   0           1             2             3            4             5              6                              
// | IP        | MAC Address | Stream Port | Http Port  | System Name | Model Type   | Firmware Version | 
// | 7           8             9            10           11            12
// | Resolution| Video format| AIN cnt     | AOUT cnt   | Audio in    | Audio out
// ----------------------------------------------------------------------------------------------------
// FIX ME : Display column 항목을 추가하거나 변경 시 하기 함수를 같이 변경해야한다.
// static
//int  tagSCAN_STRUCT::_CompareScanInfo(int nItemColumn, tagSCAN_STRUCT* pInfo1, tagSCAN_STRUCT* pInfo2)
//{
//	ASSERT(0); // 쓰지마세요. main dialog에서 필드를 변경 할 수 있으므로 여기서 비교하지 않도록 적용합니다
//	int nResult = 0;
//	switch(nItemColumn)
//	{
//	//case 0: // IP
//	//	nResult = _CompareIP(pInfo1->szAddr, pInfo2->szAddr);
//	//	break;
//	//case 1: // MAC
//	//	nResult = wcscmp(pInfo1->szMAC, pInfo2->szMAC);
//	//	break;
//	//case 2: // Stream Port
//	//	nResult = pInfo1->nStreamPort - pInfo2->nStreamPort;
//	//	break;
//	//case 3: // Http Port
//	//	nResult = pInfo1->nHTTPPort - pInfo2->nHTTPPort;
//	//	break;
//	//case 4: // Server Name
//	//	nResult = pInfo1->_ReadValue(L"System Name").Compare(pInfo2->_ReadValue(L"System Name"));
//	//	break;
//	//case 5: // Model
//	//	nResult = pInfo1->_ReadValue(L"Model Type").Compare(pInfo2->_ReadValue(L"Model Type"));
//	//	break;
//	//case 6: // Firmware Version
//	//	nResult = pInfo1->_ReadValue(L"Firmware Version").Compare(pInfo2->_ReadValue(L"Firmware Version"));
//	//	break;
//	//case 7: // Video Count
//	//	nResult = pInfo1->_ReadValue(L"S/W Version").Compare(pInfo2->_ReadValue(L"S/W Version"));
//	//	break;
//	//case 8: // Video Count
//	//	nResult = pInfo1->_ReadValue(L"Video Count").Compare(pInfo2->_ReadValue(L"Video Count"));
//	//	break;
//	//case 7: // Resolution
//	//	nResult = pInfo1->_ReadValue(L"Support Resolution").Compare(pInfo2->_ReadValue(L"Support Resolution"));
//	//	break;
//	//case 8: // Video format
//	//	nResult = pInfo1->_ReadValue(L"Video Format").Compare(pInfo2->_ReadValue(L"Video Format"));
//	//	break;
//	//case 9: // Alarm in count
//	//	nResult = pInfo1->_ReadValue(L"Alarm In Count").Compare(pInfo2->_ReadValue(L"Alarm In Count"));
//	//	break;
//	//case 10:// Alarm out count
//	//	nResult = pInfo1->_ReadValue(L"Alarm Out Count").Compare(pInfo2->_ReadValue(L"Alarm Out Count"));
//	//	break;
//	//case 11: // Audio in count
//	//	nResult = pInfo1->_ReadValue(L"Audio In Count").Compare(pInfo2->_ReadValue(L"Audio In Count"));
//	//	break;
//	//case 12: // Audio out count
//	//	nResult = pInfo1->_ReadValue(L"Audio Out Count").Compare(pInfo2->_ReadValue(L"Audio Out Count"));
//	//	break;
//	default:
//		ASSERT(0); // column added?
//		nResult = 0;
//	}
//	return nResult;
//}

DWORD CNetScanVision::thrScanThread(LPVOID pParam)
{
	CNetScanVision* pThis = (CNetScanVision*)pParam;
	if (NULL == pThis)
		return 0;

	pThis->thrReceiver();

	return 0;
}

BOOL CNetScanVision::StartScan()
{
	// 부모 함수 호출
	m_iRevPort = VH_UDP_SCAN_PORT;

	this->StartScanF((LPTHREAD_START_ROUTINE)thrScanThread);

	return TRUE;
}


// scanner logic
void CNetScanVision::thrReceiver()
{
	HEADER2*			pReceive		= NULL;
	IPUTIL_INFO*		pInfo			= NULL;
	IPUTIL_INFO2*		pInfo2			= NULL;
	SCAN_INFO*			pScanInfo		= NULL;
	BYTE*				pExtField		= NULL;
	CHAR*				pszTemp			= NULL;
	SCAN_EXT_INFO*		pExtInfos		= NULL;
	int					iToRead			= 0;
	int					nItemCount		= 0;
	int					i				= 0;
	int					iSenderAddrLen	= 0;
	LPCAPTION_HEADER	lpCapt			= NULL;
	DWORD				dwLastError		= 0;
	BOOL				bIsSuccessBind	= FALSE;
	SOCKADDR			stSockAddr;

	bIsSuccessBind = SocketBind();

	if (bIsSuccessBind)
	{
		// 서버의 응답을 기다린다
		iSenderAddrLen = sizeof(SOCKADDR_IN);

		m_pReceive_buffer = new char[SCAN_INFO_RECEIVE_BUFFER_SIZE]; // allocate 64 k bytes buffer
		//if (m_pReceive_buffer == NULL)
		//{
		//	if (this->m_hNotifyWnd)
		//		::SendMessage(this->m_hNotifyWnd, this->m_lNotifyMsg, 0, SCAN_ERR_MEMORY); // PostMessage to MainWindow

		//	return;
		//}

		memset(m_pReceive_buffer, 0, SCAN_INFO_RECEIVE_BUFFER_SIZE);

		pReceive = (HEADER2*)m_pReceive_buffer;
		iToRead = pReceive->body_size - sizeof(IPUTIL_INFO2);
		USES_CONVERSION;

		while (this->m_dwScanThreadID)
		{

			if (TRUE == m_bUserCancel)
				goto FINAL;

			if (SOCKET_ERROR == recvfrom(this->m_hReceiveSock, m_pReceive_buffer, SCAN_INFO_RECEIVE_BUFFER_SIZE, 0, (SOCKADDR*)&stSockAddr, &iSenderAddrLen))
			{
				dwLastError = WSAGetLastError();
				TRACE("Vision recvfrom error = %d\n", dwLastError);
				if (this->m_hNotifyWnd && dwLastError != 10004)
					::SendMessage(this->m_hNotifyWnd, this->m_lNotifyMsg, 0, SCAN_ERR_RECV);

				break;
			}

			if (pReceive->magic == MAGIC2_CODE)
			{
				// parsing and update list
				if (pReceive->protocol_mode == PROTOCOL_MODE_RSP_GET_IPINFO_EXT)
				{
					pInfo = (IPUTIL_INFO *)(m_pReceive_buffer + sizeof(HEADER2));
					pInfo2 = (IPUTIL_INFO2*)(m_pReceive_buffer + sizeof(HEADER2));

					pScanInfo = new SCAN_INFO;
					if (pScanInfo)
					{
						pScanInfo->iScanType = 1;
						//USES_CONVERSION;

						WideCopyStringFromAnsi(pScanInfo->szAddr, 30, pInfo->szIPAddress);
						WideCopyStringFromAnsi(pScanInfo->szGateWay, 30, pInfo->szGatewayIP);
						WideCopyStringFromAnsi(pScanInfo->szMAC, 30, pInfo->szMACAddress);

						//pScanInfo->nStreamPort	= pInfo->dwStreamPort;
						pScanInfo->nHTTPPort = pInfo->dwHTTPPort;
						//pScanInfo->version      = VERSION_2; // IPUTIL version 1

						if (pReceive->body_size >= sizeof(IPUTIL_INFO2)) // read extend field
						{
							pScanInfo->cIsDHCP = pInfo2->cIsDHCP;
							//WideCopyStringFromAnsi(pScanInfo->szGateWay, 30, pInfo2->szGatewayIP);
							WideCopyStringFromAnsi(pScanInfo->szSubnetMask, 30, pInfo2->szSubnetmask);
						}

						pExtField = NULL;
						// read extended field
						if (pReceive->body_size > sizeof(IPUTIL_INFO2))
						{
							iToRead = pReceive->body_size - sizeof(IPUTIL_INFO2);
							nItemCount = 0;
							i = 0;
							nItemCount = 0;
							lpCapt = NULL;

							pExtField = (BYTE*)(m_pReceive_buffer + sizeof(HEADER2)+sizeof(IPUTIL_INFO2)); // set pointer
							iToRead = pReceive->body_size - sizeof(IPUTIL_INFO2);

							while (iToRead > 0)
							{
								lpCapt = (LPCAPTION_HEADER)pExtField;

								pExtField = pExtField + (sizeof(CAPTION_HEADER)+lpCapt->nDataLen);
								iToRead -= (sizeof(CAPTION_HEADER)+lpCapt->nDataLen);
								nItemCount++;
							}

							// read data into array
							pExtField = (BYTE*)(m_pReceive_buffer + sizeof(HEADER2) + sizeof(IPUTIL_INFO2)); // reset pointer
							if (nItemCount > 0)
							{
								iToRead = pReceive->body_size - (sizeof(HEADER2)+sizeof(IPUTIL_INFO2));
								pExtInfos = new SCAN_EXT_INFO[nItemCount];
								if (pExtInfos)
								{
									memset(pExtInfos, 0, sizeof(SCAN_EXT_INFO)* nItemCount);
									i = 0;
									while (iToRead > 0)
									{
										lpCapt = (LPCAPTION_HEADER)pExtField;

										WideCopyStringFromAnsi(pExtInfos[i].aszCaption, 32, lpCapt->aszCaption);
										//TRACE( pExtInfos[i].aszCaption );
										//TRACE( L" = " );

										pExtInfos[i].nValueLen = lpCapt->nDataLen + 2;

										pszTemp = new CHAR[pExtInfos[i].nValueLen];

										memset(pszTemp, 0, sizeof(CHAR)*(pExtInfos[i].nValueLen));
										memcpy(pszTemp, (char*)(pExtField + sizeof(CAPTION_HEADER)), lpCapt->nDataLen);

										pExtInfos[i].lpszValue = new WCHAR[pExtInfos[i].nValueLen];
										memset(pExtInfos[i].lpszValue, 0, sizeof(WCHAR)*(pExtInfos[i].nValueLen));

										if (pExtInfos[i].lpszValue)
										{
											//if (0 == wcscmp(pExtInfos[i].aszCaption, L"Upgrade Port"))
											//{
											//	//int it = 0;
											//}

											// FIX ME: A2W가 문제될 거 같은데?
											WideCopyStringFromAnsi(pExtInfos[i].lpszValue, pExtInfos[i].nValueLen, pszTemp);

											//	TRACE( pExtInfos[i].lpszValue );
											//TRACE( L"\n" );

										}
										pExtField = pExtField + (sizeof(CAPTION_HEADER)+lpCapt->nDataLen);
										iToRead -= (sizeof(CAPTION_HEADER)+lpCapt->nDataLen);
										i++;

										if (NULL != pszTemp)
										{
											delete[] pszTemp;
											pszTemp = NULL;
										}
									}

									pScanInfo->pExtScanInfos = pExtInfos;
									pScanInfo->nExtraFieldCount = nItemCount;
								}
							}
						}

						if (0 != m_dwScanThreadID)
						{
							if (this->m_hNotifyWnd)
							{
								if (FALSE == m_bUserCancel)
								{
									//::SendMessageTimeout(this->m_hNotifyWnd, this->m_lNotifyMsg, (WPARAM)pScanInfo, 0, SMTO_NORMAL, 10, NULL);
									::SendMessage(this->m_hNotifyWnd, this->m_lNotifyMsg, (WPARAM)pScanInfo, 0);
								}
							}
						}
					}
				}
			}
		}
	}
	else
	{
		TRACE("Bind Fail = %d\n", WSAGetLastError());
		return;
	}
	
	return;

FINAL:
	if (NULL != pScanInfo)
	{
		delete pScanInfo;
		pScanInfo = NULL;
	}
	this->DelBuff();

}

//
//BOOL CNetScanVision::RequestIPChange(WCHAR* strTargetServerMAC, WCHAR* strNewIP, WCHAR* strNewGateWay, int nStreamPort/*=2700*/, int nHTTPPort/*=80*/)
//{
//	SOCKET sock;
//	char   send_buffer[255] = { 0 };
//	BOOL bEnable = FALSE;
//	SOCKADDR_IN TargetAddr;
//
//	sock = socket(AF_INET, SOCK_DGRAM, 0);
//
//	// broadcast 가능하도록 socket 옵션 조정
//	bEnable = TRUE;
//
//	if(setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&bEnable, sizeof(bEnable) == SOCKET_ERROR))
//	{
//		return FALSE;
//	}
//	
//	TargetAddr.sin_family = AF_INET;
//	TargetAddr.sin_port = htons(VH_UDP_SCAN_PORT);
//	TargetAddr.sin_addr.s_addr = INADDR_BROADCAST; // broad casting
//
//	memset(send_buffer, 0, sizeof(send_buffer));
//	//typedef struct tagIPUTIL_INFO
//	//{
//	//	char	szIPAddress[16];
//	//	char	szGatewayIP[16];
//	//	char	szMACAddress[20];
//	//	DWORD   dwStreamPort;
//	//	DWORD	dwHTTPPort;
//	//}IPUTIL_INFO, *LPIPUTIL_INFO;
//	HEADER2*	pHeader = (HEADER2*)send_buffer;
//	IPUTIL_INFO* pInfo = (IPUTIL_INFO*)(send_buffer + sizeof(HEADER2));
//	// header set
//	pHeader->magic = MAGIC2_CODE;
//	pHeader->protocol_type = PROTOCOL_TYPE_IPUTILITY;
//	pHeader->protocol_mode = PROTOCOL_MODE_REQ_SET_IPINFO;
//	pHeader->body_size = sizeof(IPUTIL_INFO);
//	// body set
//	USES_CONVERSION;
//	strcpy_s(pInfo->szIPAddress, W2A(strNewIP));
//	strcpy_s(pInfo->szGatewayIP, W2A(strNewGateWay));
//	strcpy_s(pInfo->szMACAddress, W2A(strTargetServerMAC));
//	pInfo->dwHTTPPort	= nHTTPPort;
//	pInfo->dwStreamPort	= nStreamPort;
//
//	// send 
//	if(sendto(sock, send_buffer, sizeof(HEADER2)+sizeof(IPUTIL_INFO), 0, (sockaddr*)&TargetAddr, sizeof(TargetAddr)) == SOCKET_ERROR)
//	{
//		closesocket(sock);
//		return FALSE;
//	}
//	// clear temp datas
//	Sleep(300); // 0.3 seconds wait
//	closesocket(sock);
//	// clear temp datas
//	Sleep(300); // 0.3 seconds wait
//	
//	return TRUE;
//}
//
//BOOL CNetScanVision::RequestIPChange2(WCHAR* strTargetServerMAC, WCHAR* strNewIP, WCHAR* strNewGateWay, int nStreamPort/* = 2700*/, int nHTTPPort/* = 80*/, int cIsDHCP/* = 0*/, WCHAR* strNewsubnetMask/*=L"255.255.255.0"*/, WCHAR* szID /*= L""*/, WCHAR* szPass/* = L""*/, int nEncMode/* = 0*/)
//{
//	SOCKET			sock;
//	char			send_buffer[500] = { 0 };
//	HEADER2*		pHeader = NULL;
//	IPUTIL_INFO2*	pInfo = NULL;
//	IPUTIL_AUTH*	pAuth = NULL;
//
//	// broadcast 가능하도록 socket 옵션 조정
//	BOOL bEnable = TRUE;
//
//	sock = socket(AF_INET, SOCK_DGRAM, 0);
//
//	if(setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&bEnable, sizeof(bEnable) == SOCKET_ERROR))
//	{
//		return FALSE;
//	}
//	SOCKADDR_IN TargetAddr;
//	TargetAddr.sin_family = AF_INET;
//	TargetAddr.sin_port   = htons(VH_UDP_SCAN_PORT);
//	TargetAddr.sin_addr.s_addr = INADDR_BROADCAST; // broad casting
//
//	memset(send_buffer, 0, sizeof(send_buffer));
//
//	//typedef struct tagIPUTIL_INFO2
//	//{
//	//	char	szIPAddress[16];
//	//	char	szGatewayIP[16];
//	//	char	szMACAddress[20];
//	//	DWORD   dwStreamPort;
//	//	DWORD	dwHTTPPort;
//	//  char	cIPMode;              // IPUTIL INFO version 2
//	//  char	szSubnetmask[16];
//	//}IPUTIL_INFO2, *LPIPUTIL_INFO2;
//
//	pHeader = (HEADER2*)send_buffer;
//	pInfo = (IPUTIL_INFO2*)(send_buffer + sizeof(HEADER2));
//	pAuth = (IPUTIL_AUTH*)((BYTE*)pInfo + sizeof(IPUTIL_INFO2));
//
//	// header set
//	pHeader->magic = MAGIC2_CODE;
//	pHeader->protocol_type = PROTOCOL_TYPE_IPUTILITY;
//	pHeader->protocol_mode = PROTOCOL_MODE_REQ_SET_IPINFO;
//	pHeader->body_size = sizeof(IPUTIL_INFO2);
//	int nPacketSize = sizeof(HEADER2)+sizeof(IPUTIL_INFO2);
//	// body set
//	USES_CONVERSION;
//	strcpy_s(pInfo->szIPAddress, W2A(strNewIP));
//	strcpy_s(pInfo->szGatewayIP, W2A(strNewGateWay));
//	strcpy_s(pInfo->szMACAddress, W2A(strTargetServerMAC));
//	pInfo->dwHTTPPort	= nHTTPPort;
//	pInfo->dwStreamPort	= nStreamPort;
//	pInfo->cIsDHCP = (char)cIsDHCP;
//	strcpy_s(pInfo->szSubnetmask, W2A(strNewsubnetMask));
//
//	// ID가 설정된 경우에는 Login 정보를 패킷에 함께 보낸다
//	if(szID != NULL && wcscmp(szID, L"") != 0)
//	{
//		nPacketSize += sizeof(IPUTIL_AUTH);
//		pHeader->body_size += sizeof(IPUTIL_AUTH);
//		strcpy_s(pAuth->ID, 32, W2A(szID));
//		strcpy_s(pAuth->Password, 32, W2A(szPass));
//		pAuth->EncMode = nEncMode;
//		// FIX ME: encription 적용
//	}
//
//	// send
//	if(sendto(sock, send_buffer, nPacketSize, 0, (sockaddr*)&TargetAddr, sizeof(TargetAddr)) == SOCKET_ERROR)
//	{
//		closesocket(sock);
//		return FALSE;
//	}
//	Sleep(300); // 0.3 seconds wait
//	closesocket(sock);
//	// clear temp datas
//	Sleep(300); // 0.3 seconds wait
//
//	return TRUE;
//}

BOOL CNetScanVision::SendScanRequest()
{
	char send_buffer[255] = { 0 };
	sockaddr_in VisionSendSock;
	BOOL bEnable = TRUE;
	SOCKET hSockSend = NULL;
	HEADER2* pSendHeader = NULL;

	//TRACE(_T("Send broadcast ping request\n"));
	hSockSend = socket(AF_INET, SOCK_DGRAM, 0);
	if (SOCKET_ERROR == setsockopt(hSockSend, SOL_SOCKET, SO_BROADCAST, (char*)&bEnable, sizeof(bEnable)))
	{
		TRACE("1.setsocketopt error = %d\n", WSAGetLastError());
		return FALSE;
	}

	VisionSendSock.sin_family = AF_INET;
	VisionSendSock.sin_port = htons(VH_UDP_SCAN_PORT);
	VisionSendSock.sin_addr.s_addr = INADDR_BROADCAST; // FIX ME : TEST

	pSendHeader = (HEADER2*)send_buffer;
	pSendHeader->magic = MAGIC2_CODE;
	pSendHeader->protocol_type = PROTOCOL_TYPE_IPUTILITY;
	pSendHeader->protocol_mode = PROTOCOL_MODE_REQ_GET_IPINFO;
	pSendHeader->body_size = 0;

	if (SOCKET_ERROR == sendto(hSockSend, send_buffer, sizeof(HEADER2), 0, (SOCKADDR*)&VisionSendSock, sizeof(sockaddr_in)))
	{
		TRACE("Vision sendto to error = %d\n", WSAGetLastError());
		//closesocket(hSockSend);
		return FALSE;
	}
	//// clear temp datas

	return TRUE;
}
