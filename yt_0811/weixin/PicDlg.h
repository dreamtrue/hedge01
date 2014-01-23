#if !defined(AFX_PICDLG_H__9692154D_57A4_4C91_B68C_BC096A96EAE6__INCLUDED_)
#define AFX_PICDLG_H__9692154D_57A4_4C91_B68C_BC096A96EAE6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PicDlg.h : header file
//
#include "..\weixin\Weixin.h"

/////////////////////////////////////////////////////////////////////////////
// CPicDlg dialog

class CPicDlg : public CDialog
{
// Construction
public:
	CWeixin *m_pWXin;
	HGLOBAL m_hImg;
	static UINT LoadQRCodeProc(LPVOID pParam);
	CPicDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPicDlg)
	enum { IDD = IDD_DIALOG_PIC };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPicDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPicDlg)
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	afx_msg LRESULT OnScanOk(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLoginOk(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnHasMsg(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton2();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PICDLG_H__9692154D_57A4_4C91_B68C_BC096A96EAE6__INCLUDED_)
