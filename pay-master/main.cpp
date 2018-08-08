#include <string>
#include <iostream>
#include "Pay/Alipay.h"
#include "Pay/WeChat.h"

#define ALIPAY_APP_ID			"your alipay app id"
#define ALIPAY_PUB_KEY			"your alipay pub key"
#define ALIPAY_PRIV_KEY			"your alipay priv key"

#define WECHAT_APP_ID			"your wechat app id"
#define WECHAT_MCH_ID			"your wechat mch id"
#define WECHAT_MCH_KEY			"your wechat mch key"
#define WECHAT_APP_SECRET		"your wechat app secret"

using namespace std;
using namespace SAPay;

int main()
{
	CAlipay alipay(
		ALIPAY_APP_ID,
		ALIPAY_PUB_KEY,
		ALIPAY_PRIV_KEY
	);

	//create alipay content for client to call the alipay client sdk
	string& strAlipayContent = alipay.appendPayContent(
		10000,
		"trading code ABC",
		"your subject",
		"your notify url",
		"90m",
		"your custom params"
	);
	cout << "alipay content : " << strAlipayContent << endl << endl << endl << endl;

	//refund
	CAlipayResps alipayResps;
	enumAlipayRet iAlpayRet = alipay.refund(
		10000,
		"your refund trading code",
		"trading code ABC",
		alipayResps
	);

	if (iAlpayRet == CALIPAY_RET_OK)
	{
		cout << "success" << endl;
		cout << alipayResps.strRefundFee << endl;
		cout << alipayResps.strGmtRefundPay << endl;
	}
	else if (iAlpayRet == CALIPAY_RET_NETWORK_ERROR)
	{
		cout << "net work error, code : " << alipay.getLastErrInfo() << endl;
	}
	else if (iAlpayRet == CALIPAY_RET_VERIFY_ERROR)
	{
		cout << "verify error, req : " << alipay.getLastReq() << " resps : " << alipay.getLastResps() << endl;
	}
	else if (iAlpayRet == CALIPAY_RET_PARSE_ERROR)
	{
		cout << "resps parse error, req : " << alipay.getLastReq() << " resps : " << alipay.getLastResps() << endl;
	}
	else if (iAlpayRet == CALIPAY_RET_SUB_CODE_ERROR)
	{
		cout << "sub code error, sub_code : " << alipay.getLastErrInfo() << " req : " << alipay.getLastReq() << " resps : " << alipay.getLastResps() << endl;
	}
	cout << endl << endl << endl;




	CWeChat wechat(
		WECHAT_APP_ID,
		WECHAT_MCH_ID,
		WECHAT_MCH_KEY,
		false,
		WECHAT_APP_SECRET
	);

	//prepay and sign again
	CWeChatResps wechatResps;
	enumWechatRet iWeChatRet = wechat.prepayWithSign(
		10000,
		5000,
		"your trading code",
		"client ip address",
		"body",
		"your notify url",
		wechatResps
	);

	if (iWeChatRet == CWECHAT_RET_OK)
	{
		cout << "success" << endl;
		cout << wechatResps.strPrepaySignedContent << endl;
	}
	else if (iWeChatRet == CWECHAT_RET_NETWORK_ERROR)
	{
		cout << "net work error, code : " << wechat.getLastErrInfo() << endl;
	}
	else if (iWeChatRet == CWECHAT_RET_VERIFY_ERROR)
	{
		cout << "verify error, req : " << wechat.getLastReq() << " resps : " << wechat.getLastResps() << endl;
	}
	else if (iWeChatRet == CWECHAT_RET_PARSE_ERROR)
	{
		cout << "resps parse error, req : " << wechat.getLastReq() << " resps : " << wechat.getLastResps() << endl;
	}
	else if (iWeChatRet == CWECHAT_RET_ERR_CODE_ERROR)
	{
		cout << "sub code error, err_code : " << wechat.getLastErrInfo() << " req : " << wechat.getLastReq() << " resps : " << wechat.getLastResps() << endl;
	}
	else if (iWeChatRet == CWECHAT_RET_RET_MSG_ERROR)
	{
		cout << "return msg error, return_msg : " << wechat.getLastErrInfo() << " req : " << wechat.getLastReq() << " resps : " << wechat.getLastResps() << endl;
	}
	return 0;
}