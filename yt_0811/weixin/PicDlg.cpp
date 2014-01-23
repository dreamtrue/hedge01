// PicDlg.cpp : implementation file
//

#include "stdafx.h"
#include "..\IBTRADE\resource.h"
#include "PicDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPicDlg dialog


CPicDlg::CPicDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPicDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPicDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_hImg = NULL;
}


void CPicDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPicDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPicDlg, CDialog)
	//{{AFX_MSG_MAP(CPicDlg)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_WX_SCANOK, OnScanOk)
	ON_MESSAGE(WM_WX_LOGINOK, OnLoginOk)
	ON_MESSAGE(WM_WX_HASMSG, OnHasMsg)
	ON_BN_CLICKED(IDC_BUTTON2, &CPicDlg::OnBnClickedButton2)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPicDlg message handlers

BOOL CPicDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW);

	AfxBeginThread(LoadQRCodeProc, this);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//��ȡ��ά���߳�
UINT CPicDlg::LoadQRCodeProc(LPVOID pParam)
{
	CPicDlg *pDlg = (CPicDlg *)pParam;
	if((pDlg->m_hImg = pDlg->m_pWXin->GetQRCode())!= NULL){
		TRACE("�ɹ���ȡ��ά��\r\n");
	}
	else{
		return 0;
	}
	pDlg->m_pWXin->GotoScan();
	if(pDlg != NULL){
		pDlg->SetDlgItemText(IDC_STATIC_TIP, _T("�����ֻ���΢��ɨ��ö�ά��"));
		pDlg->Invalidate();
	}
	return 0;
}

void CPicDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	if (NULL != m_hImg)
	{
		IStream *pstm;
		::CreateStreamOnHGlobal(m_hImg, FALSE, &pstm);
		Image *pImg = Image::FromStream(pstm);
		Graphics graph(dc);
		TRACE("��ά���%d��%d\r\n",pImg->GetWidth(), pImg->GetHeight());
		graph.DrawImage(pImg, 0, 0, pImg->GetWidth(), pImg->GetHeight());
		delete pImg;
		pImg = NULL;
		pstm->Release();
		pstm = NULL;
	}
	
	// Do not call CDialog::OnPaint() for painting messages
}

void CPicDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	// TODO: Add your message handler code here
	//m_hImg ? ::GlobalFree(m_hImg) : NULL;
}


HBRUSH CPicDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	if (pWnd->GetDlgCtrlID() == IDC_STATIC_TIP)
	{
		pDC->SetTextColor(RGB(255, 0, 0));
	}
	
	// TODO: Return a different brush if the default is not desired
	return hbr;
}
//ɨ��ɹ���Ϣ��Ӧ����
LRESULT CPicDlg::OnScanOk(WPARAM wParam, LPARAM lParam)
{
	if(this->m_hWnd != NULL){
		this->SetDlgItemText(IDC_STATIC_TIP, _T("ɨ��ɹ��������ֻ���ȷ�ϵ�¼"));
	}
	return 1;
}

//��¼�ɹ���Ϣ��Ӧ����
LRESULT CPicDlg::OnLoginOk(WPARAM wParam, LPARAM lParam)
{
	if(this->m_hWnd != NULL){
		this->SetDlgItemText(IDC_STATIC_TIP, _T("�ѵ�¼�����ڻ�ȡ��ϵ���б�"));
	}
	//AfxBeginThread(LoadContactsListProc, this);

	return 1;
}

//�յ�΢����Ϣ��Ӧ����
LRESULT CPicDlg::OnHasMsg(WPARAM wParam, LPARAM lParam)
{
	if (1 != wParam) //��ֻ���˴����ı���Ϣ�������յ����ı���Ϣ����
	{
		return 1;
	}

	WXMSG *pwxMsg = (WXMSG *)lParam;
	MessageBox(pwxMsg->strMsg, pwxMsg->strUserName, MB_OK | MB_ICONINFORMATION);

	return 1;
}


void CPicDlg::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	this->DestroyWindow();
}
