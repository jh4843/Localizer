#pragma once

#include "MyDicomLanguage.h"
#include "DicomImage.h"
#include "LLDicomDS.h"

#include <math.h>

class CDicomParser :
	public LDicomDS
{
public:
	CDicomParser();
	~CDicomParser();

public:
	CLLDicomDS::DICOM_HEADER_INFO_ m_dcmHeaderInfo;
	CArray<CDicomImage, CDicomImage> m_aryDicomImage;	// for multi frame

	// to Control
	BOOL m_bInitialized;
	pDICOMELEMENT m_pParentElement;
	pDICOMELEMENT m_pCurrentElement;

private:
	static CMyDicomLanguage m_dcmLanguage;
	int m_nCurNLS;

public:
	UINT Initialize();

	//
	L_UINT16 LoadDS(L_TCHAR *pszName, L_UINT16 nFlags);

	// GET
	CString GetValue(UINT uTag = 0, int nOffset = 1);
	CString GetValue(pDICOMELEMENT pElement, int nOffset = 1);
	CString GetValue(UINT uTag, int nOffset, pDICOMELEMENT pCurElement);

	CString GetStudyID();
	CString GetSeriesID();
	CString GetInstanceID();

	//SET
	void SetDcmHeaderInfo(CLLDicomDS::DICOM_HEADER_INFO_ dsHeaderInfo);
	void AddDcmImageInfo(CDicomImage dsImageInfo);

	//
	BOOL ParseDicomHeader();
	BOOL ParseImageInfo();

	UINT MoveRootElement();
	VOID SetFileName(CString strFileName);
	VOID SetCurrentElement(pDICOMELEMENT pCurrentElement);
	L_VOID ResetDS();

	int ParseNLS();
	int& GetCurNLS();
	CLLDicomDS GetLLDicomDS(INT_PTR nFrameIndex = 0);

	CDicomParser& operator=(const CDicomParser& obj);

private:
	FLOAT GetOrientationOrthogonalX(INT_PTR nRowY, INT_PTR nRowZ, INT_PTR nColY, INT_PTR nColZ, INT_PTR nFrameIndex = 0);
	FLOAT GetOrientationOrthogonalY(INT_PTR nRowX, INT_PTR nRowZ, INT_PTR nColX, INT_PTR nColZ, INT_PTR nFrameIndex = 0);
	FLOAT GetOrientationOrthogonalZ(INT_PTR nRowX, INT_PTR nRowY, INT_PTR nColX, INT_PTR nColY, INT_PTR nFrameIndex = 0);
};

