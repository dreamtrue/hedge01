#if !defined(AFX_CLIENT2DLG_H__681373C1_0AC0_11D6_B0ED_00B0D074179C__INCLUDED_)
#define AFX_CLIENT2DLG_H__681373C1_0AC0_11D6_B0ED_00B0D074179C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "EWrapper.h"       // '<INSTALL_DIR>\SocketClient\include' must be added to include path
#include "HScrollListBox.h" // '<INSTALL_DIR>\Shared' must be added to include path
#include <memory>
#include "..\weixin\picdlg.h"
#include "..\weixin\weixin.h"
#include "..\CtpMarketData.h"
#include "thostftdctraderapi.h"
#include "afxwin.h"
#include "Order.h"
#include <list>
#include <fstream>
#include <map>
#include <list>
#include "ctptrader.h"
#define NUM_FRONT_TRADE 3 //交易前置地址数量
#define NUM_FRONT_MD 24 //行情前置地址数量
class CTradingDataCtp{
public:
	//*************************************
	CThostFtdcInputOrderField req;//请求命令
	//******************************************
	//参考价格,作为强行成交或者止损的依据
	double priceRef;
	double avgFilledPrice;//平均成交价格
	//状态变化
	int volumeFilled;//已成交张数
	bool isNeedNewOrder;//在cancel后检查此项,决定是否重新下单
	bool expired;//过期
	int volumeEnd;//因为取消等原因导致合约结束时完成的成交量
	int waitStatus;//等待下单状态
	TThostFtdcOrderSysIDType sysId;
	TThostFtdcFrontIDType	frontIdTrade;	//交易前置编号
	TThostFtdcSessionIDType	sessionIdTrade;	//交易会话编号
	int requestId;
public:
	void reset();
	CTradingDataCtp::CTradingDataCtp(TThostFtdcBrokerIDType brokerId,TThostFtdcInvestorIDType investorId,TThostFtdcInstrumentIDType instrumentId);
};
class CTradingDataIb{
	public:
	//*************************************
	Order IbOrder;//请求命令
	//******************************************
	double avgFilledPrice;//平均成交价格
	//状态变化
	int volumeFilled;//已成交张数
	bool expired;//过期
	int ladderRef;//参考梯级
	int waitStatus;//只有初始下单前才会有此状态,0代表正常,1代表等待下单，2代表等待改单,3代表等待取消订单
public:
	CTradingDataIb();
	void reset();
};
class EClient;
class CHScrollListBox;

struct Contract;
struct ContractDetails;
struct Order;
struct OrderState;
struct Execution;
struct ExecutionFilter;
struct ScannerSubscription;
struct CommissionReport;

class CDlgOrder;
class CDlgMktDepth;
#define NOT_AN_FA_ACCOUNT_ERROR 321
//ADD NEW
/////////////////////////////////////////////////////////////////////////////
// CClient2Dlg dialog
class CClient2Dlg : public CDialog, public EWrapper ,public CThostFtdcTraderSpi
{
// Construction
public:
	CClient2Dlg(CWnd* pParent = NULL);	// standard constructor
	~CClient2Dlg();
 
    void cancelMktDepth(int tickerId);
    bool isFinancialAdvisor()     { return m_financialAdvisor;}

