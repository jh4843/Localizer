#include "stdafx.h"
#include "LLDicomDS.h"


CLLDicomDS::CLLDicomDS()
{
	Init();
}


CLLDicomDS::~CLLDicomDS()
{
}

void CLLDicomDS::Init()
{
	m_dcmHeaderInfo.Init();
	m_DicomImage.Init();
}

CString CLLDicomDS::GetStudyID()
{
	return m_dcmHeaderInfo.m_strStudyInstanceUID;
}

CString CLLDicomDS::GetSeriesID()
{
	return m_dcmHeaderInfo.m_strSeriesInstanceUID;
}

CString CLLDicomDS::GetInstanceID()
{
	return m_dcmHeaderInfo.m_strSOPInstanceUID;
}

CString CLLDicomDS::GetPatientOrientationUp()
{
	CString strRes = _T("[");

	if (IsValidPatientOrientation() == FALSE)
	{
		strRes = _T("");
		return strRes;
	}

	//
	if (m_DicomImage.m_stImageInfo.m_fImageOrientationColX > 0)
	{
		strRes += _T("R");
	}
	else if (m_DicomImage.m_stImageInfo.m_fImageOrientationColX < 0)
	{
		strRes += _T("L");
	}

	//
	if (m_DicomImage.m_stImageInfo.m_fImageOrientationColY > 0)
	{
		strRes += _T("A");
	}
	else if (m_DicomImage.m_stImageInfo.m_fImageOrientationColY < 0)
	{
		strRes += _T("P");
	}

	//
	if (m_DicomImage.m_stImageInfo.m_fImageOrientationColZ > 0)
	{
		strRes += _T("F");
	}
	else if (m_DicomImage.m_stImageInfo.m_fImageOrientationColZ < 0)
	{
		strRes += _T("H");
	}

	strRes += _T("]");
	return strRes;
}

CString CLLDicomDS::GetPatientOrientationDown()
{
	CString strRes = _T("[");

	if (IsValidPatientOrientation() == FALSE)
	{
		strRes = _T("");
		return strRes;
	}

	//
	if (m_DicomImage.m_stImageInfo.m_fImageOrientationColX > 0)
	{
		strRes += _T("L");
	}
	else if (m_DicomImage.m_stImageInfo.m_fImageOrientationColX < 0)
	{
		strRes += _T("R");
	}

	//
	if (m_DicomImage.m_stImageInfo.m_fImageOrientationColY > 0)
	{
		strRes += _T("P");
	}
	else if (m_DicomImage.m_stImageInfo.m_fImageOrientationColY < 0)
	{
		strRes += _T("A");
	}

	//
	if (m_DicomImage.m_stImageInfo.m_fImageOrientationColZ > 0)
	{
		strRes += _T("H");
	}
	else if (m_DicomImage.m_stImageInfo.m_fImageOrientationColZ < 0)
	{
		strRes += _T("F");
	}

	strRes += _T("]");
	return strRes;
}

CString CLLDicomDS::GetPatientOrientationLeft()
{
	CString strRes = _T("[");

	if (IsValidPatientOrientation() == FALSE)
	{
		strRes = _T("");
		return strRes;
	}

	//
	if (m_DicomImage.m_stImageInfo.m_fImageOrientationRowX > 0)
	{
		strRes += _T("R");
	}
	else if (m_DicomImage.m_stImageInfo.m_fImageOrientationRowX < 0)
	{
		strRes += _T("L");
	}

	//
	if (m_DicomImage.m_stImageInfo.m_fImageOrientationRowY > 0)
	{
		strRes += _T("A");
	}
	else if (m_DicomImage.m_stImageInfo.m_fImageOrientationRowY < 0)
	{
		strRes += _T("P");
	}

	//
	if (m_DicomImage.m_stImageInfo.m_fImageOrientationRowZ > 0)
	{
		strRes += _T("F");
	}
	else if (m_DicomImage.m_stImageInfo.m_fImageOrientationRowZ < 0)
	{
		strRes += _T("H");
	}

	strRes += _T("]");
	return strRes;
}

CString CLLDicomDS::GetPatientOrientationRight()
{
	CString strRes = _T("[");

	if (IsValidPatientOrientation() == FALSE)
	{
		strRes = _T("");
		return strRes;
	}

	//
	if (m_DicomImage.m_stImageInfo.m_fImageOrientationRowX > 0)
	{
		strRes += _T("L");
	}
	else if (m_DicomImage.m_stImageInfo.m_fImageOrientationRowX < 0)
	{
		strRes += _T("R");
	}

	//
	if (m_DicomImage.m_stImageInfo.m_fImageOrientationRowY > 0)
	{
		strRes += _T("P");
	}
	else if (m_DicomImage.m_stImageInfo.m_fImageOrientationRowY < 0)
	{
		strRes += _T("A");
	}

	//
	if (m_DicomImage.m_stImageInfo.m_fImageOrientationRowZ > 0)
	{
		strRes += _T("H");
	}
	else if (m_DicomImage.m_stImageInfo.m_fImageOrientationRowZ < 0)
	{
		strRes += _T("F");
	}

	strRes += _T("]");
	return strRes;
}

BOOL CLLDicomDS::IsSamePosAndOrient(CLLDicomDS dicomDS)
{
	if (m_DicomImage.m_stImageInfo.m_fImagePosition_X == dicomDS.m_DicomImage.m_stImageInfo.m_fImagePosition_X &&
		m_DicomImage.m_stImageInfo.m_fImagePosition_Y == dicomDS.m_DicomImage.m_stImageInfo.m_fImagePosition_Y &&
		m_DicomImage.m_stImageInfo.m_fImagePosition_Z == dicomDS.m_DicomImage.m_stImageInfo.m_fImagePosition_Z &&
		m_DicomImage.m_stImageInfo.m_fImageOrientationRowX == dicomDS.m_DicomImage.m_stImageInfo.m_fImageOrientationRowX &&
		m_DicomImage.m_stImageInfo.m_fImageOrientationRowY == dicomDS.m_DicomImage.m_stImageInfo.m_fImageOrientationRowY &&
		m_DicomImage.m_stImageInfo.m_fImageOrientationRowZ == dicomDS.m_DicomImage.m_stImageInfo.m_fImageOrientationRowZ &&
		m_DicomImage.m_stImageInfo.m_fImageOrientationColX == dicomDS.m_DicomImage.m_stImageInfo.m_fImageOrientationColX &&
		m_DicomImage.m_stImageInfo.m_fImageOrientationColY == dicomDS.m_DicomImage.m_stImageInfo.m_fImageOrientationColY &&
		m_DicomImage.m_stImageInfo.m_fImageOrientationColZ == dicomDS.m_DicomImage.m_stImageInfo.m_fImageOrientationColZ
		)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CLLDicomDS::IsValidPatientOrientation()
{
	if (m_DicomImage.m_stImageInfo.m_fImageOrientationRowX == 0.0 &&
		m_DicomImage.m_stImageInfo.m_fImageOrientationRowY == 0.0 &&
		m_DicomImage.m_stImageInfo.m_fImageOrientationRowZ == 0.0 &&
		m_DicomImage.m_stImageInfo.m_fImageOrientationColX == 0.0 &&
		m_DicomImage.m_stImageInfo.m_fImageOrientationColY == 0.0 &&
		m_DicomImage.m_stImageInfo.m_fImageOrientationColZ == 0.0)
	{
		return FALSE;
	}

	return TRUE;
}