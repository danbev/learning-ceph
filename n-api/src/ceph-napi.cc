#include <napi.h>
#include <rados/librados.hpp>
#include <string>
#include <iostream>
#include "ceph-napi.h"

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set("version", Napi::Function::New(env, cephnapi::version));
  exports.Set("init", Napi::Function::New(env, cephnapi::init));
  return exports;
}

namespace cephnapi {

Napi::String version(const Napi::CallbackInfo& info) {
  int major, minor, extra;
  rados_version(&major, &minor, &extra);
  std::string version = std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(extra);
  Napi::String returnValue = Napi::String::New(info.Env(), version);
  return returnValue;
}

void init(const Napi::CallbackInfo& info) {
  std::cout << "ceph-napi init..." << '\n';
  Napi::Env env = info.Env();
  if (!info.Length() > 2) {
    Napi::TypeError::New(env, "username and cluster arguments "
        "must be specified").ThrowAsJavaScriptException();
    return;
  }
  if (!info[0].IsString()) {
    Napi::TypeError::New(env, "username argument must be specified")
       .ThrowAsJavaScriptException();
    return;
  }
  if (!info[1].IsString()) {
    Napi::TypeError::New(env, "clustername argument must be specified")
       .ThrowAsJavaScriptException();
    return;
  }

  Napi::String user_name = info[0].As<Napi::String>();
  Napi::String cluster_name = info[1].As<Napi::String>();
  uint64_t flags = 0;
  librados::Rados cluster;

  std::cout << "user_name: " << user_name.Utf8Value() << ", cluster_name: " << cluster_name.Utf8Value() << '\n';
  int ret = cluster.init2(user_name.Utf8Value().c_str(),
                          cluster_name.Utf8Value().c_str(),
                          flags);
  if (ret < 0) {
    Napi::TypeError::New(env, "Cound not initialize cluster handle")
     .ThrowAsJavaScriptException();
    return;
  }

  std::cout << "Created a cluster handle." << '\n';
  ret = cluster.conf_read_file("/etc/ceph/ceph.conf");
  if (ret < 0) {
    Napi::TypeError::New(env, "Cound not read /etc/ceph/ceph.conf")
     .ThrowAsJavaScriptException();
    return;
  }
  std::cout << "Read cluster config: " << '\n';

  ret = cluster.connect();
  if (ret < 0) {
    std::cout << "Connected not connect to the cluster. ret: " << ret << '\n';
    Napi::TypeError::New(env, "Cound not connect to the cluster")
     .ThrowAsJavaScriptException();
    return;
  }
  std::cout << "Connected to the cluster..." << '\n';
}

} // namespace cephnapi

NODE_API_MODULE(cephnapi, Init)
