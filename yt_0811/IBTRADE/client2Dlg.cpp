#include "stdafx.h"
#include "client2.h"
#include "client2Dlg.h"
#include "EClientSocket.h"   // C:\JTS\SocketClient\include must be added to include path
#include "TwsSocketClientErrors.h"   // C:\JTS\SocketClient\include must be added to include path
#include "DlgOrder.h"
#include "DlgExtOrd.h"
#include "DlgAccount.h"
#include "DlgConnect.h"
#include "DlgMktDepth.h"
#include "Contract.h"
#include "Execution.h"
#include "ScannerSubscription.h"
#include "DlgNewsBulletins.h"
#include "DlgLogConfig.h"
#include "RptFilterDlg.h"
#include "AcctUpdatesDlg.h"
#include "DlgFinancialAdvisor.h"
#include "DlgScanner.h"
#include "CommissionReport.h"
#include "resource.h"
#include "OrderState.h"
#include "..\Common.h"
#include "calendar.h"
#include "..\Index.h"
#include "..\CtpTrader.h"
#undef _DEBUG
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
std::fstream fout;//�����־
std::fstream fout1;//ctp��־
//HANDLE g_hFrontTradeConnected;//����ǰ�����ӳɹ��ź�
double g_a50Bid1 = 0.0f,g_a50Ask1 = 0.0f;
int g_a50Bid1Size = 0,g_a50Ask1Size = 0;
extern double g_ifBid1,g_ifAsk1;
double g_A50Index,g_HS300Index;
ParamCtpMd ParamMdCtp;//ctp����ϵͳ��ʼ����,�����˺š���Լ�ȵ�
ParamCtpTd ParamTdCtp;
extern int getIndexData();
CTradingDataCtp::CTradingDataCtp(TThostFtdcBrokerIDType brokerId,TThostFtdcInvestorIDType investorId,TThostFtdcInstrumentIDType instrumentId){
	memset(&req,0,sizeof(req));
	memset(&sysId,0,sizeof(sysId));
	//�������
	strcpy(req.BrokerID,brokerId);
	///Ͷ���ߴ���
	strcpy(req.InvestorID,investorId);
	///��Լ����
	strcpy(req.InstrumentID,instrumentId);
	///�û�����
	//	TThostFtdcUserIDType	UserID;
	///�����۸�����: �޼�
	req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
	///���Ͷ���ױ���־
	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	///��Ч������: ������Ч
	req.TimeCondition = THOST_FTDC_TC_GFD;
	//GTD����
	//TThostFtdcDateType	GTDDate;
	///�ɽ�������: �κ�����
	req.VolumeCondition = THOST_FTDC_VC_AV;
	///��С�ɽ���: 1
	req.MinVolume = 1;
	///��������: ����
	req.ContingentCondition = THOST_FTDC_CC_Immediately;
	///ֹ���
	// TThostFtdcPriceType	StopPrice;
	///ǿƽԭ��: ��ǿƽ
	req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	///�Զ������־: ��
	req.IsAutoSuspend = 0;
	///ҵ��Ԫ
	//	TThostFtdcBusinessUnitType	BusinessUnit;
	///������
	//	TThostFtdcRequestIDType	RequestID;
	///�û�ǿ����־: ��
	req.UserForceClose = 0;
	//*************************************************
	//���������
	priceRef = 0;
	volumeFilled = 0;
	expired = false;//Ĭ��δ����
	isNeedNewOrder = false;
	avgFilledPrice = 0;
	volumeEnd = 0;
	waitStatus = 0;
	frontIdTrade = 0;	//����ǰ�ñ��
	sessionIdTrade = 0;	//���׻Ự���
	requestId = 0;
}
void CTradingDataCtp::reset(){
	req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
	///���Ͷ���ױ���־
	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	///��Ч������: ������Ч
	req.TimeCondition = THOST_FTDC_TC_GFD;
	//GTD����
	//TThostFtdcDateType	GTDDate;
	///�ɽ�������: �κ�����
	req.VolumeCondition = THOST_FTDC_VC_AV;
	///��С�ɽ���: 1
	req.MinVolume = 1;
	///��������: ����
	req.ContingentCondition = THOST_FTDC_CC_Immediately;
	///ֹ���
	// TThostFtdcPriceType	StopPrice;
	///ǿƽԭ��: ��ǿƽ
	req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	///�Զ������־: ��
	req.IsAutoSuspend = 0;
	///ҵ��Ԫ
	//	TThostFtdcBusinessUnitType	BusinessUnit;
	///������
	//	TThostFtdcRequestIDType	RequestID;
	///�û�ǿ����־: ��
	req.UserForceClose = 0;
	//*************************************************
	//���������
	priceRef = 0;
	volumeFilled = 0;
	expired = false;//Ĭ��δ����
	isNeedNewOrder = false;
	avgFilledPrice = 0;
	volumeEnd = 0;
	waitStatus = 0;
	frontIdTrade = 0;	//����ǰ�ñ��
	sessionIdTrade = 0;	//���׻Ự���
	requestId = 0;
}
CTradingDataIb::CTradingDataIb(){
	avgFilledPrice = 0.0f;//ƽ���ɽ��۸�
	//״̬�仯
	volumeFilled = 0;//�ѳɽ�����
	expired = false;//Ĭ��δ����
	waitStatus = 0;
	//ָ���ʼ��
	IbOrder.orderType = "LMT";
	IbOrder.whatIf = false;
}
void CTradingDataIb::reset(){
	avgFilledPrice = 0.0f;//ƽ���ɽ��۸�
	//״̬�仯
	volumeFilled = 0;//�ѳɽ�����
	expired = false;//Ĭ��δ����
	waitStatus = 0;
	//ָ���ʼ��
	IbOrder.orderType = "LMT";
	IbOrder.whatIf = false;
}
/////////////////////////////////////////////////////////////////////////////
// consts
const int TIMER = 0;
const int N = 5;
const CString CClient2Dlg::RETURN_NEWLINE("\r\n");
const CString CClient2Dlg::NEWLINE("\n");
const CString CClient2Dlg::RETURN("\r");
const CString CClient2Dlg::TAB("\t");
const CString CClient2Dlg::FOUR_SPACES("    ");

// statics
static CDlgAccount      s_accountDlg;
static int              s_maxOrderStatusStrSize;
static CDlgLogConfig    s_dlgLogConfig;

/////////////////////////////////////////////////////////////////////////////
// utility funcs
CString getField( TickType tickType) {
	switch( tickType)
	{
	case BID_SIZE:	                    return "bidSize";
	case BID:		                    return "bidPrice";
	case ASK:		                    return "askPrice";
	case ASK_SIZE:	                    return "askSize";
	case LAST:		                    return "lastPrice";
	case LAST_SIZE:	                    return "lastSize";
	case HIGH:		                    return "high";
	case LOW:		                    return "low";
	case VOLUME:	                    return "volume";
	case CLOSE:		                    return "close";
	case BID_OPTION_COMPUTATION:		return "bidOptComp";
	case ASK_OPTION_COMPUTATION:		return "askOptComp";
	case LAST_OPTION_COMPUTATION:		return "lastOptComp";
	case MODEL_OPTION:					return "optionModel";
	case OPEN:                          return "open";
	case LOW_13_WEEK:                   return "13WeekLow";
	case HIGH_13_WEEK:                  return "13WeekHigh";
	case LOW_26_WEEK:                   return "26WeekLow";
	case HIGH_26_WEEK:                  return "26WeekHigh";
	case LOW_52_WEEK:                   return "52WeekLow";
	case HIGH_52_WEEK:                  return "52WeekHigh";
	case AVG_VOLUME:                    return "AvgVolume";
	case OPEN_INTEREST:                 return "OpenInterest";
	case OPTION_HISTORICAL_VOL:         return "OptionHistoricalVolatility";
	case OPTION_IMPLIED_VOL:            return "OptionImpliedVolatility";
	case OPTION_BID_EXCH:               return "OptionBidExchStr";
	case OPTION_ASK_EXCH:               return "OptionAskExchStr";
	case OPTION_CALL_OPEN_INTEREST:     return "OptionCallOpenInterest";
	case OPTION_PUT_OPEN_INTEREST:      return "OptionPutOpenInterest";
	case OPTION_CALL_VOLUME:            return "OptionCallVolume";
	case OPTION_PUT_VOLUME:             return "OptionPutVolume";
	case INDEX_FUTURE_PREMIUM:          return "IndexFuturePremium";
	case BID_EXCH:                      return "bidExch";
	case ASK_EXCH:                      return "askExch";
	case AUCTION_VOLUME:                return "auctionVolume";
	case AUCTION_PRICE:                 return "auctionPrice";
	case AUCTION_IMBALANCE:             return "auctionImbalance";
	case MARK_PRICE:                    return "markPrice";
	case BID_EFP_COMPUTATION:           return "bidEFP";
	case ASK_EFP_COMPUTATION:           return "askEFP";
	case LAST_EFP_COMPUTATION:          return "lastEFP";
	case OPEN_EFP_COMPUTATION:          return "openEFP";
	case HIGH_EFP_COMPUTATION:          return "highEFP";
	case LOW_EFP_COMPUTATION:           return "lowEFP";
	case CLOSE_EFP_COMPUTATION:         return "closeEFP";
	case LAST_TIMESTAMP:                return "lastTimestamp";
	case SHORTABLE:                     return "shortable";
	case FUNDAMENTAL_RATIOS:            return "fundamentals";
	case RT_VOLUME:                     return "RTVolume";
	case HALTED:                        return "halted";
	case BID_YIELD:                     return "bidYield";
	case ASK_YIELD:                     return "askYield";
	case LAST_YIELD:                    return "lastYield";             
	case CUST_OPTION_COMPUTATION:       return "custOptComp";
	case TRADE_COUNT:                   return "trades";
	case TRADE_RATE:                    return "trades/min";
	case VOLUME_RATE:                   return "volume/min";
	case LAST_RTH_TRADE:                return "lastRTHTrade";
	default:                            return "unknown";
	}
}

#define NUM_FA_ERROR_CODES 6
static int faErrorCodes[NUM_FA_ERROR_CODES] =
{ 503, 504, 505, 522, 1100, NOT_AN_FA_ACCOUNT_ERROR } ;


struct CClient2Dlg::PropPrintHelpers {

	static int Print(CHScrollListBox& listBox, const char* name, bool boolVal)
	{
		CString str;
		str.Format("  %s=%i", name, boolVal);
		return listBox.AddString(str);
	}

	static int Print(CHScrollListBox& listBox, const char* name, int intVal)
	{
		CString str;
		if (intVal == INT_MAX) {
			str.Format("  %s=", name);
		}
		else {
			str.Format("  %s=%i", name, intVal);
		}
		return listBox.AddString(str);
	}

	static int Print(CHScrollListBox& listBox, const char* name, double dblVal)
	{
		CString str;
		if (dblVal == DBL_MAX) {
			str.Format("  %s=", name);
		}
		else {
			str.Format("  %s=%f", name, dblVal);
		}
		return listBox.AddString(str);
	}

	static int Print(CHScrollListBox& listBox, const char* name, const CString& strVal)
	{
		CString str;
		str.Format("  %s=%s", name, strVal);
		return listBox.AddString(str);
	}
};

#define PRINT_PROP(Prop,Value) \
	PropPrintHelpers::Print(m_orderStatus, #Prop, Value);

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClient2Dlg dialog

CClient2Dlg::CClient2Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CClient2Dlg::IDD, pParent)
	, m_dlgOrder(new CDlgOrder(this))
	, m_dlgMktDepth(new CDlgMktDepth(this))
	, m_execFilter(new ExecutionFilter)
	, m_scannerSubscr(new ScannerSubscription)
	, m_maxOrderRef(0)
	, m_iRequestId(100)//��100��ʼ
	, m_isIbReady(false)
	, m_isFrontConnectedCtpTrade(false)
	, m_isHedgeLoopSetUp(false)
	, m_availIb0(0)
	, m_availCtp0(0)
	, m_isCtpReady(false)
	, m_portfolioIb0(0)
	, m_portfolioCtp0(0)
	, m_LongMarginRatio(0.15)
	, m_ShortMarginRatio(0.15)
	, m_isUpdateMarginRatioCtp(false)
	, m_ifAsk10(0)
	, m_ifBid10(0)
	, m_deviationSell(0)
	, m_deviationBuy(0)
	, m_dltInterest(0)
	, m_deviationAvg(0)
	, m_datumDiff(0)
	, m_multiA50(15)//Ĭ�ϳ�����15
	, m_netCtp(0)
	, m_portfolioCtp(0)
	, m_availCtp(0)
	, m_MarginA50(625.0)//A50��֤��,����ֵ
	, m_RMBtoUSD(6.15)//����
	, m_isIni(true)
	, m_netIb(0)
	, m_a50Bid10(0)
	, m_a50Ask10(0)
	, m_valuePointCtp(300.0)//ÿ��ctp��Լ��ļ�ֵ
	, m_tradingPortfolioBuyCtp(0)
	, m_tradingPortfolioSellCtp(0)
	, m_tradingPortfolioBuyOpenCtp(0)
	, m_tradingPortfolioBuyCloseCtp(0)
	, m_tradingPortfolioSellOpenCtp(0)
	, m_tradingPortfolioSellCloseCtp(0)
	, m_netCtp0(0)
	, m_netIb0(0)
	, m_portfolioIb(0)
	, m_tradingPortfolioSellIb(0)
	, m_tradingPortfolioBuyIb(0)
	, m_valuePointIb(1.0)//ÿ��Ib��Լ���ֵ
	, m_isPause(true)
	, m_aimMulti(1)
	, m_ladderMulti(20)
	, m_preSettlementPriceCtp(0)
	, m_isUpdateSettlementPriceCtp(false)
	, m_isQryInstrumentCtp(false)
	, m_isNetIniCtp(false)
	, m_portfolioCtpYd(0)
	, m_isPorliIniCtp(false)
	, m_isReqTradeCtp(false)
{
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	fout.open("HEDGE.log",std::ios::out|std::ios::app);
	fout1.open("CTP.log",std::ios::out|std::ios::app);
	fout << "************************\n" << "��ʼ" << "\n";
	//g_hFrontTradeConnected = CreateSemaphore(NULL, 0, 1, NULL);//��ʼ��
	m_pCtpMarketDataThread = NULL;
	m_pCtpTraderThread = NULL;
	m_pIndexThread = NULL;
	//{{AFX_DATA_INIT(CClient2Dlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pClient = new EClientSocket( this);
	s_maxOrderStatusStrSize = 0;

	m_financialAdvisor = false;

	{
#define PUT_PROP(Prop, Value) \
	m_scannerSubscr->Prop = Value;

		PUT_PROP(numberOfRows, 10);
		PUT_PROP(instrument, "STK");
		PUT_PROP(locationCode, "STK.US.MAJOR");
		PUT_PROP(scanCode, "TOP_PERC_GAIN");
		PUT_PROP(abovePrice, 3);
		PUT_PROP(marketCapAbove, 100000000);
		PUT_PROP(scannerSettingPairs, "Annual,true");

#undef PUT_PROP
	}
	//��ʼ������ϵͳ����
	memset(&m_brokerId, 0, sizeof(m_brokerId));
	memset(&m_investorId, 0, sizeof(m_investorId));
	memset(&m_password, 0, sizeof(m_password));
	memset(&m_instrumentId,0,sizeof(m_instrumentId));
	strcpy(m_brokerId,"66666");
	strcpy(m_investorId,"10127111");
	strcpy(m_password,"003180");
	//�����߳�����
	{
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
	}
}

