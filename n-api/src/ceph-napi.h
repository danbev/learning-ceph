#include <napi.h>
#include <string>

#ifndef SRC_CEPHNAPI_H_
#define SRC_CEPHNAPI_H_

class Ceph : public Napi::ObjectWrap<Ceph> {
 public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports);

  Ceph(const Napi::CallbackInfo& info);
  Napi::Value Version(const Napi::CallbackInfo& info);
  Napi::Value Connect(const Napi::CallbackInfo& info);

 private:
  static Napi::FunctionReference constructor;
  std::string ceph_conf_;
};

#endif
