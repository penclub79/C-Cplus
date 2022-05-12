
// UpdateManagerDlg.h : ��� ����
//

#pragma once

#include <GrTypeBase.h>
#include "FirmUpdate.h"
#include "afxcmn.h"
#include "afxwin.h"

#define	E_MkUpdt_IniFcc			0x55696E69

// ---------------------------------------------------- struct 
typedef struct _stUpdateTreeNode
{
	unsigned int			uiType;
	HTREEITEM				stNode;
	TCHAR					szaNode[16];
} *pUpdateTreeNode;

typedef struct _stUpdateInfoEntity
{
	unsigned int			uiType;
	TCHAR					szaFile[256];
} *pUpdateInfoEntity;

typedef struct _stUpdateInfoModel
{
	unsigned int			uiType;
	_stUpdateInfoEntity		staEntity[16];
} *pUpdateInfoModel;

typedef struct _stUpdateInfo
{
	unsigned int			uiFcc;
	unsigned int			uiUpgdVersion;
	_stUpdateInfoModel		staModelInfo[16];
	WCHAR					szaUpgdFileName[1024];
} *pUpdateInfo;

// ---------------------------------------------------- struct 



// CUpdateManagerDlg ��ȭ ����
class CUpdateManagerDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CUpdateManagerDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.
	~CUpdateManagerDlg();
// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_UPDATEMANAGER_DIALOG };

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

	void Init();
	//BOOL CheckFcc(pUpdateInfo _p);	// MkUpdate.init ������ ���� �� FCCüũ
	void InitCtrl();				// MkUpdate.init ������ ���� ��
	//void AddModel(unsigned int _uiModelType);

	void TreeAddModel(int _iModelType); // Ʈ���� �� �߰�
	void TreeAddVerFile(int _iModelIdx, int _iVerFileType, TCHAR* _pszFilePath, int _iFileLen); // Ʈ���� �𵨿��� �������� �߰�
	void ProcErrCode(int _iResult); // ���� ó��
	
private:
	WCHAR				m_szaNowPath[2048];

	CFirmUpdate*		m_pObjFwUp;
	_stUpdateTreeNode	m_staTreeNode[E_FirmUpInfoTypeMaxIdx];  // 24
	WCHAR				m_szaMkFileName[1024];
	_stUpdateInfo		m_stUpdateInfo;

	CTreeCtrl	m_CTreeCtrl;
	CEdit		m_CEditPath;
	CEdit		m_CEditVer1;
	CEdit		m_CEditVer2;
	CEdit		m_CEditVer3;
	CEdit		m_CEditVer4;
	CButton		m_CBtnPath;
	CButton		m_CBtnMake;

public:
	afx_msg void OnClickedButtonModelCreate(); // �� �߰�
	afx_msg void OnClickedButtonEntitySelete(); // ���� ���� ����
};