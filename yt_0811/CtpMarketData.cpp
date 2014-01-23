//CtpMarketData.cpp:ʵ���ļ�
//
#include "stdafx.h"
#include "CtpMarketData.h"
// CCtpMarketData
#define NUM_ADD_MD 3
/*
char FRONT_ADDR_MD[NUM_ADD_MD][60] = {
	"tcp://ctp1-md5.citicsf.com:41213",
	"tcp://ctp1-md7.citicsf.com:41213",
	"tcp://ctp1-md13.citicsf.com:41213",
	"tcp://115.238.108.184:41213",
	"tcp://ctp1-md11.citicsf.com:41213",
	"tcp://ctp1-md12.citicsf.com:41213",
	"tcp://ctp1-md13.citicsf.com:41213",
	"tcp://121.15.139.123:41213",
	"tcp://ctp1-md1.citicsf.com:41213",
	"tcp://ctp1-md3.citicsf.com:41213",
	"tcp://ctp1-md13.citicsf.com:41213",
	"tcp://180.169.101.177:41213",
	"tcp://ctp1-md6.citicsf.com:41213",
	"tcp://ctp1-md8.citicsf.com:41213",
	"tcp://ctp1-md13.citicsf.com:41213",
	"tcp://221.12.30.12:41213",
	"tcp://ctp1-md9.citicsf.com:41213",
	"tcp://ctp1-md10.citicsf.com:41213",
	"tcp://ctp1-md13.citicsf.com:41213",
	"tcp://123.124.247.8:41213",
	"tcp://ctp1-md2.citicsf.com:41213",
	"tcp://ctp1-md4.citicsf.com:41213",
	"tcp://ctp1-md13.citicsf.com:41213",
	"tcp://27.115.78.193:41213"
};//ǰ�õ�ַ
*/
char FRONT_ADDR_MD[NUM_ADD_MD][60] = {
	"tcp://ctp1-md1.citicsf.com:41213",
	"tcp://ctp1-md3.citicsf.com:41213",
	"tcp://180.169.101.177:41213"
};//ǰ�õ�ַ
double g_ifBid1,g_ifAsk1;
extern int getIndexData();
IMPLEMENT_DYNCREATE(CCtpMarketData, CWinThread)

	CCtpMarketData::CCtpMarketData()
	: m_isFrontConnectedMd(false)
	, m_iRequestId(100)
{
	memset(&m_brokerId, 0, sizeof(m_brokerId));
	memset(&m_investorId, 0, sizeof(m_investorId));
	memset(&m_password, 0, sizeof(m_password));
	memset(&m_instrumentId,0,sizeof(m_instrumentId));
}

CCtpMarketData::~CCtpMarketData()
{
}

BOOL CCtpMarketData::InitInstance()
{
	pUserApiMD = CThostFtdcMdApi::CreateFtdcMdApi();
	pUserApiMD->RegisterSpi((CThostFtdcMdSpi*)this);
	//��ǰ��ע��
	for(int i = 0;i < NUM_ADD_MD;i++){
		pUserApiMD->RegisterFront(FRONT_ADDR_MD[i]);							
	}
	// connect
	//pUserApiMD->Init();
	//pUserApiMD->Join();
	TRACE("ctp����ϵͳע�����\r\n");
	//ɾ������,���������,����ʾǰ�öϿ�;
	//������ֱ�ӵ���Init��������RegisterSpi;
	//����pUerApiMd����û�б��ָ��,ֻ�ǲ����κ���Ӧ,����������Ȼ���ܻ����쳣�׳�
	//pUserApiMD->Release();
	return TRUE;
}

int CCtpMarketData::ExitInstance()
{
	TRACE("Ctp�����˳�\r\n");
	//pUserApiMD->RegisterSpi(NULL);
	pUserApiMD->Release();
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CCtpMarketData, CWinThread)
	ON_THREAD_MESSAGE(WM_LOGIN_CTP_MD,remoteReqUserLogin)
	ON_THREAD_MESSAGE(WM_LOGOUT_CTP_MD,remoteReqUserLogout)
	ON_THREAD_MESSAGE(WM_SUBMARKETDATA_CTP_MD,remoteSubMarketData)
	ON_THREAD_MESSAGE(WM_INIDATA_CTP_MD,remoteIniData)
