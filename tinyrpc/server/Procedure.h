#ifndef TINYRPC_PROCEDURE_H_
#define TINYRPC_PROCEDURE_H_

#include <tinyrpc/util.h>
#include <type_traits>

namespace tinyrpc
{

using ProcedureReturnCallback = std::function<void(Value&, const RpcDoneCallback&)>;
using ProcedureNotifyCallback = std::function<void(Value&)>;

template <typename Func>
class Procedure: noncopyable
{
 public:
  template <typename... ParamNameAndTypes>
  explicit Procedure(Func&& callback, ParamNameAndTypes&&... nameWithType)
      : callback_(std::forward<Func>(callback)) {
    constexpr int n = sizeof...(nameWithType);
    static_assert((n & 1) == 0, "Parameter name and type must exist in pairs");
    if constexpr (n > 0) 
      initProcedure(nameWithType...);
  }

  void invoke(Value& request, const RpcDoneCallback& done) {
    // validateRequest(request);
    callback_(request, done);    
  }
  void invoke(Value& request) {
  // validateRequest(request);
  callback_(request);
  }

 private:
  template<typename Name, typename... ParamNameAndTypes>
  void initProcedure(Name paramName, JsonType parmType, ParamNameAndTypes&&... rest) {
      static_assert(std::is_same_v<Name, const char*> || std::is_same_v<Name, std::string_view>,
                    "param name must be 'const char*' or 'std::string_view'");
      params_.emplace_back(paramName, parmType);
      if constexpr (sizeof...(ParamNameAndTypes) > 0)
          initProcedure(rest...);
  }

  //  match this func if T != Type 
  template<typename Name, typename T, typename... ParamNameAndTypes>
  void initProcedure(Name paramName, T parmType, ParamNameAndTypes&&... nameWithType) {
      static_assert(std::is_same_v<T, JsonType>, "param type must be rapidjson::Type");
  }

  struct Param {
    Param(std::string_view paramName_, JsonType paramType_)
        : paramName(paramName_), paramType(paramType_) {}

    std::string_view paramName;
    JsonType paramType;
  };

  Func callback_;
  std::vector<Param> params_;
};

using ProcedureReturn = Procedure<ProcedureReturnCallback>;
using ProcedureNotify = Procedure<ProcedureNotifyCallback>;

} // namespace tinyrpc


#endif // TINYRPC_PROCEDURE_H_