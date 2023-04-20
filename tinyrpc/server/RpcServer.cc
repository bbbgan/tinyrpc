#include <tinyrpc/server/RpcServer.h>
#include <tinyrpc/server/RpcService.h>
#include <tinyrpc/util.h>

using namespace tinyrpc;
using namespace tinynet;
namespace {

bool isNotify(const Value& request) {
  LOG_INFO("Parsed request type: %d", request.GetType());
  return request.FindMember("id") == request.MemberEnd();
}

}  // namespace

void RpcServer::addService(std::string_view serviceName, RpcService* service) {
  assert(services_.find(serviceName) == services_.end());
  services_.emplace(serviceName, service);
}

void RpcServer::handleRequest(const std::string& json,
                              const RpcDoneCallback& done) {
  Document request;
  request.Parse(json.c_str(), json.size());
  if (request.HasParseError()) {
    LOG_ERROR("RpcServer::handleRequest : Parse error");
    return;
  }
  LOG_INFO("id : %d", request["id"].GetInt64());
  switch (request.GetType()) {
    case rapidjson::kObjectType:
      if (isNotify(request))
        handleSingleNotify(request);
      else
        handleSingleRequest(request, done);
      break;
    case rapidjson::kArrayType:
      LOG_ERROR("kArrayType is not support");
      break;
    default:
      LOG_ERROR("RpcServer::handleRequest : unknown type");
  }
}
void RpcServer::handleSingleRequest(Value& request,
                                    const RpcDoneCallback& done) {
  // FIXME : check request is valid
  //{"jsonrpc":"2.0","method":"Arithmetic.Add","params":{"lhs":10.0,"rhs":3.0},"id":0}
  // must ensure the request is valid
  LOG_TRACE("start handleSingleRequest");

  std::string_view methodName = request["method"].GetString();
  LOG_TRACE("methodName : %s", methodName.data());
  auto pos = methodName.find('.');
  if (pos == std::string_view::npos)
    LOG_ERROR("missing service name in method");
  auto serviceName = methodName.substr(0, pos);
  auto it = services_.find(serviceName);
  if (it == services_.end())
    LOG_ERROR("service not found");
  methodName.remove_prefix(pos + 1);
  if (methodName.length() == 0)
    LOG_ERROR("missing method name in method");
  auto& service = it->second;
  service->callProcedureReturn(methodName, request, done);
}

// handleBatchRequests

void RpcServer::handleSingleNotify(Value& request) {
  // FIXME : check request is valid
  std::string_view methodName = request["method"].GetString();
  auto pos = methodName.find('.');
  if (pos == std::string_view::npos || pos == 0)
    LOG_ERROR("missing service name in method");
  auto serviceName = methodName.substr(0, pos);
  auto it = services_.find(serviceName);
  if (it == services_.end())
    LOG_ERROR("service not found");
  methodName.remove_prefix(pos + 1);
  if (methodName.length() == 0)
    LOG_ERROR("missing method name in method");
  auto& service = it->second;
  service->callProcedureNotify(methodName, request);
}