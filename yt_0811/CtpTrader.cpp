// CtpTrader.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CtpTrader.h"
#include "UserMsg.h"

// CCtpTrader

IMPLEMENT_DYNCREATE(CCtpTrader, CWinThread)

CCtpTrader::CCtpTrader()
{
}

CCtpTrader::~CCtpTrader()
{
}

BOOL CCtpTrader::InitInstance()
{
	//��ʼ������ϵͳ����
	memset(&m_brokerId, 0, sizeof(m_brokerId));
	memset(&m_investorId, 0, sizeof(m_investorId));
	memset(&m_password, 0, sizeof(m_password));
	memset(&m_instrumentId,0,sizeof(m_instrumentId));
	//��ʼ������ǰ�õ�ַ
	/*
	char frontAddrTrade[NUM_FRONT_TRADE][60] = {
		"tcp://ctp1-front5.citicsf.com:41205",
		"tcp://ctp1-front7.citicsf.com:41205",
		"tcp://ctp1-front13.citicsf.com:41205",
		"tcp://115.238.108.184:41205",
		"tcp://ctp1-front11.citicsf.com:41205",
		"tcp://ctp1-front12.citicsf.com:41205",
		"tcp://ctp1-front13.citicsf.com:41205",
		"tcp://121.15.139.123:41205",
		"tcp://ctp1-front1.citicsf.com:41205",
		"tcp://ctp1-front3.citicsf.com:41205",
		"tcp://ctp1-front13.citicsf.com:41205",
		"tcp://180.169.101.177:41205",
		"tcp://ctp1-front6.citicsf.com:41205",
		"tcp://ctp1-front8.citicsf.com:41205",
		"tcp://ctp1-front13.citicsf.com:41205",
		"tcp://221.12.30.12:41205",
		"tcp://ctp1-front9.citicsf.com:41205",
		"tcp://ctp1-front10.citicsf.com:41205",
		"tcp://ctp1-front13.citicsf.com:41205",
		"tcp://123.124.247.8:41205",
		"tcp://ctp1-front2.citicsf.com:41205",
		"tcp://ctp1-front4.citicsf.com:41205",
		"tcp://ctp1-front13.citicsf.com:41205",
		"tcp://27.115.78.193:41205"
	};// ����ϵͳǰ�õ�ַ
	*/
	char frontAddrTrade[NUM_FRONT_TRADE][60] = {
		"tcp://ctp1-front1.citicsf.com:41205",
		"tcp://ctp1-front3.citicsf.com:41205",
		"tcp://180.169.101.177:41205"
	};
	for(int i = 0;i < NUM_FRONT_TRADE;i++){
		memset(&m_frontAddrTrade[i], 0, sizeof(m_frontAddrTrade[i]));
		strcpy(m_frontAddrTrade[i],frontAddrTrade[i]);
	}
	m_pUserApiTrade = CThostFtdcTraderApi::CreateFtdcTraderApi();// ����UserApi
	m_pUserApiTrade->RegisterSpi(this);			// ע���¼���
	m_pUserApiTrade->SubscribePublicTopic(TERT_RESTART);					// ע�ṫ����
	m_pUserApiTrade->SubscribePrivateTopic(TERT_RESTART);					// ע��˽����
	for(int i = 0;i < NUM_FRONT_TRADE;i++){
		m_pUserApiTrade->RegisterFront(m_frontAddrTrade[i]);							
	}
	return TRUE;
}

int CCtpTrader::ExitInstance()
{
	// TODO: �ڴ�ִ���������߳�����
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CCtpTrader, CWinThread)
	ON_THREAD_MESSAGE(WM_INIDATA_CTP_TD,remoteIniData)
END_MESSAGE_MAP()

