#include "stdafx.h"
#include "DicomImageProcessor.h"
#include "template_image_process.h"

CDicomImageProcessor::CDicomImageProcessor()
{
}


CDicomImageProcessor::~CDicomImageProcessor()
{
}

 BOOL CDicomImageProcessor::ProcessWindowLevel(BYTE* pOutImage, CDicomImage outImageInfo, CDicomImage inputDicomImage, INT_PTR nW1, INT_PTR nW2)
{
	BOOL bRes = FALSE;

	if (!inputDicomImage.m_stImageInfo.IsValid())
	{
		return FALSE;
	}

	if (outImageInfo.m_stImageInfo.m_nWidth != inputDicomImage.m_stImageInfo.m_nWidth)
	{
		return FALSE;
	}

	if (outImageInfo.m_stImageInfo.m_nHeight != inputDicomImage.m_stImageInfo.m_nHeight)
	{
		return FALSE;
	}

	if (nW1 >= nW2)
	{
		nW1 = nW2 - 1;
	}

	INT_PTR a = sizeof(inputDicomImage.m_pImageData);

	switch (outImageInfo.m_stImageInfo.m_nSamplesPerPixel)
	{
	case 1:
	{
		if (inputDicomImage.m_stImageInfo.m_nBytesPerPixel == 1)
		{
			if (outImageInfo.m_stImageInfo.m_nBytesPerPixel == 1)			// BYTE to BYTE
			{
				bRes = window_level_image<BYTE, BYTE>((BYTE*)inputDicomImage.m_pImageData,
					(BYTE*)pOutImage,
					(UINT)inputDicomImage.m_stImageInfo.m_nBitsPerPixel,
					(UINT)outImageInfo.m_stImageInfo.m_nBitsPerPixel,
					(UINT)inputDicomImage.m_stImageInfo.m_nBytesPerLine,
					(UINT)outImageInfo.m_stImageInfo.m_nBytesPerLine,
					(UINT)inputDicomImage.m_stImageInfo.m_nWidth,
					(UINT)inputDicomImage.m_stImageInfo.m_nHeight,
					(int)nW1,
					(int)nW2);
			}
		}
		else if (inputDicomImage.m_stImageInfo.m_nBytesPerPixel == 2)
		{
			if (outImageInfo.m_stImageInfo.m_nBytesPerPixel == 1)			// WORD to BYTE
			{
				bRes = window_level_image<WORD, BYTE>((WORD*)inputDicomImage.m_pImageData,
					(BYTE*)pOutImage,
					(UINT)inputDicomImage.m_stImageInfo.m_nBitsPerPixel,
					(UINT)outImageInfo.m_stImageInfo.m_nBitsPerPixel,
					(UINT)inputDicomImage.m_stImageInfo.m_nBytesPerLine,
					(UINT)outImageInfo.m_stImageInfo.m_nBytesPerLine,
					(UINT)inputDicomImage.m_stImageInfo.m_nWidth,
					(UINT)inputDicomImage.m_stImageInfo.m_nHeight,
					(int)nW1,
					(int)nW2);
			}
			else if (outImageInfo.m_stImageInfo.m_nBytesPerPixel == 2)		// WORD to WORD
			{
				bRes = window_level_image<WORD, WORD>((WORD*)inputDicomImage.m_pImageData,
					(WORD*)pOutImage,
					(UINT)inputDicomImage.m_stImageInfo.m_nBitsPerPixel,
					(UINT)outImageInfo.m_stImageInfo.m_nBitsPerPixel,
					(UINT)inputDicomImage.m_stImageInfo.m_nBytesPerLine,
					(UINT)outImageInfo.m_stImageInfo.m_nBytesPerLine,
					(UINT)inputDicomImage.m_stImageInfo.m_nWidth,
					(UINT)inputDicomImage.m_stImageInfo.m_nHeight,
					(int)nW1,
					(int)nW2);
			}
		}
	}
	break;
	case 3:
	{
		if (outImageInfo.m_stImageInfo.m_nSamplesPerPixel == 3)
		{
			if (inputDicomImage.m_stImageInfo.m_nBytesPerPixel == 1)
			{
				if (outImageInfo.m_stImageInfo.m_nBytesPerPixel == 1)		// RGB to RGB
				{
					bRes = window_level_image<BYTE, BYTE>((BYTE*)inputDicomImage.m_pImageData,
						(BYTE*)pOutImage,
						(UINT)inputDicomImage.m_stImageInfo.m_nBitsPerPixel,
						(UINT)outImageInfo.m_stImageInfo.m_nBitsPerPixel,
						(UINT)inputDicomImage.m_stImageInfo.m_nBytesPerLine,
						(UINT)outImageInfo.m_stImageInfo.m_nBytesPerLine,
						(UINT)(inputDicomImage.m_stImageInfo.m_nWidth * 3),
						(UINT)inputDicomImage.m_stImageInfo.m_nHeight,
						(int)nW1,
						(int)nW2);
				}
			}
		}
	}
	break;
	}

	return bRes;
}

