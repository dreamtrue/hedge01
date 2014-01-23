#pragma once
#include "ThostFtdcTraderApi.h"
#define NUM_FRONT_TRADE 3 //����ǰ�õ�ַ����
class ParamCtpTd{
public:
	TThostFtdcBrokerIDType	brokerId;
	TThostFtdcInvestorIDType investorId;
	TThostFtdcPasswordType	password;
	TThostFtdcInstrumentIDType instrumentId;
};
// CCtpTrader
//��ʱδʹ�ø���
class CCtpTrader : public CWinThread,public CThostFtdcTraderSpi

{
	DECLARE_DYNCREATE(CCtpTrader)

protected:
	CCtpTrader();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CCtpTrader();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	DECLARE_MESSAGE_MAP()
public:
	TThostFtdcBrokerIDType	m_brokerId;
	TThostFtdcInvestorIDType m_investorId;
	TThostFtdcPasswordType	m_password;
	TThostFtdcInstrumentIDType m_instrumentId;
	//���̽��׻Ự����
	char m_frontAddrTrade[NUM_FRONT_TRADE][60];
	TThostFtdcFrontIDType	m_frontIdTrade;	//����ǰ�ñ��
	TThostFtdcSessionIDType	m_sessionIdTrade;	//���׻Ự��� 
	//TThostFtdcOrderRefType	m_ordefRef;  //��������
	int m_iRequestId;
	int m_maxOrderRef;
	double m_LongMarginRatio;
	double m_ShortMarginRatio;
	double m_availCtp0;
	int m_portfolioCtp0;
	CThostFtdcTraderApi *m_pUserApiTrade;
	void remoteIniData(UINT wParam,LONG lParam);
	void ReqUserLoginCtpTrade(void);
	void ReqQryInstrument(void);
	bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);
	void ReqQryTradingAccount();
	void ReqQryInvestorPosition();
public:
	virtual void OnFrontConnected(void);
	virtual void CCtpTrader::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField * pTradingAccount, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast);
	virtual void CCtpTrader::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
};


