#include "stdafx.h"
//#include "Control.h"
#include "Search.h"
#include "MFC21_Text_SearchExDlg.h"

// 생성자
CSearch::CSearch()
{
}

// 소멸자
CSearch::~CSearch()
{
}

char* CSearch::TextFind(char* _pszBuffer, int _iBufferSize, char* _pszText, int _iTextSize)
{

	// 동적 할당한 메모리 주소
	char* pszFirstBufferAddress = NULL;
	pszFirstBufferAddress = _pszBuffer;

	char* pszWordAddress = NULL;
	pszWordAddress = _pszText;

	// 단어 체크
	bool bCheckWord = FALSE;

	// 비교
	// 텍스트 단어
	for (int i = 0; i < (_iBufferSize - _iTextSize); i++)
	{
		bCheckWord = WordCompare(pszFirstBufferAddress, _pszText, _iTextSize);	
		
		if (TRUE == bCheckWord)
		{
			return pszWordAddress;
		}
		else
		{
			pszFirstBufferAddress++;
		}
	}
	return NULL;
}

bool CSearch::WordCompare(char* _pszText, char* _pszWord, int _iWordSize)
{	
	for (int i = 0; i < _iWordSize; i++)
	{	
		// 맞으면 검색 단어의 다음 인덱스
		if (*_pszWord == *_pszText)
		{
			_pszWord++;
			_pszText++;
		}
		// 아니면 검색 단어 처음 인덱스
		else
		{
			return FALSE;
		}	
	}
	return TRUE;
}




















//// 배열 값 넣기
//void CSearch::InputValue(int* _piBuffer, int _iCount, int _iValue)
//{
//	int* piNum = NULL;
//	piNum = _piBuffer;
//
//	for (int i = 0; i < _iCount; i++)
//	{
//		*piNum = _iValue;
//		piNum++;
//	}
//}
//
//// 오름차순
//void CSearch::ValueAsc(int* _piBuffer, int _iCount)
//{
//	int* piNum = NULL;
//	piNum = _piBuffer;
//
//	for (int i = 0; i < _iCount; i++)
//	{
//		*piNum = i;
//		piNum++;
//	}
//}
//
//// 3의 배수를 합한 것
//int CSearch::SumResult(int* _piBuffer, int _iCount)
//{
//	int sum = 0;
//	int* piNum = NULL;
//	piNum = _piBuffer;
//
//	for (int i = 0; i < _iCount; i++)
//	{
//		if (*piNum % 3 == 0)
//			sum += i;
//		
//		piNum++;
//	}
//	return sum;
//}
//
//void CSearch::GetValue(int* _piBuffer, int _iCount, St_Test* _pstResult)
//{
//	int* piNum = NULL;
//	piNum = _piBuffer;
//	
//	for (int i = 0; i < _iCount; i++)
//	{
//		switch (i)
//		{
//		case 0:
//			_pstResult->test0 = *piNum;
//			break;
//
//		case 100:
//			_pstResult->test100 = *piNum;
//			break;
//
//		case 200:
//			_pstResult->test200 = *piNum;
//			break;
//
//		case 300:
//			_pstResult->test300 = *piNum;
//			break;
//
//		default:
//			break;
//		}
//		piNum++;
//	}
//
//}

//BOOL CSearch::IsStrFind(char* A_PtrFindStr, int A_FindStrSize, char* A_PtrTextStr, int A_TextSize)
//{
//#if 0
//	char* Tv_PtrTxt;
//
//	Tv_PtrTxt = A_PtrTextStr;
//	for (int i = 0; i < A_TextSize; i++)
//	{
//		*A_PtrTextStr = 10;
//
//		//next
//		A_PtrTextStr++;
//	}
//
//	Tv_PtrTxt = A_PtrTextStr;
//
//	Tv_PtrTxt = (char*)((unsigned int)A_PtrTextStr + 200);
//	for (int i = 0; i < 5; i++)
//	{
//		*Tv_PtrTxt = 10;
//		Tv_PtrTxt++;
//	}
//#endif
//		/*
//
//		Search_Word		= A_PtrFindStr;
//		Search_WordCnt	= A_FindStrSize;
//		Text			= A_PtrTextStr;
//		TextCnt			= A_TextSize;
//		
//		for (TextCnt = 0; Search_WordCnt < TextCnt; Search_WordCnt++)
//		{
//			IsStrMatch(Search_Word);
//
//
//			next
//			Search_WordCnt++;
//		}
//		
//		
//		*/
//	return true;
//}

//BOOL CSearch::IsStrMatch()
//{
//	/*
//	
//	if (Tv_PtrFindStr == *Tv_PtrTextStr)
//	{
//
//	}
//	
//	*/
//	return FALSE;
//}