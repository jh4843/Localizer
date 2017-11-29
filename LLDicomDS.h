#pragma once
#include "DicomImage.h"

class CLLDicomDS
{
public:
	typedef struct _DICOM_HEADER_INFO_
	{
		// File
		CString m_strFileName;

		// Patient
		CString m_strPatientName;
		CString m_strPatientID;
		CString m_strBirthDate;
		CString m_strSex;
		CString m_strPatientComments;

		// Study
		CString m_strStudyID;
		CString m_strStudyInstanceUID;
		CString m_strStudyDescription;

		// Series
		CString m_strModality;
		LONG m_lSeriesNumber;
		CString m_strSeriesInstanceUID;

		// Instance
		LONG m_lInstanceNumber;
		CString m_strSOPInstanceUID;
		CString m_strSOPClassUID;
		CString m_strTransferSyntaxUID;

		void Init()
		{
			// File
			m_strFileName = _T("");

			// Patient
			m_strPatientName = _T("");
			m_strPatientID = _T("");
			m_strBirthDate = _T("");
			m_strSex = _T("");
			m_strPatientComments = _T("");

			// Study
			m_strStudyID = _T("");
			m_strStudyInstanceUID = _T("");
			m_strStudyDescription = _T("");

			// Series
			m_strModality = _T("");
			m_lSeriesNumber = 0;
			m_strSeriesInstanceUID = _T("");

			// Instance
			m_lInstanceNumber = 0;
			m_strSOPInstanceUID = _T("");
			m_strSOPClassUID = _T("");
			m_strTransferSyntaxUID = _T("");


		}

	}DICOM_HEADER_INFO_;

public:
	CLLDicomDS();
	~CLLDicomDS();

	void Init();
	CString GetStudyID();
	CString GetSeriesID();
	CString GetInstanceID();

	CString GetPatientOrientationUp();
	CString GetPatientOrientationDown();
	CString GetPatientOrientationLeft();
	CString GetPatientOrientationRight();

	BOOL IsSamePosAndOrient(CLLDicomDS dicomDS);

protected:
	BOOL IsValidPatientOrientation();
	
public:
	DICOM_HEADER_INFO_ m_dcmHeaderInfo;
	CDicomImage m_DicomImage;
};

