#include "stdafx.h"
#include "StudyViewer.h"
#include "CoordinatorUtill.h"

CStudyViewer::CStudyViewer(INT_PTR nLayoutIndex)
{
	m_nLayoutIndex = nLayoutIndex;

	m_pDragSource = nullptr;
	m_pDropTarget = nullptr;

	bOnlySeriesViewer = FALSE;
}


CStudyViewer::~CStudyViewer()
{
}

CStudyViewer& CStudyViewer::operator=(const CStudyViewer& obj)
{
	m_pParent = obj.m_pParent;
	m_pStudy = obj.m_pStudy;

	m_nCurSeriesIndex = obj.m_nCurSeriesIndex;
	m_nCurInstanceIndex = obj.m_nCurInstanceIndex;
	m_dsDisplayDicom = obj.m_dsDisplayDicom;

	return *this;
}

void CStudyViewer::SetStudy(CStudy* pStudy)
{
	if (!IsValidStudy(pStudy))
		return;

	m_pStudy = pStudy;

	SetDisplayInstance();

	if (m_pParent->IsKindOf(RUNTIME_CLASS(CLayoutManager)))
	{
		CLayoutManager* pLayoutManager = dynamic_cast<CLayoutManager*>(m_pParent);
		pLayoutManager->SetSelectedStudyViewer(m_nLayoutIndex);
	}
}

void CStudyViewer::SetDisplayInstance()
{
	CArray<CSeries*, CSeries*>* pArySeries;
	CArray<CInstance*, CInstance*>* pAryInstance;
	CInstance* pDisplayedInstance;

	pArySeries = m_pStudy->GetSeriesArray();
	pAryInstance = pArySeries->GetAt(m_nCurSeriesIndex)->GetInstanceArray();

	pDisplayedInstance = pAryInstance->GetAt(m_nCurInstanceIndex);

	m_dsDisplayDicom = pDisplayedInstance->GetDicomDS();

	CDicomImage imageDisplayInfo;	// different with DICOM information
	imageDisplayInfo = m_dsDisplayDicom.m_DicomImage;

	imageDisplayInfo.m_stImageInfo.m_nBitsPerPixel = 8;
	imageDisplayInfo.m_stImageInfo.m_nBytesPerPixel = (UINT)Bits2Bytes(imageDisplayInfo.m_stImageInfo.m_nBitsPerPixel);
	imageDisplayInfo.m_stImageInfo.m_nTotalAllocatedBytes = (UINT)(imageDisplayInfo.m_stImageInfo.m_nBytesPerPixel*imageDisplayInfo.m_stImageInfo.m_nSamplesPerPixel);
	imageDisplayInfo.m_stImageInfo.m_nBytesPerLine = imageDisplayInfo.BytesPerLine((UINT)(imageDisplayInfo.m_stImageInfo.m_nWidth*imageDisplayInfo.m_stImageInfo.m_nSamplesPerPixel), 8);

	imageDisplayInfo.m_stImageInfo.m_fW1 = 0.0f;
	imageDisplayInfo.m_stImageInfo.m_fW2 = 0.0f;

	FreeDisplayImage();
	AllocDisplayImage();

	m_dsDisplayDicom.m_DicomImage.GetImageProcessedImage(m_pDisplayImage, imageDisplayInfo);


}

void CStudyViewer::SetViewOnlySameSeries()
{
	bOnlySeriesViewer = TRUE;
}

void CStudyViewer::SetViewAllSeries()
{
	bOnlySeriesViewer = FALSE;
}

void CStudyViewer::SetCurrentInstanceIndex(INT_PTR nInstanceIndex)
{
	m_nCurInstanceIndex = nInstanceIndex;
	SetDisplayInstance();

}

void CStudyViewer::AddLocalizerPoints(Gdiplus::PointF ptLocalizerPointF)
{
	if (m_aryLocalizerPointF.GetCount() > MAX_LOCALIZER_POINT_COUNT)
		return;

	m_aryLocalizerPointF.Add(ptLocalizerPointF);
}

