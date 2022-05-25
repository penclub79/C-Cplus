
// UpdateManagerDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "afxdialogex.h"
#include "UpdateManager.h"
#include "UpdateManagerDlg.h"
#include "FirmUpdate.h"
#include "DlgModelAdd.h"
#include "DlgVerFileAdd.h"

#include <GrDumyTool.h>
#include <GrStrTool.h>
#include <GrFileCtrl.h>
#include <GrFileTool.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
	
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CUpdateManagerDlg 대화 상자


// 생성자
CUpdateManagerDlg::CUpdateManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CUpdateManagerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pObjFwUp = NULL;
	memset(m_aszNowPath, 0, 2048);
	memset(m_aszMkFileName, 0, 1024);
	memset(m_aszMkModelName, 0, 2048);
	memset(m_astTreeNode, 0, E_FirmUpInfoTypeMaxIdx);
	m_stUpdateInfo = { 0 };
	
	for (int i = 0; i < E_FirmUpInfoTypeMaxIdx; i++)
	{
		m_astTreeNode[i] = { 0 };
	}
	

}

// 소멸자
CUpdateManagerDlg::~CUpdateManagerDlg()
{
	if (NULL != m_pObjFwUp)
	{
		delete m_pObjFwUp;
		m_pObjFwUp = NULL;
	}
}


void CUpdateManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_ITEM, m_CTreeCtrl);
	DDX_Control(pDX, IDC_EDIT_PATH, m_CEditPath);
	DDX_Control(pDX, IDC_EDIT_VERSION1, m_CEditVer1);
	DDX_Control(pDX, IDC_EDIT_VERSION2, m_CEditVer2);
	DDX_Control(pDX, IDC_EDIT_VERSION3, m_CEditVer3);
	DDX_Control(pDX, IDC_EDIT_VERSION4, m_CEditVer4);
	DDX_Control(pDX, IDC_EDIT_MODEL_PATH, m_CEditModelPath);
}

BEGIN_MESSAGE_MAP(CUpdateManagerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_MODEL_CREATE, &CUpdateManagerDlg::OnClickedButtonModelCreate)
	ON_BN_CLICKED(IDC_BUTTON_ENTITY_SELETE, &CUpdateManagerDlg::OnClickedButtonEntitySelete)
	ON_BN_CLICKED(IDC_BUTTON_MAIN_CANCEL, &CUpdateManagerDlg::OnClickedButtonMainCancel)
	ON_BN_CLICKED(IDC_BUTTON_MODEL_LOAD2, &CUpdateManagerDlg::OnClickedButtonModelLoad)
	ON_BN_CLICKED(IDC_BUTTON_MODEL_SAVE_PATH, &CUpdateManagerDlg::OnClickedButtonModelSavePath)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_PATH, &CUpdateManagerDlg::OnClickedButtonSavePath)
	ON_BN_CLICKED(IDC_BUTTON_MODEL_MAKES, &CUpdateManagerDlg::OnClickedButtonModelMakes)
	ON_BN_CLICKED(IDC_BUTTON_PACKAGE_MAKE, &CUpdateManagerDlg::OnClickedButtonPackageMake)
	ON_BN_CLICKED(IDC_BUTTON_ENTITY_DELETE, &CUpdateManagerDlg::OnClickedButtonEntityDelete)
	ON_BN_CLICKED(IDC_BUTTON_MODEL_DELETE, &CUpdateManagerDlg::OnClickedButtonModelDelete)
END_MESSAGE_MAP()


// CUpdateManagerDlg 메시지 처리기

BOOL CUpdateManagerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	Init();

	if (m_strPathItem.IsEmpty())
		GetDlgItem(IDC_BUTTON_MODEL_MAKES)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_BUTTON_MODEL_MAKES)->EnableWindow(TRUE);


	
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CUpdateManagerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CUpdateManagerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CUpdateManagerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CUpdateManagerDlg::Init()
{
	// 현재 실행 경로 얻기
	::GetModuleFileName(NULL, m_aszNowPath, 2048);
	// 경로에서 파일명과 확장자만 제거
	PathRemoveFileSpec(m_aszNowPath);

	m_pObjFwUp = new CFirmUpdate();
	m_pObjFwUp->FirmInit();

	// Tree 초기화 & UpdateInfo 구조체 초기화
	memset(m_astTreeNode, 0, sizeof(_stUpdateTreeNode) * 3); // 72
	memset(&m_stUpdateInfo, 0, sizeof(_stUpdateInfo));
	
}

// Init 체크
BOOL CUpdateManagerDlg::CheckInit(pUpdateInfo _pstUpdateInfo)
{
	BOOL bResult = FALSE;

	if (E_MkUpdt_IniFcc == _pstUpdateInfo->uiFcc)
	{
		bResult = TRUE;
	}

	return bResult;
}

