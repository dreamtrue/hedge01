#include <StdAfx.h>
#include "IBTRADE\client2Dlg.h"
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
extern std::fstream fout;//输出日志
extern std::fstream fout1;//ctp日志
extern double g_a50Bid1,g_a50Ask1;
extern int g_a50Bid1Size,g_a50Ask1Size;
extern double g_ifBid1,g_ifAsk1;
extern double g_A50Index,g_HS300Index;
extern ParamCtpMd ParamMdCtp;//ctp行情系统初始参数,包括账号、合约等等
extern ParamCtpTd ParamTdCtp;
extern int getIndexData();
const int N = 5;
LRESULT CClient2Dlg::OnAllSysIni(UINT wParam, long lParam)
{
	if(lParam == 1){
		m_isIbReady = true;
		int i = m_errors.AddString(_T("IB交易系统初始化完毕\r\n"));
		int top = i - N < 0 ? 0 : i - N;
		m_errors.SetTopIndex( top);
	}
	if(lParam == 2){
		m_isCtpReady = true;
		int i = m_errors.AddString(_T("Ctp交易交易确认完毕\r\n"));
		int top = i - N < 0 ? 0 : i - N;
		m_errors.SetTopIndex( top);
	}
	if(lParam == 3){
		m_isUpdateMarginRatioCtp = true;
		int i = m_errors.AddString(_T("Ctp保证金初始化完毕\r\n"));
		int top = i - N < 0 ? 0 : i - N;
		m_errors.SetTopIndex( top);
	}
	if(lParam == 4){
		m_isUpdateSettlementPriceCtp = true;
		int i = m_errors.AddString(_T("IF合约结算价获取完毕\r\n"));
		int top = i - N < 0 ? 0 : i - N;
		m_errors.SetTopIndex( top);
	}
	if(lParam == 5){
		m_isQryInstrumentCtp = true;
		int i = m_errors.AddString(_T("IF合约查询完毕\r\n"));
		int top = i - N < 0 ? 0 : i - N;
		m_errors.SetTopIndex( top);
	}
	if(lParam == 6){
		m_isNetIniCtp = true;
		int i = m_errors.AddString(_T("CTP静态权益初始化完毕\r\n"));
		int top = i - N < 0 ? 0 : i - N;
		m_errors.SetTopIndex( top);
	}
	if(lParam == 7){
		m_isPorliIniCtp = true;
		int i = m_errors.AddString(_T("CTP持仓初始化完毕\r\n"));
		int top = i - N < 0 ? 0 : i - N;
		m_errors.SetTopIndex( top);
	}
	if(lParam == 8){
		m_isReqTradeCtp = true;
		int i = m_errors.AddString(_T("CTP成交初始化完毕\r\n"));
		int top = i - N < 0 ? 0 : i - N;
		m_errors.SetTopIndex( top);
	}
	if(m_isIbReady && m_isCtpReady && m_isUpdateMarginRatioCtp && m_isUpdateSettlementPriceCtp
		&& m_isQryInstrumentCtp && m_isNetIniCtp && m_isPorliIniCtp && m_isReqTradeCtp){
			GetDlgItem(IDC_HEDGE_SETUP)->EnableWindow(1);
			m_isIbReady = false;
			m_isCtpReady = false;
			m_isUpdateMarginRatioCtp = false;
			m_isUpdateSettlementPriceCtp = false;
			m_isQryInstrumentCtp = false;
			m_isNetIniCtp = false;
			m_isPorliIniCtp = false;
			m_isReqTradeCtp = false;
	}
	return LRESULT();
}

