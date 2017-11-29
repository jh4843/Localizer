#include "stdafx.h"
#include "LocalizerCore.h"


CLocalizerCore::CLocalizerCore()
{
}


CLocalizerCore::~CLocalizerCore()
{
}

void CLocalizerCore::Init()
{
	m_nSelectedIndex = 0;
	m_aryStudyViewer.RemoveAll();
}

void CLocalizerCore::SetSelectedIndex(INT_PTR nSelectedIndex)
{
	if (nSelectedIndex < 0)
		m_nSelectedIndex = 0;

	m_nSelectedIndex = nSelectedIndex;
}

void CLocalizerCore::AddStudyViewer(CStudyViewer* pStudyViewer)
{
	m_aryStudyViewer.Add(pStudyViewer);
}

CLocalizerCore::LOCALIZER_IMAGE_INFO_ CLocalizerCore::GetLocalizerImageInfo(CLLDicomDS* dsSrcDicomDS)
{
	LOCALIZER_IMAGE_INFO_ stLocalizerImageInfo;

	// Row & Col
	stLocalizerImageInfo.nRow = dsSrcDicomDS->m_DicomImage.m_stImageInfo.m_nHeight;
	stLocalizerImageInfo.nCol = dsSrcDicomDS->m_DicomImage.m_stImageInfo.m_nWidth;

	FLOAT nImageXSpacing;
	FLOAT nImageYSpacing;
	if (dsSrcDicomDS->m_DicomImage.m_stImageInfo.m_fPixelXSpacing <= 0)
	{
		nImageXSpacing = 1;
	}
	else
	{
		nImageXSpacing = dsSrcDicomDS->m_DicomImage.m_stImageInfo.m_fPixelXSpacing;
	}


	if (dsSrcDicomDS->m_DicomImage.m_stImageInfo.m_fPixelYSpacing <= 0)
	{
		nImageYSpacing = 1;
	}
	else
	{
		nImageYSpacing = dsSrcDicomDS->m_DicomImage.m_stImageInfo.m_fPixelYSpacing;
	}


	stLocalizerImageInfo.nRowLength = dsSrcDicomDS->m_DicomImage.m_stImageInfo.m_nWidth * nImageXSpacing;
	stLocalizerImageInfo.nColLength = dsSrcDicomDS->m_DicomImage.m_stImageInfo.m_nHeight * nImageYSpacing;

	// Position
	stLocalizerImageInfo.fOriginX = dsSrcDicomDS->m_DicomImage.m_stImageInfo.m_fImagePosition_X;
	stLocalizerImageInfo.fOriginY = dsSrcDicomDS->m_DicomImage.m_stImageInfo.m_fImagePosition_Y;
	stLocalizerImageInfo.fOriginZ = dsSrcDicomDS->m_DicomImage.m_stImageInfo.m_fImagePosition_Z;

	// Orientation
	stLocalizerImageInfo.fRowOrientX = dsSrcDicomDS->m_DicomImage.m_stImageInfo.m_fImageOrientationRowX;
	stLocalizerImageInfo.fRowOrientY = dsSrcDicomDS->m_DicomImage.m_stImageInfo.m_fImageOrientationRowY;
	stLocalizerImageInfo.fRowOrientZ = dsSrcDicomDS->m_DicomImage.m_stImageInfo.m_fImageOrientationRowZ;

	stLocalizerImageInfo.fColOrientX = dsSrcDicomDS->m_DicomImage.m_stImageInfo.m_fImageOrientationColX;
	stLocalizerImageInfo.fColOrientY = dsSrcDicomDS->m_DicomImage.m_stImageInfo.m_fImageOrientationColY;
	stLocalizerImageInfo.fColOrientZ = dsSrcDicomDS->m_DicomImage.m_stImageInfo.m_fImageOrientationColZ;

	stLocalizerImageInfo.fOrthoOrientX = dsSrcDicomDS->m_DicomImage.m_stImageInfo.m_fImageOrientationOrthogonalX;
	stLocalizerImageInfo.fOrthoOrientY = dsSrcDicomDS->m_DicomImage.m_stImageInfo.m_fImageOrientationOrthogonalY;
	stLocalizerImageInfo.fOrthoOrientZ = dsSrcDicomDS->m_DicomImage.m_stImageInfo.m_fImageOrientationOrthogonalZ;

	stLocalizerImageInfo.m_fPixelXSpacing = dsSrcDicomDS->m_DicomImage.m_stImageInfo.m_fPixelXSpacing;
	stLocalizerImageInfo.m_fPixelYSpacing = dsSrcDicomDS->m_DicomImage.m_stImageInfo.m_fPixelYSpacing;
	stLocalizerImageInfo.m_fImagerXSpacing = dsSrcDicomDS->m_DicomImage.m_stImageInfo.m_fImagerXSpacing;
	stLocalizerImageInfo.m_fImagerYSpacing = dsSrcDicomDS->m_DicomImage.m_stImageInfo.m_fImagerYSpacing;

	stLocalizerImageInfo.fRectPosX[0] = stLocalizerImageInfo.fOriginX;
	stLocalizerImageInfo.fRectPosY[0] = stLocalizerImageInfo.fOriginY;
	stLocalizerImageInfo.fRectPosZ[0] = stLocalizerImageInfo.fOriginZ;

	// TRHC
	stLocalizerImageInfo.fRectPosX[1] = stLocalizerImageInfo.fOriginX + stLocalizerImageInfo.fRowOrientX * stLocalizerImageInfo.nRowLength;
	stLocalizerImageInfo.fRectPosY[1] = stLocalizerImageInfo.fOriginY + stLocalizerImageInfo.fRowOrientY * stLocalizerImageInfo.nRowLength;
	stLocalizerImageInfo.fRectPosZ[1] = stLocalizerImageInfo.fOriginZ + stLocalizerImageInfo.fRowOrientZ * stLocalizerImageInfo.nRowLength;

	// BRHC
	stLocalizerImageInfo.fRectPosX[2] = stLocalizerImageInfo.fOriginX + stLocalizerImageInfo.fRowOrientX * stLocalizerImageInfo.nRowLength + stLocalizerImageInfo.fColOrientX * stLocalizerImageInfo.nColLength;
	stLocalizerImageInfo.fRectPosY[2] = stLocalizerImageInfo.fOriginY + stLocalizerImageInfo.fRowOrientY * stLocalizerImageInfo.nRowLength + stLocalizerImageInfo.fColOrientY * stLocalizerImageInfo.nColLength;
	stLocalizerImageInfo.fRectPosZ[2] = stLocalizerImageInfo.fOriginZ + stLocalizerImageInfo.fRowOrientZ * stLocalizerImageInfo.nRowLength + stLocalizerImageInfo.fColOrientZ * stLocalizerImageInfo.nColLength;

	// BLHC
	stLocalizerImageInfo.fRectPosX[3] = stLocalizerImageInfo.fOriginX + stLocalizerImageInfo.fColOrientX * stLocalizerImageInfo.nColLength;
	stLocalizerImageInfo.fRectPosY[3] = stLocalizerImageInfo.fOriginY + stLocalizerImageInfo.fColOrientY * stLocalizerImageInfo.nColLength;
	stLocalizerImageInfo.fRectPosZ[3] = stLocalizerImageInfo.fOriginZ + stLocalizerImageInfo.fColOrientZ * stLocalizerImageInfo.nColLength;

	return stLocalizerImageInfo;
}

