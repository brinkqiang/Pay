#include "Utils.h"
#include <codecvt>
#include <random>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;

using WCHAR_GBK = codecvt_byname<wchar_t, char, mbstate_t>;
using WCHAR_UTF8 = codecvt_utf8<wchar_t>;

#ifdef _WIN32
#define GBK_NAME ".936"
#else
#define GBK_NAME "zh_CN.GBK"
#endif

bool ch_trans::is_utf8(const char* str)
{
	if (str == 0) 
	{
		return false;
	}

	bool IsUTF8 = true;
	unsigned char* start = (unsigned char*)str;
	unsigned char* end = (unsigned char*)str + strlen(str);
	while (start < end)
	{
		if (*start < 0x80) // (10000000): 值小于0x80的为ASCII字符  
		{
			start++;
		}
		else if (*start < (0xC0)) // (11000000): 值介于0x80与0xC0之间的为无效UTF-8字符  
		{
			IsUTF8 = false;
			break;
		}
		else if (*start < (0xE0)) // (11100000): 此范围内为2字节UTF-8字符  
		{
			if (start >= end - 1)
				break;
			if ((start[1] & (0xC0)) != 0x80)
			{
				IsUTF8 = false;
				break;
			}
			start += 2;
		}
		else if (*start < (0xF0)) // (11110000): 此范围内为3字节UTF-8字符  
		{
			if (start >= end - 2)
				break;
			if ((start[1] & (0xC0)) != 0x80 || (start[2] & (0xC0)) != 0x80)
			{
				IsUTF8 = false;
				break;
			}
			start += 3;
		}
		else
		{
			IsUTF8 = false;
			break;
		}
	}
	return IsUTF8;
}

string ch_trans::utf8_to_ascii(const string& utf8)
{
	wstring_convert<WCHAR_GBK>  cvtGBK(new WCHAR_GBK(GBK_NAME));
	wstring_convert<WCHAR_UTF8> cvtUTF8;
	wstring ustr = cvtUTF8.from_bytes(utf8);
	return cvtGBK.to_bytes(ustr);
}

string ch_trans::ascii_to_utf8(const string& ascii)
{
	wstring_convert<WCHAR_GBK>  cvtGBK(new WCHAR_GBK(GBK_NAME));
	wstring_convert<WCHAR_UTF8> cvtUTF8;
	wstring ustr = cvtGBK.from_bytes(ascii);
	return cvtUTF8.to_bytes(ustr);
}

void CUtils::split_string(string str, const string& strToken, vector<std::string>& vecStr)
{
	if (str.empty())
		return;

	while (true) {
		size_t pos = str.find(strToken);
		if (pos != string::npos) {
			string val = str.substr(0, pos);
			vecStr.push_back(val);
		}
		else {
			vecStr.push_back(str);
			break;
		}

		str = str.substr(pos + strToken.length());
		if (str.empty()) {
			break;
		}
	}
}

static string trimStr(string& str, bool bNeedSpace)
{
	static const string space = " ";
	size_t pos = str.find('T');
	if (pos != string::npos &&
		pos != str.size() - 1)
	{
		string& time = str.substr(pos + 1);
		string& date = str.substr(0, pos);
		bNeedSpace ?
			str = date + space + time :
			str = date + time;
	}
	return str;
}

string CUtils::getCurentTime(bool bExtended /*= true*/)
{
	ptime p = second_clock::local_time();
	return trimStr(
		bExtended?
			to_iso_extended_string(p):
			to_iso_string(p),
		bExtended
	);
}

string CUtils::getCurentDate(bool bExtended /*= true*/)
{
	return bExtended ?
		to_iso_extended_string(day_clock::local_day()) :
		to_iso_string(day_clock::local_day());
}

string CUtils::getDelayTime(
	long long llDelay,
	bool bExtended /*= true*/,
	const string& strOriginalTime /*= string("")*/
)
{
	ptime p;
	strOriginalTime.empty() ?
		p = second_clock::local_time() :
		p = time_from_string(strOriginalTime);
	p += seconds(llDelay);
	return trimStr(
		bExtended ?
			to_iso_extended_string(p) :
			to_iso_string(p),
		bExtended
	);
}

int CUtils::get_random_int(int start, int end) {

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(start, end);

	return dis(gen);
}
string CUtils::generate_unique_string(const unsigned int max_str_len /*= 8*/)
{
	static const char szAcsiiTable[] = {
		'a', 'b', 'c', 'd', 'e',
		'f', 'g', 'h', 'i', 'j',
		'k', 'l', 'm', 'n', 'o',
		'p', 'q', 'r', 's', 't',
		'u', 'v', 'w', 'x', 'y',
		'z', '1', '2', '3', '4',
		'5', '6', '7', '8', '9',
		'0'
	};
	static const int table_len = sizeof(szAcsiiTable) / sizeof(char);

	string str;
	for (unsigned int i = 0; i < max_str_len; i++) {

		int index = get_random_int(0, table_len - 1);
		str.append(1, szAcsiiTable[index]);
	}

	return str;
}

string CUtils::generate_unique_int(const unsigned int max_str_len /*= 4*/)
{
	static const char szAcsiiTable[] = {
		'0', '1', '2', '3', '4',
		'5', '6', '7', '8', '9'
	};

	static const int table_len = sizeof(szAcsiiTable) / sizeof(char);

	string str;
	for (unsigned int i = 0; i < max_str_len; i++) {

		int index = get_random_int(0, table_len - 1);
		str.insert(i, 1, szAcsiiTable[index]);
	}

	return str;
}