	static void CStringReplace(CString &pXml, const CString &cStr, const CString &rStr, CString &pDest);
	static const CString RETURN_NEWLINE;
	static const CString RETURN;
	static const CString NEWLINE;
	static const CString TAB;
	static const CString FOUR_SPACES;

// Dialog Data
	//{{AFX_DATA(CClient2Dlg)
	enum { IDD = IDD_CLIENT2_DIALOG };
	CHScrollListBox	    m_orderStatus;
	CHScrollListBox	    m_ticks;
    CHScrollListBox     m_errors;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClient2Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL


// Implementation
private:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CClient2Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnConnect();
	afx_msg void OnReqMktData();
	afx_msg void OnCancelMktData();
	afx_msg void OnWhatIf();
	afx_msg void OnPlaceOrder();
	afx_msg void OnCancelOrder();
	afx_msg void OnDisconnect();
	afx_msg void OnClear();
	afx_msg void OnExtord();
	afx_msg void OnReqOpenOrders();
	afx_msg void OnReqAccountUpdate();
	afx_msg void OnReqExecutions();
	afx_msg void OnReqIds();
	afx_msg void OnReqContractDetails();
	afx_msg void OnReqMktDepth();
	afx_msg void OnCancelMktDepth();
	afx_msg void OnNewsBulletins();
	afx_msg void OnSetServerLogLevel();
	afx_msg void OnReqAllOpenOrders();
	afx_msg void OnReqAutoOpenOrders();
	afx_msg void OnReqAccts();
	afx_msg void OnFinancialAdvisor();
	afx_msg void OnReqHistoricalData();
	afx_msg void OnExerciseOptions();
	afx_msg void OnCancelHistData();
	afx_msg void OnReqRealTimeBars();
    afx_msg void OnCancelRealTimeBars();
	afx_msg void OnReqCurrentTime();
	afx_msg void OnMarketScanner();
	afx_msg void OnCalculateImpliedVolatility();
	afx_msg void OnCalculateOptionPrice();
	afx_msg void OnCancelCalculateImpliedVolatility();
	afx_msg void OnCancelCalculateOptionPrice();
	afx_msg void OnReqGlobalCancel();
	afx_msg void OnReqMarketDataType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// handle events from client

	void tickPrice( TickerId ddeId, TickType field, double price, int canAutoExecute);
	void tickSize( TickerId ddeId, TickType field, int size);
	void tickOptionComputation( TickerId ddeId, TickType field, double impliedVol,
		double delta, double optPrice, double pvDividend,
		double gamma, double vega, double theta, double undPrice);
    void tickGeneric(TickerId tickerId, TickType tickType, double value);
    void tickString(TickerId tickerId, TickType tickType, const CString& value);
	void tickEFP(TickerId tickerId, TickType tickType, double basisPoints,
		const CString& formattedBasisPoints, double totalDividends, int holdDays,
		const CString& futureExpiry, double dividendImpact, double dividendsToExpiry);
	void orderStatus( OrderId orderId, const CString &status, int filled, int remaining, 
        double avgFillPrice, int permId, int parentId, double lastFillPrice,
		int clientId, const CString& whyHeld);
	void openOrder( OrderId orderId, const Contract&, const Order&, const OrderState&);
	void openOrderEnd();
	void winError( const CString &str, int lastError);
	void connectionClosed();
	void updateAccountValue(const CString &key, const CString &val,
		const CString &currency, const CString &accountName);
	virtual void updatePortfolio( const Contract& contract, int position,
		double marketPrice, double marketValue, double averageCost,
		double unrealizedPNL, double realizedPNL, const CString &accountName);
	void updateAccountTime(const CString &timeStamp);
	void accountDownloadEnd(const CString &accountName);
	void nextValidId( OrderId orderId);
	void contractDetails( int reqId, const ContractDetails& contractDetails);
	void bondContractDetails( int reqId, const ContractDetails& contractDetails);
	void contractDetailsEnd( int reqId);
    void execDetails( int reqId, const Contract& contract, const Execution& execution);
	void execDetailsEnd( int reqId);
	void error(const int id, const int errorCode, const CString errorString);
	void error(const CString errorString);
	void updateMktDepth( TickerId id, int position, int operation, int side, 
			double price, int size);
	void updateMktDepthL2( TickerId id, int position, CString marketMaker, int operation, 
			int side, double price, int size);
	void updateNewsBulletin(int msgId, int msgType, const CString& newsMessage, const CString& originExch);
    void managedAccounts(const CString& accountsList);
    void receiveFA(faDataType pFaDataType, const CString& cxml);
    void historicalData(TickerId reqId, const CString& date, double open, double high, double low,
                      double close, int volume, int barCount, double WAP, int hasGaps) ;
	void scannerParameters(const CString &xml);
	void scannerData(int reqId, int rank, const ContractDetails &contractDetails, const CString &distance,
		const CString &benchmark, const CString &projection, const CString &legsStr);
	void scannerDataEnd(int reqId);
	void realtimeBar(TickerId reqId, long time, double open, double high, double low, double close,
	   long volume, double wap, int count);
	void currentTime(long time);
	void fundamentalData(TickerId reqId, const CString& data);
	void deltaNeutralValidation(int reqId, const UnderComp& underComp);
	void tickSnapshotEnd( int reqId);
	void marketDataType( TickerId reqId, int marketDataType);
	void commissionReport( const CommissionReport& commissionReport);

private:

