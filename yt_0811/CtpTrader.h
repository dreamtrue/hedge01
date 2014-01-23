#pragma once
#include "ThostFtdcTraderApi.h"
#define NUM_FRONT_TRADE 3 //交易前置地址数量
class ParamCtpTd{
public:
	TThostFtdcBrokerIDType	brokerId;
	TThostFtdcInvestorIDType investorId;
	TThostFtdcPasswordType	password;
	TThostFtdcInstrumentIDType instrumentId;
};
// CCtpTrader
//暂时未使用该类
class CCtpTrader : public CWinThread,public CThostFtdcTraderSpi

{
	DECLARE_DYNCREATE(CCtpTrader)

protected:
	CCtpTrader();           // 动态创建所使用的受保护的构造函数
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
	//内盘交易会话参数
	char m_frontAddrTrade[NUM_FRONT_TRADE][60];
	TThostFtdcFrontIDType	m_frontIdTrade;	//交易前置编号
	TThostFtdcSessionIDType	m_sessionIdTrade;	//交易会话编号 
	//TThostFtdcOrderRefType	m_ordefRef;  //报单引用
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