CClient2Dlg::~CClient2Dlg()
{
	delete m_pClient;
}

void CClient2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CClient2Dlg)
	DDX_Control(pDX, IDC_LIST3, m_orderStatus);
	DDX_Control(pDX, IDC_LIST2, m_ticks);
	DDX_Control(pDX, IDC_LIST1, m_errors);
	DDX_Text(pDX,IDC_DATUMDIFF,m_datumDiff);
	TRACE("��׼����Ϊ%.f\r\n",m_datumDiff);
	DDX_Text(pDX, IDC_EDIT15, m_aimMulti);
	TRACE("Ŀ���������Ϊ%d\r\n",m_aimMulti);
	DDX_Text(pDX, IDC_EDIT16, m_ladderMulti);
	TRACE("��������Ϊ%d\r\n",m_ladderMulti);
	DDX_Text(pDX,IDC_EDIT18,m_multiA50);
	TRACE("A50��������Ϊ%d\r\n",m_multiA50);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_A50_MONTH, m_a50Days);
	DDX_Control(pDX, IDC_IF_DAYS, m_ifDays);
	DDX_Control(pDX, IDC_A50Index, m_A50IndexLabel);
	DDX_Control(pDX, IDC_HS300Index, m_HS300IndexLabel);
	DDX_Control(pDX, IDC_A50Bid1, m_A50Bid1Label);
	DDX_Control(pDX, IDC_A50Ask1, m_A50Ask1Label);
	DDX_Control(pDX, IDC_IFBid1, m_ifBid1Label);
	DDX_Control(pDX, IDC_IFASK1, m_ifAsk1Label);
	DDX_Control(pDX, IDC_DEVIATION, m_deviationLabel);
	DDX_Control(pDX, IDC_A50Potofolio, m_a50PortLabel);
	DDX_Control(pDX, IDC_IFPortofolio, m_ifPortLabel);
	DDX_Control(pDX, IDC_IBNET, m_ibNetLabel);
	DDX_Control(pDX, IDC_CTPNet, m_CtpNetLabel);
	DDX_Control(pDX, IDC_TOTALNET, m_totalLabel);
	DDX_Control(pDX, IDC_A50Buy, m_A50BuyLabel);
	DDX_Control(pDX, IDC_A50Sell, m_A50SellLabel);
	DDX_Control(pDX, IDC_IFBuyClose, m_IFBuyClose);
	DDX_Control(pDX, IDC_IFBuyOpen, m_IFBuyOpen);
	DDX_Control(pDX, IDC_IFSellClose, m_IFSellClose);
	DDX_Control(pDX, IDC_IFSellOpen, m_IFSellOpen);
	DDX_Control(pDX, IDC_CONTINUE, m_PauseButton);
	DDX_Control(pDX, IDC_CONNECT_HEDGESYS, m_ConnectButton);
}

BEGIN_MESSAGE_MAP(CClient2Dlg, CDialog)
	//{{AFX_MSG_MAP(CClient2Dlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON2, OnConnect)
	ON_BN_CLICKED(IDC_BUTTON3, OnReqMktData)
	ON_BN_CLICKED(IDC_BUTTON4, OnCancelMktData)
	ON_BN_CLICKED(IDC_WHAT_IF, OnWhatIf)
	ON_BN_CLICKED(IDC_BUTTON5, OnPlaceOrder)
	ON_BN_CLICKED(IDC_BUTTON6, OnCancelOrder)
	ON_BN_CLICKED(IDC_BUTTON7, OnDisconnect)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	ON_BN_CLICKED(IDC_EXTORD, OnExtord)
	ON_BN_CLICKED(IDC_ReqOpenOrders, OnReqOpenOrders)
	ON_BN_CLICKED(IDC_ACCT_UPDATES, OnReqAccountUpdate)
	ON_BN_CLICKED(IDC_ReqExecutions, OnReqExecutions)
	ON_BN_CLICKED(IDC_ReqIds, OnReqIds)
	ON_BN_CLICKED(IDC_REQ_CONTRACT, OnReqContractDetails)
	ON_BN_CLICKED(IDC_REQ_MKT_DEPTH, OnReqMktDepth)
	ON_BN_CLICKED(IDC_CANCEL_MKT_DEPTH, OnCancelMktDepth)
	ON_BN_CLICKED(IDC_NEWS_BULLETINS, OnNewsBulletins)
	ON_BN_CLICKED(IDC_LOGGING, OnSetServerLogLevel)
	ON_BN_CLICKED(IDC_REQ_ALL_OPEN_ORDERS, OnReqAllOpenOrders)
	ON_BN_CLICKED(IDC_REQ_AUTO_OPEN_ORDERS, OnReqAutoOpenOrders)
	ON_BN_CLICKED(IDC_REQ_ACCTS, OnReqAccts)
	ON_BN_CLICKED(IDC_FINANCIAL_ADVISOR, OnFinancialAdvisor)
	ON_BN_CLICKED(IDC_HISTORICAL_DATA, OnReqHistoricalData)
	ON_BN_CLICKED(IDC_EXERCISE_OPTIONS, OnExerciseOptions)
	ON_BN_CLICKED(IDC_CANCEL_HIST_DATA, OnCancelHistData)
	ON_BN_CLICKED(IDC_REQ_REAL_TIME_BARS, OnReqRealTimeBars)
	ON_BN_CLICKED(IDC_CANCEL_REAL_TIME_BARS, OnCancelRealTimeBars)
	ON_BN_CLICKED(IDC_REQ_CURRENT_TIME, OnReqCurrentTime)
	ON_BN_CLICKED(IDC_MARKET_SCANNER, OnMarketScanner)
	ON_BN_CLICKED(IDC_CALC_IMPL_VOL, OnCalculateImpliedVolatility)
	ON_BN_CLICKED(IDC_CALC_OPTION_PRICE, OnCalculateOptionPrice)
	ON_BN_CLICKED(IDC_CANCEL_CALC_IMPL_VOL, OnCancelCalculateImpliedVolatility)
	ON_BN_CLICKED(IDC_CANCEL_CALC_OPTION_PRICE, OnCancelCalculateOptionPrice)
	ON_BN_CLICKED(IDC_REQ_GLOBAL_CANCEL, OnReqGlobalCancel)
	ON_BN_CLICKED(IDC_REQ_MARKET_DATA_TYPE, OnReqMarketDataType)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_SETUP_MM, &CClient2Dlg::OnSetupMM)
	ON_BN_CLICKED(IDC_CONNECT_HEDGESYS, &CClient2Dlg::OnConnectHedgeSys)
	ON_LBN_SELCHANGE(IDC_LIST2, &CClient2Dlg::OnLbnSelchangeList2)
	ON_BN_CLICKED(IDC_DISCONNECT_HEDGE_SYS, &CClient2Dlg::OnDisconnectHedgeSys)
	ON_BN_CLICKED(IDC_HEDGE_SETUP, &CClient2Dlg::HedgeLoopSetupAndIni)
	ON_MESSAGE(WM_PRICE_CHANGED,HedgeLoop)
	ON_MESSAGE(WM_ORDER_STATUS_CTP,HedgeLoop)
	ON_BN_CLICKED(IDC_UPDATE, &CClient2Dlg::OnBnClickedUpdate)
	ON_BN_CLICKED(IDC_PAUSE, &CClient2Dlg::OnPause)
	ON_BN_CLICKED(IDC_CONTINUE, &CClient2Dlg::OnContinue)
	ON_MESSAGE(WM_ONRTNORDER_TRANSFER,OnRtnOrderMsg)
	ON_MESSAGE(WM_ONRTNTRADE_TRANSFER,OnRtnTradeMsg)
	ON_MESSAGE(WM_OnRspOrderInsert_Transfer,OnRspOrderInsertMsg)
	ON_MESSAGE(WM_OnErrRtnOrderInsert_Transfer,OnErrRtnOrderInsertMsg)
	ON_MESSAGE(WM_OnRspError_Transfer,OnRspErrorTransfer)
	ON_MESSAGE(WM_SYS_INI,OnAllSysIni)
	ON_BN_CLICKED(IDC_BUTTON10, &CClient2Dlg::OnBnClickedButton10)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClient2Dlg message handlers

BOOL CClient2Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX  &0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	m_orderStatus.ModifyStyle( 0, WM_HSCROLL, 0 );

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	return TRUE;
}

void CClient2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID  &0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CClient2Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CClient2Dlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}



/////////////////////////////////////////////////////////////////////////////
// handle user events
void CClient2Dlg::OnConnect()
{
	// IMPORTANT: TWS must be running, and the "Enable Excel Integration"
	// checkbox on the "Settings" menu must be checked!

	// get connection parameters
	CDlgConnect dlg;
	if( dlg.DoModal() == IDCANCEL) {
		return;
	}
	// connect to TWS
	{
		CString displayString;
		displayString.Format( "Connecting to Tws using clientId %d ...", dlg.m_clientId);
		int i = m_orderStatus.AddString( displayString);
		m_orderStatus.SetTopIndex( i);
	}
	m_pClient->eConnect( dlg.m_ipAddress, dlg.m_port, dlg.m_clientId);

	if( m_pClient->serverVersion() > 0)	{
		CString displayString;
		displayString.Format( "Connected to Tws server version %d at %s.",
			m_pClient->serverVersion(), m_pClient->TwsConnectionTime());
		int i = m_orderStatus.AddString( displayString);
		m_orderStatus.SetTopIndex( i);
	}
}

// req mkt data
void CClient2Dlg::OnReqMktData()
{
	// run dlg box
	m_dlgOrder->init( this, "Request Market Data", CDlgOrder::REQ_MKT_DATA, m_managedAccounts);
	if( m_dlgOrder->DoModal() != IDOK) return;

	// request ticker
	m_pClient->reqMktData( m_dlgOrder->m_id, m_dlgOrder->getContract(),
		m_dlgOrder->m_genericTicks, m_dlgOrder->m_snapshotMktData);

}

void CClient2Dlg::OnCancelMktData()
{
	// get ticker id
	m_dlgOrder->init( this, "Cancel Market Data", CDlgOrder::CANCEL_MKT_DATA, m_managedAccounts);
	if( m_dlgOrder->DoModal() != IDOK) return;

	// cancel market data
	m_pClient->cancelMktData( m_dlgOrder->m_id);
}

void CClient2Dlg::OnCancelMktDepth()
{
	// get ticker id
	m_dlgOrder->init( this, "Cancel Market Depth", CDlgOrder::CANCEL_MKT_DEPTH, m_managedAccounts);
	if( m_dlgOrder->DoModal() != IDOK) return;

	// cancel market data
	m_pClient->cancelMktDepth( m_dlgOrder->m_id);
}

void CClient2Dlg::cancelMktDepth(int tickerId)
{
	// cancel market data
	m_pClient->cancelMktDepth( tickerId);
}

void CClient2Dlg::OnWhatIf()
{
	placeOrder(/* whatIf */ true);
}

void CClient2Dlg::OnPlaceOrder()
{
	placeOrder(/* whatIf */ false);
}

void CClient2Dlg::placeOrder(bool whatIf)
{
	// run order box
	m_dlgOrder->init(this, "Place Order", CDlgOrder::ORDER, m_managedAccounts);
	if( m_dlgOrder->DoModal() != IDOK) return;

	Order& order = m_dlgOrder->getOrder();

	// save old and set new value of whatIf attribute
	bool savedWhatIf = order.whatIf;
	order.whatIf = whatIf;

	// place order
	m_pClient->placeOrder( m_dlgOrder->m_id, m_dlgOrder->getContract(), order);

	// restore whatIf attribute
	order.whatIf = savedWhatIf;
}

void CClient2Dlg::OnCancelOrder()
{
	// get order id
	m_dlgOrder->init( this, "Cancel Order", CDlgOrder::CANCEL_ORDER, m_managedAccounts);
	if( m_dlgOrder->DoModal() != IDOK) return;

	// cancel order
	m_pClient->cancelOrder( m_dlgOrder->m_id);
}

void CClient2Dlg::OnReqOpenOrders()
{
	// request open orders
	m_pClient->reqOpenOrders();
}

void CClient2Dlg::OnReqAllOpenOrders()
{
	// request list of all open orders
	m_pClient->reqAllOpenOrders();
}

void CClient2Dlg::OnReqAutoOpenOrders()
{
	// request to automatically bind any newly entered TWS orders
	// to this API client. NOTE: TWS orders can only be bound to
	// client's with clientId=0.
	m_pClient->reqAutoOpenOrders( true);
}

void CClient2Dlg::OnReqAccountUpdate()
{
	//��Ҫ��ȡ��,���������Ч
	CAcctUpdatesDlg dlg;

	if ( dlg.DoModal() != IDOK) return;

	if ( dlg.getSubscribe()) {
		s_accountDlg.accountDownloadBegin(dlg.getAcctCode());
	}

	m_pClient->reqAccountUpdates(dlg.getSubscribe(), dlg.getAcctCode());

	// Show the account details dialog if we are subscribing.
	if ( dlg.getSubscribe() ) {
		s_accountDlg.DoModal();
	}
}

void CClient2Dlg::OnReqExecutions()
{
	CRptFilterDlg dlg(m_execFilter.get());

	if ( dlg.DoModal() != IDOK) return;

	m_pClient->reqExecutions(dlg.reqId(), *m_execFilter);
}

void CClient2Dlg::OnReqContractDetails()
{
	// run dlg box
	m_dlgOrder->init( this, "Request Contract Details", CDlgOrder::REQ_CONTRACT_DETAILS, m_managedAccounts);
	if( m_dlgOrder->DoModal() != IDOK) return;

	// request contract details
	m_pClient->reqContractDetails( m_dlgOrder->m_id, m_dlgOrder->getContract());

}

void CClient2Dlg::OnReqIds()
{
	// request a block of 20 id's;
	// next id is returned via nextValidId()
	TRACE("����20��ID\r\n");
	m_pClient->reqIds( 20);
}

void CClient2Dlg::OnDisconnect()
{
	// disconnect
	m_pClient->eDisconnect();
}

void CClient2Dlg::OnExtord()
{
	CDlgExtOrd dlgExtOrd(&m_dlgOrder->getOrder(), this);
	if( dlgExtOrd.DoModal() != IDOK) return;
	// nothing to do here
}

void CClient2Dlg::OnClear()
{
	// clear list boxes
	m_ticks.ResetContent();
	m_orderStatus.ResetContent();
	m_errors.ResetContent();
}

void CClient2Dlg::OnCancelHistData()
{
	m_dlgOrder->init( this, "Cancel Historical Data", CDlgOrder::CANCEL_HISTORICAL_DATA, m_managedAccounts);
	if( m_dlgOrder->DoModal() != IDOK) return;

	const CString& whatToShow = m_dlgOrder->m_whatToShow;

	if( whatToShow == "estimates" || whatToShow == "finstat" || whatToShow == "snapshot") {
		m_pClient->cancelFundamentalData( m_dlgOrder->m_id);
		return;
	}

	m_pClient->cancelHistoricalData( m_dlgOrder->m_id);
}

void CClient2Dlg::OnReqCurrentTime()
{
	m_pClient->reqCurrentTime();
}

