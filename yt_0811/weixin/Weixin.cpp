#pragma once
#include "StdAfx.h"
#include "Weixin.h"

/*****************************************************************/
//UTF8 转 GBK
void CWeixin::UTF8ToGBK(CString& strTarget)
{
	int len = strTarget.GetLength();
	int unicodeLen = MultiByteToWideChar(CP_UTF8, 0, strTarget, -1, NULL, 0);

	wchar_t *pUnicode;
	pUnicode = new wchar_t[unicodeLen + 1];

	memset(pUnicode, 0, (unicodeLen + 1) * sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, strTarget, -1, (LPWSTR)pUnicode, unicodeLen);

	BYTE *pTargetData = NULL;
	int targetLen = WideCharToMultiByte(CP_ACP, 0, (LPWSTR)pUnicode, -1, (char*)pTargetData, 0, NULL, NULL);

	pTargetData = new BYTE[targetLen + 1];
	memset(pTargetData, 0, targetLen + 1);

	WideCharToMultiByte(CP_ACP, 0, (LPWSTR)pUnicode, -1 ,(char *)pTargetData, targetLen, NULL, NULL);

	strTarget.Format("%s", pTargetData);

	delete []pUnicode;
	delete []pTargetData;
}

//GBK 转 UTF8
void CWeixin::GBKToUTF8(CString& strTarget)
{
	int len = MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)strTarget, -1, NULL,0);
	unsigned short *wszUtf8 = new unsigned short[len+1];
	memset(wszUtf8, 0, len * 2 + 2);
	MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)strTarget, -1, (LPWSTR)wszUtf8, len);

	len = WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)wszUtf8, -1, NULL, 0, NULL, NULL);
	char *szUtf8 = new char[len + 1];
	memset(szUtf8, 0, len + 1);
	WideCharToMultiByte (CP_UTF8, 0, (LPWSTR)wszUtf8, -1, szUtf8, len, NULL,NULL);

	strTarget = szUtf8;
	delete[] szUtf8;
	delete[] wszUtf8;
}

//监听二维码被扫描线程
UINT CWeixin::CheckScanProc(LPVOID pParam)
{
	CWeixin *pWXin = (CWeixin *)pParam;

	while (1)
	{
		time_t tm;
		time(&tm);
		SYSTEMTIME st;
		GetLocalTime(&st);

		CString strURL;
		strURL.Format(_T("https://login.weixin.qq.com/cgi-bin/mmwebwx-bin/login?uuid=%s&tip=1&_=%ld%d"),
			pWXin->m_strCode, tm, st.wMilliseconds);
		CHttpFile *pFile = NULL;
		try{
			pFile = (CHttpFile *)pWXin->m_pSession->OpenURL(strURL, 1, 
				INTERNET_FLAG_TRANSFER_BINARY | INTERNET_FLAG_RELOAD);
		}
		catch(CInternetException*pException){
			pException->Delete();
		}		
		char ch;
		CString strData;
		if(pFile == NULL){
			continue;
		}
		try{
			while(pFile->Read(&ch, 1))
			{
				strData +=ch;
			}
		}
		catch(CInternetException*pException){
			pException->Delete();
		}
		pFile->Close();
		delete pFile;
		pFile = NULL;

		if(strData.Find(_T("201")) != -1)
		{
			AfxBeginThread(CheckOKProc, pWXin); //创建线程，检测确认登录
			break;
		}
	}

	//已经扫描，通知窗口
	::SendMessage(pWXin->m_hWnd, WM_WX_SCANOK, 0, 0);

	return 1;
}

//检测确认登录线程
UINT CWeixin::CheckOKProc(LPVOID pParam)
{
	CWeixin *pWXin = (CWeixin *)pParam;

	while (1)
	{
		time_t tm;
		time(&tm);
		SYSTEMTIME st;
		GetLocalTime(&st);

		CString strURL;
		strURL.Format(_T("https://login.weixin.qq.com/cgi-bin/mmwebwx-bin/login?uuid=%s&tip=0&_=%ld%d"),
			pWXin->m_strCode, tm, st.wMilliseconds);
		CHttpFile *pFile = NULL;
		try{
			pFile = (CHttpFile *)pWXin->m_pSession->OpenURL(strURL, 1, 
				INTERNET_FLAG_TRANSFER_ASCII | INTERNET_FLAG_RELOAD);
		}
		catch(CInternetException*pException){
			pException->Delete();
		}
		char ch;
		CString strData;
		if(pFile == NULL){
			continue;
		}
		try{
			while(pFile->Read(&ch, 1))
			{
				strData += ch;
			}
		}
		catch(CInternetException*pException){
			pException->Delete();
		}
		pFile->Close();
		delete pFile;
		pFile = NULL;

		if (strData.Find(_T("window.redirect_uri=")) != -1)
		{
			CString strURI;
			for (int i = strData.Find(_T("window.redirect_uri=\"")) + 21; strData.GetAt(i) != '\"'; i++)
			{
				strURI += strData.GetAt(i);
			}
			try{
				pFile = (CHttpFile *)pWXin->m_pSession->OpenURL(strURI + _T("&fun=new"), 1, INTERNET_FLAG_TRANSFER_ASCII | INTERNET_FLAG_RELOAD);
			}
			catch(CInternetException*pException){
				pException->Delete();
			}
			CString strCookie;
			try{
				pWXin->m_pSession->GetCookie(pFile->GetFileURL(), _T(""), strCookie);
			}
			catch(CInternetException*pException){
				pException->Delete();
			}
			for (int i = strCookie.Find(_T("wxsid=")) + 6; strCookie.GetAt(i) != ';'; i++)
			{
				pWXin->m_strSid += strCookie.GetAt(i);

			}
			pFile->Close();
			delete pFile;
			pFile = NULL;;
			break;
		}
	}

	//获取自己的账号和用户名
	pWXin->GetSelfInfo();

	//登录成功通知窗口
	::SendMessage(pWXin->m_hWnd, WM_WX_LOGINOK, 0, 0);

	//保持在线
	AfxBeginThread(SyncProc, pWXin);

	//接收消息
	AfxBeginThread(CheckMsgProc, pWXin);

	return 1;
}

