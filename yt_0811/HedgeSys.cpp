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
extern std::fstream fout;//�����־
extern std::fstream fout1;//ctp��־
extern double g_a50Bid1,g_a50Ask1;
extern int g_a50Bid1Size,g_a50Ask1Size;
extern double g_ifBid1,g_ifAsk1;
extern double g_A50Index,g_HS300Index;
extern ParamCtpMd ParamMdCtp;//ctp����ϵͳ��ʼ����,�����˺š���Լ�ȵ�
extern ParamCtpTd ParamTdCtp;
extern int getIndexData();
const int N = 5;
LRESULT CClient2Dlg::OnAllSysIni(UINT wParam, long lParam)
{
	if(lParam == 1){
		m_isIbReady = true;
		int i = m_errors.AddString(_T("IB����ϵͳ��ʼ�����\r\n"));
		int top = i - N < 0 ? 0 : i - N;
		m_errors.SetTopIndex( top);
	}
	if(lParam == 2){
		m_isCtpReady = true;
		int i = m_errors.AddString(_T("Ctp���׽���ȷ�����\r\n"));
		int top = i - N < 0 ? 0 : i - N;
		m_errors.SetTopIndex( top);
	}
	if(lParam == 3){
		m_isUpdateMarginRatioCtp = true;
		int i = m_errors.AddString(_T("Ctp��֤���ʼ�����\r\n"));
		int top = i - N < 0 ? 0 : i - N;
		m_errors.SetTopIndex( top);
	}
	if(lParam == 4){
		m_isUpdateSettlementPriceCtp = true;
		int i = m_errors.AddString(_T("IF��Լ����ۻ�ȡ���\r\n"));
		int top = i - N < 0 ? 0 : i - N;
		m_errors.SetTopIndex( top);
	}
	if(lParam == 5){
		m_isQryInstrumentCtp = true;
		int i = m_errors.AddString(_T("IF��Լ��ѯ���\r\n"));
		int top = i - N < 0 ? 0 : i - N;
		m_errors.SetTopIndex( top);
	}
	if(lParam == 6){
		m_isNetIniCtp = true;
		int i = m_errors.AddString(_T("CTP��̬Ȩ���ʼ�����\r\n"));
		int top = i - N < 0 ? 0 : i - N;
		m_errors.SetTopIndex( top);
	}
	if(lParam == 7){
		m_isPorliIniCtp = true;
		int i = m_errors.AddString(_T("CTP�ֲֳ�ʼ�����\r\n"));
		int top = i - N < 0 ? 0 : i - N;
		m_errors.SetTopIndex( top);
	}
	if(lParam == 8){
		m_isReqTradeCtp = true;
		int i = m_errors.AddString(_T("CTP�ɽ���ʼ�����\r\n"));
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
	fout1 << "========================================================================" << "\n";//����
	fout << "========================================================================" << "\n";//����
	fout1 << "ʱ��" << sys.wMonth << "/" << sys.wDay << "/" << sys.wHour << "/" << sys.wMinute << "/"
		<< sys.wSecond << "/" << sys.wMilliseconds << "\n";
	fout << "ʱ��" << sys.wMonth << "/" << sys.wDay << "/" << sys.wHour << "/" << sys.wMinute << "/"
		<< sys.wSecond << "/" << sys.wMilliseconds << "\n";
		*/
	bool isPriceCorrect = true;
	//TRACE("ʱ��%d/%d/%d/%d\r\n",sys.wHour,sys.wMinute,sys.wSecond,sys.wMilliseconds);
	if(g_ifBid1 > 200000 || g_ifBid1 < 100 || _isnan(g_ifBid1)!= 0){
		g_ifBid1 = m_ifBid10;
		isPriceCorrect = false;
		fout << " g_ifBid1�۸񲻺��� " << "\n";
	}
	if(g_ifAsk1 > 200000 || g_ifAsk1 < 100 || _isnan(g_ifAsk1)!= 0){
		g_ifAsk1 = m_ifAsk10;
		isPriceCorrect = false;
		fout << " g_ifAsk1�۸񲻺��� " << "\n";
	}
	if(g_ifAsk1 == m_ifAsk10 && g_ifBid1 == m_ifBid10){
		isPriceCorrect = false;
		fout << " IF�۸�����������һ�� " << "\n";
	}
	//double ladder[21] = {-9.5,-8.5,-7.5,-6.5,-5.5,-4.5,-3.5,-2.5,-1.5,-0.5,0.5,1.5,2.5,3.5,4.5,5.5,6.5,7.5,8.5,9.5,10.5};//�ݼ�����
	double ladder[21] = {-10,-9,-8,-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7,8,9,10};
	for(int i= 0;i< 21;i++){
		ladder[i] = m_ladderMulti * ladder[i];
	}
	int aimPortfolioLadder[21] = {10,9,8,7,6,5,4,3,2,1,0,-1,-2,-3,-4,-5,-6,-7,-8,-9,-10};//Ŀ��ֲ��ݼ�
	std::list<CTradingDataIb>::iterator iTDIB;
	int portfolioCtpEST;//�����ֲ�
	int nowLadder = 0;
	int limitLongPofolioIb = 0,limitShortPofolioIb = 0;//Ib�˻�����ͷ�Ϳ�ͷ�ֲ�
	int limitLongPofolioCtp = 0,limitShortPofolioCtp = 0;//ctp�˻�����ͷ�Ϳ�ͷ�ֲ�
	int limitLongPofolioHedge = 0,limitShortPofolioHedge = 0;//�Գ��˻��ֲ�����
	static int limitLongPofolioHedge_save = 1000,limitShortPofolioHedge_save = 1000;//��ʼֵ����
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
	//������ʾ����
	label.Format(_T("A50ָ��%.2f"),g_A50Index);
	m_A50IndexLabel.SetWindowTextA(label);
	label.Format(_T("300ָ��%.2f"),g_HS300Index);
	m_HS300IndexLabel.SetWindowTextA(label);
	label.Format(_T("A50��һ%.2f"),g_a50Bid1);
	m_A50Bid1Label.SetWindowTextA(label);
	label.Format(_T("A50��һ%.2f"),g_a50Ask1);
	m_A50Ask1Label.SetWindowTextA(label);
	label.Format(_T("IF��һ%.2f"),g_ifBid1);
	m_ifBid1Label.SetWindowTextA(label);
	label.Format(_T("IF��һ%.2f"),g_ifAsk1);
	m_ifAsk1Label.SetWindowTextA(label);
	CalDeviation();//����������
	label.Format(_T("����ƫ��%.2f"),m_deviationAvg + m_datumDiff);
	m_deviationLabel.SetWindowTextA(label);
	fout << "����ƫ��" << m_deviationAvg + m_datumDiff << "��׼" << m_datumDiff << "\n";
	//�����Ƿ��������
	if(g_A50Index < 0.1 || g_HS300Index < 0.1 || g_ifAsk1 < 0.1 || g_ifBid1 < 0.1 || g_a50Bid1 < 0.1 || g_a50Ask1 < 0.1 
		|| g_ifAsk1 > 200000 || g_ifBid1 >200000 || g_a50Ask1 > 800000 | g_a50Ask1 > 800000){
			TRACE("�۸��������,����\r\n");
			return -1;
	}
	if(_isnan(m_datumDiff) != 0 || _isnan(m_deviationAvg)!=0){
		return -1;//�жϷ���ֵ����
	}
	if(m_deviationSell >= 1000 || m_deviationBuy <= -1000){
		return -1;//������Χ������
	}
	/*
	//�ǽ����շ���
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
	fout << "A50ָ��" << g_A50Index << "HS300ָ��" << g_HS300Index << "A50�۸�" << g_a50Bid1
		<< "  " << g_a50Ask1 << "IF�۸�" << g_ifBid1 << "  " << g_ifAsk1 << "\n";
	//��ȡ�ɽ���Ϣ
	std::map<CString,std::list<CThostFtdcTradeField>>::iterator iTradeRtn;
	std::list<CThostFtdcTradeField>::iterator iTradeRtnVec;
	std::list<CTradingDataCtp>::iterator iTDCTP;//������
	CTradingDataCtp CTDC(m_brokerId,m_investorId,m_instrumentId);//�����ʼ��
	CTradingDataCtp *pNewDataCtp = &CTDC;
	CTradingDataIb CDIB;//�����ʼ��
	CTradingDataIb *pNewDataIb = &CDIB;
	fout << "#######################ͳ�Ƴɽ����������Ƿ������µ�########################\n";
	std::list<CTradingDataCtp>::iterator iEndCtp = m_CTradingDataCtp.end();
	for(iTDCTP = m_CTradingDataCtp.begin();iTDCTP != iEndCtp;iTDCTP++){
		if(iTDCTP->expired){
			continue;
		}
		fout << "sysID" << iTDCTP->sysId << "requestId" << iTDCTP->requestId << "OrderRef" << iTDCTP->req.OrderRef
			<< "�����ﷵ�ص�����ID" << iTDCTP->req.RequestID << "\n";
		iTradeRtn = m_ifRtn.find(CString(iTDCTP->sysId));
		if(iTradeRtn != m_ifRtn.end()){//�ҵ��˼�ֵ
			fout << "�ɽ��ر����ҵ�ϵͳID" << iTDCTP->sysId << "\n";
			for(iTradeRtnVec = iTradeRtn->second.begin();iTradeRtnVec != iTradeRtn->second.end();iTradeRtnVec++){
				iTDCTP->avgFilledPrice = (iTDCTP->avgFilledPrice * iTDCTP->volumeFilled  + iTradeRtnVec->Price * iTradeRtnVec->Volume) / (iTDCTP->volumeFilled + iTradeRtnVec->Volume);
				iTDCTP->volumeFilled = iTDCTP->volumeFilled + iTradeRtnVec->Volume;
			}
			fout << " �ɽ�ƽ���� " << iTDCTP->avgFilledPrice << " �ѳɽ����� " << iTDCTP->volumeFilled << "\n";
			iTradeRtn->second.clear();//��ձ���ֵ��Ӧ�ĳɽ���Ϣ
		}
		if(iTDCTP->volumeFilled == iTDCTP->volumeEnd){
			fout << "sysID" << iTDCTP->sysId << " �ɽ�ͳ�ƽ��� " << "������" << iTDCTP->volumeEnd << "\n";
			iTDCTP->expired = true;
			fout << "sysID" << "��Ч״̬" << iTDCTP->expired << "\n";
			if(iTDCTP->volumeEnd < iTDCTP->req.VolumeTotalOriginal){
				fout << " ����ʱδ�ɽ����� " << iTDCTP->req.VolumeTotalOriginal - iTDCTP->volumeEnd << "\n";
				if(iTDCTP->isNeedNewOrder){//�����µ�
					fout << "�����µ�" << "\n";
					pNewDataCtp->reset();
					pNewDataCtp->req = iTDCTP->req;
					if(iTDCTP->req.Direction == THOST_FTDC_D_Buy){
						pNewDataCtp->req.VolumeTotalOriginal = iTDCTP->req.VolumeTotalOriginal - iTDCTP->volumeFilled;
						TRACE("CTP���γɽ���Ŀ%d,����,�¶�����Ŀ%d\r\n",iTDCTP->volumeFilled,iTDCTP->req.VolumeTotalOriginal - iTDCTP->volumeFilled);
						pNewDataCtp->priceRef = iTDCTP->priceRef;
						int newRef = ::InterlockedIncrement((long*)(&m_maxOrderRef));
						sprintf(pNewDataCtp->req.OrderRef,"%d",newRef);
						if(g_ifAsk1 > pNewDataCtp->priceRef + 7.0){//7������Ϊֹ���
							pNewDataCtp->req.LimitPrice = g_ifAsk1 + 100.0;//ǿ�гɽ�
							fout << g_ifAsk1 << " " << pNewDataCtp->priceRef << "����ֹ��\n";
						}
						else{
							pNewDataCtp->req.LimitPrice = g_ifBid1;
						}
						fout << "�¶���" << "��������" << pNewDataCtp->req.OrderRef << " ��(ƽ/��)��־" << pNewDataCtp->req.CombOffsetFlag[0] << ":"<< pNewDataCtp->req.VolumeTotalOriginal 
							<< "��," << "�۸�" << pNewDataCtp->req.LimitPrice << "�ο��۸�" << pNewDataCtp->priceRef << "\n";
					}
					else{
						pNewDataCtp->req.VolumeTotalOriginal = iTDCTP->req.VolumeTotalOriginal - iTDCTP->volumeFilled;
						TRACE("CTP���γɽ���Ŀ%d,����,�¶�����Ŀ%d\r\n",iTDCTP->volumeFilled,iTDCTP->req.VolumeTotalOriginal - iTDCTP->volumeFilled);
						pNewDataCtp->priceRef = iTDCTP->priceRef;
						int newRef = ::InterlockedIncrement((long*)(&m_maxOrderRef));
						sprintf(pNewDataCtp->req.OrderRef,"%d",newRef);
						if(g_ifBid1 < pNewDataCtp->priceRef - 7.0){//7������Ϊֹ���
							pNewDataCtp->req.LimitPrice = g_ifBid1 - 100.0;//ǿ�гɽ�
							fout << g_ifBid1 << " " << pNewDataCtp->priceRef << "����ֹ��\n";
						}
						else{
							pNewDataCtp->req.LimitPrice = g_ifAsk1;
						}
						fout << "�¶���" << "��������" << pNewDataCtp->req.OrderRef << " ��(ƽ/��)��־" << pNewDataCtp->req.CombOffsetFlag[0] << ":"<< pNewDataCtp->req.VolumeTotalOriginal 
							<< "��," << "�۸�" << pNewDataCtp->req.LimitPrice << "�ο��۸�" << pNewDataCtp->priceRef << "\n";
					}
					//ReqOrderInsert(pNewDataCtp);
					pNewDataCtp->waitStatus = 1;
					//����ĩβ����һ��
					m_CTradingDataCtp.push_back(*pNewDataCtp);
					fout << "�¶���״̬" << pNewDataCtp->waitStatus << "�Ƿ���Ч" << pNewDataCtp->expired << "����ĩβ���Ӷ�������\n"; 
				}
			}
		}
	}
	fout << "#######################�����ݼ������########################\n";
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
				break;//�ҵ�ladder����������
			}
		}
	}
	fout << "������" << m_deviationAvg << "\n";
	fout << "�ڵ�λ��" << nowLadder << " " << ladder[nowLadder] << "  " << ladder[nowLadder + 1] << "\n";
	/////////////////////////////////////////////////////////////////////////
	fout << "#######################�ֲֺ����ճֲ������ʼ��########################\n";
	//����ԭ�гֲ�����
	//m_netCtp = m_netCtp0 + m_portfolioCtp0 * ((g_ifAsk1 + g_ifBid1) / 2.0 - (m_ifAsk10 + m_ifBid10) / 2.0) * m_valuePointCtp;
	m_netCtp = m_netCtp0;
	//m_portfolioCtp = m_portfolioCtp0;
	m_portfolioCtp = 0;
	fout << "����ctp�ֲ� " << m_portfolioCtp0 << " ���γ�ʼ�ʲ���ֵ " << m_netCtp0 << "\n";
	//*******************************************************************************************************************
	fout << "#######################ͳ��IF���ڳɽ��е����########################\n";
	//��if���д���
	for(iTDCTP = m_CTradingDataCtp.begin();iTDCTP != m_CTradingDataCtp.end();iTDCTP++){
		//�����³ֲ�����,�Լ����ֲ�
		if(iTDCTP->req.Direction == THOST_FTDC_D_Buy){
			m_netCtp = m_netCtp + ((g_ifAsk1 + g_ifBid1) / 2.0 - iTDCTP->avgFilledPrice * (1.0 + 0.000055/*Ӷ��*/)) * m_valuePointCtp * iTDCTP->volumeFilled;
			m_portfolioCtp = m_portfolioCtp + iTDCTP->volumeFilled;
			if(!iTDCTP->expired){//�����еĺ�Լ
				if(iTDCTP->req.CombOffsetFlag[0] == THOST_FTDC_OF_Open){//��
					m_tradingPortfolioBuyOpenCtp = m_tradingPortfolioBuyOpenCtp + iTDCTP->req.VolumeTotalOriginal - iTDCTP->volumeFilled;
					fout << "������" << "��������" << iTDCTP->req.OrderRef << "����ID(req�е�)" << iTDCTP->requestId << "ϵͳID" << iTDCTP->sysId
						<< iTDCTP->req.VolumeTotalOriginal - iTDCTP->volumeFilled << "�۸�" << iTDCTP->req.LimitPrice << "\n";
				}
				else{//��ƽ
					m_tradingPortfolioBuyCloseCtp = m_tradingPortfolioBuyCloseCtp + iTDCTP->req.VolumeTotalOriginal - iTDCTP->volumeFilled;
					fout << "������ƽ" << "��������" << iTDCTP->req.OrderRef << "����ID(req�е�)" << iTDCTP->requestId << "ϵͳID" << iTDCTP->sysId
						<< iTDCTP->req.VolumeTotalOriginal - iTDCTP->volumeFilled << "�۸�" << iTDCTP->req.LimitPrice << "\n";
				}
			}
		}
		else{
			m_netCtp = m_netCtp + (iTDCTP->avgFilledPrice * (1.0 - 0.000055/*Ӷ��*/) - (g_ifAsk1 + g_ifBid1) / 2.0) * m_valuePointCtp * iTDCTP->volumeFilled;
			m_portfolioCtp = m_portfolioCtp - iTDCTP->volumeFilled;
			if(!iTDCTP->expired){//�����еĺ�Լ
				if(iTDCTP->req.CombOffsetFlag[0] == THOST_FTDC_OF_Open){//����
					m_tradingPortfolioSellOpenCtp = m_tradingPortfolioSellOpenCtp + iTDCTP->req.VolumeTotalOriginal - iTDCTP->volumeFilled;
					fout << "��������" << "��������" << iTDCTP->req.OrderRef << "����ID(req�е�)" << iTDCTP->requestId << "ϵͳID" << iTDCTP->sysId
						<< iTDCTP->req.VolumeTotalOriginal - iTDCTP->volumeFilled << "�۸�" << iTDCTP->req.LimitPrice << "\n";
				}
				else{//��ƽ
					m_tradingPortfolioSellCloseCtp = m_tradingPortfolioSellCloseCtp + iTDCTP->req.VolumeTotalOriginal - iTDCTP->volumeFilled;
					fout << "������ƽ" << "��������" << iTDCTP->req.OrderRef << "����ID(req�е�)" << iTDCTP->requestId << "ϵͳID" << iTDCTP->sysId
						<< iTDCTP->req.VolumeTotalOriginal - iTDCTP->volumeFilled << "�۸�" << iTDCTP->req.LimitPrice << "\n";
				}
			}
		}
	}
	fout << "������IF����" << m_tradingPortfolioBuyOpenCtp << "������ƽIF����" << m_tradingPortfolioBuyCloseCtp << "\n";
	fout << "��������IF����" << m_tradingPortfolioSellOpenCtp << "������ƽIF����" << m_tradingPortfolioSellCloseCtp << "\n";
	m_tradingPortfolioBuyCtp = m_tradingPortfolioBuyOpenCtp + m_tradingPortfolioBuyCloseCtp;
	m_tradingPortfolioSellCtp = m_tradingPortfolioSellOpenCtp + m_tradingPortfolioSellCloseCtp;
	portfolioCtpEST = m_tradingPortfolioBuyCtp - m_tradingPortfolioSellCtp + m_portfolioCtp;
	label.Format(_T("IF��ƽ%d"),m_tradingPortfolioBuyCloseCtp);
	m_IFBuyClose.SetWindowTextA(label);
	label.Format(_T("IF��%d"),m_tradingPortfolioBuyOpenCtp);
	m_IFBuyOpen.SetWindowTextA(label);
	label.Format(_T("IF��ƽ%d"),m_tradingPortfolioSellCloseCtp);
	m_IFSellClose.SetWindowTextA(label);
	label.Format(_T("IF����%d"),m_tradingPortfolioSellOpenCtp);
	m_IFSellOpen.SetWindowTextA(label);
	TRACE("CTP���ʲ�(����Ӷ��)%.f,���ֲ�%d,�����ֲ�%d\r\n",m_netCtp,m_portfolioCtp,portfolioCtpEST);
	fout << "CTP���ʲ� "<< m_netCtp << "���ֲ�" << m_portfolioCtp << "�����е���" << m_tradingPortfolioBuyCtp << "�����е�����" << m_tradingPortfolioSellCtp << "\n";
	//��A50���д���
	//ԭ��A50�ֲ�����
	fout << "#######################����A50ԭ�гֲּ������ʼ��########################\n";
	m_netIb = m_netIb0 + m_portfolioIb0 * ((g_a50Ask1 + g_a50Bid1) / 2.0 - (m_a50Ask10 + m_a50Bid10) / 2.0) * m_valuePointIb;
	m_portfolioIb = m_portfolioIb0;
	fout << "A50ԭ�гֲ�" << m_portfolioIb << ",��ֵ����Ϊ" << m_netIb << "\n";
	fout << "####################################IB�ֲּ��������################################\n";
	for(iTDIB = m_CTradingDataIb.begin();iTDIB != m_CTradingDataIb.end();iTDIB++){
		//�����³ֲ�����,�Լ����ֲ�
		if(strcmp(iTDIB->IbOrder.action,"BUY") == 0){
			m_netIb = m_netIb + ((g_a50Ask1 + g_a50Bid1) / 2.0 - (iTDIB->avgFilledPrice + 1.5/*Ӷ��*/))* iTDIB->volumeFilled * m_valuePointIb;
			m_portfolioIb = m_portfolioIb + iTDIB->volumeFilled;
			if(!iTDIB->expired && iTDIB->waitStatus == 0){//�����еĺ�Լ
				m_tradingPortfolioBuyIb = m_tradingPortfolioBuyIb + iTDIB->IbOrder.totalQuantity - iTDIB->volumeFilled;
				if(iTDIB->ladderRef > nowLadder){//���ڵ�ǰ�ڵ����۵�(�󲿷�ʱ��Ӧ���Ѿ��ɽ���)
					tradingBuyMoreThanNowladderIb = tradingBuyMoreThanNowladderIb + iTDIB->IbOrder.totalQuantity - iTDIB->volumeFilled;
				}
				else if(iTDIB->ladderRef < nowLadder){
					tradingBuyLessThanNowladderIb = tradingBuyLessThanNowladderIb + iTDIB->IbOrder.totalQuantity - iTDIB->volumeFilled;
				}
			}
		}
		else{
			m_netIb = m_netIb + ((iTDIB->avgFilledPrice - 1.5/*Ӷ��*/) - (g_a50Ask1 + g_a50Bid1) / 2.0) * iTDIB->volumeFilled * m_valuePointIb;
			m_portfolioIb = m_portfolioIb - iTDIB->volumeFilled;
			if(!iTDIB->expired && iTDIB->waitStatus == 0){//�����еĺ�Լ
				m_tradingPortfolioSellIb = m_tradingPortfolioSellIb + iTDIB->IbOrder.totalQuantity - iTDIB->volumeFilled;
				if(iTDIB->ladderRef < nowLadder){//���ڵ�ǰ�ڵ�����۵�(�󲿷�ʱ��Ӧ���Ѿ��ɽ���)
					tradingSellLessThanNowladderIb = tradingSellLessThanNowladderIb + iTDIB->IbOrder.totalQuantity - iTDIB->volumeFilled;
				}
				else if(iTDIB->ladderRef > nowLadder){
					tradingSellMoreThanNowladderIb = tradingSellMoreThanNowladderIb + iTDIB->IbOrder.totalQuantity - iTDIB->volumeFilled;
				}
			}
		}
	}
	label.Format(_T("A50��%d"),m_tradingPortfolioBuyIb);
	m_A50BuyLabel.SetWindowTextA(label);
	label.Format(_T("A50��%d"),m_tradingPortfolioSellIb);
	m_A50SellLabel.SetWindowTextA(label);
	TRACE("IB���ʲ�%.f,���ֲ�%d,�����е���%d,�����е�����%d\r\n",m_netIb,m_portfolioIb,m_tradingPortfolioBuyIb,m_tradingPortfolioSellIb);
	fout << "IB���ʲ� "<< m_netIb << "���ֲ�" << m_portfolioIb << "�����е���" << m_tradingPortfolioBuyIb << "�����е�����" << m_tradingPortfolioSellIb << "\n";
	label.Format(_T("Ctp����%.2f"),m_netCtp);
	m_CtpNetLabel.SetWindowTextA(label);
	label.Format(_T("Ctp����%d"),m_portfolioCtp);
	m_ifPortLabel.SetWindowTextA(label);
	label.Format(_T("IB����%.2f"),m_netIb);
	m_ibNetLabel.SetWindowTextA(label);
	label.Format(_T("IB����%d"),m_portfolioIb);
	m_a50PortLabel.SetWindowTextA(label);
	label.Format(_T("����%.2f"),m_netCtp + m_netIb * m_RMBtoUSD);
	m_totalLabel.SetWindowTextA(label);
	//*************************************************************************************************************************************
	//�µ����㼰ָ���
	//���ֲֳ̲����Ƽ���,limitֵ��ԶΪ��
	fout << "############################���̲����Ƽ�Ŀ��ֲּ���################################\n";
	limitLongPofolioIb = max(0,(int)((m_netIb - 200.0)) / m_MarginA50);//��200USD���,ʹ��max(0,)��Ŀ�������䲻С��0
	limitShortPofolioIb = max(0,(int)((m_netIb - 200.0) / m_MarginA50));
	limitLongPofolioCtp = max(0,(int)((m_netCtp - 1000.0) / ((g_ifAsk1 + g_ifBid1) / 2.0 * m_LongMarginRatio *  m_valuePointCtp)));//��1000RMB���
	limitShortPofolioCtp = max(0,(int)((m_netCtp - 1000.0) / ((g_ifAsk1 + g_ifBid1) / 2.0 * m_ShortMarginRatio * m_valuePointCtp)));
	limitLongPofolioHedge = min(limitLongPofolioIb / m_multiA50,limitShortPofolioCtp);
	limitShortPofolioHedge = min(limitShortPofolioIb / m_multiA50,limitLongPofolioCtp);
	//�����ⲿ���жϱ�֤���ʽ𲻹�����ǿ��ƽ��ʱ��������Ϊ�ʽ���ڲ�������Ҫ�����¼Ӳ֣������ֶ�ϵͳ�������ܼӲ�
	if(limitLongPofolioHedge > limitLongPofolioHedge_save){
		limitLongPofolioHedge = limitLongPofolioHedge_save;
	}
	if(limitShortPofolioHedge > limitShortPofolioHedge_save){
		limitShortPofolioHedge = limitShortPofolioHedge_save;
	}
	limitLongPofolioHedge_save = limitLongPofolioHedge;
	limitShortPofolioHedge_save = limitShortPofolioHedge;
	int aimPortfolioIb1 = 0,aimPortfolioIb2 = 0,aimPortfolioCtp1 = 0,aimPortfolioCtp2 = 0;//1��2�ֱ����nowLadder��nowLadder+1λ��
	fout << "IB��������" << limitLongPofolioIb << "�̲�����" << limitShortPofolioIb << "Ctp��������" << limitLongPofolioCtp << "�̲�����" << limitShortPofolioCtp << "\n";
	fout << "Hedge��������" << limitLongPofolioHedge << "Hedge�̲�����" << limitShortPofolioHedge << "\n";
	//�������Ƴֲִ���
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
	//��Ҫ��������\����Ctp����Ŀ,���ڵ��޹أ�����A50�ֲֽ��е���
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
	fout << "IB���ֲ�" << m_portfolioIb << " CTPĿ��ֲ�" << aimCtp << "\n";
	fout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ CTP���� $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n";
	//�۸񲻷���Ҫ���ctpɾ��
	int needCancelDirect = m_portfolioCtp + m_tradingPortfolioBuyCtp - m_tradingPortfolioSellCtp - aimCtp;//��Ҫֱ��ɾ��������
	fout << "�ܹ���Ҫֱ��ȡ��������Ŀ(����)" << needCancelDirect << "\n";
	for(iTDCTP = m_CTradingDataCtp.begin();iTDCTP != m_CTradingDataCtp.end();iTDCTP++){
		if(!iTDCTP->expired){
			if(iTDCTP->req.Direction == THOST_FTDC_D_Buy){
				if(needCancelDirect > 0){//���ඩ����ɾ��������ֻ��needCancelDirect == 0��ֹͣ���ֲ���
					//ReqDeleteOrder(&iTDCTP->req);
					iTDCTP->waitStatus = 3;
					needCancelDirect = needCancelDirect - (iTDCTP->req.VolumeTotalOriginal - iTDCTP->volumeFilled);
					fout << "����ֱ��ȡ����Ŀ" << " �� " << iTDCTP->req.VolumeTotalOriginal - iTDCTP->volumeFilled
						<< "ʣ����ȡ����Ŀ" << needCancelDirect << " ";
					fout << "sysID" << iTDCTP->sysId << "requestId" << iTDCTP->requestId << "OrderRef" << iTDCTP->req.OrderRef
						<< "�����ﷵ�ص�����ID" << iTDCTP->req.RequestID << "\n";
				}
				else if(iTDCTP->req.LimitPrice < g_ifBid1){
					//ReqDeleteOrder(&iTDCTP->req);
					iTDCTP->waitStatus = 3;
					iTDCTP->isNeedNewOrder = true;
					fout << "�򵥼۸�ƫ�ͣ�ȡ������������Ҫ�����µ���־" << "\n";
					fout << "ȡ����Ŀ" << " �� " << iTDCTP->req.VolumeTotalOriginal - iTDCTP->volumeFilled
						<< "ʣ����ȡ����Ŀ" << needCancelDirect << " ";
					fout << "sysID" << iTDCTP->sysId << "requestId" << iTDCTP->requestId << "OrderRef" << iTDCTP->req.OrderRef
						<< "�����ﷵ�ص�����ID" << iTDCTP->req.RequestID << "\n";
				}
			}
			else{
				if(needCancelDirect < 0){//���ඩ����ɾ��������ֻ��needCancelDirect == 0��ֹͣ���ֲ���
					//ReqDeleteOrder(&iTDCTP->req);
					iTDCTP->waitStatus = 3;
					needCancelDirect = needCancelDirect + (iTDCTP->req.VolumeTotalOriginal - iTDCTP->volumeFilled);
					fout << "����ֱ��ȡ����Ŀ" << " ���� " << iTDCTP->req.VolumeTotalOriginal - iTDCTP->volumeFilled;
					fout << "sysID" << iTDCTP->sysId << "requestId" << iTDCTP->requestId << "OrderRef" << iTDCTP->req.OrderRef
						<< "�����ﷵ�ص�����ID" << iTDCTP->req.RequestID << "\n";
				}
				else if(iTDCTP->req.LimitPrice > g_ifAsk1){
					//ReqDeleteOrder(&iTDCTP->req);
					iTDCTP->waitStatus = 3;
					iTDCTP->isNeedNewOrder = true;
					fout << "�����۸�ƫ�ߣ�ȡ������������Ҫ�����µ���־" << "\n";
					fout << "ȡ����Ŀ" << " ���� " << iTDCTP->req.VolumeTotalOriginal - iTDCTP->volumeFilled;
					fout << "sysID" << iTDCTP->sysId << "requestId" << iTDCTP->requestId << "OrderRef" << iTDCTP->req.OrderRef
						<< "�����ﷵ�ص�����ID" << iTDCTP->req.RequestID << "\n";
				}
			}
		}
	}
	if(aimCtp - m_portfolioCtp - m_tradingPortfolioBuyCtp > 0){
		needBuyCtp = aimCtp - m_portfolioCtp - m_tradingPortfolioBuyCtp;
		fout << "��Ҫ������Ŀ" << needBuyCtp << "\n";
		pNewDataCtp->reset();
		pNewDataCtp->req.LimitPrice = g_ifBid1;
		pNewDataCtp->priceRef = g_ifBid1;
		pNewDataCtp->req.Direction = THOST_FTDC_D_Buy;
		if(m_portfolioCtp + m_tradingPortfolioBuyCtp < 0){
			if(needBuyCtp <= -(m_portfolioCtp + m_tradingPortfolioBuyCtp)){//ֻ��ƽ��
				pNewDataCtp->req.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
				pNewDataCtp->req.VolumeTotalOriginal = needBuyCtp;
				int newRef = ::InterlockedIncrement((long*)(&m_maxOrderRef));
				sprintf(pNewDataCtp->req.OrderRef,"%d",newRef);
				fout << "ctpȫ��ƽ" << needBuyCtp << "\n";
				fout << "sysID" << pNewDataCtp->sysId << "requestId" << pNewDataCtp->requestId << "OrderRef" << pNewDataCtp->req.OrderRef
					<< "�����ﷵ�ص�����ID" << pNewDataCtp->req.RequestID << "\n";
				//ReqOrderInsert(pNewDataCtp);
				pNewDataCtp->waitStatus = 1;
				//����ĩβ����һ��
				m_CTradingDataCtp.push_back(*pNewDataCtp);
			}
			else{
				int needClose = 0;
				//��ƽ��
				if(-(m_portfolioCtp + m_tradingPortfolioBuyCtp) > 0){
					pNewDataCtp->req.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
					needClose = -(m_portfolioCtp + m_tradingPortfolioBuyCtp);
					pNewDataCtp->req.VolumeTotalOriginal = needClose;
					int newRef = ::InterlockedIncrement((long*)(&m_maxOrderRef));
					sprintf(pNewDataCtp->req.OrderRef,"%d",newRef);
					fout << "ctp����ƽ" << needClose << "\n";
					fout << "sysID" << pNewDataCtp->sysId << "requestId" << pNewDataCtp->requestId << "OrderRef" << pNewDataCtp->req.OrderRef
					<< "�����ﷵ�ص�����ID" << pNewDataCtp->req.RequestID << "\n";
					//ReqOrderInsert(pNewDataCtp);
					pNewDataCtp->waitStatus = 1;
					//����ĩβ����һ��
					m_CTradingDataCtp.push_back(*pNewDataCtp);
				}
				else{
					needClose = 0;
				}
				//�ٿ���
				if(needBuyCtp - needClose > m_tradingPortfolioSellCtp){
					pNewDataCtp->req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
					pNewDataCtp->req.VolumeTotalOriginal = needBuyCtp - needClose;
					int newRef = ::InterlockedIncrement((long*)(&m_maxOrderRef));
					sprintf(pNewDataCtp->req.OrderRef,"%d",newRef);
					fout << "ctp����" << needBuyCtp - needClose << "\n";
					fout << "sysID" << pNewDataCtp->sysId << "requestId" << pNewDataCtp->requestId << "OrderRef" << pNewDataCtp->req.OrderRef
					<< "�����ﷵ�ص�����ID" << pNewDataCtp->req.RequestID << "\n";
					//ReqOrderInsert(pNewDataCtp);
					pNewDataCtp->waitStatus = 1;
					//����ĩβ����һ��
					m_CTradingDataCtp.push_back(*pNewDataCtp);
				}
			}
		}
		else{//ȫ����
			if(needBuyCtp > m_tradingPortfolioSellCtp){
				pNewDataCtp->req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
				pNewDataCtp->req.VolumeTotalOriginal = needBuyCtp;
				int newRef = ::InterlockedIncrement((long*)(&m_maxOrderRef));
				sprintf(pNewDataCtp->req.OrderRef,"%d",newRef);
				fout << "ctpȫ��" << needBuyCtp << "\n";
				fout << "sysID" << pNewDataCtp->sysId << "requestId" << pNewDataCtp->requestId << "OrderRef" << pNewDataCtp->req.OrderRef
					<< "�����ﷵ�ص�����ID" << pNewDataCtp->req.RequestID << "\n";
				//ReqOrderInsert(pNewDataCtp);
				pNewDataCtp->waitStatus = 1;
				//����ĩβ����һ��
				m_CTradingDataCtp.push_back(*pNewDataCtp);
			}
		}	
	}
	if(m_portfolioCtp - aimCtp - m_tradingPortfolioSellCtp > 0){
		needSellCtp = m_portfolioCtp - aimCtp - m_tradingPortfolioSellCtp;
		fout << "��Ҫ������Ŀ" << needSellCtp << "\n";
		pNewDataCtp->reset();
		pNewDataCtp->req.LimitPrice = g_ifAsk1;
		pNewDataCtp->priceRef = g_ifAsk1;
		pNewDataCtp->req.Direction = THOST_FTDC_D_Sell;
		if(m_portfolioCtp - m_tradingPortfolioSellCtp > 0){
			if(needSellCtp <= m_portfolioCtp - m_tradingPortfolioSellCtp){//ֻ��ƽ��
				pNewDataCtp->req.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
				pNewDataCtp->req.VolumeTotalOriginal = needSellCtp;
				int newRef = ::InterlockedIncrement((long*)(&m_maxOrderRef));
				sprintf(pNewDataCtp->req.OrderRef,"%d",newRef);
				fout << "ctpȫ��ƽ" << needSellCtp << "\n";
				fout << "sysID" << pNewDataCtp->sysId << "requestId" << pNewDataCtp->requestId << "OrderRef" << pNewDataCtp->req.OrderRef
					<< "�����ﷵ�ص�����ID" << pNewDataCtp->req.RequestID << "\n";
				//ReqOrderInsert(pNewDataCtp);
				pNewDataCtp->waitStatus = 1;
				//����ĩβ����һ��
				m_CTradingDataCtp.push_back(*pNewDataCtp);
			}
			else{//��ƽ��,�ٿ���
				int needClose = 0;
				//��ƽ��
				if(m_portfolioCtp - m_tradingPortfolioSellCtp > 0){
					pNewDataCtp->req.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
					needClose = m_portfolioCtp - m_tradingPortfolioSellCtp;
					pNewDataCtp->req.VolumeTotalOriginal = needClose;
					int newRef = ::InterlockedIncrement((long*)(&m_maxOrderRef));
					sprintf(pNewDataCtp->req.OrderRef,"%d",newRef);
					fout << "ctp����ƽ" << needClose << "\n";
					fout << "sysID" << pNewDataCtp->sysId << "requestId" << pNewDataCtp->requestId << "OrderRef" << pNewDataCtp->req.OrderRef
					<< "�����ﷵ�ص�����ID" << pNewDataCtp->req.RequestID << "\n";
					//ReqOrderInsert(pNewDataCtp);
					pNewDataCtp->waitStatus = 1;
					//����ĩβ����һ��
					m_CTradingDataCtp.push_back(*pNewDataCtp);
				}
				else{
					needClose = 0;
				}
				//�ٿ���
				if(needSellCtp - needClose > m_tradingPortfolioBuyCtp){
					pNewDataCtp->req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
					pNewDataCtp->req.VolumeTotalOriginal = needSellCtp - needClose;
					int newRef = ::InterlockedIncrement((long*)(&m_maxOrderRef));
					sprintf(pNewDataCtp->req.OrderRef,"%d",newRef);
					fout << "ctp������" << needSellCtp - needClose << "\n";
					fout << "sysID" << pNewDataCtp->sysId << "requestId" << pNewDataCtp->requestId << "OrderRef" << pNewDataCtp->req.OrderRef
					<< "�����ﷵ�ص�����ID" << pNewDataCtp->req.RequestID << "\n";
					//ReqOrderInsert(pNewDataCtp);
					pNewDataCtp->waitStatus = 1;
					//����ĩβ����һ��
					m_CTradingDataCtp.push_back(*pNewDataCtp);
				}
			}
		}
		else{//ȫ����
			if(needSellCtp > m_tradingPortfolioBuyCtp){
				pNewDataCtp->req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
				pNewDataCtp->req.VolumeTotalOriginal = needSellCtp;
				int newRef = ::InterlockedIncrement((long*)(&m_maxOrderRef));
				sprintf(pNewDataCtp->req.OrderRef,"%d",newRef);
				fout << "ctpȫ����" << needSellCtp << "\n";
				fout << "sysID" << pNewDataCtp->sysId << "requestId" << pNewDataCtp->requestId << "OrderRef" << pNewDataCtp->req.OrderRef
					<< "�����ﷵ�ص�����ID" << pNewDataCtp->req.RequestID << "\n";
				//ReqOrderInsert(pNewDataCtp);
				pNewDataCtp->waitStatus = 1;
				//����ĩβ����һ��
				m_CTradingDataCtp.push_back(*pNewDataCtp);
			}
		}	
	}
	fout << "#################################A50����###################################\n";
	//�޸��Ѿ����ڵĵ���,��������nowLadder�ڵ��IB��۵�ɾ��,����nowLadder�ڵ�����۵�ɾ��;ͬʱnowLadder�ڵ�Ķ����۸�
	for(iTDIB = m_CTradingDataIb.begin();iTDIB != m_CTradingDataIb.end();iTDIB++){
		if(!iTDIB->expired && iTDIB->waitStatus == 0){
			if((iTDIB->ladderRef < nowLadder) && (strcmp(iTDIB->IbOrder.action,"BUY") == 0)){
				iTDIB->waitStatus = 3;//ɾ��
			}
			else if((iTDIB->ladderRef > nowLadder) && (strcmp(iTDIB->IbOrder.action,"SELL") == 0)){
				iTDIB->waitStatus = 3;//ɾ��
			}
			else{//�޸ļ۸�
				if(strcmp(iTDIB->IbOrder.action,"BUY") == 0){
					double a50BuyLimitPrice = ladder[iTDIB->ladderRef] +  (g_ifAsk1 + g_ifBid1) / 2.0 * g_A50Index / g_HS300Index 
						+  g_A50Index * m_dltInterest + m_datumDiff;
					a50BuyLimitPrice = dealA50Price(true,a50BuyLimitPrice);
					TRACE("�޶��۸�%.f,ԭ�۸�%.f\r\n",a50BuyLimitPrice,iTDIB->IbOrder.lmtPrice);
					if(fabs(a50BuyLimitPrice - iTDIB->IbOrder.lmtPrice) > 0.00001){
						iTDIB->IbOrder.lmtPrice = a50BuyLimitPrice;
						iTDIB->waitStatus = 2;//�޸�
					}
				}
				else{
					double a50SellLimitPrice = ladder[iTDIB->ladderRef + 1] +  (g_ifAsk1 + g_ifBid1) / 2.0 * g_A50Index / g_HS300Index 
						+  g_A50Index * m_dltInterest + m_datumDiff;
					TRACE("����ǰ�ļ۸�%.f\r\n",a50SellLimitPrice);
					a50SellLimitPrice = dealA50Price(false,a50SellLimitPrice);
					TRACE("�޶��۸�%.f,ԭ�۸�%.f\r\n",a50SellLimitPrice,iTDIB->IbOrder.lmtPrice);
					if(fabs(a50SellLimitPrice - iTDIB->IbOrder.lmtPrice) > 0.00001){
						iTDIB->IbOrder.lmtPrice = a50SellLimitPrice;
						iTDIB->waitStatus = 2;//�޸�
					}
				}
			}
		}
	}
	//����nowLadder�ڵ��ָ�����
	//ǿ������A50
	if(m_portfolioIb - tradingSellLessThanNowladderIb - aimPortfolioIb1 > 0){
		fout << "ǿ��SELL" << m_portfolioIb - tradingSellLessThanNowladderIb - aimPortfolioIb1 << "\n";
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
	//��Ҫ��������A50����Ŀ,����ѵͽڵ�Ľ�������һ���������,����֮ǰ��ʵ�Ѿ�����ɾ��ָ��
	if(aimPortfolioIb1 - max(m_portfolioIb,aimPortfolioIb2) - m_tradingPortfolioBuyIb > 0){
		fout << "��ҪBUY" << aimPortfolioIb1 - max(m_portfolioIb,aimPortfolioIb2) - m_tradingPortfolioBuyIb << "\n";
		TRACE("��Ҫ��������A50%d��\r\n",aimPortfolioIb1 - max(m_portfolioIb,aimPortfolioIb2) - m_tradingPortfolioBuyIb);
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
	//һ������������ɳ�����ν��������붩��
	//��Ҫ������������붩��
	if(aimPortfolioIb1 -  m_portfolioIb - (m_tradingPortfolioBuyIb - tradingBuyMoreThanNowladderIb) < 0){
		fout << "��Ҫ����BUY" << -(aimPortfolioIb1 - m_portfolioIb - (m_tradingPortfolioBuyIb - tradingBuyMoreThanNowladderIb)) << "\n";
		int remain = -(aimPortfolioIb1 - m_portfolioIb - (m_tradingPortfolioBuyIb - tradingBuyMoreThanNowladderIb));//��ʣ����
		for(iTDIB = m_CTradingDataIb.begin();iTDIB != m_CTradingDataIb.end();iTDIB++){
			if(!iTDIB->expired && iTDIB->waitStatus == 0 && iTDIB->ladderRef == nowLadder && iTDIB->IbOrder.action == "BUY"){
				if(remain == 0){
					break;
				}
				if(iTDIB->IbOrder.totalQuantity - iTDIB->volumeFilled <= remain){
					remain = remain - (iTDIB->IbOrder.totalQuantity - iTDIB->volumeFilled);
					iTDIB->waitStatus = 3;//ɾ��
					fout << "����ֱ��ɾ��BUY" << iTDIB->IbOrder.totalQuantity - iTDIB->volumeFilled << "\n";
				}
				else{
					iTDIB->IbOrder.totalQuantity = iTDIB->IbOrder.totalQuantity - remain;
					fout << "��������BUY" << remain << "\n";
					remain = 0;
					iTDIB->waitStatus = 2;//�޸�
					fout << "���޸�����Ϊ" << iTDIB->IbOrder.totalQuantity << "\n";
				}
			}
		}
	}
	//**********************************************************************************************
	//����nowLadder+1�ڵ��ָ�����
	//ǿ������A50
	if(aimPortfolioIb2 - m_portfolioIb - tradingBuyMoreThanNowladderIb > 0){
		fout << "��Ҫǿ��BUY" << aimPortfolioIb2 - m_portfolioIb - tradingBuyMoreThanNowladderIb << "\n";
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
	//��Ҫ��������A50����Ŀ
	if(min(m_portfolioIb,aimPortfolioIb1) - aimPortfolioIb2 - m_tradingPortfolioSellIb  > 0){
		fout << "��Ҫ����SELL" << min(m_portfolioIb,aimPortfolioIb1) - aimPortfolioIb2 - m_tradingPortfolioSellIb << "\n";
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
	//��Ҫ����������������
	if(m_portfolioIb - (m_tradingPortfolioSellIb - tradingSellLessThanNowladderIb) - aimPortfolioIb2 < 0){
		fout << "��Ҫ��������SELL" << -(m_portfolioIb - (m_tradingPortfolioSellIb - tradingSellLessThanNowladderIb) - aimPortfolioIb2) << "\n";
		int remain = -(m_portfolioIb - (m_tradingPortfolioSellIb - tradingSellLessThanNowladderIb) - aimPortfolioIb2);//��ʣ����
		for(iTDIB = m_CTradingDataIb.begin();iTDIB != m_CTradingDataIb.end();iTDIB++){
			if(!iTDIB->expired && iTDIB->waitStatus == 0 && iTDIB->ladderRef == nowLadder && iTDIB->IbOrder.action == "SELL"){
				if(remain == 0){
					break;
				}
				if(iTDIB->IbOrder.totalQuantity - iTDIB->volumeFilled <= remain){
					remain = remain - (iTDIB->IbOrder.totalQuantity - iTDIB->volumeFilled);
					iTDIB->waitStatus = 3;//ɾ��
					fout << "ֱ��ɾ��SELL" << iTDIB->IbOrder.totalQuantity - iTDIB->volumeFilled << "\n";
				}
				else{
					iTDIB->IbOrder.totalQuantity = iTDIB->IbOrder.totalQuantity - remain;
					fout << "��������SELL" << remain << "\n";
					remain = 0;
					iTDIB->waitStatus = 2;//�޸�
					fout << "���޸�SELL����Ϊ" << iTDIB->IbOrder.totalQuantity << "\n";
				}
			}
		}
	}
	bool CanTrade = true;
	bool isTradingTime = true;
	/*
	//�ǳ��潻��ʱ��
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
	fout << "��������ָ��" << "\n";
	//��������ctpָ��
	for(iTDCTP = m_CTradingDataCtp.begin();iTDCTP != m_CTradingDataCtp.end();iTDCTP++){
		if(iTDCTP->expired){
			continue;
		}
		//�����¶���
		if(iTDCTP->waitStatus == 1){
			if(CanTrade){
				iTDCTP->frontIdTrade = m_frontIdTrade;
				iTDCTP->sessionIdTrade = m_sessionIdTrade;
				ReqOrderInsert(&(*iTDCTP));
				iTDCTP->waitStatus = 0;
				fout << "��������:" << "\n";
				fout << "ǰ��ID" << iTDCTP->frontIdTrade << "�ỰID" << iTDCTP->sessionIdTrade << "\n";
				fout << "sysID" << iTDCTP->sysId << "requestId" << iTDCTP->requestId << "OrderRef" << iTDCTP->req.OrderRef
						<< "�����ﷵ�ص�����ID" << iTDCTP->req.RequestID << "\n";
				fout << "OrderRef" << iTDCTP->req.OrderRef << "PriceRef" << iTDCTP->priceRef << " Direction " << iTDCTP->req.Direction 
					<< "O/C" <<iTDCTP->req.CombOffsetFlag[0] << " ��Ŀ "<< iTDCTP->req.VolumeTotalOriginal << " �۸� " << iTDCTP->req.LimitPrice << "\n";
			}
			else{
				iTDCTP->expired = true;
				iTDCTP->waitStatus = 0;
			}
		}
		//ɾ������
		if(iTDCTP->waitStatus == 3){
			if(CanTrade){
				ReqDeleteOrder(&(*iTDCTP));
				iTDCTP->waitStatus = 0;
				fout << "ɾ������:" << "\n";
				fout << "ǰ��ID" << iTDCTP->frontIdTrade << "�ỰID" << iTDCTP->sessionIdTrade << "\n";
				fout << "sysID" << iTDCTP->sysId << "requestId" << iTDCTP->requestId << "OrderRef" << iTDCTP->req.OrderRef
						<< "�����ﷵ�ص�����ID" << iTDCTP->req.RequestID << "\n";
				fout << "OrderRef" << iTDCTP->req.OrderRef << "PriceRef" << iTDCTP->priceRef << " Direction " << iTDCTP->req.Direction 
					<< "O/C" <<iTDCTP->req.CombOffsetFlag[0] << " ��Ŀ "<< iTDCTP->req.VolumeTotalOriginal << " �۸� " << iTDCTP->req.LimitPrice << "\n";
			}
			else{
				iTDCTP->waitStatus = 0;
			}
		}
	}
	//��������ָ��
	for(iTDIB = m_CTradingDataIb.begin();iTDIB != m_CTradingDataIb.end();iTDIB++){
		if(!iTDIB->expired){
			//������ָ��
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
			//�޸��϶���
			if(iTDIB->waitStatus == 2){
				if(CanTrade){
					TRACE("�޸��϶���id = %d\r\n",iTDIB->IbOrder.orderId);
					m_pClient->placeOrder(iTDIB->IbOrder.orderId, m_dlgOrder->getContract(), iTDIB->IbOrder);
				}
			}
			//ȡ���϶���
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
