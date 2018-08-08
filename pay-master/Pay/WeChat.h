#pragma once
#include <map>
#include <vector>
#include <functional>
#include "Pay/PayBase.h"

namespace SAPay
{

enum enumWechatTradeState
{
	CWECHAT_TRADE_STATE_SUCCESS,
	CWECHAT_TRADE_STATE_REFUND,
	CWECHAT_TRADE_STATE_NOTPAY,
	CWECHAT_TRADE_STATE_CLOSED,
	CWECHAT_TRADE_STATE_REVOKED,
	CWECHAT_TRADE_STATE_USERPAYING,
	CWECHAT_TRADE_STATE_PAYERROR,
	CWECHAT_TRADE_STATE_UNKNOW
};

struct CWeChatResps
{
	//small program login
	std::string strSessionKey;
	std::string strOpenId;

	//prepay content
	std::string strTradeType;
	std::string strPrepayId;
	std::string strPrepaySignedContent;

	//query
	enumWechatTradeState iTradeState;
	std::string strBankType;
	std::string strTotalFee;
	std::string strCashFee;
	std::string strTransactionId;
	std::string strOutTradeNo;
	std::string strTimeEnd;
	std::string strTradeStateDesc;
};

enum enumWechatRet
{
	CWECHAT_RET_OK = 0,
	CWECHAT_RET_UNKNOW_ERROR,

	//you can call getLastErrInfo() to get err_code when it return CWECHAT_RET_ERR_CODE_ERROR
	CWECHAT_RET_ERR_CODE_ERROR,

	//you can call getLastErrInfo() to get return_msg when it return CWECHAT_RET_RET_MSG_ERROR
	CWECHAT_RET_RET_MSG_ERROR,
	CWECHAT_RET_NETWORK_ERROR,
	CWECHAT_RET_PARSE_ERROR,
	CWECHAT_RET_VERIFY_ERROR,
	CWECHAT_RET_MISSING_APP_SECRET
};

class CWeChat : public CPayBase
{
public:
	static std::map<std::string, std::string> parseRespsContentWechat(
		const std::string& strNotify
	);

	static int verifyWechatRespsAndNotify(
		const std::map<std::string, std::string>& mapResps, 
		const std::string& strMchKey
	);

public:
	CWeChat() = delete;

	/**
	* @name CWeChat
	*
	* @brief								
	*
	* @note									the cert and the key is used to
	*										send request that need Two-Way Authentication
	*
	* @param strAppId						wechat app id
	* @param strMchId						wechat mch id
	* @param strMchKey						wechat mch key
	* @param bIsApp							true-APP false-small program
	* @param strAppSecret					wechat app secret 
	* @param strCertPath					cert path
	* @param strCertPassword				cert password
	* @param strKeyPath						key path
	* @param strKeyPassword					key password
	*/
	CWeChat(
		const std::string& strAppId,
		const std::string& strMchId,
		const std::string& strMchKey,
		bool bIsApp = true,
		const std::string& strAppSecret = std::string(""),
		const std::string& strCertPath = std::string(""),
		const std::string& strCertPassword = std::string(""),
		const std::string& strKeyPath = std::string(""),
		const std::string& strKeyPassword = std::string("")
	);

	virtual ~CWeChat() { }

	/**
	* @name queryPayStatus
	*
	* @brief								query bill status
	*
	* @note									see https://pay.weixin.qq.com/wiki/doc/api/app/app.php?chapter=9_2&index=4
	*										the out put include strTradeState, strBankType, strTotalFee,
	*										strCashFee,strTransactionId, strOutTradeNo,strTimeEnd, strTradeStateDesc
	*
	* @param strOutTradingCode				the trading code which you want to query
	*
	* @return enumWechatRet					
	*/
	enumWechatRet queryPayStatus(
		const std::string& strOutTradingCode,
		CWeChatResps& wechatResps
	);

	/**
	* @name smallProgramLogin
	*
	* @brief								
	*
	* @note									see https://developers.weixin.qq.com/miniprogram/dev/api/api-login.html?t=20161122
	*										the output include strSessionKey and strOpenId
	*
	* @param strJsCode						the jscode from client
	*
	* @return enumWechatRet					
	*/
	enumWechatRet smallProgramLogin(
		const std::string& strJsCode,
		CWeChatResps& wechatResps
	);

