#include "stdafx.h"
#include "DicomParser.h"

CMyDicomLanguage CDicomParser::m_dcmLanguage;

CDicomParser::CDicomParser()
	: m_bInitialized(FALSE)
{
	if (!m_bInitialized)
	{
		Initialize();
	}
	m_dcmHeaderInfo.m_strFileName = _T("");
	m_nCurNLS = -1;
}


CDicomParser::~CDicomParser()
{
	ResetDS();
}


UINT CDicomParser::Initialize()
{
	m_pParentElement = NULL;
	m_pCurrentElement = NULL;

	MoveRootElement();

	UINT ndcmRet = DICOM_SUCCESS;

	m_bInitialized = TRUE;

	m_nCurNLS = -1;

	m_dcmHeaderInfo.Init();

	return ndcmRet;
}

L_UINT16 CDicomParser::LoadDS(L_TCHAR *pszName, L_UINT16 nFlags)
{
	m_dcmHeaderInfo.m_strFileName.Format(_T("%s"), pszName);
	//
	//return LDicomDS::LoadDS(pszName, nFlags);
	L_UINT16 nRet = LDicomDS::LoadDS(pszName, nFlags);

	return nRet;
}

CString CDicomParser::GetValue(pDICOMELEMENT pElement, int nOffset)
{
	CString strValue;
	pDICOMELEMENT pOldCurrentElement = m_pCurrentElement;
	m_pCurrentElement = pElement;
	strValue = GetValue((UINT)0, nOffset);
	m_pCurrentElement = pOldCurrentElement;
	return strValue;
}

CString CDicomParser::GetValue(UINT uTag, int nOffset, pDICOMELEMENT pCurElement)
{
	CString strValue;
	pDICOMELEMENT pOldCurrentElement = m_pCurrentElement;
	m_pCurrentElement = pCurElement;
	strValue = GetValue(uTag, nOffset);
	m_pCurrentElement = pOldCurrentElement;

	return strValue;
}

CString CDicomParser::GetStudyID()
{
	return m_dcmHeaderInfo.m_strStudyInstanceUID;
}

CString CDicomParser::GetSeriesID()
{
	return m_dcmHeaderInfo.m_strSeriesInstanceUID;
}

CString CDicomParser::GetInstanceID()
{
	return m_dcmHeaderInfo.m_strSOPInstanceUID;
}