void CClient2Dlg::OnMarketScanner()
{
	CDlgScanner dlgScanner(m_scannerSubscr.get(), m_pClient);
	dlgScanner.DoModal();
}

void CClient2Dlg::OnExerciseOptions()
{
	m_dlgOrder->init( this, "Exercise Options", CDlgOrder::EXERCISE_OPTIONS, m_managedAccounts);
	if( m_dlgOrder->DoModal() != IDOK) return;

	/* TODO: get account in a less convoluted way */
	m_pClient->exerciseOptions(
		m_dlgOrder->m_id,
		m_dlgOrder->getContract(),
		m_dlgOrder->m_exerciseAction,
		m_dlgOrder->m_exerciseQuantity,
		m_dlgOrder->getOrder().account,
		m_dlgOrder->m_exerciseOverride);
}

void CClient2Dlg::OnReqHistoricalData()
{
	m_dlgOrder->init( this, "Request Historical Data", CDlgOrder::REQ_HISTORICAL_DATA, m_managedAccounts);
	if( m_dlgOrder->DoModal() != IDOK) return;

	const CString& whatToShow = m_dlgOrder->m_whatToShow;

	if ( whatToShow == "estimates" || whatToShow == "finstat" || whatToShow == "snapshot") {
		m_pClient->reqFundamentalData( m_dlgOrder->m_id, m_dlgOrder->getContract(), whatToShow);
		return;
	}

	m_pClient->reqHistoricalData( m_dlgOrder->m_id, m_dlgOrder->getContract(),
		m_dlgOrder->m_backfillEndDateTime, m_dlgOrder->m_backfillDuration,
		m_dlgOrder->m_barSizeSetting, whatToShow, m_dlgOrder->m_useRTH,
		m_dlgOrder->m_formatDate);
}

void CClient2Dlg::OnReqRealTimeBars()
{
	m_dlgOrder->init( this, "Request Real Time Bars", CDlgOrder::REQ_REAL_TIME_BARS, m_managedAccounts);
	if( m_dlgOrder->DoModal() != IDOK) return;

	m_pClient->reqRealTimeBars( m_dlgOrder->m_id, m_dlgOrder->getContract(),
		5 /* TODO: parse and use m_dlgOrder->m_barSizeSetting) */,
		m_dlgOrder->m_whatToShow, m_dlgOrder->m_useRTH > 0);
}

void CClient2Dlg::OnCancelRealTimeBars()
{
	m_dlgOrder->init( this, "Cancel Real Time Bars", CDlgOrder::CANCEL_REAL_TIME_BARS, m_managedAccounts);
	if( m_dlgOrder->DoModal() != IDOK) return;

	m_pClient->cancelRealTimeBars( m_dlgOrder->m_id);
}

/////////////////////////////////////////////////////////////////////////////
// handle events from socket

void CClient2Dlg::scannerParameters(const CString &xml)
{
	static CString myTitle("SCANNER PARAMETERS: ") ;
	DisplayMultiline(m_orderStatus, myTitle, xml);
}

void CClient2Dlg::DisplayMultiline(CHScrollListBox& box, const CString& title, const CString& data)
{
	int i = box.AddString(title);

	if (!data.IsEmpty()) {

		const char* const begin = (const char*)data;
		const char* const end   = begin + data.GetLength();

		const char* p = begin;

		for (;;) {

			ASSERT (p != end);

			const char* lineend = (const char*)memchr( p, '\n', end - p);
			if (!lineend)
				lineend = end;

			const char* e = lineend;
			if (e != p) {
				const char* c = e;
				if (*--c == '\r')
					e = c;
			}

			size_t len = e - p;
			bool cut = false;

			if (len > 1024) {
				len = 1024;
				cut = true;
			}

			CString line = data.Mid( p - begin, len);

			if (cut) {
				line += " ...";
			}

			CString expanded;
			CStringReplace( line, TAB, FOUR_SPACES, expanded);

			i = box.AddString( expanded);

			if (lineend >= end || ++lineend >= end)
				break;

			p = lineend;
		}
	}

	int top = i - N < 0 ? 0 : i - N;
	box.SetTopIndex( top);
}

void CClient2Dlg::scannerData(int reqId, int rank,
	const ContractDetails &contractDetails, const CString &distance,
	const CString &benchmark, const CString &projection, const CString &legsStr) {
		Contract contract = contractDetails.summary;
		// create string
		CString str;
		str.Format("id =%i rank=%i conId=%i symbol=%s secType=%s expiry=%s strike=%f right=%s exchange=%s currency=%s localSymbol=%s marketName=%s tradingClass=%s distance=%s benchmark=%s projection=%s legsStr=%s",
			reqId,
			rank,
			contract.conId,
			contract.symbol,
			contract.secType,
			contract.expiry,
			contract.strike,
			contract.right,
			contract.exchange,
			contract.currency,
			contract.localSymbol,
			contractDetails.marketName,
			contractDetails.tradingClass,
			distance,
			benchmark,
			projection,
			legsStr);

		int i =  m_ticks.AddString(str);

		// bring into view
		int top = i - N < 0 ? 0 : i - N;
		m_ticks.SetTopIndex( top);
}

void CClient2Dlg::scannerDataEnd(int reqId)
{
	// create string
	CString str;
	str.Format("id =%i =============== end ===============", reqId);

	int i =  m_ticks.AddString(str);

	// bring into view
	int top = i - N < 0 ? 0 : i - N;
	m_ticks.SetTopIndex( top);
}

void CClient2Dlg::historicalData(TickerId reqId, const CString& date, double open, double high, double low,
	double close, int volume, int barCount, double WAP, int hasGaps)
{
	CString displayString;
	displayString.Format(
		"id=%d date=%s open=%f high=%f low=%f close=%f volume=%d barCount = %d WAP=%f hasGaps=%d",
		reqId, (const char *)date, open, high, low, close, volume, barCount, WAP, hasGaps);
	int i =  m_ticks.AddString(displayString);

	// bring into view
	int top = i - N < 0 ? 0 : i - N;
	m_ticks.SetTopIndex( top);
}

void CClient2Dlg::realtimeBar(TickerId reqId, long time, double open, double high, double low,
	double close, long volume, double WAP, int count)
{
	CString displayString;
	displayString.Format(
		"id=%d time=%d open=%f high=%f low=%f close=%f volume=%d WAP=%f count = %d",
		reqId, time, open, high, low, close, volume, WAP, count);
	int i =  m_ticks.AddString(displayString);

	// bring into view
	int top = i - N < 0 ? 0 : i - N;
	m_ticks.SetTopIndex( top);
}

void CClient2Dlg::fundamentalData(TickerId reqId, const CString& data)
{
	CString title;
	title.Format( "fund data id=%d, len=%d", reqId, data.GetLength());
	DisplayMultiline(m_ticks, title, data);
}

void CClient2Dlg::currentTime(long time)
{
	CString displayString;
	displayString.Format("current time = %d", time);
	int i =  m_orderStatus.AddString(displayString);

	// bring into view
	int top = i - N < 0 ? 0 : i - N;
	m_orderStatus.SetTopIndex( top);
}

// VC++ 5.0 MFC does not have CString.Replace.  So, I use this:

void CClient2Dlg::CStringReplace(CString &pXml, const CString &cStr, const CString &rStr, CString &pDest)
{
	int rlen, len, clen, lenFound ;
	LPTSTR pFind, builtBuf, pBuiltBuf, pBuf ;

	len = pXml.GetLength() ;
	clen = strlen( cStr ) ;
	rlen = strlen( rStr ) ;
	pBuf = pXml.GetBuffer( len );
	// next allocation size will always be large enough!
	builtBuf = pBuiltBuf = (LPTSTR)calloc(1, len * rlen + 1) ;

	do {
		pFind = strstr(pBuf, cStr) ;
		if (pFind == NULL)
			strcat(pBuiltBuf, pBuf) ;
		else {
			lenFound = pFind - pBuf ;
			memcpy(pBuiltBuf, pBuf, lenFound) ;
			memcpy(pBuiltBuf + lenFound, rStr, rlen) ;
			pBuiltBuf += lenFound + rlen ;
			pBuf += lenFound + clen ;
		}
	}
	while (pFind != NULL) ;
	pXml.ReleaseBuffer( );
	pDest = builtBuf ;
	free(builtBuf) ;
}

void CClient2Dlg::receiveFA(faDataType pFaDataType, const CString& cxml)
{
	switch (pFaDataType) {
	case GROUPS:
		faGroupsXML = cxml;
		break ;
	case PROFILES:
		faProfilesXML = cxml;
		break ;
	case ALIASES:
		faAliasesXML = cxml;
		break ;
	}

	CString faStr("FA ");
	faStr += faDataTypeStr ( pFaDataType );
	faStr += ":";
	DisplayMultiline(m_orderStatus, faStr, cxml);

	if (!faError && faGroupsXML != "" && faProfilesXML != "" && faAliasesXML != "") {
		CDlgFinancialAdvisor dlg;
		dlg.receiveInitialXML(faGroupsXML, faProfilesXML, faAliasesXML);

		if (dlg.DoModal() != IDOK) return;

		dlg.extractXML(faGroupsXML, faProfilesXML, faAliasesXML);
		m_pClient->replaceFA( GROUPS, faGroupsXML );
		m_pClient->replaceFA( PROFILES, faProfilesXML );
		m_pClient->replaceFA( ALIASES, faAliasesXML );

	}
}

void CClient2Dlg::tickPrice( TickerId tickerId, TickType tickType, double price, int canAutoExecute)
{
	CString str;
	str.Format( "id=%i  %s=%f  canAutoExecute=%d",
		tickerId, (const char*)getField( tickType), price, canAutoExecute);
	int i = m_ticks.AddString( str);

	int top = i - N < 0 ? 0 : i - N;
	m_ticks.SetTopIndex( top);
}

void CClient2Dlg::tickSize( TickerId tickerId, TickType tickType, int size)
{
	CString str;
	str.Format( "id=%i  %s=%i",
		tickerId, (const char*)getField( tickType), size);
	int i = m_ticks.AddString( str);

	int top = i - N < 0 ? 0 : i - N;
	m_ticks.SetTopIndex( top);
}

void CClient2Dlg::tickGeneric(TickerId tickerId, TickType tickType, double value)
{
	CString str;
	str.Format( "id=%i  %s=%f",
		tickerId, (const char*)getField( tickType), value);
	int i = m_ticks.AddString( str);

	int top = i - N < 0 ? 0 : i - N;
	m_ticks.SetTopIndex( top);
}

void CClient2Dlg::tickString(TickerId tickerId, TickType tickType, const CString& value)
{
	CString str;
	str.Format( "id=%i  %s=%s",
		tickerId, (const char*)getField( tickType), value);
	int i = m_ticks.AddString( str);

	int top = i - N < 0 ? 0 : i - N;
	m_ticks.SetTopIndex( top);
}

void CClient2Dlg::tickEFP(TickerId tickerId, TickType tickType, double basisPoints, const CString& formattedBasisPoints,
	double totalDividends, int holdDays, const CString& futureExpiry, double dividendImpact,
	double dividendsToExpiry)
{
	CString str;
	str.Format( "id=%i  %s: basisPoints=%f / %s impliedFuture=%f holdDays=%i futureExpiry=%s dividendImpact=%f dividendsToExpiry=%f",
		tickerId, (const char*)getField( tickType), basisPoints, formattedBasisPoints, totalDividends, holdDays, futureExpiry, dividendImpact, dividendsToExpiry);
	int i = m_ticks.AddString( str);

	int top = i - N < 0 ? 0 : i - N;
	m_ticks.SetTopIndex( top);
}

void CClient2Dlg::tickOptionComputation( TickerId tickerId, TickType tickType, double impliedVol, double delta, double optPrice, double pvDividend,
	double gamma, double vega, double theta, double undPrice)
{
	CString str, impliedVolStr("N/A"), deltaStr("N/A"), gammaStr("N/A"), vegaStr("N/A"), thetaStr("N/A"), 
		optPriceStr("N/A"), pvDividendStr("N/A"), undPriceStr("N/A");
	if (impliedVol != DBL_MAX) {
		impliedVolStr.Format("%f", impliedVol);
	}
	if (delta != DBL_MAX) {
		deltaStr.Format("%f", delta);
	}
	if (gamma != DBL_MAX) {
		gammaStr.Format("%f", gamma);
	}
	if (vega != DBL_MAX) {
		vegaStr.Format("%f", vega);
	}
	if (theta != DBL_MAX) {
		thetaStr.Format("%f", theta);
	}
	if (optPrice != DBL_MAX) {
		optPriceStr.Format("%f", optPrice);
	}
	if (pvDividend != DBL_MAX) {
		pvDividendStr.Format("%f", pvDividend);
	}
	if (undPrice != DBL_MAX) {
		undPriceStr.Format("%f", undPrice);
	}
	str.Format( "id=%i %s vol=%s delta=%s gamma=%s vega=%s theta=%s optPrice=%s pvDividend=%s undPrice=%s",
		tickerId, (const char*)getField( tickType), impliedVolStr, deltaStr, gammaStr, vegaStr, thetaStr, 
		optPriceStr, pvDividendStr, undPriceStr);

	int i = m_ticks.AddString( str);

	int top = i - N < 0 ? 0 : i - N;
	m_ticks.SetTopIndex( top);
}

void CClient2Dlg::orderStatus( OrderId orderId, const CString &status, int filled, int remaining,
	double avgFillPrice, int permId, int parentId, double lastFillPrice,
	int clientId, const CString& whyHeld)
{
	fout << "orderStatus:" << " ID " << orderId << " status " << status << " filled " << filled
		<< " remaining " << remaining << " avgFillPrice " << avgFillPrice << " permId " << permId
		<<" parentId " << parentId << " lastFillPrice " << lastFillPrice << "\n";
	TRACE("ORDER_STATUS%d\r\n",orderId);
	TRACE("״̬�ر��߳�ID%d\r\n",::GetCurrentThreadId());
	std::list<CTradingDataIb>::iterator iTDIB;
	for(iTDIB = m_CTradingDataIb.begin();iTDIB != m_CTradingDataIb.end();iTDIB++){
		if(iTDIB->IbOrder.orderId == orderId){
			if(iTDIB->expired && iTDIB->waitStatus == 0){
				continue;//��Լ�Ѿ�����,���ٽ����κδ���
			}
			if(iTDIB->volumeFilled < filled){
				iTDIB->volumeFilled = filled;
				iTDIB->avgFilledPrice = avgFillPrice;
				if(iTDIB->volumeFilled == iTDIB->IbOrder.totalQuantity){//�Ѿ�ȫ���ɽ�
					iTDIB->expired = true;
				}
			}
			if(status == CString("Cancelled") || status == CString("ApiCancelled")){
				TRACE("IB����ȡ��,ID = %d\r\n",orderId);
				iTDIB->expired = true;
			}
		}
	}
	::PostMessage(m_hWnd,WM_ORDER_STATUS_CTP,NULL,NULL);//����״̬������֪ͨ
	// create string
	CString str;
	str.Format( "orderId=%i clientId=%i permId=%i status=%s  filled=%i  remaining=%i  avgFillPrice=%f lastFillPrice=%f parentId=%i whyHeld=%s",
		orderId, clientId, permId, (const char *)status, filled, remaining, avgFillPrice, lastFillPrice, parentId, (const char*)whyHeld);

	// add to listbox
	int i = m_orderStatus.AddString( str);

	// move into view
	int top = i - N < 0 ? 0 : i - N;
	m_orderStatus.SetTopIndex( top);
}

