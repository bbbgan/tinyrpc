#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>
#include <tinyrpc/stubgenerator/StubGenerator.h>

#include <unordered_set>
#include <string_view>

using namespace tinyrpc;

namespace {
void ensure(bool check, const char* errorMsg) {
  if (check == false) fprintf(stderr, "%s\n", errorMsg);
}
}  // namespace



// {"name":"Arithmetic","rpc":[{"name":"Add","params":{"lhs":1.0,"rhs":1.0},"returns":2.0},{"name":"Sub","params":{"lhs":1.0,"rhs":1.0},"returns":0.0},{"name":"Mul","params":{"lhs":2.0,"rhs":3.0},"returns":6.0},{"name":"Div","params":{"lhs":6.0,"rhs":2.0},"returns":3.0}]}
void StubGenerator::parseProto(Value& jsonValue) {
  ensure(jsonValue.IsObject(), "ensure object");

  auto it = jsonValue.FindMember("name");

  ensure(it != jsonValue.MemberEnd(), "missing service name");
  ensure(it->value.IsString(), "service name must be string");
  serviceInfo_.name = it->value.GetString();

  it = jsonValue.FindMember("rpc");
  ensure(it != jsonValue.MemberEnd(), "missing service rpc definition");
  ensure(it->value.IsArray(), "rpc field must be array");
  rapidjson::SizeType n = it->value.Size();
  for (rapidjson::SizeType i = 0; i < n; i++) {
    parseRpc(it->value[i]);
  }
}
// {"name":"Add","params":{"lhs":1.0,"rhs":1.0},"returns":2.0}
void StubGenerator::parseRpc(Value& rpc) {
  ensure(rpc.IsObject(), "rpc definition must be object");

  auto name = rpc.FindMember("name");
  ensure(name != rpc.MemberEnd(), "missing name in rpc definition");
  ensure(name->value.IsString(), "rpc name must be string");

  auto params = rpc.FindMember("params");
  bool hasParams = (params != rpc.MemberEnd());
  if (hasParams) {
    validateParams(params->value);
  }

  auto returns = rpc.FindMember("returns");
  bool hasReturns = (returns != rpc.MemberEnd());
  if (hasReturns) {
    validateReturns(returns->value);
  }
  
  Document paramsValue(rapidjson::kObjectType);
  if (hasParams) {
      paramsValue.CopyFrom(params->value, paramsValue.GetAllocator());
  }

  if (hasReturns) {
    RpcReturn r(name->value.GetString(), paramsValue, returns->value);
    serviceInfo_.rpcReturn.push_back(std::move(r));
  } else {
    RpcNotify r(name->value.GetString(), paramsValue);
    serviceInfo_.rpcNotify.push_back(std::move(r));
  }
}

void StubGenerator::validateParams(Value& params) {
  std::unordered_set<std::string> set;

  for (auto& p : params.GetObject()) {
    auto key = p.name.GetString();
    auto unique = set.insert(key).second;
    ensure(unique, "duplicate param name");

    switch (p.value.GetType()) {
      case rapidjson::kNullType:
        ensure(false, "bad param type");
        break;
      default:
        break;
    }
  }
}

void StubGenerator::validateReturns(Value& returns) {
  switch (returns.GetType()) {
    case rapidjson::kNullType:
    case rapidjson::kArrayType:
      ensure(false, "bad returns type");
      break;
    case rapidjson::kObjectType:
      validateParams(returns);
      break;
    default:
      break;
  }
}