void CUpdateManagerDlg::InitCtrl(pUpdateInfo _pstUpdateInfo)
{
	// Local -------------------------------
	CString				strVersion;
	pUpdateInfoModel	pstUpdateInfoModel	= { 0 };
	TCHAR				aszVersion[32]		= { 0 };
	CHAR*				pszFileName			= NULL;
	int					aiVersion[4]		= { 0 };
	int					iStrLen				= 0;
	// -------------------------------------

	GrStrIntToWstr(aszVersion, _pstUpdateInfo->uiUpgdVersion);

	aiVersion[3] = (_pstUpdateInfo->uiUpgdVersion >> 24) & 0xFF;
	aiVersion[2] = (_pstUpdateInfo->uiUpgdVersion >> 16) & 0xFF;
	aiVersion[1] = (_pstUpdateInfo->uiUpgdVersion >> 8) & 0xFF;
	aiVersion[0] = _pstUpdateInfo->uiUpgdVersion & 0xFF;

	strVersion.Format(_T("%d"), aiVersion[0]);
	m_CEditVer1.SetWindowTextW(strVersion.GetString());

	strVersion.Format(_T("%d"), aiVersion[1]);
	m_CEditVer2.SetWindowTextW(strVersion.GetString());

	strVersion.Format(_T("%d"), aiVersion[2]);
	m_CEditVer3.SetWindowTextW(strVersion.GetString());

	strVersion.Format(_T("%d"), aiVersion[3]);
	m_CEditVer4.SetWindowTextW(strVersion.GetString());

	// 16
	for (int i = 0; i < E_FirmUpInfoCnt; i++)
	{
		if (E_FirmUpInfoTypeNone != _pstUpdateInfo->astModelInfo[i].uiType)
		{
			// 트리에 모델 추가 및 출력
			TreeAddModel(_pstUpdateInfo->astModelInfo[i].uiType);

			pstUpdateInfoModel = &_pstUpdateInfo->astModelInfo[i];
			for (int iEntiIdx = 0; iEntiIdx < E_FirmUpEntityCnt; iEntiIdx++)
			{
				if (E_FirmUpEntityNone != pstUpdateInfoModel->uiType)
				{
					pszFileName = pstUpdateInfoModel->astEntity[iEntiIdx].aszFile; //Entity 구조체 파일이름을 담는다.
					iStrLen = GrStrLen(pszFileName);						// 라이브러리 길이 구하는 함수

					if (GrFileIsExist(pszFileName))
					{
						TreeAddVerFile(pstUpdateInfoModel->uiType, pstUpdateInfoModel->astEntity[iEntiIdx].uiType, pszFileName, iStrLen);
					}
				}
			}
		}
	}

	m_CEditPath.SetWindowTextW(m_stUpdateInfo.aszUpgdFileName);
	GrDumyZeroMem(m_aszMkFileName, sizeof(WCHAR) * 1024);
	GrStrWcopy(m_aszMkFileName, m_stUpdateInfo.aszUpgdFileName);
}


// 모델 생성 Button
void CUpdateManagerDlg::OnClickedButtonModelCreate()
{
	// Local ------------------------------
	DlgModelAdd*	pDlgModelAdd	= NULL;
	int				iModelType		= 0;
	// ------------------------------------

	pDlgModelAdd = new DlgModelAdd();
	
	pDlgModelAdd->DoModal();

	if (pDlgModelAdd->m_bModalResult)
	{
		iModelType = pDlgModelAdd->GetModelType();

		if (E_FirmUpInfoTypeNone != iModelType)
		{
			TreeAddModel(iModelType);
		}
	}

	if (NULL != pDlgModelAdd)
	{
		delete pDlgModelAdd;
		pDlgModelAdd = NULL;
	}
}

//트리에 모델 추가
void CUpdateManagerDlg::TreeAddModel(int _iModelType)
{
	// Local ------------------------------
	int iModelType	= _iModelType;
	int iResult		= 0;
	// ------------------------------------

	if (NULL != m_pObjFwUp)
	{
		// 모델 추가 시 중복체크 함수도 포함
		iResult = m_pObjFwUp->AddModelType(iModelType);

		if (E_FirmUpInfoTypeNone <= m_pObjFwUp)
		{
			m_astTreeNode[iResult].uiType = _iModelType;

			switch (_iModelType)
			{
			case E_FirmUpInfoTypeJa1704:
				GrStrCopy(m_astTreeNode[iResult].aszNode, "Ja1704");
				m_astTreeNode[iResult].stNode = m_CTreeCtrl.InsertItem(_T("Ja1704"), NULL, NULL);
				break;
			case E_FirmUpInfoTypeJa1708:
				GrStrCopy(m_astTreeNode[iResult].aszNode, "Ja1708");
				m_astTreeNode[iResult].stNode = m_CTreeCtrl.InsertItem(_T("Ja1708"), NULL, NULL);
				break;
			case E_FirmUpInfoTypeJa1716:
				GrStrCopy(m_astTreeNode[iResult].aszNode, "Ja1716");
				m_astTreeNode[iResult].stNode = m_CTreeCtrl.InsertItem(_T("Ja1716"), NULL, NULL);
				break;
			}

			m_stUpdateInfo.astModelInfo[iResult].uiType = _iModelType;
			GetDlgItem(IDC_BUTTON_MODEL_DELETE)->EnableWindow(TRUE);
		}
		else
		{
			ProcErrCode(iResult);
		}
	}
}

