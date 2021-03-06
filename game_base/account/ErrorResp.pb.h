// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: ErrorResp.proto

#ifndef PROTOBUF_ErrorResp_2eproto__INCLUDED
#define PROTOBUF_ErrorResp_2eproto__INCLUDED

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
void  protobuf_AddDesc_ErrorResp_2eproto();
void protobuf_AssignDesc_ErrorResp_2eproto();
void protobuf_ShutdownFile_ErrorResp_2eproto();

class ErrorResp;

// ===================================================================

class ErrorResp : public ::google::protobuf::Message {
 public:
  ErrorResp();
  virtual ~ErrorResp();

  ErrorResp(const ErrorResp& from);

  inline ErrorResp& operator=(const ErrorResp& from) {
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
  static const ErrorResp& default_instance();

  void Swap(ErrorResp* other);

  // implements Message ----------------------------------------------

  ErrorResp* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const ErrorResp& from);
  void MergeFrom(const ErrorResp& from);
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

  // optional int32 errorCode = 1;
  inline bool has_errorcode() const;
  inline void clear_errorcode();
  static const int kErrorCodeFieldNumber = 1;
  inline ::google::protobuf::int32 errorcode() const;
  inline void set_errorcode(::google::protobuf::int32 value);

  // optional uint32 ExtraInfo = 2;
  inline bool has_extrainfo() const;
  inline void clear_extrainfo();
  static const int kExtraInfoFieldNumber = 2;
  inline ::google::protobuf::uint32 extrainfo() const;
  inline void set_extrainfo(::google::protobuf::uint32 value);

  // optional string errorDescription = 3;
  inline bool has_errordescription() const;
  inline void clear_errordescription();
  static const int kErrorDescriptionFieldNumber = 3;
  inline const ::std::string& errordescription() const;
  inline void set_errordescription(const ::std::string& value);
  inline void set_errordescription(const char* value);
  inline void set_errordescription(const char* value, size_t size);
  inline ::std::string* mutable_errordescription();
  inline ::std::string* release_errordescription();
  inline void set_allocated_errordescription(::std::string* errordescription);

  // @@protoc_insertion_point(class_scope:ErrorResp)
 private:
  inline void set_has_errorcode();
  inline void clear_has_errorcode();
  inline void set_has_extrainfo();
  inline void clear_has_extrainfo();
  inline void set_has_errordescription();
  inline void clear_has_errordescription();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::google::protobuf::int32 errorcode_;
  ::google::protobuf::uint32 extrainfo_;
  ::std::string* errordescription_;
  friend void  protobuf_AddDesc_ErrorResp_2eproto();
  friend void protobuf_AssignDesc_ErrorResp_2eproto();
  friend void protobuf_ShutdownFile_ErrorResp_2eproto();

  void InitAsDefaultInstance();
  static ErrorResp* default_instance_;
};
// ===================================================================


// ===================================================================

// ErrorResp

// optional int32 errorCode = 1;
inline bool ErrorResp::has_errorcode() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void ErrorResp::set_has_errorcode() {
  _has_bits_[0] |= 0x00000001u;
}
inline void ErrorResp::clear_has_errorcode() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void ErrorResp::clear_errorcode() {
  errorcode_ = 0;
  clear_has_errorcode();
}
inline ::google::protobuf::int32 ErrorResp::errorcode() const {
  // @@protoc_insertion_point(field_get:ErrorResp.errorCode)
  return errorcode_;
}
inline void ErrorResp::set_errorcode(::google::protobuf::int32 value) {
  set_has_errorcode();
  errorcode_ = value;
  // @@protoc_insertion_point(field_set:ErrorResp.errorCode)
}

// optional uint32 ExtraInfo = 2;
inline bool ErrorResp::has_extrainfo() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void ErrorResp::set_has_extrainfo() {
  _has_bits_[0] |= 0x00000002u;
}
inline void ErrorResp::clear_has_extrainfo() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void ErrorResp::clear_extrainfo() {
  extrainfo_ = 0u;
  clear_has_extrainfo();
}
inline ::google::protobuf::uint32 ErrorResp::extrainfo() const {
  // @@protoc_insertion_point(field_get:ErrorResp.ExtraInfo)
  return extrainfo_;
}
inline void ErrorResp::set_extrainfo(::google::protobuf::uint32 value) {
  set_has_extrainfo();
  extrainfo_ = value;
  // @@protoc_insertion_point(field_set:ErrorResp.ExtraInfo)
}

// optional string errorDescription = 3;
inline bool ErrorResp::has_errordescription() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void ErrorResp::set_has_errordescription() {
  _has_bits_[0] |= 0x00000004u;
}
inline void ErrorResp::clear_has_errordescription() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void ErrorResp::clear_errordescription() {
  if (errordescription_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    errordescription_->clear();
  }
  clear_has_errordescription();
}
inline const ::std::string& ErrorResp::errordescription() const {
  // @@protoc_insertion_point(field_get:ErrorResp.errorDescription)
  return *errordescription_;
}
inline void ErrorResp::set_errordescription(const ::std::string& value) {
  set_has_errordescription();
  if (errordescription_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    errordescription_ = new ::std::string;
  }
  errordescription_->assign(value);
  // @@protoc_insertion_point(field_set:ErrorResp.errorDescription)
}
inline void ErrorResp::set_errordescription(const char* value) {
  set_has_errordescription();
  if (errordescription_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    errordescription_ = new ::std::string;
  }
  errordescription_->assign(value);
  // @@protoc_insertion_point(field_set_char:ErrorResp.errorDescription)
}
inline void ErrorResp::set_errordescription(const char* value, size_t size) {
  set_has_errordescription();
  if (errordescription_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    errordescription_ = new ::std::string;
  }
  errordescription_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:ErrorResp.errorDescription)
}
inline ::std::string* ErrorResp::mutable_errordescription() {
  set_has_errordescription();
  if (errordescription_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    errordescription_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:ErrorResp.errorDescription)
  return errordescription_;
}
inline ::std::string* ErrorResp::release_errordescription() {
  clear_has_errordescription();
  if (errordescription_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = errordescription_;
    errordescription_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void ErrorResp::set_allocated_errordescription(::std::string* errordescription) {
  if (errordescription_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete errordescription_;
  }
  if (errordescription) {
    set_has_errordescription();
    errordescription_ = errordescription;
  } else {
    clear_has_errordescription();
    errordescription_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:ErrorResp.errorDescription)
}


// @@protoc_insertion_point(namespace_scope)

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_ErrorResp_2eproto__INCLUDED
