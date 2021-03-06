#pragma once
#include "afxcmn.h"


// CPenSizeDlg 대화 상자입니다.

class CPenSizeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPenSizeDlg)

public:
	CPenSizeDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CPenSizeDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_PEN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	UINT m_nPenSize;
	CSpinButtonCtrl m_spinPen;
	virtual BOOL OnInitDialog();
};
