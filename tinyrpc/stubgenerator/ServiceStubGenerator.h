#ifndef TINYRPC_SERVICESTUBGENERATOR_H_
#define TINYRPC_SERVICESTUBGENERATOR_H_

#include <tinyrpc/stubgenerator/StubGenerator.h>

namespace tinyrpc {

class ServiceStubGenerator : public StubGenerator {
 public:
  explicit ServiceStubGenerator(Value& proto) : StubGenerator(proto) {}

  std::string genStub() override;
  std::string genStubClassName() override;

 private:
  std::string genMacroName();
  std::string genUserClassName();
  std::string genStubProcedureBindings();
  std::string genStubProcedureDefinitions();
  std::string genStubNotifyBindings();
  std::string genStubNotifyDefinitions();

  template <typename Rpc>
  std::string genStubGenericName(const Rpc& r);
  template <typename Rpc>
  std::string genGenericParams(const Rpc& r);
  template <typename Rpc>
  std::string genGenericArgs(const Rpc& r);

  template <typename Rpc>
  std::string genParamsFromJsonArray(const Rpc& r);
  template <typename Rpc>
  std::string genParamsFromJsonObject(const Rpc& r);
};

}  // namespace tinyrpc

#endif  // TINYRPC_SERVICESTUBGENERATOR_H_