// 에러 처리
void CUpdateManagerDlg::ProcErrCode(int _iResult)
{
	switch (_iResult)
	{
	case E_FirmUpErrCode:
		MessageBox(_T("Error"));
		break;
	case E_FirmUpErrExistType:
		MessageBox(_T("Exist Same Type"));
		break;
	case E_FirmUpErrExistEntity:
		MessageBox(_T("Exist Same Entity Type"));
		break;
	case E_FirmUpErrExistData:
		MessageBox(_T("Exist Same Entity Data Type"));
		break;
	default:
		MessageBox(_T("Error"));
		break;
	}
}

// 버전 파일 선택 Button
void CUpdateManagerDlg::OnClickedButtonEntitySelete()
{
	// Local ------------------------------
	CString			strModelName;
	DlgVerFileAdd*	pDlgVerFileAdd		= NULL;
	HTREEITEM		stTreeItem			= { 0 };
	int				iModelType			= 0;
	int				iModelIdx			= 0;
	int				iVerFileType		= 0;
	CHAR			pszFilePath[128]	= { 0 };
	TCHAR			aszUniNode[16]		= { 0 };
	int				iFileLen			= 0;
	// ------------------------------------

	pDlgVerFileAdd = new DlgVerFileAdd(NULL, m_aszNowPath);

	// 버전 파일 Dlg 호출

	// Tree에서 클릭한 값의 제목을 가져온다.
	stTreeItem = m_CTreeCtrl.GetSelectedItem();
	if (NULL != stTreeItem)
	{

		strModelName = m_CTreeCtrl.GetItemText(stTreeItem);
		iFileLen = strModelName.GetLength();

		// FirmUpdate 클래스에 Model이름 적용
		pDlgVerFileAdd->SetModelName(strModelName);

		for (int i = 0; i < E_FirmUpInfoTypeMaxIdx; i++)
		{
			// Char를 TCHAR로 바꾼다. 이유는 CString 변수와 네임을 비교하기 위해서
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_astTreeNode[i].aszNode, strlen(m_astTreeNode[i].aszNode), aszUniNode, 16);

			// 비교 함수
			if (0 == strModelName.Compare(aszUniNode))
			{
				pDlgVerFileAdd->SetModelType(m_astTreeNode[i].uiType);
				break;
			}
		}

		pDlgVerFileAdd->DoModal();
		if (pDlgVerFileAdd->m_bModalResult)
		{
			// 버전 파일의 정보를 FirmUpdate클래스에서 가져온다.
			pDlgVerFileAdd->GetVerFileType(&iModelType, &iVerFileType, pszFilePath, &iFileLen);
		}

		TreeAddVerFile(iModelType, iVerFileType, pszFilePath, iFileLen);
	}
	else
	{
		MessageBox(_T("선택한 모델이 없습니다"));
	}
	
	if (NULL != pDlgVerFileAdd)
	{
		delete pDlgVerFileAdd;
		pDlgVerFileAdd = NULL;
	}

}

