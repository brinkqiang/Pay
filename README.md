# Pay
用c++简单实现支付宝、微信支付服务端api，支持APP及微信小程序支付
包括：
支付宝prepay字符串生成、退款、单笔转账、查询接口
微信统一下单及再次签名、小程序登录、查询接口

如有任何问题,请联系邮箱samlior@protonmail.com

# include
```
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"  
#include "rapidjson/stringbuffer.h"

#include "Tinyxml/tinyxml.h"

#include <curl/curl.h>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
```

# 使用说明
```
//create alipay object with user information
CAlipay alipay(
	ALIPAY_APP_ID,
	ALIPAY_PUB_KEY,
	ALIPAY_PRIV_KEY
);

//create prepay content for client
string& strAlipayContent = alipay.appendPayContent(
	10000,
	"trading code ABC",
	"your subject",
	"your notify url",
	"90m",
	"your custom params"
);

//create a struct to save the respsonse inforamtion
CAlipayResps alipayResps;

//refund a bill
enumAlipayRet iAlpayRet = alipay.refund(
	10000,
	"your refund trading code",
	"trading code ABC",
	alipayResps
);



//create wechat object with user information
CWeChat wechat(
	WECHAT_APP_ID,
	WECHAT_MCH_ID,
	WECHAT_MCH_KEY,
	//if this application is not a small program, please input true
	false,
	WECHAT_APP_SECRET
);

//get prepay id and sign again for client
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

//get sign
string& strPrepaySignedContent = wechatResps.strPrepaySignedContent;
```