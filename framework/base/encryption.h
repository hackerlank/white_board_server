/*	class Encryption
	2014-7-9
*/
#ifndef ENCRYPTION_H_
#define ENCRYPTION_H_

#include<stdint.h>
#include<string>
using namespace std;

class Encryption
{
public:
	Encryption(void);
	~Encryption(void);

public:
	static bool NeedVerify(const std::string &flag_from_client);
	static std::string GenerateMsg(std::string & plaintext_to_generate);
	static std::string EncryptMsg(const std::string &msg);
	static void GenerateRc6Key();

	static void CharToHexadecimal(std::string string_in, uint32_t length, std::string & out);
	static void HexadecimalToChar(std::string string_in, uint32_t length, std::string & out);

public:
	static char rc6_key[20];
};

#endif
