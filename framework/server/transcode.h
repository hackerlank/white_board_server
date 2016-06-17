/*
 * transcode.h
 *
 *  Created on: Nov 8, 2013
 *      Author: shshang
 */

#ifndef FRAMEWORK_SERVER_TRANSCODE_H_
#define FRAMEWORK_SERVER_TRANSCODE_H_

#include <stdint.h>
#include <stdlib.h>

#include <cinttypes>
#include <cstring>
#include <vector>

#include "logging.h"

#define FACTOR 2
#define PACKETLEN sizeof(uint32_t)

class TcpConnection;

typedef struct stBinaryData {
	size_t size;
	size_t offset;
	uint8_t data[0];
}BinaryData;

class Transcode {

public:
	Transcode();
	explicit Transcode(Transcode* kernel);
	virtual ~Transcode();

	virtual  bool IsGood()
	{
		return is_good; 
	}

protected:
	BinaryData* Encode(const BinaryData* message) const;
	BinaryData* Decode(const BinaryData* message);

	bool IsDataEnough(BinaryData* message, size_t* actualSize, TcpConnection* con);

	void Write(BinaryData* &dest, const uint8_t* src, size_t len) const;
	void Read(uint8_t* dest, const BinaryData* src, size_t &offset, size_t len);

	virtual BinaryData* doEncode(const BinaryData* message) const = 0;
	virtual BinaryData* doDecode(const BinaryData* message) = 0;



protected:
	Transcode* kernel_;
	bool       is_good; 

private:
	friend class TcpConnection;
	//(TODO:shshang) delete TranscodeCodeTest friendship
	friend class TranscodeTest;
};

#endif  // FRAMEWORK_SERVER_TRANSCODE_H_