	// trace
	void trace( const char *str);
	void trace( int val);
	void trace( double val);

	// helpers
	void placeOrder(bool whatIf);

private:

	static void DisplayMultiline(CHScrollListBox&, const CString& title, const CString& xml);

private:

	struct PropPrintHelpers;

private:

	std::auto_ptr<CDlgOrder> m_dlgOrder;
	std::auto_ptr<CDlgMktDepth> m_dlgMktDepth;

	std::auto_ptr<ExecutionFilter> m_execFilter;
	std::auto_ptr<ScannerSubscription> m_scannerSubscr;

private:

	EClient     *m_pClient;
	bool        m_financialAdvisor;    
	CString     m_managedAccounts;
	CString faGroupsXML;
	CString faProfilesXML;
	CString faAliasesXML;
	bool faError ;
	//ADD NEW FUNCTIONS
public:
    afx_msg void OnSetupMM();
	CPicDlg m_dlgPic;
	CWeixin m_WXin;
	int SendWeixinMsg(LPCTSTR lpMsg);
	CCtpMarketData *m_pCtpMarketDataThread;
	//内盘账户参数
public: 
	TThostFtdcBrokerIDType	m_brokerId;
	TThostFtdcInvestorIDType m_investorId;
	TThostFtdcPasswordType	m_password;
	TThostFtdcInstrumentIDType m_instrumentId;
	// 内盘交易会话参数
	char m_frontAddrTrade[NUM_FRONT_TRADE][60];
	TThostFtdcFrontIDType	m_frontIdTrade;	//交易前置编号
	TThostFtdcSessionIDType	m_sessionIdTrade;	//交易会话编号 
	//TThostFtdcOrderRefType	m_ordefRef;  //报单引用
	CThostFtdcTraderApi *m_pUserApiTrade;

