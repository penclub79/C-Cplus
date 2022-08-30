#include "../Stdafx.h"
#include "NetScanOnvif.h"
#include <atlconv.h>
#include "../xmlite/XMLite.h"
#include <time.h>
#include <sys/timeb.h>
#include <openssl/sha.h>
//#include <openssl/md5.h>
#include <time.h>


CNetScanOnvif::CNetScanOnvif()
{
	m_bConnected = FALSE;
	m_bIsProbeRev = FALSE;
}

CNetScanOnvif::~CNetScanOnvif(void)
{
	if (m_TcpSocket)
	{
		closesocket(m_TcpSocket);
		m_TcpSocket = NULL;
	}
}

DWORD CNetScanOnvif::thrOnvifScanThread(LPVOID pParam)
{
	CNetScanOnvif* pThis = (CNetScanOnvif*)pParam;
	if (NULL == pThis)
		return 0;

	pThis->thrOnvifReceiver();

	return 0;
}

BOOL CNetScanOnvif::StartScan()
{
	this->StartScanF((LPTHREAD_START_ROUTINE)thrOnvifScanThread);
	return TRUE;
}

BOOL CNetScanOnvif::CreateMultiCastSocket()
{
	int			iError		= 0;
	char		szEnable	= 0;
	int			reuse		= 0;
	struct		ip_mreq mreq;
	struct		in_addr localInterface;
	

	if (NULL == m_hReceiveSock)
	{
		m_hReceiveSock = socket(AF_INET, SOCK_DGRAM, 0);

		/*
		* Disable loopback so you do not receive your own datagrams.
		*/
		if (SOCKET_ERROR == setsockopt(m_hReceiveSock, IPPROTO_IP, IP_MULTICAST_LOOP, (char*)&szEnable, sizeof(szEnable)))
		{
			iError = WSAGetLastError();
			TRACE(_T("SetSocket Error = %d\n"), iError);
			closesocket(m_hReceiveSock);
			return FALSE;
		}

		/*
		* Set local interface for outbound multicast datagrams.
		* The IP address specified must be associated with a local,
		* multicast-capable interface.
		*/
		localInterface.s_addr = m_ulBindAddress; //inet_addr("192.168.0.40");
		if (SOCKET_ERROR == setsockopt(m_hReceiveSock, IPPROTO_IP, IP_MULTICAST_IF, (char*)&localInterface, sizeof(localInterface)))
		{
			iError = WSAGetLastError();
			TRACE(_T("SetSocket Error = %d\n"), iError);
			closesocket(m_hReceiveSock);
			return FALSE;
		}

		/*
		* Enable SO_REUSEADDR to allow multiple instances of this
		* application to receive copies of the multicast datagrams.
		*/
		reuse = 1;
		if (SOCKET_ERROR == setsockopt(m_hReceiveSock, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)))
		{
			TRACE("getaddrinfo error = %d\n", WSAGetLastError());
			closesocket(m_hReceiveSock);
			m_hReceiveSock = NULL;
			return FALSE;
		}

		/*
		* Join the multicast group 225.1.1.1 on the local 9.5.1.1
		* interface.  Note that this IP_ADD_MEMBERSHIP option must be
		* called for each local interface over which the multicast
		* datagrams are to be received.
		*/
		mreq.imr_multiaddr.s_addr = inet_addr("239.255.255.250"); //multi cast group
		mreq.imr_interface.s_addr = m_ulBindAddress; // Local
		//mreq.imr_interface.s_addr = htonl(INADDR_ANY); // INADDR_ANY도 가능

		// 멀티캐스트 그룹 가입
		if (SOCKET_ERROR == setsockopt(m_hReceiveSock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq)))
		{
			iError = WSAGetLastError();
			TRACE(_T("IP_ADD_MEMBERSHIP error=%d\n"), iError);
			this->ThreadExit();
		}
	}

	return TRUE;
}

// Probe Message Request
BOOL CNetScanOnvif::SendScanRequest()
{
	sockaddr_in OnvifSendSock		= { 0 };
	sockaddr_in	multicast_addr		= { 0 };
	SOCKET		hSendSock			= NULL;
	DWORD		dwFileSize			= 0;
	int			iSize				= 0;
	char		szMessageID[128]	= { 0 };
	int			iError				= 0;
	int			iSendBufferSize		= 0;
	char*		pszProbeSendBuffer	= NULL;
	char*		pszResolveSendBuffer = NULL;
	char*		paszOnvifURI[2]		= {"http://www.onvif.org/ver10/network/wsdl", "http://www.onvif.org/ver10/device/wsdl"};
	char*		paszProbeType[3]	= { "dp0:Device", "dp0:NetworkVideoDisplay", "dp0:NetworkVideoTransmitter" };
	//char*		paszOnvifURI[1]		= {"http://www.onvif.org/ver10/network/wsdl"};
	//char*		paszProbeType[1] = { "dp0:NetworkVideoTransmitter" };

	// probe message
	char aszProbeXml[] =
	{
		"<?xml version=\"1.0\" encoding=\"utf-8\"?>\
			<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:a=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\">\
				<s:Header>\
					<a:Action s:mustUnderstand=\"1\">http://schemas.xmlsoap.org/ws/2005/04/discovery/Probe</a:Action>\
					<a:MessageID>uuid:%s</a:MessageID>\
					<a:ReplyTo>\
						<a:Address>http://schemas.xmlsoap.org/ws/2004/08/addressing/role/anonymous</a:Address>\
					</a:ReplyTo>\
					<a:To s:mustUnderstand=\"1\">urn:schemas-xmlsoap-org:ws:2005:04:discovery</a:To>\
				</s:Header>\
				<s:Body>\
					<Probe xmlns=\"http://schemas.xmlsoap.org/ws/2005/04/discovery\">\
						<d:Types xmlns:d=\"http://schemas.xmlsoap.org/ws/2005/04/discovery\"xmlns:dp0=\"%s\">%s</Types>\
					</Probe>\
				</s:Body>\
			</Envelope>"
	};

	//char aszResolveXml[] =
	//{
	//	"<?xml version=\"1.0\" encoding=\"utf-8\"?>\
	//		<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:a=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\">\
	//			<s:Header>\
	//				<a:Action s:mustUnderstand=\"1\">http://schemas.xmlsoap.org/ws/2005/04/discovery/Resolve</a:Action>\
	//				<a:MessageID>uuid:%s</a:MessageID>\
	//				<a:ReplyTo>\
	//					<a:Address>http://schemas.xmlsoap.org/ws/2004/08/addressing/role/anonymous</a:Address>\
	//				</a:ReplyTo>\
	//				<a:To s:mustUnderstand=\"1\">urn:schemas-xmlsoap-org:ws:2005:04:discovery</a:To>\
	//			</s:Header>\
	//			<s:Body>\
	//				<d:Resolve xmlns=\"http://schemas.xmlsoap.org/ws/2005/04/discovery\">\
	//					<a:ReferenceProperties>%s</a:ReferenceProperties>\
	//				</d:Resolve>\
	//			</s:Body>\
	//		</Envelope>"
	//};


	// 소켓 생성
	if (NULL == m_hReceiveSock)
		CreateMultiCastSocket();

	if (NULL == m_hReceiveSock)
		return FALSE;

	OnvifSendSock.sin_family = AF_INET;
	OnvifSendSock.sin_port = htons(3702);
	OnvifSendSock.sin_addr.s_addr = inet_addr("239.255.255.250"); // multicast group

	int iTypeIdx = 0;
	int iUriIdx = 0;
	// Probe Type 배열 요소 갯수 만큼 Packet Send
	while (iTypeIdx < sizeof(paszProbeType) / sizeof(paszProbeType[0]))
	{
		// UUID 얻기
		GenerateMsgID(szMessageID, 127);
		strlwr(szMessageID);

		if (iTypeIdx > 1)
			iUriIdx++;

		iSendBufferSize = strlen(aszProbeXml) + strlen(szMessageID)+ strlen(paszOnvifURI[iUriIdx]) + strlen(paszProbeType[iTypeIdx]);
		pszProbeSendBuffer = new char[iSendBufferSize + 1];
		memset(pszProbeSendBuffer, 0, iSendBufferSize + 1);

		// 얻어온 UUID를 XML에 적용
		sprintf_s(pszProbeSendBuffer, iSendBufferSize, aszProbeXml, szMessageID, paszOnvifURI[iUriIdx], paszProbeType[iTypeIdx]);

		iSize = strlen(pszProbeSendBuffer);
		if (SOCKET_ERROR == sendto(m_hReceiveSock, pszProbeSendBuffer, iSize, 0, (struct sockaddr*)&OnvifSendSock, sizeof(OnvifSendSock)))
		{
			iError = WSAGetLastError();
			TRACE(_T("sendto Error = %d\n"), iError);
			closesocket(m_hReceiveSock);

			delete[] pszProbeSendBuffer;
			pszProbeSendBuffer = NULL;
			return FALSE;
		}

		if (NULL != pszProbeSendBuffer)
		{
			delete[] pszProbeSendBuffer;
			pszProbeSendBuffer = NULL;
		}

		iTypeIdx++;
	}

	m_bConnected = TRUE;

	if (NULL != pszResolveSendBuffer)
	{
		delete[] pszResolveSendBuffer;
		pszResolveSendBuffer = NULL;
	}
	
	return TRUE;
}