CString CDicomParser::GetValue(UINT uTag, int nOffset)
{
	L_INT32 nIndex = 0;
	L_INT32 nCount = 0;
	pDICOMELEMENT pElement = NULL;

	CString strValue;
	CString	strToken;
	CString strDelimiter;

	strValue.Empty();
	strToken.Empty();
	strDelimiter.Empty();

	if (nOffset < 0)
		strDelimiter.Format(_T("\\"));

	if (!m_pParentElement && !m_pCurrentElement)
		MoveRootElement();

	if (uTag == 0)
		pElement = m_pCurrentElement;
	else
		pElement = LDicomDS::FindFirstElement(m_pCurrentElement, uTag, TRUE);

	if (!pElement)
	{
		pElement = LDicomDS::FindFirstElement(NULL, uTag, FALSE);
	}

	if (pElement == NULL)
		return strValue;

	nCount = LDicomDS::GetCountValue(pElement);
	if (((pElement->nLength == ELEMENT_LENGTH_MAX)) || (pElement->nLength > 1024) || (nCount == 0))
		return strValue;

	switch (pElement->nVR)
	{
	case VR_OB:
	case VR_UN:
	{
		L_UCHAR* pChar = NULL;

		pChar = GetCharValue(pElement, 0, nCount);
		for (nIndex = 0; nIndex < nCount; nIndex++)
		{
			if (nOffset > 0)
			{
				if (nIndex == (nOffset - 1))
				{
					strValue.Format(_T("%02X"), (L_UCHAR)pChar[nIndex]);
					break;
				}
			}
			else
			{
				strToken.Format(_T("%02X"), (L_UCHAR)pChar[nIndex]);
				if (nIndex == 0)
					strValue = strValue + strToken;
				else
					strValue = strValue + strDelimiter + strToken;
			}
		}
	}
	break;

	case VR_SS:
	{
		L_INT16* pShort = NULL;
		pShort = GetShortValue(pElement, 0, nCount);
		for (nIndex = 0; nIndex < nCount; nIndex++)
		{
			if (nOffset > 0)
			{
				if (nIndex == (nOffset - 1))
				{
					strValue.Format(_T("%d"), pShort[nIndex]);
					break;
				}
			}
			else
			{
				strToken.Format(_T("%d"), pShort[nIndex]);
				if (nIndex == 0)
					strValue = strValue + strToken;
				else
					strValue = strValue + strDelimiter + strToken;
			}
		}
	}
	break;

	case VR_US:
	case VR_OW:
	{
		L_INT16* pShort = NULL;
		pShort = GetShortValue(pElement, 0, nCount);
		for (nIndex = 0; nIndex < nCount; nIndex++)
		{
			if (nOffset > 0)
			{
				if (nIndex == (nOffset - 1))
				{
					strValue.Format(_T("%u"), (L_UINT16)pShort[nIndex]);
					break;
				}
			}
			else
			{
				strToken.Format(_T("%u"), (L_UINT16)pShort[nIndex]);
				if (nIndex == 0)
					strValue = strValue + strToken;
				else
					strValue = strValue + strDelimiter + strToken;
			}
		}
	}
	break;

	case VR_SL:
	case VR_IS:
	{
		L_INT32* pLong = NULL;
		pLong = GetLongValue(pElement, 0, nCount);
		for (nIndex = 0; nIndex < nCount; nIndex++)
		{
			if (nOffset > 0)
			{
				if (nIndex == (nOffset - 1))
				{
					strValue.Format(_T("%ld"), pLong[nIndex]);
					break;
				}
			}
			else
			{
				strToken.Format(_T("%ld"), pLong[nIndex]);
				if (nIndex == 0)
					strValue = strValue + strToken;
				else
					strValue = strValue + strDelimiter + strToken;
			}
		}
	}
	break;

	case VR_UL:
	{
		L_INT32* pLong = NULL;
		pLong = GetLongValue(pElement, 0, nCount);
		for (nIndex = 0; nIndex < nCount; nIndex++)
		{
			if (nOffset > 0)
			{
				if (nIndex == (nOffset - 1))
				{
					strValue.Format(_T("%lu"), (L_UINT32)pLong[nIndex]);
					break;
				}
			}
			else
			{
				strToken.Format(_T("%lu"), (L_UINT32)pLong[nIndex]);
				if (nIndex == 0)
					strValue = strValue + strToken;
				else
					strValue = strValue + strDelimiter + strToken;
			}
		}
	}
	break;

	case VR_AT:
	{
		L_INT32* pLong = NULL;
		pLong = GetLongValue(pElement, 0, nCount);
		for (nIndex = 0; nIndex < nCount; nIndex++)
		{
			if (nOffset > 0)
			{
				if (nIndex == (nOffset - 1))
				{
					strValue.Format(_T("%04X:%04x"), GETGROUP((L_UINT32)pLong[nIndex]), GETELEMENT((L_UINT32)pLong[nIndex]));
					break;
				}
			}
			else
			{
				strToken.Format(_T("%04X:%04x"), GETGROUP((L_UINT32)pLong[nIndex]), GETELEMENT((L_UINT32)pLong[nIndex]));
				if (nIndex == 0)
					strValue = strValue + strToken;
				else
					strValue = strValue + strDelimiter + strToken;
			}
		}
	}
	break;

	case VR_FL:
	{
		L_FLOAT* pFloat = NULL;
		pFloat = GetFloatValue(pElement, 0, nCount);
		for (nIndex = 0; nIndex < nCount; nIndex++)
		{
			if (nOffset > 0)
			{
				if (nIndex == (nOffset - 1))
				{
					strValue.Format(_T("%f"), pFloat[nIndex]);
					break;
				}
			}
			else
			{
				strToken.Format(_T("%f"), pFloat[nIndex]);
				if (nIndex == 0)
					strValue = strValue + strToken;
				else
					strValue = strValue + strDelimiter + strToken;
			}
		}
	}
	break;

	case VR_FD:
	case VR_DS:
	{
		L_DOUBLE* pDouble = NULL;
		pDouble = GetDoubleValue(pElement, 0, nCount);
		for (nIndex = 0; nIndex < nCount; nIndex++)
		{
			if (nOffset > 0)
			{
				if (nIndex == (nOffset - 1))
				{
					strValue.Format(_T("%f"), pDouble[nIndex]);
					break;
				}
			}
			else
			{
				strToken.Format(_T("%f"), pDouble[nIndex]);
				if (nIndex == 0)
					strValue = strValue + strToken;
				else
					strValue = strValue + strDelimiter + strToken;
			}
		}
	}
	break;

	case VR_CS:
	case VR_SH:
	case VR_LO:
	case VR_AE:
	case VR_LT:
	case VR_ST:
	case VR_UI:
	case VR_UT:
	case VR_PN:
	{
		int nLength = pElement->nLength;
		if (nLength > 0)
		{
			//				strValue = LDicomDS::GetStringValue(pElement, 0, nCount);
			//				USES_CONVERSION;

			CStringA strTempA;
			LDicomDS::GetBinaryValue(pElement, strTempA.GetBuffer(nLength + 1), nLength);
			strTempA.ReleaseBuffer(nLength);

			strValue = m_dcmLanguage.DecodeNLSEx(strTempA, GetCurNLS());
		}
	}
	break;

	case VR_AS:
	{
		pVALUEAGE pAge;
		pAge = LDicomDS::GetAgeValue(pElement, 0, nCount);
		for (nIndex = 0; nIndex < nCount; nIndex++)
		{
			if (nOffset > 0)
			{
				if (nIndex == (nOffset - 1))
					strValue.Format(_T("%u%c"), pAge[nIndex].nNumber, pAge[nIndex].nReference);
			}
			else
			{
				strToken.Format(_T("%u%c"), pAge[nIndex].nNumber, pAge[nIndex].nReference);
				if (nIndex == 0)
					strValue = strValue + strToken;
				else
					strValue = strValue + strDelimiter + strToken;
			}
		}
	}
	break;

	case VR_DA:
		if (nCount > 0)
		{
			BOOL bRet = FALSE;
			L_CHAR szValue[MAX_PATH];
			memset(szValue, 0, sizeof(szValue));
			bRet = LDicomDS::GetBinaryValue(pElement, szValue, MAX_PATH);
			strValue = CA2W(szValue);
		}
		break;

	case VR_TM:
		if (nCount > 0)
		{
			BOOL bRet = FALSE;
			L_CHAR szValue[MAX_PATH];
			memset(szValue, 0, sizeof(szValue));
			bRet = LDicomDS::GetBinaryValue(pElement, szValue, MAX_PATH);
			//strValue.Format(_T("%s"), CA2W(szValue));
			strValue = CA2W(szValue);
		}
		break;

	case VR_DT:
		if (nCount > 0)
		{
			BOOL bRet = FALSE;
			L_CHAR szValue[MAX_PATH];
			memset(szValue, 0, sizeof(szValue));
			bRet = LDicomDS::GetBinaryValue(pElement, szValue, MAX_PATH);
			//strValue.Format(_T("%s"), CA2W(szValue));
			strValue = CA2W(szValue);
		}
		break;
	}

	FreeValue(pElement);
	return strValue;
}