void CClient2Dlg::openOrder( OrderId orderId, const Contract& contract,
	const Order& order, const OrderState& orderState)
{
	// create string
	CString str;
	str.Format("openOrder: orderId=%i", orderId);

	// add to listbox
	int i =  m_orderStatus.AddString(str);

	{
#define PRINT_CONTRACT_PROP(Prop) \
	PRINT_PROP(Prop,contract.Prop)

		i = m_orderStatus.AddString("Contract:");

		PRINT_CONTRACT_PROP(conId);
		PRINT_CONTRACT_PROP(symbol);
		PRINT_CONTRACT_PROP(secType);
		PRINT_CONTRACT_PROP(expiry);
		PRINT_CONTRACT_PROP(strike);
		PRINT_CONTRACT_PROP(right);
		PRINT_CONTRACT_PROP(multiplier);
		PRINT_CONTRACT_PROP(exchange);
		PRINT_CONTRACT_PROP(primaryExchange);
		PRINT_CONTRACT_PROP(currency);
		PRINT_CONTRACT_PROP(localSymbol);
		i = PRINT_CONTRACT_PROP(comboLegsDescrip);

		m_orderStatus.AddString("  comboLegs={");
		if (contract.comboLegs.get()) {
			const Contract::ComboLegList& comboLegs = *contract.comboLegs;
			const size_t comboLegsCount = comboLegs.size();

			const Order::OrderComboLegList& orderComboLegs = *order.orderComboLegs;
			size_t orderComboLegsCount = 0;
			if (order.orderComboLegs.get()) {
				orderComboLegsCount = orderComboLegs.size();
			}

			for( size_t i = 0; i < comboLegsCount; ++i) {
				const ComboLeg& comboLeg = *comboLegs[i];
				CString orderComboLegPriceStr("");

				if (comboLegsCount == orderComboLegsCount){
					const OrderComboLeg& orderComboLeg = *orderComboLegs[i];

					if (orderComboLeg.price != DBL_MAX) {
						orderComboLegPriceStr.Format("price=%f", orderComboLeg.price);
					}
				}

				str.Format( "    leg %i: conId=%i ratio=%i action=%s exchange=%s openClose=%i shortSaleSlot=%i designatedLocation=%s exemptCode=%i %s", 
					(i + 1), comboLeg.conId, comboLeg.ratio, comboLeg.action, comboLeg.exchange, comboLeg.openClose, 
					comboLeg.shortSaleSlot, comboLeg.designatedLocation, comboLeg.exemptCode, orderComboLegPriceStr);

				m_orderStatus.AddString( str);
			}
		}
		i = m_orderStatus.AddString("  }");

#undef PRINT_CONTRACT_PROP
	}

	if (contract.underComp) {

		const UnderComp& underComp = *contract.underComp;

#define PRINT_UNDER_COMP_PROP(Prop) \
	PRINT_PROP(underComp.##Prop,underComp.Prop)

		PRINT_UNDER_COMP_PROP(conId);
		PRINT_UNDER_COMP_PROP(delta);
		i = PRINT_UNDER_COMP_PROP(price);

#undef PRINT_UNDER_COMP_PROP
	}

	{
#define PRINT_ORDER_PROP(Prop) \
	PRINT_PROP(Prop,order.Prop)

		i = m_orderStatus.AddString("Order:");

		PRINT_ORDER_PROP(orderId);
		PRINT_ORDER_PROP(clientId);
		PRINT_ORDER_PROP(permId);
		PRINT_ORDER_PROP(action);
		PRINT_ORDER_PROP(totalQuantity);
		PRINT_ORDER_PROP(orderType);
		PRINT_ORDER_PROP(lmtPrice);
		PRINT_ORDER_PROP(auxPrice);

		i = m_orderStatus.AddString("Extended Attrs:");

		PRINT_ORDER_PROP(tif);
		PRINT_ORDER_PROP(ocaGroup);
		PRINT_ORDER_PROP(ocaType);
		PRINT_ORDER_PROP(orderRef);
		PRINT_ORDER_PROP(transmit);
		PRINT_ORDER_PROP(parentId);
		PRINT_ORDER_PROP(blockOrder);
		PRINT_ORDER_PROP(sweepToFill);
		PRINT_ORDER_PROP(displaySize);
		PRINT_ORDER_PROP(triggerMethod);
		PRINT_ORDER_PROP(outsideRth);
		PRINT_ORDER_PROP(hidden);
		PRINT_ORDER_PROP(goodAfterTime);
		PRINT_ORDER_PROP(goodTillDate);
		PRINT_ORDER_PROP(overridePercentageConstraints);
		PRINT_ORDER_PROP(rule80A);
		PRINT_ORDER_PROP(allOrNone);
		PRINT_ORDER_PROP(minQty);
		PRINT_ORDER_PROP(percentOffset);
		PRINT_ORDER_PROP(trailStopPrice);
		PRINT_ORDER_PROP(trailingPercent);
		PRINT_ORDER_PROP(whatIf);
		PRINT_ORDER_PROP(notHeld);

		PRINT_ORDER_PROP(faGroup);
		PRINT_ORDER_PROP(faProfile);
		PRINT_ORDER_PROP(faMethod);
		PRINT_ORDER_PROP(faPercentage);

		PRINT_ORDER_PROP(account);
		PRINT_ORDER_PROP(settlingFirm);
		PRINT_ORDER_PROP(clearingAccount);
		PRINT_ORDER_PROP(clearingIntent);

		PRINT_ORDER_PROP(openClose);
		PRINT_ORDER_PROP(origin);
		PRINT_ORDER_PROP(shortSaleSlot);
		PRINT_ORDER_PROP(designatedLocation);
		PRINT_ORDER_PROP(exemptCode);

		PRINT_ORDER_PROP(discretionaryAmt);
		PRINT_ORDER_PROP(eTradeOnly);
		PRINT_ORDER_PROP(firmQuoteOnly);
		PRINT_ORDER_PROP(nbboPriceCap);
		PRINT_ORDER_PROP(optOutSmartRouting);

		PRINT_ORDER_PROP(auctionStrategy);

		PRINT_ORDER_PROP(startingPrice);
		PRINT_ORDER_PROP(stockRefPrice);
		PRINT_ORDER_PROP(delta);

		PRINT_ORDER_PROP(stockRangeLower);
		PRINT_ORDER_PROP(stockRangeUpper);

		PRINT_ORDER_PROP(volatility);
		PRINT_ORDER_PROP(volatilityType);
		PRINT_ORDER_PROP(continuousUpdate);
		PRINT_ORDER_PROP(referencePriceType);
		PRINT_ORDER_PROP(deltaNeutralOrderType);
		PRINT_ORDER_PROP(deltaNeutralAuxPrice);
		PRINT_ORDER_PROP(deltaNeutralConId);
		PRINT_ORDER_PROP(deltaNeutralSettlingFirm);
		PRINT_ORDER_PROP(deltaNeutralClearingAccount);
		PRINT_ORDER_PROP(deltaNeutralClearingIntent);
		PRINT_ORDER_PROP(deltaNeutralOpenClose);
		PRINT_ORDER_PROP(deltaNeutralShortSale);
		PRINT_ORDER_PROP(deltaNeutralShortSaleSlot);
		PRINT_ORDER_PROP(deltaNeutralDesignatedLocation);

		PRINT_ORDER_PROP(basisPoints);
		PRINT_ORDER_PROP(basisPointsType);
		m_orderStatus.AddString("  smartComboRoutingParams={");
		if (order.smartComboRoutingParams.get()) {
			const TagValueList& smartComboRoutingParams = *order.smartComboRoutingParams;
			const size_t smartComboRoutingParamsCount = smartComboRoutingParams.size();
			if( smartComboRoutingParamsCount > 0) {
				const CString tagPrefix("  ");
				for( size_t i = 0; i < smartComboRoutingParamsCount; ++i) {
					const TagValue& tagValue = *smartComboRoutingParams[i];
					PropPrintHelpers::Print(m_orderStatus,
						tagPrefix + tagValue.tag, tagValue.value);
				}
			}
		}
		i = m_orderStatus.AddString("  }");

		PRINT_ORDER_PROP(scaleInitLevelSize);
		PRINT_ORDER_PROP(scaleSubsLevelSize);
		PRINT_ORDER_PROP(scalePriceIncrement);
		PRINT_ORDER_PROP(scalePriceAdjustValue);
		PRINT_ORDER_PROP(scalePriceAdjustInterval);
		PRINT_ORDER_PROP(scaleProfitOffset);
		PRINT_ORDER_PROP(scaleAutoReset);
		PRINT_ORDER_PROP(scaleInitPosition);
		PRINT_ORDER_PROP(scaleInitFillQty);
		PRINT_ORDER_PROP(scaleRandomPercent);

		PRINT_ORDER_PROP(hedgeType);
		i = PRINT_ORDER_PROP(hedgeParam);

		if (!order.algoStrategy.IsEmpty()) {
			PRINT_ORDER_PROP(algoStrategy);
			m_orderStatus.AddString("  algoParams={");
			if (order.algoParams.get()) {
				const TagValueList& algoParams = *order.algoParams;
				const size_t algoParamsCount = algoParams.size();
				if( algoParamsCount > 0) {
					const CString tagPrefix("  ");
					for( size_t i = 0; i < algoParamsCount; ++i) {
						const TagValue& tagValue = *algoParams[i];
						PropPrintHelpers::Print(m_orderStatus,
							tagPrefix + tagValue.tag, tagValue.value);
					}
				}
			}
			i = m_orderStatus.AddString("  }");
		}

#undef PRINT_ORDER_PROP
	}

	{
#define PRINT_ORDER_STATE_PROP(Prop) \
	PRINT_PROP(Prop,orderState.Prop)

		i = m_orderStatus.AddString("OrderState:");

		PRINT_ORDER_STATE_PROP(status);
		PRINT_ORDER_STATE_PROP(initMargin);
		PRINT_ORDER_STATE_PROP(maintMargin);
		PRINT_ORDER_STATE_PROP(equityWithLoan);
		PRINT_ORDER_STATE_PROP(commission);
		PRINT_ORDER_STATE_PROP(minCommission);
		PRINT_ORDER_STATE_PROP(maxCommission);
		PRINT_ORDER_STATE_PROP(commissionCurrency);
		i = PRINT_ORDER_STATE_PROP(warningText);

#undef PRINT_ORDER_STATE_PROP
	}

	// bring into view
	int top = i - N < 0 ? 0 : i - N;
	m_orderStatus.SetTopIndex(top);
}

void CClient2Dlg::openOrderEnd()
{
	int i = m_orderStatus.AddString(" =============== end ===============");

	// bring into view
	int top = i - N < 0 ? 0 : i - N;
	m_orderStatus.SetTopIndex(top);
}


void CClient2Dlg::deltaNeutralValidation(int reqId, const UnderComp& underComp)
{
	// create string
	CString str;
	str.Format("deltaNeutralValidation: reqId=%i", reqId);

	int i = m_orderStatus.AddString(str);

#define PRINT_UNDER_COMP_PROP(Prop) \
	PRINT_PROP(underComp.##Prop,underComp.Prop)

	PRINT_UNDER_COMP_PROP(conId);
	PRINT_UNDER_COMP_PROP(delta);
	i = PRINT_UNDER_COMP_PROP(price);

#undef PRINT_UNDER_COMP_PROP

	// bring into view
	int top = i - N < 0 ? 0 : i - N;
	m_orderStatus.SetTopIndex(top);
}

void CClient2Dlg::tickSnapshotEnd( int reqId)
{
	CString str;
	str.Format("id=%i =============== end ===============", reqId);

	int i = m_ticks.AddString(str);

	// bring into view
	int top = i - N < 0 ? 0 : i - N;
	m_ticks.SetTopIndex(top);
}

void CClient2Dlg::marketDataType( TickerId reqId, int marketDataType) 
{	
	CString str;
	switch (marketDataType){
	case REALTIME:
		str.Format("id=%i marketDataType=Real-Time", reqId);
		break;
	case FROZEN:
		str.Format("id=%i marketDataType=Frozen", reqId);
		break;
	default:
		str.Format("id=%i marketDataType=Unknown", reqId);
	}

	int i = m_ticks.AddString(str);

	// bring into view
	int top = i - N < 0 ? 0 : i - N;
	m_ticks.SetTopIndex(top);
}

void CClient2Dlg::updateAccountValue( const CString &key, const CString &val,
	const CString &currency, const CString &accountName)
{ 
	if(key == "AvailableFunds" && currency == "USD"){
		//�����ʽ�
		TRACE("IB�����ʽ�%.f\r\n",atof(val));
		if(m_isIni){
			m_availIb0 = atof(val);
		}
	}
	if(key == "NetLiquidation" && currency == "USD"){
		//���ʲ�
	}

	if(IsWindow(s_accountDlg.m_hWnd)){
		s_accountDlg.updateAccountValue(key, val, currency, accountName);
	}
}

void CClient2Dlg::updatePortfolio( const Contract& contract, int position,
	double marketPrice, double marketValue,
	double averageCost, double unrealizedPNL,
	double realizedPNL, const CString &accountName)

{
	TRACE("����%s�ֲ�,%d\r\n",contract.symbol,position);
	if(m_isIni){
		if(m_dlgOrder->getContract().symbol == contract.symbol
			&& m_dlgOrder->getContract().expiry == contract.expiry.Left(6))
		{
			m_portfolioIb0 = m_portfolioIb0 + position;
		}
	}
	if(IsWindow(s_accountDlg.m_hWnd)){
		s_accountDlg.updatePortfolio(contract, position, marketPrice, marketValue,
			averageCost, unrealizedPNL, realizedPNL, accountName);
	}

}

void CClient2Dlg::updateAccountTime(const CString &timeStamp)
{
	if(IsWindow(s_accountDlg.m_hWnd)){
		s_accountDlg.updateAccountTime(timeStamp);
	}
}

void CClient2Dlg::accountDownloadEnd(const CString &accountName)
{
	if(IsWindow(s_accountDlg.m_hWnd)){
		s_accountDlg.accountDownloadEnd( accountName);
	}

	CString str;
	str.Format("Account Download End: %s", accountName);
	int i = m_orderStatus.AddString(str);

	// bring into view
	int top = i - N < 0 ? 0 : i - N;
	m_orderStatus.SetTopIndex(top);
}

void CClient2Dlg::contractDetails( int reqId, const ContractDetails &contractDetails)
{
	// add to listbox
	CString str;
	str.Format("id =%i ===================================", reqId);
	m_orderStatus.AddString(str);

	int i =  m_orderStatus.AddString(" ---- Contract Details Ex begin ----");

	CString secType;

	{
		const Contract& contract = contractDetails.summary;

#define PRINT_CONTRACT_PROP(Prop) \
	PRINT_PROP(Prop,contract.Prop)

		i = m_orderStatus.AddString("Contract:");

		PRINT_CONTRACT_PROP(conId);
		PRINT_CONTRACT_PROP(symbol);
		PRINT_CONTRACT_PROP(secType);

		// store secType string for future use
		secType = contract.secType;

		PRINT_CONTRACT_PROP(expiry);
		PRINT_CONTRACT_PROP(strike);
		PRINT_CONTRACT_PROP(right);
		PRINT_CONTRACT_PROP(multiplier);
		PRINT_CONTRACT_PROP(exchange);
		PRINT_CONTRACT_PROP(primaryExchange);
		PRINT_CONTRACT_PROP(currency);
		i = PRINT_CONTRACT_PROP(localSymbol);

#undef PRINT_CONTRACT_PROP
	}

	{
#define PRINT_CONTRACT_DETAILS_PROP(Prop) \
	PRINT_PROP(Prop,contractDetails.Prop)

		i = m_orderStatus.AddString("Details:");

		PRINT_CONTRACT_DETAILS_PROP(marketName);
		PRINT_CONTRACT_DETAILS_PROP(tradingClass);
		PRINT_CONTRACT_DETAILS_PROP(minTick);
		PRINT_CONTRACT_DETAILS_PROP(priceMagnifier);
		PRINT_CONTRACT_DETAILS_PROP(orderTypes);
		PRINT_CONTRACT_DETAILS_PROP(validExchanges);
		PRINT_CONTRACT_DETAILS_PROP(underConId);
		PRINT_CONTRACT_DETAILS_PROP(longName);

		if (secType != "BOND") {
			PRINT_CONTRACT_DETAILS_PROP(contractMonth);
			PRINT_CONTRACT_DETAILS_PROP(industry);
			PRINT_CONTRACT_DETAILS_PROP(category);
			PRINT_CONTRACT_DETAILS_PROP(subcategory);
			PRINT_CONTRACT_DETAILS_PROP(timeZoneId);
			PRINT_CONTRACT_DETAILS_PROP(tradingHours);
			PRINT_CONTRACT_DETAILS_PROP(liquidHours);
		}
		PRINT_CONTRACT_DETAILS_PROP(evRule);
		PRINT_CONTRACT_DETAILS_PROP(evMultiplier);

		if (secType == "BOND") {
			i = m_orderStatus.AddString("Bond Details:");
			PRINT_CONTRACT_DETAILS_PROP(cusip);
			PRINT_CONTRACT_DETAILS_PROP(ratings);
			PRINT_CONTRACT_DETAILS_PROP(descAppend);
			PRINT_CONTRACT_DETAILS_PROP(bondType);
			PRINT_CONTRACT_DETAILS_PROP(couponType);
			PRINT_CONTRACT_DETAILS_PROP(callable);
			PRINT_CONTRACT_DETAILS_PROP(putable);
			PRINT_CONTRACT_DETAILS_PROP(coupon);
			PRINT_CONTRACT_DETAILS_PROP(convertible);
			PRINT_CONTRACT_DETAILS_PROP(maturity);
			PRINT_CONTRACT_DETAILS_PROP(issueDate);
			PRINT_CONTRACT_DETAILS_PROP(nextOptionDate);
			PRINT_CONTRACT_DETAILS_PROP(nextOptionType);
			PRINT_CONTRACT_DETAILS_PROP(nextOptionPartial);
			PRINT_CONTRACT_DETAILS_PROP(notes);
		}

		m_orderStatus.AddString("  secIdList={");
		if (contractDetails.secIdList.get()) {
			const TagValueList& secIdList = *contractDetails.secIdList;
			const size_t secIdListCount = secIdList.size();
			if( secIdListCount > 0) {
				const CString tagPrefix("  ");
				for( size_t i = 0; i < secIdListCount; ++i) {
					const TagValue& tagValue = *secIdList[i];
					PropPrintHelpers::Print(m_orderStatus,
						tagPrefix + tagValue.tag, tagValue.value);
				}
			}
		}
		i = m_orderStatus.AddString("  }");


#undef PRINT_CONTRACT_DETAILS_PROP
	}

	i = m_orderStatus.AddString(" ---- Contract Details Ex end ------");

	// bring into view
	int top = i - N < 0 ? 0 : i - N;
	m_orderStatus.SetTopIndex(top);
}

void CClient2Dlg::bondContractDetails( int reqId, const ContractDetails &contractDetails)
{
	this->contractDetails(reqId, contractDetails);
}

void CClient2Dlg::contractDetailsEnd( int reqId)
{
	CString str;
	str.Format("id =%i =============== end ===============", reqId);

	int i = m_orderStatus.AddString(str);

	// bring into view
	int top = i - N < 0 ? 0 : i - N;
	m_orderStatus.SetTopIndex(top);
}

void CClient2Dlg::execDetails( int reqId, const Contract& contract, const Execution& execution)
{
	int i = m_orderStatus.AddString("---- Execution Details begin ----");

	// create string
	CString str;
	str.Format("execDetails: reqId=%i", reqId);

	// add to listbox
	i =  m_orderStatus.AddString(str);

	{
#define PRINT_CONTRACT_PROP(Prop) \
	PRINT_PROP(Prop,contract.Prop)

		i = m_orderStatus.AddString("Contract:");

		PRINT_CONTRACT_PROP(conId);
		PRINT_CONTRACT_PROP(symbol);
		PRINT_CONTRACT_PROP(secType);
		PRINT_CONTRACT_PROP(expiry);
		PRINT_CONTRACT_PROP(strike);
		PRINT_CONTRACT_PROP(right);
		PRINT_CONTRACT_PROP(multiplier);
		PRINT_CONTRACT_PROP(exchange);
		PRINT_CONTRACT_PROP(primaryExchange);
		PRINT_CONTRACT_PROP(currency);
		PRINT_CONTRACT_PROP(localSymbol);

#undef PRINT_CONTRACT_PROP
	}

	{
#define PRINT_EXECUTION_PROP(Prop) \
	PRINT_PROP(Prop,execution.Prop)

		i = m_orderStatus.AddString("Execution:");

		PRINT_EXECUTION_PROP(execId);
		PRINT_EXECUTION_PROP(orderId);
		PRINT_EXECUTION_PROP(clientId);
		PRINT_EXECUTION_PROP(permId);
		PRINT_EXECUTION_PROP(time);
		PRINT_EXECUTION_PROP(acctNumber);
		PRINT_EXECUTION_PROP(exchange);
		PRINT_EXECUTION_PROP(side);
		PRINT_EXECUTION_PROP(shares);
		PRINT_EXECUTION_PROP(price);
		PRINT_EXECUTION_PROP(liquidation);
		PRINT_EXECUTION_PROP(cumQty);
		PRINT_EXECUTION_PROP(avgPrice);
		PRINT_EXECUTION_PROP(orderRef);
		PRINT_EXECUTION_PROP(evRule);
		PRINT_EXECUTION_PROP(evMultiplier);

#undef PRINT_EXECUTION_PROP
	}

	i = m_orderStatus.AddString(" ---- Execution Details End ----");

	// bring into view
	int top = i - N < 0 ? 0 : i - N;
	m_orderStatus.SetTopIndex(top);
}

void CClient2Dlg::execDetailsEnd( int reqId)
{
	CString str;
	str.Format("reqId=%i =============== end ===============", reqId);

	int i = m_orderStatus.AddString(str);

	// bring into view
	int top = i - N < 0 ? 0 : i - N;
	m_orderStatus.SetTopIndex(top);
}

void CClient2Dlg::commissionReport( const CommissionReport& commissionReport)
{
	int i = m_orderStatus.AddString("---- Commission Report ----");

	{
#define PRINT_COMMISSION_REPORT_PROP(Prop) \
	PRINT_PROP(Prop,commissionReport.Prop)

		PRINT_COMMISSION_REPORT_PROP(execId);
		PRINT_COMMISSION_REPORT_PROP(commission);
		PRINT_COMMISSION_REPORT_PROP(currency);
		PRINT_COMMISSION_REPORT_PROP(realizedPNL);
		PRINT_COMMISSION_REPORT_PROP(yield);
		PRINT_COMMISSION_REPORT_PROP(yieldRedemptionDate);

#undef PRINT_COMMISSION_REPORT_PROP
	}

	i = m_orderStatus.AddString(" ---- Commission Report End ----");

	// bring into view
	int top = i - N < 0 ? 0 : i - N;
	m_orderStatus.SetTopIndex(top);
}

void CClient2Dlg::updateMktDepth(TickerId id, int position, int operation, int side,
	double price, int size)
{
	//m_dlgMktDepth->updateMktDepth(id, position, "", operation, side, price, size);
	if(position == 0){
		if(side == 1){
			if(price != g_a50Bid1){
				/*
				getIndexData();
				*/
				::PostMessage(m_hWnd,WM_PRICE_CHANGED,NULL,NULL);
				g_a50Bid1 = price;
				g_a50Bid1Size = size;
			}
		}
		else{
			if(price != g_a50Ask1){
				/*
				getIndexData();
				*/
				::PostMessage(m_hWnd,WM_PRICE_CHANGED,NULL,NULL);
				g_a50Ask1 = price;
				g_a50Ask1Size = size;
			}
		}
		//TRACE("%f %f\r\n",g_a50Bid1,g_a50Ask1);
	}
}

void CClient2Dlg::updateMktDepthL2(TickerId id, int position, CString marketMaker,
	int operation, int side, double price, int size)
{
	m_dlgMktDepth->updateMktDepth(id, position, marketMaker, operation, side, price, size);
}

void CClient2Dlg::error(const CString errorMsg)
{
	fout << "error:" << errorMsg << "\n";
	int i = m_errors.AddString( errorMsg);
	int top = i - N < 0 ? 0 : i - N;
	m_errors.SetTopIndex( top);
}

void CClient2Dlg::error(const int id, const int errorCode, const CString errorMsg) {
	fout << "errorMsg:" << " id " << id << " errorCode " << errorCode 
		<< " errorMsg " << errorMsg << "\n";
	if(id == -1){
		switch (errorCode)
		{
		case 1100:
			SendWeixinMsg("LOST WITH SERVER\r\n");
			TRACE("�������ʧȥ��ϵ\r\n");
			break;
		case 2110:
			SendWeixinMsg("TWS AND SERVER CONNECTION BROKEN\r\n");
			TRACE("����ƽ̨�������������ֹ\r\n");
			break;
		case 502:
			SendWeixinMsg("CANNOT CONNECT TO GATEWAY\r\n");
			TRACE("������������\r\n");
			break;
		case 2104:
			SendWeixinMsg("MARKET DATA is OK\r\n");
			TRACE("�г�����OK\r\n");
			break;
		case 2109:
			SendWeixinMsg("Market Data Farm is OK\r\n");
			break;
		}
	}
	CString errorStr("Id: ");
	char buf[20];
	itoa(id, buf, 10);
	errorStr += CString(buf);
	errorStr += " | ";
	errorStr += "Error Code: ";
	itoa(errorCode, buf, 10);
	errorStr += CString(buf);
	errorStr += " | ";
	errorStr += "Error Msg: ";
	errorStr += errorMsg;
	error(errorStr) ;
	for (int ctr=0; ctr < NUM_FA_ERROR_CODES; ctr++) {
		faError |= (errorCode == faErrorCodes[ctr]) ;
	}
	if (errorCode == CDlgMktDepth::MKT_DEPTH_DATA_RESET) {
		//m_dlgMktDepth->clear();
	}
}

void CClient2Dlg::winError( const CString &str, int lastError)
{
	// get windows error msg text
	TRACE("��Ϣ����\r\n");
	void *lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR)&lpMsgBuf,
		0,
		NULL);

	// format msg
	CString fullMsg;
	if( lpMsgBuf && strlen( (const char *)lpMsgBuf) > 0)
		fullMsg.Format( "%s - %s (%i)", (const char *)str, lpMsgBuf, lastError);
	else
		fullMsg.Format( "%s (%i)", (const char *)str, lastError);

	// free the buffer.
	LocalFree( lpMsgBuf);

	// display it
	error( NO_VALID_ID, SYSTEM_ERROR, fullMsg);
}

