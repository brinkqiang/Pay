#include "WeChat.h"
#include "Tinyxml/tinyxml.h"
#include "rapidjson/document.h"
#include "PayUtils/Utils.h"
#include "PayUtils/Md5Utils.h"
#include "PayUtils/HttpClient.h"
#include "PayHeader.h"
#include <stdarg.h>
#include <boost/format.hpp>

using namespace std;
using namespace boost;
using namespace SAPay;

map<string, string> CWeChat::parseRespsContentWechat(const string& strNotify)
{
	map<string, string> mapNameValue;
	TiXmlDocument document;
	document.Parse(strNotify.c_str());
	TiXmlNode* pRoot = document.FirstChild();
	if (pRoot)
	{
		for (TiXmlNode* pChild = pRoot->FirstChild(); pChild != nullptr; pChild = pChild->NextSibling())
		{
			TiXmlNode* pTextChild = pChild->FirstChild();
			if (pChild->Type() == TiXmlNode::TINYXML_ELEMENT &&
				pTextChild &&
				pTextChild->Type() == TiXmlNode::TINYXML_TEXT)
			{
				mapNameValue[pChild->Value()] = pTextChild->ToText()->Value();
			}
		}
	}
	return mapNameValue;
}

int CWeChat::verifyWechatRespsAndNotify(
	const map<string, string>& mapResps,
	const string& strMchKey
)
{
	string content(""), sign("");
	vector<string>& vecDictionary = CUtils::createDictionaryWithMap(mapResps);
	for (auto itr = vecDictionary.cbegin(); itr != vecDictionary.cend(); ++itr)
	{
		auto itrr = mapResps.find(*itr);
		if (itrr != mapResps.end())
		{
			if (*itr == WECHAT_RESPS_SIGN)
			{
				sign = itrr->second;
				continue;
			}
			CUtils::AppendContentWithoutUrlEncode(*itr, itrr->second, content, !content.empty());
		}
	}
	CUtils::AppendContentWithoutUrlEncode("key", strMchKey, content);
	Md5Utils md5;
	string signResult;
	md5.encStr32(content.c_str(), signResult);
	return signResult == sign ? 1 : -1;
}

static void addXmlChild(TiXmlElement* pRoot, const string& key, const string& value)
{
	TiXmlText* pValue = new TiXmlText(value.c_str());
	pValue->SetCDATA(true);
	TiXmlElement* pKey = new TiXmlElement(key.c_str());
	pKey->LinkEndChild(pValue);
	pRoot->LinkEndChild(pKey);
}

CWeChat::CWeChat(
	const std::string& strAppId,
	const std::string& strMchId,
	const std::string& strMchKey,
	bool bIsApp /*= true*/,
	const std::string& strAppSecret /*= std::string("")*/,
	const std::string& strCertPath /*= std::string("")*/,
	const std::string& strCertPassword /*= std::string("")*/,
	const std::string& strKeyPath /*= std::string("")*/,
	const std::string& strKeyPassword /*= std::string("")*/
) :
	CPayBase(),
	m_bIsApp(bIsApp),
	m_strAppId(strAppId),
	m_strMchId(strMchId),
	m_strMchKey(strMchKey),
	m_strAppSecret(strAppSecret),
	m_strCertPath(strCertPath),
	m_strCertPassword(strCertPassword),
	m_strKeyPath(strKeyPath),
	m_strKeyPassword(strKeyPassword)
{
}

