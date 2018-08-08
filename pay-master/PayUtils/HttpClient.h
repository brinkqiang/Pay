#pragma once
#include <map>
#include <vector>
#include <string>

class CHttpClient
{
#define CHTTPCLIENT_DEFAULT_TOME_OUT 30
#define CHTTPCLIENT_RET_OK 0
#define CHTTPCLIENT_RET_UNKNOW_ERROR -65535
public:
	static std::vector<std::string> makeHeaderWithMap(const std::map<std::string, std::string>& mapHeader);


	static int get(
		const std::string &strHref,
		std::string& strRespsContent,
		int iTimeOut = CHTTPCLIENT_DEFAULT_TOME_OUT
	);


	static int post(
		const std::string& strHref,
		const std::string& strData,
		std::string& strRespsContent,
		std::string& strRespsHeader = std::string(""),
		int iTimeOut = CHTTPCLIENT_DEFAULT_TOME_OUT,
		const std::vector<std::string>& vecHeader = std::vector<std::string>()
	);


	static int postWithCert(
		const std::string& strHref,
		const std::string& strData,
		const std::string& strCertPath,
		const std::string& strKeyPath,
		const std::string& strCertPassword,
		const std::string& strKeyPassword,
		std::string& strRespsContent,
		std::string& strRespsHeader = std::string(""),
		int iTimeOut = CHTTPCLIENT_DEFAULT_TOME_OUT,
		const std::vector<std::string>& vecHeader = std::vector<std::string>()
	);
};

