/**
 * Autogenerated by Thrift Compiler (0.9.1)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef Topup_H
#define Topup_H

#include <thrift/TDispatchProcessor.h>
#include "TopupInterface_types.h"

namespace topupinterface {

class TopupIf {
 public:
  virtual ~TopupIf() {}
  virtual void SendRequest(std::string& _return, const TopupRequest& request) = 0;
  virtual int32_t Admin(const ManageRequest& request) = 0;
};

class TopupIfFactory {
 public:
  typedef TopupIf Handler;

  virtual ~TopupIfFactory() {}

  virtual TopupIf* getHandler(const ::apache::thrift::TConnectionInfo& connInfo) = 0;
  virtual void releaseHandler(TopupIf* /* handler */) = 0;
};

class TopupIfSingletonFactory : virtual public TopupIfFactory {
 public:
  TopupIfSingletonFactory(const boost::shared_ptr<TopupIf>& iface) : iface_(iface) {}
  virtual ~TopupIfSingletonFactory() {}

  virtual TopupIf* getHandler(const ::apache::thrift::TConnectionInfo&) {
    return iface_.get();
  }
  virtual void releaseHandler(TopupIf* /* handler */) {}

 protected:
  boost::shared_ptr<TopupIf> iface_;
};

class TopupNull : virtual public TopupIf {
 public:
  virtual ~TopupNull() {}
  void SendRequest(std::string& /* _return */, const TopupRequest& /* request */) {
    return;
  }
  int32_t Admin(const ManageRequest& /* request */) {
    int32_t _return = 0;
    return _return;
  }
};

typedef struct _Topup_SendRequest_args__isset {
  _Topup_SendRequest_args__isset() : request(false) {}
  bool request;
} _Topup_SendRequest_args__isset;

class Topup_SendRequest_args {
 public:

  Topup_SendRequest_args() {
  }

  virtual ~Topup_SendRequest_args() throw() {}

  TopupRequest request;

  _Topup_SendRequest_args__isset __isset;

  void __set_request(const TopupRequest& val) {
    request = val;
  }

