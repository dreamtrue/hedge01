/***********************************************************
CWeixin class
Version 1.1

新增获取联系人详细信息，和获取头像。

本源码只供研究和学习，请不要用于商业用途。
转账请注明出处。
						zzjconfig@qq.com
								2013-3-10
***********************************************************/
#include "stdafx.h"
#include "..\IBTRADE\resource.h"
#include <Gdiplus.h>
#include "afxsock.h"
#include <afxinet.h> 
#include "..\UserMsg.h"
#pragma comment(lib, "Gdiplus.lib")
using namespace Gdiplus;
#ifndef _WEIXIN_
#define _WEIXIN_
//联系人
typedef struct tagWXCONTACT
{
	CString strUin;			//微信号
	CString strUserName;	//用户名
	CString strNickName;	//昵称
	CString strRemarkName;	//备注名
	int		iSex;			//性别（0-未知，1-男，2-女）
	CString strSignature;	//签名
	CString strProvince;	//地址-省
	CString strCity;		//地址-市
	HGLOBAL hHeadImg;		//头像
	int		ihiSize;		//头像大小（字节）
} WXCONTACT;

//微信消息
typedef struct tagWXMSG
{
	CString strUserName;
	CString strMsg;
} WXMSG;

class CWeixin
{
public:
	CWeixin();
	CWeixin(CWeixin& wxin);
	~CWeixin();
	void Attach(HWND hWnd);										//设置关联窗口
	HGLOBAL GetQRCode();						 				//得到登录二维码（内存）
	BOOL GetQRCode(LPCTSTR lpszPath);							//得到登录二维码（文件）
	void GotoScan();											//准备扫描
	BOOL QueryContacts(BOOL bHeadImg = TRUE);					//请求联系人列表
	int GetContanctsCount();									//得到联系人数量
	WXCONTACT GetContanctAt(int nIndex);						//得到指定联系人
	BOOL SendMsg(LPCTSTR lpszUserName, LPCTSTR lpszMsg);		//发送文本
	BOOL SendImg(LPCTSTR lpszUserName, LPCTSTR lpszFilePath);	//发送图片

private:
	static UINT CheckScanProc(LPVOID pParam);		//检测二维码被扫描
	static UINT CheckOKProc(LPVOID pParam);			//检测确认登录
	static UINT SyncProc(LPVOID pParam);			//保持在线状态（暂时未用到）
	static UINT CheckMsgProc(LPVOID pParam);		//检测是否有消息
	void UTF8ToGBK(CString& strTarget);				//UTF8 转 GBK
	void GBKToUTF8(CString& strTarget);				//GBK 转 UTF8
	void GetSelfInfo();								//得到自己的信息
	UINT PeekMsg(WXMSG& wxMsg);						//取出消息

private:
	HWND m_hWnd;
	CInternetSession *m_pSession;
	CString m_strCode, m_strSid, m_strSkey;
	CPtrArray m_paContacts;
	CString m_strMyUin, m_strMyUserName;
	CString m_strVal1, m_strVal2, m_strVal3;
};

#endif