INT_PTR CStudyViewer::GetLayoutIndex()
{
	return m_nLayoutIndex;
}

CStudy* CStudyViewer::GetStudy()
{
	return m_pStudy;
}

INT_PTR CStudyViewer::GetCurrentInstanceIndex()
{
	return m_nCurInstanceIndex;
}

INT_PTR CStudyViewer::GetCurrentSeriesIndex()
{
	return m_nCurSeriesIndex;
}

CLLDicomDS* CStudyViewer::GetDisplayingDicomDS()
{
	return &m_dsDisplayDicom;
}

void CStudyViewer::RedrawWnd()
{
	Invalidate(FALSE);
	UpdateWindow();
}

void CStudyViewer::ClearLocalizerPoints()
{
	m_aryLocalizerPointF.RemoveAll();
	return;
}

INT_PTR CStudyViewer::GetInstanceCount()
{
	CArray<CSeries*, CSeries*>* pArySeries;
	CArray<CInstance*, CInstance*>* pAryInstance;

	pArySeries = m_pStudy->GetSeriesArray();

	pAryInstance = pArySeries->GetAt(m_nCurSeriesIndex)->GetInstanceArray();

	return pAryInstance->GetCount();
}

INT_PTR CStudyViewer::GetSeriesCount()
{
	CArray<CSeries*, CSeries*>* pArySeries;

	pArySeries = m_pStudy->GetSeriesArray();

	return pArySeries->GetCount();
}

void CStudyViewer::Init(INT_PTR nCurSeriesIndex, INT_PTR nCurInstanceIndex)
{
	m_pStudy = nullptr;

	m_pDisplayImage = nullptr;
	
	FreeDisplayImage();
	
	m_nCurSeriesIndex = nCurSeriesIndex;
	m_nCurInstanceIndex = nCurInstanceIndex;

	m_DibInfo.Init();
	m_dsDisplayDicom.m_DicomImage.FreeDicomImage();

	m_rtCanvas = CRect(0, 0, 0, 0);
	m_rtImage = CRect(0, 0, 0, 0);

	m_aryLocalizerPointF.RemoveAll();

	RedrawWnd();
}

BOOL CStudyViewer::CreateCtrl(CWnd* pParent)
{
	if (!CWnd::Create(NULL, _T("UIStudyViewer"), WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), pParent, ID_LAYOUT_VIEWER))
	{
		return FALSE;
	}

	m_pParent = pParent;

	Init();

	CreateDragDropObjects();

	return TRUE;
}

BOOL CStudyViewer::IsValidStudy(CStudy* pStudy)
{
	CArray<CSeries*, CSeries*>* pArrSeries = pStudy->GetSeriesArray();

	if (pArrSeries->GetCount() <= 0)
		return FALSE;

	CArray<CInstance*, CInstance*>* pArrInstance = pArrSeries->GetAt(0)->GetInstanceArray();

	if (pArrInstance->GetCount() <= 0)
		return FALSE;

	return TRUE;
}