// 트리에 해당 모델에서 버전파일 확인 후 구조체에 데이터 적용
void CUpdateManagerDlg::TreeAddVerFile(int _iModelIdx, int _iVerFileType, CHAR* _pszFilePath, int _iFileLen)
{
	// Local ------------------------------
	CString			strPath;
	CString			strFileName;
	Cls_GrFileCtrl* pFileCtrl			= NULL; // 라이브러리
	CHAR			aszMulFileName[64]	= { 0 };
	PCHAR			pszBuff				= NULL;
	int				iModelType			= 0;
	int				iVerFileType		= 0;
	unsigned int	uiFileSize			= 0;
	int				iResult				= 0;
	int				iModelIdx			= 0;
	// ------------------------------------

	if (NULL != m_pObjFwUp)
	{
		iModelType = _iModelIdx;
		iVerFileType = _iVerFileType;
		pFileCtrl = new Cls_GrFileCtrl(_pszFilePath, FALSE, FALSE);
		
		// open시 파일 선택했다면 IsOpened = TRUE
		if (pFileCtrl->IsOpened())
		{
			uiFileSize = pFileCtrl->GetSize();

			//pszBuff = (PCHAR)malloc(uiFileSize);
			pszBuff = (PCHAR)malloc(uiFileSize + 1);
			memset(pszBuff, 0, uiFileSize + 1);

			pFileCtrl->Seek(0);
			pFileCtrl->Read(pszBuff, uiFileSize);
			iResult = m_pObjFwUp->AddVerFile(iModelType, iVerFileType, pszBuff, uiFileSize);

			if (0 <= iResult)
			{
				// 모델1704가 있으면 0번째 인덱스를 반환함.
				iModelIdx = m_pObjFwUp->GetModelTypeIdx(iModelType);

				if (0 <= iModelIdx)
				{
					m_stUpdateInfo.astModelInfo[iModelIdx].astEntity[iResult].uiType = _iVerFileType;
					
					// 파일 경로 복사
					GrStrCopy(m_stUpdateInfo.astModelInfo[iModelIdx].astEntity[iResult].aszFile, _pszFilePath);

					// 파일 이름만 복사
					GrStrFnGetFileName(m_stUpdateInfo.astModelInfo[iModelIdx].astEntity[iResult].aszFile, aszMulFileName);

					// 메모리 카피
					TreeAddVerFileNode(iModelType, iVerFileType, aszMulFileName);
				}
			}
			else
			{
				ProcErrCode(iResult);
			}

			if (NULL != pszBuff)
			{
				free(pszBuff);
				pszBuff = NULL;
			}
		}
		
		if (NULL != pFileCtrl)
		{
			delete pFileCtrl;
			pFileCtrl = NULL;
		}
	}
}


void CUpdateManagerDlg::TreeAddVerFileNode(int _iModelType, int _iVerFileType, char* _pszFileName)
{
	// Local ------------------------------
	int		iNodeIdx			= 0;
	CHAR	szFileName[64]		= { 0 };
	WCHAR	szNodeName[64]		= { 0 };
	CString strFormatName;
	// ------------------------------------

	iNodeIdx = FindTreeNode(_iModelType);

	switch (_iVerFileType)
	{
	case E_FirmUpEntityLoader:
		GrStrCopy(szFileName, "Loader(");
		break;
	case E_FirmUpEntityFdt:
		GrStrCopy(szFileName, "FDT(");
		break;
	case E_FirmUpEntityUboot:
		GrStrCopy(szFileName, "U-Boot(");
		break;
	case E_FirmUpEntityKernel:
		GrStrCopy(szFileName, "Kernel(");
		break;
	case E_FirmUpEntityLogo:
		GrStrCopy(szFileName, "Logo(");
		break;
	case E_FirmUpEntityRootfs:
		GrStrCopy(szFileName, "RootFs(");
		break;
	}

	// Node에 들어갈 제목 수정
	GrStrCat(szFileName, _pszFileName);
	GrStrCat(szFileName, ")");
	GrStrStrToWstr(szNodeName, szFileName);

	// 트리 Node에 추가
	m_CTreeCtrl.InsertItem(szNodeName, m_astTreeNode[iNodeIdx].stNode, NULL);
	m_CTreeCtrl.Invalidate(TRUE);

	m_stUpdateInfo.astModelInfo[iNodeIdx].uiType = _iModelType;
}


// 모델타입 트리에서 찾기
int CUpdateManagerDlg::FindTreeNode(int _iModelType)
{
	int iResult = 0;

	iResult = -1;

	for (int iNodeIdx = 0; iNodeIdx < E_FirmUpInfoTypeMaxIdx; iNodeIdx++)
	{
		if (_iModelType == m_astTreeNode[iNodeIdx].uiType)
		{
			iResult = iNodeIdx;
			break;
		}
	}

	return iResult;
}

// 저장 경로 설정
void CUpdateManagerDlg::OnClickedButtonSavePath()
{
	// Local ------------------------------
	CFileDialog*	pFileDlg = NULL;
	CString			strPath;
	CString			strInitFile;
	// ------------------------------------

	pFileDlg = new CFileDialog(TRUE, NULL, NULL, OFN_PATHMUSTEXIST, _T("All Files(*.*)|*.*"), NULL);

	//FileDialog 오픈 했을 때 lpstrInitialDir를 통해 초기 경로 폴더로 열기
	pFileDlg->m_ofn.lpstrInitialDir = m_aszNowPath;
	
	if (IDOK == pFileDlg->DoModal())
	{
		strPath = pFileDlg->GetPathName();
		GrDumyZeroMem(m_aszMkFileName, sizeof(WCHAR) * 1024);

		// 경로 + 파일 복사
		//wsprintf(m_aszMkFileName, strPath.GetBuffer(0));
		GrStrWcopy(m_aszMkFileName, (LPWSTR)(LPCTSTR)strPath);
		m_CEditPath.SetWindowTextW(strPath);

		// 구조체에 업데이트 경로+파일명 넣기
		GrDumyZeroMem(m_stUpdateInfo.aszUpgdFileName, 1024);
		GrStrWcopy(m_stUpdateInfo.aszUpgdFileName, (LPWSTR)(LPCTSTR)strPath);
	}
}