BOOL CDicomImageProcessor::AdjustDICOMLut(void* pImage, RGBQUAD* pLUT, CDicomImage imageinfoDicom)
{
	if (!pLUT)
	{
		return FALSE;
	}

	if (!pImage)
	{
		return FALSE;
	}

	BOOL bRet = FALSE;

	switch (imageinfoDicom.m_stImageInfo.m_nTotalAllocatedBytes)
	{
	case 1:
	{
		if (imageinfoDicom.m_stImageInfo.m_nSamplesPerPixel == 1)
		{
			if (adjust_dicom_lut<BYTE>((BYTE*)pImage, (RGBQUAD*)pLUT, (UINT)imageinfoDicom.m_stImageInfo.m_nWidth, (UINT)imageinfoDicom.m_stImageInfo.m_nHeight, (UINT)imageinfoDicom.m_stImageInfo.m_nBitsPerPixel))
			{
				bRet = TRUE;
			}
		}
	}
	break;
	case 2:
	{
		if (imageinfoDicom.m_stImageInfo.m_nSamplesPerPixel == 1)
		{
			if (adjust_dicom_lut<WORD>((WORD*)pImage, (RGBQUAD*)pLUT, (UINT)imageinfoDicom.m_stImageInfo.m_nWidth, (UINT)imageinfoDicom.m_stImageInfo.m_nHeight, (UINT)imageinfoDicom.m_stImageInfo.m_nBitsPerPixel))
			{
				bRet = TRUE;
			}
		}
	}
	break;
	case 3:
	{
		if (imageinfoDicom.m_stImageInfo.m_nSamplesPerPixel == 3)
		{
			if (adjust_dicom_lut<RGBTRIPLE*>((RGBTRIPLE**)&pImage, (RGBQUAD*)pLUT, (UINT)imageinfoDicom.m_stImageInfo.m_nWidth, (UINT)imageinfoDicom.m_stImageInfo.m_nHeight, (UINT)imageinfoDicom.m_stImageInfo.m_nBitsPerPixel))
			{
				bRet = TRUE;
			}
		}
	}
	break;
	case 4:
	{
		if (imageinfoDicom.m_stImageInfo.m_nSamplesPerPixel == 1)
		{
			if (adjust_dicom_lut<DWORD>((DWORD*)pImage, (RGBQUAD*)pLUT, (UINT)imageinfoDicom.m_stImageInfo.m_nWidth, (UINT)imageinfoDicom.m_stImageInfo.m_nHeight, (UINT)imageinfoDicom.m_stImageInfo.m_nBitsPerPixel))
			{
				bRet = TRUE;
			}
		}
		else if (imageinfoDicom.m_stImageInfo.m_nSamplesPerPixel == 4)
		{
			if (adjust_dicom_lut<RGBQUAD*>((RGBQUAD**)&pImage, (RGBQUAD*)pLUT, (UINT)imageinfoDicom.m_stImageInfo.m_nWidth, (UINT)imageinfoDicom.m_stImageInfo.m_nHeight, (UINT)imageinfoDicom.m_stImageInfo.m_nBitsPerPixel))
			{
				bRet = TRUE;
			}
		}
	}
	break;
	}

	return bRet;
}