BOOL CStudyViewer::DrawInstanceImage(CDC* pDC)
{
	if (!pDC)
	{
		return FALSE;
	}

	if (!m_pStudy)
	{
		return FALSE;
	}

	BITMAPINFOHEADER& bih = GetDibInfo()->bmiHeader;
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biWidth = m_dsDisplayDicom.m_DicomImage.m_stImageInfo.m_nWidth;
	bih.biHeight = -m_dsDisplayDicom.m_DicomImage.m_stImageInfo.m_nHeight;
	bih.biPlanes = 1;
	bih.biBitCount = 8 * m_dsDisplayDicom.m_DicomImage.m_stImageInfo.m_nSamplesPerPixel;
	bih.biCompression = BI_RGB;
	bih.biSizeImage = m_dsDisplayDicom.m_DicomImage.BytesPerLine((UINT)((double)m_dsDisplayDicom.m_DicomImage.m_stImageInfo.m_nWidth*(double)m_dsDisplayDicom.m_DicomImage.m_stImageInfo.m_nSamplesPerPixel), 8) * m_dsDisplayDicom.m_DicomImage.m_stImageInfo.m_nHeight;
	bih.biXPelsPerMeter = 0;
	bih.biYPelsPerMeter = 0;
	bih.biClrUsed = 0;
	bih.biClrImportant = 0;

// 안되는데 이유를 모르겠다. 나중에 다시 확인해보자.
	Graphics g(pDC->GetSafeHdc());

	// 나중에 시간날떄 한번씩 써보자. enum InterpolationMode
	g.SetInterpolationMode(InterpolationModeBilinear);

	Gdiplus::Bitmap bitmap((BITMAPINFO*)GetDibInfo(), m_pDisplayImage);

	m_rtImage.OffsetRect(0, 0);

	INT nSrcWidth = m_rtImage.Width();
	INT nSrcHeight = m_rtImage.Height();

	//
	g.DrawImage(&bitmap,
		Rect(m_rtCanvas.left, m_rtCanvas.top, m_rtCanvas.Width(), m_rtCanvas.Height()),
		m_rtImage.left,
		m_rtImage.top,
		nSrcWidth,
		nSrcHeight,
		UnitPixel);
	
	return TRUE;
}

BOOL CStudyViewer::DrawLocalizer(CDC* pDC)
{
	if (!pDC)
	{
		return FALSE;
	}

	if (!m_pStudy)
	{
		return FALSE;
	}

	if (m_aryLocalizerPointF.GetCount() < MAX_LOCALIZER_POINT_COUNT)
	{
		return FALSE;
	}

	CCoordinatorUtill util;

	Gdiplus::Graphics graphics(pDC->m_hDC);
	FLOAT fLineThick = 5; 
	Gdiplus::Pen penLine(Color(120, 240, 155), fLineThick);

	Gdiplus::PointF ptStart;
	Gdiplus::PointF ptEnd;

	INT_PTR iEnd = 0;

	for (INT_PTR iStart = 0; iStart < MAX_LOCALIZER_POINT_COUNT; iStart++)
	{
		iEnd = iStart + 1;
		if (iEnd == MAX_LOCALIZER_POINT_COUNT)
			iEnd = 0;

		ptStart = util.ConvertCanvas(m_aryLocalizerPointF.GetAt(iStart), m_rtCanvas, m_rtImage);
		ptEnd = util.ConvertCanvas(m_aryLocalizerPointF.GetAt(iEnd), m_rtCanvas, m_rtImage);

		graphics.DrawLine(&penLine, ptStart, ptEnd);
		//graphics.DrawLine(&penLine, m_aryLocalizerPointF.GetAt(iStart), m_aryLocalizerPointF.GetAt(iEnd));
	}

}

BOOL CStudyViewer::DrawImageInfo(CDC* pDC)
{
	if (!pDC)
	{
		return FALSE;
	}

	if (!m_pStudy)
	{
		return FALSE;
	}

	INT_PTR nMargin = 5;
	INT_PTR nSpace = 1;
	COLORREF crTextColor = RGB(255, 255, 255);
	COLORREF crShadowColor = RGB(0, 0, 0);

	INT_PTR nFontSize = 100;
	CFont font;
	font.CreatePointFont(nFontSize, L"Segoe UI");

	CFont* pOldFont = pDC->SelectObject(&font);

	INT_PTR nPosX = m_rtCanvas.left + nMargin;
	INT_PTR nPosY = m_rtCanvas.top + nMargin;

	CStringArray aryImageInfo;

	CString strStudyID = m_dsDisplayDicom.GetStudyID();
	aryImageInfo.Add(strStudyID);
	CString strSeriesID = m_dsDisplayDicom.GetSeriesID();
	aryImageInfo.Add(strSeriesID);
	CString strInstanceID = m_dsDisplayDicom.GetInstanceID();
	aryImageInfo.Add(strInstanceID);

	for (INT_PTR nIndex = 0; nIndex < aryImageInfo.GetCount(); nIndex++)
	{
		pDC->SetTextColor(crTextColor);
		pDC->TextOut(nPosX, nPosY, aryImageInfo.GetAt(nIndex));

		nPosY += 20 + nSpace;
	}

	pDC->SelectObject(pOldFont);
}

