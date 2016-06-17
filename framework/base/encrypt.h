#include "encrypthelper.h"
#include <string>
#include <vector>

using namespace std;

#define TIMESTAMP_LENGTH 10
#define IV_LENGTH 26
#define MOD_KEY_LENGTH 16
#define PLAINTTEXT_LENGTH 42
#define MD5_LENGTH 32
#define N_LENGTH 1

enum EncryptType
{
	BLOCK_LINK_ENCRYPT = 1,
	BLOCK_MD5_XOR_ENCRYPT,
	BLOCK_DES_MD5_XOR_ENCRYPT,
	BLOCK_AES_MD5_ENCRYPT,
	BLOCK_XOR_MD5_ENCRYPT,
	BLOCK_IDEA_XOR_ENCRYPT,
	BLOCK_BLOWFISH_MD5_XOR_ENCRYPT,
	BLOCK_AES_XOR_ENCRYPT,
	BLOCK_MD5_ENCRYPT,
	BLCOK_SHA1_ENCRYPT,
	ENCRYPT_ALGORITHM_COUNT
};

class Encrypt
{
private:
	std::string iv_;
	std::string timestamp_;
	std::string mod_key_;
	std::string plainttext_;
	std::string	ciphertext_;
public:
	Encrypt();
	Encrypt(std::string plainttext_message);
	~Encrypt();
	void InitPlainttext();
	void InitPlainttext(std::string& plainttext_message);
	void GenerateRandomString( const char* source_str, char* target_str, int target_len );
	std::string plainttext();
	std::string ciphertext();
	int EncryptMessage(enum EncryptType algorithm_type);
	int EncryptMessage(int32_t& num);
protected:
	int EncryptAlgorithmOne();
	int EncryptAlgorithmTwo();
	int EncryptAlgorithmThree();
	int EncryptAlgorithmFour();
	int EncryptAlgorithmFive();
	int EncryptAlgorithmSix();
	int EncryptAlgorithmSeven();
	int EncryptAlgorithmEight();
	int EncryptAlgorithmNight();
	int EncryptAlgorithmTen();
	int GetEncryptAlgorithmIndex();
};
