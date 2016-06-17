#ifndef FRAMEWORK_SERVER_TRANSCODE_PROTOBUF_H_
#define FRAMEWORK_SERVER_TRANSCODE_PROTOBUF_H_

#include <string>
#include <cstdint>
#include "transcode_endpoint.h"
#include "transcode_packet.h"

class ProtobufTranscode : public EndpointTranscode
{
public:
    ProtobufTranscode() :
        opcode_(0)
    {
    }

    ProtobufTranscode(uint32_t opcode, std::string&& body);

    bool FromPacketTranscode(PacketTranscode& packet);

	virtual void Reset();

	virtual BinaryData* Encode() const;
	virtual void Decode(const BinaryData* message);

	uint32_t GetOpcode() const
    {
        return opcode_;
    }

	void SetOpcode(uint32_t opcode);

    void SetBody(std::string&& body)
    {
        body_ = body;
    }

    const std::string& GetBody() const
    {
        return body_;
    }
private:
	virtual BinaryData* doEndpointEncode() const;
	virtual void doEndpointDecode(const BinaryData* message);
private:
	uint32_t opcode_;
    std::string body_;
};

#endif