//得到自己的信息
void CWeixin::GetSelfInfo()
{
	time_t tm;
	time(&tm);
	SYSTEMTIME st;
	GetLocalTime(&st);

	CString strURL;
	strURL.Format(_T("https://wx.qq.com/cgi-bin/mmwebwx-bin/webwxinit?r=%ld%d"),
		tm, st.wSecond);

	char ch;
	CString strData;
	CHttpFile *pFile = NULL;
	try{
		pFile = (CHttpFile *)m_pSession->OpenURL(strURL, 1, INTERNET_FLAG_TRANSFER_ASCII | INTERNET_FLAG_RELOAD);
	}
	catch(CInternetException*pException){
		pException->Delete();
	}
	if(pFile == NULL){
		return;
	}
	try{
		while (pFile->Read(&ch, 1))
		{
			strData += ch;
		}
	}
	catch(CInternetException*pException){
		pException->Delete();
	}
	pFile->Close();
	UTF8ToGBK(strData);

	int i = strData.Find(_T("\"User\":"));
	//账号
	for (i = strData.Find(_T("\"Uin\":"), i) + 7; strData.GetAt(i) != ','; i++)
	{
		m_strMyUin += strData.GetAt(i);
	}
	//用户名
	for (i = strData.Find(_T("\"UserName\":"), i) + 13; strData.GetAt(i) != '\"'; i++)
	{
		m_strMyUserName += strData.GetAt(i);
	}
	//Skey
	for (i = strData.Find(_T("\"SKey\":")) + 9; strData.GetAt(i) != '\"'; i++)
	{
		m_strSkey += strData.GetAt(i);
	}

	//获取 SyncKey
	CString strVal;
	int nCount = 0;
	i = 0;
	while (i = strData.Find(_T("\"Val\": "), i))
	{
		for(; strData.GetAt(i) != '\n'; i++)
		{
			strVal += strData.GetAt(i);
		}
		strVal.Replace(_T("\"Val\": "), _T(""));
		nCount++;
		if (nCount == 1)
		{
			m_strVal1  = strVal;
		}
		if (nCount == 2)
		{
			m_strVal2  = strVal;
		}
		if (nCount == 3)
		{
			m_strVal3 = strVal;
			break;
		}
		/*	if(nCount == 4)
		{
		strVal1000  = strVal;
		break;
		}*/
		strVal.Empty();
	}
}

//保持在线状态
UINT CWeixin::SyncProc(LPVOID pParam)
{
	CWeixin *pWXin = (CWeixin *)pParam;
	//BOOL bFailed = FALSE;

	while (1)
	{
		//if (!bFailed)
		{
			Sleep(1000 * 60 * 1); //1分钟一次
			//bFailed = FALSE;
		}

		CString strVal;
		int nCount;
		int i;
		time_t tm;
		SYSTEMTIME st;
		time(&tm);
		GetLocalTime(&st);
		CString strObj, strJson, strHeaders;
		strObj.Format(_T("/cgi-bin/mmwebwx-bin/webwxsync?sid=%s&r=%ld%d%d"),
			pWXin->m_strSid, tm, st.wSecond, st.wMilliseconds);
		strJson.Format(_T("{\"BaseRequest\":{\"Uin\":%s,\"Sid\":\"%s\"},\"SyncKey\":{\"Count\":4,\"List\":[{\"Key\":1,\"Val\":%s},{\"Key\":2,\"Val\":%s},{\"Key\":3,\"Val\":%s}]}}"),
			pWXin->m_strMyUin, pWXin->m_strSid, pWXin->m_strVal1, pWXin->m_strVal2, pWXin->m_strVal3);
		strHeaders += _T("Accept:application/json, text/javascript, */*; q=0.01\r\n");
		strHeaders += _T("Content-Type:application/json; charset=utf-8\r\n");
		strHeaders += _T("Referer:https://wx.qq.com/cgi-bin/mmwebwx-bin/webwxreadtemplate?t=chat#chat\r\n");
		strHeaders += _T("Accept-Language:zh-cn\r\n");
		strHeaders += _T("User-Agent:Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0)\r\n");
		strHeaders += _T("Connection:Keep-Alive\r\n");
		strHeaders += _T("Cache-Control:no-cache");
		CHttpFile *pFile = NULL;
		CHttpConnection *pConn;
		try{
			pConn = pWXin->m_pSession->GetHttpConnection(_T("wx.qq.com"));
			pFile = pConn->OpenRequest(CHttpConnection::HTTP_VERB_POST, strObj);
			pFile->SendRequest(strHeaders, (LPVOID)(LPCTSTR)strJson, strJson.GetLength());
		}
		catch(CInternetException*pException){
			pException->Delete();
		}
		CString strData;
		char ch;
		if(pFile == NULL){
			continue;
		}
		try{
			while (pFile->Read(&ch, 1))
			{
				strData += ch;
			}
		}
		catch(CInternetException*pException){
			pException->Delete();
		}
		pFile->Close();
		delete pFile;
		pFile = NULL;
		pConn->Close();
		delete pConn;
		pConn = NULL;

		if (strData.Find(_T("\"Ret\": 0")) == -1)
		{
			TRACE(_T("保持在线失败"));
			Sleep(1000); //3秒重试
			//bFailed = TRUE;
			continue;
		}

		//刷新Skey
		//测试版去除这段代码
		for (i = strData.Find(_T("\"SKey\":")) + 9; strData.GetAt(i) != '\"'; i++)
		{
			pWXin->m_strSkey += strData.GetAt(i);
		}

		//刷新val
		nCount = 0;
		i = 0;
		while (i = strData.Find(_T("\"Val\": "), i))
		{
			for (; strData.GetAt(i) != '\n'; i++)
			{
				strVal += strData.GetAt(i);
			}
			strVal.Replace(_T("\"Val\": "), _T(""));
			nCount++;
			if(nCount == 1)
			{
				pWXin->m_strVal1  = strVal;
			}
			if(nCount == 2)
			{
				pWXin->m_strVal2  = strVal;
			}
			if(nCount == 3)
			{
				pWXin->m_strVal3 = strVal;
				break;
			}
			strVal.Empty();
		}
	}

	return 0;
}