BOOL CStudyViewer::DrawPatientOrientation(CDC* pDC)
{
	if (!pDC)
	{
		return FALSE;
	}

	if (!m_pStudy)
	{
		return FALSE;
	}

	INT_PTR nMargin = 5;
	COLORREF crTextColor = RGB(255, 255, 255);
	COLORREF crShadowColor = RGB(0, 0, 0);

	INT_PTR nFontSize = 2;
	CFont font;
	font.CreatePointFont(nFontSize, L"Segoe UI");

	CFont* pOldFont = pDC->SelectObject(&font);

	CString strPatientOrientationUp = m_dsDisplayDicom.GetPatientOrientationUp();
	CString strPatientOrientationDown = m_dsDisplayDicom.GetPatientOrientationDown();
	CString strPatientOrientationLeft = m_dsDisplayDicom.GetPatientOrientationLeft();
	CString strPatientOrientationRight = m_dsDisplayDicom.GetPatientOrientationRight();

	CSize sizeMax = pDC->GetTextExtent(strPatientOrientationRight);

	CSize szText = CSize(5, 5);
	INT_PTR nLeftStartPosX = m_rtCanvas.left + nMargin;
	INT_PTR nRightStartPosX = m_rtCanvas.right - sizeMax.cx;
	INT_PTR nMiddleStartPosX = (INT_PTR)(m_rtCanvas.Width()/2) - (INT_PTR)(sizeMax.cx/2);

	INT_PTR nTopStartPosY = m_rtCanvas.top + nMargin;
	INT_PTR nBottomStartPosY = m_rtCanvas.bottom - sizeMax.cy;
	INT_PTR nMiddleStartPosY = (INT_PTR)(m_rtCanvas.Height() / 2) - (INT_PTR)(sizeMax.cy / 2);

	pDC->SetTextColor(crTextColor);
	pDC->TextOut(nMiddleStartPosX, nTopStartPosY, strPatientOrientationUp);

	pDC->SetTextColor(crTextColor);
	pDC->TextOut(nMiddleStartPosX, nBottomStartPosY, strPatientOrientationDown);

	pDC->SetTextColor(crTextColor);
	pDC->TextOut(nLeftStartPosX, nMiddleStartPosY, strPatientOrientationLeft);

	pDC->SetTextColor(crTextColor);
	pDC->TextOut(nRightStartPosX, nMiddleStartPosY, strPatientOrientationRight);

	pDC->SelectObject(pOldFont);

	return TRUE;

}

void CStudyViewer::ChangeInstanceIndex(BOOL bIsIncrease)
{
	INT_PTR nLastIndex = GetInstanceCount() - 1;

	if (bIsIncrease == TRUE)
	{
		if (m_nCurInstanceIndex >= nLastIndex)
		{
			if (bOnlySeriesViewer == FALSE)
			{
				ChangeSeriesIndex(TRUE);
			}
			else
			{
				m_nCurInstanceIndex = 0;
			}
		}
		else
		{
			m_nCurInstanceIndex++;
		}
	}
	else
	{
		if (m_nCurInstanceIndex <= 0)
		{
			if (bOnlySeriesViewer == FALSE)
			{
				ChangeSeriesIndex(FALSE);
			}
			else
			{
				m_nCurInstanceIndex = nLastIndex;
			}
		}
		else
		{
			m_nCurInstanceIndex--;
		}
	}
}

