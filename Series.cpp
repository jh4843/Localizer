#include "stdafx.h"
#include "Series.h"


CSeries::CSeries(CLLDicomDS dsDicomDS)
{
	m_dsDicomDS = dsDicomDS;
}


CSeries::~CSeries()
{
	for (INT_PTR iArr = 0; iArr < m_arrInstance.GetCount(); iArr++)
	{
		CInstance* pSeries = m_arrInstance.GetAt(iArr);
		if (pSeries)
		{
			delete pSeries;
			pSeries = NULL;
		}
	}
	m_arrInstance.RemoveAll();
}

BOOL CSeries::IsSameSeriesID(CString strSeriesId)
{
	if (m_dsDicomDS.m_dcmHeaderInfo.m_strStudyID.IsEmpty())
	{
		return FALSE;
	}
	else if (!strSeriesId.CompareNoCase(m_dsDicomDS.m_dcmHeaderInfo.m_strSeriesInstanceUID))
	{
		return TRUE;
	}

	return FALSE;
}

void CSeries::AddInstance(CLLDicomDS dsLLDicomDS)
{
	CInstance* pInstance = new CInstance(dsLLDicomDS);
	BOOL bFindSeries = FALSE;

	for (INT_PTR iArray = 0; iArray < m_arrInstance.GetCount(); iArray++)
	{
		CInstance* pInstanceInfo = m_arrInstance.GetAt(iArray);
		if (pInstanceInfo->IsSameInstanceID(m_dsDicomDS.m_dcmHeaderInfo.m_strSeriesInstanceUID) == TRUE)
		{
			return;
		}
	}

	m_arrInstance.Add(pInstance);

	return;
}

void CSeries::UpdateDicomDs(CLLDicomDS dsLLDicom)
{
	m_dsDicomDS = dsLLDicom;
}

CArray<CInstance*, CInstance*>* CSeries::GetInstanceArray()
{
	return &m_arrInstance;
}

CSeries& CSeries::operator=(const CSeries& obj)
{
	m_dsDicomDS = obj.m_dsDicomDS;
	m_arrInstance.Copy(obj.m_arrInstance);

	return *this;
}