LRESULT CClient2Dlg::HedgeLoop(UINT wParam, long lParam)
{
	/*
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	fout1 << "========================================================================" << "\n";//换行
	fout << "========================================================================" << "\n";//换行
	fout1 << "时刻" << sys.wMonth << "/" << sys.wDay << "/" << sys.wHour << "/" << sys.wMinute << "/"
		<< sys.wSecond << "/" << sys.wMilliseconds << "\n";
	fout << "时刻" << sys.wMonth << "/" << sys.wDay << "/" << sys.wHour << "/" << sys.wMinute << "/"
		<< sys.wSecond << "/" << sys.wMilliseconds << "\n";
		*/
	bool isPriceCorrect = true;
	//TRACE("时间%d/%d/%d/%d\r\n",sys.wHour,sys.wMinute,sys.wSecond,sys.wMilliseconds);
	if(g_ifBid1 > 200000 || g_ifBid1 < 100 || _isnan(g_ifBid1)!= 0){
		g_ifBid1 = m_ifBid10;
		isPriceCorrect = false;
		fout << " g_ifBid1价格不合理 " << "\n";
	}
	if(g_ifAsk1 > 200000 || g_ifAsk1 < 100 || _isnan(g_ifAsk1)!= 0){
		g_ifAsk1 = m_ifAsk10;
		isPriceCorrect = false;
		fout << " g_ifAsk1价格不合理 " << "\n";
	}
	if(g_ifAsk1 == m_ifAsk10 && g_ifBid1 == m_ifBid10){
		isPriceCorrect = false;
		fout << " IF价格与昨天结算价一样 " << "\n";
	}
	//double ladder[21] = {-9.5,-8.5,-7.5,-6.5,-5.5,-4.5,-3.5,-2.5,-1.5,-0.5,0.5,1.5,2.5,3.5,4.5,5.5,6.5,7.5,8.5,9.5,10.5};//梯级划分
	double ladder[21] = {-10,-9,-8,-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7,8,9,10};
	for(int i= 0;i< 21;i++){
		ladder[i] = m_ladderMulti * ladder[i];
	}
	int aimPortfolioLadder[21] = {10,9,8,7,6,5,4,3,2,1,0,-1,-2,-3,-4,-5,-6,-7,-8,-9,-10};//目标持仓梯级
	std::list<CTradingDataIb>::iterator iTDIB;
	int portfolioCtpEST;//期望持仓
	int nowLadder = 0;
	int limitLongPofolioIb = 0,limitShortPofolioIb = 0;//Ib账户最大多头和空头持仓
	int limitLongPofolioCtp = 0,limitShortPofolioCtp = 0;//ctp账户最大多头和空头持仓
	int limitLongPofolioHedge = 0,limitShortPofolioHedge = 0;//对冲账户持仓限制
	static int limitLongPofolioHedge_save = 1000,limitShortPofolioHedge_save = 1000;//初始值设大点
	int tradingSellLessThanNowladderIb = 0,tradingBuyMoreThanNowladderIb = 0,tradingSellMoreThanNowladderIb = 0,tradingBuyLessThanNowladderIb = 0;
	CString label;
	m_tradingPortfolioBuyCloseCtp = 0;
	m_tradingPortfolioBuyOpenCtp = 0;
	m_tradingPortfolioBuyCloseCtp = 0;
	m_tradingPortfolioBuyOpenCtp = 0;
	m_tradingPortfolioSellOpenCtp = 0;
	m_tradingPortfolioSellCloseCtp = 0;
	m_tradingPortfolioSellCtp = 0;
	m_tradingPortfolioBuyCtp = 0;
	m_tradingPortfolioBuyIb = 0;
	m_tradingPortfolioSellIb = 0;
	m_portfolioCtp = 0;
	m_portfolioIb = 0;
	m_netCtp = 0;
	m_netIb = 0;
	//更新显示参数
	label.Format(_T("A50指数%.2f"),g_A50Index);
	m_A50IndexLabel.SetWindowTextA(label);
	label.Format(_T("300指数%.2f"),g_HS300Index);
	m_HS300IndexLabel.SetWindowTextA(label);
	label.Format(_T("A50买一%.2f"),g_a50Bid1);
	m_A50Bid1Label.SetWindowTextA(label);
	label.Format(_T("A50卖一%.2f"),g_a50Ask1);
	m_A50Ask1Label.SetWindowTextA(label);
	label.Format(_T("IF买一%.2f"),g_ifBid1);
	m_ifBid1Label.SetWindowTextA(label);
	label.Format(_T("IF卖一%.2f"),g_ifAsk1);
	m_ifAsk1Label.SetWindowTextA(label);
	CalDeviation();//计算相对溢价
	label.Format(_T("绝对偏差%.2f"),m_deviationAvg + m_datumDiff);
	m_deviationLabel.SetWindowTextA(label);
	fout << "绝对偏差" << m_deviationAvg + m_datumDiff << "基准" << m_datumDiff << "\n";
	//行情是否存在问题
	if(g_A50Index < 0.1 || g_HS300Index < 0.1 || g_ifAsk1 < 0.1 || g_ifBid1 < 0.1 || g_a50Bid1 < 0.1 || g_a50Ask1 < 0.1 
		|| g_ifAsk1 > 200000 || g_ifBid1 >200000 || g_a50Ask1 > 800000 | g_a50Ask1 > 800000){
			TRACE("价格存在问题,返回\r\n");
			return -1;
	}
	if(_isnan(m_datumDiff) != 0 || _isnan(m_deviationAvg)!=0){
		return -1;//判断非零值错误
	}
	if(m_deviationSell >= 1000 || m_deviationBuy <= -1000){
		return -1;//超过范围，返回
	}
	/*
	//非交易日返回
	if(!isTradeDay(sys.wYear,sys.wMonth,sys.wDay))
	{
		return -1;
	}
	*/
	if(!m_isHedgeLoopSetUp){
		return -1;
	}
	if(m_isPause){
		return -1;
	}
	fout << "A50指数" << g_A50Index << "HS300指数" << g_HS300Index << "A50价格" << g_a50Bid1
		<< "  " << g_a50Ask1 << "IF价格" << g_ifBid1 << "  " << g_ifAsk1 << "\n";
	//获取成交信息
	std::map<CString,std::list<CThostFtdcTradeField>>::iterator iTradeRtn;
	std::list<CThostFtdcTradeField>::iterator iTradeRtnVec;
	std::list<CTradingDataCtp>::iterator iTDCTP;//迭代器
	CTradingDataCtp CTDC(m_brokerId,m_investorId,m_instrumentId);//命令初始化
	CTradingDataCtp *pNewDataCtp = &CTDC;
	CTradingDataIb CDIB;//命令初始化
	CTradingDataIb *pNewDataIb = &CDIB;
	fout << "#######################统计成交，并决定是否重新下单########################\n";
	std::list<CTradingDataCtp>::iterator iEndCtp = m_CTradingDataCtp.end();
	for(iTDCTP = m_CTradingDataCtp.begin();iTDCTP != iEndCtp;iTDCTP++){
		if(iTDCTP->expired){
			continue;
		}
		fout << "sysID" << iTDCTP->sysId << "requestId" << iTDCTP->requestId << "OrderRef" << iTDCTP->req.OrderRef
			<< "命令里返回的请求ID" << iTDCTP->req.RequestID << "\n";
		iTradeRtn = m_ifRtn.find(CString(iTDCTP->sysId));
		if(iTradeRtn != m_ifRtn.end()){//找到了键值
			fout << "成交回报里找到系统ID" << iTDCTP->sysId << "\n";
			for(iTradeRtnVec = iTradeRtn->second.begin();iTradeRtnVec != iTradeRtn->second.end();iTradeRtnVec++){
				iTDCTP->avgFilledPrice = (iTDCTP->avgFilledPrice * iTDCTP->volumeFilled  + iTradeRtnVec->Price * iTradeRtnVec->Volume) / (iTDCTP->volumeFilled + iTradeRtnVec->Volume);
				iTDCTP->volumeFilled = iTDCTP->volumeFilled + iTradeRtnVec->Volume;
			}
			fout << " 成交平均价 " << iTDCTP->avgFilledPrice << " 已成交张数 " << iTDCTP->volumeFilled << "\n";
			iTradeRtn->second.clear();//清空本键值对应的成交信息
		}
		if(iTDCTP->volumeFilled == iTDCTP->volumeEnd){
			fout << "sysID" << iTDCTP->sysId << " 成交统计结束 " << "总张数" << iTDCTP->volumeEnd << "\n";
			iTDCTP->expired = true;
			fout << "sysID" << "有效状态" << iTDCTP->expired << "\n";
			if(iTDCTP->volumeEnd < iTDCTP->req.VolumeTotalOriginal){
				fout << " 结束时未成交张数 " << iTDCTP->req.VolumeTotalOriginal - iTDCTP->volumeEnd << "\n";
				if(iTDCTP->isNeedNewOrder){//重新下单
					fout << "重新下单" << "\n";
					pNewDataCtp->reset();
					pNewDataCtp->req = iTDCTP->req;
					if(iTDCTP->req.Direction == THOST_FTDC_D_Buy){
						pNewDataCtp->req.VolumeTotalOriginal = iTDCTP->req.VolumeTotalOriginal - iTDCTP->volumeFilled;
						TRACE("CTP本次成交数目%d,撤单,新订单数目%d\r\n",iTDCTP->volumeFilled,iTDCTP->req.VolumeTotalOriginal - iTDCTP->volumeFilled);
						pNewDataCtp->priceRef = iTDCTP->priceRef;
						int newRef = ::InterlockedIncrement((long*)(&m_maxOrderRef));
						sprintf(pNewDataCtp->req.OrderRef,"%d",newRef);
						if(g_ifAsk1 > pNewDataCtp->priceRef + 7.0){//7个点作为止损点
							pNewDataCtp->req.LimitPrice = g_ifAsk1 + 100.0;//强行成交
							fout << g_ifAsk1 << " " << pNewDataCtp->priceRef << "紧急止损\n";
						}
						else{
							pNewDataCtp->req.LimitPrice = g_ifBid1;
						}
						fout << "新订单" << "报单引用" << pNewDataCtp->req.OrderRef << " 买(平/开)标志" << pNewDataCtp->req.CombOffsetFlag[0] << ":"<< pNewDataCtp->req.VolumeTotalOriginal 
							<< "手," << "价格" << pNewDataCtp->req.LimitPrice << "参考价格" << pNewDataCtp->priceRef << "\n";
					}
					else{
						pNewDataCtp->req.VolumeTotalOriginal = iTDCTP->req.VolumeTotalOriginal - iTDCTP->volumeFilled;
						TRACE("CTP本次成交数目%d,撤单,新订单数目%d\r\n",iTDCTP->volumeFilled,iTDCTP->req.VolumeTotalOriginal - iTDCTP->volumeFilled);
						pNewDataCtp->priceRef = iTDCTP->priceRef;
						int newRef = ::InterlockedIncrement((long*)(&m_maxOrderRef));
						sprintf(pNewDataCtp->req.OrderRef,"%d",newRef);
						if(g_ifBid1 < pNewDataCtp->priceRef - 7.0){//7个点作为止损点
							pNewDataCtp->req.LimitPrice = g_ifBid1 - 100.0;//强行成交
							fout << g_ifBid1 << " " << pNewDataCtp->priceRef << "紧急止损\n";
						}
						else{
							pNewDataCtp->req.LimitPrice = g_ifAsk1;
						}
						fout << "新订单" << "报单引用" << pNewDataCtp->req.OrderRef << " 卖(平/开)标志" << pNewDataCtp->req.CombOffsetFlag[0] << ":"<< pNewDataCtp->req.VolumeTotalOriginal 
							<< "手," << "价格" << pNewDataCtp->req.LimitPrice << "参考价格" << pNewDataCtp->priceRef << "\n";
					}
					//ReqOrderInsert(pNewDataCtp);
					pNewDataCtp->waitStatus = 1;
					//链表末尾增加一项
					m_CTradingDataCtp.push_back(*pNewDataCtp);
					fout << "新订单状态" << pNewDataCtp->waitStatus << "是否有效" << pNewDataCtp->expired << "链表末尾增加订单数据\n"; 
				}
			}
		}
	}
	fout << "#######################计算梯级和溢价########################\n";
	///////////////////////////////////////////////////
	for(int i = 0;i < 21;i++){
		aimPortfolioLadder[i] = aimPortfolioLadder[i] * m_aimMulti;
	}
	for(int i = 0;i < 21;i++){
		static bool isFirst = true;
		if(i == 0){
			if(m_deviationAvg < ladder[i]){
				nowLadder = 0;
				break;
			}
		}
		if(i == 20){
			if(m_deviationAvg >= ladder[i]){
				nowLadder = 20;
				break;
			}
		}
		if(i < 20 && i > 0){
			if(m_deviationAvg >= ladder[i] && m_deviationAvg < ladder[i + 1]){
				nowLadder = i;
				break;//找到ladder后立刻跳出
			}
		}
	}
	fout << "相对溢价" << m_deviationAvg << "\n";
	fout << "节点位置" << nowLadder << " " << ladder[nowLadder] << "  " << ladder[nowLadder + 1] << "\n";
	/////////////////////////////////////////////////////////////////////////
	fout << "#######################持仓和昨日持仓利润初始化########################\n";
	//叠加原有持仓利润
	//m_netCtp = m_netCtp0 + m_portfolioCtp0 * ((g_ifAsk1 + g_ifBid1) / 2.0 - (m_ifAsk10 + m_ifBid10) / 2.0) * m_valuePointCtp;
	m_netCtp = m_netCtp0;
	//m_portfolioCtp = m_portfolioCtp0;
	m_portfolioCtp = 0;
	fout << "昨日ctp持仓 " << m_portfolioCtp0 << " 本次初始资产净值 " << m_netCtp0 << "\n";
	//*******************************************************************************************************************
	fout << "#######################统计IF正在成交中的情况########################\n";
	//对if进行处理
	for(iTDCTP = m_CTradingDataCtp.begin();iTDCTP != m_CTradingDataCtp.end();iTDCTP++){
		//叠加新持仓利润,以及净持仓
		if(iTDCTP->req.Direction == THOST_FTDC_D_Buy){
			m_netCtp = m_netCtp + ((g_ifAsk1 + g_ifBid1) / 2.0 - iTDCTP->avgFilledPrice * (1.0 + 0.000055/*佣金*/)) * m_valuePointCtp * iTDCTP->volumeFilled;
			m_portfolioCtp = m_portfolioCtp + iTDCTP->volumeFilled;
			if(!iTDCTP->expired){//交易中的合约
				if(iTDCTP->req.CombOffsetFlag[0] == THOST_FTDC_OF_Open){//买开
					m_tradingPortfolioBuyOpenCtp = m_tradingPortfolioBuyOpenCtp + iTDCTP->req.VolumeTotalOriginal - iTDCTP->volumeFilled;
					fout << "正在买开" << "报单引用" << iTDCTP->req.OrderRef << "请求ID(req中的)" << iTDCTP->requestId << "系统ID" << iTDCTP->sysId
						<< iTDCTP->req.VolumeTotalOriginal - iTDCTP->volumeFilled << "价格" << iTDCTP->req.LimitPrice << "\n";
				}
				else{//买平
					m_tradingPortfolioBuyCloseCtp = m_tradingPortfolioBuyCloseCtp + iTDCTP->req.VolumeTotalOriginal - iTDCTP->volumeFilled;
					fout << "正在买平" << "报单引用" << iTDCTP->req.OrderRef << "请求ID(req中的)" << iTDCTP->requestId << "系统ID" << iTDCTP->sysId
						<< iTDCTP->req.VolumeTotalOriginal - iTDCTP->volumeFilled << "价格" << iTDCTP->req.LimitPrice << "\n";
				}
			}
		}
		else{
			m_netCtp = m_netCtp + (iTDCTP->avgFilledPrice * (1.0 - 0.000055/*佣金*/) - (g_ifAsk1 + g_ifBid1) / 2.0) * m_valuePointCtp * iTDCTP->volumeFilled;
			m_portfolioCtp = m_portfolioCtp - iTDCTP->volumeFilled;
			if(!iTDCTP->expired){//交易中的合约
				if(iTDCTP->req.CombOffsetFlag[0] == THOST_FTDC_OF_Open){//卖开
					m_tradingPortfolioSellOpenCtp = m_tradingPortfolioSellOpenCtp + iTDCTP->req.VolumeTotalOriginal - iTDCTP->volumeFilled;
					fout << "正在卖开" << "报单引用" << iTDCTP->req.OrderRef << "请求ID(req中的)" << iTDCTP->requestId << "系统ID" << iTDCTP->sysId
						<< iTDCTP->req.VolumeTotalOriginal - iTDCTP->volumeFilled << "价格" << iTDCTP->req.LimitPrice << "\n";
				}
				else{//卖平
					m_tradingPortfolioSellCloseCtp = m_tradingPortfolioSellCloseCtp + iTDCTP->req.VolumeTotalOriginal - iTDCTP->volumeFilled;
					fout << "正在卖平" << "报单引用" << iTDCTP->req.OrderRef << "请求ID(req中的)" << iTDCTP->requestId << "系统ID" << iTDCTP->sysId
						<< iTDCTP->req.VolumeTotalOriginal - iTDCTP->volumeFilled << "价格" << iTDCTP->req.LimitPrice << "\n";
				}
			}
		}
	}
	fout << "正在买开IF总数" << m_tradingPortfolioBuyOpenCtp << "正在买平IF总数" << m_tradingPortfolioBuyCloseCtp << "\n";
	fout << "正在卖开IF总数" << m_tradingPortfolioSellOpenCtp << "正在卖平IF总数" << m_tradingPortfolioSellCloseCtp << "\n";
	m_tradingPortfolioBuyCtp = m_tradingPortfolioBuyOpenCtp + m_tradingPortfolioBuyCloseCtp;
	m_tradingPortfolioSellCtp = m_tradingPortfolioSellOpenCtp + m_tradingPortfolioSellCloseCtp;
	portfolioCtpEST = m_tradingPortfolioBuyCtp - m_tradingPortfolioSellCtp + m_portfolioCtp;
	label.Format(_T("IF买平%d"),m_tradingPortfolioBuyCloseCtp);
	m_IFBuyClose.SetWindowTextA(label);
	label.Format(_T("IF买开%d"),m_tradingPortfolioBuyOpenCtp);
	m_IFBuyOpen.SetWindowTextA(label);
	label.Format(_T("IF卖平%d"),m_tradingPortfolioSellCloseCtp);
	m_IFSellClose.SetWindowTextA(label);
	label.Format(_T("IF卖开%d"),m_tradingPortfolioSellOpenCtp);
	m_IFSellOpen.SetWindowTextA(label);
	TRACE("CTP总资产(忽略佣金)%.f,净持仓%d,期望持仓%d\r\n",m_netCtp,m_portfolioCtp,portfolioCtpEST);
	fout << "CTP总资产 "<< m_netCtp << "净持仓" << m_portfolioCtp << "交易中的买单" << m_tradingPortfolioBuyCtp << "交易中的卖单" << m_tradingPortfolioSellCtp << "\n";
	//对A50进行处理
	//原有A50持仓利润
	fout << "#######################计算A50原有持仓及利润初始化########################\n";
	m_netIb = m_netIb0 + m_portfolioIb0 * ((g_a50Ask1 + g_a50Bid1) / 2.0 - (m_a50Ask10 + m_a50Bid10) / 2.0) * m_valuePointIb;
	m_portfolioIb = m_portfolioIb0;
	fout << "A50原有持仓" << m_portfolioIb << ",净值现在为" << m_netIb << "\n";
	fout << "####################################IB持仓及利润计算################################\n";
	for(iTDIB = m_CTradingDataIb.begin();iTDIB != m_CTradingDataIb.end();iTDIB++){
		//叠加新持仓利润,以及净持仓
		if(strcmp(iTDIB->IbOrder.action,"BUY") == 0){
			m_netIb = m_netIb + ((g_a50Ask1 + g_a50Bid1) / 2.0 - (iTDIB->avgFilledPrice + 1.5/*佣金*/))* iTDIB->volumeFilled * m_valuePointIb;
			m_portfolioIb = m_portfolioIb + iTDIB->volumeFilled;
			if(!iTDIB->expired && iTDIB->waitStatus == 0){//交易中的合约
				m_tradingPortfolioBuyIb = m_tradingPortfolioBuyIb + iTDIB->IbOrder.totalQuantity - iTDIB->volumeFilled;
				if(iTDIB->ladderRef > nowLadder){//高于当前节点的买价单(大部分时候应该已经成交了)
					tradingBuyMoreThanNowladderIb = tradingBuyMoreThanNowladderIb + iTDIB->IbOrder.totalQuantity - iTDIB->volumeFilled;
				}
				else if(iTDIB->ladderRef < nowLadder){
					tradingBuyLessThanNowladderIb = tradingBuyLessThanNowladderIb + iTDIB->IbOrder.totalQuantity - iTDIB->volumeFilled;
				}
			}
		}
		else{
			m_netIb = m_netIb + ((iTDIB->avgFilledPrice - 1.5/*佣金*/) - (g_a50Ask1 + g_a50Bid1) / 2.0) * iTDIB->volumeFilled * m_valuePointIb;
			m_portfolioIb = m_portfolioIb - iTDIB->volumeFilled;
			if(!iTDIB->expired && iTDIB->waitStatus == 0){//交易中的合约
				m_tradingPortfolioSellIb = m_tradingPortfolioSellIb + iTDIB->IbOrder.totalQuantity - iTDIB->volumeFilled;
				if(iTDIB->ladderRef < nowLadder){//低于当前节点的卖价单(大部分时候应该已经成交了)
					tradingSellLessThanNowladderIb = tradingSellLessThanNowladderIb + iTDIB->IbOrder.totalQuantity - iTDIB->volumeFilled;
				}
				else if(iTDIB->ladderRef > nowLadder){
					tradingSellMoreThanNowladderIb = tradingSellMoreThanNowladderIb + iTDIB->IbOrder.totalQuantity - iTDIB->volumeFilled;
				}
			}
		}
	}
	label.Format(_T("A50买%d"),m_tradingPortfolioBuyIb);
	m_A50BuyLabel.SetWindowTextA(label);
	label.Format(_T("A50卖%d"),m_tradingPortfolioSellIb);
	m_A50SellLabel.SetWindowTextA(label);
	TRACE("IB总资产%.f,净持仓%d,交易中的买单%d,交易中的卖单%d\r\n",m_netIb,m_portfolioIb,m_tradingPortfolioBuyIb,m_tradingPortfolioSellIb);
	fout << "IB总资产 "<< m_netIb << "净持仓" << m_portfolioIb << "交易中的买单" << m_tradingPortfolioBuyIb << "交易中的卖单" << m_tradingPortfolioSellIb << "\n";
	label.Format(_T("Ctp净资%.2f"),m_netCtp);
	m_CtpNetLabel.SetWindowTextA(label);
	label.Format(_T("Ctp净持%d"),m_portfolioCtp);
	m_ifPortLabel.SetWindowTextA(label);
	label.Format(_T("IB净资%.2f"),m_netIb);
	m_ibNetLabel.SetWindowTextA(label);
	label.Format(_T("IB净持%d"),m_portfolioIb);
	m_a50PortLabel.SetWindowTextA(label);
	label.Format(_T("总资%.2f"),m_netCtp + m_netIb * m_RMBtoUSD);
	m_totalLabel.SetWindowTextA(label);
	//*************************************************************************************************************************************
	//下单计算及指令发出
	//长短仓持仓限制计算,limit值永远为正
	fout << "############################长短仓限制及目标持仓计算################################\n";
	limitLongPofolioIb = max(0,(int)((m_netIb - 200.0)) / m_MarginA50);//留200USD余地,使用max(0,)的目的是让其不小于0
	limitShortPofolioIb = max(0,(int)((m_netIb - 200.0) / m_MarginA50));
	limitLongPofolioCtp = max(0,(int)((m_netCtp - 1000.0) / ((g_ifAsk1 + g_ifBid1) / 2.0 * m_LongMarginRatio *  m_valuePointCtp)));//留1000RMB余地
	limitShortPofolioCtp = max(0,(int)((m_netCtp - 1000.0) / ((g_ifAsk1 + g_ifBid1) / 2.0 * m_ShortMarginRatio * m_valuePointCtp)));
	limitLongPofolioHedge = min(limitLongPofolioIb / m_multiA50,limitShortPofolioCtp);
	limitShortPofolioHedge = min(limitShortPofolioIb / m_multiA50,limitLongPofolioCtp);
	//增加这部分判断保证当资金不够发生强行平仓时不会再因为资金短期波动满足要求重新加仓，必须手动系统重启才能加仓
	if(limitLongPofolioHedge > limitLongPofolioHedge_save){
		limitLongPofolioHedge = limitLongPofolioHedge_save;
	}
	if(limitShortPofolioHedge > limitShortPofolioHedge_save){
		limitShortPofolioHedge = limitShortPofolioHedge_save;
	}
	limitLongPofolioHedge_save = limitLongPofolioHedge;
	limitShortPofolioHedge_save = limitShortPofolioHedge;
	int aimPortfolioIb1 = 0,aimPortfolioIb2 = 0,aimPortfolioCtp1 = 0,aimPortfolioCtp2 = 0;//1和2分别代表nowLadder和nowLadder+1位置
	fout << "IB长仓限制" << limitLongPofolioIb << "短仓限制" << limitShortPofolioIb << "Ctp长仓限制" << limitLongPofolioCtp << "短仓限制" << limitShortPofolioCtp << "\n";
	fout << "Hedge长仓限制" << limitLongPofolioHedge << "Hedge短仓限制" << limitShortPofolioHedge << "\n";
	//进行限制持仓处理
	if(aimPortfolioLadder[nowLadder] > limitLongPofolioHedge){
		aimPortfolioIb1 = limitLongPofolioHedge * m_multiA50;
	}
	else if(aimPortfolioLadder[nowLadder] < -limitShortPofolioHedge){
		aimPortfolioIb1 = -limitShortPofolioHedge * m_multiA50;
	}
	else{
		aimPortfolioIb1 = aimPortfolioLadder[nowLadder] * m_multiA50;
	}
	if(aimPortfolioLadder[nowLadder + 1] > limitLongPofolioHedge){
		aimPortfolioIb2 = limitLongPofolioHedge * m_multiA50;
	}
	else if(aimPortfolioLadder[nowLadder + 1] < -limitShortPofolioHedge){
		aimPortfolioIb2 = -limitShortPofolioHedge * m_multiA50;
	}
	else{
		aimPortfolioIb2 = aimPortfolioLadder[nowLadder + 1] * m_multiA50;
	}
	fout << "aimPortfolioIb1 =" << aimPortfolioIb1 << "  " << "aimPortfolioIb2 =" << aimPortfolioIb2 << "\n";
	//需要增加买入\卖出Ctp的数目,跟节点无关，根据A50持仓进行调整
	int aimCtp = 0;
	int needBuyCtp = 0,needSellCtp = 0;
	if(m_portfolioIb > 0){
		aimCtp = -(abs(m_portfolioIb) + m_multiA50 / 2) / m_multiA50;
		//aimCtp = -m_portfolioIb / m_multiA50;
	}
	else{
		aimCtp = (abs(m_portfolioIb) + m_multiA50 / 2) / m_multiA50;
		//aimCtp = -m_portfolioIb / m_multiA50;
	}
	fout << "IB净持仓" << m_portfolioIb << " CTP目标持仓" << aimCtp << "\n";
	fout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ CTP操作 $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n";
	//价格不符合要求的ctp删除
	int needCancelDirect = m_portfolioCtp + m_tradingPortfolioBuyCtp - m_tradingPortfolioSellCtp - aimCtp;//需要直接删除的数量
	fout << "总共需要直接取消订单数目(正负)" << needCancelDirect << "\n";
	for(iTDCTP = m_CTradingDataCtp.begin();iTDCTP != m_CTradingDataCtp.end();iTDCTP++){
		if(!iTDCTP->expired){
			if(iTDCTP->req.Direction == THOST_FTDC_D_Buy){
				if(needCancelDirect > 0){//多余订单的删除操作，只有needCancelDirect == 0才停止这种操作
					//ReqDeleteOrder(&iTDCTP->req);
					iTDCTP->waitStatus = 3;
					needCancelDirect = needCancelDirect - (iTDCTP->req.VolumeTotalOriginal - iTDCTP->volumeFilled);
					fout << "本次直接取消数目" << " 买单 " << iTDCTP->req.VolumeTotalOriginal - iTDCTP->volumeFilled
						<< "剩余需取消数目" << needCancelDirect << " ";
					fout << "sysID" << iTDCTP->sysId << "requestId" << iTDCTP->requestId << "OrderRef" << iTDCTP->req.OrderRef
						<< "命令里返回的请求ID" << iTDCTP->req.RequestID << "\n";
				}
				else if(iTDCTP->req.LimitPrice < g_ifBid1){
					//ReqDeleteOrder(&iTDCTP->req);
					iTDCTP->waitStatus = 3;
					iTDCTP->isNeedNewOrder = true;
					fout << "买单价格偏低，取消，并设置需要重新下单标志" << "\n";
					fout << "取消数目" << " 买单 " << iTDCTP->req.VolumeTotalOriginal - iTDCTP->volumeFilled
						<< "剩余需取消数目" << needCancelDirect << " ";
					fout << "sysID" << iTDCTP->sysId << "requestId" << iTDCTP->requestId << "OrderRef" << iTDCTP->req.OrderRef
						<< "命令里返回的请求ID" << iTDCTP->req.RequestID << "\n";
				}
			}
			else{
				if(needCancelDirect < 0){//多余订单的删除操作，只有needCancelDirect == 0才停止这种操作
					//ReqDeleteOrder(&iTDCTP->req);
					iTDCTP->waitStatus = 3;
					needCancelDirect = needCancelDirect + (iTDCTP->req.VolumeTotalOriginal - iTDCTP->volumeFilled);
					fout << "本次直接取消数目" << " 卖单 " << iTDCTP->req.VolumeTotalOriginal - iTDCTP->volumeFilled;
					fout << "sysID" << iTDCTP->sysId << "requestId" << iTDCTP->requestId << "OrderRef" << iTDCTP->req.OrderRef
						<< "命令里返回的请求ID" << iTDCTP->req.RequestID << "\n";
				}
				else if(iTDCTP->req.LimitPrice > g_ifAsk1){
					//ReqDeleteOrder(&iTDCTP->req);
					iTDCTP->waitStatus = 3;
					iTDCTP->isNeedNewOrder = true;
					fout << "卖单价格偏高，取消，并设置需要重新下单标志" << "\n";
					fout << "取消数目" << " 卖单 " << iTDCTP->req.VolumeTotalOriginal - iTDCTP->volumeFilled;
					fout << "sysID" << iTDCTP->sysId << "requestId" << iTDCTP->requestId << "OrderRef" << iTDCTP->req.OrderRef
						<< "命令里返回的请求ID" << iTDCTP->req.RequestID << "\n";
				}
			}
		}
	}
	if(aimCtp - m_portfolioCtp - m_tradingPortfolioBuyCtp > 0){
		needBuyCtp = aimCtp - m_portfolioCtp - m_tradingPortfolioBuyCtp;
		fout << "需要买入数目" << needBuyCtp << "\n";
		pNewDataCtp->reset();
		pNewDataCtp->req.LimitPrice = g_ifBid1;
		pNewDataCtp->priceRef = g_ifBid1;
		pNewDataCtp->req.Direction = THOST_FTDC_D_Buy;
		if(m_portfolioCtp + m_tradingPortfolioBuyCtp < 0){
			if(needBuyCtp <= -(m_portfolioCtp + m_tradingPortfolioBuyCtp)){//只有平仓
				pNewDataCtp->req.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
				pNewDataCtp->req.VolumeTotalOriginal = needBuyCtp;
				int newRef = ::InterlockedIncrement((long*)(&m_maxOrderRef));
				sprintf(pNewDataCtp->req.OrderRef,"%d",newRef);
				fout << "ctp全买平" << needBuyCtp << "\n";
				fout << "sysID" << pNewDataCtp->sysId << "requestId" << pNewDataCtp->requestId << "OrderRef" << pNewDataCtp->req.OrderRef
					<< "命令里返回的请求ID" << pNewDataCtp->req.RequestID << "\n";
				//ReqOrderInsert(pNewDataCtp);
				pNewDataCtp->waitStatus = 1;
				//链表末尾增加一项
				m_CTradingDataCtp.push_back(*pNewDataCtp);
			}
			else{
				int needClose = 0;
				//先平仓
				if(-(m_portfolioCtp + m_tradingPortfolioBuyCtp) > 0){
					pNewDataCtp->req.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
					needClose = -(m_portfolioCtp + m_tradingPortfolioBuyCtp);
					pNewDataCtp->req.VolumeTotalOriginal = needClose;
					int newRef = ::InterlockedIncrement((long*)(&m_maxOrderRef));
					sprintf(pNewDataCtp->req.OrderRef,"%d",newRef);
					fout << "ctp先买平" << needClose << "\n";
					fout << "sysID" << pNewDataCtp->sysId << "requestId" << pNewDataCtp->requestId << "OrderRef" << pNewDataCtp->req.OrderRef
					<< "命令里返回的请求ID" << pNewDataCtp->req.RequestID << "\n";
					//ReqOrderInsert(pNewDataCtp);
					pNewDataCtp->waitStatus = 1;
					//链表末尾增加一项
					m_CTradingDataCtp.push_back(*pNewDataCtp);
				}
				else{
					needClose = 0;
				}
				//再开仓
				if(needBuyCtp - needClose > m_tradingPortfolioSellCtp){
					pNewDataCtp->req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
					pNewDataCtp->req.VolumeTotalOriginal = needBuyCtp - needClose;
					int newRef = ::InterlockedIncrement((long*)(&m_maxOrderRef));
					sprintf(pNewDataCtp->req.OrderRef,"%d",newRef);
					fout << "ctp再买开" << needBuyCtp - needClose << "\n";
					fout << "sysID" << pNewDataCtp->sysId << "requestId" << pNewDataCtp->requestId << "OrderRef" << pNewDataCtp->req.OrderRef
					<< "命令里返回的请求ID" << pNewDataCtp->req.RequestID << "\n";
					//ReqOrderInsert(pNewDataCtp);
					pNewDataCtp->waitStatus = 1;
					//链表末尾增加一项
					m_CTradingDataCtp.push_back(*pNewDataCtp);
				}
			}
		}
		else{//全开仓
			if(needBuyCtp > m_tradingPortfolioSellCtp){
				pNewDataCtp->req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
				pNewDataCtp->req.VolumeTotalOriginal = needBuyCtp;
				int newRef = ::InterlockedIncrement((long*)(&m_maxOrderRef));
				sprintf(pNewDataCtp->req.OrderRef,"%d",newRef);
				fout << "ctp全买开" << needBuyCtp << "\n";
				fout << "sysID" << pNewDataCtp->sysId << "requestId" << pNewDataCtp->requestId << "OrderRef" << pNewDataCtp->req.OrderRef
					<< "命令里返回的请求ID" << pNewDataCtp->req.RequestID << "\n";
				//ReqOrderInsert(pNewDataCtp);
				pNewDataCtp->waitStatus = 1;
				//链表末尾增加一项
				m_CTradingDataCtp.push_back(*pNewDataCtp);
			}
		}	
	}
	if(m_portfolioCtp - aimCtp - m_tradingPortfolioSellCtp > 0){
		needSellCtp = m_portfolioCtp - aimCtp - m_tradingPortfolioSellCtp;
		fout << "需要卖出数目" << needSellCtp << "\n";
		pNewDataCtp->reset();
		pNewDataCtp->req.LimitPrice = g_ifAsk1;
		pNewDataCtp->priceRef = g_ifAsk1;
		pNewDataCtp->req.Direction = THOST_FTDC_D_Sell;
		if(m_portfolioCtp - m_tradingPortfolioSellCtp > 0){
			if(needSellCtp <= m_portfolioCtp - m_tradingPortfolioSellCtp){//只有平仓
				pNewDataCtp->req.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
				pNewDataCtp->req.VolumeTotalOriginal = needSellCtp;
				int newRef = ::InterlockedIncrement((long*)(&m_maxOrderRef));
				sprintf(pNewDataCtp->req.OrderRef,"%d",newRef);
				fout << "ctp全卖平" << needSellCtp << "\n";
				fout << "sysID" << pNewDataCtp->sysId << "requestId" << pNewDataCtp->requestId << "OrderRef" << pNewDataCtp->req.OrderRef
					<< "命令里返回的请求ID" << pNewDataCtp->req.RequestID << "\n";
				//ReqOrderInsert(pNewDataCtp);
				pNewDataCtp->waitStatus = 1;
				//链表末尾增加一项
				m_CTradingDataCtp.push_back(*pNewDataCtp);
			}
			else{//先平仓,再开仓
				int needClose = 0;
				//先平仓
				if(m_portfolioCtp - m_tradingPortfolioSellCtp > 0){
					pNewDataCtp->req.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
					needClose = m_portfolioCtp - m_tradingPortfolioSellCtp;
					pNewDataCtp->req.VolumeTotalOriginal = needClose;
					int newRef = ::InterlockedIncrement((long*)(&m_maxOrderRef));
					sprintf(pNewDataCtp->req.OrderRef,"%d",newRef);
					fout << "ctp先卖平" << needClose << "\n";
					fout << "sysID" << pNewDataCtp->sysId << "requestId" << pNewDataCtp->requestId << "OrderRef" << pNewDataCtp->req.OrderRef
					<< "命令里返回的请求ID" << pNewDataCtp->req.RequestID << "\n";
					//ReqOrderInsert(pNewDataCtp);
					pNewDataCtp->waitStatus = 1;
					//链表末尾增加一项
					m_CTradingDataCtp.push_back(*pNewDataCtp);
				}
				else{
					needClose = 0;
				}
				//再开仓
				if(needSellCtp - needClose > m_tradingPortfolioBuyCtp){
					pNewDataCtp->req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
					pNewDataCtp->req.VolumeTotalOriginal = needSellCtp - needClose;
					int newRef = ::InterlockedIncrement((long*)(&m_maxOrderRef));
					sprintf(pNewDataCtp->req.OrderRef,"%d",newRef);
					fout << "ctp再卖开" << needSellCtp - needClose << "\n";
					fout << "sysID" << pNewDataCtp->sysId << "requestId" << pNewDataCtp->requestId << "OrderRef" << pNewDataCtp->req.OrderRef
					<< "命令里返回的请求ID" << pNewDataCtp->req.RequestID << "\n";
					//ReqOrderInsert(pNewDataCtp);
					pNewDataCtp->waitStatus = 1;
					//链表末尾增加一项
					m_CTradingDataCtp.push_back(*pNewDataCtp);
				}
			}
		}
		else{//全开仓
			if(needSellCtp > m_tradingPortfolioBuyCtp){
				pNewDataCtp->req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
				pNewDataCtp->req.VolumeTotalOriginal = needSellCtp;
				int newRef = ::InterlockedIncrement((long*)(&m_maxOrderRef));
				sprintf(pNewDataCtp->req.OrderRef,"%d",newRef);
				fout << "ctp全卖开" << needSellCtp << "\n";
				fout << "sysID" << pNewDataCtp->sysId << "requestId" << pNewDataCtp->requestId << "OrderRef" << pNewDataCtp->req.OrderRef
					<< "命令里返回的请求ID" << pNewDataCtp->req.RequestID << "\n";
				//ReqOrderInsert(pNewDataCtp);
				pNewDataCtp->waitStatus = 1;
				//链表末尾增加一项
				m_CTradingDataCtp.push_back(*pNewDataCtp);
			}
		}	
	}
	fout << "#################################A50操作###################################\n";
	//修改已经存在的单子,并将低于nowLadder节点的IB买价单删除,高于nowLadder节点的卖价单删除;同时nowLadder节点的订单价格
	for(iTDIB = m_CTradingDataIb.begin();iTDIB != m_CTradingDataIb.end();iTDIB++){
		if(!iTDIB->expired && iTDIB->waitStatus == 0){
			if((iTDIB->ladderRef < nowLadder) && (strcmp(iTDIB->IbOrder.action,"BUY") == 0)){
				iTDIB->waitStatus = 3;//删除
			}
			else if((iTDIB->ladderRef > nowLadder) && (strcmp(iTDIB->IbOrder.action,"SELL") == 0)){
				iTDIB->waitStatus = 3;//删除
			}
			else{//修改价格
				if(strcmp(iTDIB->IbOrder.action,"BUY") == 0){
					double a50BuyLimitPrice = ladder[iTDIB->ladderRef] +  (g_ifAsk1 + g_ifBid1) / 2.0 * g_A50Index / g_HS300Index 
						+  g_A50Index * m_dltInterest + m_datumDiff;
					a50BuyLimitPrice = dealA50Price(true,a50BuyLimitPrice);
					TRACE("修订价格%.f,原价格%.f\r\n",a50BuyLimitPrice,iTDIB->IbOrder.lmtPrice);
					if(fabs(a50BuyLimitPrice - iTDIB->IbOrder.lmtPrice) > 0.00001){
						iTDIB->IbOrder.lmtPrice = a50BuyLimitPrice;
						iTDIB->waitStatus = 2;//修改
					}
				}
				else{
					double a50SellLimitPrice = ladder[iTDIB->ladderRef + 1] +  (g_ifAsk1 + g_ifBid1) / 2.0 * g_A50Index / g_HS300Index 
						+  g_A50Index * m_dltInterest + m_datumDiff;
					TRACE("处理前的价格%.f\r\n",a50SellLimitPrice);
					a50SellLimitPrice = dealA50Price(false,a50SellLimitPrice);
					TRACE("修订价格%.f,原价格%.f\r\n",a50SellLimitPrice,iTDIB->IbOrder.lmtPrice);
					if(fabs(a50SellLimitPrice - iTDIB->IbOrder.lmtPrice) > 0.00001){
						iTDIB->IbOrder.lmtPrice = a50SellLimitPrice;
						iTDIB->waitStatus = 2;//修改
					}
				}
			}
		}
	}
	//处理nowLadder节点的指令操作
	//强制卖出A50
	if(m_portfolioIb - tradingSellLessThanNowladderIb - aimPortfolioIb1 > 0){
		fout << "强制SELL" << m_portfolioIb - tradingSellLessThanNowladderIb - aimPortfolioIb1 << "\n";
		pNewDataIb->reset();
		pNewDataIb->IbOrder.totalQuantity = m_portfolioIb - tradingSellLessThanNowladderIb - aimPortfolioIb1;
		pNewDataIb->IbOrder.action = "SELL";
		double a50SellLimitPrice = ladder[nowLadder] +  (g_ifAsk1 + g_ifBid1) / 2.0 * g_A50Index / g_HS300Index 
			+  g_A50Index * m_dltInterest + m_datumDiff;
		a50SellLimitPrice = dealA50Price(false,a50SellLimitPrice);
		pNewDataIb->IbOrder.lmtPrice = a50SellLimitPrice;
		pNewDataIb->ladderRef = nowLadder - 1;
		pNewDataIb->waitStatus = 1;
		m_CTradingDataIb.push_back(*pNewDataIb);
	}
	//需要增加买入A50的数目,这里把低节点的进行中买单一并计算进来,在这之前其实已经发出删除指令
	if(aimPortfolioIb1 - max(m_portfolioIb,aimPortfolioIb2) - m_tradingPortfolioBuyIb > 0){
		fout << "需要BUY" << aimPortfolioIb1 - max(m_portfolioIb,aimPortfolioIb2) - m_tradingPortfolioBuyIb << "\n";
		TRACE("需要增加买入A50%d手\r\n",aimPortfolioIb1 - max(m_portfolioIb,aimPortfolioIb2) - m_tradingPortfolioBuyIb);
		pNewDataIb->reset();
		pNewDataIb->IbOrder.totalQuantity = aimPortfolioIb1 - max(m_portfolioIb,aimPortfolioIb2) - m_tradingPortfolioBuyIb;
		pNewDataIb->IbOrder.action = "BUY";
		double a50BuyLimitPrice = ladder[nowLadder] +  (g_ifAsk1 + g_ifBid1) / 2.0 * g_A50Index / g_HS300Index 
			+  g_A50Index * m_dltInterest + m_datumDiff;
		a50BuyLimitPrice = dealA50Price(true,a50BuyLimitPrice);
		pNewDataIb->IbOrder.lmtPrice = a50BuyLimitPrice;
		pNewDataIb->ladderRef = nowLadder;
		pNewDataIb->waitStatus = 1;
		m_CTradingDataIb.push_back(*pNewDataIb);
	}
	//一般情况下无理由出现所谓多余的买入订单
	//需要撤销多余的买入订单
	if(aimPortfolioIb1 -  m_portfolioIb - (m_tradingPortfolioBuyIb - tradingBuyMoreThanNowladderIb) < 0){
		fout << "需要撤销BUY" << -(aimPortfolioIb1 - m_portfolioIb - (m_tradingPortfolioBuyIb - tradingBuyMoreThanNowladderIb)) << "\n";
		int remain = -(aimPortfolioIb1 - m_portfolioIb - (m_tradingPortfolioBuyIb - tradingBuyMoreThanNowladderIb));//还剩多少
		for(iTDIB = m_CTradingDataIb.begin();iTDIB != m_CTradingDataIb.end();iTDIB++){
			if(!iTDIB->expired && iTDIB->waitStatus == 0 && iTDIB->ladderRef == nowLadder && iTDIB->IbOrder.action == "BUY"){
				if(remain == 0){
					break;
				}
				if(iTDIB->IbOrder.totalQuantity - iTDIB->volumeFilled <= remain){
					remain = remain - (iTDIB->IbOrder.totalQuantity - iTDIB->volumeFilled);
					iTDIB->waitStatus = 3;//删除
					fout << "其中直接删除BUY" << iTDIB->IbOrder.totalQuantity - iTDIB->volumeFilled << "\n";
				}
				else{
					iTDIB->IbOrder.totalQuantity = iTDIB->IbOrder.totalQuantity - remain;
					fout << "数量减少BUY" << remain << "\n";
					remain = 0;
					iTDIB->waitStatus = 2;//修改
					fout << "并修改数量为" << iTDIB->IbOrder.totalQuantity << "\n";
				}
			}
		}
	}
	//**********************************************************************************************
	//处理nowLadder+1节点的指令操作
	//强制买入A50
	if(aimPortfolioIb2 - m_portfolioIb - tradingBuyMoreThanNowladderIb > 0){
		fout << "需要强制BUY" << aimPortfolioIb2 - m_portfolioIb - tradingBuyMoreThanNowladderIb << "\n";
		pNewDataIb->reset();
		pNewDataIb->IbOrder.totalQuantity = aimPortfolioIb2 - m_portfolioIb - tradingBuyMoreThanNowladderIb;
		pNewDataIb->IbOrder.action = "BUY";
		double a50BuyLimitPrice = ladder[nowLadder + 1] +  (g_ifAsk1 + g_ifBid1) / 2.0 * g_A50Index / g_HS300Index 
			+  g_A50Index * m_dltInterest + m_datumDiff;
		a50BuyLimitPrice = dealA50Price(true,a50BuyLimitPrice);
		pNewDataIb->IbOrder.lmtPrice = a50BuyLimitPrice;
		pNewDataIb->ladderRef = nowLadder + 1;
		pNewDataIb->waitStatus = 1;
		m_CTradingDataIb.push_back(*pNewDataIb);
	}
	//需要增加卖出A50的数目
	if(min(m_portfolioIb,aimPortfolioIb1) - aimPortfolioIb2 - m_tradingPortfolioSellIb  > 0){
		fout << "需要增加SELL" << min(m_portfolioIb,aimPortfolioIb1) - aimPortfolioIb2 - m_tradingPortfolioSellIb << "\n";
		pNewDataIb->reset();
		pNewDataIb->IbOrder.totalQuantity = min(m_portfolioIb,aimPortfolioIb1) - aimPortfolioIb2 - m_tradingPortfolioSellIb;
		pNewDataIb->IbOrder.action = "SELL";
		double a50SellLimitPrice = ladder[nowLadder + 1] +  (g_ifAsk1 + g_ifBid1) / 2.0 * g_A50Index / g_HS300Index 
			+  g_A50Index * m_dltInterest + m_datumDiff;
		a50SellLimitPrice = dealA50Price(false,a50SellLimitPrice);
		pNewDataIb->IbOrder.lmtPrice = a50SellLimitPrice;
		pNewDataIb->ladderRef = nowLadder;
		pNewDataIb->waitStatus = 1;
		m_CTradingDataIb.push_back(*pNewDataIb);
	}
	//需要撤销多余卖出订单
	if(m_portfolioIb - (m_tradingPortfolioSellIb - tradingSellLessThanNowladderIb) - aimPortfolioIb2 < 0){
		fout << "需要撤销多余SELL" << -(m_portfolioIb - (m_tradingPortfolioSellIb - tradingSellLessThanNowladderIb) - aimPortfolioIb2) << "\n";
		int remain = -(m_portfolioIb - (m_tradingPortfolioSellIb - tradingSellLessThanNowladderIb) - aimPortfolioIb2);//还剩多少
		for(iTDIB = m_CTradingDataIb.begin();iTDIB != m_CTradingDataIb.end();iTDIB++){
			if(!iTDIB->expired && iTDIB->waitStatus == 0 && iTDIB->ladderRef == nowLadder && iTDIB->IbOrder.action == "SELL"){
				if(remain == 0){
					break;
				}
				if(iTDIB->IbOrder.totalQuantity - iTDIB->volumeFilled <= remain){
					remain = remain - (iTDIB->IbOrder.totalQuantity - iTDIB->volumeFilled);
					iTDIB->waitStatus = 3;//删除
					fout << "直接删除SELL" << iTDIB->IbOrder.totalQuantity - iTDIB->volumeFilled << "\n";
				}
				else{
					iTDIB->IbOrder.totalQuantity = iTDIB->IbOrder.totalQuantity - remain;
					fout << "数量减少SELL" << remain << "\n";
					remain = 0;
					iTDIB->waitStatus = 2;//修改
					fout << "并修改SELL数量为" << iTDIB->IbOrder.totalQuantity << "\n";
				}
			}
		}
	}
	bool CanTrade = true;
	bool isTradingTime = true;
	/*
	//非常规交易时间
	if((sys.wHour == 9 && sys.wMinute == 10 && sys.wSecond < 10) ||
		(sys.wHour == 9 && sys.wMinute <= 9) ||
		(sys.wHour == 9 && sys.wMinute == 14 && sys.wSecond < 50) ||
		(sys.wHour == 11 && sys.wMinute >= 30) ||
		(sys.wHour == 11 && sys.wMinute == 29 && sys.wSecond > 50) ||
		(sys.wHour == 12 && sys.wMinute <= 58) ||
		(sys.wHour == 12 && sys.wMinute == 59 && sys.wSecond < 50) ||
		(sys.wHour == 15 && sys.wMinute == 14 && sys.wSecond > 50) ||
		(sys.wHour == 15 && sys.wMinute >= 15) ||
		(sys.wHour < 9) || 
		(sys.wHour > 15)){
			isTradingTime = false;
	}
	*/
	CanTrade = isTradingTime && isPriceCorrect;
	fout << "批量发出指令" << "\n";
	//批量发出ctp指令
	for(iTDCTP = m_CTradingDataCtp.begin();iTDCTP != m_CTradingDataCtp.end();iTDCTP++){
		if(iTDCTP->expired){
			continue;
		}
		//发送新订单
		if(iTDCTP->waitStatus == 1){
			if(CanTrade){
				iTDCTP->frontIdTrade = m_frontIdTrade;
				iTDCTP->sessionIdTrade = m_sessionIdTrade;
				ReqOrderInsert(&(*iTDCTP));
				iTDCTP->waitStatus = 0;
				fout << "发出订单:" << "\n";
				fout << "前置ID" << iTDCTP->frontIdTrade << "会话ID" << iTDCTP->sessionIdTrade << "\n";
				fout << "sysID" << iTDCTP->sysId << "requestId" << iTDCTP->requestId << "OrderRef" << iTDCTP->req.OrderRef
						<< "命令里返回的请求ID" << iTDCTP->req.RequestID << "\n";
				fout << "OrderRef" << iTDCTP->req.OrderRef << "PriceRef" << iTDCTP->priceRef << " Direction " << iTDCTP->req.Direction 
					<< "O/C" <<iTDCTP->req.CombOffsetFlag[0] << " 数目 "<< iTDCTP->req.VolumeTotalOriginal << " 价格 " << iTDCTP->req.LimitPrice << "\n";
			}
			else{
				iTDCTP->expired = true;
				iTDCTP->waitStatus = 0;
			}
		}
		//删除订单
		if(iTDCTP->waitStatus == 3){
			if(CanTrade){
				ReqDeleteOrder(&(*iTDCTP));
				iTDCTP->waitStatus = 0;
				fout << "删除订单:" << "\n";
				fout << "前置ID" << iTDCTP->frontIdTrade << "会话ID" << iTDCTP->sessionIdTrade << "\n";
				fout << "sysID" << iTDCTP->sysId << "requestId" << iTDCTP->requestId << "OrderRef" << iTDCTP->req.OrderRef
						<< "命令里返回的请求ID" << iTDCTP->req.RequestID << "\n";
				fout << "OrderRef" << iTDCTP->req.OrderRef << "PriceRef" << iTDCTP->priceRef << " Direction " << iTDCTP->req.Direction 
					<< "O/C" <<iTDCTP->req.CombOffsetFlag[0] << " 数目 "<< iTDCTP->req.VolumeTotalOriginal << " 价格 " << iTDCTP->req.LimitPrice << "\n";
			}
			else{
				iTDCTP->waitStatus = 0;
			}
		}
	}
	//批量发出指令
	for(iTDIB = m_CTradingDataIb.begin();iTDIB != m_CTradingDataIb.end();iTDIB++){
		if(!iTDIB->expired){
			//发出新指令
			if(iTDIB->waitStatus == 1){
				if(CanTrade){
					m_dlgOrder->m_id++;
					iTDIB->IbOrder.orderId = m_dlgOrder->m_id;
					m_pClient->placeOrder(m_dlgOrder->m_id, m_dlgOrder->getContract(), iTDIB->IbOrder);
				}
				else{
					iTDIB->expired = true;
				}
			}
			//修改老订单
			if(iTDIB->waitStatus == 2){
				if(CanTrade){
					TRACE("修改老订单id = %d\r\n",iTDIB->IbOrder.orderId);
					m_pClient->placeOrder(iTDIB->IbOrder.orderId, m_dlgOrder->getContract(), iTDIB->IbOrder);
				}
			}
			//取消老订单
			if(iTDIB->waitStatus == 3){
				if(CanTrade){
					m_pClient->cancelOrder(iTDIB->IbOrder.orderId);
				}
			}
			iTDIB->waitStatus = 0;
		}
	}
	return 0;
}