void CClient2Dlg::connectionClosed()
{
	MessageBox( "Connection closed");
}

void CClient2Dlg::nextValidId( OrderId orderId)
{
	TRACE("��ȡ��Ч��ID = %d\r\n",orderId);
	//m_isIbReady = true;//�Ѿ���ȡ����Чid,���Խ��н�����
	m_dlgOrder->m_id = orderId;
	::PostMessage(m_hWnd,WM_SYS_INI,NULL,1);//1����IBϵͳ
}

void CClient2Dlg::OnReqMktDepth()
{
	// run dlg box
	m_dlgOrder->init( this, "Request Market Depth", CDlgOrder::REQ_MKT_DEPTH, m_managedAccounts);
	if( m_dlgOrder->DoModal() != IDOK) return;

	// request ticker
	m_pClient->reqMktDepth(m_dlgOrder->m_id, m_dlgOrder->getContract(), m_dlgOrder->m_numRows);
	m_dlgMktDepth->DoModal();
	m_pClient->cancelMktDepth(m_dlgOrder->m_id);
}

void CClient2Dlg::updateNewsBulletin(int msgId, int msgType, const CString& newsMessage,
	const CString& originExch)
{
	CString displayString;
	displayString.Format(" MsgId=%d :: MsgType = %d :: Origin= %s :: Message= %s",
		msgId, msgType, originExch, newsMessage);

	MessageBox( displayString, "IB News Bulletin", MB_ICONINFORMATION);
}

void CClient2Dlg::OnNewsBulletins()
{
	// TODO: Add your control notification handler code here
	CDlgNewsBulletins dlg;

	if (dlg.DoModal() != IDOK) return;

	dlg.subscribe()
		? m_pClient->reqNewsBulletins( dlg.allMsgs())
		: m_pClient->cancelNewsBulletins();
}

void CClient2Dlg::OnFinancialAdvisor()
{
	faGroupsXML = faProfilesXML = faAliasesXML = "" ;
	faError = false ;
	m_pClient->requestFA(GROUPS) ;
	m_pClient->requestFA(PROFILES) ;
	m_pClient->requestFA(ALIASES) ;
}

void CClient2Dlg::OnSetServerLogLevel()
{
	if (s_dlgLogConfig.DoModal() != IDOK) return;

	// set the TWS log level for API requests/responses
	m_pClient->setServerLogLevel( s_dlgLogConfig.serverLogLevel() );
}

void CClient2Dlg::OnReqAccts()
{
	// request the list of managed accounts
	m_pClient->reqManagedAccts();
}

void CClient2Dlg::managedAccounts(const CString& accountsList)
{
	m_financialAdvisor = true;
	m_managedAccounts = accountsList;

	CString displayString;
	displayString.Format("Connected : The list of managed accounts are : [%s]", accountsList);
	m_orderStatus.AddString( displayString);
}

void CClient2Dlg::OnCalculateImpliedVolatility()
{
	// run dlg box
	m_dlgOrder->init( this, "Calculate Implied Volatility", CDlgOrder::CALC_IMPL_VOL, m_managedAccounts);
	if( m_dlgOrder->DoModal() != IDOK) return;

	// request calculate implied volatility
	m_pClient->calculateImpliedVolatility( m_dlgOrder->m_id, m_dlgOrder->getContract(),
		m_dlgOrder->m_lmtPrice, m_dlgOrder->m_auxPrice);
}

void CClient2Dlg::OnCalculateOptionPrice()
{
	// run dlg box
	m_dlgOrder->init( this, "Calculate Option Price", CDlgOrder::CALC_OPTION_PRICE, m_managedAccounts);
	if( m_dlgOrder->DoModal() != IDOK) return;

	// request calculate option price
	m_pClient->calculateOptionPrice( m_dlgOrder->m_id, m_dlgOrder->getContract(),
		m_dlgOrder->m_lmtPrice, m_dlgOrder->m_auxPrice);
}

