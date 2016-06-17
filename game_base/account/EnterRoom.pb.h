// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: EnterRoom.proto

#ifndef PROTOBUF_EnterRoom_2eproto__INCLUDED
#define PROTOBUF_EnterRoom_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2006000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2006000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_EnterRoom_2eproto();
void protobuf_AssignDesc_EnterRoom_2eproto();
void protobuf_ShutdownFile_EnterRoom_2eproto();

class EnterRoom;

// ===================================================================

class EnterRoom : public ::google::protobuf::Message {
 public:
  EnterRoom();
  virtual ~EnterRoom();

  EnterRoom(const EnterRoom& from);

  inline EnterRoom& operator=(const EnterRoom& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const EnterRoom& default_instance();

  void Swap(EnterRoom* other);

  // implements Message ----------------------------------------------

  EnterRoom* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const EnterRoom& from);
  void MergeFrom(const EnterRoom& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional string room_key = 1;
  inline bool has_room_key() const;
  inline void clear_room_key();
  static const int kRoomKeyFieldNumber = 1;
  inline const ::std::string& room_key() const;
  inline void set_room_key(const ::std::string& value);
  inline void set_room_key(const char* value);
  inline void set_room_key(const char* value, size_t size);
  inline ::std::string* mutable_room_key();
  inline ::std::string* release_room_key();
  inline void set_allocated_room_key(::std::string* room_key);

  // @@protoc_insertion_point(class_scope:EnterRoom)
 private:
  inline void set_has_room_key();
  inline void clear_has_room_key();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::std::string* room_key_;
  friend void  protobuf_AddDesc_EnterRoom_2eproto();
  friend void protobuf_AssignDesc_EnterRoom_2eproto();
  friend void protobuf_ShutdownFile_EnterRoom_2eproto();

  void InitAsDefaultInstance();
  static EnterRoom* default_instance_;
};
// ===================================================================


// ===================================================================

// EnterRoom

// optional string room_key = 1;
inline bool EnterRoom::has_room_key() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void EnterRoom::set_has_room_key() {
  _has_bits_[0] |= 0x00000001u;
}
inline void EnterRoom::clear_has_room_key() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void EnterRoom::clear_room_key() {
  if (room_key_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    room_key_->clear();
  }
  clear_has_room_key();
}
inline const ::std::string& EnterRoom::room_key() const {
  // @@protoc_insertion_point(field_get:EnterRoom.room_key)
  return *room_key_;
}
inline void EnterRoom::set_room_key(const ::std::string& value) {
  set_has_room_key();
  if (room_key_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    room_key_ = new ::std::string;
  }
  room_key_->assign(value);
  // @@protoc_insertion_point(field_set:EnterRoom.room_key)
}
inline void EnterRoom::set_room_key(const char* value) {
  set_has_room_key();
  if (room_key_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    room_key_ = new ::std::string;
  }
  room_key_->assign(value);
  // @@protoc_insertion_point(field_set_char:EnterRoom.room_key)
}
inline void EnterRoom::set_room_key(const char* value, size_t size) {
  set_has_room_key();
  if (room_key_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    room_key_ = new ::std::string;
  }
  room_key_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:EnterRoom.room_key)
}
inline ::std::string* EnterRoom::mutable_room_key() {
  set_has_room_key();
  if (room_key_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    room_key_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:EnterRoom.room_key)
  return room_key_;
}
inline ::std::string* EnterRoom::release_room_key() {
  clear_has_room_key();
  if (room_key_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = room_key_;
    room_key_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void EnterRoom::set_allocated_room_key(::std::string* room_key) {
  if (room_key_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete room_key_;
  }
  if (room_key) {
    set_has_room_key();
    room_key_ = room_key;
  } else {
    clear_has_room_key();
    room_key_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:EnterRoom.room_key)
}


// @@protoc_insertion_point(namespace_scope)

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_EnterRoom_2eproto__INCLUDED