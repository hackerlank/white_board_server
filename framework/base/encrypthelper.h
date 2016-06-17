#ifndef _SOURCE_GAMEBASE_ENCRYPTHELPER_H_
#define _SOURCE_GAMEBASE_ENCRYPTHELPER_H_
#include <stdio.h>
#include <stdint.h>


class EncryptHelper
{
public:
	static void XOR(uint8_t* r, uint8_t* l, int32_t count, uint8_t* out);
	static void XORForCircle(uint8_t* r, int32_t count_r, uint8_t* l, int32_t count_l, uint8_t* out);

	static bool DESEncryptionHelper(uint8_t* key, uint8_t* input, uint32_t input_len, uint8_t* output, uint32_t& output_len);
	static bool DESDecryptionHelper(uint8_t* key, uint8_t*input, uint32_t input_len, uint8_t* output, uint32_t& output_len);

	static bool AESEncryptionHelper(uint8_t* key, uint8_t* input, uint32_t input_len, uint8_t* output, uint32_t& output_len);
	static bool AESDecrtptionHelper(uint8_t* key, uint8_t* input, uint32_t input_len, uint8_t* output, uint32_t& output_len);

	static bool BlowFishEncryptionHelper(uint8_t* key, uint8_t* input, uint32_t input_len, uint8_t* output, uint32_t& output_len);
	static bool BlowFishDecryptionHelper(uint8_t* key, uint8_t* input, uint32_t input_len, uint8_t* output, uint32_t& output_len);

	static bool MD5GenerationHelper(uint8_t* input, uint32_t input_len, uint8_t* output, uint32_t& output_len);
	static bool SHA1GenerationHelper(uint8_t* input, uint32_t input_len, uint8_t* output, uint32_t& output_len);

	static bool RC6EncryptionHelper(uint8_t* key, uint8_t* input, uint32_t input_len, uint8_t* output, uint32_t& output_len);
	static bool RC6DecryptionHelper(uint8_t* key, uint8_t* input, uint32_t input_len, uint8_t* output, uint32_t& output_len);

	static bool IDEAEncryptionHelper(uint8_t* key, uint8_t* input, uint32_t input_len, uint8_t* output, uint32_t& output_len);
	static bool IDEADecryptionHelper(uint8_t* key, uint8_t* input, uint32_t input_len, uint8_t* output, uint32_t& output_len);

protected:
private:
};

template<class Cipher> void BlockEncryptionHelper(uint8_t* key, uint8_t* input, uint32_t input_len, uint8_t* output, uint32_t& output_len);
template<class Cipher> void BlockDecryptionHelper(uint8_t* key, uint8_t* input, uint32_t input_len, uint8_t* output, uint32_t& output_len);

#endif
