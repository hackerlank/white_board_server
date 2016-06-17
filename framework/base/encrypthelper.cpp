#include "encrypthelper.h"
#include <crypto++/des.h>
#include <crypto++/aes.h>
#include <crypto++/blowfish.h>
#include <crypto++/rc6.h>
#include <crypto++/idea.h>
#include <crypto++/md5.h>
#include <crypto++/sha.h>

using namespace CryptoPP;

void EncryptHelper::XOR(uint8_t* r, uint8_t* l, int32_t count, uint8_t* out)
{
	for (int n = 0; n < count; ++n)
	{
		*(out + n) = *(r + n) ^ *(l + n);
	}
}

void EncryptHelper::XORForCircle(uint8_t* r, int32_t count_r, uint8_t* l, int32_t count_l, uint8_t* out)
{
	if (count_r < count_l)
	{
		return ;
	}

	int n = 0, m = 0;
	for (; n < count_r; ++n)
	{
		*(out + n) = *(r + n) ^ *(l + m);
		++m;
		if (m >= count_l)
		{
			m = 0;
		}
	}
}

bool EncryptHelper::DESEncryptionHelper(uint8_t* key, uint8_t* input, uint32_t input_len, uint8_t* output, uint32_t& output_len)
{
	if (!key || !input || !output)
	{
		return false;
	}
	BlockEncryptionHelper<DES>(key, input, input_len, output, output_len);
	return true;
}

bool EncryptHelper::DESDecryptionHelper(uint8_t* key, uint8_t* input, uint32_t input_len, uint8_t* output, uint32_t& output_len)
{
	if (!key || !input || !output)
	{
		return false;
	}
	BlockDecryptionHelper<DES>(key, input, input_len, output, output_len);
	return true;
}

bool EncryptHelper::AESEncryptionHelper(uint8_t* key, uint8_t* input, uint32_t input_len, uint8_t* output, uint32_t& output_len)
{
	if (!key || !input || !output)
	{
		return false;
	}
	BlockEncryptionHelper<AES>(key, input, input_len, output, output_len);

	return true;
}

bool EncryptHelper::AESDecrtptionHelper(uint8_t* key, uint8_t* input, uint32_t input_len, uint8_t* output, uint32_t& output_len)
{
	if (!key || !input || !output)
	{
		return false;
	}
	BlockDecryptionHelper<AES>(key, input, input_len, output, output_len);
	

	return true;
}

bool EncryptHelper::BlowFishEncryptionHelper(uint8_t* key, uint8_t* input, uint32_t input_len, uint8_t* output, uint32_t& output_len)
{
	if (!key || !input || !output)
	{
		return false;
	}
	BlockEncryptionHelper<Blowfish>(key, input, input_len, output, output_len);
	
	return true;
}

bool EncryptHelper::BlowFishDecryptionHelper(uint8_t* key, uint8_t* input, uint32_t input_len, uint8_t* output, uint32_t& output_len)
{
	if (!key || !input || !output)
	{
		return false;
	}
	BlockDecryptionHelper<Blowfish>(key, input, input_len, output, output_len);
	return true;
}

bool EncryptHelper::MD5GenerationHelper(uint8_t* input, uint32_t input_len, uint8_t* output, uint32_t& output_len)
{
	if (!input || !output)
	{
		return false;
	}
	MD5 md5;
	md5.Update(input, input_len);
	md5.Final(output);
	output_len = md5.DigestSize() ;
	return true;
}

bool EncryptHelper::SHA1GenerationHelper(uint8_t* input, uint32_t input_len, uint8_t* output, uint32_t& output_len)
{
	if (!input || !output)
	{
		return false;
	}
	SHA1 sha1;
	sha1.Update(input, input_len);
	sha1.Final(output);
	output_len = sha1.DigestSize();
	return true;
}

bool EncryptHelper::RC6EncryptionHelper(uint8_t* key, uint8_t* input, uint32_t input_len, uint8_t* output, uint32_t& output_len)
{
	if (!key || !input || !output)
	{
		return false;
	}
	BlockEncryptionHelper<RC6>(key, input, input_len, output, output_len);

	return true;
}

bool EncryptHelper::RC6DecryptionHelper(uint8_t* key, uint8_t* input, uint32_t input_len, uint8_t* output, uint32_t& output_len)
{
	if (!key || !input || !output)
	{
		return false;
	}
	BlockDecryptionHelper<RC6>(key, input, input_len, output, output_len);

	return true;
}

bool EncryptHelper::IDEAEncryptionHelper(uint8_t* key, uint8_t* input, uint32_t input_len, uint8_t* output, uint32_t& output_len)
{
	if (!key || !input || !output)
	{
		return false;
	}
	BlockEncryptionHelper<IDEA>(key, input, input_len, output, output_len);
	return true;
}

bool EncryptHelper::IDEADecryptionHelper(uint8_t* key, uint8_t* input, uint32_t input_len, uint8_t* output, uint32_t& output_len)
{
	if (!key || !input || !output)
	{
		return false;
	}
	BlockDecryptionHelper<IDEA>(key, input, input_len, output, output_len);
	return true;
}

template<class Cipher> 
void 	BlockEncryptionHelper(uint8_t* key, uint8_t* input, uint32_t input_len, uint8_t* output, uint32_t& output_len)
{
	typename Cipher::Encryption encryptor;
	encryptor.SetKey(key, Cipher::DEFAULT_KEYLENGTH);
	uint32_t len_temp = input_len;
	uint32_t out_temp_len = 0;
	uint8_t* input_temp = input;
	uint8_t* output_temp = output;

	while (len_temp > 0)
	{
		int next_cipher_size = (len_temp < (uint32_t)Cipher::BLOCKSIZE) ? len_temp : (uint32_t)Cipher::BLOCKSIZE;
		if (output_len < (uint32_t)Cipher::BLOCKSIZE)
		{
			break;
		}
		encryptor.ProcessAndXorBlock( input_temp, NULL, output_temp);
		input_temp += Cipher::BLOCKSIZE;

		out_temp_len += Cipher::BLOCKSIZE;
		output_temp += Cipher::BLOCKSIZE;

		output_len -= Cipher::BLOCKSIZE;
		len_temp -= next_cipher_size;

	}
	output_len = out_temp_len;
}

template<class Cipher>
void	BlockDecryptionHelper(uint8_t* key, uint8_t* input, uint32_t input_len, uint8_t* output, uint32_t& output_len)
{
	typename Cipher::Decryption decryptor;
	decryptor.SetKey(key, Cipher::DEFAULT_KEYLENGTH);
	uint32_t len_temp = input_len;
	uint8_t* input_temp = input;
	uint8_t* output_temp = output;
	uint32_t out_temp_len = 0;

	while (len_temp > 0)
	{
		int next_cipher_size = (len_temp < (uint32_t)Cipher::BLOCKSIZE) ? len_temp : (uint32_t)Cipher::BLOCKSIZE;
		if (output_len < (uint32_t)Cipher::BLOCKSIZE)
		{
			break;
		}
		decryptor.ProcessAndXorBlock( input_temp, NULL, output_temp);
		input_temp += Cipher::BLOCKSIZE;

		out_temp_len += Cipher::BLOCKSIZE;
		output_temp += Cipher::BLOCKSIZE;

		output_len -= Cipher::BLOCKSIZE;
		len_temp -= next_cipher_size;

	}
	output_len = out_temp_len;
}
