#include <napi.h>
#include <rados/librados.h>
#include <string>
#include <iostream>
#include "ceph-napi.h"

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set("version", Napi::Function::New(env, cephnapi::version));
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

} // namespace cephnapi

NODE_API_MODULE(cephnapi, Init)
