/*
	DVR FirmWare Upgrade
*/

#include "GrDumyTool.h"
#include <GrLnxApp.h>
#include <GrDebug.h>
#include "FirmUpdate.h"

CFirmUpdate::CFirmUpdate()
{
	m_stFirmEd = { 0 };
	m_stFirmHeader = { 0 };
	m_pMkUpdt = NULL;
	
	for (int i = 0; i < E_FirmUpInfoCnt; i++)
	{
		for (int si = 0; si < E_FirmUpInfoCnt; si++)
		{
			m_pData[i][si] = NULL;
		}
	}

}

CFirmUpdate::~CFirmUpdate()
{

}

void CFirmUpdate::FirmInit()
{
	// local ---------------------------------------
	unsigned int	uiResult		= 0;
	unsigned int	uiIndex			= 0;
	unsigned int	uiCount			= 0;

	pFirmUpHeader	stFirmHd		= { 0 };
	pFirmUpInfo		stFirmUpInfo	= { 0 };
	pFirmUpEnd		stFirmEnd		= { 0 };
	BOOL			bIsExist		= FALSE;
	// ---------------------------------------------

	// �߿��� ����ü �� ����
	m_stFirmHeader.Fcc		= E_FirmUpHdFcc;
	m_stFirmHeader.UpgdVer	= E_FirmUpVer;
	m_stFirmEd.Fcc			= E_FirmUpEdFcc;
	m_stFirmHeader.Size		= sizeof(pFirmUpHeader) + sizeof(pFirmUpEnd);

	// m_pData NULL üũ
	for (int i = 0; i < E_FirmUpInfoCnt; i++)
	{
		for (int si = 0; si < E_FirmUpInfoCnt; si++)
		{
			if (NULL != m_pData[i][si])
			{
				free(m_pData[i][si]);
				m_pData[i][si] = NULL;
			}
		}
	}

	// make Update NULL üũ
	if (NULL != m_pMkUpdt)
	{
		free(m_pMkUpdt);
		m_pMkUpdt = NULL;
	}

}

// �� Ÿ�� �߰�
int	CFirmUpdate::AddModelType(unsigned int _uiModelType)
{
	// local ---------------------------------------
	int iResult = 0;
	pFirmUpInfo stFirmUpInfo = { 0 };
	BOOL bIsExist = FALSE;
	int iIndex = 0;
	// ---------------------------------------------

	iResult = E_FirmUpErrCode;
	
	// 24���� �۰ų� ���� 0�� �ƴ� model type
	if ((E_FirmUpInfoTypeMaxIdx >= _uiModelType) && (_uiModelType != E_FirmUpInfoTypeNone))
	{
		// �� �ߺ� üũ
		if (E_FirmUpErrCode != ChkModelType(_uiModelType))
		{
			// �� ����
			bIsExist = TRUE;
		}

		// ���� �������� ������
		if (!bIsExist)
		{
			// �� ������ ã�´�. (�� ������ �־�� ���� �߰��� �� ����)
			iIndex = ChkModelType(E_FirmUpInfoTypeNone);

			// ������ �ƴҶ�
			if (E_FirmUpErrCode != iIndex)
			{
				stFirmUpInfo = &m_stFirmHeader.FirmInfo[iIndex];
				stFirmUpInfo->ModelType = _uiModelType;
				iResult = iIndex;
			}
		}
	}
	else
	{
		DbgMsgPrint("Cls_FirmUpdt::AddType() : Model Type Index error.\n");
	}

	return iResult;
}

// �� Ÿ�� üũ
int	CFirmUpdate::ChkModelType(unsigned int _uiModelType)
{
	// local ---------------------------------------
	int iResult = 0;
	pFirmUpInfo stFirmUpInfo = { 0 };
	// ---------------------------------------------
	iResult = E_FirmUpErrCode;

	stFirmUpInfo = m_stFirmHeader.FirmInfo;
	
	// E_FirmUpInfoCnt = 16
	for (int i = 0; i < E_FirmUpInfoCnt; i++)
	{
		if (stFirmUpInfo->ModelType == _uiModelType)
		{
			iResult = i;
			break;
		}

		stFirmUpInfo++;
	}

	return iResult;
}

int	CFirmUpdate::AddVerFile(unsigned int _uiModelType, unsigned int _uiVersionType, void* _pSrc, unsigned int _uiSize)
{

}