void CNetScanOnvif::thrOnvifReceiver()
{
	SOCKADDR		stSockAddr;
	XNode			stNode;
	XNode			stDeviceInfoNode;
	XNode			stNetworkInfoNode;
	XNode			stOnvifVersionNode;
	XNode			stVideoInfoNode;
	BOOL			bIsSuccessBind			= FALSE;
	int				iRevLen					= sizeof(sockaddr_in);
	LPXNode			lpTypeCheck				= NULL;
	LPXNode			lpBody					= NULL;
	LPXNode			lpDeviceInfoBody		= NULL;
	LPXNode			lpProfileBody = NULL;
	LPXNode			lpUUID					= NULL;
	LPXNode			lpIPAddress				= NULL;
	LPXNode			lpMAC					= NULL;
	LPXNode			lpScope					= NULL;
	LPXNode			lpSupportedVersions		= NULL;
	LPXNode			lpMajor					= NULL;
	LPXNode			lpMinor					= NULL;
	int				iHTTPPort				= 0;
	char			aszUUID[UUID_SIZE]		= { 0 };
	char			aszIPData[128]			= { 0 };
	char			aszMAC[32]				= { 0 };
	char*			pszData					= NULL;
	char			aszIPAddress[32]		= { 0 };
	char*			pszSlice				= NULL;
	char*			pszNameSlice			= NULL;
	char*			pszMacSlice				= NULL;
	DWORD			dwLastError				= 0;
	SCAN_INFO*		pScanInfo				= NULL;
	char*			pszProbeMatchType[2]	= { "wsdd:ProbeMatch", "d:ProbeMatch" };
	char*			pszAddressType[2]		= { "wsdd:XAddrs", "d:XAddrs" };
	BOOL			bIsTCPConnect			= FALSE;
	char			aszNonceResult[36]		= { 0 }; // Base64로 인코딩된 16바이트여야 한다.
	char			aszDateResult[56]		= { 0 };
	char			aszPwdDigest[DIGEST_SIZE] = { 0 };
	char			aszPwdShaHash[41]		= { 0 };
	char			aszPwdBaseHash[56]		= { 0 };
	char			aszNonceBase64[BASE64_SIZE] = { 0 };
	char aszIP[20] = "192.168.0.200";
	unsigned char	auszNonceDecode[64]		= { 0 };
	char*			pszDeviceInfoData		= NULL;
	char*			pszNetworkInfoData		= NULL;
	char*			pszOnvifVerData			= NULL;
	char*			pszVideoSourceData		= NULL;
	char*			pszMacType				= NULL;
	char			aszFirmwareVer[32]		= { 0 };

	m_pReceive_buffer = new char[SCAN_INFO_RECEIVE_BUFFER_SIZE];
	memset(m_pReceive_buffer, 0, sizeof(char) * SCAN_INFO_RECEIVE_BUFFER_SIZE);

	while (this->m_dwScanThreadID)
	{
		if (NULL == m_hReceiveSock || FALSE == m_bConnected)
		{
			Sleep(100);
			continue;
		}
		
		if (SOCKET_ERROR == recvfrom(m_hReceiveSock, m_pReceive_buffer, SCAN_INFO_RECEIVE_BUFFER_SIZE, 0, (SOCKADDR*)&stSockAddr, &iRevLen))
		{
			dwLastError = WSAGetLastError();
			TRACE(_T("recvfrom error=%d\n"), dwLastError);

			if (this->m_hNotifyWnd && dwLastError != WSAEINTR && dwLastError != WSAEINVAL)
				::PostMessage(this->m_hNotifyWnd, this->m_lNotifyMsg, 0, SCAN_ERR_RECV);
			
			this->ThreadExit();
			break;
		}

		m_bIsProbeRev = TRUE;

		if ( 0 < strlen(m_pReceive_buffer) )
		{
			// 파싱할 데이터
			stNode.Load(m_pReceive_buffer);
			lpBody = stNode.GetChildArg("SOAP-ENV:Body", NULL);
			if (NULL != lpBody)
			{
				lpUUID = lpBody->GetChildArg("wsa:Address", NULL);
				if (NULL != lpUUID)
				{
					// urn:uuid:XXXXXX-XXXXXX-XXXX 에서 urn:uuid: 문자열을 자름
					lpUUID->value = lpUUID->value.Right(36);
					strcpy(&aszUUID[0], lpUUID->value);
					// 배열에 uuid 쌓고 다 쌓았으면 uuid를Resolve Message에 담아서 다시 Send
				}
			}

			//::OutputDebugStringA("ONVIF RECEIVE DEVICE DATA -----------------------\n");
			//::OutputDebugStringA(aszIPAddress);
			//::OutputDebugStringA("\n");
			//::OutputDebugStringA(m_pReceive_buffer);
			//::OutputDebugStringA("\n");

			// table input Data
			pScanInfo = new SCAN_INFO;
			if (pScanInfo)
			{
				memset(pScanInfo, 0, sizeof(SCAN_INFO));
				// IP Parsing

				lpTypeCheck = ( NULL != stNode.GetChildArg("wsdd:ProbeMatch", NULL) ) ? stNode.GetChildArg("wsdd:ProbeMatch", NULL) : stNode.GetChildArg("d:ProbeMatch", NULL);

				if (NULL != lpTypeCheck)
				{
					lpIPAddress = ( NULL != lpTypeCheck->GetChildArg("wsdd:XAddrs", NULL) ) ? lpTypeCheck->GetChildArg("wsdd:XAddrs", NULL) : lpTypeCheck->GetChildArg("d:XAddrs", NULL);

					// IP Address
					if (NULL != lpIPAddress)
					{
						int iIndex = 0;
						strcpy(&aszIPData[0], lpIPAddress->value);
						pszSlice = strtok(aszIPData, ":");
						
						while (NULL != pszSlice)
						{
							pszSlice = strtok(NULL, ":");
							if (1 == iIndex)
								break;

							iIndex++;
						}

						if (pszSlice)
						{
							pszSlice = strtok(pszSlice, "/");
							if (10 > strlen(pszSlice))
							{
								iHTTPPort = atoi(pszSlice);
							}
						}
						else
							iHTTPPort = 80;

						pScanInfo->nHTTPPort = iHTTPPort;

						lpIPAddress->value = lpIPAddress->value.Left(20);
						lpIPAddress->value = lpIPAddress->value.Right(13);
						strcpy(&aszIPAddress[0], lpIPAddress->value);
						//strcpy(&aszIPAddress[0], aszIP);

						if (0 == strcmp(aszIP, aszIPAddress))
						{
							TRACE(_T("TRUE"));
						}

						if (strlen(aszIPAddress) > 0)
						{
							this->WideCopyStringFromAnsi(pScanInfo->szAddr, 32, aszIPAddress);
						}

						pszDeviceInfoData = new char[sizeof(char)* SCAN_INFO_RECEIVE_BUFFER_SIZE];
						memset(&pszDeviceInfoData[0], 0, sizeof(char)* SCAN_INFO_RECEIVE_BUFFER_SIZE);

						pszNetworkInfoData = new char[sizeof(char)* SCAN_INFO_RECEIVE_BUFFER_SIZE];
						memset(&pszNetworkInfoData[0], 0, sizeof(char)* SCAN_INFO_RECEIVE_BUFFER_SIZE);

						pszOnvifVerData = new char[sizeof(char)* SCAN_INFO_RECEIVE_BUFFER_SIZE];
						memset(&pszOnvifVerData[0], 0, sizeof(char)* SCAN_INFO_RECEIVE_BUFFER_SIZE);
						
						pszVideoSourceData = new char[sizeof(char)* SCAN_INFO_RECEIVE_BUFFER_SIZE];
						memset(&pszVideoSourceData[0], 0, sizeof(char)* SCAN_INFO_RECEIVE_BUFFER_SIZE);

						//// ProbeMatch에서 받은 IP갯수 만큼 반복문으로 돌릴 예정 - 지금은 1개로 테스트 중
						GetAuthenticateData(aszIPAddress, iHTTPPort, &aszDateResult[0], &aszNonceResult[0], &pszDeviceInfoData[0]); // Device 날짜 얻기
						
						if (0 == strlen(aszNonceResult))
						{
							strcpy(aszNonceResult, __aszNonce);
						}

						::OutputDebugStringA(pszDeviceInfoData);
						::OutputDebugStringA("\n");

						// Onvif Version 얻어오기
						GetOnvifVersion(aszIPAddress, iHTTPPort, &pszOnvifVerData[0]);
						
						// Onvif Version Data 전처리
						if (0 < strlen(pszOnvifVerData))
						{
							stOnvifVersionNode.Load(pszOnvifVerData);
							lpSupportedVersions = stOnvifVersionNode.GetChildArg("tt:SupportedVersions", NULL);
							char aszMajor[4] = { 0 };
							char aszMinor[4] = { 0 };
							char aszVersion[8] = { 0 };
							if (NULL != lpSupportedVersions)
							{
								lpMajor = lpSupportedVersions->GetChildArg("tt:Major", NULL);
								if (NULL != lpMajor)
									strcpy(aszMajor, lpMajor->value);
								
								lpMinor = lpSupportedVersions->GetChildArg("tt:Minor", NULL);
								if (NULL != lpMinor)
									strcpy(aszMinor, lpMinor->value);

							}

							sprintf_s(&aszVersion[0], sizeof(char) * 8, "%d.%d", atoi(aszMajor), atoi(aszMinor));
							this->WideCopyStringFromAnsi(pScanInfo->szSwVersion, 30, aszVersion);
						}
						else
							wsprintf(pScanInfo->szSwVersion, _T("N/A"));


						// 인증 로직
						if (0 < strlen(aszNonceResult))
						{
							Base64Encoding(aszNonceResult, strlen(aszNonceResult), &aszNonceBase64[0]); // nonce값 Base64로 인코딩
							//Base64Decoding(aszNonce, &auszNonceDecode[0], sizeof(auszNonceDecode));

							sprintf_s(aszPwdDigest, sizeof(aszPwdDigest), "%s%s%s", aszNonceResult, aszDateResult, m_aszPassword);

							SHA1Encoding(aszPwdDigest, &aszPwdShaHash[0]);
							Base64Encoding(aszPwdShaHash, SHA_DIGEST_LENGTH, &aszPwdBaseHash[0]);

							// Firmware Version 얻어오기
							GetDeviceInfo(aszIPAddress, iHTTPPort, aszPwdBaseHash, aszNonceBase64, aszDateResult, &pszDeviceInfoData[0]);
						}

						// MAC 어드레스 가져오기
						GetNetworkInterface(aszIPAddress, iHTTPPort, aszPwdBaseHash, aszNonceBase64, aszDateResult, &pszNetworkInfoData[0]);

						if ( 0 < strlen(pszNetworkInfoData) )
						{
							stNetworkInfoNode.Load(pszNetworkInfoData);
							lpMAC = stNetworkInfoNode.GetChildArg("tt:HwAddress", NULL);
							if (NULL != lpMAC )
							{
								if (1 < lpMAC->value.GetLength())
								{
									strcpy(&aszMAC[0], lpMAC->value);
									
									for (int i = 0; i < strlen(aszMAC); i++)
									{
										if (aszMAC[i] >= 'A' && aszMAC[i] <= 'Z')
										{
											aszMAC[i] += 32;
										}
									}

									this->WideCopyStringFromAnsi(pScanInfo->szMAC, 32, aszMAC);
								}	
								else
									wsprintf(pScanInfo->szMAC, _T("N/A"));
							}
							else
							{
								int iHTTPStatus = 0;
								iHTTPStatus = atoi(pszNetworkInfoData);

								switch (iHTTPStatus)
								{
								case BAD_REQUEST:
									wsprintf(pScanInfo->szMAC, _T("BAD_REQUEST"));
									break;

								case UNAUTHORIZED:
									wsprintf(pScanInfo->szMAC, _T("UNAUTHORIZED"));
									break;

								case NOT_DATA:
									wsprintf(pScanInfo->szMAC, _T("N/A"));
									break;
								}
							}
						}
						else
							wsprintf(pScanInfo->szMAC, _T("N/A"));

						if (0 < strlen(pszDeviceInfoData))
						{
							stDeviceInfoNode.Load(pszDeviceInfoData);
							lpDeviceInfoBody = stDeviceInfoNode.GetChildArg("tds:GetDeviceInformationResponse", NULL);

							if (NULL != lpDeviceInfoBody)
								lpDeviceInfoBody = lpDeviceInfoBody->GetChildArg("tds:FirmwareVersion", NULL);

							// FirwareVersion Value Check
							if (NULL != lpDeviceInfoBody)
							{
								if (1 < lpDeviceInfoBody->value.GetLength())
								{
									strcpy(&aszFirmwareVer[0], lpDeviceInfoBody->value);
									this->WideCopyStringFromAnsi(pScanInfo->szFirmwareVer, 32, aszFirmwareVer);
								}
								else
									wsprintf(pScanInfo->szFirmwareVer, _T("N/A"));
							}
							else
							{
								int iHTTPStatus = 0;
								iHTTPStatus = atoi(pszDeviceInfoData);

								switch (iHTTPStatus)
								{
								case BAD_REQUEST:
									wsprintf(pScanInfo->szFirmwareVer, _T("BAD_REQUEST"));
									break;

								case UNAUTHORIZED:
									wsprintf(pScanInfo->szFirmwareVer, _T("UNAUTHORIZED"));
									break;

								case NOT_DATA:
									wsprintf(pScanInfo->szFirmwareVer, _T("N/A"));
									break;
								}
							}
						}
						else
							wsprintf(pScanInfo->szFirmwareVer, _T("N/A"));

						// Channel Cnt 가져오기
						//GetProfile(aszIPAddress, iHTTPPort, aszPwdBaseHash, aszNonceBase64, aszDateResult, &pszVideoSourceData[0]);

						//if (0 < pszVideoSourceData)
						//{
						//	LPXNode lpToken = NULL;
						//	stVideoInfoNode.Load(pszVideoSourceData);
						//	//lpProfileBody = stVideoInfoNode.GetChildArg("trt:GetProfilesResponse", NULL);
						//	//lpToken = lpProfileBody->GetChildArg("trt:Profiles", NULL);
						//	//lpToken = lpProfileBody->GetChildArg("trt:Profiles", NULL);
						//}
						
					}

					lpScope = ( NULL != lpTypeCheck->GetChildArg("wsdd:Scopes", NULL) ) ? lpTypeCheck->GetChildArg("wsdd:Scopes", NULL) : lpTypeCheck->GetChildArg("d:Scopes", NULL);

					if (NULL != lpScope)
					{
						pszData = new char[sizeof(char)* lpScope->value.GetLength() + 1];
						strcpy(&pszData[0], lpScope->value);

						pszNameSlice = strstr(pszData, "name"); // return NULL
						pszMacSlice = (NULL != strstr(pszData, "macaddress") ? strstr(pszData, "macaddress") : strstr(pszData, "mac"));

						if (NULL != pszNameSlice)
						{
							pszNameSlice = strtok(pszNameSlice, " ");
							pszNameSlice = strncpy(pszNameSlice, pszNameSlice + 5, 20);

							this->WideCopyStringFromAnsi(pScanInfo->szModelName, 30, pszNameSlice);
						}
						else
							wsprintf(pScanInfo->szModelName, _T("N/A"));
					}

					pScanInfo->iBasePort = 0;
					pScanInfo->iVideoCnt = 0;

					if (this->m_hNotifyWnd)
						::PostMessage(this->m_hNotifyWnd, this->m_lNotifyMsg, (WPARAM)pScanInfo, 0);
				}
			}
		}

		if (NULL != pszData)
		{
			delete[] pszData;
			pszData = NULL;
		}

		if (NULL != pszDeviceInfoData)
		{
			delete[] pszDeviceInfoData;
			pszDeviceInfoData = NULL;
		}
		
		if (NULL != pszNetworkInfoData)
		{
			delete[] pszNetworkInfoData;
			pszNetworkInfoData = NULL;
		}
		
		//동적 할당 변수 너무 많음
		if (NULL != pszOnvifVerData)
		{
			delete[] pszOnvifVerData;
			pszOnvifVerData = NULL;
		}

		if (NULL != pszVideoSourceData)
		{
			delete[] pszVideoSourceData;
			pszVideoSourceData = NULL;
		}
	}

	return;
}