// 업데이트 패키지 생성
void CUpdateManagerDlg::OnClickedButtonPackageMake()
{
	// Local ------------------------------
	CString			strPath;
	Cls_GrFileCtrl* pFileCtrl		= NULL;
	void*			pFile			= NULL;
	BOOL			bIsSuccess		= FALSE;
	unsigned int	uiFileSize		= 0;
	unsigned int	uiVersion		= 0;
	unsigned int	uiWorteSize		= 0;
	unsigned int	uiWriteSize		= 0;
	unsigned int	uiResult		= 0;
	int				aiVersion[4]	= { 0 };
	// ------------------------------------

	if (NULL != m_pObjFwUp)
	{
		if (!m_stUpdateInfo.uiUpgdVersion)
		{
			aiVersion[0] = this->GetDlgItemInt(IDC_EDIT_VERSION4);
			aiVersion[1] = this->GetDlgItemInt(IDC_EDIT_VERSION3);
			aiVersion[2] = this->GetDlgItemInt(IDC_EDIT_VERSION2);
			aiVersion[3] = this->GetDlgItemInt(IDC_EDIT_VERSION1);
		}
		else
		{
			aiVersion[0] = (m_stUpdateInfo.uiUpgdVersion >> 24) & 0xFF;
			aiVersion[1] = (m_stUpdateInfo.uiUpgdVersion >> 16) & 0xFF;
			aiVersion[2] = (m_stUpdateInfo.uiUpgdVersion >> 8)	& 0xFF;
			aiVersion[3] = m_stUpdateInfo.uiUpgdVersion			& 0xFF;
		}

		uiVersion = (aiVersion[0] << 24) | (aiVersion[1] << 16) | (aiVersion[2] << 8) | aiVersion[3];

		// 구조체에 버전 저장
		m_pObjFwUp->SetUpdateVersion(uiVersion);

		// 저장할 패키지 사이즈 얻어오기
		pFile = m_pObjFwUp->GetMakeUpdate(&uiFileSize); //에러나서 주석처리
		// 해결! int자료형을 unsigned int로 양수의 정수 데이터만 담는 방법으로 해결함.
		
		// 파일 만드는 클래스 할당
		pFileCtrl = new Cls_GrFileCtrl(m_aszMkFileName, TRUE, TRUE);

		// 구조체에 업데이트 경로+파일명 넣기
		memset(m_stUpdateInfo.aszUpgdFileName, 0, 1024);
		//GrStrWcopy(m_stUpdateInfo.aszUpgdFileName, m_aszMkFileName);
		wsprintf(m_stUpdateInfo.aszUpgdFileName, m_aszMkFileName);
		
		if (TRUE == pFileCtrl->IsOpened())
		{
			uiWorteSize = 0;

			// GEtMakeUpdate에서 받아온 파일 사이즈 담기
			uiWriteSize = uiFileSize;
			
			while (!bIsSuccess)
			{
				// Write함수에 파일 포인터, 파일 사이즈로 결과값 담기
				uiResult = pFileCtrl->Write(pFile, uiWriteSize);
				uiWorteSize = uiWorteSize + uiResult;
				uiWriteSize = uiWriteSize - uiResult;

				// 파일 사이즈랑 같다면
				if (uiWorteSize == uiFileSize)
				{
				//	// 결과 TRUE
					bIsSuccess = TRUE;
					InitMakeModel();
				}
			}
		}
		else
		{
			MessageBox(_T("File Open Error"), _T("Error"), MB_OK | MB_ICONWARNING);
		}

		// 메모리 해제
		if (NULL != pFileCtrl)
		{
			free(pFileCtrl);
			pFileCtrl = NULL;
		}

	}
	if (bIsSuccess)
	{
		MessageBox(_T("Make File Success"), NULL, MB_OK | MB_ICONWARNING);
	}

}