	/**
	* @name prepay
	*
	* @brief								get prepayId
	*
	* @note									see https://pay.weixin.qq.com/wiki/doc/api/wxa/wxa_api.php?chapter=9_1&index=1
	*										the output include strTradeType and strPrepayId
	*
	* @param iAmount						trading amount
	* @param llValidTime					trading valid time
	* @param strTradingCode					user trading code
	* @param strRemoteIP					client ip address
	* @param strBody						trading body
	* @param strCallBackAddr				notify url
	* @param strAttach						custom params
	* @param strOpenId						wechat open id
	*										if this is a small program appliction, 
	*										you must input the strOpenId
	*
	*
	* @return enumWechatRet
	*/
	enumWechatRet prepay(
		int iAmount,
		long long llValidTime,
		const std::string& strTradingCode,
		const std::string& strRemoteIP,
		const std::string& strBody,
		const std::string& strCallBackAddr,
		CWeChatResps& wechatResps,
		const std::string& strAttach = std::string(""),
		const std::string& strOpenId = std::string("")
	);

	/**
	* @name prepayWithSign
	*
	* @brief								get prepayId first, then sign again to response client
	*
	* @note									see https://pay.weixin.qq.com/wiki/doc/api/wxa/wxa_api.php?chapter=7_7&index=3
	*										and https://pay.weixin.qq.com/wiki/doc/api/app/app.php?chapter=8_3
	*										the output include strPrepaySignedContent
	*
	*										if you want to custom the strPrepaySignedContent,
	*										you should create a class that inherits from CWechat
	*										and rewrite signSmallProgramPrepayInfo or signAppPrepayInfo
	*
	* @param iAmount						trading amount
	* @param llValidTime					trading valid time
	* @param strTradingCode					user trading code
	* @param strRemoteIP					client ip address
	* @param strBody						trading body
	* @param strCallBackAddr				notify url
	* @param strAttach						custom params
	* @param strOpenId						wechat open id
	*										if this is a small program appliction,
	*										you must input the strOpenId
	*
	*
	* @return enumWechatRet
	*/
	enumWechatRet prepayWithSign(
		int iAmount,
		long long llValidTime,
		const std::string& strTradingCode,
		const std::string& strRemoteIP,
		const std::string& strBody,
		const std::string& strCallBackAddr,
		CWeChatResps& wechatResps,
		const std::string& strAttach = std::string(""),
		const std::string& strOpenId = std::string("")
	);

protected:

	//token
	bool m_bIsApp;
	std::string m_strAppId;
	std::string m_strMchId;
	std::string m_strMchKey;
	std::string m_strAppSecret;
	std::string m_strCertPath;
	std::string m_strCertPassword;
	std::string m_strKeyPath;
	std::string m_strKeyPassword;

protected:
	/**
	* @name signSmallProgramPrepayInfo
	*
	* @brief								append response content to small program
	*
	* @note
	*
	* @param strNonceStr					random string
	* @param strTimeStamp					current timestamp
	* @param strPrepayId					prepayId
	* @param strSignResult					sign
	*
	* @return void
	*/
	virtual void appendSmallProgramPrepayInfo(
		const std::string& strNonceStr,
		const std::string& strTimeStamp,
		const std::string& strPrepayId,
		const std::string& strSignResult,
		std::string& strPrepaySignedContent
	);
	/**
	* @name appendAppPrepayInfo
	*
	* @brief								append response content to app
	*
	* @note
	*
	* @param strNonceStr					random string
	* @param strTimeStamp					current timestamp
	* @param strPrepayId					prepayId
	* @param strSignResult					sign
	*
	* @return void
	*/
	virtual void appendAppPrepayInfo(
		const std::string& strNonceStr,
		const std::string& strTimeStamp,
		const std::string& strPrepayId,
		const std::string& strSignResult,
		std::string& strPrepaySignedContent
	);

	//sign again for client
	std::string signPrepay(
		const std::string& strNonceStr,
		const std::string& strTimeStamp,
		const std::string& strPrepayId
	);

	/**
	* @name sendReqAndParseResps
	*
	* @brief								send request and parse response
	*
	* @note
	*
	* @param strReq							request content
	* @param strHref						request href
	* @param func							if func return true, it will save error info
	*
	* @return int							return enumWechatRet
	*/
	enumWechatRet sendReqAndParseResps(
		const std::string& strReq,
		const std::string& strHref,
		std::function<bool(std::map<std::string, std::string>&, enumWechatRet&)> func
	);

	//append request content
	std::string appendSmallProgramLoginContent(
		const std::string& strJsCode
	);
	std::string appendQueryStatusContent(
		const std::string& strOutTradingCode
	);
	std::string appendPrepayContent(
		int iAmount,
		long long llValidTime,
		const std::string& strTradingCode,
		const std::string& strRemoteIP,
		const std::string& strBody,
		const std::string& strCallBackAddr,
		const std::string& strAttach = "",
		const std::string& strOpenId = ""
	);
};

}

