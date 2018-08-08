#pragma once
#include "HttpClient.h"
#include <curl/curl.h>

using namespace std;

vector<string> CHttpClient::makeHeaderWithMap(const map<string, string>& mapHeader)
{
	vector<string> vecHeader;
	for (auto itr = mapHeader.begin(); itr != mapHeader.end(); ++itr)
	{
		string strHeader = itr->first + ": " + itr->second;
		vecHeader.push_back(strHeader);
	}
	return vecHeader;
}

static size_t on_write_data(const char* ptr, size_t size, size_t nmemb, void* data)
{
	string *buffer = (string *)data;
	if (buffer != NULL) 
	{
		size_t read_data_size = size * nmemb;
		buffer->append((const char *)ptr, read_data_size);
		return read_data_size;
	}
	return -1;
}

int CHttpClient::post(
	const string& strHref,
	const string& strData,
	string& strRespsContent,
	string& strRespsHeader /*= string("")*/,
	int iTimeOut /*= CHTTPCLIENT_DEFAULT_TOME_OUT*/,
	const vector<string>& vecHeader /*= vector<string>()*/
) 
{
	int ret = CHTTPCLIENT_RET_UNKNOW_ERROR;
	CURL *curl = curl_easy_init();
	if (curl) 
	{
		struct curl_slist* headers = NULL;
		headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
		for (auto itr = vecHeader.begin(); itr != vecHeader.end(); ++itr)
			headers = curl_slist_append(headers, (*itr).c_str());
		curl_easy_setopt(curl, CURLOPT_URL, strHref.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_POST, true);
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, iTimeOut);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strData.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strData.size());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, on_write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &strRespsContent);
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, on_write_data);
		curl_easy_setopt(curl, CURLOPT_HEADERDATA, &strRespsHeader);
		ret = curl_easy_perform(curl);
		if (headers != NULL)
			curl_slist_free_all(headers);
		curl_easy_cleanup(curl);
	}
	return ret;
}

int CHttpClient::get(
	const string &strHref,
	string& strRespsContent,
	int iTimeOut /*= CHTTPCLIENT_DEFAULT_TOME_OUT*/
)
{
	int ret = CHTTPCLIENT_RET_UNKNOW_ERROR;
	CURL* curl = curl_easy_init();
	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, strHref.c_str());
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, iTimeOut);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, on_write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &strRespsContent);
		ret = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}
	return ret;
}

int CHttpClient::postWithCert(
	const string& strHref,
	const string& strData,
	const string& strCertPath,
	const string& strKeyPath,
	const string& strCertPassword,
	const string& strKeyPassword,
	string& strRespsContent,
	string& strRespsHeader /*= string("")*/,
	int iTimeOut /*= CHTTPCLIENT_DEFAULT_TOME_OUT*/,
	const vector<string>& vecHeader /*= vector<string>()*/
)
{
	int ret = CHTTPCLIENT_RET_UNKNOW_ERROR;
	CURL *curl = curl_easy_init();
	if (curl)
	{
		struct curl_slist* headers = NULL;
		headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
		for (auto itr = vecHeader.begin(); itr != vecHeader.end(); ++itr)
			headers = curl_slist_append(headers, (*itr).c_str());
		curl_easy_setopt(curl, CURLOPT_URL, strHref.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_POST, true);
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, iTimeOut);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSLCERT, strCertPath.c_str());
		curl_easy_setopt(curl, CURLOPT_SSLCERTPASSWD, strCertPassword.c_str());
		curl_easy_setopt(curl, CURLOPT_SSLCERTTYPE, "PEM");
		curl_easy_setopt(curl, CURLOPT_SSLKEY, strKeyPath.c_str());
		curl_easy_setopt(curl, CURLOPT_SSLKEYPASSWD, strKeyPassword.c_str());
		curl_easy_setopt(curl, CURLOPT_SSLKEYTYPE, "PEM");
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strData.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strData.size());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, on_write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &strRespsContent);
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, on_write_data);
		curl_easy_setopt(curl, CURLOPT_HEADERDATA, &strRespsHeader);
		ret = curl_easy_perform(curl);
		if (headers != NULL)
			curl_slist_free_all(headers);
		curl_easy_cleanup(curl);
	}
	return ret;
}