enumWechatRet CWeChat::sendReqAndParseResps(
	const std::string& strReq,
	const std::string& strHref,
	std::function<bool(std::map<std::string, std::string>&, enumWechatRet&)> func
)
{
	clearErrorInfo();
	enumWechatRet iRet = CWECHAT_RET_OK;
	while (true)
	{
		string strResps("");
		int iNetWorkRet = CHttpClient::post(strHref, strReq, strResps);
		if (iNetWorkRet)
		{
			SAVE_REQ_RESPS_AND_SET_RET_NETWORK_CODE(strReq, strResps, CWECHAT_RET_NETWORK_ERROR, iNetWorkRet);
			break;
		}

		map<string, string>& mapResps = parseRespsContentWechat(strResps);
		auto itrReturnCode = mapResps.find(WECHAT_RESPS_RETURN_CODE);
		auto itrResultCode = mapResps.find(WECHAT_RESPS_RESULT_CODE);
		if (itrReturnCode == mapResps.end() ||
			itrResultCode == mapResps.end() ||
			itrReturnCode->second != "SUCCESS" ||
			itrResultCode->second != "SUCCESS")
		{
			SAVE_REQ_RESPS_AND_SET_RET(strReq, strResps, CWECHAT_RET_UNKNOW_ERROR);
			auto itrErrCode = mapResps.find(WECHAT_RESPS_ERR_CODE);
			auto itrReturnMsg = mapResps.find(WECHAT_RESPS_RETURN_MSG);
			if (itrErrCode != mapResps.end())
			{
				m_strLastErrInfo = itrErrCode->second;
				iRet = CWECHAT_RET_ERR_CODE_ERROR;
			}
			else if (itrReturnMsg != mapResps.end())
			{
				m_strLastErrInfo = itrReturnMsg->second;
				iRet = CWECHAT_RET_RET_MSG_ERROR;
			}
			break;
		}

		if (verifyWechatRespsAndNotify(mapResps, m_strMchKey) < 0)
		{
			SAVE_REQ_RESPS_AND_SET_RET(strReq, strResps, CWECHAT_RET_VERIFY_ERROR);
			break;
		}
		
		if (func(mapResps, iRet))
		{
			SAVE_REQ_RESPS(strReq, strResps);
		}
		break;
	}
	return iRet;
}

enumWechatRet CWeChat::queryPayStatus(
	const std::string& strOutTradingCode, 
	CWeChatResps& wechatResps
)
{
	return sendReqAndParseResps(
		appendQueryStatusContent(strOutTradingCode),
		WECHAT_HREF_QUERY,
		[&wechatResps](map<string, string>& mapResps, enumWechatRet& iRet) -> bool
	{
		auto itrOpenId = mapResps.find(WECHAT_RESPS_OPEN_ID);
		auto itrTradeType = mapResps.find(WECHAT_RESPS_TRADE_TYPE);
		auto itrTradeState = mapResps.find(WECHAT_RESPS_TRADE_STATE);
		auto itrBankType = mapResps.find(WECHAT_RESPS_BANK_TYPE);
		auto itrTotalFee = mapResps.find(WECHAT_RESPS_TOTAL_FEE);
		auto itrCashFee = mapResps.find(WECHAT_RESPS_CASH_FEE);
		auto itrTransactionId = mapResps.find(WECHAT_RESPS_TRANSACTION_ID);
		auto itrOutTradeNo = mapResps.find(WECHAT_RESPS_OUT_TRADE_NO);
		auto itrTimeEnd = mapResps.find(WECHAT_RESPS_TIME_END);
		auto itrTradeStateDesc = mapResps.find(WECHAT_RESPS_TRADE_STATE_DESC);
		if (itrOpenId == mapResps.end() ||
			itrTradeType == mapResps.end() ||
			itrTradeState == mapResps.end() ||
			itrBankType == mapResps.end() ||
			itrTotalFee == mapResps.end() ||
			itrCashFee == mapResps.end() ||
			itrTransactionId == mapResps.end() ||
			itrOutTradeNo == mapResps.end() ||
			itrTimeEnd == mapResps.end() ||
			itrTradeStateDesc == mapResps.end())
		{
			iRet = CWECHAT_RET_PARSE_ERROR;
			return true;
		}

		wechatResps.strOpenId = itrOpenId->second;
		wechatResps.strTradeType = itrTradeType->second;
		wechatResps.strBankType = itrBankType->second;
		wechatResps.strTotalFee = itrTotalFee->second;
		wechatResps.strCashFee = itrCashFee->second;
		wechatResps.strTransactionId = itrTransactionId->second;
		wechatResps.strOutTradeNo = itrOutTradeNo->second;
		wechatResps.strTimeEnd = itrTimeEnd->second;
		wechatResps.strTradeStateDesc = itrTradeStateDesc->second;

		if (itrTradeState->second == WECHAT_TRADE_STATE_SECCESS)
			wechatResps.iTradeState = CWECHAT_TRADE_STATE_SUCCESS;
		else if(itrTradeState->second == WECHAT_TRADE_STATE_REFUND)
			wechatResps.iTradeState = CWECHAT_TRADE_STATE_REFUND;
		else if (itrTradeState->second == WECHAT_TRADE_STATE_NOTPAY)
			wechatResps.iTradeState = CWECHAT_TRADE_STATE_NOTPAY;
		else if (itrTradeState->second == WECHAT_TRADE_STATE_CLOSED)
			wechatResps.iTradeState = CWECHAT_TRADE_STATE_CLOSED;
		else if (itrTradeState->second == WECHAT_TRADE_STATE_REVOKED)
			wechatResps.iTradeState = CWECHAT_TRADE_STATE_REVOKED;
		else if (itrTradeState->second == WECHAT_TRADE_STATE_USERPAYING)
			wechatResps.iTradeState = CWECHAT_TRADE_STATE_USERPAYING;
		else if (itrTradeState->second == WECHAT_TRADE_STATE_PAYERROR)
			wechatResps.iTradeState = CWECHAT_TRADE_STATE_PAYERROR;
		else
			wechatResps.iTradeState = CWECHAT_TRADE_STATE_UNKNOW;
		return false;
	}
	);
}

