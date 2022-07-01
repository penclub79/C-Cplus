//
// Copyright (C) Since 2022 DAYOU. All rights reserved.
// 
//

#pragma once

#define MAGIC_CODE_UPGRADEFILE1	0x7668

#define MAGIC_CODE_UPGRADEFILE2	0x7667
#define MAGIC_CODE_SOFTWARE		0x7668
#define MAGIC_CODE_MCU			0x7669

//#define MAGIC_CODE_PACKET		0x7668

#define PROTOCOL_TYPE_UPGRADE	0x0c
#define UpgradeProtocolVersion	1

#pragma pack(push, VHT_PACK, 1)

typedef struct tagMCU_PACKAGE_FILE_INFO
{
	int magic_code;
	short mcu_model;
	short mcu_version;
	long mcu_checksumsize;
	long mcu_size;
	long mcu_opset;

}MCU_PACKAGE_FILE_INFO, *LPMCU_PACKAGE_FILE_INFO;

typedef struct tagSW_PACKAGE_FILE_INFO
{
	int magic_code;
	int modelType;
	int dsptype;
	int oemtype;
	char szSWVersion[25];
}SW_PACKAGE_FILE_INFO, *LPSW_PACKAGE_FILE_INFO;


typedef struct tagUPGRADE_HEADER
{
	short magic_code;
	char protocol_type;
	int protocol_mode;
	int body_size;
}UPGRADE_HEADER, *LPUPGRADE_HEADER;

typedef struct tagUPGRADE_PROTOCOL_VERSION_RSP
{
	int messages;
	int protocol_version;
}UPGRADE_PROTOCOL_VERSION_RSP, *LPUPGRADE_PROTOCOL_VERSION_RSP;

typedef struct tagUPGRADE_AUTH
{
	char id[32];
	char password[32];
}UPGRADE_AUTH, *LPUPGRADE_AUTH;

typedef struct tagUPGRADE_FILE_STREAM
{
	int index;
	int length;
	char data[1024];
}UPGRADE_FILE_STREAM, *LPUPGRADE_FILE_STREAM;

typedef struct tagUPGRADE_START_INFO
{
	int		model_type;
	int		dsp_type;
	int		oem_type;
	long	length;
	char	name[128];
}UPGRADE_START_INFO, *LPUPGRADE_START_INFO;

typedef struct tagUPGRADE_RSP
{
	int messages;
}UPGRADE_RSP, *LPUPGRADE_RSP;

typedef struct tagUPGRADE_START_INFO_RSP
{
	int messages;
}UPGRADE_START_INFO_RSP, *LPUPGRADE_START_INFO_RSP;

typedef struct tagUPGRADE_AUTH_RSP
{
	int messages;
	int protocol_version;
}UPGRADE_AUTH_RSP, *LPUPGRADE_AUTH_RSP;

typedef struct tagUPGRADE_FILE_STREAM_RSP
{
	int messages;
	int index;
}UPGRADE_FILE_STREAM_RSP, *LPUPGRADE_FILE_STREAM_RSP;

typedef struct tagUPGRADE_NAND_WRITE_RSP
{
	int messages;
	int written_percent;	//OS�� Application�� Nand�� ������ ���� ���¸� OS�� Application ���� 0~100���� �˸�.
}UPGRADE_NAND_WRITE_RSP, *LPUPGRADE_NAND_WRITE_RSP;

typedef struct tagUPGRADE_SW_VERSION_RSP
{
	int messages;
	char sw_version[25];
}UPGRADE_SW_VERSION_RSP, *LPUPGRADE_SW_VERSION_RSP;


typedef struct tagUPGRADE_MCU_INFO_RSP
{
	int messages;
	int mcu_model;
	int mcu_version;
}UPGRADE_MCU_INFO_RSP, *LPUPGRADE_MCU_INFO_RSP;


typedef struct tagUPGRADE_MCU_VERSION_RSP
{
	int messages;
	int mcu_version;
}UPGRADE_MCU_VERSION_RSP, *LPUPGRADE_MCU_VERSION_RSP;

typedef struct tagMCU_INFO
{
	int mcu_model;
	int mcu_version;
}MCU_INFO, *LPMCU_INFO;

typedef struct tagUPGRADE_MCU_FILE_INFO
{
	int		model_type;
	long	length;
	char	name[128];
}UPGRADE_MCU_FILE_INFO, *LPUPGRADE_MCU_FILE_INFO;


typedef struct tagUPGRADE_MCU_WRITE_RSP
{
	int messages;
	int written_percent;	//OS�� Application�� Nand�� ������ ���� ���¸� OS�� Application ���� 0~100���� �˸�.
}UPGRADE_MCU_WRITE_RSP, *LPUPGRADE_MCU_WRITE_RSP;