BOOL CNetScanOnvif::GenerateMsgID(char* szMessageID, int nBufferLen)
{
	if (szMessageID == NULL || nBufferLen == 0)
		return FALSE;

	GUID guid;
	WCHAR wszGUID[129];

	USES_CONVERSION;
	memset(wszGUID, 0, sizeof(WCHAR)* 129);
	CoCreateGuid(&guid);
	StringFromGUID2(guid, wszGUID, 128); // ex. {D23370B9-3007-47d7-BAEA-30DDD6B0D24B}
	int nGUIDStringLen = wcslen(wszGUID);

	strncpy_s(szMessageID, nBufferLen, W2A(wszGUID) + 1, nGUIDStringLen - 2);

	return TRUE;
}



BOOL CNetScanOnvif::ConnectTCPSocket(char* pszIP, int iPort)
{
	sockaddr_in		HTTPSendSock	= { 0 };
	int				iError			= 0;

	m_TcpSocket = socket(PF_INET, SOCK_STREAM, 0);

	HTTPSendSock.sin_family = AF_INET;
	HTTPSendSock.sin_port = htons(iPort);
	HTTPSendSock.sin_addr.s_addr = inet_addr(pszIP);

	// --------------------------------------------------------------------------------
	// GetSystemDate Send & Recv 로직
	if (SOCKET_ERROR == connect(m_TcpSocket, (SOCKADDR*)&HTTPSendSock, sizeof(SOCKADDR)))
	{
		iError = WSAGetLastError();
		TRACE(_T("TCP-HTTP Connect Error = %d\n"), iError);
		closesocket(m_TcpSocket);

		return FALSE;
	}

	return TRUE;
}