  bool operator == (const Topup_SendRequest_args & rhs) const
  {
    if (!(request == rhs.request))
      return false;
    return true;
  }
  bool operator != (const Topup_SendRequest_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Topup_SendRequest_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class Topup_SendRequest_pargs {
 public:


  virtual ~Topup_SendRequest_pargs() throw() {}

  const TopupRequest* request;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _Topup_SendRequest_result__isset {
  _Topup_SendRequest_result__isset() : success(false) {}
  bool success;
} _Topup_SendRequest_result__isset;

class Topup_SendRequest_result {
 public:

  Topup_SendRequest_result() : success() {
  }

  virtual ~Topup_SendRequest_result() throw() {}

  std::string success;

  _Topup_SendRequest_result__isset __isset;

  void __set_success(const std::string& val) {
    success = val;
  }

  bool operator == (const Topup_SendRequest_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const Topup_SendRequest_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Topup_SendRequest_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _Topup_SendRequest_presult__isset {
  _Topup_SendRequest_presult__isset() : success(false) {}
  bool success;
} _Topup_SendRequest_presult__isset;

class Topup_SendRequest_presult {
 public:


  virtual ~Topup_SendRequest_presult() throw() {}

  std::string* success;

  _Topup_SendRequest_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

typedef struct _Topup_Admin_args__isset {
  _Topup_Admin_args__isset() : request(false) {}
  bool request;
} _Topup_Admin_args__isset;

class Topup_Admin_args {
 public:

  Topup_Admin_args() {
  }

  virtual ~Topup_Admin_args() throw() {}

  ManageRequest request;

  _Topup_Admin_args__isset __isset;

  void __set_request(const ManageRequest& val) {
    request = val;
  }

  bool operator == (const Topup_Admin_args & rhs) const
  {
    if (!(request == rhs.request))
      return false;
    return true;
  }
  bool operator != (const Topup_Admin_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Topup_Admin_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class Topup_Admin_pargs {
 public:


  virtual ~Topup_Admin_pargs() throw() {}

  const ManageRequest* request;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _Topup_Admin_result__isset {
  _Topup_Admin_result__isset() : success(false) {}
  bool success;
} _Topup_Admin_result__isset;

class Topup_Admin_result {
 public:

  Topup_Admin_result() : success(0) {
  }

  virtual ~Topup_Admin_result() throw() {}

  int32_t success;

  _Topup_Admin_result__isset __isset;

  void __set_success(const int32_t val) {
    success = val;
  }

  bool operator == (const Topup_Admin_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const Topup_Admin_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const Topup_Admin_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _Topup_Admin_presult__isset {
  _Topup_Admin_presult__isset() : success(false) {}
  bool success;
} _Topup_Admin_presult__isset;

class Topup_Admin_presult {
 public:


  virtual ~Topup_Admin_presult() throw() {}

  int32_t* success;

  _Topup_Admin_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

class TopupClient : virtual public TopupIf {
 public:
  TopupClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) :
    piprot_(prot),
    poprot_(prot) {
    iprot_ = prot.get();
    oprot_ = prot.get();
  }
  TopupClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, boost::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) :
    piprot_(iprot),
    poprot_(oprot) {
    iprot_ = iprot.get();
    oprot_ = oprot.get();
  }
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getInputProtocol() {
    return piprot_;
  }
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getOutputProtocol() {
    return poprot_;
  }
  void SendRequest(std::string& _return, const TopupRequest& request);
  void send_SendRequest(const TopupRequest& request);
  void recv_SendRequest(std::string& _return);
  int32_t Admin(const ManageRequest& request);
  void send_Admin(const ManageRequest& request);
  int32_t recv_Admin();
 protected:
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> piprot_;
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> poprot_;
  ::apache::thrift::protocol::TProtocol* iprot_;
  ::apache::thrift::protocol::TProtocol* oprot_;
};

class TopupProcessor : public ::apache::thrift::TDispatchProcessor {
 protected:
  boost::shared_ptr<TopupIf> iface_;
  virtual bool dispatchCall(::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, const std::string& fname, int32_t seqid, void* callContext);
 private:
  typedef  void (TopupProcessor::*ProcessFunction)(int32_t, ::apache::thrift::protocol::TProtocol*, ::apache::thrift::protocol::TProtocol*, void*);
  typedef std::map<std::string, ProcessFunction> ProcessMap;
  ProcessMap processMap_;
  void process_SendRequest(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_Admin(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
 public:
  TopupProcessor(boost::shared_ptr<TopupIf> iface) :
    iface_(iface) {
    processMap_["SendRequest"] = &TopupProcessor::process_SendRequest;
    processMap_["Admin"] = &TopupProcessor::process_Admin;
  }

  virtual ~TopupProcessor() {}
};

class TopupProcessorFactory : public ::apache::thrift::TProcessorFactory {
 public:
  TopupProcessorFactory(const ::boost::shared_ptr< TopupIfFactory >& handlerFactory) :
      handlerFactory_(handlerFactory) {}

  ::boost::shared_ptr< ::apache::thrift::TProcessor > getProcessor(const ::apache::thrift::TConnectionInfo& connInfo);

 protected:
  ::boost::shared_ptr< TopupIfFactory > handlerFactory_;
};

class TopupMultiface : virtual public TopupIf {
 public:
  TopupMultiface(std::vector<boost::shared_ptr<TopupIf> >& ifaces) : ifaces_(ifaces) {
  }
  virtual ~TopupMultiface() {}
 protected:
  std::vector<boost::shared_ptr<TopupIf> > ifaces_;
  TopupMultiface() {}
  void add(boost::shared_ptr<TopupIf> iface) {
    ifaces_.push_back(iface);
  }
 public:
  void SendRequest(std::string& _return, const TopupRequest& request) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->SendRequest(_return, request);
    }
    ifaces_[i]->SendRequest(_return, request);
    return;
  }

  int32_t Admin(const ManageRequest& request) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->Admin(request);
    }
    return ifaces_[i]->Admin(request);
  }

};

} // namespace

#endif