enumWechatRet CWeChat::smallProgramLogin(
	const std::string& strJsCode,
	CWeChatResps& wechatResps
)
{
	clearErrorInfo();
	enumWechatRet iRet = CWECHAT_RET_OK;
	while (true)
	{
		if (m_strAppSecret.empty())
		{
			iRet = CWECHAT_RET_MISSING_APP_SECRET;
			break;
		}

		string& strReq = appendSmallProgramLoginContent(strJsCode);
		string strResps("");
		int iNetWorkRet = CHttpClient::get(WECHAT_HREF_SMALL_PROGRAM_LOGIN + strReq, strResps);
		if (iNetWorkRet)
		{
			SAVE_REQ_RESPS_AND_SET_RET_NETWORK_CODE(strReq, strResps, CWECHAT_RET_NETWORK_ERROR, iNetWorkRet);
			break;
		}

		rapidjson::Document respsDocument;
		respsDocument.Parse(strResps.c_str(), strResps.length());
		if (!respsDocument.IsObject() ||
			!respsDocument.HasMember(WECHAT_RESPS_SESSION_KEY) ||
			!respsDocument.HasMember(WECHAT_RESPS_OPEN_ID) ||
			!respsDocument[WECHAT_RESPS_SESSION_KEY].IsString() ||
			!respsDocument[WECHAT_RESPS_OPEN_ID].IsString())
		{
			SAVE_REQ_RESPS_AND_SET_RET(strReq, strResps, CWECHAT_RET_PARSE_ERROR);
			break;
		}

		wechatResps.strSessionKey = respsDocument[WECHAT_RESPS_SESSION_KEY].GetString();
		wechatResps.strOpenId = respsDocument[WECHAT_RESPS_OPEN_ID].GetString();
		break;
	}
	return iRet;
}