void CStudyViewer::ChangeSeriesIndex(BOOL bIsIncrease)
{
	INT_PTR nLastIndex = GetSeriesCount() - 1;

	if (bIsIncrease == TRUE)
	{
		if (m_nCurSeriesIndex >= nLastIndex)
		{
			m_nCurSeriesIndex = 0;
			m_nCurInstanceIndex = 0;
		}
		else
		{
			m_nCurSeriesIndex++;
			m_nCurInstanceIndex = 0;
		}
	}
	else
	{
		if (m_nCurSeriesIndex <= 0)
		{
			m_nCurSeriesIndex = nLastIndex;
			m_nCurInstanceIndex = 0;
		}
		else
		{
			m_nCurSeriesIndex--;
			m_nCurInstanceIndex = 0;
		}
	}
}

void CStudyViewer::ChangeInstanceImageByWheel(short zDelta)
{
	INT_PTR nLastIndex = GetInstanceCount() - 1;

	if (zDelta > 0)
	{
		ChangeInstanceIndex(FALSE);
	}
	else
	{
		ChangeInstanceIndex(TRUE);
	}

	SetDisplayInstance();

	if (m_pParent->IsKindOf(RUNTIME_CLASS(CLayoutManager)))
	{
		CLayoutManager* pLayoutManager = dynamic_cast<CLayoutManager*>(m_pParent);
		pLayoutManager->SetSelectedStudyViewer(m_nLayoutIndex);
	}

	RedrawWnd();

	return;
}

void CStudyViewer::ChangeSeriesImageByWheel(short zDelta)
{
	INT_PTR nLastIndex = GetSeriesCount() - 1;

	if (zDelta > 0)
	{
		ChangeSeriesIndex(FALSE);
	}
	else
	{
		ChangeSeriesIndex(TRUE);
	}

	SetDisplayInstance();

	if (m_pParent->IsKindOf(RUNTIME_CLASS(CLayoutManager)))
	{
		CLayoutManager* pLayoutManager = dynamic_cast<CLayoutManager*>(m_pParent);
		pLayoutManager->SetSelectedStudyViewer(m_nLayoutIndex);
	}

	RedrawWnd();

	return;
}



BOOL CStudyViewer::Draw(CDC* pDC)
{
	if (!pDC)
	{
		return FALSE;
	}

	CMemDC memDC(*pDC, this);
	pDC = &memDC.GetDC();

	CRect rtClient;
	GetClientRect(&rtClient);

	pDC->FillSolidRect(&rtClient, RGB(100, 100, 100));

	DrawInstanceImage(pDC);

	DrawPatientOrientation(pDC);

	DrawLocalizer(pDC);

	DrawImageInfo(pDC);

	//pDC->SetBkMode(TRANSPARENT);
	
	return TRUE;
}


BOOL CStudyViewer::CalcLayout()
{
	BOOL bRes = TRUE;

	GetClientRect(&m_rtCanvas);
	CalcImageRect(&m_dsDisplayDicom.m_DicomImage);

	return bRes;
}