// Device Info Request
void CNetScanOnvif::SendDeviceInfo(char* pszIP, int iPort, char* pszNonceResult, char* pszGetData)
{
	XNode			stNode;
	LPXNode			lpBody				= NULL;
	char*			pszRecvBuffer		= NULL;
	char*			pszSendBuffer		= NULL;
	int				iSendDataSize		= 0;
	int				iError				= 0;
	sockaddr_in		HTTPSendSock		= { 0 };
	char*			pszSendAuthBuff		= NULL;
	char*			pszSlice			= NULL;
	char*			pszSliceNonce		= NULL;
	int				iHTTPStatus			= 0;
	char*			pszStrCompare		= "nonce";
	BOOL			bIsConnect			= FALSE;
	int				iSendBufferSize		= 0;
	char			aszCopyBuffer[20] = { 0 };
	

	char aszGetSystemTime[] = "POST /onvif/device_service HTTP/1.1\r\nContent-Type: application/soap+xml; charset=utf-8; action=\"http://www.onvif.org/ver10/device/wsdl/GetSystemDateAndTime\"\r\nHost: %s\r\nContent-Length: %d\r\nAccept-Encoding: gzip, deflate\r\nConnection: Close\r\n\r\n";
	
	char aszDeviceInformation[] = 
	{
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
			<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:tds=\"http://www.onvif.org/ver10/device/wsdl\">\
				<SOAP-ENV:Body>\
					<tds:GetDeviceInformation/>\
				</SOAP-ENV:Body>\
			</SOAP-ENV:Envelope>" // UserToken Header - GetDeviceInformation Body
	};

	if (0 < strlen(pszNonceResult))
		memset(pszNonceResult, 0, sizeof(char) * 36);

	bIsConnect = ConnectTCPSocket(pszIP, iPort);

	if (CONNECT_SUCCESS == bIsConnect)
	{
		iSendBufferSize = strlen(aszGetSystemTime) + strlen(aszDeviceInformation) + strlen(pszIP) + 4;
		
		pszSendBuffer = new char[sizeof(char) * iSendBufferSize + 1];
		memset(&pszSendBuffer[0], 0, iSendBufferSize + 1);

		pszRecvBuffer = new char[SCAN_INFO_RECEIVE_BUFFER_SIZE];
		memset(&pszRecvBuffer[0], 0, SCAN_INFO_RECEIVE_BUFFER_SIZE);

		sprintf_s(pszSendBuffer, sizeof(char) * iSendBufferSize, aszGetSystemTime, pszIP, strlen(aszDeviceInformation));
		strcat_s(pszSendBuffer, sizeof(char) * iSendBufferSize, aszDeviceInformation);

		iSendDataSize = strlen(pszSendBuffer);
		if ( SOCKET_ERROR == send(m_TcpSocket, pszSendBuffer, iSendDataSize, 0) )
		{
			iError = WSAGetLastError();
			TRACE(_T("TCP-HTTP send Error = %d\n"), iError);
			closesocket(m_TcpSocket);
		}

		if ( SOCKET_ERROR == recv(m_TcpSocket, pszRecvBuffer, sizeof(char) * SCAN_INFO_RECEIVE_BUFFER_SIZE, 0) )
		{
			iError = WSAGetLastError();
			TRACE(_T("TCP-HTTP recv Error = %d\n"), iError);
			closesocket(m_TcpSocket);
		}

		::OutputDebugStringA("ONVIF RECEIVE DEVICE DATA -----------------------\n");
		::OutputDebugStringA(pszIP);
		::OutputDebugStringA("\n");
		::OutputDebugStringA(pszRecvBuffer);
		::OutputDebugStringA("\n");
		
		if (0 < strlen(pszRecvBuffer))
		{
			memcpy(&aszCopyBuffer[0], pszRecvBuffer, 20);

			// 401 Status 찾기
			pszSlice = strtok(aszCopyBuffer, " ");
			pszSlice = strtok(NULL, " ");
			iHTTPStatus = atoi(pszSlice);

			// 200 일때
			if (RES_SUCCESS == iHTTPStatus)
			{
				stNode.Load(pszRecvBuffer);
				lpBody = stNode.GetChildArg("SOAP-ENV:Fault", NULL);

				if (NULL != lpBody)
					sprintf_s(pszGetData, sizeof(int), "%d", UNAUTHORIZED);
				
				else
					strcpy_s(pszGetData, sizeof(char)* SCAN_INFO_RECEIVE_BUFFER_SIZE, pszRecvBuffer);
				
				pszNonceResult = NULL;
			}

			if (UNAUTHORIZED == iHTTPStatus)
			{
				pszSlice = strtok(pszRecvBuffer, " ");
				pszSlice = strtok(NULL, " ");
				while (NULL != pszRecvBuffer)
				{
					// nonce 찾기
					pszSlice = strtok(NULL, ", =");
					if (0 == strcmp(pszSlice, pszStrCompare))
					{
						// Hash 값 얻기
						pszSliceNonce = strtok(NULL, "\" \"");
						strcat_s(pszNonceResult, sizeof(char)* strlen(pszSliceNonce) + 1, pszSliceNonce);
						pszGetData = NULL;
						break;
					}
				}
			}
		}
	}

	if (NULL != pszSendBuffer)
	{
		delete[] pszSendBuffer;
		pszSendBuffer = NULL;
	}

	if (NULL != pszRecvBuffer)
	{
		delete[] pszRecvBuffer;
		pszRecvBuffer = NULL;
	}

}

