#pragma once
#include <string>
#include <openssl/pem.h>
#include <openssl/rsa.h>

class CRSAUtils
{
public:
	//验签
	static bool rsa_verify_from_pubKey_with_base64(const std::string &content, const std::string &sign, const std::string &key);

	//加签
	static std::string rsa_sign_from_privKey_with_base64(const std::string& content, const std::string& key);

	//公钥加密。
	static std::string rsa_encrypt_from_pubKey(const std::string& plainText, const std::string& pubKeyBuffer);

	//私钥解密。
	static std::string rsa_decrypt_from_privKey(const std::string& encryptedText, const std::string& privKeyBuffer);

	//私钥加密。
	static std::string rsa_encrypt_from_privKey(const std::string& plainText, const std::string& privKeyBuffer);

	//公钥解密。
	static std::string rsa_decrypt_from_pubKey(const std::string& encryptedText, const std::string& pubKeyBuffer);


private:
	//读取公钥/私钥。
	static RSA* rsa_key_from_buffer(const std::string& keyBuff, bool isPublic);

	//公钥/私钥加密。
	static std::string rsa_encrypt(RSA* rsaPubKey, RSA* rsaPrivKey, const std::string& plainText);

	//私钥/公钥解密。
	static std::string rsa_decrypt(RSA* rsaPubKey, RSA* rsaPrivKey, const std::string& encryptedText);

	//base64加密
	static std::string base64Encode(const unsigned char *bytes, int len);

	//base64解密
	static bool base64Decode(const std::string &str, unsigned char *bytes, int &len);
};