void CLocalizerCore::CalcReferenceLine()
{
	if (m_aryStudyViewer.GetCount() <= 0)
		return;

	CStudyViewer* pSrcStudyViewer = m_aryStudyViewer.GetAt(m_nSelectedIndex);

	if (!pSrcStudyViewer->GetStudy())
		return;

	CLLDicomDS* dsSrcDicomDS = pSrcStudyViewer->GetDisplayingDicomDS();
	LOCALIZER_IMAGE_INFO_ stSrcLocalizerImageInfo = GetLocalizerImageInfo(dsSrcDicomDS);

	CLLDicomDS* dsDestDicomDS = nullptr;
	LOCALIZER_IMAGE_INFO_ stDestLocalizerImageInfo;

	FLOAT fSrcXPosInDestImage_Pixel[4];	// [0] : TLHC(Top Left Hand Corner), [1]: TRHC(Top Right Hand), [2] : BRHC(Bottom Right Hand), [3] : BLHC(Bottom Left Hand)
	FLOAT fSrcYPosInDestImage_Pixel[4];	// [0] : TLHC(Top Left Hand Corner), [1]: TRHC(Top Right Hand), [2] : BRHC(Bottom Right Hand), [3] : BLHC(Bottom Left Hand)

	FLOAT fSrcPosX[4] = { 0.0, };
	FLOAT fSrcPosY[4] = { 0.0, };
	FLOAT fSrcPosZ[4] = { 0.0, };

	FLOAT fSrcPosXBasedOnDest;
	FLOAT fSrcPosYBasedOnDest;
	FLOAT fSrcPosZBasedOnDest;

	for (int iStudyViewer = 0; iStudyViewer < m_aryStudyViewer.GetCount(); iStudyViewer++)
	{
		if (iStudyViewer == m_nSelectedIndex)
			continue;

		CStudyViewer* pStudyViewer = m_aryStudyViewer.GetAt(iStudyViewer);

		if (!pStudyViewer)
			continue;

		if (!pStudyViewer->GetStudy())
			continue;

		if (pSrcStudyViewer->GetStudy()->IsSameStudyID(pStudyViewer->GetStudy()->GetStudyId()) == FALSE)
			continue;

		pStudyViewer->ClearLocalizerPoints();
		dsDestDicomDS = pStudyViewer->GetDisplayingDicomDS();
		stDestLocalizerImageInfo = GetLocalizerImageInfo(dsDestDicomDS);

		if (IsSamePatientOrientation(stSrcLocalizerImageInfo, stDestLocalizerImageInfo))
			continue;

		for (INT_PTR iInit = 0; iInit < 4; iInit++)
		{
			fSrcPosX[iInit] = stSrcLocalizerImageInfo.fRectPosX[iInit];
			fSrcPosY[iInit] = stSrcLocalizerImageInfo.fRectPosY[iInit];
			fSrcPosZ[iInit] = stSrcLocalizerImageInfo.fRectPosZ[iInit];
		}
				
		for (INT_PTR iRect = 0; iRect < 4; iRect++)
		{
			fSrcPosXBasedOnDest = 0.0;
			fSrcPosYBasedOnDest = 0.0;
			fSrcPosZBasedOnDest = 0.0;

			// Move points to calculate based on destination image information.
			fSrcPosX[iRect] -= stDestLocalizerImageInfo.fRectPosX[0];
			fSrcPosY[iRect] -= stDestLocalizerImageInfo.fRectPosY[0];
			fSrcPosZ[iRect] -= stDestLocalizerImageInfo.fRectPosZ[0];

			RotateBasedOnDestOrientation(stDestLocalizerImageInfo, fSrcPosX[iRect], fSrcPosY[iRect], fSrcPosZ[iRect],
				fSrcPosXBasedOnDest, fSrcPosYBasedOnDest, fSrcPosZBasedOnDest);

			fSrcXPosInDestImage_Pixel[iRect] = INT_PTR(fSrcPosXBasedOnDest / stDestLocalizerImageInfo.m_fPixelXSpacing + 0.5);
			fSrcYPosInDestImage_Pixel[iRect] = INT_PTR(fSrcPosYBasedOnDest / stDestLocalizerImageInfo.m_fPixelYSpacing + 0.5);

			pStudyViewer->AddLocalizerPoints(Gdiplus::PointF(fSrcXPosInDestImage_Pixel[iRect], fSrcYPosInDestImage_Pixel[iRect]));
		}
	}
}