enumWechatRet CWeChat::prepay(
	int iAmount,
	long long llValidTime,
	const std::string& strTradingCode,
	const std::string& strRemoteIP,
	const std::string& strBody,
	const std::string& strCallBackAddr,
	CWeChatResps& wechatResps,
	const std::string& strAttach /*= std::string("")*/,
	const std::string& strOpenId /*= std::string("")*/
)
{
	return sendReqAndParseResps(
		appendPrepayContent(iAmount, llValidTime, strTradingCode, strRemoteIP, 
			strBody, strCallBackAddr, strAttach, strOpenId),
		WECHAT_HREF_PREPAY,
		[&wechatResps](map<string, string>& mapResps, enumWechatRet& iRet) -> bool
	{
		auto itrTradeType = mapResps.find(WECHAT_RESPS_TRADE_TYPE);
		auto itrPrepayId = mapResps.find(WECHAT_RESPS_PREPAY_ID);
		if (itrTradeType == mapResps.end() ||
			itrPrepayId == mapResps.end())
		{
			iRet = CWECHAT_RET_PARSE_ERROR;
			return true;
		}

		wechatResps.strTradeType = itrTradeType->second;
		wechatResps.strPrepayId = itrPrepayId->second;
		return false;
	}
	);
}

enumWechatRet CWeChat::prepayWithSign(
	int iAmount,
	long long llValidTime,
	const std::string& strTradingCode,
	const std::string& strRemoteIP,
	const std::string& strBody,
	const std::string& strCallBackAddr,
	CWeChatResps& wechatResps,
	const std::string& strAttach /*= std::string("")*/,
	const std::string& strOpenId /*= std::string("")*/
)
{
	enumWechatRet iRet = prepay(iAmount, llValidTime, strTradingCode, strRemoteIP,
		strBody, strCallBackAddr, wechatResps, strAttach, strOpenId);
	if (iRet == CWECHAT_RET_OK)
	{
		string& strNonceStr = CUtils::generate_unique_string(32);
		string& strTimeStamp = CUtils::getCurentTimeStampStr();
		string& strSignResult = signPrepay(strNonceStr, strTimeStamp, wechatResps.strPrepayId);
		m_bIsApp ?
			appendAppPrepayInfo(strNonceStr, strTimeStamp, wechatResps.strPrepayId, strSignResult, wechatResps.strPrepaySignedContent) :
			appendSmallProgramPrepayInfo(strNonceStr, strTimeStamp, wechatResps.strPrepayId, strSignResult, wechatResps.strPrepaySignedContent);
	}
	return iRet;
}

string CWeChat::signPrepay(
	const std::string& strNonceStr,
	const std::string& strTimeStamp,
	const std::string& strPrepayId
)
{
	string signContent("");
	if (m_bIsApp)
	{
		CUtils::AppendContentWithoutUrlEncode("appid", m_strAppId, signContent, false);
		CUtils::AppendContentWithoutUrlEncode("noncestr", strNonceStr, signContent);
		CUtils::AppendContentWithoutUrlEncode("package", "Sign=WXPay", signContent);
		CUtils::AppendContentWithoutUrlEncode("partnerid", m_strMchId, signContent);
		CUtils::AppendContentWithoutUrlEncode("prepayid", strPrepayId, signContent);
		CUtils::AppendContentWithoutUrlEncode("timestamp", strTimeStamp, signContent);
		CUtils::AppendContentWithoutUrlEncode("key", m_strMchKey, signContent);
	}
	else
	{
		string packageContent = "prepay_id=" + strPrepayId;
		CUtils::AppendContentWithoutUrlEncode("appId", m_strAppId, signContent, false);
		CUtils::AppendContentWithoutUrlEncode("nonceStr", strNonceStr, signContent);
		CUtils::AppendContentWithoutUrlEncode("package", packageContent, signContent);
		CUtils::AppendContentWithoutUrlEncode("signType", "MD5", signContent);
		CUtils::AppendContentWithoutUrlEncode("timeStamp", strTimeStamp, signContent);
		CUtils::AppendContentWithoutUrlEncode("key", m_strMchKey, signContent);
	}
	string signResult("");
	Md5Utils m5;
	m5.encStr32(signContent.c_str(), signResult);
	return signResult;
}