BOOL CDicomParser::ParseDicomHeader()
{
	CString strValue;
	pDICOMELEMENT pElement = nullptr;

	// Patient
	m_dcmHeaderInfo.m_strPatientName = GetValue(TAG_PATIENT_NAME);
	m_dcmHeaderInfo.m_strPatientName.Replace(_T("^"), _T(" "));
	m_dcmHeaderInfo.m_strPatientID = GetValue(TAG_PATIENT_ID);
	m_dcmHeaderInfo.m_strBirthDate = GetValue(TAG_PATIENT_BIRTH_DATE);
	m_dcmHeaderInfo.m_strSex = GetValue(TAG_PATIENT_SEX);
	m_dcmHeaderInfo.m_strPatientComments = GetValue(TAG_PATIENT_COMMENTS);

	//Study
	m_dcmHeaderInfo.m_strStudyID = GetValue(TAG_STUDY_ID);
	m_dcmHeaderInfo.m_strStudyInstanceUID = GetValue(TAG_STUDY_INSTANCE_UID);
	m_dcmHeaderInfo.m_strStudyDescription = GetValue(TAG_STUDY_DESCRIPTION);

	// Series
	m_dcmHeaderInfo.m_strModality = GetValue(TAG_MODALITY);
	strValue = GetValue(TAG_SERIES_NUMBER);
	if (!strValue.IsEmpty())
	{
		m_dcmHeaderInfo.m_lSeriesNumber = _ttol(strValue);
	}
	m_dcmHeaderInfo.m_strSeriesInstanceUID = GetValue(TAG_SERIES_INSTANCE_UID);

	// Instance
	strValue = GetValue(TAG_INSTANCE_NUMBER);
	if (!strValue.IsEmpty())
	{
		m_dcmHeaderInfo.m_lInstanceNumber = _ttol(strValue);
	}
	m_dcmHeaderInfo.m_strSOPInstanceUID = GetValue(TAG_SOP_INSTANCE_UID);
	m_dcmHeaderInfo.m_strSOPClassUID = GetValue(TAG_SOP_CLASS_UID);
	m_dcmHeaderInfo.m_strTransferSyntaxUID = GetValue(TAG_REFERENCED_TRANSFER_SYNTAX_UID_IN_FILE);

	
	
	return TRUE;

}

