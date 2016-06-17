// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: RemoveShape.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "RemoveShape.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace {

const ::google::protobuf::Descriptor* RemoveShape_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  RemoveShape_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_RemoveShape_2eproto() {
  protobuf_AddDesc_RemoveShape_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "RemoveShape.proto");
  GOOGLE_CHECK(file != NULL);
  RemoveShape_descriptor_ = file->message_type(0);
  static const int RemoveShape_offsets_[1] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RemoveShape, shapeid_),
  };
  RemoveShape_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      RemoveShape_descriptor_,
      RemoveShape::default_instance_,
      RemoveShape_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RemoveShape, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RemoveShape, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(RemoveShape));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_RemoveShape_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    RemoveShape_descriptor_, &RemoveShape::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_RemoveShape_2eproto() {
  delete RemoveShape::default_instance_;
  delete RemoveShape_reflection_;
}

void protobuf_AddDesc_RemoveShape_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\021RemoveShape.proto\"\036\n\013RemoveShape\022\017\n\007sh"
    "apeID\030\001 \001(\r", 51);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "RemoveShape.proto", &protobuf_RegisterTypes);
  RemoveShape::default_instance_ = new RemoveShape();
  RemoveShape::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_RemoveShape_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_RemoveShape_2eproto {
  StaticDescriptorInitializer_RemoveShape_2eproto() {
    protobuf_AddDesc_RemoveShape_2eproto();
  }
} static_descriptor_initializer_RemoveShape_2eproto_;

// ===================================================================

#ifndef _MSC_VER
const int RemoveShape::kShapeIDFieldNumber;
#endif  // !_MSC_VER

RemoveShape::RemoveShape()
  : ::google::protobuf::Message() {
  SharedCtor();
  // @@protoc_insertion_point(constructor:RemoveShape)
}

void RemoveShape::InitAsDefaultInstance() {
}

RemoveShape::RemoveShape(const RemoveShape& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:RemoveShape)
}

void RemoveShape::SharedCtor() {
  _cached_size_ = 0;
  shapeid_ = 0u;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

RemoveShape::~RemoveShape() {
  // @@protoc_insertion_point(destructor:RemoveShape)
  SharedDtor();
}

void RemoveShape::SharedDtor() {
  if (this != default_instance_) {
  }
}

void RemoveShape::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* RemoveShape::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return RemoveShape_descriptor_;
}

const RemoveShape& RemoveShape::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_RemoveShape_2eproto();
  return *default_instance_;
}

RemoveShape* RemoveShape::default_instance_ = NULL;

RemoveShape* RemoveShape::New() const {
  return new RemoveShape;
}

void RemoveShape::Clear() {
  shapeid_ = 0u;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool RemoveShape::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:RemoveShape)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional uint32 shapeID = 1;
      case 1: {
        if (tag == 8) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32>(
                 input, &shapeid_)));
          set_has_shapeid();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:RemoveShape)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:RemoveShape)
  return false;
#undef DO_
}

void RemoveShape::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:RemoveShape)
  // optional uint32 shapeID = 1;
  if (has_shapeid()) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32(1, this->shapeid(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:RemoveShape)
}

::google::protobuf::uint8* RemoveShape::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:RemoveShape)
  // optional uint32 shapeID = 1;
  if (has_shapeid()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt32ToArray(1, this->shapeid(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:RemoveShape)
  return target;
}

int RemoveShape::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional uint32 shapeID = 1;
    if (has_shapeid()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt32Size(
          this->shapeid());
    }

  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void RemoveShape::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const RemoveShape* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const RemoveShape*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void RemoveShape::MergeFrom(const RemoveShape& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_shapeid()) {
      set_shapeid(from.shapeid());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void RemoveShape::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void RemoveShape::CopyFrom(const RemoveShape& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool RemoveShape::IsInitialized() const {

  return true;
}

void RemoveShape::Swap(RemoveShape* other) {
  if (other != this) {
    std::swap(shapeid_, other->shapeid_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata RemoveShape::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = RemoveShape_descriptor_;
  metadata.reflection = RemoveShape_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

// @@protoc_insertion_point(global_scope)