//返回消息类型
//1为文本，2为语音
UINT CWeixin::CheckMsgProc(LPVOID pParam)
{
	CWeixin *pWXin = (CWeixin *)pParam;

	time_t tm;
	SYSTEMTIME st;
	CString strObj, strHeaders, strJson;
	CHttpConnection *pConn = NULL;
	CHttpFile *pFile = NULL;
	char ch;
	CString strData;

	for(int n = 0; n < 3; n++)
	{
		CString strVal;
		int nCount;
		int i;

		time(&tm);
		GetLocalTime(&st);
		strObj.Format(_T("/cgi-bin/mmwebwx-bin/webwxsync?sid=%s&r=%ld%d%d"),
			pWXin->m_strSid, tm, st.wSecond, st.wMilliseconds);
		strJson.Format(_T("{\"BaseRequest\":{\"Uin\":%s,\"Sid\":\"%s\"},\"SyncKey\":{\"Count\":4,\"List\":[{\"Key\":1,\"Val\":%s},{\"Key\":2,\"Val\":%s},{\"Key\":3,\"Val\":%s}]}}"),
			pWXin->m_strMyUin, pWXin->m_strSid, pWXin->m_strVal1, pWXin->m_strVal2, pWXin->m_strVal3);
		strHeaders += _T("Accept:application/json, text/javascript, */*; q=0.01\r\n");
		strHeaders += _T("Content-Type:application/json; charset=utf-8\r\n");
		strHeaders += _T("X-Requested-With:XMLHttpRequest\r\n");
		strHeaders += _T("Referer:https://wx.qq.com/cgi-bin/mmwebwx-bin/webwxreadtemplate?t=chat\r\n");
		strHeaders += _T("Accept-Language:zh-cn\r\n");
		strHeaders += _T("User-Agent:Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0)\r\n");
		strHeaders += _T("Connection:Keep-Alive\r\n");
		strHeaders += _T("Cache-Control:no-cache");
		try{
			pConn = pWXin->m_pSession->GetHttpConnection(_T("wx.qq.com"));
			pFile = pConn->OpenRequest(CHttpConnection::HTTP_VERB_POST, strObj);
			pFile->SendRequest(strHeaders, (LPVOID)(LPCTSTR)strJson, strJson.GetLength());
		}
		catch(CInternetException*pException){
			pException->Delete();
		}
		try{
			while(pFile->Read(&ch, 1))
			{
				strData += ch;
			}
		}
		catch(CInternetException*pException){
			pException->Delete();
		}
		//	AfxMessageBox(strData);
		//	pHelper->SetTextToClipboard(strData);
		pFile->Close();
		delete pFile;
		pFile = NULL;
		pConn->Close();
		delete pConn;
		pConn = NULL;
		nCount = 0;
		i = 0;
		while(i = strData.Find(_T("\"Val\": "), i))
		{
			for(; strData.GetAt(i) != '\n'; i++)
			{
				strVal += strData.GetAt(i);
			}
			strVal.Replace(_T("\"Val\": "), _T(""));
			nCount++;
			if(nCount == 1)
			{
				pWXin->m_strVal1  = strVal;
			}
			if(nCount == 2)
			{
				pWXin->m_strVal2  = strVal;
			}
			if(nCount == 3)
			{
				pWXin->m_strVal3 = strVal;
				break;
			}
			/*	if(nCount == 4)
			{
			pHelper->strVal1000  = strVal;
			break;
			}*/
			strVal.Empty();
		}
		strData.Empty();
		strObj.Empty();
		strHeaders.Empty();
		strJson.Empty();
		strVal.Empty();
	}

	while(1)
	{
		Sleep(1000);//1秒一次,减少cpu负担
		strData.Empty();
		strObj.Empty();
		strHeaders.Empty();
		strJson.Empty();

		time(&tm);
		GetLocalTime(&st);
		strObj.Format(_T("/cgi-bin/mmwebwx-bin/synccheck?sid=%s&uin=%s&deviceid=e829323553833479&synckey=1_%s%%7C2_%s%%7C3_%s&f=json&r=%ld%d%d&r="),
			pWXin->m_strSid, pWXin->m_strMyUin, pWXin->m_strVal1, pWXin->m_strVal2, pWXin->m_strVal3, 
			tm, st.wSecond, st.wMilliseconds);
		CString strTemp = strObj;
		strObj.Format(_T("%s%ld%d%d"), strTemp, tm, st.wSecond, st.wMilliseconds);
		strJson.Format(_T("{\"sid\":\"%s\",\"uin\":%s,\"deviceid\":\"e829323553833479\",\"synckey\":\"1_%s|2_%s|3_%s\",\"f\":\"json\",\"r\":%ld%d%d}"),
			pWXin->m_strSid, pWXin->m_strMyUin, pWXin->m_strVal1, pWXin->m_strVal2, pWXin->m_strVal3, 
			tm, st.wSecond, st.wMilliseconds);

		strHeaders = _T("Accept:application/json, text/javascript, */*; q=0.01\r\n");
		strHeaders += _T("Content-Type:application/json; charset=utf-8\r\n");
		strHeaders += _T("Referer:https://webpush.weixin.qq.com/cgi-bin/mmwebwx-bin/webwxreadtemplate?t=domain&d=qq.com\r\n");
		strHeaders += _T("Accept-Language:zh-cn\r\n");
		strHeaders += _T("User-Agent:Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0)\r\n");
		strHeaders += _T("Connection:Keep-Alive\r\n");
		strHeaders += _T("Cache-Control:no-cache");
		try{
			pConn = pWXin->m_pSession->GetHttpConnection(_T("webpush.weixin.qq.com"));
			pFile = pConn->OpenRequest(CHttpConnection::HTTP_VERB_POST, strObj);
			pFile->SendRequest(strHeaders, (LPVOID)(LPCTSTR)strJson, strJson.GetLength());
		}
		catch(CInternetException*pException){
			pException->Delete();
		}
		/*	char ch;
		CString strData;*/
		if(pFile == NULL){
			continue;
		}
		try{
			while(pFile->Read(&ch, 1))
			{
				strData += ch;
			}
		}
		catch(CInternetException*pException){
			pException->Delete();
		}

		pFile->Close();
		delete pFile;
		pFile = NULL;
		pConn->Close();
		delete pConn;
		pConn = NULL;
		if(strData.Find(_T("\"selector\":\"6\"")) != -1)
		{
			WXMSG wxMsg;
			UINT uRet = pWXin->PeekMsg(wxMsg);
			::SendMessage(pWXin->m_hWnd, WM_WX_HASMSG, uRet, (LPARAM)&wxMsg);
		}
	}

	return 1;
}
/*****************************************************************/

