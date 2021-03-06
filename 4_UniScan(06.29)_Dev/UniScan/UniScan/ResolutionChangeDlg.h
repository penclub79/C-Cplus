#pragma once

#include <afxinet.h>
#include "afxwin.h"
#include "afxcmn.h"

#include "resource.h"
#include "progresslistctrl.h"
#include "./Network/NetScanVision.h"
#include "ResolutionChangeThread.h"

// CResolutionChangeDlg 대화 상자입니다.

class CResolutionChangeDlg : public CDialog
{
	DECLARE_DYNAMIC(CResolutionChangeDlg)

public:
	CResolutionChangeDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CResolutionChangeDlg();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_CAM_RESOLCHANGE };

	enum {
		SUBITEM_MODEL = 0,
		SUBITEM_MACADDRESS = 1,
		SUBITEM_IPADDRESS = 2,
		SUBITEM_USER_ID = 3,
		SUBITEM_USER_PW = 4,
		SUBITEM_STATUS = 5
	};

	enum {
		STREAMTYPE_SINGLE = 0,
		STREAMTYPE_DUAL = 1,
		STREAMTYPE_TRIPLE = 2
	};

	enum {
		RATECONTROL_CVBR = 0,
		RATECONTROL_CBR = 1
	};

	enum {
		CODEC_SINGLE_H264,
		CODEC_DUAL_H264_H264,
		CODEC_DUAL_H264_MJPEG,
		CODEC_TRIPLE_H264_MJPEG_H264
	};


	enum {
		RESOL_S_H_D1 = 0,
		RESOL_S_H_HD,
		RESOL_S_H_SXGA,
		RESOL_S_H_FHD,
		RESOL_S_H_QXGA,
		RESOL_S_H_960_576,
		RESOL_S_H_720_576,
		RESOL_S_H_960_288,
		RESOL_S_H_720_288,
		RESOL_S_H_480_288,
		RESOL_S_H_352_288,
		RESOL_D_HH______D1______D1 = 50,
		RESOL_D_HH______HD_____CIF,
		RESOL_D_HH______HD____QVGA,
		RESOL_D_HH____SXGA_____VGA,
		RESOL_D_HH____SXGA______D1,
		RESOL_D_HH_____FHD_____CIF,
		RESOL_D_HH_____FHD____QVGA,
		RESOL_D_HH_____FHD_____VGA,
		RESOL_D_HH____QXGA____QVGA,
		RESOL_D_HH____QXGA_____VGA,
		RESOL_D_HH_960X480_960X480, // NTSC
		RESOL_D_HH_960X480_960X240,
		RESOL_D_HH_960X480_480X240,
		RESOL_D_HH_720X480_720X480,
		RESOL_D_HH_720X480_720X240,
		RESOL_D_HH_720X480_352X240,
		RESOL_D_HH_960X240_960X240,
		RESOL_D_HH_960X240_480X240,
		RESOL_D_HH_720X240_720X240,
		RESOL_D_HH_720X240_352X240,
		RESOL_D_HH_480X240_480X240,
		RESOL_D_HH_352X240_352X240,
		RESOL_D_HH_960X576_960X576, // PAL
		RESOL_D_HH_960X576_960X288,
		RESOL_D_HH_960X576_480X288,
		RESOL_D_HH_720X576_720X576,
		RESOL_D_HH_720X576_720X288,
		RESOL_D_HH_720X576_352X288,
		RESOL_D_HH_960X288_960X288,
		RESOL_D_HH_960X288_480X288,
		RESOL_D_HH_720X288_720X288,
		RESOL_D_HH_720X288_352X288,
		RESOL_D_HH_480X288_480X288,
		RESOL_D_HH_352X288_352X288,
		RESOL_D_HM______D1______D1 = 100,
		RESOL_D_HM______HD_____CIF,
		RESOL_D_HM______HD____QVGA,
		RESOL_D_HM______HD_____VGA,
		RESOL_D_HM____SXGA_____VGA,
		RESOL_D_HM____SXGA______D1,
		RESOL_D_HM_____FHD_____CIF,
		RESOL_D_HM_____FHD____QVGA,
		RESOL_D_HM_____FHD_____VGA,
		RESOL_D_HM____QXGA____QVGA,
		RESOL_D_HM____QXGA_____VGA,
		RESOL_D_HM_960X480_960X480, // NTSC
		RESOL_D_HM_960X480_960X240,
		RESOL_D_HM_960X480_480X240,
		RESOL_D_HM_720X480_720X480,
		RESOL_D_HM_720X480_720X240,
		RESOL_D_HM_720X480_352X240,
		RESOL_D_HM_960X240_960X240,
		RESOL_D_HM_960X240_480X240,
		RESOL_D_HM_720X240_720X240,
		RESOL_D_HM_720X240_352X240,
		RESOL_D_HM_480X240_480X240,
		RESOL_D_HM_352X240_352X240,
		RESOL_D_HM_960X576_960X576, // PAL
		RESOL_D_HM_960X576_960X288,
		RESOL_D_HM_960X576_480X288,
		RESOL_D_HM_720X576_720X576,
		RESOL_D_HM_720X576_720X288,
		RESOL_D_HM_720X576_352X288,
		RESOL_D_HM_960X288_960X288,
		RESOL_D_HM_960X288_480X288,
		RESOL_D_HM_720X288_720X288,
		RESOL_D_HM_720X288_352X288,
		RESOL_D_HM_480X288_480X288,
		RESOL_D_HM_352X288_352X288,
		RESOL_T_HMH___HD__VGA__CIF = 200
	};

	BOOL	SetScaninfo(int iCount, SCAN_INFO* pScanInfo, int iAdapterID);
	int		GetSelectScanInfoCount();


