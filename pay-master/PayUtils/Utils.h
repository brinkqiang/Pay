#pragma once
#include <map>
#include <vector>
#include <string>

class CUtils
{
public:
	static void split_string(std::string str, const std::string& strToken, std::vector<std::string>& vecStr);
	static int get_random_int(int start, int end);
	static std::string generate_unique_string(const unsigned int max_str_len = 8);
	static std::string generate_unique_int(const unsigned int max_str_len = 4);
	static std::string i2str(int i);
	static std::string i2str(long long ll);

	static std::vector<std::string> createDictionaryWithMap(std::map<std::string, std::string> mapNameValue);

	static std::string getCurentTime(bool bExtended = true);
	static std::string getCurentDate(bool bExtended = true);
	static std::string getDelayTime(long long llDelay, bool bExtended = true, const std::string& strOriginalTime = std::string(""));
	static long long getCurentTimeStampLL();
	static std::string getCurentTimeStampStr();

	static std::string UrlEncode(const std::string& str);
	static std::string UrlDecode(const std::string& str);
	static void AppendContent(const std::string& strName, const std::string& strValue, std::string& strTotalString, std::string& strClearString = std::string(""), bool bNeedSep = true);
	static void AppendContentWithUrlEncode(const std::string& strName, const std::string& strValue, std::string& strTotalString, bool bNeedSep = true);
	static void AppendContentWithoutUrlEncode(const std::string& strName, const std::string& strValue, std::string& strClearString, bool bNeedSep = true);
};

class ch_trans
{
public:
	static bool is_utf8(const char* str);
	static std::string utf8_to_ascii(const std::string& utf8);
	static std::string ascii_to_utf8(const std::string& ascii);
};

class CharHexConverter {
public:
	static void hex2Char(char *pszHexStr, int iSize, char *pucCharStr);
	static void char2Hex(char *pucCharStr, int iSize, char *pszHexStr);
	static void Char2Hex(unsigned char ch, char *szHex);
	static void Hex2Char(char *szHex, unsigned char *rch);
	static unsigned char ToHex(unsigned char x);
	static unsigned char FromHex(unsigned char x);
};