// CCtpTrader ��Ϣ�������
void CCtpTrader::remoteIniData(UINT wParam,LONG lParam){
	TRACE("Զ�����ý��׺�Լ\r\n");
	strcpy(m_brokerId,((ParamCtpTd *)wParam)->brokerId);
	strcpy(m_instrumentId,((ParamCtpTd *)wParam)->instrumentId);
	strcpy(m_investorId,((ParamCtpTd *)wParam)->investorId);
	strcpy(m_password,((ParamCtpTd *)wParam)->password);
	// connect
	m_pUserApiTrade->Init();
}
void CCtpTrader::OnFrontConnected(void){
	TRACE("���̽���ϵͳǰ�����ӳɹ�...\r\n");
	ReqUserLoginCtpTrade();
}
void CCtpTrader::ReqUserLoginCtpTrade(void)
{
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, m_brokerId);
	strcpy(req.UserID, this->m_investorId);
	strcpy(req.Password, m_password);
	int newRequestId = ::InterlockedIncrement((long*)(&m_iRequestId));
	int iResult = m_pUserApiTrade->ReqUserLogin(&req,newRequestId);
}
void CCtpTrader::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		// ����Ự����
		m_frontIdTrade = pRspUserLogin->FrontID;
		m_sessionIdTrade = pRspUserLogin->SessionID;
		TRACE("ǰ��ID=%d\r\n",m_frontIdTrade);
		TRACE("�ỰID=%d\r\n",m_sessionIdTrade);
		m_maxOrderRef = atoi(pRspUserLogin->MaxOrderRef);
		///Ͷ���߽�����ȷ��
		CThostFtdcSettlementInfoConfirmField req;
		memset(&req, 0, sizeof(req));
		strcpy(req.BrokerID, m_brokerId);
		strcpy(req.InvestorID, m_investorId);
		int newRequestId = ::InterlockedIncrement((long*)(&m_iRequestId));
		int iResult = m_pUserApiTrade->ReqSettlementInfoConfirm(&req, newRequestId);
	}
	else{
		TRACE("���̽���ϵͳ��½ʧ��!\r\n");
	}
}
void CCtpTrader::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		TRACE("����ȷ�Ͻ���...\r\n");
		//��ѯ����
		ReqQryTradingAccount();
		ReqQryInvestorPosition();
		ReqQryInstrument();
	}
	else{
		TRACE("����ȷ��ʧ��!\r\n");
	}
}
void CCtpTrader::ReqQryInstrument(void)
{
	CThostFtdcQryInstrumentField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.InstrumentID, m_instrumentId);
	int newRequestId = ::InterlockedIncrement((long*)(&m_iRequestId));
	int iResult =  m_pUserApiTrade->ReqQryInstrument(&req, newRequestId);
}
void CCtpTrader::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	if (bIsLast && !IsErrorRspInfo(pRspInfo)){
		m_LongMarginRatio = pInstrument->LongMarginRatio + 0.03;
		m_ShortMarginRatio = pInstrument->ShortMarginRatio + 0.03;
		TRACE("��֤�������ʼ��\r\n");
	}
}
void CCtpTrader::ReqQryTradingAccount()
{
	TRACE("�����˻���ѯָ��\r\n");
	CThostFtdcQryTradingAccountField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, m_brokerId);
	strcpy(req.InvestorID, m_investorId);
	int newRequestId = ::InterlockedIncrement((long*)(&m_iRequestId));
	int iResult = m_pUserApiTrade->ReqQryTradingAccount(&req,newRequestId);
}
void CCtpTrader::OnRspQryTradingAccount(CThostFtdcTradingAccountField * pTradingAccount, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	TRACE("�����˻���ѯ���\r\n");
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
			if(pTradingAccount != NULL){
				m_availCtp0 = pTradingAccount->Available;
			}
			else{
				m_availCtp0 = 0;
			}
			TRACE("Ctp�����ʽ�%.f\r\n",m_availCtp0);
	}
}
void CCtpTrader::ReqQryInvestorPosition()
{
	CThostFtdcQryInvestorPositionField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, m_brokerId);
	strcpy(req.InvestorID, m_investorId);
	strcpy(req.InstrumentID, m_instrumentId);
	int newRequestId = ::InterlockedIncrement((long*)(&m_iRequestId));
	int iResult = m_pUserApiTrade->ReqQryInvestorPosition(&req,newRequestId);
}
void CCtpTrader::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
			if(pInvestorPosition != NULL){	

				switch(pInvestorPosition->PosiDirection){
				case THOST_FTDC_PD_Long:
					m_portfolioCtp0 = pInvestorPosition->Position;
					break;
				case THOST_FTDC_PD_Short:
					m_portfolioCtp0 = -pInvestorPosition->Position;
					break;
				}
			}
			else{
				m_portfolioCtp0 = 0;			
			}
			TRACE("��Ctp����%d\r\n",m_portfolioCtp0);
	}
}
bool CCtpTrader::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// ���ErrorID != 0, ˵���յ��˴������Ӧ
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	return bResult;
}