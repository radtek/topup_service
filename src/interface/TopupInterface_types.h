/**
 * Autogenerated by Thrift Compiler (0.9.1)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef TopupInterface_TYPES_H
#define TopupInterface_TYPES_H

#include <thrift/Thrift.h>
#include <thrift/TApplicationException.h>
#include <thrift/protocol/TProtocol.h>
#include <thrift/transport/TTransport.h>

#include <thrift/cxxfunctional.h>


namespace topupinterface {

typedef struct _TopupRequest__isset {
  _TopupRequest__isset() : query(false), checksum(false), ip(false), version(false), uri(false), itimestamp(false), exparams(false) {}
  bool query;
  bool checksum;
  bool ip;
  bool version;
  bool uri;
  bool itimestamp;
  bool exparams;
} _TopupRequest__isset;

class TopupRequest {
 public:

  static const char* ascii_fingerprint; // = "A55DCCE3096C666DFC42A860CE0388C3";
  static const uint8_t binary_fingerprint[16]; // = {0xA5,0x5D,0xCC,0xE3,0x09,0x6C,0x66,0x6D,0xFC,0x42,0xA8,0x60,0xCE,0x03,0x88,0xC3};

  TopupRequest() : query(), checksum(), ip(), version(), uri(), itimestamp(0) {
  }

  virtual ~TopupRequest() throw() {}

  std::string query;
  std::string checksum;
  std::string ip;
  std::string version;
  std::string uri;
  int32_t itimestamp;
  std::map<std::string, std::string>  exparams;

  _TopupRequest__isset __isset;

  void __set_query(const std::string& val) {
    query = val;
  }

  void __set_checksum(const std::string& val) {
    checksum = val;
  }

  void __set_ip(const std::string& val) {
    ip = val;
  }

  void __set_version(const std::string& val) {
    version = val;
  }

  void __set_uri(const std::string& val) {
    uri = val;
  }

  void __set_itimestamp(const int32_t val) {
    itimestamp = val;
  }

  void __set_exparams(const std::map<std::string, std::string> & val) {
    exparams = val;
  }

  bool operator == (const TopupRequest & rhs) const
  {
    if (!(query == rhs.query))
      return false;
    if (!(checksum == rhs.checksum))
      return false;
    if (!(ip == rhs.ip))
      return false;
    if (!(version == rhs.version))
      return false;
    if (!(uri == rhs.uri))
      return false;
    if (!(itimestamp == rhs.itimestamp))
      return false;
    if (!(exparams == rhs.exparams))
      return false;
    return true;
  }
  bool operator != (const TopupRequest &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const TopupRequest & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

void swap(TopupRequest &a, TopupRequest &b);

typedef struct _ManageRequest__isset {
  _ManageRequest__isset() : cmd(false), key(false), value(false) {}
  bool cmd;
  bool key;
  bool value;
} _ManageRequest__isset;

class ManageRequest {
 public:

  static const char* ascii_fingerprint; // = "3368C2F81F2FEF71F11EDACDB2A3ECEF";
  static const uint8_t binary_fingerprint[16]; // = {0x33,0x68,0xC2,0xF8,0x1F,0x2F,0xEF,0x71,0xF1,0x1E,0xDA,0xCD,0xB2,0xA3,0xEC,0xEF};

  ManageRequest() : cmd(0), key(), value() {
  }

  virtual ~ManageRequest() throw() {}

  int32_t cmd;
  std::string key;
  std::string value;

  _ManageRequest__isset __isset;

  void __set_cmd(const int32_t val) {
    cmd = val;
  }

  void __set_key(const std::string& val) {
    key = val;
  }

  void __set_value(const std::string& val) {
    value = val;
  }

  bool operator == (const ManageRequest & rhs) const
  {
    if (!(cmd == rhs.cmd))
      return false;
    if (!(key == rhs.key))
      return false;
    if (!(value == rhs.value))
      return false;
    return true;
  }
  bool operator != (const ManageRequest &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const ManageRequest & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

void swap(ManageRequest &a, ManageRequest &b);

} // namespace

#endif