void CNetScanOnvif::GetOnvifVersion(char* pszIP, int iPort, char* pszGetData)
{
	XNode			stNode;
	char*			pszRecvBuffer = NULL;
	char*			pszSendBuffer = NULL;
	int				iSendDataSize = 0;
	int				iError = 0;
	char*			pszSlice = NULL;
	int				iHTTPStatus = 0;
	BOOL			bIsConnect = FALSE;
	int				iSendBufferSize = 0;
	char			aszCopyBuffer[20] = { 0 };
	
	char aszGetSystemTime[] = "POST /onvif/device_service HTTP/1.1\r\nContent-Type: application/soap+xml; charset=utf-8; action=\"http://www.onvif.org/ver10/device/wsdl/GetCapabilities\"\r\nHost: %s\r\nContent-Length: %d\r\nAccept-Encoding: gzip, deflate\r\nConnection: Close\r\n\r\n";

	char aszDeviceInformation[] = 
	{
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
			<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:tds=\"http://www.onvif.org/ver10/device/wsdl\">\
				<SOAP-ENV:Body>\
					<tds:GetCapabilities>\
						<tds:Category>All</tds:Category>\
					</tds:GetCapabilities>\
				</SOAP-ENV:Body>\
			</SOAP-ENV:Envelope>" // UserToken Header - GetDeviceInformation Body
	};

	bIsConnect = ConnectTCPSocket(pszIP, iPort);

	if (CONNECT_SUCCESS == bIsConnect)
	{
		iSendBufferSize = strlen(aszGetSystemTime) + strlen(aszDeviceInformation) + strlen(pszIP) + 4;

		pszSendBuffer = new char[sizeof(char)* iSendBufferSize + 1];
		memset(&pszSendBuffer[0], 0, iSendBufferSize + 1);

		pszRecvBuffer = new char[SCAN_INFO_RECEIVE_BUFFER_SIZE];
		memset(&pszRecvBuffer[0], 0, SCAN_INFO_RECEIVE_BUFFER_SIZE);

		sprintf_s(pszSendBuffer, sizeof(char)* iSendBufferSize, aszGetSystemTime, pszIP, strlen(aszDeviceInformation));
		strcat_s(pszSendBuffer, sizeof(char)* iSendBufferSize, aszDeviceInformation);

		iSendDataSize = strlen(pszSendBuffer);
		if (SOCKET_ERROR == send(m_TcpSocket, pszSendBuffer, iSendDataSize, 0))
		{
			iError = WSAGetLastError();
			TRACE(_T("TCP-HTTP send Error = %d\n"), iError);
			closesocket(m_TcpSocket);
		}

		if (SOCKET_ERROR == recv(m_TcpSocket, pszRecvBuffer, sizeof(char)* SCAN_INFO_RECEIVE_BUFFER_SIZE, 0))
		{
			iError = WSAGetLastError();
			TRACE(_T("TCP-HTTP recv Error = %d\n"), iError);
			closesocket(m_TcpSocket);
		}

		if (0 < strlen(pszRecvBuffer))
		{
			memcpy(&aszCopyBuffer[0], pszRecvBuffer, 20);

			// 401 Status 찾기
			pszSlice = strtok(aszCopyBuffer, " ");
			pszSlice = strtok(NULL, " ");
			iHTTPStatus = atoi(pszSlice);

			// 200 일때
			if (RES_SUCCESS == iHTTPStatus)
			{
				strcpy_s(pszGetData, sizeof(char)* SCAN_INFO_RECEIVE_BUFFER_SIZE, pszRecvBuffer);
			}
		}
	}

	if (NULL != pszSendBuffer)
	{
		delete[] pszSendBuffer;
		pszSendBuffer = NULL;
	}

	if (NULL != pszRecvBuffer)
	{
		delete[] pszRecvBuffer;
		pszRecvBuffer = NULL;
	}
}


// SystemDate Request
void CNetScanOnvif::GetAuthenticateData(char* pszIP, int iPort, char* pszDateResult, char* pszNonceResult, char* pszGetData)
{
	XNode		stNode;
	LPXNode		lpaDateType[6]		= { 0 }; // CStringA
	LPXNode		lpaTimeData[6]		= { 0 };
	LPXNode		lpBody				= NULL;
	char*		paiDate[6]			= { 0 };
	char*		paszDateType[2]		= { "tt:Date", "tt:Time" };
	char*		paszChild[6]		= { "tt:Year", "tt:Month", "tt:Day", "tt:Hour", "tt:Minute", "tt:Second" };
	char		aszTime[32]			= { 0 };
	char*		pszRecvBuffer		= NULL;
	char*		pszSendBuffer		= NULL;
	BOOL		bIsConnect			= FALSE;
	int			iSendBufferSize		= 0;
	int			iError				= 0;
	BOOL		bIsGetNonce			= FALSE;

	char aszGetSystemTime[] = "POST /onvif/device_service HTTP/1.1\r\nContent-Type: application/soap+xml; charset=utf-8; action=\"http://www.onvif.org/ver10/device/wsdl/GetSystemDateAndTime\"\r\nHost: %s\r\nContent-Length: %d\r\nAccept-Encoding: gzip, deflate\r\nConnection: Close\r\n\r\n";

	char aszSystemDateXml[] =
	{
		"<?xml version=\"1.0\" encoding=\"utf-8\"?>\
		<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\">\
			<s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">\
				<GetSystemDateAndTime xmlns=\"http://www.onvif.org/ver10/device/wsdl\"/>\
			</s:Body>\
		</s:Envelope>"
	};

	bIsConnect = ConnectTCPSocket(pszIP, iPort);

	iSendBufferSize = strlen(aszGetSystemTime) + strlen(pszIP) + strlen(aszSystemDateXml) + 4;
	pszSendBuffer = new char[iSendBufferSize + 1];
	memset(&pszSendBuffer[0], 0, iSendBufferSize + 1);

	pszRecvBuffer = new char[SCAN_INFO_RECEIVE_BUFFER_SIZE];
	memset(&pszRecvBuffer[0], 0, SCAN_INFO_RECEIVE_BUFFER_SIZE);

	sprintf_s(pszSendBuffer, sizeof(char)* iSendBufferSize, aszGetSystemTime, pszIP, strlen(aszSystemDateXml));
	strcat_s(pszSendBuffer, sizeof(char)* iSendBufferSize, aszSystemDateXml);

	if (CONNECT_SUCCESS == bIsConnect)
	{
		
		if (SOCKET_ERROR == send(m_TcpSocket, pszSendBuffer, iSendBufferSize, 0))
		{
			iError = WSAGetLastError();
			TRACE(_T("TCP-HTTP send Error = %d\n"), iError);
			closesocket(m_TcpSocket);
		}

		if (SOCKET_ERROR == recv(m_TcpSocket, pszRecvBuffer, SCAN_INFO_RECEIVE_BUFFER_SIZE, 0))
		{
			iError = WSAGetLastError();
			TRACE(_T("TCP-HTTP recv Error = %d\n"), iError);
			closesocket(m_TcpSocket);
		}

		stNode.Load(pszRecvBuffer);
		lpBody = stNode.GetChildArg("tt:UTCDateTime", NULL);

		if (lpBody)
		{
			int index = 0;
			int iArrLen = sizeof(lpaTimeData) / sizeof(lpaTimeData[0]);

			while (index < iArrLen)
			{
				// 1~3 Date , 4~6 Time
				if (index < iArrLen / 2)
					lpaDateType[index] = lpBody->GetChildArg(paszDateType[0], NULL);
				else
					lpaDateType[index] = lpBody->GetChildArg(paszDateType[1], NULL);

				lpaTimeData[index] = lpaDateType[index]->GetChildArg(paszChild[index], NULL);

				if (NULL != lpaTimeData[index])
				{
					paiDate[index] = lpaTimeData[index]->value.GetBuffer(0);
				}
				index++;
			}
			sprintf_s(aszTime, sizeof(aszTime), "%04d-%02d-%02dT%02d:%02d:%02d.000Z",
				atoi(paiDate[0]),
				atoi(paiDate[1]),
				atoi(paiDate[2]),
				atoi(paiDate[3]),
				atoi(paiDate[4]),
				atoi(paiDate[5])
				);

		} // Created 날짜 가져오기 
	}

	memcpy(pszDateResult, aszTime, sizeof(char) * strlen(aszTime));

	SendDeviceInfo(pszIP, iPort, &pszNonceResult[0], &pszGetData[0]); // Nonce 해쉬 얻기

	if (NULL != pszSendBuffer)
	{
		delete[] pszSendBuffer;
		pszSendBuffer = NULL;
	}

	if (NULL != pszRecvBuffer)
	{
		delete[] pszRecvBuffer;
		pszRecvBuffer = NULL;
	}
}

