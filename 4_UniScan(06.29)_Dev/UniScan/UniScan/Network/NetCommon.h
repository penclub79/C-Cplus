#pragma once

#define MK_UDP_REQ_PORT		9010
#define MK_UDP_RSP_PORT		9011
#define VH_UDP_SCAN_PORT	64988
#define ONVIF_



// Scan�� ����
enum SCANTYPE {
	VISION = 0x00,
	MARKIN,
	ONVIF
};

const UINT32 SCAN_INFO_m_pReceive_buffer_SIZE = 64 * 1024; // 64 Kbytes temp buffer for receive temporary data
const UINT32 SCAN_ERR_NONE = 0x00000000;
const UINT32 SCAN_ERR_SOCKET_OPT = 0x00000001;
const UINT32 SCAN_ERR_BIND = 0x00000002;
const UINT32 SCAN_ERR_MEMORY = 0x00000003;
const UINT32 SCAN_ERR_RECV = 0x00000004;

const UINT32 MARKIN_PACKET_REQ_DEVICEINFO = 0x00001001;
const UINT32 MARKIN_PACKET_RSP_DEVICEINFO = 0xA0001001;