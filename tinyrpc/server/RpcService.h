#ifndef TINYRPC_RPCSERVICE_H_
#define TINYRPC_RPCSERVICE_H_

#include <rapidjson/document.h>
#include <tinyrpc/util.h>
#include <tinyrpc/server/Procedure.h>
#include <unordered_map>
#include <string_view>

namespace tinyrpc
{
class RpcService : noncopyable
{
 public:
  void addProcedureReturn(std::string_view methodName, ProcedureReturn* procedure) {
    if (procedureReturnMap.find(methodName) != procedureReturnMap.end()) {
      LOG_ERROR(" procedureNotify already exist ");
      return;
    }
    procedureReturnMap.emplace(methodName, procedure);
  }

  void addProcedureNotify(std::string_view methodName, ProcedureNotify* procedure) {
    if(procedureNotfiyMap.find(methodName) != procedureNotfiyMap.end()) {
      LOG_ERROR(" procedureNotify already exist ");
      return;
    }
    procedureNotfiyMap.emplace(methodName, procedure);
  }
  void callProcedureReturn(std::string_view methodName, Value& request,
                           const RpcDoneCallback& done) {
    auto it = procedureReturnMap.find(methodName);
    if (it == procedureReturnMap.end()) {
      LOG_ERROR(" procedureReturn not exist ");
      return;
    }
    it->second->invoke(request, done);
  }
  void callProcedureNotify(std::string_view methodName, Value& request) {
    auto it = procedureNotfiyMap.find(methodName);
    if (it == procedureNotfiyMap.end()) {
      LOG_ERROR(" procedureNotify not exist ");
      return;
    }
    it->second->invoke(request);
  }
 private:
  typedef std::unique_ptr<ProcedureReturn> ProcedureReturnPtr;
  typedef std::unique_ptr<ProcedureNotify> ProcedureNotifyPtr;
  typedef std::unordered_map<std::string_view, ProcedureReturnPtr>
      ProcedureReturnList;
  typedef std::unordered_map<std::string_view, ProcedureNotifyPtr>
      ProcedureNotifyList;

  ProcedureReturnList procedureReturnMap;
  ProcedureNotifyList procedureNotfiyMap;
};

} // namespace tinyrpc


#endif // TINYRPC_RPCSERVICE_H_