#pragma once
#include "DicomImage.h"

class CDicomImageProcessor
{
public:
	CDicomImageProcessor();
	~CDicomImageProcessor();

public:
	BOOL ProcessWindowLevel(BYTE* pOutImage, CDicomImage outImageInfo, CDicomImage inputDicomImage, INT_PTR nW1, INT_PTR nW2);
	BOOL AdjustDICOMLut(void* pImage, RGBQUAD* pLUT, CDicomImage* pImageInfo);
};

