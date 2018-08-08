#include "RSAUtils.h"
#include "PayUtils/Utils.h"

#define RSA_ENC_DATA_SIZE 128
#define RSA_PLAIN_TEXT_SIZE (RSA_ENC_DATA_SIZE - 11)

//密钥的最大长度是512字节 
#define  MAX_RSA_KEY_LENGTH 512 

#ifndef XRSA_KEY_BITS
#define XRSA_KEY_BITS (2048)
#endif

using namespace std;

//公钥加密。
string CRSAUtils::rsa_encrypt_from_pubKey(const string& plainText, const string& pubKeyBuffer)
{
	RSA* rsa = rsa_key_from_buffer(pubKeyBuffer, true);
	if (rsa == NULL)
		return "";


	string encryptedTextHex = rsa_encrypt(rsa, NULL,plainText.c_str());

	RSA_free(rsa);
	
	return encryptedTextHex;
}

//私钥解密。
string CRSAUtils::rsa_decrypt_from_privKey(const string& encryptedText, const string& privKeyBuffer) 
{	
	RSA* rsa = rsa_key_from_buffer(privKeyBuffer, false);
	if (rsa == NULL)
		return "";


	string plainText = rsa_decrypt(NULL,rsa, encryptedText);

	RSA_free(rsa);

	return plainText;
}

//私钥加密。
string CRSAUtils::rsa_encrypt_from_privKey(const string& plainText, const string& privKeyBuffer)
{
	RSA* rsa = rsa_key_from_buffer(privKeyBuffer, false);
	if (rsa == NULL)
		return "";


	string encryptedTextHex = rsa_encrypt(NULL,rsa, plainText.c_str());

	RSA_free(rsa);

	return encryptedTextHex;
}

//公钥解密。
string CRSAUtils::rsa_decrypt_from_pubKey(const string& encryptedText, const string& pubKeyBuffer)
{
	RSA* rsa = rsa_key_from_buffer(pubKeyBuffer, true);
	if (rsa == NULL)
		return "";


	string plainText = rsa_decrypt(rsa, NULL,encryptedText);

	RSA_free(rsa);

	return plainText;
}

RSA* CRSAUtils::rsa_key_from_buffer(const string& keyBuff,bool isPublic) 
{
	BIO *bio = NULL;
	RSA *rsa = NULL;
	char *pcKey = const_cast<char *>(keyBuff.c_str());
	if ((bio = BIO_new_mem_buf(pcKey, -1)) == NULL){ //从字符串读取RSA公钥/私钥。	
		return NULL;
	}

	if (isPublic) {
		rsa = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);   //从bio结构中得到rsa结构
	}
	else {
		rsa = PEM_read_bio_RSAPrivateKey(bio, NULL, NULL, NULL);   //从bio结构中得到rsa结构
	}

	BIO_free_all(bio);

	return rsa;
}

string CRSAUtils::rsa_encrypt(RSA* rsaPubKey, RSA* rsaPrivKey, const string& plainText)
{
	if (rsaPubKey == NULL && rsaPrivKey == NULL) {
		return "";
	}

	string strEncryptedText("");

	int loopCount = plainText.length() / RSA_PLAIN_TEXT_SIZE;
	if (plainText.length() % RSA_PLAIN_TEXT_SIZE > 0) {
		loopCount++;
	}

	unsigned char* encryptedData = new unsigned char[loopCount*RSA_ENC_DATA_SIZE + 1];// 用来存放密文

	int totalSize = 0,encIndex = 0;
	for (int i = 0; i < plainText.length();i += RSA_PLAIN_TEXT_SIZE) {

		string singlePainTextBlock = plainText.substr(i, RSA_PLAIN_TEXT_SIZE);

		int len = 0;		
		if (rsaPubKey != NULL) {// 用公钥加密			
			len = RSA_public_encrypt(singlePainTextBlock.length(), (unsigned char*)singlePainTextBlock.c_str(), &encryptedData[encIndex*RSA_ENC_DATA_SIZE], rsaPubKey, RSA_PKCS1_PADDING);
		}
		else {//私钥加密。
			len = RSA_private_encrypt(singlePainTextBlock.length(), (unsigned char*)singlePainTextBlock.c_str(), &encryptedData[encIndex*RSA_ENC_DATA_SIZE], rsaPrivKey, RSA_PKCS1_PADDING);
		}
		encIndex++;

		if (len <= 0) {
			delete encryptedData;
			return "";
		}
		totalSize += len;
	}

	char *hexData = new char[totalSize * 2 + 1];
	CharHexConverter::char2Hex((char*)encryptedData, totalSize, hexData);


	strEncryptedText.append(hexData, totalSize * 2);

	delete hexData;
	delete encryptedData;

	return strEncryptedText;
}

