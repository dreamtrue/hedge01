// client2.cpp : Defines the class behaviors for the application.
//
#include "stdafx.h"
#define _CRTDBG_MAP_ALLOC//顺序改变后 函数可能无法正常工作
#include <stdlib.h>
#include <crtdbg.h>//可以将函数malloc（）和free（）映射到对应的调试板本的_malloc_dbg，_free_dbg, 该函数会跟踪内存分配和释放
#include "client2.h"
#include "client2Dlg.h"
#include "..\UserMsg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClient2App

BEGIN_MESSAGE_MAP(CClient2App, CWinApp)
	//{{AFX_MSG_MAP(CClient2App)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClient2App construction

CClient2App::CClient2App()
{
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CClient2App object

CClient2App theApp;

/////////////////////////////////////////////////////////////////////////////
// CClient2App initialization

BOOL CClient2App::InitInstance()
{

	//检查内存泄漏
	AfxEnableMemoryTracking(true);
	_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_DEBUG );
	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG));  
	_CrtDumpMemoryLeaks();
	_CrtSetBreakAlloc(136);
	_CrtMemState s1, s2, s3;  
    _CrtMemCheckpoint(&s1);
	afxMemDF = allocMemDF | delayFreeMemDF | checkAlwaysMemDF;



	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}
	GdiplusStartup(&m_gdiToken,&m_gdiInput,NULL);
	AfxEnableControlContainer();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
	CClient2Dlg dlg;   
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
