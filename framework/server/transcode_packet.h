/*
 * IStreamBuffer.h
 *
 *  Created on: Nov 8, 2013
 *      Author: vagrant
 */

#ifndef FRAMEWORK_SERVER_TRANSCODE_PACKET_H_
#define FRAMEWORK_SERVER_TRANSCODE_PACKET_H_

#include "transcode_endpoint.h"

#define BYTESIZE 8
#define OPCODELEN sizeof(uint32_t)
#define HEADERLEN (PACKETLEN + OPCODELEN)

class PacketTranscode : public EndpointTranscode {
public:
	PacketTranscode();
	explicit PacketTranscode(size_t opcode);
	explicit PacketTranscode(Transcode* kernel);
	virtual ~PacketTranscode();

	virtual void Reset();
	void SetOpcode(size_t opcode);
	size_t GetOpcode() const;
	BinaryData *GetBuffer() const;

	PacketTranscode& operator<<(PacketTranscode& value);
	PacketTranscode& operator>>(PacketTranscode& value);

	PacketTranscode& operator<<(const std::string& value);
	PacketTranscode& operator>>(std::string& value);
	PacketTranscode& operator<<(const char* value);
	PacketTranscode& operator>>(char* value);

	PacketTranscode& operator<<(const char value);
	PacketTranscode& operator>>(char& value);
	PacketTranscode& operator<<(const double value);
	PacketTranscode& operator>>(double& value);

	PacketTranscode& operator<<(const float value);
	PacketTranscode& operator>>(float& value);

	PacketTranscode& operator<<(const bool value);
	PacketTranscode& operator>>(bool& value);

    PacketTranscode& operator<<(const int8_t value);
	PacketTranscode& operator>>(int8_t& value);
	PacketTranscode& operator<<(const int16_t value);
	PacketTranscode& operator>>(int16_t& value);
	PacketTranscode& operator<<(const int32_t value);
	PacketTranscode& operator>>(int32_t& value);
	PacketTranscode& operator<<(const int64_t value);
	PacketTranscode& operator>>(int64_t& value);
	PacketTranscode& operator<<(const uint8_t value);
	PacketTranscode& operator>>(uint8_t& value);
	PacketTranscode& operator<<(const uint16_t value);
	PacketTranscode& operator>>(uint16_t& value);
	PacketTranscode& operator<<(const uint32_t value);
	PacketTranscode& operator>>(uint32_t& value);
	PacketTranscode& operator<<(const uint64_t value);
	PacketTranscode& operator>>(uint64_t& value);


	PacketTranscode& append(uint8_t* buffer, int32_t len);

#ifdef __APPLE__
    PacketTranscode& operator<<(const long value);
    PacketTranscode& operator>>(long& value);
    PacketTranscode& operator<<(const size_t value);
    PacketTranscode& operator>>(size_t& value);
#endif

protected:
	virtual BinaryData* Encode() const;
	virtual void Decode(const BinaryData* message);

	virtual BinaryData* doEndpointEncode() const;
	virtual void doEndpointDecode(const BinaryData* message);

protected:
	BinaryData* buf_;
	size_t offset_;

private:
	//(TODO:shshang) delete TranscodeCodeTest friendship
	friend class TranscodeTest;
	friend class GameLoopTest;
	friend class AccountModuleTest;
	friend class CaiShenTest;
	friend struct NotificationMessage;
	friend class GameServerService;
	friend class LuaHostEngine;
};

#endif  // FRAMEWORK_SERVER_TRANSCODE_PACKET_H_