void CWeChat::appendAppPrepayInfo(
	const std::string& strNonceStr,
	const std::string& strTimeStamp,
	const std::string& strPrepayId,
	const std::string& strSignResult,
	string& strPrepaySignedContent
)
{
	format f("{\"timeStamp\":\"%s\",\"nonceStr\":\"%s\",\"package\":\"Sign=WXPay\",\"paySign\":\"%s\","\
		"\"prepayid\":\"%s\",\"partnerid\":\"%s\",\"appid\":\"%s\"}");
	f % strTimeStamp.c_str() % strNonceStr.c_str() % strSignResult.c_str() 
		% strPrepayId.c_str() % m_strMchId.c_str() % m_strAppId.c_str();
	strPrepaySignedContent = f.str();
};

void CWeChat::appendSmallProgramPrepayInfo(
	const std::string& strNonceStr,
	const std::string& strTimeStamp,
	const std::string& strPrepayId,
	const std::string& strSignResult,
	string& strPrepaySignedContent
)
{
	format f("{\"timeStamp\":\"%s\",\"nonceStr\":\"%s\",\"package\":\"prepay_id=%s\",\"signType\":\"MD5\",\"paySign\":\"\%s\"}");
	f % strTimeStamp.c_str() % strNonceStr.c_str() % strPrepayId.c_str() % strSignResult.c_str();
	strPrepaySignedContent = f.str();
};

string CWeChat::appendSmallProgramLoginContent(const string& strJsCode)
{
	string strTotalContent("");
	CUtils::AppendContentWithUrlEncode(WECHAT_REQ_APP_ID, m_strAppId, strTotalContent, false);
	CUtils::AppendContentWithUrlEncode(WECHAT_REQ_SECRET, m_strAppSecret, strTotalContent);
	CUtils::AppendContentWithUrlEncode(WECHAT_REQ_JS_CODE, strJsCode, strTotalContent);
	CUtils::AppendContentWithUrlEncode(WECHAT_REQ_GRANT_TYPE, "authorization_code", strTotalContent);
	return strTotalContent;
}

string CWeChat::appendQueryStatusContent(const string& strOutTradingCode)
{
	string& strNonceStr = CUtils::generate_unique_string(32);
	string signContent("");
	CUtils::AppendContentWithoutUrlEncode(WECHAT_REQ_APP_ID, m_strAppId, signContent, false);
	CUtils::AppendContentWithoutUrlEncode(WECHAT_REQ_MCH_ID, m_strMchId, signContent);
	CUtils::AppendContentWithoutUrlEncode(WECHAT_REQ_NONCE_STR, strNonceStr, signContent);
	CUtils::AppendContentWithoutUrlEncode(WECHAT_REQ_OUT_TRADE_NO, strOutTradingCode, signContent);
	CUtils::AppendContentWithoutUrlEncode(WECHAT_REQ_MCH_KEY, m_strMchKey, signContent);
	string strSignResult("");
	Md5Utils m5;
	m5.encStr32(signContent.c_str(), strSignResult);

	TiXmlElement* root = new TiXmlElement(WECHAT_XML_ROOT);
	addXmlChild(root, WECHAT_REQ_APP_ID, m_strAppId);
	addXmlChild(root, WECHAT_REQ_MCH_ID, m_strMchId);
	addXmlChild(root, WECHAT_REQ_NONCE_STR, strNonceStr);
	addXmlChild(root, WECHAT_REQ_OUT_TRADE_NO, strOutTradingCode);
	addXmlChild(root, WECHAT_REQ_SIGN, strSignResult);

	TiXmlDocument document;
	document.LinkEndChild(root);
	TiXmlPrinter printer;
	document.Accept(&printer);
	return printer.CStr();
}