END_MESSAGE_MAP()
//Զ��������
void CCtpMarketData::remoteIniData(UINT wParam,LONG lParam){
	TRACE("Զ�����������˻��ͺ�Լ����\r\n");
	strcpy(m_brokerId,((ParamCtpMd *)wParam)->brokerId);
	strcpy(m_instrumentId,((ParamCtpMd *)wParam)->instrumentId);
	strcpy(m_investorId,((ParamCtpMd *)wParam)->investorId);
	strcpy(m_password,((ParamCtpMd *)wParam)->password);
	// connect
	pUserApiMD->Init();
}
void CCtpMarketData::remoteReqUserLogin(UINT wParam,LONG lParam){
	TRACE("Զ�������½ctp����ϵͳ\r\n");
	while(true){
		Sleep(1000);//�ȴ�ǰ�����ӳɹ�
		if(m_isFrontConnectedMd){
			break;
		}
	}
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, m_brokerId);
	strcpy(req.UserID, m_investorId);
	strcpy(req.Password, m_password);
	int newRequestId = ::InterlockedIncrement((long*)(&m_iRequestId));
	int iResult = pUserApiMD->ReqUserLogin(&req, newRequestId);
}
void CCtpMarketData::remoteReqUserLogout(UINT wParam,LONG lParam){
	TRACE("Զ������ǳ�ctp����ϵͳ\r\n");
	CThostFtdcUserLogoutField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, m_brokerId);
	strcpy(req.UserID, m_investorId);
	int newRequestId = ::InterlockedIncrement((long*)(&m_iRequestId));
	int iResult = pUserApiMD->ReqUserLogout(&req, newRequestId);
}
void CCtpMarketData::remoteSubMarketData(UINT wParam,LONG lParam){
	TRACE("Զ�̶�������\r\n");
	char *ppInstrumentID[] = {m_instrumentId};
	m_iInstrumentID = 1;
	int iResult = pUserApiMD->SubscribeMarketData(ppInstrumentID, m_iInstrumentID);
}

// CCtpMarketData ��Ϣ�������

void CCtpMarketData::ReqUserLogin(void)
{
	/*
	while(true){
		Sleep(1000);//�ȴ�ǰ�����ӳɹ�
		if(m_isFrontConnectedMd){
			break;
		}
	}
	*/
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, m_brokerId);
	strcpy(req.UserID, m_investorId);
	strcpy(req.Password, m_password);
	int newRequestId = ::InterlockedIncrement((long*)(&m_iRequestId));
	int iResult = pUserApiMD->ReqUserLogin(&req,newRequestId);
	if(iResult == 0){
		TRACE("ctp����ϵͳ��½ָ��ͳɹ�\r\n");
	}
	else{
		TRACE("ctp����ϵͳ��½ָ���ʧ��\r\n");
	}
}

void CCtpMarketData::ReqUserLogout(void)
{
	CThostFtdcUserLogoutField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, m_brokerId);
	strcpy(req.UserID, m_investorId);
	int newRequestId = ::InterlockedIncrement((long*)(&m_iRequestId));
	int iResult = pUserApiMD->ReqUserLogout(&req, newRequestId);
	if(iResult == 0){
		TRACE("ctp����ϵͳ�ǳ�ָ��ͳɹ�\r\n");
	}
	else{
		TRACE("ctp����ϵͳ�ǳ�ָ���ʧ��\r\n");
	}
}
//�����Ӧ
void CCtpMarketData::OnRspError(CThostFtdcRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast)
{
	IsErrorRspInfo(pRspInfo);
	TRACE("ctp����ϵͳ����%s\r\n",pRspInfo->ErrorMsg);
}

void CCtpMarketData::OnFrontDisconnected(int nReason)
{
	//m_isFrontConnectedMd = false;
	TRACE("ctp����ϵͳǰ�öϿ�\r\n");
}

void CCtpMarketData::OnHeartBeatWarning(int nTimeLapse)
{
	TRACE("�����澯\r\n");
}

void CCtpMarketData::OnFrontConnected()
{
	TRACE("ctp����ϵͳǰ�����ӳɹ�\r\n");
	//m_isFrontConnectedMd = true;
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, m_brokerId);
	strcpy(req.UserID, m_investorId);
	strcpy(req.Password, m_password);
	int newRequestId = ::InterlockedIncrement((long*)(&m_iRequestId));
	int iResult = pUserApiMD->ReqUserLogin(&req, newRequestId);
}
void CCtpMarketData::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		TRACE("ctp����ϵͳ��½�ɹ�\r\n");
		char *ppInstrumentID[] = {m_instrumentId};
		m_iInstrumentID = 1;
		int iResult = pUserApiMD->SubscribeMarketData(ppInstrumentID, m_iInstrumentID);
	}
	else{
		TRACE("ctp����ϵͳ��½ʧ��\r\n");
	}
}
void CCtpMarketData::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		TRACE("ctp���鶩�ĳɹ�\r\n");
	}
	else{

		TRACE("ctp���鶩��ʧ��\r\n");
	}
}
void CCtpMarketData::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{

}
void CCtpMarketData::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	/*
	if(g_ifBid1 != pDepthMarketData->BidPrice1 || g_ifAsk1 != pDepthMarketData->AskPrice1){
	getIndexData();
	//TRACE("%f %f\r\n",g_ifBid1,g_ifAsk1);
	}
	*/
	if(g_ifBid1 != pDepthMarketData->BidPrice1){
		g_ifBid1 = pDepthMarketData->BidPrice1 + 0.1;
		g_ifAsk1 = pDepthMarketData->BidPrice1 + 0.1;
		PostMessage(GetMainWnd()->m_hWnd,WM_PRICE_CHANGED,NULL,NULL);
	}
	if(g_ifAsk1 != pDepthMarketData->AskPrice1){
		g_ifBid1 = pDepthMarketData->AskPrice1 - 0.1;
		g_ifAsk1 = pDepthMarketData->AskPrice1 - 0.1;
		PostMessage(GetMainWnd()->m_hWnd,WM_PRICE_CHANGED,NULL,NULL);
	}
}
bool CCtpMarketData::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	//���ErrorID != 0, ˵���յ��˴������Ӧ
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	return bResult;
}
