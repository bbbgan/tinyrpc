#ifndef TINYRPC_STUBGENERATOR_H_
#define TINYRPC_STUBGENERATOR_H_

#include <tinyrpc/util.h>

namespace tinyrpc {

class StubGenerator {
 public:
  explicit StubGenerator(Value& proto) { parseProto(proto); }
  virtual ~StubGenerator() = default;

 public:
  virtual std::string genStub() = 0;
  virtual std::string genStubClassName() = 0;

 protected:
   // {"name":"Add","params":{"lhs":1.0,"rhs":1.0},"returns":2.0}
  struct RpcReturn {
    RpcReturn(const std::string& name_, Value& params_, Value& returns_)
        : name(name_) {
      params.CopyFrom(params_, params.GetAllocator());
      returns.CopyFrom(returns_, returns.GetAllocator());
    }
    std::string name;
    mutable Document params;
    mutable Document returns;
  };

  struct RpcNotify 
  {
    RpcNotify(const std::string& name_, Value& params_)
        : name(name_) {
      params.CopyFrom(params_, params.GetAllocator());
    }
    std::string name;
    mutable Document params;
  };

  struct ServiceInfo {
    std::string name;
    std::vector<RpcReturn> rpcReturn;
    std::vector<RpcNotify> rpcNotify;
  };

  ServiceInfo serviceInfo_;  //解析的json参数对象都存在这里

 private:
  void parseProto(Value& proto);
  void parseRpc(Value& rpc);
  void validateParams(Value& params);
  void validateReturns(Value& returns);
};

inline void replaceAll(std::string& str, const std::string& from,
                       const std::string& to) {
  while (true) {
    size_t i = str.find(from);
    if (i != std::string::npos) {
      str.replace(i, from.size(), to);
    } else
      return;
  }
}

}  // namespace tinyrpc

#endif  // TINYRPC_STUBGENERATOR_H_