	virtual void OnFrontConnected(void);
	void ReqUserLoginCtpTrade(void);
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	int m_maxOrderRef;
	bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);
	virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	int m_iRequestId;
	afx_msg void OnConnectHedgeSys();
	afx_msg void OnLbnSelchangeList2();
	void SetContractIb(void);
	void SetContractCtp(void);
	bool m_isIbReady;
	afx_msg void OnDisconnectHedgeSys();
	virtual void OnFrontDisconnected(int nReason);
	bool m_isFrontConnectedCtpTrade;
	afx_msg void HedgeLoopSetupAndIni();
	bool m_isHedgeLoopSetUp;
	LRESULT HedgeLoop(UINT wParam, long lParam);
	double m_availIb0;
	double m_availCtp0;
	bool m_isCtpReady;
	void ReqQryTradingAccount(void);
	void ReqQryInvestorPosition();
	void ReqQryInvestorPositionDetail();
	int m_portfolioIb0;
	int m_portfolioCtp0;
    std::list<CTradingDataCtp> m_CTradingDataCtp;//记录CTP报单状态
	//ctp虚函数
	bool isMyOrder(CThostFtdcOrderField *pOrder);
	///报单录入(错误,正确时不响应)请求响应
	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField * pTradingAccount, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryInstrumentMarginRate(CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryInvestorPositionDetail(CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspQryTrade(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	///报单录入错误回报
	virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo);
	virtual void OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo);
	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);
	virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);
	///错误应答
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	void ReqQryInstrument(void);
	double m_LongMarginRatio;
	double m_ShortMarginRatio;
	bool m_isUpdateMarginRatioCtp;
	void ReqOrderInsert(CTradingDataCtp *pOrderCtp);
	double m_ifAsk10;
	double m_ifBid10;
	WORD m_lifeA50;
	WORD m_lifeIf;
	double m_deviationSell;
	double m_deviationBuy;
	public:
		void CalDeviation();
		double m_dltInterest;
		double m_datumDiff;
		double m_deviationAvg;
		int m_multiA50;
		std::list<CTradingDataIb> m_CTradingDataIb;
		double m_netCtp;
		int m_portfolioCtp;
		double m_availCtp;
		double m_MarginA50;
		double m_RMBtoUSD;
		bool m_isIni;
		double m_netIb;
		double m_a50Bid10;
		double m_a50Ask10;
		double m_valuePointCtp;
		int m_tradingPortfolioBuyCtp;
		int m_tradingPortfolioSellCtp;
		int m_tradingPortfolioBuyOpenCtp;
		int m_tradingPortfolioBuyCloseCtp;
		int m_tradingPortfolioSellOpenCtp;
		int m_tradingPortfolioSellCloseCtp;
		double m_netCtp0;
		double m_netIb0;
		int m_portfolioIb;
		int m_tradingPortfolioSellIb;
		int m_tradingPortfolioBuyIb;
		double m_valuePointIb;
		double dealA50Price(bool isbuy, double A50Price);
		int ReqDeleteOrder(CTradingDataCtp * pOrder);
		std::map<CString,std::list<CThostFtdcTradeField>> m_ifRtn;
		afx_msg void OnBnClickedUpdate();
		CStatic m_a50Days;
		CStatic m_ifDays;
		CStatic m_A50IndexLabel;
		CStatic m_HS300IndexLabel;
		CStatic m_A50Bid1Label;
		CStatic m_A50Ask1Label;
		CStatic m_ifBid1Label;
		CStatic m_ifAsk1Label;
		CStatic m_deviationLabel;
		CStatic m_a50PortLabel;
		CStatic m_ifPortLabel;
		CStatic m_ibNetLabel;
		CStatic m_CtpNetLabel;
		CStatic m_totalLabel;
		CStatic m_A50BuyLabel;
		CStatic m_A50SellLabel;
		CStatic m_IFBuyClose;
		CStatic m_IFBuyOpen;
		CStatic m_IFSellClose;
		CStatic m_IFSellOpen;
		afx_msg void OnPause();
		afx_msg void OnContinue();
		bool m_isPause;
		CButton m_PauseButton;
		int m_aimMulti;
		int m_ladderMulti;
		CButton m_ConnectButton;
		CCtpTrader * m_pCtpTraderThread;
		CIndex *m_pIndexThread;
		//消息转发响应
		LRESULT OnRtnOrderMsg(UINT wParam, long lParam);
		LRESULT OnRtnTradeMsg(UINT wParam, long lParam);
		LRESULT OnRspOrderInsertMsg(UINT wParam, long lParam);
		LRESULT OnErrRtnOrderInsertMsg(UINT wParam, long lParam);
		LRESULT OnRspErrorTransfer(UINT wParam, long lParam);
		LRESULT OnAllSysIni(UINT wParam, long lParam);
		void ReqQryDepthMarketData(void);
		void ReqQryTrade(void);
		double m_preSettlementPriceCtp;
		void ReqQryInstrumentMarginRate(void);
		bool m_isUpdateSettlementPriceCtp;
		bool m_isQryInstrumentCtp;
		bool m_isNetIniCtp;
		int m_portfolioCtpYd;
		bool m_isPorliIniCtp;
		afx_msg void OnBnClickedButton10();
		bool m_isReqTradeCtp;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLIENT2DLG_H__681373C1_0AC0_11D6_B0ED_00B0D074179C__INCLUDED_)