string CWeChat::appendPrepayContent(
	int iAmount,
	long long llValidTime,
	const string& strTradingCode,
	const string& strRemoteIP,
	const string& strBody,
	const string& strCallBackAddr,
	const string& strAttach, /*= ""*/
	const string& strOpenId /*= ""*/
)
{
	string& strNonceStr = CUtils::generate_unique_string(32);
	string& strTimeExpire = CUtils::getDelayTime(llValidTime, false);
#ifdef CHECK_INPUT_STRING_TYPE
	const string& u8Body = ch_trans::is_utf8(strBody.c_str()) ? strBody : ch_trans::ascii_to_utf8(strBody);
#else
	const string& u8Body = strBody;
#endif

#ifdef CHECK_INPUT_STRING_TYPE
	string u8Attach("");
	if (!strAttach.empty())
		u8Attach = ch_trans::is_utf8(strAttach.c_str()) ? strAttach : ch_trans::ascii_to_utf8(strAttach);
#else
	const string& u8Attach = strAttach;
#endif

	const string& strTradeType = m_bIsApp ? "APP" : "JSAPI";

	string signContent("");
	CUtils::AppendContentWithoutUrlEncode(WECHAT_REQ_APP_ID, m_strAppId, signContent, false);
	//add attach id if exist
	if (!u8Attach.empty())
		CUtils::AppendContentWithoutUrlEncode(WECHAT_REQ_ATTACH, u8Attach, signContent);
	CUtils::AppendContentWithoutUrlEncode(WECHAT_REQ_BODY, u8Body, signContent);
	CUtils::AppendContentWithoutUrlEncode(WECHAT_REQ_MCH_ID, m_strMchId, signContent);
	CUtils::AppendContentWithoutUrlEncode(WECHAT_REQ_NONCE_STR, strNonceStr, signContent);
	CUtils::AppendContentWithoutUrlEncode(WECHAT_REQ_NOTIFY_URL, strCallBackAddr, signContent);
	//add open id if exist
	if (!strOpenId.empty())
		CUtils::AppendContentWithoutUrlEncode(WECHAT_REQ_OPEN_ID, strOpenId, signContent);
	CUtils::AppendContentWithoutUrlEncode(WECHAT_REQ_OUT_TRADE_NO, strTradingCode, signContent);
	CUtils::AppendContentWithoutUrlEncode(WECHAT_REQ_SPBILL_CREATE_IP, strRemoteIP, signContent);
	CUtils::AppendContentWithoutUrlEncode(WECHAT_REQ_TIME_EXPIRE, strTimeExpire, signContent);
	CUtils::AppendContentWithoutUrlEncode(WECHAT_REQ_TOTAL_FEE, CUtils::i2str(iAmount), signContent);
	CUtils::AppendContentWithoutUrlEncode(WECHAT_REQ_TRADE_TYPE, strTradeType, signContent);
	CUtils::AppendContentWithoutUrlEncode(WECHAT_REQ_MCH_KEY, m_strMchKey, signContent);
	string signResult("");
	Md5Utils m5;
	m5.encStr32(signContent.c_str(), signResult);

	TiXmlElement* root = new TiXmlElement(WECHAT_XML_ROOT);
	addXmlChild(root, WECHAT_REQ_APP_ID, m_strAppId);
	addXmlChild(root, WECHAT_REQ_MCH_ID, m_strMchId);
	addXmlChild(root, WECHAT_REQ_NONCE_STR, strNonceStr);
	addXmlChild(root, WECHAT_REQ_BODY, u8Body);
	addXmlChild(root, WECHAT_REQ_OUT_TRADE_NO, strTradingCode);
	addXmlChild(root, WECHAT_REQ_TOTAL_FEE, CUtils::i2str(iAmount));
	addXmlChild(root, WECHAT_REQ_SPBILL_CREATE_IP, strRemoteIP);
	addXmlChild(root, WECHAT_REQ_TIME_EXPIRE, strTimeExpire);
	addXmlChild(root, WECHAT_REQ_NOTIFY_URL, strCallBackAddr);
	addXmlChild(root, WECHAT_REQ_TRADE_TYPE, strTradeType);
	addXmlChild(root, WECHAT_REQ_SIGN, signResult);
	//add attach id if exist
	if (!u8Attach.empty())
		addXmlChild(root, WECHAT_REQ_ATTACH, u8Attach);
	//add open id if exist
	if (!strOpenId.empty())
		addXmlChild(root, WECHAT_REQ_OPEN_ID, strOpenId);

	TiXmlDocument document;
	document.LinkEndChild(root);
	TiXmlPrinter printer;
	document.Accept(&printer);
	return printer.CStr();
}