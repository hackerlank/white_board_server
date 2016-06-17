#include "encrypt.h"
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <time.h>

typedef int (Encrypt::*HandleEncryptType)(void);
static HandleEncryptType _encrypt[ENCRYPT_ALGORITHM_COUNT] = {0};

Encrypt::Encrypt()
{
	static bool _encrypt_initialized = false;
	if (!_encrypt_initialized)
	{
		_encrypt_initialized =  true;
		_encrypt[BLOCK_LINK_ENCRYPT]				= &Encrypt::EncryptAlgorithmOne;
		_encrypt[BLOCK_MD5_XOR_ENCRYPT]				= &Encrypt::EncryptAlgorithmTwo;
		_encrypt[BLOCK_DES_MD5_XOR_ENCRYPT]			= &Encrypt::EncryptAlgorithmThree;
		_encrypt[BLOCK_AES_MD5_ENCRYPT]				= &Encrypt::EncryptAlgorithmThree;//�����ָõ��õ�����
		_encrypt[BLOCK_XOR_MD5_ENCRYPT]				= &Encrypt::EncryptAlgorithmFive;
		_encrypt[BLOCK_IDEA_XOR_ENCRYPT]			= &Encrypt::EncryptAlgorithmSix;
		_encrypt[BLOCK_BLOWFISH_MD5_XOR_ENCRYPT]	= &Encrypt::EncryptAlgorithmSeven;
		_encrypt[BLOCK_AES_XOR_ENCRYPT]				= &Encrypt::EncryptAlgorithmEight;
		_encrypt[BLOCK_MD5_ENCRYPT]					= &Encrypt::EncryptAlgorithmNight;
		_encrypt[BLCOK_SHA1_ENCRYPT]				= &Encrypt::EncryptAlgorithmTen;
	}
	
	InitPlainttext();
}

Encrypt::Encrypt(std::string plainttext_message)
{
	iv_ = "";
	mod_key_ = "";
	plainttext_ = "";
	ciphertext_ = "";
	static bool _encrypt_initialized = false;
	if (!_encrypt_initialized)
	{ 
		_encrypt_initialized =  true;
		_encrypt[BLOCK_LINK_ENCRYPT]				= &Encrypt::EncryptAlgorithmOne;
		_encrypt[BLOCK_MD5_XOR_ENCRYPT]				= &Encrypt::EncryptAlgorithmTwo;
		_encrypt[BLOCK_DES_MD5_XOR_ENCRYPT]			= &Encrypt::EncryptAlgorithmThree;
		_encrypt[BLOCK_AES_MD5_ENCRYPT]				= &Encrypt::EncryptAlgorithmThree;
		_encrypt[BLOCK_XOR_MD5_ENCRYPT]				= &Encrypt::EncryptAlgorithmFive;
		_encrypt[BLOCK_IDEA_XOR_ENCRYPT]			= &Encrypt::EncryptAlgorithmSix;
		_encrypt[BLOCK_BLOWFISH_MD5_XOR_ENCRYPT]	= &Encrypt::EncryptAlgorithmSeven;
		_encrypt[BLOCK_AES_XOR_ENCRYPT]				= &Encrypt::EncryptAlgorithmEight;
		_encrypt[BLOCK_MD5_ENCRYPT]					= &Encrypt::EncryptAlgorithmNight;
		_encrypt[BLCOK_SHA1_ENCRYPT]				= &Encrypt::EncryptAlgorithmTen;
	}
	InitPlainttext(plainttext_message);
}

Encrypt::~Encrypt()
{

}

void Encrypt::GenerateRandomString(const char* source_str, char* target_str, int target_len)
{
	assert(source_str);
	assert(target_str);
	assert(target_len);
	if( !(source_str && target_str && target_len > 0) )
	{
		return;
	}
	
	std::vector<char> vec_chars;

	for( size_t i = 0; i < strlen(source_str); i++)
	{
		vec_chars.push_back( source_str[i] );
	}

	if( vec_chars.size() <= 0)
	{
		return;
	}

	for( int i =0; i< target_len; i++)
	{
		int len = vec_chars.size();
		if(len == 0)
		{
			break;
		}
		int random_index =  rand() % len;
		target_str[i] = vec_chars[random_index];			
		vec_chars.erase( vec_chars.begin() + random_index );
	}
}

void Encrypt::InitPlainttext()
{
	char iv_temp[17] = {0};
	char mod_key_temp[17] = {0};
#ifdef WIN32	
	System::GenerateRandomString("0123456789abcdef", MOD_KEY_LENGTH, iv_temp);
	System::GenerateRandomString("abcdef0123456789", MOD_KEY_LENGTH, mod_key_temp);
#else
	GenerateRandomString("0123456789abcdef", iv_temp, sizeof(iv_temp)/sizeof(iv_temp[0]) );
	GenerateRandomString("abcdef0123456789", mod_key_temp, sizeof(mod_key_temp)/sizeof(mod_key_temp[0]) );
#endif
	iv_ = std::string(iv_temp);
	mod_key_ = std::string(mod_key_temp);

	char t[16] = {0};
	time_t time_now;
	time_now = time(NULL);
	sprintf(t, "%ld", time_now);
	timestamp_ = std::string(t);
	if (timestamp_.length() != TIMESTAMP_LENGTH)
	{
		timestamp_ = "0000000000";
	}
	iv_ += timestamp_;
	plainttext_ = std::string(iv_) + std::string(mod_key_);
}

