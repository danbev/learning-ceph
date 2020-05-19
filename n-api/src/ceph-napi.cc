#include <napi.h>
#include <rados/librados.h>
#include <string>
#include <iostream>
#include "ceph-napi.h"

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set("version", Napi::Function::New(env, cephnapi::VersionWrapped)
  );
 
  return exports;
  return exports;
}

namespace cephnapi {

std::string version(){
  int major, minor, extra;
  rados_version(&major, &minor, &extra);
  return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(extra);
}

Napi::String VersionWrapped(const Napi::CallbackInfo& info)
{
  Napi::String returnValue = Napi::String::New(info.Env(), version());
  return returnValue;
}

}

NODE_API_MODULE(cephnapi, Init)
