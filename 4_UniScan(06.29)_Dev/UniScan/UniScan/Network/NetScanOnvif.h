#pragma once

#include "NetScanBase.h"
#include "../../UniScan/MD5.h"

typedef struct _ONVIFINFO
{
	char szInfo[SCAN_INFO_RECEIVE_BUFFER_SIZE];
}ONVIFINFO;


class CNetScanOnvif : public NetScanBase
{
public:
	CNetScanOnvif();
	~CNetScanOnvif(void);

	virtual BOOL StartScan();
	virtual BOOL SendScanRequest();

protected:
	static DWORD thrOnvifScanThread(LPVOID pParam);
	void thrOnvifReceiver();
	BOOL SendAuthentication(char* pszIP);
	BOOL SendSSDP();
	BOOL CreateSocket();
	BOOL GenerateMsgID(char* szMessageID, int nBufferLen);
	void MD5Function(char* pszStr, unsigned char* pszResult);
	BOOL m_bConnected;

private:

};