void CNetScanOnvif::GetDeviceInfo(char* pszIP, int iPort, char* pszDigest, char* pszNonceResult, char* pszDateResult, char* pszGetData)
{
	BOOL		bIsConnect			= FALSE;
	char*		pszRecvBuffer		= NULL;
	char*		pszPacketBuffer		= NULL;
	char*		pszSendBuffer		= NULL;
	char*		pszSlice			= NULL;
	int			iHeaderSize			= 0;
	int			iBodySize			= 0;
	int			iPacketSize			= 0;
	int			iError				= 0;
	int			iContentLen			= 0;
	int			iHTTPStatus			= 0;
	char		aszCopyBuffer[20]	= { 0 };

	char aszDeviceInfoHeader[] = "POST /onvif/device_service HTTP/1.1\r\nContent-Type: application/soap+xml; charset=utf-8; action=\"http://www.onvif.org/ver10/device/wsdl/GetDeviceInformation\"\r\nHost: %s\r\nContent-Length: %d\r\nAccept-Encoding: gzip, deflate\r\nConnection: Close\r\n\r\n";

	char aszDeviceInformationXML[] = 
	{
		"<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\">\
			<s:Header>\
				<Security s:mustUnderstand=\"1\" xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\">\
					<UsernameToken>\
						<Username>%s</Username>\
						<Password Type=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest\">%s</Password>\
						<Nonce EncodingType=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary\">%s</Nonce>\
						<Created xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\">%s</Created>\
					</UsernameToken>\
				</Security>\
			</s:Header>\
			<s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">\
				<GetDeviceInformation xmlns=\"http://www.onvif.org/ver10/device/wsdl\"/>\
			</s:Body>\
		</s:Envelope>" // UserToken Header - GetDeviceInformation Body
	};
	
	bIsConnect = ConnectTCPSocket(pszIP, iPort);

	iHeaderSize = strlen(aszDeviceInfoHeader) + strlen(pszIP) + 3;
	iBodySize = strlen(aszDeviceInformationXML) + strlen(pszDigest) + strlen(pszNonceResult) + strlen(pszDateResult) + strlen(m_aszUserName);
	iPacketSize = iHeaderSize + iBodySize;
	iContentLen = iBodySize;

	pszPacketBuffer = new char[iPacketSize + 1];
	memset(&pszPacketBuffer[0], 0, iPacketSize + 1);

	sprintf_s(pszPacketBuffer, sizeof(char)* iHeaderSize, "%s", aszDeviceInfoHeader);
	strcat_s(pszPacketBuffer, sizeof(char)* iPacketSize, aszDeviceInformationXML);

	pszSendBuffer = new char[iPacketSize + 1];
	memset(&pszSendBuffer[0], 0, iPacketSize + 1);
	sprintf_s(pszSendBuffer, sizeof(char)* iPacketSize, pszPacketBuffer, pszIP, iContentLen, m_aszUserName, pszDigest, pszNonceResult, pszDateResult);

	pszRecvBuffer = new char[SCAN_INFO_RECEIVE_BUFFER_SIZE];
	memset(&pszRecvBuffer[0], 0, SCAN_INFO_RECEIVE_BUFFER_SIZE);



	if (CONNECT_SUCCESS == bIsConnect)
	{
		if (SOCKET_ERROR == send(m_TcpSocket, pszSendBuffer, iPacketSize, 0))
		{
			iError = WSAGetLastError();
			TRACE(_T("TCP-HTTP send Error = %d\n"), iError);
			closesocket(m_TcpSocket);
		}

		if (SOCKET_ERROR == recv(m_TcpSocket, pszRecvBuffer, SCAN_INFO_RECEIVE_BUFFER_SIZE, 0))
		{
			iError = WSAGetLastError();
			TRACE(_T("TCP-HTTP recv Error = %d\n"), iError);
			closesocket(m_TcpSocket);
		}

		//::OutputDebugStringA("ONVIF RECEIVE DEVICE DATA -----------------------\n");
		//::OutputDebugStringA(pszIP);
		::OutputDebugStringA("\n");
		::OutputDebugStringA(pszRecvBuffer);
		//::OutputDebugStringA("\n");


		if (0 < strlen(pszRecvBuffer))
		{
			memcpy(&aszCopyBuffer[0], pszRecvBuffer, 20);
			pszSlice = strstr(aszCopyBuffer, " ");
			if (NULL != pszSlice)
			{
				pszSlice = strtok(pszSlice, " ");
				iHTTPStatus = atoi(pszSlice);

				switch (iHTTPStatus)
				{
					case RES_SUCCESS:
						strcpy_s(pszGetData, sizeof(char)* SCAN_INFO_RECEIVE_BUFFER_SIZE, pszRecvBuffer);
						break;

					case BAD_REQUEST:
						sprintf_s(pszGetData, sizeof(int), "%d", BAD_REQUEST);
						break;

					case UNAUTHORIZED:
						sprintf_s(pszGetData, sizeof(int), "%d", UNAUTHORIZED);
						break;

					default:
						break;
				}
			}
		}
	}

	if (NULL != pszPacketBuffer)
	{
		delete[] pszPacketBuffer;
		pszPacketBuffer = NULL;
	}

	if (NULL != pszSendBuffer)
	{
		delete[] pszSendBuffer;
		pszSendBuffer = NULL;
	}

	if (NULL != pszRecvBuffer)
	{
		delete[] pszRecvBuffer;
		pszRecvBuffer = NULL;
	}
}