// init파일 만드는 함수
void CUpdateManagerDlg::InitMakeFile()
{
	// Local ------------------------------
	CString			strFile;
	TCHAR			aszPath[2048]	= { 0 };
	Cls_GrFileCtrl* pFileCtrl		= NULL;
	unsigned int	uiVersion		= 0;
	int				iFileNameLen	= 0;
	int				aiVersion[4]	= { 0 };
	// ------------------------------------

	m_stUpdateInfo.uiFcc = E_MkUpdt_IniFcc;

	if (!m_stUpdateInfo.uiUpgdVersion)
	{
		aiVersion[0] = this->GetDlgItemInt(IDC_EDIT_VERSION4);
		aiVersion[1] = this->GetDlgItemInt(IDC_EDIT_VERSION3);
		aiVersion[2] = this->GetDlgItemInt(IDC_EDIT_VERSION2);
		aiVersion[3] = this->GetDlgItemInt(IDC_EDIT_VERSION1);
	}
	else
	{
		aiVersion[0] = (m_stUpdateInfo.uiUpgdVersion >> 24) & 0xFF;
		aiVersion[1] = (m_stUpdateInfo.uiUpgdVersion >> 16) & 0xFF;
		aiVersion[2] = (m_stUpdateInfo.uiUpgdVersion >> 8) & 0xFF;
		aiVersion[3] = m_stUpdateInfo.uiUpgdVersion & 0xFF;
	}

	uiVersion = (aiVersion[0] << 24) | (aiVersion[1] << 16) | (aiVersion[2] << 8) | aiVersion[3];

	m_stUpdateInfo.uiUpgdVersion = uiVersion;
	
	// 경로에 내 파일 생성
	pFileCtrl = new Cls_GrFileCtrl(aszPath, TRUE, TRUE);
	
	if (pFileCtrl)
	{
		if (pFileCtrl->IsOpened())
		{	// 파일 생성
			pFileCtrl->Write(&m_stUpdateInfo, sizeof(_stUpdateInfo));
		}
	}

	if (NULL != pFileCtrl)
	{
		delete pFileCtrl;
		pFileCtrl = NULL;
	}
}

BOOL CUpdateManagerDlg::DestroyWindow()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	//InitMakeFile();
	return CDialogEx::DestroyWindow();
}

int CUpdateManagerDlg::FindModelIndex(int _iModelType)
{
	return m_pObjFwUp->GetModelTypeIdx(_iModelType);
}

int CUpdateManagerDlg::GetModelType(int _iIndex)
{
	int iResult = 0;
	
	iResult = -1;

	switch (_iIndex)
	{
	case 1:
		iResult = E_FirmUpInfoTypeJa1704;
		break;
	case 2:
		iResult = E_FirmUpInfoTypeJa1708;
		break;
	case 3:
		iResult = E_FirmUpInfoTypeJa1716;
		break;
	}

	return iResult;
}

// Cancel 버튼
void CUpdateManagerDlg::OnClickedButtonMainCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	PostMessage(WM_CLOSE);
}


void CUpdateManagerDlg::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CDialogEx::OnOK();
}


// 모델 불러오기 버튼
void CUpdateManagerDlg::OnClickedButtonModelLoad()
{
	// Local ------------------------------
	CString			strPath;
	TCHAR			aszLdPathFile[2048] = { 0 };
	CFileDialog*	pFileDlg			= NULL;
	Cls_GrFileCtrl* pObjFile			= NULL;
	unsigned int	uiFileSize			= 0;
	BOOL			bIsSameModel		= FALSE;
	// ------------------------------------
	
	if (NULL == pFileDlg)
	{
		pFileDlg = new CFileDialog(TRUE, NULL, NULL, OFN_PATHMUSTEXIST, _T("Init Files(*.init)|*.init"), NULL);
	}
	
	pFileDlg->m_ofn.lpstrInitialDir = m_aszNowPath;

	if (IDOK == pFileDlg->DoModal())
	{
		// Init
		m_pObjFwUp->FirmInit();
		m_CTreeCtrl.DeleteAllItems();

		GrDumyZeroMem(&m_stUpdateInfo, sizeof(_stUpdateInfo));

		strPath = pFileDlg->GetPathName();
		wsprintf(aszLdPathFile, strPath);
	
		GrStrWcopy(m_aszMkModelName, (LPWSTR)(LPCTSTR)strPath);
		m_CEditModelPath.SetWindowTextW(strPath);
		m_strPathItem = strPath;

		if (m_strPathItem.IsEmpty())
			GetDlgItem(IDC_BUTTON_MODEL_MAKES)->EnableWindow(FALSE);
		else
			GetDlgItem(IDC_BUTTON_MODEL_MAKES)->EnableWindow(TRUE);

		// 현재 트리에서 있는 모델과 불러온 파일의 모델이 같은 FCC면 TRUE 다르면 FALSE
		bIsSameModel = CheckInit(&m_stUpdateInfo);

		// 파일 존재하는지 체크
		if (GrFileIsExist(aszLdPathFile))
		{
			// 파일을 클래스 포인터로 담는다
			pObjFile = new Cls_GrFileCtrl(aszLdPathFile, FALSE, FALSE);

			// 파일 열기
			if (pObjFile->IsOpened())
			{
				// 파일 사이즈 가져오기
				uiFileSize = (unsigned int)pObjFile->GetSize();

				if (uiFileSize == sizeof(_stUpdateInfo))
				{
					pObjFile->Read(&m_stUpdateInfo, uiFileSize);
				}
			}

			// UpdateInfo 체크
			if (CheckInit(&m_stUpdateInfo))
			{
				InitCtrl(&m_stUpdateInfo);
			}
		}

		if (NULL != pObjFile)
		{
			delete pObjFile;
			pObjFile = NULL;
		}
	}

	if (NULL != pFileDlg)
	{
		delete pFileDlg;
		pFileDlg = NULL;
	}

}