void CLocalizerCore::RotateBasedOnDestOrientation(LOCALIZER_IMAGE_INFO_ stDestLocalizerImageInfo, 
	FLOAT fSrcPosX, FLOAT fSrcPosY, FLOAT fSrcPosZ, 
	FLOAT &fDestPosX, FLOAT &fDestPosY, FLOAT &fDestPosZ)
{
	fDestPosX = stDestLocalizerImageInfo.fRowOrientX * fSrcPosX
		+ stDestLocalizerImageInfo.fRowOrientY * fSrcPosY
		+ stDestLocalizerImageInfo.fRowOrientZ * fSrcPosZ;

	fDestPosY = stDestLocalizerImageInfo.fColOrientX * fSrcPosX
		+ stDestLocalizerImageInfo.fColOrientY * fSrcPosY
		+ stDestLocalizerImageInfo.fColOrientZ * fSrcPosZ;

	fDestPosZ = stDestLocalizerImageInfo.fOrthoOrientX * fSrcPosX
		+ stDestLocalizerImageInfo.fOrthoOrientY * fSrcPosY
		+ stDestLocalizerImageInfo.fOrthoOrientZ * fSrcPosZ;
}

BOOL CLocalizerCore::IsSamePatientOrientation(LOCALIZER_IMAGE_INFO_ stSrcLocalizerImageInfo, LOCALIZER_IMAGE_INFO_ stDestLocalizerImageInfo)
{
	if (stSrcLocalizerImageInfo.fRowOrientX != stDestLocalizerImageInfo.fRowOrientX)
		return FALSE;

	if (stSrcLocalizerImageInfo.fRowOrientY != stDestLocalizerImageInfo.fRowOrientY)
		return FALSE;

	if (stSrcLocalizerImageInfo.fRowOrientZ != stDestLocalizerImageInfo.fRowOrientZ)
		return FALSE;

	if (stSrcLocalizerImageInfo.fColOrientX != stDestLocalizerImageInfo.fColOrientX)
		return FALSE;

	if (stSrcLocalizerImageInfo.fColOrientY != stDestLocalizerImageInfo.fColOrientY)
		return FALSE;

	if (stSrcLocalizerImageInfo.fColOrientZ != stDestLocalizerImageInfo.fColOrientZ)
		return FALSE;

	return TRUE;
}