/*
 * stream_buffer.cpp
 *
 *  Created on: Nov 8, 2013
 *      Author: vagrant
 */

#include "transcode_packet.h"

PacketTranscode::PacketTranscode() : PacketTranscode(nullptr) {
}

PacketTranscode::PacketTranscode(size_t opcode) : PacketTranscode(nullptr) {
	SetOpcode(opcode);
}

PacketTranscode::PacketTranscode(Transcode* kernel) : EndpointTranscode(kernel), buf_(nullptr), offset_(0)  {
	this->buf_ = reinterpret_cast<BinaryData*>(calloc(1, sizeof(BinaryData) + HEADERLEN));
	this->buf_->offset = HEADERLEN;
	this->buf_->size = HEADERLEN;
}


PacketTranscode::~PacketTranscode() {
	if (buf_ != nullptr) {
		free(buf_);
		buf_ = nullptr;
	}
}

void PacketTranscode::Reset() {
	memset(this->buf_->data, 0, this->buf_->size);
	this->buf_->offset = HEADERLEN;
	offset_ = HEADERLEN;
}

BinaryData* PacketTranscode::Encode() const {
	return Transcode::Encode (this->buf_);
}

void PacketTranscode::Decode(const BinaryData* message) {
	Transcode::Decode (message);
}

void PacketTranscode::SetOpcode(size_t opcode) {
	memcpy(this->buf_->data + PACKETLEN, &opcode, OPCODELEN);
}

size_t PacketTranscode::GetOpcode() const 
{
	size_t opcode = 0;
	if (this->buf_->size >= HEADERLEN)
	{
		memcpy(&opcode, this->buf_->data + PACKETLEN, OPCODELEN);
	}
	return opcode;
}

BinaryData* PacketTranscode::GetBuffer() const {
	return buf_;
}

PacketTranscode& PacketTranscode::append(uint8_t* buffer, int32_t len)
{
	Write(this->buf_, reinterpret_cast<const uint8_t*>(buffer), len);
	return *this;
}


BinaryData* PacketTranscode::doEndpointEncode() const {
	BinaryData* encodeRes = reinterpret_cast<BinaryData*>(calloc(1, this->buf_->offset + sizeof(*this->buf_)));
	encodeRes->offset = 0;
	encodeRes->size = this->buf_->offset;
	Write(encodeRes, reinterpret_cast<uint8_t*>(this->buf_->data), this->buf_->offset);
	size_t dataOffset = encodeRes->offset;
	encodeRes->offset = 0;
	Write(encodeRes, reinterpret_cast<uint8_t*>(&dataOffset), PACKETLEN);
	encodeRes->offset = dataOffset;
	return encodeRes;
}

void PacketTranscode::doEndpointDecode(const BinaryData* message) {
	size_t size = 0;
	offset_ = 0;
	Read(reinterpret_cast<uint8_t*>(&size), message, offset_, PACKETLEN);
	if (size > message->offset) {
		LOG(ERROR) << " TcpConnection BinaryData " << (BinaryData*)message << " In PacketTranscode::doEndpointDecode, message is not integrity ";
		return;
	} else {
		BinaryData* newBuf = reinterpret_cast<BinaryData*>(realloc(this->buf_, size * sizeof(uint8_t) + sizeof(*message)));
		if (newBuf != nullptr) {
			this->buf_ = newBuf;
			this->buf_->offset = 0;
			this->buf_->size = size;
			Write(this->buf_, reinterpret_cast<const uint8_t*>(message->data), size);
			offset_ = HEADERLEN;
		} else {
			LOG(INFO) << "In PacketTranscode::doEndpointDecode, realloc failed, nothing been decoded, please check memory is enough";
		}
	}
}

PacketTranscode& PacketTranscode::operator<<(PacketTranscode& value) {
	BinaryData* data = value.buf_;
	uint32_t len = data->offset - HEADERLEN;
	Write(this->buf_, reinterpret_cast<uint8_t*>(&len), sizeof(len));
	Write(this->buf_, reinterpret_cast<const uint8_t*>(data->data + HEADERLEN), len);
	return *this;
}

PacketTranscode& PacketTranscode::operator>>(PacketTranscode& value) {
	uint32_t len = 0;
	Read(reinterpret_cast<uint8_t*>(&len), this->buf_, offset_, sizeof(len));
	if (value.buf_->size < len) {
		BinaryData* newDest = reinterpret_cast<BinaryData*>(realloc(value.buf_, len + HEADERLEN + sizeof(*value.buf_)));
		if (newDest != nullptr) {
			value.buf_ = newDest;
		} else {
			LOG(INFO) << "In PacketTranscode::::operator>>(PacketTranscode& value), realloc failed, write nothing, please check memory is enough";
			memset(value.buf_, 0, value.buf_->size);
			return *this;
		}
	}
	Read(reinterpret_cast<uint8_t*>(value.buf_->data + HEADERLEN), this->buf_, offset_, len);
	value.offset_ = HEADERLEN;
	value.buf_->offset = len + HEADERLEN;
	value.buf_->size = value.buf_->offset;
	return *this;
}