private:
	static DWORD WINAPI ProcLensInit(LPVOID _lpParam);

	void	InitModelCombo();
	void	CheckArp(int iAdapterID);

	void	EnableControlMain(BOOL bEnable);
	void	EnableControlFunction(BOOL bEnable);
	void	EnableControlUserInfo(BOOL bEnable);
	void	EnableControlH264Stream1(BOOL bEnable);
	void	EnableControlH264Stream2(BOOL bEnable);
	void	EnableControlMJPEGStream(BOOL bEnable);

	void	InitResolutionComboForStreamSingle();
	void	InitResolutionComboForStreamDualHH();
	void	InitResolutionComboForStreamDualHM();
	void	InitResolutionComboForStreamTriple();

	int		CallHttpRequest(CString ipaddr, int port, CString query, CString &response);

	int		GetResolutionTextByID(int iResolution, WCHAR* pszResolution1, WCHAR* pszResolution2, WCHAR* pszResolution3);
	void	GetSetupStreamInfo(SETUP_STREAMINFO* pSetupStreamInfo);

	int					m_iAdapterID;
	int					m_nSelectScanInfoCnt;
	int					m_nScanInfoCnt;
	SCAN_INFO*			m_pScanInfo;

	CInternetSession*	m_pSession;
	CHttpConnection*	m_pHttpConnect;
	CHttpFile*			m_pHttpFile;

	CString				m_strSelectedModel;

	int					m_iSelectStreamType;
	int					m_iSelectCodec;
	int					m_iSelectResolution;
	int					m_iBitrateStream1;
	int					m_iBitrateStream2;

	HANDLE				m_hThreadLensInit;
	DWORD				m_dwThreadLensInitID;

	SETUP_STREAMINFO	m_SetupStreamInfo;
	CMap < int, int, CResolutionChangeThread*, CResolutionChangeThread* > m_aSettingThread;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboModeltype();
	afx_msg void OnCbnSelchangeComboStreamtype();
	afx_msg void OnCbnSelchangeComboResolution();
	afx_msg void OnCbnSelchangeComboRatecontrol();
	afx_msg void OnCbnSelchangeComboCodectype();
	afx_msg void OnNMCustomdrawSliderBitrate1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderBitrate2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonLensInit();
	afx_msg void OnEnChangeUserid();
	afx_msg void OnEnChangePassword();
	afx_msg LRESULT OnMessageStatus(WPARAM wParam, LPARAM lParam);
	afx_msg void OnEnChangeEditBitrate1();
	afx_msg void OnEnChangeEditBitrate2();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnDestroy();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	virtual BOOL PreTranslateMessage(MSG* pMsg);


	CComboBox			m_ctlComboStreamType;
	CComboBox			m_ctlComboCodec;
	CComboBox			m_ctlComboResolution;
	CComboBox			m_ctlComboRateControl;
	CComboBox			m_ctlComboMjpegQuality;
	CComboBox			m_ctlComboMjpegFPS;
	CComboBox			m_ctlComboFPS1;
	CComboBox			m_ctlComboFPS2;
	CComboBox			m_ctlComboGOP1;
	CComboBox			m_ctlComboGOP2;
	CComboBox			m_ctlComboModelType;
	CSliderCtrl			m_ctlSliderBitrate1;
	CSliderCtrl			m_ctlSliderBitrate2;
	CEdit				m_ctlEditBitrate1;
	CEdit				m_ctlEditBitrate2;
	CProgressListCtrl	m_ctlListDevice;
	CButton				m_ctlButtonLensInit;
	CButton				m_ctlButtonApply;
	CButton				m_ctlButtonCancel;

};