BOOL CDicomParser::ParseImageInfo()
{
	DICOMIMAGE dicomImage;
	BITMAPHANDLE BitmapHandle;

	pDICOMELEMENT pElement = FindFirstElement(NULL, TAG_IMAGE_POSITION_PATIENT, FALSE);
	if (pElement)
	{
		L_DOUBLE* pDouble_X = NULL;
		L_DOUBLE* pDouble_Y = NULL;
		L_DOUBLE* pDouble_Z = NULL;

		pDouble_X = GetDoubleValue(pElement, 0, 1);
		pDouble_Y = GetDoubleValue(pElement, 1, 1);
		pDouble_Z = GetDoubleValue(pElement, 2, 1);

		if (pDouble_X != NULL && pDouble_Y != NULL && pDouble_Z != NULL)
		{
			m_DicomImage.m_stImageInfo.m_fImagePosition_X = *pDouble_X;
			m_DicomImage.m_stImageInfo.m_fImagePosition_Y = *pDouble_Y;
			m_DicomImage.m_stImageInfo.m_fImagePosition_Z = *pDouble_Z;
		}
	}

	pElement = FindFirstElement(NULL, TAG_IMAGE_ORIENTATION_PATIENT, FALSE);
	if (pElement)
	{
		L_DOUBLE* pDouble_RowX = NULL;
		L_DOUBLE* pDouble_RowY = NULL;
		L_DOUBLE* pDouble_RowZ = NULL;
		L_DOUBLE* pDouble_ColX = NULL;
		L_DOUBLE* pDouble_ColY = NULL;
		L_DOUBLE* pDouble_ColZ = NULL;

		pDouble_RowX = GetDoubleValue(pElement, 0, 1);
		pDouble_RowY = GetDoubleValue(pElement, 1, 1);
		pDouble_RowZ = GetDoubleValue(pElement, 2, 1);
		pDouble_ColX = GetDoubleValue(pElement, 3, 1);
		pDouble_ColY = GetDoubleValue(pElement, 4, 1);
		pDouble_ColZ = GetDoubleValue(pElement, 5, 1);

		if (pDouble_RowX != NULL && pDouble_RowY != NULL && pDouble_RowZ != NULL &&
			pDouble_ColX != NULL && pDouble_ColY != NULL && pDouble_ColZ != NULL)
		{
			m_DicomImage.m_stImageInfo.m_fImageOrientationRowX = *pDouble_RowX;
			m_DicomImage.m_stImageInfo.m_fImageOrientationRowY = *pDouble_RowY;
			m_DicomImage.m_stImageInfo.m_fImageOrientationRowZ = *pDouble_RowZ;
			m_DicomImage.m_stImageInfo.m_fImageOrientationColX = *pDouble_ColX;
			m_DicomImage.m_stImageInfo.m_fImageOrientationColY = *pDouble_ColY;
			m_DicomImage.m_stImageInfo.m_fImageOrientationColZ = *pDouble_ColZ;
			m_DicomImage.m_stImageInfo.m_fImageOrientationOrthogonalX = GetOrientationOrthogonalX();
			m_DicomImage.m_stImageInfo.m_fImageOrientationOrthogonalY = GetOrientationOrthogonalY();
			m_DicomImage.m_stImageInfo.m_fImageOrientationOrthogonalZ = GetOrientationOrthogonalZ();
		}
	}

	// Pixel Information
	m_DicomImage.m_stImageInfo.m_fPixelXSpacing = _ttof((LPTSTR)(LPCTSTR)GetValue(TAG_PIXEL_SPACING));
	if (m_DicomImage.m_stImageInfo.m_fPixelXSpacing < 0.0f)
	{
		m_DicomImage.m_stImageInfo.m_fPixelXSpacing = 0.0f;
	}

	m_DicomImage.m_stImageInfo.m_fPixelYSpacing = _ttof((LPTSTR)(LPCTSTR)GetValue(TAG_PIXEL_SPACING));
	if (m_DicomImage.m_stImageInfo.m_fPixelYSpacing < 0.0f)
	{
		m_DicomImage.m_stImageInfo.m_fPixelYSpacing = 0.0f;
	}

	m_DicomImage.m_stImageInfo.m_fImagerXSpacing = _ttof((LPTSTR)(LPCTSTR)GetValue(TAG_IMAGER_PIXEL_SPACING));
	if (m_DicomImage.m_stImageInfo.m_fImagerXSpacing < 0.0f)
	{
		m_DicomImage.m_stImageInfo.m_fImagerXSpacing = 0.0f;
	}

	m_DicomImage.m_stImageInfo.m_fImagerYSpacing = _ttof((LPTSTR)(LPCTSTR)GetValue(TAG_IMAGER_PIXEL_SPACING));
	if (m_DicomImage.m_stImageInfo.m_fImagerYSpacing < 0.0f)
	{
		m_DicomImage.m_stImageInfo.m_fImagerYSpacing = 0.0f;
	}

	m_DicomImage.m_stImageInfo.m_fMagFactor = _ttof((LPTSTR)(LPCTSTR)GetValue(TAG_ESTIMATED_RADIOGRAPHIC_MAGNIFICATION_FACTOR));
	//

	pElement = FindLastElement(NULL, TAG_PIXEL_DATA, FALSE);
	m_DicomImage.m_stImageInfo.m_nFrameCount = GetCountImage(pElement);

	double dFrameTime = _ttof((LPTSTR)(LPCTSTR)GetValue(TAG_FRAME_TIME));
	if (dFrameTime)
	{
		m_DicomImage.m_stImageInfo.m_nFramePerSecond = (UINT)(((double)1000 / dFrameTime) + 0.05);
	}
	
	if (GetInfoImage(pElement, &dicomImage, 0) == DICOM_SUCCESS)
	{
		//int nWidth, int nHeight, int nBitsPerPixel, int nSamplesPerPixel
		//dicomImage.nColumns, dicomImage.nRows, dicomImage.nHighBit + 1, dicomImage.nSamplesPerPixel

		INT_PTR nWidth = dicomImage.nColumns;
		INT_PTR nHeight = dicomImage.nRows;
		INT_PTR nBitsPerPixel = dicomImage.nHighBit + 1;
		INT_PTR nSamplesPerPixel = dicomImage.nSamplesPerPixel;

		if (nWidth <= 0)
		{
			AfxDebugBreak();
			return FALSE;
		}

		//
		if (nHeight <= 0)
		{
			AfxDebugBreak();
			return FALSE;
		}

		//
		if (nBitsPerPixel <= 0)
		{
			AfxDebugBreak();
			return FALSE;
		}

		m_DicomImage.m_stImageInfo.m_nBitsPerPixel = nBitsPerPixel;
		if (m_DicomImage.m_stImageInfo.m_nBitsPerPixel <= 0)
		{
			if (m_DicomImage.m_stImageInfo.m_nBitsPerPixel == 24)
			{
				m_DicomImage.m_stImageInfo.m_nSamplesPerPixel = 3;
			}
			else if (m_DicomImage.m_stImageInfo.m_nBitsPerPixel > 0 && m_DicomImage.m_stImageInfo.m_nBitsPerPixel < 24)
			{
				m_DicomImage.m_stImageInfo.m_nSamplesPerPixel = 1;
			}
		}
		else
		{
			m_DicomImage.m_stImageInfo.m_nSamplesPerPixel = nSamplesPerPixel;
		}


		UINT nFlags = DICOM_GETIMAGE_AUTO_APPLY_MODALITY_LUT | DICOM_GETIMAGE_AUTO_APPLY_VOI_LUT;

		L_UINT16 nResult = GetImage(pElement,
			&BitmapHandle,
			sizeof(BITMAPHANDLE),
			0,
			0,
			ORDER_RGBORGRAY,
			nFlags,
			NULL,
			NULL);

		if (nResult == DICOM_ERROR_MEMORY)
		{
			AfxThrowMemoryException();
		}
		else if (nResult != DICOM_SUCCESS)
		{
			Sleep(0);
			nResult = GetImage(pElement,
				&BitmapHandle,
				sizeof(BITMAPHANDLE),
				0,
				0,
				ORDER_RGBORGRAY,
				nFlags,
				NULL,
				NULL);

			if (nResult != DICOM_SUCCESS)
			{
				Sleep(0);
				nResult = GetImage(pElement,
					&BitmapHandle,
					sizeof(BITMAPHANDLE),
					0,
					0,
					ORDER_RGBORGRAY,
					nFlags,
					NULL,
					NULL);
			}

			if (nResult != DICOM_SUCCESS)
			{
				AfxThrowUserException();
			}

			if (BitmapHandle.Flags.Allocated != 1)
			{
				AfxThrowUserException();
			}

			if (BitmapHandle.Flags.Compressed == 1)
			{
				AfxThrowUserException();
			}

			// Not linear image data.
			if (BitmapHandle.Flags.Tiled == 1)
			{
				m_DicomImage.SetTiledPiexlData(TRUE);
			}

			if (BitmapHandle.Flags.SuperCompressed == 1)
			{
				AfxThrowUserException();
			}

			if (BitmapHandle.Flags.UseLUT == 1)
			{
				if (!BitmapHandle.pLUT)
				{
					AfxThrowUserException();
				}

				if (BitmapHandle.LUTLength < 1)
				{
					AfxThrowUserException();
				}
			}

			if (!BitmapHandle.Addr.Windows.pData)
			{
				AfxThrowUserException();
			}

			if (BitmapHandle.Width < 2)
			{
				AfxThrowUserException();
			}

			if (BitmapHandle.Height < 2)
			{
				AfxThrowUserException();
			}

			if (BitmapHandle.BitsPerPixel < 1)
			{
				AfxThrowUserException();
			}

			if (BitmapHandle.BytesPerLine < 4)
			{
				AfxThrowUserException();
			}

			m_DicomImage.FreeDicomImage();
			m_DicomImage.m_stImageInfo.Init();

			if (BitmapHandle.HighBit < 1)
			{
				AfxThrowUserException();
			}
			else
			{
				m_DicomImage.m_stImageInfo.m_nBitsPerPixel = (UINT)(BitmapHandle.HighBit + 1);
			}

			// RGB : SamplesPerPixel = 3
			if (BitmapHandle.BitsPerPixel == 24)
			{
				m_DicomImage.m_stImageInfo.m_nSamplesPerPixel = 3;
			}
			// Gray : SamplesPerPixel = 1 (default)
			else if (BitmapHandle.BitsPerPixel > 0 && BitmapHandle.BitsPerPixel < 24)
			{
				m_DicomImage.m_stImageInfo.m_nSamplesPerPixel = 1;
			}
			else
			{
				AfxThrowUserException();
			}
		}

		m_DicomImage.m_stImageInfo.m_nBytesPerPixel = (UINT)Bits2Bytes(m_DicomImage.m_stImageInfo.m_nBitsPerPixel);
		m_DicomImage.m_stImageInfo.m_nTotalAllocatedBytes = (UINT)(m_DicomImage.m_stImageInfo.m_nBytesPerPixel*m_DicomImage.m_stImageInfo.m_nSamplesPerPixel);
		m_DicomImage.m_stImageInfo.m_nWidth = nWidth;
		m_DicomImage.m_stImageInfo.m_nHeight = nHeight;
		int nBytesPerLineX = ((m_DicomImage.m_stImageInfo.m_nWidth * m_DicomImage.m_stImageInfo.m_nTotalAllocatedBytes) / 4) * 4;
		int nBytesPerLineY = ((m_DicomImage.m_stImageInfo.m_nHeight * m_DicomImage.m_stImageInfo.m_nTotalAllocatedBytes) / 4) * 4;
		m_DicomImage.m_stImageInfo.m_nWidth = nBytesPerLineX / m_DicomImage.m_stImageInfo.m_nTotalAllocatedBytes;	// rounded by four
		m_DicomImage.m_stImageInfo.m_nHeight = nBytesPerLineY / m_DicomImage.m_stImageInfo.m_nTotalAllocatedBytes;	// rounded by four
		m_DicomImage.m_stImageInfo.m_nBytesPerLine = m_DicomImage.m_stImageInfo.m_nWidth * m_DicomImage.m_stImageInfo.m_nTotalAllocatedBytes;

		m_DicomImage.m_stImageInfo.m_fW1 = 0.0f;
		m_DicomImage.m_stImageInfo.m_fW2 = Bits2MaxValue(m_DicomImage.m_stImageInfo.m_nBitsPerPixel);
		//
		// Get W/L
		L_DOUBLE dWindowCenter = 0;
		L_DOUBLE dWindowWidth = 0;
		// Window Center
		pElement = FindFirstElement(NULL, TAG_WINDOW_CENTER, FALSE);
		if (pElement && pElement->nLength)
		{
			dWindowCenter = *GetDoubleValue(pElement, 0, 1);
			m_DicomImage.m_stImageInfo.m_nW1 = (int)dWindowCenter;
		}
		else
		{
			m_DicomImage.m_stImageInfo.m_nW1 = (int)m_DicomImage.m_stImageInfo.m_fW1;
		}

		// Window Width
		pElement = FindFirstElement(NULL, TAG_WINDOW_WIDTH, FALSE);
		if (pElement && pElement->nLength)
		{
			dWindowWidth = *GetDoubleValue(pElement, 0, 1);
			m_DicomImage.m_stImageInfo.m_nW2 = (int)dWindowWidth;
		}
		else
		{
			m_DicomImage.m_stImageInfo.m_nW2 = (int)m_DicomImage.m_stImageInfo.m_fW2;
		}
		//

		m_DicomImage.LoadDicomImage(&BitmapHandle);
	}
	else
	{
		L_FreeBitmap(&BitmapHandle);
		return FALSE;
	}
	L_FreeBitmap(&BitmapHandle);
	return TRUE;
}

