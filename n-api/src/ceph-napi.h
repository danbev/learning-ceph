#include <napi.h>

#ifndef SRC_CEPHNAPI_H_
#define SRC_CEPHNAPI_H_

Napi::Object Init(Napi::Env env, Napi::Object exports);

namespace cephnapi {

Napi::String version(const Napi::CallbackInfo& info);

}

#endif