CWeixin::CWeixin()
{
	m_pSession = new CInternetSession(_T("Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0)"));
	if (NULL == m_pSession)
	{
		AfxMessageBox(_T("构造对象失败"));
		abort();
	}
}

CWeixin::CWeixin(CWeixin& wxin)
{
	m_hWnd = wxin.m_hWnd;
	m_pSession = new CInternetSession(/*_T("Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0)")*/);
	CopyMemory(*m_pSession, *wxin.m_pSession, sizeof (*wxin.m_pSession)); //这样貌似不对 =_=!
	m_strCode = wxin.m_strCode;
	m_strSid = wxin.m_strSid;
	m_strSkey = wxin.m_strSkey;
	m_strMyUin = wxin.m_strMyUin;
	m_strMyUserName = wxin.m_strMyUserName;
	m_strVal1 = wxin.m_strVal1;
	m_strVal2 = wxin.m_strVal2;
	m_strVal3 = wxin.m_strVal3;

	for (int i = 0; i < m_paContacts.GetSize(); i++)
	{
		WXCONTACT *pwxCont = new WXCONTACT;
		*pwxCont = (*(WXCONTACT *)wxin.m_paContacts.GetAt(i));
		HGLOBAL hHeadImg = ::GlobalAlloc(GPTR, pwxCont->ihiSize);   //->
		CopyMemory(hHeadImg, pwxCont->hHeadImg, pwxCont->ihiSize);  //-> 拷贝头像
		pwxCont->hHeadImg = hHeadImg;								//->
		m_paContacts.Add(pwxCont);
	}
}

CWeixin::~CWeixin()
{
	if (NULL != m_pSession)
	{
		m_pSession->Close();
		delete m_pSession;
	}

	for (int i = 0; i < m_paContacts.GetSize(); i++)
	{
		WXCONTACT *pwxCont = (WXCONTACT *)m_paContacts.GetAt(i);
		if (NULL != pwxCont)
		{
			pwxCont->hHeadImg ? ::GlobalFree(pwxCont->hHeadImg) : 0; //释放头像所占内存
			delete pwxCont;
			pwxCont = NULL;
		}
	}

}

void CWeixin::Attach(HWND hWnd)
{
	m_hWnd = hWnd;
}

HGLOBAL CWeixin::GetQRCode()
{
	//获取二维码的 URL
	CHttpFile * pFile;
	try{
		pFile = (CHttpFile *)m_pSession->OpenURL(_T("https://login.weixin.qq.com/qrlogin?appid=wx782c26e4c19acffb&type=login&lang=zh_CN&redirect_uri=https://wx.qq.com/cgi-bin/mmwebwx-bin/webwxnewloginpage"), 
			1, INTERNET_FLAG_TRANSFER_ASCII | INTERNET_FLAG_RELOAD);
	}
	catch(CInternetException*pException){
		pException->Delete();
	}
	//	CHttpFile *pFile = (CHttpFile *)m_pSession->OpenURL(_T("https://wx.qq.com"), 
	//	1, INTERNET_FLAG_TRANSFER_ASCII | INTERNET_FLAG_RELOAD);
	char ch;
	CString strData;
	try{
		while (pFile->Read(&ch, 1))
		{
			strData += ch;
		}
	}
	catch(CInternetException*pException){
		pException->Delete();
	}
	pFile->Close();
	delete pFile;
	pFile = NULL;

	for (int i = strData.Find(_T("src=\"/qrcode/"))+13; strData.GetAt(i) != '?'; i++)
	{
		m_strCode += strData.GetAt(i);
	}

	//获取二维码
	CString strImgURL;
	strImgURL.Format(_T("https://login.weixin.qq.com/qrcode/%s?t=webwx"), m_strCode);
	try{
		pFile = (CHttpFile *)m_pSession->OpenURL(strImgURL, 1, INTERNET_FLAG_TRANSFER_ASCII | INTERNET_FLAG_RELOAD);
	}
	catch(CInternetException*pException){
		pException->Delete();
	}
	BYTE b;
	strData.Empty();
	try{
		while (pFile->Read(&b, 1))
		{
			strData += b;
		}
	}
	catch(CInternetException*pException){
		pException->Delete();
	}
	pFile->Close();
	delete pFile;
	pFile = NULL;

	HGLOBAL hImgData = GlobalAlloc(GPTR, strData.GetLength());
	CopyMemory(hImgData, (LPVOID)(LPCTSTR)strData, strData.GetLength());
	return hImgData;
}

BOOL CWeixin::GetQRCode(LPCTSTR lpszPath)
{
	//获取二维码的 URL
	CHttpFile *pFile = NULL;
	try{
		pFile = (CHttpFile *)m_pSession->OpenURL(_T("https://login.weixin.qq.com/qrlogin?appid=wx782c26e4c19acffb&type=login&lang=zh_CN&redirect_uri=https://wx.qq.com/cgi-bin/mmwebwx-bin/webwxnewloginpage"), 
			1, INTERNET_FLAG_TRANSFER_ASCII | INTERNET_FLAG_RELOAD);
	}
	catch(CInternetException*pException){
		pException->Delete();
	}
	char ch;
	CString strData;
	try{
		while (pFile->Read(&ch, 1))
		{
			strData += ch;
		}
	}
	catch(CInternetException*pException){
		pException->Delete();
	}
	pFile->Close();
	delete pFile;
	pFile = NULL;

	if (strData.IsEmpty())
	{
		return FALSE;
	}

	for (int i = strData.Find(_T("src=\"/qrcode/"))+13; strData.GetAt(i) != '?'; i++)
	{
		m_strCode += strData.GetAt(i);
	}

	//获取二维码
	CString strImgURL;
	strImgURL.Format(_T("https://login.weixin.qq.com/qrcode/%s?t=webwx"), m_strCode);
	try{
		pFile = (CHttpFile *)m_pSession->OpenURL(strImgURL, 1, INTERNET_FLAG_TRANSFER_ASCII | INTERNET_FLAG_RELOAD);
	}
	catch(CInternetException*pException){
		pException->Delete();
	}	
	BYTE b;
	strData.Empty();
	try{
		while (pFile->Read(&b, 1))
		{
			strData += b;
		}
	}
	catch(CInternetException*pException){
		pException->Delete();
	}
	pFile->Close();
	delete pFile;
	pFile = NULL;

	if (strData.IsEmpty())
	{
		return FALSE;
	}

	CFile file;
	if (!file.Open(lpszPath, CFile::modeCreate | CFile::modeWrite))
	{
		return FALSE;
	}
	file.Write((LPCTSTR)strData, strData.GetLength());
	file.Close();

	return TRUE;
}

