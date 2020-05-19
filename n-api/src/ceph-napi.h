#include <napi.h>

#ifndef SRC_CEPHNAPI_H_
#define SRC_CEPHNAPI_H_

Napi::Object Init(Napi::Env env, Napi::Object exports);

namespace cephnapi {

std::string version();
Napi::String VersionWrapped(const Napi::CallbackInfo& info);

}

#endif