string CRSAUtils::rsa_decrypt(RSA* rsaPubKey, RSA* rsaPrivKey, const string& encryptedHexText)
{
	if (rsaPubKey == NULL && rsaPrivKey == NULL) {
		return "";
	}

	const int SINGLE_HEX_BLOCK_SIZE = RSA_ENC_DATA_SIZE * 2;

	char* pcEncText = (char*)encryptedHexText.c_str();
	unsigned char * pcText = new  unsigned char[encryptedHexText.length()/2];

	int plainIndex = 0;
	for (int i = 0; i < encryptedHexText.length(); i += SINGLE_HEX_BLOCK_SIZE) {

		char encData[RSA_ENC_DATA_SIZE + 1] = { 0 };
		CharHexConverter::hex2Char(&pcEncText[i], SINGLE_HEX_BLOCK_SIZE, (char*)encData);

		unsigned char decrypted[RSA_PLAIN_TEXT_SIZE + 1] = { 0 };
		int len = 0;
		if (rsaPubKey != NULL) {//公钥解密。
			len = RSA_public_decrypt(RSA_ENC_DATA_SIZE, (unsigned char*)encData, &pcText[plainIndex], rsaPubKey, RSA_PKCS1_PADDING);
		}
		else {//私钥解密。			  
			len = RSA_private_decrypt(RSA_ENC_DATA_SIZE, (unsigned char*)encData, &pcText[plainIndex], rsaPrivKey, RSA_PKCS1_PADDING);
		}

		if (len <= 0) {
			delete pcText;
			return "";
		}
		plainIndex += len;
	}

	string plainText("");
	plainText.append((char*)pcText, plainIndex);

	delete pcText;

	return plainText;
}

string CRSAUtils::base64Encode(const unsigned char *bytes, int len) {

	BIO *bmem = NULL;
	BIO *b64 = NULL;
	BUF_MEM *bptr = NULL;

	b64 = BIO_new(BIO_f_base64());
	BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
	bmem = BIO_new(BIO_s_mem());
	b64 = BIO_push(b64, bmem);
	BIO_write(b64, bytes, len);
	BIO_flush(b64);
	BIO_get_mem_ptr(b64, &bptr);

	string str = string(bptr->data, bptr->length);
	BIO_free_all(b64);
	return str;
}

bool CRSAUtils::base64Decode(const string &str, unsigned char *bytes, int &len) 
{
	const char *cstr = str.c_str();
	BIO *bmem = NULL;
	BIO *b64 = NULL;

	b64 = BIO_new(BIO_f_base64());
	BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
	bmem = BIO_new_mem_buf((void *)cstr, strlen(cstr));
	b64 = BIO_push(b64, bmem);
	len = BIO_read(b64, bytes, len);

	BIO_free_all(b64);
	return len > 0;
}

bool CRSAUtils::rsa_verify_from_pubKey_with_base64(const string &content, const string &sign, const string &key)
{
	bool result = false;
	const char *key_cstr = key.c_str();
	int key_len = strlen(key_cstr);
	BIO *p_key_bio = BIO_new_mem_buf((void *)key_cstr, key_len);
	RSA *p_rsa = PEM_read_bio_RSA_PUBKEY(p_key_bio, NULL, NULL, NULL);

	if (p_rsa != NULL) {
		const char *cstr = content.c_str();
		unsigned char hash[SHA256_DIGEST_LENGTH] = { 0 };
		SHA256((unsigned char *)cstr, strlen(cstr), hash);
		unsigned char sign_cstr[XRSA_KEY_BITS / 8] = { 0 };
		int len = XRSA_KEY_BITS / 8;
		base64Decode(sign, sign_cstr, len);
		unsigned int sign_len = XRSA_KEY_BITS / 8;
		int r = RSA_verify(NID_sha256, hash, SHA256_DIGEST_LENGTH, (unsigned char *)sign_cstr, sign_len, p_rsa);

		if (r > 0) {
			result = true;
		}
	}

	RSA_free(p_rsa);
	BIO_free(p_key_bio);
	return result;
}

string CRSAUtils::rsa_sign_from_privKey_with_base64(const string& content, const string& key)
{
	string signed_str;
	const char *key_cstr = key.c_str();
	int key_len = strlen(key_cstr);
	BIO *p_key_bio = BIO_new_mem_buf((void *)key_cstr, key_len);
	RSA *p_rsa = PEM_read_bio_RSAPrivateKey(p_key_bio, NULL, NULL, NULL);

	if (p_rsa != NULL) {

		const char *cstr = content.c_str();
		unsigned char hash[SHA256_DIGEST_LENGTH] = { 0 };
		SHA256((unsigned char *)cstr, strlen(cstr), hash);
		unsigned char sign[XRSA_KEY_BITS / 8] = { 0 };
		unsigned int sign_len = sizeof(sign);
		int r = RSA_sign(NID_sha256, hash, SHA256_DIGEST_LENGTH, sign, &sign_len, p_rsa);

		if (0 != r && sizeof(sign) == sign_len) {
			signed_str = base64Encode(sign, sign_len);
		}
	}

	RSA_free(p_rsa);
	BIO_free(p_key_bio);
	return signed_str;
}