enum _PROTOCOL_UPGRADE_REQ
{
	_UPGRADE_REQ_AUTH = 0x01,
	_UPGRADE_REQ_START,
	_UPGRADE_REQ_STOP,
	_UPGRADE_REQ_FILE_STREAM,
	_UPGRADE_REQ_STATUS,
	_UPGRADE_REQ_SW_VERSION,
	_UPGRADE_REQ_REBOOT,
	_UPGRADE_REQ_PROTOCOL_VERSION,
	_UPGRADE_REQ_MCU_INFO,
	_UPGRADE_REQ_STOP_MCU_UPGRADE,
	_UPGRADE_REQ_START_MCU_UPGRADE,
	_UPGRADE_REQ_MCU_VERSION,
};

enum _PROTOCOL_UPGRADE_RSP
{
	_UPGRADE_RSP_AUTH = 0x01,
	_UPGRADE_RSP_FILE_INFO,
	_UPGRADE_RSP_STOP,
	_UPGRADE_RSP_FILE_STREAM,
	_UPGRADE_RSP_NAND_WRITE,
	_UPGRADE_RSP_SW_VERSION,
	_UPGRADE_RSP_STATUS,
	_UPGRADE_RSP_REBOOT,
	_UPGRADE_RSP_PROTOCOL_VERSION,
	_UPGRADE_RSP_MCU_INFO,
	_UPGRADE_RSP_MCU_FILE_INFO,
	_UPGRADE_RSP_MCU_WRITE,
	_UPGRADE_RSP_MCU_VERION,
	_UPGRADE_RSP_MCU_FILE_STREAM


};

enum _PROTOCOL_UPGRADE_RSP_MSG
{
	_UPGRADE_RSP_MSG_AUTH_SUCCESS = 0x00,	// 0
	_UPGRADE_RSP_MSG_FILE_INFO_SUCCESS,		// 1
	_UPGRADE_RSP_MSG_STOP_SUCCESS,		// 2
	_UPGRADE_RSP_MSG_SW_VERSION_SUCCESS,		// 3
	_UPGRADE_RSP_MSG_TIMEOUT,		// 4
	_UPGRADE_RSP_MSG_READY_TO_UPGRADE,		// 5
	_UPGRADE_RSP_MSG_UPGRADING_NOW,		// 6
	_UPGRADE_RSP_MSG_FWUP_NOW,		// 7
	_UPGRADE_RSP_MSG_NO_AUTHENTIFICATION,		// 8
	_UPGRADE_RSP_MSG_NO_AUTHORITY,		// 9
	_UPGRADE_RSP_MSG_NO_MATCH_FILE_INDEX,		// 10
	_UPGRADE_RSP_MSG_FILE_RECEIVED_OK,		// 11
	_UPGRADE_RSP_MSG_NO_MATCH_MODEL,		// 12
	_UPGRADE_RSP_MSG_INVALID_FIRMWARE,		// 13
	_UPGRADE_RSP_MSG_MEMORY_IS_NOT_ENOUGH,		// 14
	_UPGRADE_RSP_MSG_FILE_NOT_EXIST,		// 15
	_UPGRADE_RSP_MSG_UPGRADING_OS,		// 16
	_UPGRADE_RSP_MSG_UPGRADED_OS,		// 17
	_UPGRADE_RSP_MSG_UPGRADING_APPLICATION,		// 18
	_UPGRADE_RSP_MSG_UPGRADED_APPLICATION,		// 19
	_UPGRADE_RSP_MSG_FINISHED_UPGRADE,		// 20
	_UPGRADE_RSP_MSG_SYSTEM_RESTARTING,		// 21
	_UPGRADE_RSP_MSG_BADCOMMAND,		// 22
	_UPGRADE_RSP_MSG_PROTOCOL_VERSION_SUCCESS,		// 23
	_UPGRADE_RSP_MSG_UPGRADING_BOOTLOADER,		// 22
	_UPGRADE_RSP_MSG_UPGRADED_BOOTLOADER,		// 22
	_UPGRADE_RSP_MSG_PROTOCAL_MCU_INFO_SUCCESS,		// 22
	_UPGRADE_RSP_MSG_PROTOCAL_MCU_INFO_FAIL,		// 22
	_UPGRADE_RSP_MSG_FILE_NOT_EXIST_MCU,		// 22
	_UPGRADE_RSP_MSG_READY_TO_UPGRADE_MCU,		// 22
	_UPGRADE_RSP_MSG_NO_MATCH_MODEL_MCU,		// 22
	_UPGRADE_RSP_MSG_INVALID_FIRMWARE_MCU,		// 22
	_UPGRADE_RSP_MSG_UPGRADING_MCU,		// 22
	_UPGRADE_RSP_MSG_UPGRADED_MCU,		// 22
	_UPGRADE_RSP_MSG_FINISHED_UPGRADE_MCU,		// 22
	_UPGRADE_RSP_MSG_MCU_VERSION_SUCCESS,		// 22
	_UPGRADE_RSP_MSG_MCU_VERSION_FAIL,		// 22
};