INT_PTR CStudyViewer::CalcImageRect(CDicomImage* pImageInfo)
{
	INT_PTR nRetCode = 1;

	double dImgWidth = 0., dImgHeight = 0.;
	double dCanvasWidth = 0., dCanvasHeight = 0.;
	double dRatio = 0.;
	double dTemp = 0.;

	//
	dImgWidth = pImageInfo->m_stImageInfo.m_nWidth;
	dImgHeight = pImageInfo->m_stImageInfo.m_nHeight;
	dCanvasWidth = (double)m_rtCanvas.Width();
	dCanvasHeight = (double)m_rtCanvas.Height();

	if (dCanvasWidth >= dCanvasHeight)
	{
		dRatio = dCanvasWidth / dCanvasHeight;
		dTemp = dImgHeight * dRatio;

		if (dTemp >= dImgWidth)
		{
			m_rtImage.left = (int)(dImgWidth / 2) - (int)(dTemp / 2);
			m_rtImage.top = 0;
			m_rtImage.right = m_rtImage.left + (int)(dTemp + 0.5f);
			m_rtImage.bottom = (int)dImgHeight;

			CPoint ptCenter = CPoint((int)((m_rtImage.right + m_rtImage.left)*0.5f), (int)((m_rtImage.bottom + m_rtImage.top)*0.5f));
			int nConst = (int)(m_rtImage.Width()*0.5f);
			int nConst1 = (int)(m_rtImage.Width());
			m_rtImage.left = ptCenter.x - nConst;
			m_rtImage.right = m_rtImage.left + nConst1;
			nConst = (int)(m_rtImage.Height()*0.5f);
			nConst1 = (int)(m_rtImage.Height());
			m_rtImage.top = ptCenter.y - nConst;
			m_rtImage.bottom = m_rtImage.top + nConst1;
		}
		else
		{
			dRatio = dCanvasHeight / dCanvasWidth;
			dTemp = dImgWidth * dRatio;

			m_rtImage.left = 0;
			m_rtImage.top = (int)(dImgHeight / 2) - (int)(dTemp / 2);
			m_rtImage.right = (int)dImgWidth;
			m_rtImage.bottom = m_rtImage.top + (int)(dTemp + 0.5f);

			CPoint ptCenter = CPoint((int)((m_rtImage.right + m_rtImage.left)*0.5f), (int)((m_rtImage.bottom + m_rtImage.top)*0.5f));
			int nConst = (int)(m_rtImage.Width()*0.5f);
			int nConst1 = (int)(m_rtImage.Width());
			m_rtImage.left = ptCenter.x - nConst;
			m_rtImage.right = m_rtImage.left + nConst1;
			nConst = (int)(m_rtImage.Height()*0.5f);
			nConst1 = (int)(m_rtImage.Height());
			m_rtImage.top = ptCenter.y - nConst;
			m_rtImage.bottom = m_rtImage.top + nConst1;
		}
	}
	else
	{
		dRatio = dCanvasHeight / dCanvasWidth;
		dTemp = dImgWidth * dRatio;

		if (dTemp >= dImgHeight)
		{
			m_rtImage.left = 0;
			m_rtImage.top = (int)(dImgHeight / 2) - (int)(dTemp / 2);
			m_rtImage.right = (int)dImgWidth;
			m_rtImage.bottom = m_rtImage.top + (int)(dTemp + 0.5f);

			CPoint ptCenter = CPoint((int)((m_rtImage.right + m_rtImage.left)*0.5f), (int)((m_rtImage.bottom + m_rtImage.top)*0.5f));
			int nConst = (int)(m_rtImage.Width()*0.5f);
			int nConst1 = (int)(m_rtImage.Width());
			m_rtImage.left = ptCenter.x - nConst;
			m_rtImage.right = m_rtImage.left + nConst1;
			nConst = (int)(m_rtImage.Height()*0.5f);
			nConst1 = (int)(m_rtImage.Height());
			m_rtImage.top = ptCenter.y - nConst;
			m_rtImage.bottom = m_rtImage.top + nConst1;
		}
		else
		{
			dRatio = dCanvasWidth / dCanvasHeight;
			dTemp = dImgHeight * dRatio;

			m_rtImage.left = (int)(dImgWidth / 2) - (int)(dTemp / 2);
			m_rtImage.top = 0;
			m_rtImage.right = m_rtImage.left + (int)(dTemp + 0.5f);
			m_rtImage.bottom = (int)dImgHeight;

			CPoint ptCenter = CPoint((int)((m_rtImage.right + m_rtImage.left)*0.5f), (int)((m_rtImage.bottom + m_rtImage.top)*0.5f));
			int nConst = (int)(m_rtImage.Width()*0.5f);
			int nConst1 = (int)(m_rtImage.Width());
			m_rtImage.left = ptCenter.x - nConst;
			m_rtImage.right = m_rtImage.left + nConst1;
			nConst = (int)(m_rtImage.Height()*0.5f);
			nConst1 = (int)(m_rtImage.Height());
			m_rtImage.top = ptCenter.y - nConst;
			m_rtImage.bottom = m_rtImage.top + nConst1;
		}
	}

	return nRetCode;
}