// 모델 저장할 경로 버튼
void CUpdateManagerDlg::OnClickedButtonModelSavePath()
{
	// Local ------------------------------
	CFileDialog*	pFileDlg = NULL;
	CString			strPath;
	CString			strInitFile;
	// ------------------------------------

	pFileDlg = new CFileDialog(TRUE, NULL, NULL, OFN_PATHMUSTEXIST, _T("All Files(*.init)|*.init"), NULL);

	pFileDlg->m_ofn.lpstrInitialDir = m_aszNowPath;

	if (IDOK == pFileDlg->DoModal())
	{
		strPath = pFileDlg->GetPathName();
		GrDumyZeroMem(m_aszMkModelName, sizeof(WCHAR) * 2048);

		GrStrWcopy(m_aszMkModelName, (LPWSTR)(LPCTSTR)strPath);
		m_CEditModelPath.SetWindowTextW(strPath);
		m_strPathItem = strPath;
	}

	if (m_strPathItem.IsEmpty())
		GetDlgItem(IDC_BUTTON_MODEL_MAKES)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_BUTTON_MODEL_MAKES)->EnableWindow(TRUE);

}


// 모델 파일 생성 버튼
void CUpdateManagerDlg::OnClickedButtonModelMakes()
{
	unsigned int uiVersion = 0;
	
	if (NULL != m_pObjFwUp)
	{
		// 구조체에 버전 저장
		m_pObjFwUp->SetUpdateVersion(uiVersion);

		InitMakeModel();
		MessageBox(_T("Make Model Success"), NULL, MB_OK | MB_ICONWARNING);
	}
}

// Model파일 만드는 함수
void CUpdateManagerDlg::InitMakeModel() 
{
	// Local ------------------------------
	CString			strFile;
	CString			strMakeName;
	CFileFind		pFileFind;
	BOOL			bIsFile			= FALSE;
	TCHAR			aszPath[2048]	= { 0 };
	Cls_GrFileCtrl* pFileCtrl		= NULL;
	int				aiVersion[4]	= { 0 };
	unsigned int	uiVersion		= 0;
	int				iFileNameLen	= 0;
	// ------------------------------------

	m_stUpdateInfo.uiFcc = E_MkUpdt_IniFcc;

	if (!m_stUpdateInfo.uiUpgdVersion)
	{
		aiVersion[0] = this->GetDlgItemInt(IDC_EDIT_VERSION4);
		aiVersion[1] = this->GetDlgItemInt(IDC_EDIT_VERSION3);
		aiVersion[2] = this->GetDlgItemInt(IDC_EDIT_VERSION2);
		aiVersion[3] = this->GetDlgItemInt(IDC_EDIT_VERSION1);
	}
	else
	{
		aiVersion[0] = (m_stUpdateInfo.uiUpgdVersion >> 24) & 0xFF;
		aiVersion[1] = (m_stUpdateInfo.uiUpgdVersion >> 16) & 0xFF;
		aiVersion[2] = (m_stUpdateInfo.uiUpgdVersion >> 8)	& 0xFF;
		aiVersion[3] = m_stUpdateInfo.uiUpgdVersion			& 0xFF;
	}

	uiVersion = (aiVersion[0] << 24) | (aiVersion[1] << 16) | (aiVersion[2] << 8) | aiVersion[3];

	m_stUpdateInfo.uiUpgdVersion = uiVersion;
	
	// 파일 존재 여부 체크
	bIsFile = pFileFind.FindFile(m_aszMkModelName); 
	
	// 파일이 존재하지 않으면 새로운 포맷으로 파일을 만들기
	if (!bIsFile)
	{
		strMakeName.Format(_T("%s_V(%d%d%d%d).init"), m_aszMkModelName, aiVersion[3], aiVersion[2], aiVersion[1], aiVersion[0]);
		wsprintf(m_aszMkModelName, strMakeName.GetBuffer(0));
	}
	
	memcpy(aszPath, m_aszMkModelName, 2048);

	// init파일로 만들기
	// 경로에 내 파일 생성
	pFileCtrl = new Cls_GrFileCtrl(aszPath, TRUE, TRUE);

	if (pFileCtrl)
	{
		if (pFileCtrl->IsOpened())
		{	// 파일 생성
			pFileCtrl->Write(&m_stUpdateInfo, sizeof(_stUpdateInfo));
		}
	}

	if (NULL != pFileCtrl)
	{
		delete pFileCtrl;
		pFileCtrl = NULL;
	}

}

