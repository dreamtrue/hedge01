/***********************************************************
CWeixin class
Version 1.1

������ȡ��ϵ����ϸ��Ϣ���ͻ�ȡͷ��

��Դ��ֻ���о���ѧϰ���벻Ҫ������ҵ��;��
ת����ע��������
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
//��ϵ��
typedef struct tagWXCONTACT
{
	CString strUin;			//΢�ź�
	CString strUserName;	//�û���
	CString strNickName;	//�ǳ�
	CString strRemarkName;	//��ע��
	int		iSex;			//�Ա�0-δ֪��1-�У�2-Ů��
	CString strSignature;	//ǩ��
	CString strProvince;	//��ַ-ʡ
	CString strCity;		//��ַ-��
	HGLOBAL hHeadImg;		//ͷ��
	int		ihiSize;		//ͷ���С���ֽڣ�
} WXCONTACT;

//΢����Ϣ
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
	void Attach(HWND hWnd);										//���ù�������
	HGLOBAL GetQRCode();						 				//�õ���¼��ά�루�ڴ棩
	BOOL GetQRCode(LPCTSTR lpszPath);							//�õ���¼��ά�루�ļ���
	void GotoScan();											//׼��ɨ��
	BOOL QueryContacts(BOOL bHeadImg = TRUE);					//������ϵ���б�
	int GetContanctsCount();									//�õ���ϵ������
	WXCONTACT GetContanctAt(int nIndex);						//�õ�ָ����ϵ��
	BOOL SendMsg(LPCTSTR lpszUserName, LPCTSTR lpszMsg);		//�����ı�
	BOOL SendImg(LPCTSTR lpszUserName, LPCTSTR lpszFilePath);	//����ͼƬ

private:
	static UINT CheckScanProc(LPVOID pParam);		//����ά�뱻ɨ��
	static UINT CheckOKProc(LPVOID pParam);			//���ȷ�ϵ�¼
	static UINT SyncProc(LPVOID pParam);			//��������״̬����ʱδ�õ���
	static UINT CheckMsgProc(LPVOID pParam);		//����Ƿ�����Ϣ
	void UTF8ToGBK(CString& strTarget);				//UTF8 ת GBK
	void GBKToUTF8(CString& strTarget);				//GBK ת UTF8
	void GetSelfInfo();								//�õ��Լ�����Ϣ
	UINT PeekMsg(WXMSG& wxMsg);						//ȡ����Ϣ

private:
	HWND m_hWnd;
	CInternetSession *m_pSession;
	CString m_strCode, m_strSid, m_strSkey;
	CPtrArray m_paContacts;
	CString m_strMyUin, m_strMyUserName;
	CString m_strVal1, m_strVal2, m_strVal3;
};

#endif