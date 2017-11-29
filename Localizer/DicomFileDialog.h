#pragma once
#include "afxdlgs.h"
class CDicomFileDialog :
	public CFileDialog
{
public:
	CDicomFileDialog(BOOL bOpenFileDialog, // FileOpen�� TRUE, FileSaveAs�� FALSE
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);

	~CDicomFileDialog();
};

