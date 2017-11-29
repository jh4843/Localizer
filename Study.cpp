#include "stdafx.h"
#include "LLDicomDS.h"
#include "Study.h"

CStudy::CStudy(CLLDicomDS dsDicomDS)
{
	m_dsDicomDS = dsDicomDS;
}

CStudy::CStudy()
{
}

CStudy::~CStudy()
{
	for(INT_PTR iArr = 0; iArr < m_arrSeries.GetCount(); iArr++)
	{
		CSeries* pSeries = m_arrSeries.GetAt(iArr);
		if (pSeries)
		{
			delete pSeries;
			pSeries = NULL;
		}
	}
	m_arrSeries.RemoveAll();
}

BOOL CStudy::IsSameStudyID(CString strStudyId)
{
	if (m_dsDicomDS.m_dcmHeaderInfo.m_strStudyID.IsEmpty())
	{
		return FALSE;
	}
	else if (!strStudyId.CompareNoCase(m_dsDicomDS.m_dcmHeaderInfo.m_strStudyInstanceUID))
	{
		return TRUE;
	}

	return FALSE;
}

void CStudy::AddSeries(CLLDicomDS dsLLDicomDS)
{
	CSeries* pSeries = new CSeries(dsLLDicomDS);
	BOOL bFindSeries = FALSE;

	for (INT_PTR iArray = 0; iArray < m_arrSeries.GetCount(); iArray++)
	{
		CSeries* pSeriesInfo = m_arrSeries.GetAt(iArray);
		if (pSeriesInfo->IsSameSeriesID(dsLLDicomDS.m_dcmHeaderInfo.m_strSeriesInstanceUID) == TRUE)
		{
			pSeries = pSeriesInfo;
			bFindSeries = TRUE;
			break;
		}
	}

	pSeries->AddInstance(dsLLDicomDS);

	if (bFindSeries == FALSE)
	{
		m_arrSeries.Add(pSeries);
	}

	return;
}

void CStudy::AddStudyViewerIndex(INT_PTR nStudyViewerIndex)
{
	for (INT_PTR i = 0; i < m_arrIndexDisplayStudyViewer.GetCount(); i++)
	{
		if (nStudyViewerIndex == m_arrIndexDisplayStudyViewer.GetAt(i))
		{
			return;
		}
	}
	
	m_arrIndexDisplayStudyViewer.Add(nStudyViewerIndex);
	return;
}

void CStudy::UpdateDicomDs(CLLDicomDS dicomParser)
{
	m_dsDicomDS = dicomParser;
}

CLLDicomDS CStudy::GetLLDicomDS()
{
	return m_dsDicomDS;
}

CString CStudy::GetStudyId()
{
	return m_dsDicomDS.m_dcmHeaderInfo.m_strStudyInstanceUID;
}

CArray<CSeries*, CSeries*>* CStudy::GetSeriesArray()
{
	return &m_arrSeries;
}

CStudy& CStudy::operator=(const CStudy& obj)
{
	m_dsDicomDS = obj.m_dsDicomDS;
	m_arrSeries.Copy(obj.m_arrSeries);

	return *this;
}