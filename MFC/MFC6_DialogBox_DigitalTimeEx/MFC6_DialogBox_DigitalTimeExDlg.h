
// MFC6_DialogBox_DigitalTimeExDlg.h : ��� ����
//

#pragma once
#include "ClockHelpDlg.h"


// CMFC6_DialogBox_DigitalTimeExDlg ��ȭ ����
class CMFC6_DialogBox_DigitalTimeExDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CMFC6_DialogBox_DigitalTimeExDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_MFC6_DIALOGBOX_DIGITALTIMEEX_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEdit1();
	CString m_strYear;
	CString m_strSecond;
	CString m_strMonth;
	CString m_strMinute;
	CString m_strHour;
	CString m_strDay;
	CString m_strAMPM;
	bool m_bRadioClockType;
	afx_msg void OnRadio12();
	afx_msg void OnRadio24();
	bool m_bCheckYear;
	bool m_bCheckHour;
	afx_msg void OnClickedCheckYear();
	afx_msg void OnClickedCheckHour();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	bool m_bViewHelp;
	CClockHelpDlg m_dlgClockHelp;
	afx_msg void OnClickedButtonHelp();
};