#pragma pack(pop, VHT_PACK)

// constants move from UpgradeThreadTcp.h
const UINT UPGRADE_PROGRESS = 0x00;
const UINT UPGRADE_SETSTATUS = 0x01;
const UINT UPGRADE_ERROR = 0x02;

typedef struct tagSOCKET_MODE_INFO
{
	void*	pThis;
	int		iSocketMode;
}SOCKET_MODE_INFO, *LPSOCKET_MODE_INFO;

enum _UPGRADESOCKET_MODE
{
	_UPGRADESOCKET_MODE_UPGRADE = 0x01,
	_UPGRADESOCKET_MODE_CHECKCOMPLETE,
	_UPGRADESOCKET_MODE_SWUPGRADE_WITHOUT_MCU,
};

enum _UPGRADE_STATUS
{
	_UPGRADE_STATUS_CONNECTING = 0x00,
	_UPGRADE_STATUS_FILE_TRANSFERING = 0x01,
	_UPGRADE_STATUS_READY_UPGRADE = 0x02,
	_UPGRADE_STATUS_UPGRADE_OS = 0x03,
	_UPGRADE_STATUS_UPGRADE_APPLICATION = 0x04,
	_UPGRADE_STATUS_UPGRADE_APPLICATION_WEB = 0x05,
	_UPGRADE_STATUS_RESTARTING_SW = 0x06,
	_UPGRADE_STATUS_FILE_RETRANSFERING = 0x07,
	_UPGRADE_STATUS_COMPLETE = 0x08,
	_UPGRADE_STATUS_UPGRADE_PROGRESS = 0x09,
	_UPGRADE_STATUS_READY_TO_SW_FILE_TRANSFERRING = 0x0a,
	_UPGRADE_STATUS_PREPARE_UPGRADING = 0x0b, // 10
	_UPGRADE_STATUS_WAITFOR_REBOOT = 0x0c, // 11
	_UPGRADE_STATUS_ATTEMPT_REBOOT = 0x0d, // 12
	_UPGRADE_STATUS_ATTEMPT_RETRY = 0x0e, // 13
	_UPGRADE_STATUS_MCU_FILE_TRANSFERING = 0x0f, // 14
	_UPGRADE_STATUS_UPGRADE_MCU = 0x10, // 15
	_UPGRADE_STATUS_RESTARTING_ERROR = 0x11, // 16
	_UPGRADE_STATUS_RESTARTING_MCU = 0x12, // 17
	_UPGRADE_STATUS_READY_TO_MCU_FILE_TRANSFERRING = 0x13, // 18
};

enum _UPGRADE_ERROR
{
	_UPGRADE_ERROR_OPEN_ERROR = -1,
	_UPGRADE_ERROR_CONNECT_FAILED = -2,
	_UPGRADE_ERROR_DISCONNECTED = -3,
	_UPGRADE_ERROR_AUTH_FAILED = -4,
	_UPGRADE_ERROR_NOT_SUPPORT_UPGRAE_FILE = -5,
	_UPGRADE_ERROR_IN_ORDER_TO_VERSION_CHECKING_CAN_NOT_CONNECT = -6,
	_UPGRADE_ERROR_INVALID_FIRMWARE = -7,
	_UPGRADE_ERROR_NO_MATCH_MODEL = -8,
	_UPGRADE_ERROR_NO_MATCH_VERSION = -9,
	_UPGRADE_ERROR_UPGRADE_IS_WORKING_IN_OTHER_PLACE = -10,
	_UPGRADE_ERROR_RECEIVE_TIME_OUT = -11,
	_UPGRADE_ERROR_THIS_VERSION_DOES_NOT_SUPPORT = -12,
	_UPGRADE_ERROR_UPGRADE_FILE_IS_NOT_AVAILABLE = -13,
	_UPGRADE_ERROR_UPGRADE_CANCELED = -14,
	_UPGRADE_ERROR_MEMORY_NOT_ENOUGH = -15,
	_UPGRADE_ERROR_BAD_COMMAND = -16,
	_UPGRADE_ERROR_PROTOCOL_MCU_INFO_FAIL = -17,
	_UPGRADE_ERROR_NO_MATCH_MCU_MODEL = -18
};