UINT CDicomParser::MoveRootElement()
{
	pDICOMELEMENT pElement = LDicomDS::GetFirstElement(NULL, FALSE, FALSE);
	SetCurrentElement(pElement);
	return DICOM_SUCCESS;
}

VOID CDicomParser::SetFileName(CString strFileName)
{
	if (strFileName.IsEmpty())
	{
		m_dcmHeaderInfo.m_strFileName = _T("");
		return;
	}

	m_dcmHeaderInfo.m_strFileName = strFileName;
	return;
}

VOID CDicomParser::SetCurrentElement(pDICOMELEMENT pCurrentElement)
{
	UINT ndcmRet = DICOM_SUCCESS;
	m_pCurrentElement = pCurrentElement;
	if (m_pCurrentElement)
	{
		m_pParentElement = LDicomDS::GetParentElement(m_pCurrentElement);
	}
}

L_VOID CDicomParser::ResetDS()
{
	LDicomDS::ResetDS();
}

int CDicomParser::ParseNLS()
{
	int nNLS = 0;
	pDICOMELEMENT pElement = LDicomDS::FindFirstElement(NULL, TAG_SPECIFIC_CHARACTER_SET, FALSE);
	if (!pElement)
	{
		nNLS = DCM_NLS_DEFAULT;
		return nNLS;
	}

	CString strNLS;
	CStringArray aryNLS;
	L_UINT32 nCount = LDicomDS::GetCountValue(pElement);
	for (L_UINT32 nIndex = 0; nIndex < nCount; nIndex++)
	{
		strNLS += LDicomDS::GetStringValue(pElement, nIndex, 1);
		if (nIndex + 1 < nCount)
		{
			strNLS += _T("\\");
		}

		aryNLS.Add(strNLS);
	}

	for (INT_PTR nIndex = 0; nIndex < aryNLS.GetCount(); nIndex++)
	{
		CString strNLS = aryNLS.GetAt(nIndex);
		strNLS = strNLS.MakeUpper();

		if (strNLS.Find(_T("ISO 2022 IR 100")) != -1 || strNLS.Find(_T("ISO_IR 100")) != -1)
		{
			nNLS |= DCM_NLS_ISO_IR_100;
		}
		else if (strNLS.Find(_T("ISO 2022 IR 101")) != -1 || strNLS.Find(_T("ISO_IR 101")) != -1)
		{
			nNLS |= DCM_NLS_ISO_IR_101;
		}
		else if (strNLS.Find(_T("ISO 2022 IR 109")) != -1 || strNLS.Find(_T("ISO_IR 109")) != -1)
		{
			nNLS |= DCM_NLS_ISO_IR_109;
		}
		else if (strNLS.Find(_T("ISO 2022 IR 110")) != -1 || strNLS.Find(_T("ISO_IR 110")) != -1)
		{
			nNLS |= DCM_NLS_ISO_IR_110;
		}
		else if (strNLS.Find(_T("ISO 2022 IR 144")) != -1 || strNLS.Find(_T("ISO_IR 144")) != -1)
		{
			nNLS |= DCM_NLS_ISO_IR_144;
		}
		else if (strNLS.Find(_T("ISO 2022 IR 127")) != -1 || strNLS.Find(_T("ISO_IR 127")) != -1)
		{
			nNLS |= DCM_NLS_ISO_IR_127;
		}
		else if (strNLS.Find(_T("ISO 2022 IR 126")) != -1 || strNLS.Find(_T("ISO_IR 126")) != -1)
		{
			nNLS |= DCM_NLS_ISO_IR_126;
		}
		else if (strNLS.Find(_T("ISO 2022 IR 138")) != -1 || strNLS.Find(_T("ISO_IR 138")) != -1)
		{
			nNLS |= DCM_NLS_ISO_IR_138;
		}
		else if (strNLS.Find(_T("ISO 2022 IR 148")) != -1 || strNLS.Find(_T("ISO_IR 148")) != -1)
		{
			nNLS |= DCM_NLS_ISO_IR_148;
		}
		else if (strNLS.Find(_T("ISO 2022 IR 13")) != -1 || strNLS.Find(_T("ISO_IR 13")) != -1)
		{
			nNLS |= DCM_NLS_ISO_IR_13;
		}
		else if (strNLS.Find(_T("ISO 2022 IR 166")) != -1 || strNLS.Find(_T("ISO_IR 166")) != -1)
		{
			nNLS |= DCM_NLS_ISO_IR_166;
		}
		else if (strNLS.Find(_T("ISO 2022 IR 14")) != -1 || strNLS.Find(_T("ISO_IR 14")) != -1)
		{
			nNLS |= DCM_NLS_ISO_IR_14;
		}
		else if (strNLS.Find(_T("ISO 2022 IR 87")) != -1 || strNLS.Find(_T("ISO_IR 87")) != -1)
		{
			nNLS |= DCM_NLS_ISO_IR_87;
		}
		else if (strNLS.Find(_T("ISO 2022 IR 159")) != -1 || strNLS.Find(_T("ISO_IR 159")) != -1)
		{
			nNLS |= DCM_NLS_ISO_IR_159;
		}
		else if (strNLS.Find(_T("ISO 2022 IR 149")) != -1 || strNLS.Find(_T("ISO_IR 149")) != -1)
		{
			nNLS |= DCM_NLS_ISO_IR_149;
		}
		else if (strNLS.Find(_T("ISO 2022 IR 192")) != -1 || strNLS.Find(_T("ISO_IR 192")) != -1)
		{
			nNLS |= DCM_NLS_ISO_IR_192;
		}
		else if (strNLS.Find(_T("GB18030")) != -1)
		{
			nNLS |= DCM_NLS_GB18030;
		}
		else if (strNLS.Find(_T("ISO 2022 IR 58")) != -1 || strNLS.Find(_T("ISO_IR 58")) != -1)
		{
			nNLS |= DCM_NLS_ISO_IR_58;
		}
	}

	if (!nNLS)
	{
		nNLS = DCM_NLS_DEFAULT;
	}

	return nNLS;
}

