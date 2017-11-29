#pragma once
#include "afx.h"
#include "LLDicomds.h"
#include "DicomImage.h"

class CInstance :
	public CObject
{
public:
	CInstance(CLLDicomDS dsLLDicomds);
	~CInstance();

protected:
	CLLDicomDS m_dsLLDicomds;
	CDicomImage* m_pImage;

public:
	BOOL IsSameInstanceID(CString strInstanceId);

	// Update
	void UpdateDicomDs(CLLDicomDS dsLLDicom);

	// Get
	CDicomImage* GetDicomImage();
	CLLDicomDS GetDicomDS();

	// 
	CInstance& operator=(const CInstance& obj);

};

