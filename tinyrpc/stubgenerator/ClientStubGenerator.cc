#include <tinyrpc/stubgenerator/ClientStubGenerator.h>

using namespace tinyrpc;

namespace {

std::string clientStubTemplate(const std::string& macroName,
                               const std::string& stubClassName,
                               const std::string& procedureDefinitions,
                               const std::string& notifyDefinitions) {
  std::string str = R"(
/*
 * This stub is generated by tinyrpc, DO NOT modify it!
 */

#ifndef TINYRPC_[macroName]_H_
#define TINYRPC_[macroName]_H_

#include <tinyrpc/client/BaseClient.h>
#include <tinyrpc/util.h>

namespace tinyrpc
{

class [stubClassName]: noncopyable
{
public:
    [stubClassName](EventLoop* loop, const InetAddress& serverAddress):
            client_(loop, serverAddress)
    {
        client_.setConnectionCallback([this](const TcpConnectionPtr& conn){
            if (conn->connected()) {
                LOG_INFO("connected");
                conn_ = conn;
                cb_(conn_);
            }
            else {
                LOG_INFO("disconnected");
                assert(conn_ != nullptr);
                cb_(conn_);
            }
        });
    }

    ~[stubClassName]() = default;

    void start() { client_.start(); }

    void setConnectionCallback(const ConnectionCallback& cb) { cb_ = cb; }

    [procedureDefinitions]
    [notifyDefinitions]

private:
    TcpConnectionPtr conn_;
    ConnectionCallback cb_;
    BaseClient client_;
};

} // namespace tinyrpc
#endif // TINYRPC_[macroName]_H_
)";
  replaceAll(str, "[macroName]", macroName);
  replaceAll(str, "[stubClassName]", stubClassName);
  replaceAll(str, "[procedureDefinitions]", procedureDefinitions);
  replaceAll(str, "[notifyDefinitions]", notifyDefinitions);
  return str;
}

std::string procedureDefineTemplate(const std::string& serviceName,
                                    const std::string& procedureName,
                                    const std::string& procedureArgs,
                                    const std::string& paramMembers) {
  std::string str = R"(
void [procedureName]([procedureArgs] const ResponseCallback& cb)
{
    Document request(rapidjson::kObjectType);
    Value jsonrpc;
    jsonrpc.SetString("2.0", request.GetAllocator());
    request.AddMember("jsonrpc", jsonrpc, request.GetAllocator());   
    Value method;
    method.SetString("[serviceName].[procedureName]", request.GetAllocator());
    request.AddMember("method", method, request.GetAllocator()); 

    Value params(rapidjson::kObjectType);  
    [paramMembers]
    request.AddMember("params", params, request.GetAllocator());

    assert(conn_ != nullptr);
    client_.sendCall(conn_, request, cb);
}
)";
  replaceAll(str, "[serviceName]", serviceName);
  replaceAll(str, "[procedureName]", procedureName);
  replaceAll(str, "[procedureArgs]", procedureArgs);
  replaceAll(str, "[paramMembers]", paramMembers);
  return str;
}

std::string notifyDefineTemplate(const std::string& serviceName,
                                 const std::string& notifyName,
                                 const std::string& notifyArgs,
                                 const std::string& paramMembers) {
  std::string str = R"(
void [notifyName]([notifyArgs])
{

    Document request(rapidjson::kObjectType);
    Value jsonrpc;
    jsonrpc.SetString("2.0", request.GetAllocator());
    request.AddMember("jsonrpc", jsonrpc, request.GetAllocator());   
    Value method;
    method.SetString("[serviceName].[procedureName]", request.GetAllocator());
    request.AddMember("method", method, request.GetAllocator()); 

    Value params(rapidjson::kObjectType);  
    [paramMembers]
    request.AddMember("params", params, request.GetAllocator());

    assert(conn_ != nullptr);
    client_.sendNotify(conn_, notify);
}
)";
  replaceAll(str, "[serviceName]", serviceName);
  replaceAll(str, "[notifyName]", notifyName);
  replaceAll(str, "[notifyArgs]", notifyArgs);
  replaceAll(str, "[paramMembers]", paramMembers);
  return str;
}

std::string paramMemberTemplate(const std::string& paramName) {
  std::string str = R"(
params.AddMember("[paramName]", [paramName], request.GetAllocator());
)";
  replaceAll(str, "[paramName]", paramName);
  return str;
}

std::string argTemplate(const std::string& argName, JsonType argType) {
  std::string str = R"([argType] [argName])";
  auto typeStr = [=]() {
    switch (argType) {
      case rapidjson::kNumberType:
        return "double";
      case rapidjson::kFalseType:
        return "bool";
      case rapidjson::kStringType:
        return "std::string";
      case rapidjson::kObjectType:
      case rapidjson::kArrayType:
        return "Value";
      default:
        fprintf(stderr, "%s\n", "bad type");
        return "";
    }
  }();
  replaceAll(str, "[argType]", typeStr);
  replaceAll(str, "[argName]", argName);
  return str;
}

}  // namespace
std::string ClientStubGenerator::genStub() {
  auto macroName = genMacroName();
  auto stubClassName = genStubClassName();
  auto procedureDefinitions = genProcedureDefinitions();
  auto notifyDefinitions = genNotifyDefinitions();

  return clientStubTemplate(macroName, stubClassName, procedureDefinitions,
                            notifyDefinitions);
}

std::string ClientStubGenerator::genMacroName() {
  std::string result = serviceInfo_.name;
  for (char& c : result) c = static_cast<char>(toupper(c));
  return result + "CLIENTSTUB";
}

std::string ClientStubGenerator::genStubClassName() {
  return serviceInfo_.name + "ClientStub";
}

std::string ClientStubGenerator::genProcedureDefinitions() {
  std::string result;

  auto& serviceName = serviceInfo_.name;

  for (auto& r : serviceInfo_.rpcReturn) {
    auto& procedureName = r.name;
    auto procedureArgs = genGenericArgs(r, true);
    auto paramMembers = genGenericParamMembers(r);

    auto str = procedureDefineTemplate(serviceName, procedureName,
                                       procedureArgs, paramMembers);
    result.append(str);
  }
  return result;
}

std::string ClientStubGenerator::genNotifyDefinitions() {
  std::string result;

  auto& serviceName = serviceInfo_.name;

  for (auto& r : serviceInfo_.rpcNotify) {
    auto& notifyName = r.name;
    auto notifyArgs = genGenericArgs(r, false);
    auto paramMembers = genGenericParamMembers(r);

    auto str =
        notifyDefineTemplate(serviceName, notifyName, notifyArgs, paramMembers);
    result.append(str);
  }
  return result;
}

template <typename Rpc>
std::string ClientStubGenerator::genGenericArgs(const Rpc& r,
                                                bool appendComma) {
  std::string result;
  bool first = true;
  for (auto& p : r.params.GetObject()) {
    std::string one = argTemplate(p.name.GetString(), p.value.GetType());
    if (first) {
      first = false;
    } else {
      result.append(", ");
    }
    result.append(one);
  }
  if (appendComma && !first) result.append(",");
  return result;
}

template <typename Rpc>
std::string ClientStubGenerator::genGenericParamMembers(const Rpc& r) {
  std::string result;
  for (auto& p : r.params.GetObject()) {
    std::string one = paramMemberTemplate(p.name.GetString());
    result.append(one);
  }
  return result;
}