void CUpdateManagerDlg::OnClickedButtonEntityDelete()
{
	// Local ------------------------------
	HTREEITEM		stRootTree;
	HTREEITEM		stFirstTreeNode;
	HTREEITEM		stNowTreeNode;
	CString			strSelectName;
	CString			strNodeName;
	int				iModelIdx	= 0;
	int				iNodeIdx	= 0;
	// ------------------------------------

	// 내가 선택한 Node의 Root Tree를 가져오기
	stRootTree = m_CTreeCtrl.GetSelectedItem();
	while (stRootTree)
	{
		stRootTree = m_CTreeCtrl.GetNextItem(stRootTree, TVGN_NEXT);
		if (NULL == stRootTree)
		{
			break;
		}
		iModelIdx++;
	}

	// 내가 선택한 Tree의 Node를 가져오기
	stNowTreeNode = m_CTreeCtrl.GetSelectedItem();
	strSelectName = m_CTreeCtrl.GetItemText(stNowTreeNode);

	stFirstTreeNode = m_CTreeCtrl.GetChildItem(stNowTreeNode);
	stFirstTreeNode = m_CTreeCtrl.GetNextItem(stFirstTreeNode, TVGN_CHILD); // Tree Root가져오기
	stFirstTreeNode = m_CTreeCtrl.GetNextItem(stFirstTreeNode, TVGN_CHILD); // Tree Root의 Node가져오기

	while (stFirstTreeNode)
	{
		strNodeName = m_CTreeCtrl.GetItemText(stFirstTreeNode);
		stFirstTreeNode = m_CTreeCtrl.GetNextItem(stFirstTreeNode, TVGN_NEXT); // Tree Root의 다음 Node 가져오기

		if (strSelectName == strNodeName) //내가 선택한 문자와 같으면 break
		{
			break;
		}
		iNodeIdx++;
	}

	// FirmWare 데이터를 삭제
	m_pObjFwUp->DelVerFile(m_astTreeNode[iModelIdx].uiType, m_stUpdateInfo.astModelInfo[iModelIdx].astEntity[iNodeIdx].uiType);

	if (stNowTreeNode != NULL)
	{
		// 삭제
		m_CTreeCtrl.DeleteItem(stNowTreeNode);												// 트리 삭제
		m_stUpdateInfo.astModelInfo[iModelIdx].astEntity[iNodeIdx].uiType = 0;				// 구조체 데이터 삭제1
		memset(m_stUpdateInfo.astModelInfo[iModelIdx].astEntity[iNodeIdx].aszFile, 0, 256); // 구조체 데이터 삭제2
	}
	else
	{
		AfxMessageBox(_T("삭제할 파일을 선택해야 합니다."));
	}
}

void CUpdateManagerDlg::OnClickedButtonModelDelete()
{
	// Local ------------------------------
	HTREEITEM	stRootTree;
	CFileFind	pFileFind;
	int			iModelIdx = 0;
	// ------------------------------------

	stRootTree = m_CTreeCtrl.GetSelectedItem();

	if (stRootTree)
	{
		while (stRootTree)
		{
			stRootTree = m_CTreeCtrl.GetNextItem(stRootTree, TVGN_NEXT);
			if (NULL == stRootTree)
			{
				break;
			}
			iModelIdx++;
		}

		if (IDYES == AfxMessageBox(_T("해당 모델을 지우시겠습니까?"), MB_YESNO))
		{

			m_pObjFwUp->DelModelType(m_astTreeNode[iModelIdx].uiType);	// 펌웨어 데이터 삭제
			m_CTreeCtrl.DeleteItem(stRootTree);							// 트리에서 모델 삭제

			m_stUpdateInfo.astModelInfo[iModelIdx].uiType = 0;											// 구조체에서 데이터 삭제 1
			memset(m_stUpdateInfo.astModelInfo[iModelIdx].astEntity, 0, sizeof(_stUpdateInfoModel));	// 구조체에서 데이터 삭제 2

		}
	}
	else
	{
		AfxMessageBox(_T("삭제할 모델을 선택해야 합니다."));
	}
}
