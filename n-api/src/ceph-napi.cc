#include <napi.h>
#include "ceph-napi.h"

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set("version", Napi::Function::New(env, cephnapi::VersionWrapped)
  );
 
  return exports;
  return exports;
}

namespace cephnapi {

std::string version(){
  return "0.0.1";
}

Napi::String VersionWrapped(const Napi::CallbackInfo& info)
{
  Napi::String returnValue = Napi::String::New(info.Env(), version());
  return returnValue;
}

}

NODE_API_MODULE(cephnapi, Init)