PacketTranscode& PacketTranscode::operator<<(const std::string& value) {
	uint32_t len = value.size();
	Write(this->buf_, reinterpret_cast<uint8_t*>(&len), sizeof(len));
	Write(this->buf_, reinterpret_cast<const uint8_t*>(value.c_str()), len);
    return *this;
}

PacketTranscode& PacketTranscode::operator>>(std::string& value) {
	uint32_t len = 0;
	Read(reinterpret_cast<uint8_t*>(&len), this->buf_, offset_, sizeof(len));
	char* str = new char[len + 1];
	Read(reinterpret_cast<uint8_t*>(str), this->buf_, offset_, len);
	str[len] = '\0';
	value.append( str, len );
	delete []str;
    return *this;
}

PacketTranscode& PacketTranscode::operator<<(const char* value) {
	uint32_t len = strlen(value);
	Write(this->buf_, reinterpret_cast<uint8_t*>(&len), sizeof(len));
	Write(this->buf_, reinterpret_cast<const uint8_t*>(value), len);
    return *this;
}

/*
 * please make sure the value has enough space to contain the data
 */
PacketTranscode& PacketTranscode::operator>>(char* value) 
{
	uint32_t len = 0;
	Read(reinterpret_cast<uint8_t*>(&len), this->buf_, offset_, sizeof(len));
	//Read(reinterpret_cast<uint8_t*>(value), this->buf_, offset_, len);


	char* str = new char[len + 1]; 
    	Read(reinterpret_cast<uint8_t*>(str), this->buf_, offset_, len); 
    	str[len] = '\0'; 
	
	delete[] str;
	str = nullptr;

 	LOG(ERROR)<<"-----PacketTranscode::operator>>(char* value)------";


	value[0] = '\0';
    	return *this;
}

#undef DECLARE_STREAM_OPERATOR_IN
#define DECLARE_STREAM_OPERATOR_IN(TYPE)                                             \
	PacketTranscode& PacketTranscode::operator<<(const TYPE value) {                 \
    	Write(this->buf_, reinterpret_cast<const uint8_t*>(&value), sizeof(value));  \
    	return *this;                                                                \
}

DECLARE_STREAM_OPERATOR_IN( char )
DECLARE_STREAM_OPERATOR_IN( double )
DECLARE_STREAM_OPERATOR_IN(float)
DECLARE_STREAM_OPERATOR_IN( bool )
DECLARE_STREAM_OPERATOR_IN(int8_t)
DECLARE_STREAM_OPERATOR_IN(int16_t)
DECLARE_STREAM_OPERATOR_IN(int32_t)
DECLARE_STREAM_OPERATOR_IN(int64_t)
DECLARE_STREAM_OPERATOR_IN(uint8_t)
DECLARE_STREAM_OPERATOR_IN(uint16_t)
DECLARE_STREAM_OPERATOR_IN(uint32_t)
DECLARE_STREAM_OPERATOR_IN(uint64_t)


#undef DECLARE_STREAM_OPERATOR_OUT
#define DECLARE_STREAM_OPERATOR_OUT(TYPE)                                        \
	PacketTranscode& PacketTranscode::operator>>(TYPE &value) {                  \
		Read(reinterpret_cast<uint8_t*>(&value), this->buf_, offset_, sizeof(value));     \
		return *this;                                                            \
}

DECLARE_STREAM_OPERATOR_OUT( char )
DECLARE_STREAM_OPERATOR_OUT( double )
DECLARE_STREAM_OPERATOR_OUT(float)
DECLARE_STREAM_OPERATOR_OUT( bool )
DECLARE_STREAM_OPERATOR_OUT(int8_t)
DECLARE_STREAM_OPERATOR_OUT(int16_t)
DECLARE_STREAM_OPERATOR_OUT(int32_t)
DECLARE_STREAM_OPERATOR_OUT(int64_t)
DECLARE_STREAM_OPERATOR_OUT(uint8_t)
DECLARE_STREAM_OPERATOR_OUT(uint16_t)
DECLARE_STREAM_OPERATOR_OUT(uint32_t)
DECLARE_STREAM_OPERATOR_OUT(uint64_t)

#ifdef __APPLE__
DECLARE_STREAM_OPERATOR_IN( long )
DECLARE_STREAM_OPERATOR_OUT( long )
DECLARE_STREAM_OPERATOR_IN( size_t )
DECLARE_STREAM_OPERATOR_OUT( size_t )
#endif

#undef DECLARE_STREAM_OPERATOR_IN
#undef DECLARE_STREAM_OPERATOR_OUT