DIBINFO* CStudyViewer::GetDibInfo()
{
	return &m_DibInfo;
}

BOOL CStudyViewer::AllocDisplayImage()
{
	BOOL bRes = TRUE;

	INT_PTR nImageSize = m_dsDisplayDicom.m_DicomImage.BytesPerLine((UINT)(m_dsDisplayDicom.m_DicomImage.m_stImageInfo.m_nWidth*m_dsDisplayDicom.m_DicomImage.m_stImageInfo.m_nSamplesPerPixel), 8) *
		m_dsDisplayDicom.m_DicomImage.BytesPerLine((UINT)(m_dsDisplayDicom.m_DicomImage.m_stImageInfo.m_nHeight*m_dsDisplayDicom.m_DicomImage.m_stImageInfo.m_nSamplesPerPixel), 8);

	if (nImageSize <= 0)
		return FALSE;

	m_pDisplayImage = new BYTE[nImageSize];

	if (!m_pDisplayImage)
	{
		return FALSE;
	}

	memset(m_pDisplayImage, 0, nImageSize);

	return bRes;
}

void CStudyViewer::FreeDisplayImage()
{
	if (m_pDisplayImage)
	{
		delete[] m_pDisplayImage;
		m_pDisplayImage = nullptr;
	}
}

void CStudyViewer::CreateDragDropObjects()
{
	m_pDragSource = new COleDataSourceEx;
	//
	m_pDropTarget = new COleDropTargetEx(this);
	m_pDropTarget->Register(this);
}


BEGIN_MESSAGE_MAP(CStudyViewer, CWnd)
ON_WM_SIZE()
ON_WM_PAINT()
//ON_WM_MOUSEHWHEEL()
ON_WM_MOUSEWHEEL()
ON_WM_LBUTTONDOWN()
ON_WM_DESTROY()
END_MESSAGE_MAP()


void CStudyViewer::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
}


void CStudyViewer::OnPaint()
{
	CalcLayout();
	CPaintDC dc(this); 
	Draw(&dc);
}

BOOL CStudyViewer::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default

	if (m_aryLocalizerPointF.GetCount() > 0)
	{
		m_aryLocalizerPointF.RemoveAll();
	}

	if (nFlags & MK_SHIFT)
	{
		ChangeSeriesImageByWheel(zDelta);
	}
	else
	{
		ChangeInstanceImageByWheel(zDelta);
	}

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}



void CStudyViewer::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if (!m_pStudy)
		return;

	if (m_aryLocalizerPointF.GetCount() > 0)
	{
		m_aryLocalizerPointF.RemoveAll();
	}

	if (m_pParent->IsKindOf(RUNTIME_CLASS(CLayoutManager)))
	{
		CLayoutManager* pLayoutManager = dynamic_cast<CLayoutManager*>(m_pParent);
		pLayoutManager->SetSelectedStudyViewer(m_nLayoutIndex);
	}

	m_pDragSource->SetDragSourceObject(this);

	CWnd::OnLButtonDown(nFlags, point);
}


void CStudyViewer::OnDestroy()
{
	if (m_pDragSource)
	{
		delete m_pDragSource;
		m_pDragSource = nullptr;
	}
	//
	if (m_pDropTarget)
	{
		delete m_pDropTarget;
		m_pDropTarget = nullptr;
	}

	CWnd::OnDestroy();
	// TODO: Add your message handler code here
}