void CNetScanOnvif::GetNetworkInterface(char* pszIP, int iPort, char* pszDigest, char* pszNonceResult, char* pszDateResult, char* pszGetData)
{
	BOOL		bIsConnect			= FALSE;
	char*		pszRecvBuffer		= NULL;
	char*		pszPacketBuffer		= NULL;
	char*		pszSendBuffer		= NULL;
	char*		pszSlice			= NULL;
	int			iHeaderSize			= 0;
	int			iBodySize			= 0;
	int			iPacketSize			= 0;
	int			iError				= 0;
	int			iContentLen			= 0;
	int			iHTTPStatus			= 0;
	char		aszCopyBuffer[20]	= { 0 };

	char aszDeviceInfoHeader[] = "POST /onvif/device_service HTTP/1.1\r\nContent-Type: application/soap+xml; charset=utf-8; action=\"http://www.onvif.org/ver10/device/wsdl/GetNetworkInterfaces\"\r\nHost: %s\r\nContent-Length: %d\r\nAccept-Encoding: gzip, deflate\r\nConnection: Close\r\n\r\n";

	char aszDeviceInformationXML[] = 
	{
		"<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\">\
			<s:Header>\
				<Security s:mustUnderstand=\"1\" xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\">\
					<UsernameToken>\
						<Username>%s</Username>\
						<Password Type=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest\">%s</Password>\
						<Nonce EncodingType=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary\">%s</Nonce>\
						<Created xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\">%s</Created>\
					</UsernameToken>\
				</Security>\
			</s:Header>\
			<s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">\
				<GetNetworkInterfaces xmlns=\"http://www.onvif.org/ver10/device/wsdl\"/>\
			</s:Body>\
		</s:Envelope>" // UserToken Header - GetNetworkInterfaces Body
	};

	bIsConnect = ConnectTCPSocket(pszIP, iPort);

	iHeaderSize = strlen(aszDeviceInfoHeader) + strlen(pszIP) + 3;
	iBodySize = strlen(aszDeviceInformationXML) + strlen(pszDigest) + strlen(pszNonceResult) + strlen(pszDateResult) + strlen(m_aszUserName);
	iPacketSize = iHeaderSize + iBodySize;
	iContentLen = iBodySize;

	pszPacketBuffer = new char[iPacketSize + 1];
	memset(&pszPacketBuffer[0], 0, iPacketSize + 1);

	sprintf_s(pszPacketBuffer, sizeof(char)* iHeaderSize, "%s", aszDeviceInfoHeader);
	strcat_s(pszPacketBuffer, sizeof(char)* iPacketSize, aszDeviceInformationXML);

	pszSendBuffer = new char[iPacketSize + 1];
	memset(&pszSendBuffer[0], 0, iPacketSize + 1);
	sprintf_s(pszSendBuffer, sizeof(char)* iPacketSize, pszPacketBuffer, pszIP, iContentLen, m_aszUserName, pszDigest, pszNonceResult, pszDateResult);

	pszRecvBuffer = new char[SCAN_INFO_RECEIVE_BUFFER_SIZE];
	memset(&pszRecvBuffer[0], 0, SCAN_INFO_RECEIVE_BUFFER_SIZE);


	if (CONNECT_SUCCESS == bIsConnect)
	{
		if (SOCKET_ERROR == send(m_TcpSocket, pszSendBuffer, iPacketSize, 0))
		{
			iError = WSAGetLastError();
			TRACE(_T("TCP-HTTP send Error = %d\n"), iError);
			closesocket(m_TcpSocket);
		}

		if (SOCKET_ERROR == recv(m_TcpSocket, pszRecvBuffer, SCAN_INFO_RECEIVE_BUFFER_SIZE, 0))
		{
			iError = WSAGetLastError();
			TRACE(_T("TCP-HTTP recv Error = %d\n"), iError);
			closesocket(m_TcpSocket);
		}
		//::OutputDebugStringA("ONVIF RECEIVE DEVICE DATA -----------------------\n");
		//::OutputDebugStringA(pszIP);
		::OutputDebugStringA("\n");
		::OutputDebugStringA(pszRecvBuffer);
		//::OutputDebugStringA("\n");

		if (0 < strlen(pszRecvBuffer))
		{
			memcpy(&aszCopyBuffer[0], pszRecvBuffer, 20);
			pszSlice = strstr(aszCopyBuffer, " ");
			if (NULL != pszSlice)
			{
				pszSlice = strtok(pszSlice, " ");
				iHTTPStatus = atoi(pszSlice);

				switch (iHTTPStatus)
				{
				case RES_SUCCESS:
					strcpy_s(pszGetData, sizeof(char) * SCAN_INFO_RECEIVE_BUFFER_SIZE, pszRecvBuffer);
					break;

				case BAD_REQUEST:
					sprintf_s(pszGetData, sizeof(int), "%d", BAD_REQUEST);
					break;

				case UNAUTHORIZED:
					sprintf_s(pszGetData, sizeof(int), "%d", UNAUTHORIZED);
					break;

				default:
					break;
				}
			}
		}

	}

	if (NULL != pszPacketBuffer)
	{
		delete[] pszPacketBuffer;
		pszPacketBuffer = NULL;
	}

	if (NULL != pszSendBuffer)
	{
		delete[] pszSendBuffer;
		pszSendBuffer = NULL;
	}

	if (NULL != pszRecvBuffer)
	{
		delete[] pszRecvBuffer;
		pszRecvBuffer = NULL;
	}
}

//void CNetScanOnvif::GetProfile(char* pszIP, int iPort, char* pszDigest, char* pszNonceResult, char* pszDateResult, char* pszGetData)
//{
//	BOOL		bIsConnect			= FALSE;
//	char*		pszRecvBuffer		= NULL;
//	char*		pszPacketBuffer		= NULL;
//	char*		pszSendBuffer		= NULL;
//	char*		pszSlice			= NULL;
//	int			iHeaderSize			= 0;
//	int			iBodySize			= 0;
//	int			iPacketSize			= 0;
//	int			iError				= 0;
//	int			iContentLen			= 0;
//	int			iHTTPStatus			= 0;
//	char		aszCopyBuffer[20]	= { 0 };
//	enum 
//	{
//		DATALEN = 8  // port's len + content-leng's len
//	};
//	
//	char aszDeviceInfoHeader[] = "POST /onvif/media_service HTTP/1.1\r\nContent-Type: application/soap+xml; charset=utf-8; action=\"http://www.onvif.org/ver10/media/wsdl/GetProfiles\"\r\nHost: %s\r\nContent-Length: %d\r\nAccept-Encoding: gzip, deflate\r\nConnection: Close\r\n\r\n";
//
//	char aszDeviceInformationXML[] = 
//	{
//		"<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\">\
//			<s:Header>\
//				<Security s:mustUnderstand=\"1\" xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\">\
//					<UsernameToken>\
//						<Username>ADMIN</Username>\
//						<Password Type=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest\">%s</Password>\
//						<Nonce EncodingType=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary\">%s</Nonce>\
//						<Created xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\">%s</Created>\
//					</UsernameToken>\
//				</Security>\
//			</s:Header>\
//			<s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">\
//				<GetProfiles xmlns=\"http://www.onvif.org/ver10/media/wsdl\"/>\
//			</s:Body>\
//		</s:Envelope>" // UserToken Header - GetVideoSource Body
//	};
//
//	bIsConnect = ConnectTCPSocket(pszIP, iPort);
//
//	iHeaderSize = strlen(aszDeviceInfoHeader) + strlen(pszIP) + DATALEN;
//	//iBodySize = strlen(aszDeviceInformationXML) + strlen(pszDigest) + strlen(pszNonceResult) + strlen(pszDateResult) + strlen(m_aszUserName);
//	iBodySize = strlen(aszDeviceInformationXML) + strlen(pszDigest) + strlen(pszNonceResult) + strlen(pszDateResult);
//	iPacketSize = iHeaderSize + iBodySize;
//	iContentLen = iBodySize;
//
//	pszPacketBuffer = new char[iPacketSize + 1];
//	memset(&pszPacketBuffer[0], 0, iPacketSize + 1);
//
//	sprintf_s(pszPacketBuffer, sizeof(char)* iHeaderSize, "%s", aszDeviceInfoHeader);
//	strcat_s(pszPacketBuffer, sizeof(char)* iPacketSize, aszDeviceInformationXML);
//
//	pszSendBuffer = new char[iPacketSize + 1];
//	memset(&pszSendBuffer[0], 0, iPacketSize + 1);
//	//sprintf_s(pszSendBuffer, sizeof(char)* iPacketSize, pszPacketBuffer, pszIP, iPort, iContentLen, m_aszUserName, pszDigest, pszNonceResult, pszDateResult);
//	sprintf_s(pszSendBuffer, sizeof(char)* iPacketSize, pszPacketBuffer, pszIP, iContentLen, pszDigest, pszNonceResult, pszDateResult);
//
//	::OutputDebugStringA(pszSendBuffer);
//	::OutputDebugStringA("\n");
//
//	pszRecvBuffer = new char[SCAN_INFO_RECEIVE_BUFFER_SIZE];
//	memset(&pszRecvBuffer[0], 0, SCAN_INFO_RECEIVE_BUFFER_SIZE);
//
//	if (CONNECT_SUCCESS == bIsConnect)
//	{
//		if (SOCKET_ERROR == send(m_TcpSocket, pszSendBuffer, iPacketSize, 0))
//		{
//			iError = WSAGetLastError();
//			TRACE(_T("TCP-HTTP send Error = %d\n"), iError);
//			closesocket(m_TcpSocket);
//		}
//		int i = 0;
//		while (i < 10)
//		{
//			if (SOCKET_ERROR == recv(m_TcpSocket, pszRecvBuffer, SCAN_INFO_RECEIVE_BUFFER_SIZE, 0))
//			{
//				iError = WSAGetLastError();
//				TRACE(_T("TCP-HTTP recv Error = %d\n"), iError);
//				closesocket(m_TcpSocket);
//			}
//			i++;
//		}
//		
//
//		::OutputDebugStringA("ONVIF RECEIVE DEVICE DATA -----------------------\n");
//		::OutputDebugStringA(pszIP);
//		::OutputDebugStringA("\n");
//		::OutputDebugStringA(pszRecvBuffer);
//		::OutputDebugStringA("\n");
//
//		if (0 < strlen(pszRecvBuffer))
//		{
//			memcpy(&aszCopyBuffer[0], pszRecvBuffer, 20);
//			pszSlice = strstr(aszCopyBuffer, " ");
//			if (NULL != pszSlice)
//			{
//				pszSlice = strtok(pszSlice, " ");
//				iHTTPStatus = atoi(pszSlice);
//
//				switch (iHTTPStatus)
//				{
//				case RES_SUCCESS:
//					strcpy_s(pszGetData, sizeof(char)* SCAN_INFO_RECEIVE_BUFFER_SIZE, pszRecvBuffer);
//					break;
//
//				case BAD_REQUEST:
//					sprintf_s(pszGetData, sizeof(int), "%d", BAD_REQUEST);
//					break;
//
//				case UNAUTHORIZED:
//					sprintf_s(pszGetData, sizeof(int), "%d", UNAUTHORIZED);
//					break;
//
//				default:
//					break;
//				}
//			}
//		}
//	}
//
//
//	if (NULL != pszPacketBuffer)
//	{
//		delete[] pszPacketBuffer;
//		pszPacketBuffer = NULL;
//	}
//
//	if (NULL != pszSendBuffer)
//	{
//		delete[] pszSendBuffer;
//		pszSendBuffer = NULL;
//	}
//
//	if (NULL != pszRecvBuffer)
//	{
//		delete[] pszRecvBuffer;
//		pszRecvBuffer = NULL;
//	}
//}


