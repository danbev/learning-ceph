#include <napi.h>
#include <rados/librados.hpp>
#include <string>
#include <iostream>
#include "ceph-napi.h"

Napi::Object Initialize(Napi::Env env, Napi::Object exports) {
  Ceph::Init(env, exports);
  return exports;
}

Napi::Object Ceph::Init(Napi::Env env, Napi::Object exports) {
  std::cout << "Ceph init..." << '\n';
  Napi::Function function = DefineClass(env, "Ceph", {
      InstanceMethod<&Ceph::Version>("version"),
      InstanceMethod<&Ceph::Connect>("connect")
  });
  constructor = Napi::Persistent(function);
  constructor.SuppressDestruct();
  exports.Set("Ceph", function);
  return exports;
}

Napi::FunctionReference Ceph::constructor;

Ceph::Ceph(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Ceph>(info) {
    Napi::Env env = info.Env();
    std::cout << "Ceph constructor..." << '\n';
    if (info.Length() == 1) {
      if (info[0].IsObject()) {
        Napi::Object config = info[0].As<Napi::Object>();
        Napi::Value ceph_conf_value = config.Get("ceph_conf");
        if (!ceph_conf_value.IsEmpty()) {
          ceph_conf_ = ceph_conf_value.As<Napi::String>().Utf8Value();
        }
      }
    }
}


class InitWorker : public Napi::AsyncWorker {
 public:
  InitWorker(std::string& username,
             std::string& clustername,
             std::string& ceph_conf,
             Napi::Function& callback) :
    AsyncWorker(callback), username_(username), ceph_conf_(ceph_conf), clustername_(clustername) {}    

  void Execute() override {
    std::cout << "ceph-napi init..." << '\n';
    Napi::Env env = Env();
    librados::Rados cluster;
    uint64_t flags = 0;

    int ret = cluster.init2(username_.c_str(), clustername_.c_str(), flags);
    if (ret < 0) {
      SetError("Cound not initialize cluster handle");
      return;
    }

    ret = cluster.conf_read_file(ceph_conf_.c_str());
    if (ret < 0) {
      SetError("Cound not read ceph.conf");
      return;
    }

    ret = cluster.connect();
    if (ret < 0) {
      std::cout << "Could not connect to the cluster. ret: " << ret << '\n';
      SetError("Cound not connect to the cluster");
      return;
    }
    std::cout << "Connected to Ceph cluster" << '\n';
    // TODO: we should really create a ObjectWrap for this and enable user
    // to create on and call methods on it. That instance would also store the
    // Rados instance (named cluster) here. But for now I just want to try things
    // out and see if I can write data to an OSD.
    librados::IoCtx io_ctx;
    const char* poolname = "data";
    ret = cluster.pool_create(poolname);
    if (ret < 0) {
      std::cout << "Could not create pool: " << poolname << ", ret: " << ret << '\n';
      SetError("Cound not create rados pool");
      return;
    }

    ret = cluster.ioctx_create(poolname, io_ctx);
    if (ret < 0) {
      std::cout << "cannot open rados pool: " << poolname << ", ret: " << ret << '\n';
      SetError("Cound not open rados pool");
      return;
    }

    librados::bufferlist wbl;
    wbl.append("bajja");
    std::cout << std::boolalpha << "Is io_ctx valid: " << io_ctx.is_valid() << '\n';
    ret = io_ctx.write_full("msg_id", wbl);
    if (ret < 0) {
      std::cout << "could not write to pool: " << poolname << ", ret: " << ret << '\n';
      SetError("Cound not write to pool");
    }
    std::cout << "Successfully wrote to pool data."<< '\n';

    librados::bufferlist rbl;
    ret = io_ctx.read("msg_id", rbl, 6, 0);
    if (ret < 0) {
      std::cout << "could not read, ret: " << ret << '\n';
      SetError("Cound not read from pool");
    }
    std::string value;
    rbl.begin().copy(ret, value);
    std::cout << "Successfully read data: " << value << '\n';
  }

  void OnOK() override {
    Napi::HandleScope scope(Env());
    Callback().Call({Napi::String::New(Env(), "connected")});
  }

  void OnError(const Napi::Error& error) override {
    error.ThrowAsJavaScriptException();
  }

 private:
  std::string username_;
  std::string clustername_;
  std::string ceph_conf_;
};

Napi::Value Ceph::Version(const Napi::CallbackInfo& info) {
  int major, minor, extra;
  rados_version(&major, &minor, &extra);
  std::string version = std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(extra);
  Napi::String returnValue = Napi::String::New(info.Env(), version);
  return returnValue;
}

Napi::Value Ceph::Connect(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (!info.Length() > 3) {
    Napi::TypeError::New(env, "username, cluster, and callback arguments "
        "must be specified").ThrowAsJavaScriptException();
    return info.Env().Undefined();
  }
  if (!info[0].IsString()) {
    Napi::TypeError::New(env, "username argument must be specified")
       .ThrowAsJavaScriptException();
    return info.Env().Undefined();
  }
  if (!info[1].IsString()) {
    Napi::TypeError::New(env, "clustername argument must be specified")
       .ThrowAsJavaScriptException();
    return info.Env().Undefined();
  }
  if (!info[2].IsFunction()) {
    Napi::TypeError::New(env, "callback argument must be specified")
       .ThrowAsJavaScriptException();
    return info.Env().Undefined();
  }
  std::string username = info[0].As<Napi::String>().Utf8Value();
  std::string clustername = info[1].As<Napi::String>().Utf8Value();
  Napi::Function callback = info[2].As<Napi::Function>();
  InitWorker* worker = new InitWorker(username, clustername, ceph_conf_, callback);
  worker->Queue();
  return info.Env().Undefined();
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Initialize)
