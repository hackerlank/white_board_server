/*
 * transcode.cpp
 *
 *  Created on: Nov 8, 2013
 *      Author: shshang
 */

#include "transcode.h"

#include "scope_guard.h"
#include "tcp_connection.h"

Transcode::Transcode() : Transcode(nullptr)
{
	is_good = true; 
}

Transcode::Transcode(Transcode* kernel) : kernel_(kernel)
{
	is_good = true; 
}

Transcode::~Transcode() {
	if (kernel_ != nullptr) {
		delete kernel_;
		kernel_ = nullptr;
	}
}

void Transcode::Write(BinaryData* &dest, const uint8_t* src, size_t len) const {
	if (len + dest->offset > dest->size) {
		if (dest->size == 0) dest->size = 1;
		dest->size *= FACTOR;
		while (dest->size < len + dest->offset) {
			dest->size *= FACTOR;
		}
		BinaryData* newDest = reinterpret_cast<BinaryData*>(realloc(dest, dest->size + sizeof(*dest)));
		if (newDest != nullptr) {
			dest = newDest;
		} else {
			LOG(INFO) << "In Transcode::Write, realloc failed, write nothing, please check memory is enough";
			memset(dest->data + dest->offset, 0, dest->size - dest->offset);
			return;
		}
	}
	memcpy(dest->data + dest->offset, src, len);
	dest->offset += len;
}

void Transcode::Read(uint8_t* dest, const BinaryData* src, size_t &offset, size_t len) 
{
	if (offset + len > src->offset) 
	{
		//LOG(ERROR) << " TcpConnection , Buffer " << (BinaryData*)src << " there is nothing enough to be read , offset " << offset << " , len " << len << " src->offset " << src->offset;
		memset(dest, 0, len);
		//modify by hdchen,保证后面的数据都是空值(0)
		offset = src->offset; 
		is_good = false; 
	} 
	else 
	{
		memcpy(dest, src->data + offset, len);
		offset += len;
	}
}


bool Transcode::IsDataEnough(BinaryData* message, size_t* actualSize,  TcpConnection* con){
	if (message == nullptr) 
	{
		return false;
	} 
	else if (message->offset < PACKETLEN) 
	{
		return false;
	}
	size_t offset = 0;
	Read(reinterpret_cast<uint8_t*>(actualSize), message, offset, PACKETLEN);
	if (*actualSize > message->offset) 
	{
		//LOG(WARNING) << " TcpConnection , Buffer " << message << ",  Receive Data is not Enough." << " Total:" << *actualSize << " Actual:" << message->offset << " this = "<< con<<" ip = "<< con->remote_address();
		return false;
	} 
	else 
	{
		return true;
	}
}

BinaryData* Transcode::Encode(const BinaryData* message) const {
	if (message == nullptr)	{
		LOG(ERROR) << "Encode message is null";
		return nullptr;
	}
	if (kernel_ != nullptr) {
		BinaryData* myEncodeRes = doEncode(message);
		BinaryData* encodeRes = kernel_->Encode(myEncodeRes);
		if (message != myEncodeRes) {
			free(myEncodeRes);
			myEncodeRes = nullptr;
		}
		return encodeRes;
	} else {
		return doEncode(message);
	}
}

BinaryData* Transcode::Decode(const BinaryData* message) {
	if (message == nullptr)	{
		LOG(ERROR) << "Decode message is null";
		return nullptr;
	}
	if (kernel_ != nullptr) {
		BinaryData* myDecodeRes = kernel_->Decode(message);
		if (myDecodeRes == nullptr) {
			return nullptr;
		}
		BinaryData* decodeRes = doDecode(myDecodeRes);
		if (myDecodeRes != decodeRes) {
			free(myDecodeRes);
			myDecodeRes = nullptr;
		}
		return decodeRes;
	} else {
		return doDecode(message);
	}
}
