#pragma once

#include <map>
#include <vector>
#include <functional>
#include "rapidjson/document.h"
#include "Pay/PayBase.h"

namespace SAPay
{

enum enumAlipayTradeStatus
{
	CALIPAY_TRADE_STATUS_SUCCESS,
	CALIPAY_TRADE_STATUS_CLOSED,
	CALIPAY_TRADE_STATUS_FINISHED,
	CALIPAY_TRADE_STATUS_WAIT_BUYER_PAY,
	CALIPAY_TRADE_STATUS_UNKONW
};

struct CAlipayResps
{
	CAlipayResps() :iTradeStatus(CALIPAY_TRADE_STATUS_UNKONW) {}
	//withdraw
	std::string strOutBizNo;
	std::string strPayDate;
	std::string strOrderId;

	//refund and query
	std::string strTradeNo;
	std::string strOutTradeNo;
	std::string strBuyerLogonId;
	std::string strBuyerUserId;

	//refund
	std::string strGmtRefundPay;
	std::string strFundChange;
	std::string strRefundFee;

	//query
	enumAlipayTradeStatus iTradeStatus;
	std::string strTotalAmount;
};

enum enumAlipayRet
{
	CALIPAY_RET_OK = 0,
	CALIPAY_RET_UNKNOW_ERROR,

	//you can call getLastErrInfo() to get sub code when it return CALIPAY_RET_SUB_CODE_ERROR
	CALIPAY_RET_SUB_CODE_ERROR,
	CALIPAY_RET_NETWORK_ERROR,
	CALIPAY_RET_PARSE_ERROR,
	CALIPAY_RET_VERIFY_ERROR
};

class CAlipay : public CPayBase
{
public:
	//parse notify 
	static std::map<std::string, std::string> parseNotifyContentAlipay(
		const std::string& strNotify
	);

	//check notify respsonse 0-sucess other-failed
	static int verifyAlipayNotify(
		const std::map<std::string, std::string>& mapNotify,
		const std::string& strPubKey
	);

	//check respsonse 0-sucess other-failed
	static int verifyAlipayResps(
		const std::string& strRespsContent, 
		const std::string& strSign, 
		const std::string& strPubKey
	);

public:
	CAlipay() = delete;

	/**
	* @name CAlipay
	*
	* @param strAppId						alipay app id
	* @param strPubKey						alipay public key
	* @param strPrivKey						user private key
	* @param bIsDevMode						if bIsDevMode is true, href is https://openapi.alipaydev.com/gateway.do
	*										else href is https://openapi.alipay.com/gateway.do
	*/
	CAlipay(
		const std::string& strAppId,
		const std::string& strPubKey,
		const std::string& strPrivKey,
		bool bIsDevMode = false
	);
	virtual ~CAlipay() { }

	/**
	* @name appendPayContent
	*
	* @brief								append a string to call client sdk
	*
	* @note								
	*
	* @param iAmount						amount
	* @param strTradingCode					user trading code
	* @param strSubject						trade subject type
	* @param strCallBackAddr				notify url
	* @param strTimeOut						valid time, see https://docs.open.alipay.com/204/105465/
	* @param strPassBackParams				custom call back params
	*
	* @return std::string
	*/
	std::string appendPayContent(
		int iAmount,
		const std::string& strTradingCode,
		const std::string& strSubject,
		const std::string& strCallBackAddr,
		const std::string& strTimeOut = std::string("30m"),
		const std::string& strPassBackParams = std::string("")
	);

	/**
	* @name refund
	*
	* @brief								refund a trading
	*
	* @note									see https://docs.open.alipay.com/api_1/alipay.trade.refund
	*										the output include strTradeNo, strOutTradeNo, strBuyerLogonId, 
	*										strBuyerUserId, strGmtRefundPay, strFundChange, strRefundFee
	*
	* @param iAmount						refund amount
	* @param strTradingCode					user trading code
	* @param strOutTradingCode				the trading code which you want to refund
	*
	* @return enumAlipayRet							
	*/
	enumAlipayRet refund(
		int iAmount,
		const std::string& strTradingCode,
		const std::string& strOutTradingCode,
		CAlipayResps& alipayResps
	);

	/**
	* @name withdraw
	*
	* @brief								transfer money to user account
	*
	* @note									see https://docs.open.alipay.com/api_28/alipay.fund.trans.toaccount.transfer
	*										the output include strOutBizNo, strPayDate, strOrderId
	*
	* @param iAmount						withdraw amount
	* @param strTradingCode					user trading code
	* @param strAlipayAccount				user alipay account
	* @param strTrueName					user true name
	*
	* @return enumAlipayRet					
	*/
	enumAlipayRet withdraw(
		int iAmount,
		const std::string& strTradingCode,
		const std::string& strAlipayAccount,
		const std::string& strTrueName,
		CAlipayResps& alipayResps
	);

	/**
	* @name queryPayStatus
	*
	* @brief								query bill status
	*
	* @note									see https://docs.open.alipay.com/api_1/alipay.trade.query/
	*										the output include strTradeNo, strOutTradeNo, strBuyerLogonId, 
	*										strBuyerUserId, strTradeStatus, strTotalAmount
	*
	* @param strOutTradingCode				the trading code which you want to query
	*
	* @return enumAlipayRet					
	*/
	enumAlipayRet queryPayStatus(
		const std::string& strOutTradingCode,
		CAlipayResps& alipayResps
	);

protected:
	//token
	bool m_bIsDevMode;
	std::string m_strAppId;
	std::string m_strPubKey;
	std::string m_strPrivKey;

protected:
	
	/**
	* @name sendReqAndParseResps
	*
	* @brief								send request and parse response
	*
	* @note
	*
	* @param strReq							request content 
	* @param strRespsName					resposne key name
	* @param func							if func return true, it will save error info
	*
	* @return int							return enumAlipayRet
	*/
	enumAlipayRet sendReqAndParseResps(
		const std::string& strReq,
		const std::string& strRespsName,
		const std::function<bool(rapidjson::Value&, enumAlipayRet& iRet)> func
	);

	//append request content
	std::string appendTransferContent(
		int iAmount,
		const std::string& strAlipayAccount,
		const std::string& strTrueName,
		const std::string& strTradingCode
	);
	std::string appendRefundContent(
		int iAmount,
		const std::string& strTradingCode,
		const std::string& strOutTradingCode
	);
	std::string appendQueryStatusContent(
		const std::string& strOutTradingCode
	);

	//nouse
	std::string appendCloseContent(
		const std::string& strTradingCode
	);
	std::string appendDowloadUrlContent(
		const std::string& strDateTime
	);
};
}

