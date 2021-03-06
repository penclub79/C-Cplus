/*
firmware upgrade
*/

//--------------------------------------------------------------------
//Environment conditional
#ifndef	_EPC_FirmUpdt
#define	_EPC_FirmUpdt

//====================================================================
// uses
#include <GrTypeBase.h>

//====================================================================
//const
#define E_FirmUpHdFcc				0x75707302//UPS1
#define E_FirmUpEdFcc				0x75706502//UPE1
#define E_FirmUpVer					1

#define	E_FirmUpInfoCnt				16
#define E_FirmUpEntityCnt			16

#define	E_FirmUpInfoTypeMaxIdx		24
#define	E_FirmUpInfoTypeNone		0
/*#define	E_FirmUpInfoTypeJa1704	1
#define	E_FirmUpInfoTypeJa1708	2
#define	E_FirmUpInfoTypeJa1716	3*/

#define	E_FirmUpInfoTypeJa1704		21
#define	E_FirmUpInfoTypeJa1708		22
#define	E_FirmUpInfoTypeJa1716		23



#define	E_FirmUpEntityNone			0
#define	E_FirmUpEntityLoader		1
#define	E_FirmUpEntityFdt			2
#define	E_FirmUpEntityUboot			3
#define	E_FirmUpEntityKernel		4
#define	E_FirmUpEntityLogo			5
#define	E_FirmUpEntityRootfs		6

#define E_FirmUpErrCode				-1
#define E_FirmUpErrExistType		-2
#define E_FirmUpErrExistEntity		-3
#define E_FirmUpErrExistData		-4

//====================================================================
//global macro

//====================================================================
//global type
typedef struct _stFirmUpEntity
{
	__u32				Type;
	__u32				Offset;
	__u32				Size;
} *pFirmUpEntity;

typedef struct _stFirmUpInfo
{
	__u32				ModelType;
	_stFirmUpEntity		Entity[E_FirmUpEntityCnt];
} *pFirmUpInfo;

typedef struct _stFirmUpHeader
{
	__u32				Fcc; // 파일 맞는지 아닌지.(1)
	__u32				UpgdVer;
	__u32				Size;
	_stFirmUpInfo		FirmInfo[E_FirmUpInfoCnt];
} *pFirmUpHeader;

typedef struct _stFirmUpEnd
{
	__u32				Fcc; // 파일 맞는지 아닌지.(2)
} *pFirmUpEnd;

//====================================================================
//class
class CFirmUpdate
{

public:
	CFirmUpdate();
	virtual	~CFirmUpdate();

	// Firm Data 초기화
	void	FirmInit();

	// 모델 추가
	int		AddModelType(int _iModelType);
	
	// 버전 파일 추가
	int		AddVerFile(int _iModelType, int _iVerFileType, char* _pSrc, int _iFileSize);

	int		GetModelTypeIdx(int _iModelType);

	void	SetUpdateVersion(unsigned int _uiVersion);

	// 패키지 저장시 사이즈 얻기
	void*	GetMakeUpdate(unsigned int* _puiSize);

	// 모델 삭제
	void	DelModelType(int _iModelType);

	// 버전 파일 삭제
	void	DelVerFile(int _iModelType, int _iVerFileType);

protected:
	// 버전 파일 체크
	int		ChkEntityType(int _iVersionType, pFirmUpEntity _stEntity);

	// 모델 타입 체크
	int		ChkModelType(unsigned int _uiModelType);

private:
	_stFirmUpHeader		m_stFirmHeader;
	_stFirmUpEnd		m_stFirmEd;
	void*				m_pData[E_FirmUpInfoCnt][E_FirmUpEntityCnt];
	void*				m_pMakeUpdate;

	
	void	SetUpdtVer(int _iVer);

};

//====================================================================
//global function


//====================================================================
//out Environment conditional
#endif