int& CDicomParser::GetCurNLS()
{
	if (m_nCurNLS == -1)
	{
		m_nCurNLS = ParseNLS();
	}

	return m_nCurNLS;
}

CLLDicomDS CDicomParser::GetLLDicomDS()
{
	CLLDicomDS dsLLDicomDS;

	dsLLDicomDS.m_dcmHeaderInfo = m_dcmHeaderInfo;
	dsLLDicomDS.m_DicomImage = m_DicomImage;

	return dsLLDicomDS;
}

CDicomParser& CDicomParser::operator=(const CDicomParser& obj)
{
	m_dcmHeaderInfo = obj.m_dcmHeaderInfo;
	m_DicomImage = obj.m_DicomImage;

	return *this;
}

FLOAT CDicomParser::GetOrientationOrthogonalX()
{
	FLOAT fRowY = m_DicomImage.m_stImageInfo.m_fImageOrientationRowY;
	FLOAT fRowZ = m_DicomImage.m_stImageInfo.m_fImageOrientationRowZ;
	FLOAT fColY = m_DicomImage.m_stImageInfo.m_fImageOrientationColY;
	FLOAT fColZ = m_DicomImage.m_stImageInfo.m_fImageOrientationColZ;

	return (fRowY * fColZ) - (fRowZ * fColY);
}

FLOAT CDicomParser::GetOrientationOrthogonalY()
{
	FLOAT fRowX = m_DicomImage.m_stImageInfo.m_fImageOrientationRowX;
	FLOAT fRowZ = m_DicomImage.m_stImageInfo.m_fImageOrientationRowZ;
	FLOAT fColX = m_DicomImage.m_stImageInfo.m_fImageOrientationColX;
	FLOAT fColZ = m_DicomImage.m_stImageInfo.m_fImageOrientationColZ;

	return (fRowX * fColZ) - (fRowZ * fColZ);
}

FLOAT CDicomParser::GetOrientationOrthogonalZ()
{
	FLOAT fRowX = m_DicomImage.m_stImageInfo.m_fImageOrientationRowX;
	FLOAT fRowY = m_DicomImage.m_stImageInfo.m_fImageOrientationRowY;
	FLOAT fColX = m_DicomImage.m_stImageInfo.m_fImageOrientationColX;
	FLOAT fColY = m_DicomImage.m_stImageInfo.m_fImageOrientationColY;

	return (fRowX * fColX) - (fRowY * fColY);
}