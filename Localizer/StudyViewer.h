#pragma once
#include "afxwin.h"
#include "Study.h"
#include "Instance.h"
#include "DicomImage.h"
#include "OleDataSourceEx.h"
#include "OleDropTargetEx.h"

//
struct DIBINFO : public BITMAPINFO
{
	RGBQUAD arColors[255];
	operator LPBITMAPINFO()
	{
		return (LPBITMAPINFO)this;
	}
	operator LPBITMAPINFOHEADER()
	{
		return &bmiHeader;
	}
	RGBQUAD* ColorTable()
	{
		return bmiColors;
	}
	void Init()
	{
		LPRGBQUAD pColorTable = ColorTable();
		for (UINT nColor = 0; nColor < 256; nColor++)
		{
			pColorTable[nColor].rgbReserved = 0;
			pColorTable[nColor].rgbBlue = nColor;
			pColorTable[nColor].rgbRed = nColor;
			pColorTable[nColor].rgbGreen = nColor;
		}
	}
};

class COleDataSourceEx;
class COleDropTargetEx;
class CStudyViewer :	public CWnd
{
public:

	friend class CLayoutManager;

	CStudyViewer(INT_PTR nLayoutIndex);
	~CStudyViewer();

protected:
	CInstance* m_pDisplayedInstance;	// 가지고 있을 필요 없을거 같긴하다.
	
	INT_PTR m_nCurSeriesIndex;
	INT_PTR m_nCurInstanceIndex;
	INT_PTR m_nCurFrameIndex;

	CStudy* m_pStudy;
	CLLDicomDS* m_pDisplayDicomDS;

	COleDataSourceEx* m_pDragSource;
	COleDropTargetEx* m_pDropTarget;

	Gdiplus::PointF m_ptLocalizerStart;
	Gdiplus::PointF m_ptLocalizerEnd;

	DIBINFO m_DibInfo;

private:
	INT_PTR m_nLayoutIndex;
	CWnd* m_pParent;
			
	BYTE* m_pDisplayImage;				// image processed image

	CRect m_rtImage;
	CRect m_rtCanvas;

	BOOL m_bOnlySeriesViewer;

public:
	CStudyViewer& operator=(const CStudyViewer& obj);
	
	// Set
	void SetStudy(CStudy* pStudy);
	void SetDisplayInstance();
	//
	void SetViewOnlySameSeries();
	void SetViewAllSeries();
	void SetCurrentInstanceIndex(INT_PTR nInstanceIndex);
	void SetLocalizerPoints(Gdiplus::PointF ptLocalizerStart, Gdiplus::PointF ptLocalizerEnd);

	// Get
	INT_PTR GetLayoutIndex();
	CStudy* GetStudy();
	INT_PTR GetInstanceCount();
	INT_PTR GetSeriesCount();
	INT_PTR GetCurrentInstanceIndex();
	INT_PTR GetCurrentSeriesIndex();
	INT_PTR GetCurrentFrameIndex();
	CLLDicomDS* GetDisplayingDicomDS();
	void RedrawWnd();

	// Load
	void LoadImageFromDcm(CDicomImage& imageDicom);

	// Clear
	void ClearLocalizerPoints();

protected:
	BOOL IsValidStudy(CStudy* pStudy);
	BOOL DrawInstanceImage(CDC* pDC);
	BOOL DrawLocalizer(CDC* pDC);
	BOOL DrawImageInfo(CDC* pDC);
	BOOL DrawPatientOrientation(CDC* pDC);

	void ChangeInstanceIndex(BOOL bIsIncrease);
	void ChangeSeriesIndex(BOOL bIsIncrease);

	void ChangeInstanceImageByWheel(short zDelta);
	void ChangeSeriesImageByWheel(short zDelta);

// Overriding
protected:
	virtual void Init(INT_PTR nCurSeriesIndex = 0, INT_PTR nCurInstanceIndex = 0, INT_PTR nCurFrameIndex = 0);
	virtual BOOL CreateCtrl(CWnd* pParent);
	virtual BOOL Draw(CDC* pDC);
	virtual DIBINFO* GetDibInfo();
	virtual BOOL CalcLayout();
	virtual INT_PTR CalcImageRect(CDicomImage* imageInfo);
	virtual BOOL AllocDisplayImage();
	virtual void FreeDisplayImage();
	virtual void CreateDragDropObjects();

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
};

