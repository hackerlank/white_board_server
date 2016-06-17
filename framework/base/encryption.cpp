#include "encryption.h"
#include "encrypt.h"
#include "encrypthelper.h"
#include <string.h>

char Encryption::rc6_key[20] = {0};
Encryption::Encryption(void)
{
}

Encryption::~Encryption(void)
{
}

void Encryption::GenerateRc6Key()
{
	const char *rc6_key_middle = "f12c070f45a4416b80b6f6a0a47fb6e6fe709258";   

	memset(rc6_key, 0, sizeof(rc6_key)); 
	uint32_t rc6_key_length = sizeof(rc6_key); 
	uint32_t rc6_middle_length = 40; 
	EncryptHelper::MD5GenerationHelper((uint8_t*)rc6_key_middle, rc6_middle_length, (uint8_t*)rc6_key, rc6_key_length);
}

bool Encryption::NeedVerify(const std::string &flag_from_client)
{
	return true;
}

std::string Encryption::GenerateMsg(std::string & plaintext_to_generate)
{
	std::string string_rc6;
	
	Encrypt encrypt;
	plaintext_to_generate = encrypt.plainttext();

	char output[2*PLAINTTEXT_LENGTH] = {0};
	uint32_t output_length = 2*PLAINTTEXT_LENGTH;
	EncryptHelper::RC6EncryptionHelper((uint8_t*)Encryption::rc6_key, (uint8_t*)plaintext_to_generate.c_str(), PLAINTTEXT_LENGTH, (uint8_t*)output, output_length);
	string_rc6 = std::string(output, output_length);

	std::string string_out;
	CharToHexadecimal(string_rc6, string_rc6.length(), string_out);

	return string_out;
}

std::string Encryption::EncryptMsg(const std::string &msg)
{
	if (msg.length() != 42)
	{
	
	}
	Encrypt encrypt(msg);
	int num = 0;
	if (encrypt.EncryptMessage(num) < 0)
	{
		//NLOG("Encryption::EncryptMsg: Encrypt Error!");
	}
	//NLOG("Encryption::EncryptMsg: Encrypt type:%d", num);
	return encrypt.ciphertext();
}

void Encryption::CharToHexadecimal(std::string string_in, uint32_t length, std::string & out)
{
	for (size_t i = 0; i <length; ++i) 
	{ 
		static const char* HexConverter = "0123456789ABCDEF";   
		out += HexConverter[ ( string_in[i] & 0xF0 ) >> 4 ]; 
		out += HexConverter[ ( string_in[i] & 0x0F ) >> 0 ]; 
	} 
}

void Encryption::HexadecimalToChar(std::string string_in, uint32_t length, std::string & out)
{
	for (size_t i = 0; i <length; i +=2) 
	{ 
		char left = (string_in[i] >= 'A') ? string_in[i] - 'A' + 10 : string_in[i] - '0'; 
		char right = (string_in[i + 1] >= 'A') ? string_in[i + 1] - 'A' + 10 : string_in[i + 1] - '0'; 
		out += (left << 4) | (right << 0); 
	}
}
