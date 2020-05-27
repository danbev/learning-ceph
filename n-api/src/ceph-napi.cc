#include <napi.h>
#include <rados/librados.h>
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
  Napi::Value arg0 = info[0];
  if (!arg0.IsString()) {
    throw Napi::TypeError::New(info.Env(), "username argument must be of type string");
  }
  Napi::String username = arg0.As<Napi::String>();
  std::cout << "ceph-napi init..." << '\n';
  std::cout << "username: " << username.Utf8Value() << '\n';
}

} // namespace cephnapi

NODE_API_MODULE(cephnapi, Init)
