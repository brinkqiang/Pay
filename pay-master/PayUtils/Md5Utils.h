// Md5.h: interface for the MD5 class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ___MD_H__2
#define ___MD_H__2
#include <string>

class Md5Utils
{
public:
	Md5Utils();
	virtual ~Md5Utils();
	void MD5Update ( unsigned char *input, unsigned int inputLen);
	void MD5Final (unsigned char digest[16]);

	void encStr32(const char* pcData, std::string& dest);
	void encStr32(const char* pcData,unsigned int ilen, std::string& dest);
	void encStr16(const char* pcData, unsigned int ilen, unsigned char* encryptedData);
	bool encFile32(const char *path, char *hex32);

private:
	unsigned long int state[4];					/* state (ABCD) */
	unsigned long int count[2];					/* number of bits, modulo 2^64 (lsb first) */
	unsigned char buffer[64];       /* input buffer */
	unsigned char PADDING[64];		/* What? */

private:
	void MD5Init ();
	void MD5Transform (unsigned long int state[4], unsigned char block[64]);
	void MD5_memcpy (unsigned char* output, unsigned char* input,unsigned int len);
	void Encode (unsigned char *output, unsigned long int *input,unsigned int len);
	void Decode (unsigned long int *output, unsigned char *input, unsigned int len);
	void MD5_memset (unsigned char* output,int value,unsigned int len);
};

#endif