void Encrypt::InitPlainttext(std::string &plainttext_message)
{	
	if (plainttext_message.length() != PLAINTTEXT_LENGTH)
	{
		return;
	}
	iv_ = plainttext_message.substr(0, IV_LENGTH);
	mod_key_ = plainttext_message.substr(IV_LENGTH, PLAINTTEXT_LENGTH - IV_LENGTH);
	plainttext_ = plainttext_message;
}

std::string Encrypt::plainttext()
{
	return plainttext_;
}

std::string Encrypt::ciphertext()
{
	return ciphertext_;
}

int Encrypt::EncryptMessage(enum EncryptType algorithm_type)
{
	if (algorithm_type < 0 || algorithm_type > ENCRYPT_ALGORITHM_COUNT)
	{
		return -1;
	}
	return (this->*_encrypt[algorithm_type])();
}

int Encrypt::EncryptMessage(int32_t& num)
{
	num = GetEncryptAlgorithmIndex();
	if (num < 1 || num > 10)
	{
		/*NLOG("Encrypt::EncryptMessage num is wrong,num is %d", num);*/
		return -1;
	}
	return (this->*_encrypt[num])();
}


int Encrypt::EncryptAlgorithmOne()
{
	uint8_t part1[32] = {0};
	uint8_t part2[64] = {0};
	uint32_t md5_length = MD5_LENGTH * 2;
	uint8_t * xor_key = (uint8_t *)mod_key_.c_str();
	uint8_t * input = (uint8_t *)iv_.c_str();
	for (uint32_t i=0; i<iv_.length(); i++)
	{ 
		EncryptHelper::XOR(input+i, xor_key, N_LENGTH, &part1[i]);
		xor_key = &part1[i];
	}
	if (! EncryptHelper::MD5GenerationHelper((uint8_t *)mod_key_.c_str(), mod_key_.length(), part2, md5_length) )
	{
		return -1;
	}	
//	std::string((char*)part1); 
	ciphertext_ = std::string((char*)part1) + std::string((char*)part2);
	return 0;
}

int Encrypt::EncryptAlgorithmTwo()
{
	uint8_t part1[64] = {0};
	std::string in_put;
	uint8_t part2[64] = {0};
	uint32_t md5_length = MD5_LENGTH * 2;
	/*for (int32 i = 0; i<8; i++)
	{
		temp[i] = (uint8_t)iv_[i];
	}*/
	if (!EncryptHelper::MD5GenerationHelper((uint8_t *)iv_.c_str(), N_LENGTH, part1, md5_length))
	{
		return -1;
	}
	in_put = plainttext_.substr(N_LENGTH, plainttext_.length() - N_LENGTH);
	EncryptHelper::XORForCircle((uint8_t *)in_put.c_str(), in_put.length(), part1, md5_length, part2);
	//	std::string((char*)part1); 
	ciphertext_ = std::string((char*)part2);
	return 0;
}

int Encrypt::EncryptAlgorithmThree()
{
	std::string des_key = plainttext_.substr(0, 7);
	uint8_t part1[32] = {0};
	uint8_t part2[64] = {0};
	uint8_t part3[32] = {0};
	uint32_t md5_length = MD5_LENGTH * 2;
	uint32_t out_put_lennth = 32;
	if (!EncryptHelper::DESEncryptionHelper((uint8_t *)des_key.c_str(), (uint8_t *)mod_key_.c_str(), mod_key_.length(), part1, out_put_lennth))
	{
		return -1;
	}
	if (!EncryptHelper::MD5GenerationHelper((uint8_t *)iv_.c_str(), N_LENGTH, part2, md5_length))
	{
		return -1;
	}
	std::string iv_other = iv_.substr(1, iv_.length() - 1);
	EncryptHelper::XORForCircle((uint8_t *)mod_key_.c_str(), mod_key_.length(), (uint8_t *)iv_other.c_str(), iv_other.length(), part3);
	ciphertext_ = std::string((char*)part1) +std::string((char*)part2) +std::string((char*)part3);
	return 0;
}

int Encrypt::EncryptAlgorithmFour()
{
	//uint8_t part1[64] = {0};
	//uint8_t part2[64] = {0};
	//uint32_t output_length = 64;
	//uint32_t md5_length = MD5_LENGTH * 2;
	//if (!EncryptHelper::BlowFishEncryptionHelper((uint8_t *)mod_key_.c_str(), (uint8_t *)iv_.c_str(), iv_.length(), part1, output_length))  

	//{ 
	//	return -1; 
	//}
	///*if (!EncryptHelper::MD5GenerationHelper((uint8_t *)mod_key_.c_str(), mod_key_.length(), part2, md5_length))
	//{
	//	return -1;
	//}*/
	///*ciphertext_ = std::string((char*)part1, output_length); *//*+std::string((char*)part2, md5_length);*/
	//ciphertext_ = plainttext_;
	return 0;
}