void CNetScanOnvif::DigestConvert(char* pszStr, char* puszResult)
{
	char aszDigest[MAX_PATH] = { 0 };
	char aszOutput[SHA_DIGEST_LENGTH * 2 + 1] = { 0 };

	SHA1Encoding(pszStr, aszOutput);
	Base64Encoding(aszOutput, strlen(aszOutput), &aszDigest[0]);
	memcpy( puszResult, aszDigest, sizeof(char) * strlen(aszDigest));

}

void CNetScanOnvif::SHA1Encoding(char* pszStr, char* pszResult)
{
	SHA_CTX			stSHA1;
	int				iLen = 0;
	unsigned char	auszHash[SHA_DIGEST_LENGTH]			= { 0 };


	// SHA1 Library
	SHA1_Init(&stSHA1);
	SHA1_Update(&stSHA1, pszStr, strlen(pszStr));
	SHA1_Final(auszHash, &stSHA1);

	memcpy(pszResult, auszHash, sizeof(char)* (SHA_DIGEST_LENGTH));
}

// 128bit 
void CNetScanOnvif::Base64Encoding(char* pszStr, int iSize, char* pszResult)
{
	static const char MimeBase64[] = {
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
		'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
		'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
		'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
		'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
		'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
		'w', 'x', 'y', 'z', '0', '1', '2', '3',
		'4', '5', '6', '7', '8', '9', '+', '/'
	};

	unsigned char input[3]	= { 0 };
	unsigned char output[4] = { 0 };
	int index				= 0;
	int i					= 0;
	int j					= 0;
	int iDataSize			= 0;
	int	iLen				= 0;
	char* p					= NULL;
	char* plen				= NULL;
	wchar_t strUnicode[MAX_PATH] = { 0 };
	char strUtf8[MAX_PATH] = { 0 };
	char* pszBuffer			= NULL;

	plen = pszStr + iSize - 1;
	iDataSize = (4 * (iSize / 3)) + (iSize % 3 ? 4 : 0) + 1;

	pszBuffer = new char[iDataSize];
	memset(&pszBuffer[0], 0, sizeof(char) * iDataSize);

	j = 0;
	for (i = 0, p = pszStr; p <= plen; i++, p++) {
		index = i % 3;
		input[index] = *p;
		if (index == 2 || p == plen) {
			output[0] = ((input[0] & 0xFC) >> 2);
			output[1] = ((input[0] & 0x3) << 4) | ((input[1] & 0xF0) >> 4);
			output[2] = ((input[1] & 0xF) << 2) | ((input[2] & 0xC0) >> 6);
			output[3] = (input[2] & 0x3F);
			pszBuffer[j++] = MimeBase64[output[0]];
			pszBuffer[j++] = MimeBase64[output[1]];
			pszBuffer[j++] = index == 0 ? '=' : MimeBase64[output[2]];
			pszBuffer[j++] = index <  2 ? '=' : MimeBase64[output[3]];
			input[0] = input[1] = input[2] = 0;
		}
	}

	pszBuffer[j] = '\0';

	// 결과 copy
	memcpy(pszResult, pszBuffer, iDataSize);

	if (NULL != pszBuffer)
	{
		delete[] pszBuffer;
		pszBuffer = NULL;
	}
}

int CNetScanOnvif::Base64Decoding(char* pszStr, unsigned char* puszResult, int iSize)
{
	/*------ Base64 Decoding Table ------*/
	static int DecodeMimeBase64[256] = {
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  /* 00-0F */
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  /* 10-1F */
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,  /* 20-2F */
		52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,  /* 30-3F */
		-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,  /* 40-4F */
		15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,  /* 50-5F */
		-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,  /* 60-6F */
		41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,  /* 70-7F */
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  /* 80-8F */
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  /* 90-9F */
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  /* A0-AF */
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  /* B0-BF */
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  /* C0-CF */
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  /* D0-DF */
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  /* E0-EF */
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1   /* F0-FF */
	};

	const char* cp;
	int space_idx = 0, phase;
	int d, prev_d = 0;
	unsigned char c;
	space_idx = 0;
	phase = 0;
	for (cp = pszStr; *cp != '\0'; ++cp) {
		d = DecodeMimeBase64[(int)*cp];
		if (d != -1) {
			switch (phase) {
			case 0:
				++phase;
				break;
			case 1:
				c = ((prev_d << 2) | ((d & 0x30) >> 4));
				if (space_idx < iSize)
					puszResult[space_idx++] = c;
				++phase;
				break;
			case 2:
				c = (((prev_d & 0xf) << 4) | ((d & 0x3c) >> 2));
				if (space_idx < iSize)
					puszResult[space_idx++] = c;
				++phase;
				break;
			case 3:
				c = (((prev_d & 0x03) << 6) | d);
				if (space_idx < iSize)
					puszResult[space_idx++] = c;
				phase = 0;
				break;
			}
			prev_d = d;
		}
	}
	return space_idx;
}