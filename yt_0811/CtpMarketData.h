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
	CCtpMarketData();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
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
	// �˻�����	
	TThostFtdcBrokerIDType	m_brokerId;
	TThostFtdcInvestorIDType m_investorId;
	TThostFtdcPasswordType	m_password;
	TThostFtdcInstrumentIDType m_instrumentId;
	//��Լ����	
	int m_iInstrumentID;
	// UserApi����
	CThostFtdcMdApi* pUserApiMD;
	void ReqUserLogout(void);
	//Զ������
	void remoteReqUserLogin(UINT wParam,LONG lParam);
	void remoteReqUserLogout(UINT wParam,LONG lParam);
	void remoteSubMarketData(UINT wParam,LONG lParam);
	void remoteIniData(UINT wParam,LONG lParam);
	bool m_isFrontConnectedMd;
	//������Ӧ
public:
	///����Ӧ��
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast);

	///���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á���������������API���Զ��������ӣ��ͻ��˿ɲ�������
	///@param nReason ����ԭ��
	///        0x1001 �����ʧ��
	///        0x1002 ����дʧ��
	///        0x2001 ����������ʱ
	///        0x2002 ��������ʧ��
	///        0x2003 �յ�������
	virtual void OnFrontDisconnected(int nReason);

	///������ʱ���档����ʱ��δ�յ�����ʱ���÷��������á�
	///@param nTimeLapse �����ϴν��ձ��ĵ�ʱ��
	virtual void OnHeartBeatWarning(int nTimeLapse);

	///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
	virtual void OnFrontConnected();

	///��¼������Ӧ
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	///�ǳ�������Ӧ
	virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};
	///��������Ӧ��
	virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///ȡ����������Ӧ��
	virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///�������֪ͨ
	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);
	//
	bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);
	int m_iRequestId;
};