int Encrypt::EncryptAlgorithmFive()
{
	uint8_t part1[32] = {0};

	uint8_t part2[64] = {0};
	uint32_t md5_length = MD5_LENGTH * 2;
	EncryptHelper::XOR((uint8_t *)iv_.c_str(), (uint8_t *)mod_key_.c_str(), mod_key_.length(), part1);
	if (!EncryptHelper::MD5GenerationHelper((uint8_t *)mod_key_.c_str(), mod_key_.length(), part2, md5_length))
	{
		return -1;
	}
	ciphertext_ = std::string((char*)part1) +std::string((char*)part2);
	return 0;
}

int Encrypt::EncryptAlgorithmSix()
{
	std::string des_key = plainttext_.substr(0, 7);
	uint8_t part1[32] = {0}; 
	uint8_t part2[32] = {0}; 
	uint32_t output_length = 32; 

	if (!EncryptHelper::DESEncryptionHelper((uint8_t *)des_key.c_str(), (uint8_t *)mod_key_.c_str(), mod_key_.length(), part1, output_length))
	{ 
		return -1; 
	} 
	EncryptHelper::XOR((uint8_t *)part1, (uint8_t *)iv_.c_str(), iv_.length(), part2); 
	ciphertext_ = std::string((char*)part2); 

	return 0;
}

int Encrypt::EncryptAlgorithmSeven()
{
	uint8_t part1[32] = {0}; 
	uint8_t part2[32] = {0}; 
	uint8_t part3[32] = {0}; 
	uint32_t output_length = 32; 
	uint32_t md5_length = MD5_LENGTH; 
	if (!EncryptHelper::BlowFishEncryptionHelper((uint8_t *)mod_key_.c_str(), (uint8_t *)iv_.c_str(), iv_.length(), part1, output_length))  

	{ 
		return -1; 
	} 
	if (!EncryptHelper::MD5GenerationHelper((uint8_t *)mod_key_.c_str(), mod_key_.length(), part2, md5_length)) 
	{ 
		return -1; 
	} 
	EncryptHelper::XOR((uint8_t *)part2, (uint8_t *)mod_key_.c_str(), mod_key_.length(), part3); 
	ciphertext_ = std::string((char*)part3); 

	return 0; 
}

int Encrypt::EncryptAlgorithmEight()
{

	uint8_t aes_result[64] = {0};
	uint32_t aes_result_len = 64;

	EncryptHelper::AESEncryptionHelper((uint8_t*)(iv_.c_str()), (uint8_t*)(mod_key_.c_str()), mod_key_.size(), aes_result, aes_result_len);
	
	uint8_t output[64] = {0};
	if (aes_result_len > iv_.size())
	{
		EncryptHelper::XORForCircle(aes_result, aes_result_len, (uint8_t*)(iv_.c_str()), iv_.size(), output);
		ciphertext_ = std::string((char*)output, aes_result_len);
	}
	else
	{
		EncryptHelper::XORForCircle((uint8_t*)(iv_.c_str()), iv_.size(), aes_result, aes_result_len, output);
		ciphertext_ = std::string((char*)output, iv_.size());
	}

	return 0;
}

int Encrypt::EncryptAlgorithmNight()
{

	uint8_t output1[64] = {0};
	uint32_t output_len = 64;
	EncryptHelper::MD5GenerationHelper((uint8_t*)(iv_.c_str()), iv_.size(), output1, output_len);

	uint8_t output2[64] = {0};
	uint32_t output_len2 = 64;
	EncryptHelper::MD5GenerationHelper((uint8_t*)(mod_key_.c_str()), mod_key_.size(), output2, output_len2);

	uint8_t output[128] = {0};
	memcpy(output, output1, output_len);
	memcpy(output + output_len, output2, output_len2);

	uint8_t final_output[64] = {0};
	uint32_t final_output_len = 64;
	EncryptHelper::MD5GenerationHelper(output, output_len + output_len2, final_output, final_output_len);

	ciphertext_ = std::string((char*)final_output, final_output_len);
	return 0;
}

int Encrypt::EncryptAlgorithmTen()
{

	uint8_t output[64] = {0};
	uint32_t output_len = 64;

	std::string plain_message = iv_ + mod_key_;
	EncryptHelper::SHA1GenerationHelper((uint8_t*)(plain_message.c_str()), plain_message.size(), output, output_len);

	ciphertext_ = std::string((char*)output, output_len);
	return 0;
}

int Encrypt::GetEncryptAlgorithmIndex() 
{ 
	if (plainttext_.empty()) 
	{ 
		return -1; 
	}   

	int lastNum = 1; 
	uint32_t aimIndex = 1; 
	int result = 0; 
	while (aimIndex < plainttext_.length()) 
	{ 
		result += plainttext_[aimIndex]; 
		int temp = aimIndex; 
		aimIndex += lastNum; 
		lastNum = temp; 
	} 

	return ((result % 10) + 1 ); 
}

