#pragma once
#include "ThostFtdcMdApi.h"
#include "..\UserMsg.h"
#include "index.h"
class ParamCtpMd{
public:
	TThostFtdcBrokerIDType	brokerId;
	TThostFtdcInvestorIDType investorId;
	TThostFtdcPasswordType	password;
	TThostFtdcInstrumentIDType instrumentId;
};
// CCtpMarketData
class CCtpMarketData : public CWinThread,public CThostFtdcMdSpi
{
	DECLARE_DYNCREATE(CCtpMarketData)

protected:
	CCtpMarketData();           // 动态创建所使用的受保护的构造函数
	virtual ~CCtpMarketData();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
protected:
	DECLARE_MESSAGE_MAP()
public:
	int StartIfPrice(void);
	void ReqUserLogin(void);
public:
	// 账户参数	
	TThostFtdcBrokerIDType	m_brokerId;
	TThostFtdcInvestorIDType m_investorId;
	TThostFtdcPasswordType	m_password;
	TThostFtdcInstrumentIDType m_instrumentId;
	//合约参数	
	int m_iInstrumentID;
	// UserApi对象
	CThostFtdcMdApi* pUserApiMD;
	void ReqUserLogout(void);
	//远程请求
	void remoteReqUserLogin(UINT wParam,LONG lParam);
	void remoteReqUserLogout(UINT wParam,LONG lParam);
	void remoteSubMarketData(UINT wParam,LONG lParam);
	void remoteIniData(UINT wParam,LONG lParam);
	bool m_isFrontConnectedMd;
	//请求响应
public:
	///错误应答
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast);

	///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
	///@param nReason 错误原因
	///        0x1001 网络读失败
	///        0x1002 网络写失败
	///        0x2001 接收心跳超时
	///        0x2002 发送心跳失败
	///        0x2003 收到错误报文
	virtual void OnFrontDisconnected(int nReason);

	///心跳超时警告。当长时间未收到报文时，该方法被调用。
	///@param nTimeLapse 距离上次接收报文的时间
	virtual void OnHeartBeatWarning(int nTimeLapse);

	///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
	virtual void OnFrontConnected();

	///登录请求响应
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	///登出请求响应
	virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};
	///订阅行情应答
	virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///取消订阅行情应答
	virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///深度行情通知
	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);
	//
	bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);
	int m_iRequestId;
};