void CWeixin::GotoScan()
{
	//创建线程监听二维码被扫描
	AfxBeginThread(CheckScanProc, this);
}

BOOL CWeixin::QueryContacts(BOOL bHeadImg)
{
	//清空原有联系人
	m_paContacts.RemoveAll();
	//当前时间
	time_t tm;
	time(&tm);
	SYSTEMTIME st;
	GetLocalTime(&st);

	//得到联系人
	CString strURL;
	strURL.Format(_T("https://wx.qq.com/cgi-bin/mmwebwx-bin/webwxgetcontact?r=%ld%d"), tm, st.wMilliseconds);
	CHttpFile *pFile = NULL;
	try{
		pFile = (CHttpFile *)m_pSession->OpenURL(strURL, 1, INTERNET_FLAG_TRANSFER_ASCII | INTERNET_FLAG_RELOAD);
	}
	catch(CInternetException*pException){
		pException->Delete();
	}
	CString strData;
	char ch;
	try{
		while (pFile->Read(&ch, 1))
		{
			strData += ch;
		}
	}
	catch(CInternetException*pException){
		pException->Delete();
	}
	pFile->Close();
	delete pFile;
	pFile = NULL;
	UTF8ToGBK(strData);

	int nOffset = 0;
	while ((nOffset = strData.Find(_T("\"Uin\":"), nOffset)) != -1)
	{
		CString strInfo;
		for (; strInfo.Find(_T("\"UniFriend\":")) == -1; nOffset++)
		{
			strInfo += strData.GetAt(nOffset);
		}

		//去掉昵称中的表情符号
		while (strInfo.Find(_T("<span class=")) != -1)
		{
			int j = strInfo.Find(_T("<span class="));
			CString strEmoji;
			for (; strEmoji.Find(_T("</span>")) == -1; j++)
			{
				strEmoji += strInfo.GetAt(j);
			}
			strInfo.Replace(strEmoji, _T(""));
		}

		WXCONTACT *pwxCont = new WXCONTACT;

		int i;
		//微信号
		for (i = strInfo.Find(_T("\"Uin\":")) + 7; strInfo.GetAt(i) != ','; i++)
		{
			pwxCont->strUin += strInfo.GetAt(i);
		}
		//用户名
		for (i = strInfo.Find(_T("\"UserName\":")) + 13; strInfo.GetAt(i) != '\"'; i++)
		{
			pwxCont->strUserName += strInfo.GetAt(i);
		}
		//昵称
		for (i = strInfo.Find(_T("\"NickName\":")) + 13; strInfo.GetAt(i) != '\"'; i++)
		{
			pwxCont->strNickName += strInfo.GetAt(i);
		}
		//备注名
		for (i = strInfo.Find(_T("\"RemarkName\":")) + 15; strInfo.GetAt(i) != '\"'; i++)
		{
			pwxCont->strRemarkName += strInfo.GetAt(i);
		}
		//性别
		CString strTemp;
		for (i = strInfo.Find(_T("\"Sex\":")) + 7; strInfo.GetAt(i) != ','; i++)
		{
			strTemp += strInfo.GetAt(i);
		}
		pwxCont->iSex = atoi(strTemp);
		//签名
		for (i = strInfo.Find(_T("\"Signature\":")) + 14; strInfo.GetAt(i) != '\"'; i++)
		{
			pwxCont->strSignature += strInfo.GetAt(i);
		}
		//省
		for (i = strInfo.Find(_T("\"Province\":")) + 13; strInfo.GetAt(i) != '\"'; i++)
		{
			pwxCont->strProvince += strInfo.GetAt(i);
		}
		//市
		for (i = strInfo.Find(_T("\"City\":")) + 9; strInfo.GetAt(i) != '\"'; i++)
		{
			pwxCont->strCity += strInfo.GetAt(i);
		}
		if (bHeadImg)
		{
			//头像URL
			CString strHeadURL;
			for (i = strInfo.Find(_T("\"HeadImgUrl\":")) + 15; strInfo.GetAt(i) != '\"'; i++)
			{
				strHeadURL += strInfo.GetAt(i);
			}
			//获取头像
			CString strImg;
			int iSize = 0;
			try{
				pFile = (CHttpFile *)m_pSession->OpenURL(_T("https://wx.qq.com") + strHeadURL
					/*, 1, INTERNET_FLAG_TRANSFER_ASCII | INTERNET_FLAG_RELOAD*/);
			}
			catch(CInternetException*pException){
				pException->Delete();
			}
			try{
				while (pFile->Read(&ch, 1))
				{
					strImg += ch;
					iSize++;
				}
			}
			catch(CInternetException*pException){
				pException->Delete();
			}
			pFile->Close();
			delete pFile;
			pFile = NULL;

			pwxCont->hHeadImg = ::GlobalAlloc(GPTR, strImg.GetLength());
			CopyMemory(pwxCont->hHeadImg, (LPVOID)(LPCTSTR)strImg, strImg.GetLength());
			pwxCont->ihiSize = iSize;
		}
		else
		{
			pwxCont->hHeadImg = NULL;
			pwxCont->ihiSize = 0;
		}

		m_paContacts.Add(pwxCont);
	}
	if (m_paContacts.GetSize() == 0)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

int CWeixin::GetContanctsCount()
{
	return m_paContacts.GetSize();
}

WXCONTACT CWeixin::GetContanctAt(int nIndex)
{
	return (*(WXCONTACT *)m_paContacts.GetAt(nIndex));
}

BOOL CWeixin::SendMsg(LPCTSTR lpszUserName, LPCTSTR lpszMsg)
{
	CString strMsg(lpszMsg);
	GBKToUTF8(strMsg);

	time_t tm;
	time(&tm);
	SYSTEMTIME st;
	GetLocalTime(&st);

	CString strJson, strTemp;
	strJson.Format(_T("{\"BaseRequest\":{\"Uin\":%s,\"Sid\":\"%s\",\"DeviceID\":\"e829323553833479\"},\"Msg\":{\"FromUserName\":\"%s\",\"ToUserName\":\"%s\",\"Type\":1,\"Content\":\"%s\",\"ClientMsgId\":%ld%d%d,\"LocalID\":"),
		m_strMyUin, m_strSid, m_strMyUserName, lpszUserName, strMsg, tm, st.wMinute, st.wMilliseconds);
	strTemp.Format(_T("%ld%d%d}}"), strJson, tm, st.wMinute, st.wMilliseconds);
	strJson += strTemp;

	CString strObj, strHeaders;
	strObj.Format(_T("/cgi-bin/mmwebwx-bin/webwxsendmsg?sid=%s&r=%ld%d%d"), m_strSid, tm, st.wMinute, st.wMilliseconds);
	strHeaders += _T("Accept:application/json, text/javascript, */*; q=0.01\r\n");
	strHeaders += _T("Content-Type:application/json; charset=utf-8\r\n");
	strHeaders += _T("X-Requested-With:XMLHttpRequest\r\n");
	strHeaders += _T("Referer:https://wx.qq.com/cgi-bin/mmwebwx-bin/webwxreadtemplate?t=chat#chat?userName=");
	strHeaders += lpszUserName;
	strHeaders += _T("\r\n");
	strHeaders += _T("Accept-Language:zh-cn\r\n");
	strHeaders += _T("User-Agent:Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0)\r\n");
	strHeaders += _T("Connection:Keep-Alive\r\n");
	strHeaders += _T("Cache-Control:no-cache");
	CHttpConnection *pConn;
	CHttpFile *pFile = NULL;
	try{
		pConn = m_pSession->GetHttpConnection(_T("wx.qq.com"));
		pFile = pConn->OpenRequest(CHttpConnection::HTTP_VERB_POST, strObj);
		pFile->SendRequest(strHeaders, (LPVOID)(LPCTSTR)strJson, strJson.GetLength());
	}
	catch(CInternetException*pException){
		pException->Delete();
	}
	CString strData;
	char ch;
	try{
		while (pFile->Read(&ch, 1))
		{
			strData += ch;
		}
	}
	catch(CInternetException*pException){
		pException->Delete();
	}

	pFile->Close();
	delete pFile;
	pFile = NULL;
	pConn->Close();
	delete pConn;
	pConn = NULL;

	if (strData.Find(_T("\"Ret\": 0")) == -1)
	{
		//::MessageBox(NULL, _T("发送失败"), _T(""), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	//必须再请求一次
	CString strVal;
	int nCount;
	int i;
	time(&tm);
	GetLocalTime(&st);
	strObj.Format(_T("/cgi-bin/mmwebwx-bin/webwxsync?sid=%s&r=%ld%d%d"),
		m_strSid, tm, st.wSecond, st.wMilliseconds);
	strJson.Format(_T("{\"BaseRequest\":{\"Uin\":%s,\"Sid\":\"%s\"},\"SyncKey\":{\"Count\":4,\"List\":[{\"Key\":1,\"Val\":%s},{\"Key\":2,\"Val\":%s},{\"Key\":3,\"Val\":%s}]}}"),
		m_strMyUin, m_strSid, m_strVal1, m_strVal2, m_strVal3);
	strHeaders += _T("Accept:application/json, text/javascript, */*; q=0.01\r\n");
	strHeaders += _T("Content-Type:application/json; charset=utf-8\r\n");
	strHeaders += _T("Referer:https://wx.qq.com/cgi-bin/mmwebwx-bin/webwxreadtemplate?t=chat#chat\r\n");
	strHeaders += _T("Accept-Language:zh-cn\r\n");
	strHeaders += _T("User-Agent:Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0)\r\n");
	strHeaders += _T("Connection:Keep-Alive\r\n");
	strHeaders += _T("Cache-Control:no-cache");
	try{
		pConn = m_pSession->GetHttpConnection(_T("wx.qq.com"));
		pFile = pConn->OpenRequest(CHttpConnection::HTTP_VERB_POST, strObj);
		pFile->SendRequest(strHeaders, (LPVOID)(LPCTSTR)strJson, strJson.GetLength());
	}
	catch(CInternetException*pException){
		pException->Delete();
	}
	try{
		while (pFile->Read(&ch, 1))
		{
			strData += ch;
		}
	}
	catch(CInternetException*pException){
		pException->Delete();
	}
	pFile->Close();
	delete pFile;
	pFile = NULL;
	pConn->Close();
	delete pConn;
	pConn = NULL;

	nCount = 0;
	i = 0;
	while (i = strData.Find(_T("\"Val\": "), i))
	{
		for (; strData.GetAt(i) != '\n'; i++)
		{
			strVal += strData.GetAt(i);
		}
		strVal.Replace(_T("\"Val\": "), _T(""));
		nCount++;
		if(nCount == 1)
		{
			m_strVal1  = strVal;
		}
		if(nCount == 2)
		{
			m_strVal2  = strVal;
		}
		if(nCount == 3)
		{
			m_strVal3 = strVal;
			break;
		}
		strVal.Empty();
	}
	return TRUE;
}

BOOL CWeixin::SendImg(LPCTSTR lpszContact, LPCTSTR lpszFilePath)
{
	//读取文件
	CFile file;
	if (!file.Open(lpszFilePath, CFile::modeRead | CFile::typeBinary))
	{
		return FALSE;
	}
	ULONG ulFileSize = 0UL;
	char ch;
	CString strFileData;
	while (file.Read(&ch, 1))
	{
		strFileData += ch;
		ulFileSize++;
	}
	file.Close();

	if (0 == ulFileSize)
	{
		return FALSE;
	}

	//得到时间
	time_t tm;
	time(&tm);
	SYSTEMTIME st;
	::GetLocalTime(&st);

	//这个请求我也不知道干啥用的
	//********************
	CString strTestObj;
	strTestObj.Format(_T("/cgi-bin/mmwebwx-bin/webwxuploadmedia?f=json&un=%s&skey=%s&wxuin=%s"),
		lpszContact, m_strSkey, m_strMyUin);
	CHttpConnection *pTestConn;
	CHttpFile *pTestFile;
	try{
		pTestConn = m_pSession->GetHttpConnection(_T("file.wx.qq.com"), 
			INTERNET_FLAG_NO_COOKIES, INTERNET_INVALID_PORT_NUMBER, NULL, NULL);
		pTestFile = pTestConn->OpenRequest(_T("OPTIONS"), strTestObj,
			NULL, 1, NULL, NULL, INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_NO_COOKIES);
	}
	catch(CInternetException*pException){
		pException->Delete();
	}
	pTestFile->AddRequestHeaders(_T("Connection: keep-alive"));
	pTestFile->AddRequestHeaders(_T("Access-Control-Request-Method: POST"));
	pTestFile->AddRequestHeaders(_T("Origin: https://wx.qq.com"));
	pTestFile->AddRequestHeaders(_T("User-Agent: Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0)"));
	pTestFile->AddRequestHeaders(_T("Access-Control-Request-Headers: origin, content-type"));
	pTestFile->AddRequestHeaders(_T("Accept: */*"));
	pTestFile->AddRequestHeaders(_T("Accept-Language: zh-CN,zh;q=0.8"));
	pTestFile->AddRequestHeaders(_T("Accept-Charset: GBK,utf-8;q=0.7,*;q=0.3"));
	pTestFile->SendRequest();
	/*	char c;
	CString str;
	while (pTestFile->Read(&c, 1))
	{
	str += c;
	}
	AfxMessageBox(str);*/
	pTestConn->Close();
	delete pTestConn;
	pTestConn = NULL;
	pTestFile->Close();
	delete pTestFile;
	pTestFile = NULL;
	//**********************

	//上传文件
	CString strBase;
	strBase.Format(_T("{\"BaseRequest\":{\"Uin\":%s,\"Sid\":\"%s\",\"Skey\":\"%s\",\"DeviceID\":\"e829323553833479\"},\"ClientMediaId\":\"%ld%d\",\"TotalLen\":%ld,\"StartPos\":0,\"DataLen\":%ld,\"MediaType\":4}\r\n"),
		m_strMyUin, m_strSid, m_strSkey, tm, st.wMilliseconds, ulFileSize, ulFileSize);

	CString strFilePath(lpszFilePath);
	CString strFileName = strFilePath.Right(strFilePath.GetLength() - strFilePath.ReverseFind('\\') - 1);

	CString strPost;
	strPost = _T("------WebKitFormBoundarymMaoFyBatK4loov7\r\n");
	strPost += _T("Content-Disposition: form-data; name=\"uploadmediarequest\"\r\n\r\n");
	strPost += strBase;
	strPost += _T("------WebKitFormBoundarymMaoFyBatK4loov7\r\n");
	strPost += _T("Content-Disposition: form-data; name=\"filename\"; filename=\"") + strFileName + _T("\"\r\n");
	strPost += _T("Content-Type: image/jpeg\r\n\r\n");
	strPost += strFileData + _T("\r\n\r\n");
	strPost += _T("------WebKitFormBoundarymMaoFyBatK4loov7\r\n");

	CString strObj;
	strObj.Format(_T("/cgi-bin/mmwebwx-bin/webwxuploadmedia?f=json&un=%s&skey=%s&wxuin=%s"), 
		lpszContact, m_strSkey, m_strMyUin);
	CHttpConnection *pConn;
	CHttpFile *pFile = NULL;
	try{
		pConn = m_pSession->GetHttpConnection(_T("file.wx.qq.com"), 
			INTERNET_FLAG_NO_COOKIES, INTERNET_INVALID_PORT_NUMBER, NULL, NULL);
		pFile = pConn->OpenRequest(CHttpConnection::HTTP_VERB_POST, strObj,
			NULL, 1, NULL, NULL, INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_NO_COOKIES);
	}
	catch(CInternetException*pException){
		pException->Delete();
	}
	pFile->AddRequestHeaders(_T("Connection: keep-alive"));
	pFile->AddRequestHeaders(_T("Origin: https://wx.qq.com"));
	pFile->AddRequestHeaders(_T("User-Agent: Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0)"));
	pFile->AddRequestHeaders(_T("Content-Type: multipart/form-data;  boundary=----WebKitFormBoundarymMaoFyBatK4loov7"));
	pFile->AddRequestHeaders(_T("Accept: */*"));
	pFile->AddRequestHeaders(_T("Accept-Language: zh-CN,zh;q=0.8"));
	pFile->AddRequestHeaders(_T("Accept-Charset: GBK,utf-8;q=0.7,*;q=0.3"));
	pFile->SendRequest(NULL, 0, (LPVOID)(LPCTSTR)strPost, strPost.GetLength());

	CString strData;
	try{
		while (pFile->Read(&ch, 1))
		{
			strData += ch;
		}
	}
	catch(CInternetException*pException){
		pException->Delete();
	}
	pConn->Close();
	delete pConn;
	pConn = NULL;
	pFile->Close();
	delete pFile;
	pFile = NULL;

	if (strData.Find(_T("\"Ret\": 0")) == -1)
	{
		return FALSE;
	}
	CString strMediaID;
	for (int i = strData.Find("\"MediaId\":") + 12; strData.GetAt(i) != '\"'; i++)
	{
		strMediaID += strData.GetAt(i);
	}
	//通知对方接收文件
	time(&tm);
	::GetLocalTime(&st);
	strObj.Format(_T("/cgi-bin/mmwebwx-bin/webwxsendmsgimg?fun=async&f=json&r=%ld%d"), tm, st.wMilliseconds);

	CString strPost1, strPost2, strPost3;
	strPost1.Format(_T("{\"Msg\":{\"FromUserName\":\"%s\",\"MediaId\":\"%s\",\"ToUserName\":\"%s\","),
		m_strMyUserName, strMediaID, lpszContact);
	strPost2.Format(_T("\"Type\":3,\"LocalID\":\"%ld%d\"},"), 
		tm, st.wMilliseconds);
	strPost3.Format(_T("\"BaseRequest\":{\"Uin\":%s,\"Sid\":\"%s\",\"Skey\":\"%s\",\"DeviceID\":\"e589189251714280\"}}"),
		m_strMyUin, m_strSid, m_strSkey);
	strPost = strPost1 + strPost2 + strPost3;
	try{
		pConn = m_pSession->GetHttpConnection(_T("wx.qq.com"),
			INTERNET_FLAG_SECURE, INTERNET_INVALID_PORT_NUMBER, NULL, NULL);
		pFile = pConn->OpenRequest(CHttpConnection::HTTP_VERB_POST, strObj,
			NULL, 1, NULL, NULL, INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_SECURE);
	}
	catch(CInternetException*pException){
		pException->Delete();
	}
	pFile->AddRequestHeaders(_T("Accept: application/json, text/javascript, */*; q=0.01"));
	pFile->AddRequestHeaders(_T("Content-Type: application/json; charset=utf-8"));
	pFile->AddRequestHeaders(_T("X-Requested-With: XMLHttpRequest"));
	pFile->AddRequestHeaders(_T("Referer: https://wx.qq.com/?&lang=zh_CN"));
	pFile->AddRequestHeaders(_T("Accept-Language: zh-cn"));
	pFile->AddRequestHeaders(_T("User-Agent: Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0)"));
	pFile->AddRequestHeaders(_T("Connection: Keep-Alive"));
	pFile->AddRequestHeaders(_T("Cache-Control: no-cache"));
	pFile->SendRequest(NULL, 0, (LPVOID)(LPCTSTR)strPost, strPost.GetLength());

	strData.Empty();
	try{
		while (pFile->Read(&ch, 1))
		{
			strData += ch;
		}
	}
	catch(CInternetException*pException){
		pException->Delete();
	}
	pConn->Close();
	delete pConn;
	pConn = NULL;
	pFile->Close();
	delete pFile;
	pFile = NULL;

	if (strData.Find(_T("\"Ret\": 0")) == -1)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

UINT CWeixin::PeekMsg(WXMSG& wxMsg)
{
	time_t tm;
	SYSTEMTIME st;
	time(&tm);
	GetLocalTime(&st);
	CString strObj, strHeaders, strJson;
	strObj.Format(_T("/cgi-bin/mmwebwx-bin/webwxsync?sid=%s&r=%ld%d%d"),
		m_strSid, tm, st.wSecond, st.wMilliseconds);
	strJson.Format(_T("{\"BaseRequest\":{\"Uin\":%s,\"Sid\":\"%s\"},\"SyncKey\":{\"Count\":4,\"List\":[{\"Key\":1,\"Val\":%s},{\"Key\":2,\"Val\":%s},{\"Key\":3,\"Val\":%s}]}}"),
		m_strMyUin, m_strSid, m_strVal1, m_strVal2, m_strVal3);
	strHeaders += _T("Accept:application/json, text/javascript, */*; q=0.01\r\n");
	strHeaders += _T("Content-Type:application/json; charset=utf-8\r\n");
	strHeaders += _T("Referer:https://wx.qq.com/cgi-bin/mmwebwx-bin/webwxreadtemplate?t=chat#chat\r\n");
	strHeaders += _T("Accept-Language:zh-cn\r\n");
	strHeaders += _T("User-Agent:Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0)\r\n");
	strHeaders += _T("Connection:Keep-Alive\r\n");
	strHeaders += _T("Cache-Control:no-cache");
	CHttpConnection *pConn;
	CHttpFile *pFile = NULL;
	try{
		pConn = m_pSession->GetHttpConnection(_T("wx.qq.com"));
		pFile = pConn->OpenRequest(CHttpConnection::HTTP_VERB_POST, strObj);
		pFile->SendRequest(strHeaders, (LPVOID)(LPCTSTR)strJson, strJson.GetLength());
	}
	catch(CInternetException*pException){
		pException->Delete();
	}
	CString strData;
	char ch;
	try{
		while(pFile->Read(&ch, 1))
		{
			strData += ch;
		}
	}
	catch(CInternetException*pException){
		pException->Delete();
	}
	pFile->Close();
	delete pFile;
	pConn->Close();
	delete pConn;
	int nCount = 0;
	int i = 0;
	CString strVal;
	while(i = strData.Find(_T("\"Val\": "), i))
	{
		for(; strData.GetAt(i) != '\n'; i++)
		{
			strVal += strData.GetAt(i);
		}
		strVal.Replace(_T("\"Val\": "), _T(""));
		nCount++;
		if(nCount == 1)
		{
			m_strVal1  = strVal;
		}
		if(nCount == 2)
		{
			m_strVal2  = strVal;
		}
		if(nCount == 3)
		{
			m_strVal3 = strVal;
			break;
		}
		/*	if(nCount == 4)
		{
		strVal1000  = strVal;
		break;
		}*/
		strVal.Empty();
	}

	//分析消息
	CString strType;
	CString strUserName, strMsg;
	for(i = strData.Find(_T("\"FromUserName\":")) + 17; strData.GetAt(i) != '\"'; i++)
	{
		strUserName += strData.GetAt(i);
	}
	for(i = strData.Find(_T("\"MsgType\":")) + 11; strData.GetAt(i) != ','; i++)
	{
		strType += strData.GetAt(i);
	}
	for(i = strData.Find(_T("\"Content\":")) + 12; strData.GetAt(i) != '\"'; i++)
	{
		strMsg += strData.GetAt(i);
	}
	UTF8ToGBK(strMsg);

	wxMsg.strUserName = strUserName;
	TRACE(strMsg + _T("\r\n"));
	SendMsg("wxid_wv1so37or1ya21","RECEIVED.\r\n");
	SendMsg("hanqiong9347","RECEIVED.\r\n");
	return (UINT)atoi(strType);
}