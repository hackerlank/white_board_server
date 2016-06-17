#include "transcode_protobuf.h"
#include <cassert>

static const int PACKET_LEN = sizeof(uint32_t);
static const int OPCODE_LEN = sizeof(uint32_t);
static const int HEADER_LEN = (PACKET_LEN + OPCODE_LEN);
static const int STRING_LEN = sizeof(uint32_t);

ProtobufTranscode::ProtobufTranscode(uint32_t opcode, std::string&& body) :
    opcode_(opcode),
    body_(body)
{

}

bool ProtobufTranscode::FromPacketTranscode(PacketTranscode& packet)
{

    opcode_ = packet.GetOpcode();
    packet >> body_;
    LOG(DEBUG) << "Get ProtobufPacket from PacketTranscode success, opcode: " << opcode_ << ", body size: " << body_.size() ;
    return true;
}

void ProtobufTranscode::Reset()
{
    opcode_ = 0;
    body_ = "";
}

void ProtobufTranscode::SetOpcode(uint32_t opcode)
{
    opcode_ = opcode;
}

BinaryData* ProtobufTranscode::Encode() const
{
	return doEndpointEncode();
}

void ProtobufTranscode::Decode(const BinaryData* message)
{
	Transcode::Decode(message);
}

BinaryData* ProtobufTranscode::doEndpointEncode() const
{
	BinaryData* encodeRes = reinterpret_cast<BinaryData*>(calloc(1, sizeof(BinaryData) + HEADER_LEN + STRING_LEN + this->body_.size()));   //protobuf序列化出的buffer长度占2字节
	encodeRes->offset = 0;
	encodeRes->size = HEADER_LEN + body_.size() + STRING_LEN;
	size_t dataOffset = HEADER_LEN + body_.size() + STRING_LEN;
	Write(encodeRes, reinterpret_cast<uint8_t*>(&dataOffset), PACKET_LEN);
    uint32_t opcode = opcode_; 
	Write(encodeRes, reinterpret_cast<uint8_t*>(&opcode), OPCODE_LEN);
	uint32_t body_len = this->body_.size();
	Write(encodeRes, reinterpret_cast<uint8_t*>(&body_len), sizeof(body_len));
	Write(encodeRes, reinterpret_cast<const uint8_t*>(this->body_.c_str()), this->body_.size());
    LOG(DEBUG) << "protobuf encode result, opcode: " << opcode << ", packet size: " << dataOffset << ", protobuf size: " << body_len;
	return encodeRes;
}

void ProtobufTranscode::doEndpointDecode(const BinaryData* message)
{
	uint32_t size = 0;
	size_t offset = 0;
	Read(reinterpret_cast<uint8_t*>(&size), message, offset, PACKET_LEN);
	if (size < HEADER_LEN || size > message->offset) {
		LOG(ERROR) << " TcpConnection BinaryData " << (BinaryData*)message << " In ProtobufTranscode::doEndpointDecode, message is not integrity";
        LOG(DEBUG) << "size: " << size << ", offset: " << message->offset << ", binary size: " << message->size;
		return;
    } else {
		uint32_t opcode = 0;
        memcpy(&opcode, message->data + PACKET_LEN, OPCODE_LEN);
        opcode_ = opcode;   //客户端框架层为了兼容旧协议，protobuf方式时opcode最高位需为1
		body_ = std::string(reinterpret_cast<const char*>(message->data) + message->offset + HEADER_LEN + STRING_LEN, size - HEADER_LEN - STRING_LEN);
        LOG(DEBUG) << "decode success, opcode: " << opcode_ << ", body size: " << body_.size() << ", original opcode: " << opcode;
    }
}