void CClient2Dlg::OnCancelCalculateImpliedVolatility()
{
	// run dlg box
	m_dlgOrder->init( this, "Cancel Calculate Implied Volatility", CDlgOrder::CANCEL_CALC_IMPL_VOL, m_managedAccounts);
	if( m_dlgOrder->DoModal() != IDOK) return;

	// cancel calculate implied volatility
	m_pClient->cancelCalculateImpliedVolatility( m_dlgOrder->m_id);
}

void CClient2Dlg::OnCancelCalculateOptionPrice()
{
	// run dlg box
	m_dlgOrder->init( this, "Cancel Calculate Option Price", CDlgOrder::CANCEL_CALC_OPTION_PRICE, m_managedAccounts);
	if( m_dlgOrder->DoModal() != IDOK) return;

	// cancel calculate option price
	m_pClient->cancelCalculateOptionPrice( m_dlgOrder->m_id);
}

void CClient2Dlg::OnReqGlobalCancel()
{
	m_pClient->reqGlobalCancel();
}

void CClient2Dlg::OnReqMarketDataType()
{
	// run dlg box
	m_dlgOrder->init( this, "Request Market Data Type", CDlgOrder::REQ_MARKET_DATA_TYPE, m_managedAccounts);
	if( m_dlgOrder->DoModal() != IDOK) return;

	// request market data type
	m_pClient->reqMarketDataType( m_dlgOrder->m_marketDataType);
}

void CClient2Dlg::OnSetupMM()
{
	m_dlgPic.m_pWXin = &m_WXin;
	m_dlgPic.Create(IDD_DIALOG_PIC, this);
	m_WXin.Attach(m_dlgPic.m_hWnd); //����������
	m_dlgPic.ShowWindow(SW_SHOW);
	GetDlgItem(IDC_SETUP_MM)->EnableWindow(false);
}
int CClient2Dlg::SendWeixinMsg(LPCTSTR lpMsg)
{
	TRACE(lpMsg);
	this->m_WXin.SendMsg("wxid_wv1so37or1ya21",lpMsg);
	this->m_WXin.SendMsg("hanqiong9347",lpMsg);
	return 0;
}

void CClient2Dlg::OnFrontConnected(void)
{
	TRACE("���̽���ϵͳǰ�����ӳɹ�...\r\n");
	//m_isFrontConnectedCtpTrade = true;
	//ReleaseSemaphore(g_hFrontTradeConnected,1,NULL);
	ReqUserLoginCtpTrade();
}
void CClient2Dlg::ReqUserLoginCtpTrade(void)
{
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, m_brokerId);
	strcpy(req.UserID, this->m_investorId);
	strcpy(req.Password, m_password);
	int newRequestId = ::InterlockedIncrement((long*)(&m_iRequestId));
	int iResult = m_pUserApiTrade->ReqUserLogin(&req,newRequestId);
}
void CClient2Dlg::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
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
bool CClient2Dlg::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	bool bResult = false;
	if(pRspInfo){
		bResult = pRspInfo->ErrorID != 0;
	}
	else{
		bResult = false;
	}
	return bResult;
}
void CClient2Dlg::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		TRACE("����ȷ�Ͻ���...\r\n");
		//m_isCtpReady = true;
		::PostMessage(m_hWnd,WM_SYS_INI,NULL,2);//2����CTP����ϵͳ
		//��ѯ����
		ReqQryDepthMarketData();
		//ReqQryDepthMarketData();//�����ѯ����г�����,��������Ľ���۵ȴ�
		//ReqQryTradingAccount();//��̬Ȩ��
		//ReqQryInvestorPosition();//�ֲ�
		//ReqQryInstrument();
		//ReqQryInstrumentMarginRate();
		////////////////////////////////
		//ReqQryInvestorPositionDetail();
		//ReqQryTrade();
	}
	else{
		TRACE("����ȷ��ʧ��!\r\n");
		int i = m_errors.AddString(_T("CTP����ȷ��ʧ��\r\n"));
		int top = i - N < 0 ? 0 : i - N;
		m_errors.SetTopIndex( top);
	}
}


void CClient2Dlg::OnConnectHedgeSys()
{
	fout << "�ֶ�����\n";
	TRACE("���߳�ID%d\r\n",::GetCurrentThreadId());
	//INDEX
	if(!m_pIndexThread){
		m_pIndexThread = (CIndex*)AfxBeginThread(RUNTIME_CLASS(CIndex));
	}
	///////����/////////
	//����IBϵͳ
	SetContractIb();//IB��Լ����
	TRACE("IBϵͳ������...\r\n");
	CString displayString;
	displayString.Format("Connecting to Tws using clientId %d ...",0);
	int i = m_orderStatus.AddString( displayString);
	m_orderStatus.SetTopIndex(i);
	m_pClient->eConnect("127.0.0.1",4001,0);
	TRACE("����IB�ʻ���Ϣ���£�����ģʽ\r\n");
	m_pClient->reqAccountUpdates(true,"U1032950");
	m_dlgOrder->m_id++;
	m_pClient->reqMktDepth(m_dlgOrder->m_id, m_dlgOrder->getContract(), m_dlgOrder->m_numRows);
	if(m_pClient->serverVersion() > 0)	{
		CString displayString;
		displayString.Format( "Connected to Tws server version %d at %s.",
			m_pClient->serverVersion(), m_pClient->TwsConnectionTime());
		int i = m_orderStatus.AddString( displayString);
		m_orderStatus.SetTopIndex(i);
	}
	//////����//////////
	SetContractCtp();//���̺�Լ����
	//������������ϵͳ,��Ϊһ���������߳������̷ֿ߳�
	//������Ҫ���͵������˻�����
	ParamCtpMd *pParamMd = &ParamMdCtp;
	memset(&ParamMdCtp,0,sizeof(ParamMdCtp));
	strcpy(pParamMd->brokerId,m_brokerId);
	strcpy(pParamMd->instrumentId,m_instrumentId);
	strcpy(pParamMd->password,m_password);
	strcpy(pParamMd->investorId,m_investorId);
	if(!m_pCtpMarketDataThread){
		m_pCtpMarketDataThread = 
			(CCtpMarketData*)AfxBeginThread(RUNTIME_CLASS(CCtpMarketData));
		TRACE("ctp����ϵͳ����...\r\n");
		m_pCtpMarketDataThread->PostThreadMessage(WM_INIDATA_CTP_MD,(UINT)pParamMd,NULL);
		//m_pCtpMarketDataThread->PostThreadMessage(WM_LOGIN_CTP_MD,NULL,NULL);
		//m_pCtpMarketDataThread->PostThreadMessage(WM_SUBMARKETDATA_CTP_MD,NULL,NULL);
	}
	else{
		m_pCtpMarketDataThread->PostThreadMessage(WM_INIDATA_CTP_MD,(UINT)pParamMd,NULL);
		//m_pCtpMarketDataThread->PostThreadMessage(WM_LOGIN_CTP_MD,NULL,NULL);
		//m_pCtpMarketDataThread->PostThreadMessage(WM_SUBMARKETDATA_CTP_MD,NULL,NULL);
		//TRACE("���ú�Լ������ctp����ϵͳ...\r\n");
	}
	//���������߳�
	/*
	ParamCtpTd *pParamTd = &ParamTdCtp;
	memset(&ParamTdCtp,0,sizeof(ParamTdCtp));
	strcpy(pParamTd->brokerId,m_brokerId);
	strcpy(pParamTd->instrumentId,m_instrumentId);
	strcpy(pParamTd->password,m_password);
	strcpy(pParamTd->investorId,m_investorId);
	if(!m_pCtpTraderThread){
	m_pCtpTraderThread = (CCtpTrader*)AfxBeginThread(RUNTIME_CLASS(CCtpTrader));
	TRACE("ctp����ϵͳ����...\r\n");
	m_pCtpTraderThread->PostThreadMessage(WM_INIDATA_CTP_TD,(UINT)pParamTd,NULL);
	}
	else{
	m_pCtpTraderThread->PostThreadMessage(WM_INIDATA_CTP_TD,(UINT)pParamTd,NULL);
	}
	*/
	//�������̽���ϵͳ
	m_pUserApiTrade = CThostFtdcTraderApi::CreateFtdcTraderApi();// ����UserApi
	m_pUserApiTrade->RegisterSpi((CThostFtdcTraderSpi*)this);			// ע���¼���
	m_pUserApiTrade->SubscribePublicTopic(TERT_RESTART);					// ע�ṫ����
	m_pUserApiTrade->SubscribePrivateTopic(TERT_RESTART);					// ע��˽����
	for(int i = 0;i < NUM_FRONT_TRADE;i++){
		m_pUserApiTrade->RegisterFront(m_frontAddrTrade[i]);							
	}
	m_pUserApiTrade->Init();
	/*
	if(!m_isFrontConnectedCtpTrade){
	m_pUserApiTrade->RegisterSpi(this);			// ע���¼���
	m_pUserApiTrade->SubscribePublicTopic(TERT_RESTART);					// ע�ṫ����
	m_pUserApiTrade->SubscribePrivateTopic(TERT_RESTART);					// ע��˽����
	for(int i = 0;i < NUM_FRONT_TRADE;i++){
	m_pUserApiTrade->RegisterFront(m_frontAddrTrade[i]);							
	}
	m_pUserApiTrade->Init();
	}
	while(true){
	if(!m_isFrontConnectedCtpTrade){
	WaitForSingleObject(g_hFrontTradeConnected, INFINITE);
	}
	//ReqUserLoginCtpTrade();
	while(true){
	if(m_isCtpReady){
	ReqQryTradingAccount();
	ReqQryInvestorPosition();
	ReqQryInstrument();
	break;
	}
	}
	break;
	}
	*/
	//���Ŀؼ�ID�����֣����DisConnect״̬
	m_ConnectButton.SetWindowTextA(_T("DisConneHedge"));
	m_ConnectButton.SetDlgCtrlID(IDC_DISCONNECT_HEDGE_SYS);
	//GetDlgItem(IDC_HEDGE_SETUP)->EnableWindow(1);
}


void CClient2Dlg::OnLbnSelchangeList2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CClient2Dlg::SetContractIb(void)
{
	TRACE("���ò����º�Լ����...\r\n");
	long conId;
	CString symbol;
	CString expiry;
	CString right;
	CString multiplier;
	CString exchange;
	CString primaryExchange;
	CString currency;
	bool includeExpired;
	CString secType;
	double strike;
	CString localSymbol;
	CString secIdType;
	CString secId;
	//����Լ����
	secIdType = _T("");
	secId = _T("");
	conId = 0;
	symbol = _T("XINA50");
	//������Ҫ���õĺ�Լ���·�
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	WORD A50Final = A50FinalDay(sys.wYear,sys.wMonth);
	CString a50days;
	a50days.Format("%d",A50Final - sys.wDay);	
	m_a50Days.SetWindowTextA(_T("A50ʣ") + _T(a50days) + _T("��"));
	//�����һ���ǰһ�컻��Լ
	if(sys.wDay < A50Final){
		expiry.Format(_T("%.4d%.2d"),sys.wYear,sys.wMonth);
		m_lifeA50 = A50FinalDay(sys.wYear,sys.wMonth + 1) - sys.wDay + 1;
	}
	else{
		if(sys.wMonth < 12){
			expiry.Format(_T("%.4d%.2d"),sys.wYear,sys.wMonth + 1);
			m_lifeA50 = MonthDays(sys.wYear,sys.wMonth) - sys.wDay
				+ A50FinalDay(sys.wYear,sys.wMonth + 1) + 1;
		}
		else{//��ĩ
			expiry.Format(_T("%.4d%.2d"),sys.wYear + 1,1);
			m_lifeA50 = MonthDays(sys.wYear,sys.wMonth) - sys.wDay
				+ A50FinalDay(sys.wYear + 1,1) + 1;
		}
	}
	right = _T("");
	multiplier = _T("");
	exchange = _T("SGX");
	primaryExchange = _T("");
	currency = _T("USD");
	includeExpired = 0;
	strike = 0.0f;
	secType = _T("FUT");
	localSymbol = _T("");
#define PUT_PROP(Prop) \
	m_dlgOrder->m_##Prop = Prop;
	PUT_PROP(conId);
	PUT_PROP(symbol);
	PUT_PROP(secType);
	PUT_PROP(expiry);
	PUT_PROP(strike);
	PUT_PROP(right);
	PUT_PROP(multiplier);
	PUT_PROP(exchange);
	PUT_PROP(primaryExchange);
	PUT_PROP(currency);
	PUT_PROP(localSymbol);
	PUT_PROP(includeExpired);
	PUT_PROP(secIdType);
	PUT_PROP(secId);
#undef PUT_PROP
	m_dlgOrder->updateContractIB();
	TRACE("IB��Լ%s\\%s\r\n",m_dlgOrder->getContract().symbol,expiry);
}
void CClient2Dlg::SetContractCtp(void)
{
	//�������ú�Լ,�Զ�����
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	WORD ifFinal = ifFinalDay(sys.wYear,sys.wMonth);
	CString insID;
	CString ifdays;
	ifdays.Format("%d",ifFinal - sys.wDay);	
	m_ifDays.SetWindowTextA(_T("IFʣ") + _T(ifdays) + _T("��"));
	if(ifFinal - sys.wDay <= 3){
	}
	//�����һ���ǰһ�컻��Լ
	if(sys.wDay < ifFinal){
		insID.Format(_T("%.4d%.2d"),sys.wYear,sys.wMonth);
		m_lifeIf = ifFinal - sys.wDay + 1;
	}
	else{
		if(sys.wMonth < 12){
			insID.Format(_T("%.4d%.2d"),sys.wYear,sys.wMonth + 1);
			m_lifeIf = MonthDays(sys.wYear,sys.wMonth) - sys.wDay
				+ ifFinalDay(sys.wYear,sys.wMonth + 1) + 1;
		}
		else{//��ĩ
			insID.Format(_T("%.4d%.2d"),sys.wYear + 1,1);
			m_lifeIf = MonthDays(sys.wYear,sys.wMonth) - sys.wDay
				+ ifFinalDay(sys.wYear + 1,1) + 1;
		}
	}
	memset(m_instrumentId,0,sizeof(m_instrumentId));
	m_instrumentId[0] = 'I';
	m_instrumentId[1] = 'F';
	m_instrumentId[2] = insID.GetAt(2);
	m_instrumentId[3] = insID.GetAt(3);
	m_instrumentId[4] = insID.GetAt(4);
	m_instrumentId[5] = insID.GetAt(5);
	TRACE("ctp��Լ%s\r\n",m_instrumentId);
}
void CClient2Dlg::OnDisconnectHedgeSys()
{
	fout << "�ֶ��Ͽ�����\n";
	//����־������ʼ��
	m_isIni = true;
	m_isHedgeLoopSetUp = false;
	m_isIbReady = false;
	m_isCtpReady = false;
	m_isUpdateMarginRatioCtp = false;
	m_isPorliIniCtp = false;
	m_isUpdateSettlementPriceCtp = false;
	m_isReqTradeCtp = false;
	m_isQryInstrumentCtp = false;
	m_isNetIniCtp = false;
	m_availIb0  = 0;
	m_availCtp0 = 0;
	m_netCtp0 = 0;
	m_netIb0 = 0;
	m_portfolioCtp0 = 0;
	m_portfolioIb0 = 0;
	m_portfolioCtpYd = 0;
	//����ID������
	m_ConnectButton.SetWindowTextA(_T("ConnectHedge"));
	m_ConnectButton.SetDlgCtrlID(IDC_CONNECT_HEDGESYS);
	//�����Ϣ
	m_ticks.ResetContent();
	m_orderStatus.ResetContent();
	m_errors.ResetContent();
	m_CTradingDataCtp.clear();
	m_CTradingDataIb.clear();
	GetDlgItem(IDC_HEDGE_SETUP)->EnableWindow(0);

	m_pClient->eDisconnect();
	CThostFtdcUserLogoutField req;
	memset(&req,0,sizeof(req));
	strcpy(req.BrokerID,m_brokerId);
	strcpy(req.UserID,m_investorId);
	/*
	if(m_pUserApiTrade){
	int newRequestId = ::InterlockedIncrement((long*)(&m_iRequestId));
	m_pUserApiTrade->ReqUserLogout(&req,newRequestId);
	}
	*/
	//m_pUserApiTrade->RegisterSpi(NULL);
	m_pUserApiTrade->Release();//ֱ��ɾ��API
	if(m_pIndexThread){
		m_pIndexThread->PostThreadMessage(WM_QUIT,NULL,NULL);
	}
	m_pIndexThread = NULL;
	if(m_pCtpMarketDataThread){
		//m_pCtpMarketDataThread->PostThreadMessage(WM_LOGOUT_CTP_MD,NULL,NULL);
		m_pCtpMarketDataThread->PostThreadMessage(WM_QUIT,NULL,NULL);
	}
	m_pCtpMarketDataThread = NULL;
}
void CClient2Dlg::OnFrontDisconnected(int nReason)
{
	//m_isFrontConnectedCtpTrade = false;
	TRACE("CTP����ϵͳ����\r\n");
	fout1 << "CTP����ϵͳ����\n";
}