long long CUtils::getCurentTimeStampLL()
{
	return time(nullptr);
}

string CUtils::getCurentTimeStampStr()
{
	return lexical_cast<string>(time(nullptr));
}

string CUtils::i2str(int i)
{
	return lexical_cast<string>(i);
}

string CUtils::i2str(long long ll)
{
	return lexical_cast<string>(ll);
}

vector<string> CUtils::createDictionaryWithMap(map<string, string> mapNameValue)
{
	vector<string> vecDictionary;
	while (mapNameValue.size() > 0)
	{
		auto itr = mapNameValue.begin();
		for (auto itrr = mapNameValue.begin(); itrr != mapNameValue.end(); ++itrr)
		{
			if (itr->first > itrr->first)
				itr = itrr;
		}
		vecDictionary.push_back(itr->first);
		mapNameValue.erase(itr);
	}
	return vecDictionary;
}

string CUtils::UrlEncode(const std::string& str)
{
	std::string strTemp = "";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++)
	{
		if (isalnum((unsigned char)str[i]) ||
			(str[i] == '-') ||
			(str[i] == '_') ||
			(str[i] == '.') ||
			(str[i] == '~'))
			strTemp += str[i];
		else if (str[i] == ' ')
			strTemp += "+";
		else
		{
			strTemp += '%';
			strTemp += CharHexConverter::ToHex((unsigned char)str[i] >> 4);
			strTemp += CharHexConverter::ToHex((unsigned char)str[i] % 16);
		}
	}
	return strTemp;
}

string CUtils::UrlDecode(const std::string& str)
{
	std::string strTemp = "";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++)
	{
		if (str[i] == '+') strTemp += ' ';
		else if (str[i] == '%')
		{
			unsigned char high = CharHexConverter::FromHex((unsigned char)str[++i]);
			unsigned char low = CharHexConverter::FromHex((unsigned char)str[++i]);
			strTemp += high * 16 + low;
		}
		else strTemp += str[i];
	}
	return strTemp;
}

void CUtils::AppendContent(
	const std::string& strName, 
	const std::string& strValue, 
	std::string& strTotalString, 
	std::string& strClearString /*= std::string("")*/, 
	bool bNeedSep /*= true*/
)
{
	if (bNeedSep)
	{
		strTotalString = strTotalString + "&" + strName + "=" + UrlEncode(strValue);
		strClearString = strClearString + "&" + strName + "=" + strValue;
	}
	else
	{
		strTotalString = strName + "=" + UrlEncode(strValue);
		strClearString = strName + "=" + strValue;
	}
}

void CUtils::AppendContentWithUrlEncode(
	const std::string& strName,
	const std::string& strValue,
	std::string& strTotalString /*= std::string("")*/,
	bool bNeedSep /*= true*/
)
{
	bNeedSep ?
		strTotalString = strTotalString + "&" + strName + "=" + UrlEncode(strValue) :
		strTotalString = strName + "=" + UrlEncode(strValue);
}
void CUtils::AppendContentWithoutUrlEncode(
	const std::string& strName,
	const std::string& strValue,
	std::string& strClearString /*= std::string("")*/,
	bool bNeedSep /*= true*/
)
{
	bNeedSep ?
		strClearString = strClearString + "&" + strName + "=" + strValue:
		strClearString = strName + "=" + strValue;
}

void CharHexConverter::hex2Char(char *pszHexStr, int iSize, char *pucCharStr)
{
	int i;
	unsigned char ch;
	if (iSize % 2 != 0) return;
	for (i = 0; i < iSize / 2; i++)
	{
		Hex2Char(pszHexStr + 2 * i, &ch);
		pucCharStr[i] = ch;
	}
}
void CharHexConverter::char2Hex(char *pucCharStr, int iSize, char *pszHexStr)
{
	int i;
	char szHex[3];
	pszHexStr[0] = 0;
	for (i = 0; i < iSize; i++)
	{
		Char2Hex(pucCharStr[i], szHex);
		strcat(pszHexStr, szHex);
	}
}

void CharHexConverter::Char2Hex(unsigned char ch, char *szHex)
{
	int i;
	unsigned char byte[2];
	byte[0] = ch / 16;
	byte[1] = ch % 16;
	for (i = 0; i < 2; i++)
	{
		if (byte[i] >= 0 && byte[i] <= 9)
			szHex[i] = '0' + byte[i];
		else
			szHex[i] = 'a' + byte[i] - 10;
	}
	szHex[2] = 0;
}

void CharHexConverter::Hex2Char(char *szHex, unsigned char *rch)
{
	int i;
	for (i = 0; i < 2; i++)
	{
		if (*(szHex + i) >= '0' && *(szHex + i) <= '9')
			*rch = (*rch << 4) + (*(szHex + i) - '0');
		else if (*(szHex + i) >= 'a' && *(szHex + i) <= 'f')
			*rch = (*rch << 4) + (*(szHex + i) - 'a' + 10);
		else
			break;
	}
}

unsigned char CharHexConverter::ToHex(unsigned char x)
{
	return  x > 9 ? x + 55 : x + 48;
}

unsigned char CharHexConverter::FromHex(unsigned char x)
{
	unsigned char y;
	if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
	else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
	else if (x >= '0' && x <= '9') y = x - '0';
	return y;
}