void CClient2Dlg::HedgeLoopSetupAndIni()
{
	/*
	::PostMessage(m_hWnd,WM_PRICE_CHANGED,NULL,NULL);
	if((!m_isIbReady) || (!m_isCtpReady)){
		TRACE("IB����ϵͳδ��ʼ����ϣ�����\r\n");
		return;
	}
	*/
	/*
	if(!m_isUpdateMarginRatioCtp){
		TRACE("Ctp��֤�����δ��ʼ����ϣ�����\r\n");
		return;
	}
	*/
	m_isIni = false;//��ʼ������
	//m_ifAsk10 = g_ifAsk1;
	//m_ifBid10 = g_ifBid1;
	m_a50Bid10 = g_a50Bid1;
	m_a50Ask10 = g_a50Ask1;
	//CTP��ʼ�ʽ����ֱ�Ӳ�ѯ,��balance
	//����CTP��ʼ�ʽ�ֵ
	/*
	if(m_portfolioCtp0 >= 0){
	m_netCtp0 = m_availCtp0 + (m_ifAsk10 + m_ifBid10) / 2.0 * m_valuePointCtp * m_LongMarginRatio * abs(m_portfolioCtp0);
	}
	else{
	m_netCtp0 = m_availCtp0 + (m_ifAsk10 + m_ifBid10) / 2.0 * m_valuePointCtp * m_ShortMarginRatio * abs(m_portfolioCtp0);
	}
	*/
	fout << " m_ifAsk10 = " << m_ifAsk10 <<  " m_ifBid10 = " <<  m_ifBid10
		<< " ctp��ʼ���ʽ�" << m_netCtp0 << "\n";
	//����A50��ʼ�ʽ�ֵ
	//if(m_portfolioIb0 >= 0){
	m_netIb0 = m_availIb0 + m_MarginA50 * abs(m_portfolioIb0);
	//}
	//else{
	//m_netIb0 = m_availIb0 + m_MarginA50 * abs(m_portfolioIb0);
	//}
	fout << "IB��ʼ�����ʽ�" <<  m_availIb0 << " ��ʼ�ֲ�" << m_portfolioIb0 << " ��ʼ���ʽ�" << m_netIb0 << "\n";
	fout << "m_a50Bid10 = " << m_a50Bid10 << "m_a50Ask10 = " << m_a50Ask10 << "\n";
	CString label;
	label.Format(_T("Ctp����%.2f"),m_netCtp0);
	m_CtpNetLabel.SetWindowTextA(label);
	label.Format(_T("Ctp����%d"),m_portfolioCtp0);
	m_ifPortLabel.SetWindowTextA(label);
	label.Format(_T("IB����%.2f"),m_netIb0);
	m_ibNetLabel.SetWindowTextA(label);
	label.Format(_T("IB����%d"),m_portfolioIb0);
	m_a50PortLabel.SetWindowTextA(label);
	label.Format(_T("����%.2f"),m_netCtp0 + m_netIb0 * m_RMBtoUSD);
	m_totalLabel.SetWindowTextA(label);
	TRACE("ѭ���Գ��������\r\n");
	fout << "ѭ���Գ��������\n";
	m_isHedgeLoopSetUp = true;//ѭ���Գ�ϵͳ��������
	GetDlgItem(IDC_HEDGE_SETUP)->EnableWindow(0);
	::PostMessage(m_hWnd,WM_PRICE_CHANGED,NULL,NULL);
}
void CClient2Dlg::ReqQryTradingAccount()
{
	TRACE("�����˻���ѯָ��\r\n");
	CThostFtdcQryTradingAccountField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, m_brokerId);
	strcpy(req.InvestorID, m_investorId);
	int newRequestId = ::InterlockedIncrement((long*)(&m_iRequestId));
	int iResult = m_pUserApiTrade->ReqQryTradingAccount(&req,newRequestId);
	fout << "ReqQryTradingAccount" << "iResult" << iResult << "\n";
	TRACE("ReqQryTradingAccount,iResult=%d\n",iResult);
}
void CClient2Dlg::OnRspQryTradingAccount(CThostFtdcTradingAccountField * pTradingAccount, CThostFtdcRspInfoField * pRspInfo, int nRequestID, bool bIsLast)
{
	TRACE("�����˻���ѯ���\r\n");
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		//if(m_isIni){
		if(pTradingAccount != NULL){
			m_netCtp0 = pTradingAccount->PreBalance - pTradingAccount->PreCredit - pTradingAccount->PreMortgage
				+ pTradingAccount->Mortgage - pTradingAccount->Withdraw + pTradingAccount->Deposit ;
		}
		else{
			m_netCtp0 = 0;
		}
		::PostMessage(m_hWnd,WM_SYS_INI,NULL,6);//6����̬Ȩ���ʼ��
		TRACE("Ctp��̬Ȩ��%.f\r\n",m_netCtp0);
		//}
		fout1 << "Ctp��̬Ȩ��" << m_netCtp0 << "\n";
		ReqQryInvestorPosition();
	}
}
void CClient2Dlg::ReqQryInvestorPosition()
{
	CThostFtdcQryInvestorPositionField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, m_brokerId);
	strcpy(req.InvestorID, m_investorId);
	strcpy(req.InstrumentID, m_instrumentId);
	int newRequestId = ::InterlockedIncrement((long*)(&m_iRequestId));
	int iResult = m_pUserApiTrade->ReqQryInvestorPosition(&req,newRequestId);
	fout << "ReqQryInvestorPosition" << "iResult" << iResult << "\n";
	TRACE("ReqQryInvestorPosition,iResult=%d\n",iResult);
}
void CClient2Dlg::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{	
	CTradingDataCtp CTDC(m_brokerId,m_investorId,m_instrumentId);
	if(!IsErrorRspInfo(pRspInfo))
	{
		//TRACE("�ֲ�����%d,����%c,��֤�����%.2f\r\n",pInvestorPosition->Position,pInvestorPosition->PosiDirection,pInvestorPosition->MarginRateByVolume);
		//if(m_isIni){
		if(pInvestorPosition != NULL){	
			CTDC.expired = true;
			CTDC.volumeFilled = pInvestorPosition->YdPosition;
			CTDC.avgFilledPrice = m_preSettlementPriceCtp;
			switch(pInvestorPosition->PosiDirection){
			case THOST_FTDC_PD_Long:
				m_portfolioCtp0 = m_portfolioCtp0 + pInvestorPosition->YdPosition;
				//m_portfolioCtpYd = m_portfolioCtpYd + pInvestorPosition->YdPosition;
				CTDC.req.Direction = THOST_FTDC_D_Buy;
				break;
			case THOST_FTDC_PD_Short:
				m_portfolioCtp0 = m_portfolioCtp0 - pInvestorPosition->YdPosition;
				//m_portfolioCtpYd = m_portfolioCtpYd - pInvestorPosition->YdPosition;
				CTDC.req.Direction = THOST_FTDC_D_Sell;
				break;
			}
			m_CTradingDataCtp.push_back(CTDC);
		}
		if(bIsLast){
			::PostMessage(m_hWnd,WM_SYS_INI,NULL,7);//�ֲֳ�ʼ�����
			ReqQryInstrument();
		}
		TRACE("��Ctp����%d\r\n",m_portfolioCtp0);
		//}
	}
}
//����֪ͨת��
LRESULT CClient2Dlg::OnRtnOrderMsg(UINT wParam, long lParam){
	CThostFtdcOrderField *pOrder = (CThostFtdcOrderField *)wParam;
	fout1 << "�����OnRtnOrder:" << " ��������" << pOrder->OrderRef << " ϵͳid" << pOrder->OrderSysID
		<< " ����״̬" << pOrder->OrderStatus << " �Ѿ��ɽ�" << pOrder->VolumeTraded << " �����۸�" 
		<< pOrder->LimitPrice << "\n";
	TRACE("OnRtnOrder���б���֪ͨ%s,%s,%d,�Ѿ��ɽ�%d\r\n",pOrder->OrderRef, pOrder->OrderSysID, pOrder->OrderStatus,pOrder->VolumeTraded);
	std::list<CTradingDataCtp>::iterator iTDCTP;//������
	//if (isMyOrder(pOrder)){
	for(iTDCTP = m_CTradingDataCtp.begin();iTDCTP != m_CTradingDataCtp.end();iTDCTP++){
		if(iTDCTP->expired){
			continue;
		}
		if(strcmp(iTDCTP->req.OrderRef,pOrder->OrderRef) == 0 
			&& iTDCTP->frontIdTrade == pOrder->FrontID
			&& iTDCTP->sessionIdTrade == pOrder->SessionID){
				//if(strcmp(pOrder->OrderSysID,"") != 0){//�����ȥ��,����Ϊ���û���͵�������,��û��SysId��
				strcpy(iTDCTP->sysId,pOrder->OrderSysID);//��ȡϵͳID
				if(pOrder->OrderStatus == THOST_FTDC_OST_Canceled){
					if(pOrder->VolumeTraded > 0){
						iTDCTP->volumeEnd = pOrder->VolumeTraded;
					}
					else{
						iTDCTP->volumeEnd = 0;
					}
				}
				//}
		}
		//}
	}
	delete pOrder;
	return 0;
}
//����֪ͨ
void CClient2Dlg::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	CThostFtdcOrderField * pOrderNew = new CThostFtdcOrderField;
	*pOrderNew = *pOrder;
	::PostMessage(m_hWnd,WM_ONRTNORDER_TRANSFER,(WPARAM)pOrderNew,NULL);
}
LRESULT CClient2Dlg::OnRtnTradeMsg(UINT wParam, long lParam){
	CThostFtdcTradeField *pTrade = (CThostFtdcTradeField *)wParam;
	fout1 << "�ɽ�֪ͨOnRtnTrade:" << "�������� " << pTrade->OrderRef << " �������" << pTrade->OrderSysID
		<< " ����" << pTrade->Volume << " �۸�" << pTrade->Price << " ������ID " << pTrade->ExchangeID << "\n";
	TRACE("�гɽ�֪ͨ\r\n");
	CString SysID(pTrade->OrderSysID);//����ϵͳ��Ž��б��
	std::map<CString,std::list<CThostFtdcTradeField>>::iterator iTradeRtn;
	iTradeRtn = m_ifRtn.find(SysID);
	if(iTradeRtn != m_ifRtn.end()){
		iTradeRtn->second.push_back(*pTrade);
	}
	else{
		std::list<CThostFtdcTradeField> tradeRes;
		tradeRes.push_back(*pTrade);
		m_ifRtn.insert(std::pair<CString,std::list<CThostFtdcTradeField>>(SysID,tradeRes));
	}
	delete pTrade;
	return 0;
}
///�ɽ�֪ͨ
void  CClient2Dlg::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
	CThostFtdcTradeField * pTradeNew = new CThostFtdcTradeField;
	*pTradeNew = *pTrade;
	::PostMessage(m_hWnd,WM_ONRTNTRADE_TRANSFER,(WPARAM)pTradeNew,NULL);
	::PostMessage(m_hWnd,WM_ORDER_STATUS_CTP,NULL,NULL);//����״̬������֪ͨ
}
LRESULT CClient2Dlg::OnRspOrderInsertMsg(UINT wParam, long lParam){
	CThostFtdcInputOrderField * pInputOrder = (CThostFtdcInputOrderField *)wParam;
	int nRequestID = lParam;
	TRACE("������Thost�ܾ�\r\n");
	fout1 << "������Thost�ܾ�" << pInputOrder->OrderRef << "  " << pInputOrder->RequestID << "\n";
	std::list<CTradingDataCtp>::iterator iTDCTP;//������
	for(iTDCTP = m_CTradingDataCtp.begin();iTDCTP != m_CTradingDataCtp.end();iTDCTP++){
		if(iTDCTP->expired){
			continue;
		}
		if(/*strcmp(iTDCTP->req.OrderRef,pInputOrder->OrderRef) == 0*/
			iTDCTP->requestId == nRequestID){
				iTDCTP->expired = true;
		}
		::PostMessage(m_hWnd,WM_ORDER_STATUS_CTP,NULL,NULL);//����״̬������֪ͨ
	}
	delete pInputOrder;
	return 0;
}
LRESULT CClient2Dlg::OnErrRtnOrderInsertMsg(UINT wParam, long lParam){
	TRACE("ctpϵͳ���ر�������\r\n");
	CThostFtdcInputOrderField * pInputOrder = (CThostFtdcInputOrderField *)wParam;
	fout1 << "ctpϵͳ���ر�������" << pInputOrder->OrderRef << "  " << pInputOrder->RequestID << "\n";
	std::list<CTradingDataCtp>::iterator iTDCTP;//������
	for(iTDCTP = m_CTradingDataCtp.begin();iTDCTP != m_CTradingDataCtp.end();iTDCTP++){
		if(iTDCTP->expired){
			continue;
		}
		if(/*strcmp(iTDCTP->req.OrderRef,pInputOrder->OrderRef) == 0*/
			iTDCTP->requestId == pInputOrder->RequestID){
				iTDCTP->expired = true;
		}
	}
	::PostMessage(m_hWnd,WM_ORDER_STATUS_CTP,NULL,NULL);//����״̬������֪ͨ
	delete pInputOrder;
	return 0;
}
LRESULT CClient2Dlg::OnRspErrorTransfer(UINT wParam, long lParam){
	//�ݲ����κδ�����Ϊ�µ��ͳ��������ܵ����������,��������,�󲿷�������ֲ��ᷢ���������
	TRACE("�������\r\n");
	return 0;
}
//ֻ���ڱ������ܾ�ʱ�Ż��д˷���
void CClient2Dlg::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	if (bIsLast && !IsErrorRspInfo(pRspInfo)){
		fout1 << "����¼�뷵��OnRspOrderInsert:" << pInputOrder->OrderRef << " " << pRspInfo->ErrorID 
			<< " " << pRspInfo->ErrorMsg << "\n";
		CThostFtdcInputOrderField * pInputOrderNew = new CThostFtdcInputOrderField;
		*pInputOrderNew = *pInputOrder;
		::PostMessage(m_hWnd,WM_OnRspOrderInsert_Transfer,(WPARAM)pInputOrderNew,nRequestID);
	}
}	
//����¼�����ر�
void CClient2Dlg::OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo){
	if(!IsErrorRspInfo(pRspInfo)){
		fout1 << "��������OnErrRtnOrderInsert:" << pInputOrder->OrderRef << " " << pRspInfo->ErrorID 
			<< " " << pRspInfo->ErrorMsg << "\n";
		CThostFtdcInputOrderField * pInputOrderNew = new CThostFtdcInputOrderField;
		*pInputOrderNew = *pInputOrder;
		::PostMessage(m_hWnd,WM_OnErrRtnOrderInsert_Transfer,(WPARAM)pInputOrderNew,NULL);	
	}
}
//�������ر�
void CClient2Dlg::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	if (bIsLast && !IsErrorRspInfo(pRspInfo)){
		fout1 << "�������OnRspError:" << pRspInfo->ErrorID << "  " << pRspInfo->ErrorMsg << "\n";
		::PostMessage(m_hWnd,WM_OnRspError_Transfer,NULL,nRequestID);
	}
}
bool CClient2Dlg::isMyOrder(CThostFtdcOrderField *pOrder)
{
	return ((pOrder->FrontID == m_frontIdTrade) &&
		(pOrder->SessionID == m_sessionIdTrade));
}
void CClient2Dlg::ReqQryInstrument(void)
{
	CThostFtdcQryInstrumentField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.InstrumentID, m_instrumentId);
	int newRequestId = ::InterlockedIncrement((long*)(&m_iRequestId));
	int iResult =  m_pUserApiTrade->ReqQryInstrument(&req, newRequestId);
	fout << "ReqQryInstrument" << "iResult" << iResult << "\n";
	TRACE("ReqQryInstrument,iResult=%d\n",iResult);
}
void CClient2Dlg::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	if (bIsLast && !IsErrorRspInfo(pRspInfo)){
		::PostMessage(m_hWnd,WM_SYS_INI,NULL,5);//��Լ��ѯ���
		ReqQryInstrumentMarginRate();
	}
}
void CClient2Dlg::OnRspQryInstrumentMarginRate(CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	if(pInstrumentMarginRate != NULL){
		/*
		int i = m_errors.AddString(_T("CTP��Լ��֤�������ȡ�ɹ�\r\n"));
		int top = i - N < 0 ? 0 : i - N;
		m_errors.SetTopIndex( top);
		*/
		TRACE("��֤����%2f,%2f,%2f,%2f\r\n",pInstrumentMarginRate->LongMarginRatioByMoney,pInstrumentMarginRate->LongMarginRatioByVolume,pInstrumentMarginRate->ShortMarginRatioByMoney,pInstrumentMarginRate->ShortMarginRatioByVolume);
		fout1 << "��֤����" << pInstrumentMarginRate->LongMarginRatioByMoney << "\r\n";
		//m_isUpdateMarginRatioCtp = true;
		::PostMessage(m_hWnd,WM_SYS_INI,NULL,3);//3����CTP��֤���ʼ�� 
		m_LongMarginRatio = pInstrumentMarginRate->LongMarginRatioByMoney;
		m_ShortMarginRatio = pInstrumentMarginRate->ShortMarginRatioByMoney;
		ReqQryTrade();
	}
}
void CClient2Dlg::ReqOrderInsert(CTradingDataCtp *pOrderCtp)
{
	pOrderCtp->volumeEnd = pOrderCtp->req.VolumeTotalOriginal;
	int newRequestId = ::InterlockedIncrement((long*)(&m_iRequestId));
	pOrderCtp->requestId = newRequestId;
	//�۸��޸�
	if(pOrderCtp->req.Direction == THOST_FTDC_D_Buy){
		pOrderCtp->req.LimitPrice = ((int)((g_ifAsk1 + 100) / 0.2)) * 0.2;
	}
	else{
		pOrderCtp->req.LimitPrice = ((int)((g_ifBid1 - 100) / 0.2)) * 0.2;
	}
	int iResult = m_pUserApiTrade->ReqOrderInsert(&(pOrderCtp->req),newRequestId);
	fout << "¼�붩���Ѿ�����:" << "iResult = " << iResult << "\n";
	fout << "ǰ��ID" << pOrderCtp->frontIdTrade << "�ỰID" << pOrderCtp->sessionIdTrade << "\n";
	fout << "sysID" << pOrderCtp->sysId << "requestId" << pOrderCtp->requestId << "OrderRef" << pOrderCtp->req.OrderRef
		<< "�����ﷵ�ص�����ID" << pOrderCtp->req.RequestID << "\n";
	fout << "OrderRef" << pOrderCtp->req.OrderRef << "PriceRef" << pOrderCtp->priceRef << " Direction " << pOrderCtp->req.Direction 
		<< "O/C" <<pOrderCtp->req.CombOffsetFlag[0] << " ��Ŀ "<< pOrderCtp->req.VolumeTotalOriginal << " �۸� " << pOrderCtp->req.LimitPrice << "\n";
	TRACE("����¼��\r\n");
}
void CClient2Dlg::CalDeviation(){
	m_dltInterest = (m_lifeA50 - m_lifeIf) / 365.0 * 0.0631;//6.31%���޷�������
	m_deviationAvg = (g_a50Bid1 + g_a50Ask1) / 2.0 - (g_ifAsk1 + g_ifBid1) / 2.0 * g_A50Index / g_HS300Index
		- g_A50Index * m_dltInterest - m_datumDiff; 
	m_deviationSell = g_a50Ask1 - g_ifBid1 * g_A50Index / g_HS300Index
		- g_A50Index * m_dltInterest - m_datumDiff;
	m_deviationBuy = g_a50Bid1 - g_ifAsk1 * g_A50Index / g_HS300Index
		- g_A50Index * m_dltInterest - m_datumDiff;
	TRACE("����%.4f,������%.2f\r\n",m_dltInterest,m_deviationAvg);
}
double CClient2Dlg::dealA50Price(bool isbuy, double A50Price)
{
	double rtn;
	if(isbuy){
		fout << "A50�� " << A50Price;
	}
	else{
		fout << "A50�� " << A50Price;
	}
	if(isbuy){
		if(((int)A50Price) % 5 < 4){
			rtn = (double)(((int)A50Price) / 5 * 5);
		}
		else{
			rtn = (double)(((int)A50Price) / 5 * 5 + 5);
		}
	}
	else{
		if(((int)A50Price) % 5 >= 1){
			rtn = (double)(((int)A50Price) / 5 * 5 + 5);
		}
		else{
			rtn = (double)(((int)A50Price) / 5 * 5);
		}
	}
	fout << " �����۸� " << rtn << "\r\n";
	return rtn;
}
int CClient2Dlg::ReqDeleteOrder(CTradingDataCtp * pOrder)
{
	CThostFtdcInputOrderActionField req;
	memset(&req, 0, sizeof(req));
	///���͹�˾����
	strcpy(req.BrokerID, pOrder->req.BrokerID);
	///Ͷ���ߴ���
	strcpy(req.InvestorID, pOrder->req.InvestorID);
	///������������
	//	TThostFtdcOrderActionRefType	OrderActionRef;
	///��������
	strcpy(req.OrderRef, pOrder->req.OrderRef);
	///������
	//	TThostFtdcRequestIDType	RequestID;
	///ǰ�ñ��
	req.FrontID = pOrder->frontIdTrade;
	///�Ự���
	req.SessionID = pOrder->sessionIdTrade;
	///����������
	//	TThostFtdcExchangeIDType	ExchangeID;
	///�������
	//	TThostFtdcOrderSysIDType	OrderSysID;
	///������־
	req.ActionFlag = THOST_FTDC_AF_Delete;
	///�۸�
	//	TThostFtdcPriceType	LimitPrice;
	///�����仯
	//	TThostFtdcVolumeType	VolumeChange;
	///�û�����
	//	TThostFtdcUserIDType	UserID;
	///��Լ����
	strcpy(req.InstrumentID, pOrder->req.InstrumentID);
	int newRequestId = ::InterlockedIncrement((long*)(&m_iRequestId));
	pOrder->requestId = newRequestId;
	int iResult = m_pUserApiTrade->ReqOrderAction(&req,newRequestId);
	fout << "ɾ�������Ѿ�����:" << "iResult = " << iResult << "\n";
	fout << "ǰ��ID" << pOrder->frontIdTrade << "�ỰID" << pOrder->sessionIdTrade << "\n";
	fout << "sysID" << pOrder->sysId << "requestId" << pOrder->requestId << "OrderRef" << pOrder->req.OrderRef
		<< "�����ﷵ�ص�����ID" << pOrder->req.RequestID << "\n";
	fout << "OrderRef" << pOrder->req.OrderRef << "PriceRef" << pOrder->priceRef << " Direction " << pOrder->req.Direction 
		<< "O/C" <<pOrder->req.CombOffsetFlag[0] << " ��Ŀ "<< pOrder->req.VolumeTotalOriginal << " �۸� " << pOrder->req.LimitPrice << "\n";
	return 0;
}
void CClient2Dlg::OnBnClickedUpdate()
{
	UpdateData();
}
void CClient2Dlg::OnPause()
{
	m_isPause = true;
	m_PauseButton.SetWindowTextA(_T("Continue"));
	m_PauseButton.SetDlgCtrlID(IDC_CONTINUE);
	TRACE("��ͣ����\r\n");
}
void CClient2Dlg::OnContinue(){
	m_isPause = false;
	m_PauseButton.SetWindowTextA(_T("Pause"));
	m_PauseButton.SetDlgCtrlID(IDC_PAUSE);
	TRACE("��������\r\n");
}
void CClient2Dlg::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	if(pInputOrderAction){
		fout1 << "OnRspOrderAction" << pInputOrderAction->OrderRef << "  " << pInputOrderAction->OrderSysID
			<< "  " << pInputOrderAction->RequestID << "\n";
	}
}
void CClient2Dlg::OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo){
	if(pOrderAction){
		fout1 << "OnErrRtnOrderAction" << " " << pOrderAction->OrderRef << " " << pOrderAction->OrderSysID 
			<< " "<< pOrderAction->RequestID << "\n";
	}
}
void CClient2Dlg::ReqQryDepthMarketData(void)
{
	CThostFtdcQryDepthMarketDataField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.InstrumentID,m_instrumentId);
	int newRequestId = ::InterlockedIncrement((long*)(&m_iRequestId));
	int iResult = m_pUserApiTrade->ReqQryDepthMarketData(&req,newRequestId);
	fout << "ReqQryDepthMarketData" << "iResult" << iResult << "\n";
	TRACE("ReqQryDepthMarketData,iResult=%d\n",iResult);
}
void CClient2Dlg::OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	if (bIsLast && !IsErrorRspInfo(pRspInfo)){
		m_preSettlementPriceCtp = pDepthMarketData->PreSettlementPrice;
		m_ifAsk10 = m_preSettlementPriceCtp;
	    m_ifBid10 = m_preSettlementPriceCtp;
		::PostMessage(m_hWnd,WM_SYS_INI,NULL,4);//4�����Լ���ս���۳�ʼ��
		ReqQryTradingAccount();
	}
}
void CClient2Dlg::ReqQryInstrumentMarginRate(void)
{
	//��ѯ��Լ��֤��
	CThostFtdcQryInstrumentMarginRateField req;
	memset(&req,0,sizeof(req));
	strcpy(req.BrokerID,m_brokerId);
	strcpy(req.InvestorID,m_investorId);
	strcpy(req.InstrumentID, m_instrumentId);
	req.HedgeFlag = '1';
	int newRequestId = ::InterlockedIncrement((long*)(&m_iRequestId));
	int iResult = m_pUserApiTrade->ReqQryInstrumentMarginRate(&req,newRequestId);
	fout << "ReqQryInstrumentMarginRate" << "iResult" << iResult << "\n";
	TRACE("ReqQryInstrumentMarginRate,iResult=%d\n",iResult);
}
void CClient2Dlg::ReqQryInvestorPositionDetail(void){
	CThostFtdcQryInvestorPositionDetailField req;
	memset(&req,0,sizeof(req));
	strcpy(req.BrokerID,m_brokerId);
	strcpy(req.InvestorID,m_investorId);
	strcpy(req.InstrumentID, m_instrumentId);
	int newRequestId = ::InterlockedIncrement((long*)(&m_iRequestId));
	m_pUserApiTrade->ReqQryInvestorPositionDetail(&req,newRequestId);
}
void CClient2Dlg::OnRspQryInvestorPositionDetail(CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){

}
void CClient2Dlg::ReqQryTrade(){
	CThostFtdcQryTradeField req;
	memset(&req,0,sizeof(req));
	strcpy(req.BrokerID,m_brokerId);
	strcpy(req.InstrumentID, m_instrumentId);
	strcpy(req.InvestorID,m_investorId);
	/*
	strcpy(req.ExchangeID);
	strcpy(req.TradeID);
	strcpy(req.TradeTimeEnd);
	strcpy(req.TradeTimeStart);
	*/
	int newRequestId = ::InterlockedIncrement((long*)(&m_iRequestId));
	int iResult = m_pUserApiTrade->ReqQryTrade(&req,newRequestId);
	fout << "ReqQryTrade" << "iResult" << iResult << "\n";
	TRACE("ReqQryTrade,iResult=%d\n",iResult);
}
void CClient2Dlg::OnRspQryTrade(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	CTradingDataCtp CTDC(m_brokerId,m_investorId,m_instrumentId);
	if(!IsErrorRspInfo(pRspInfo)){
		if(!pTrade && bIsLast){
			TRACE("û�гɽ���Ϣ\r\n");
			::PostMessage(m_hWnd,WM_SYS_INI,NULL,8);//8����CTP�ɽ���ʼ��
		}
		else{
			CTDC.expired = true;
			TRACE("�ɽ�����%d\r\n",pTrade->Volume);
			CTDC.volumeFilled = pTrade->Volume;
			CTDC.avgFilledPrice = pTrade->Price;
			if(pTrade->Direction == THOST_FTDC_D_Buy){
				CTDC.req.Direction = THOST_FTDC_D_Buy;
			}
			else{
				CTDC.req.Direction = THOST_FTDC_D_Sell;
			}
			m_CTradingDataCtp.push_back(CTDC);
			if(bIsLast){
				TRACE("���ײ�ѯ����\r\n"); 	
				::PostMessage(m_hWnd,WM_SYS_INI,NULL,8);//8����CTP�ɽ���ʼ��
			}
		}
	}
}
void CClient2Dlg::OnBnClickedButton10()
{
	fout << std::endl;
	fout1 << std::endl;
}
