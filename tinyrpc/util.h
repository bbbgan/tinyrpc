#ifndef TINYRPC_UTIL_H_
#define TINYRPC_UTIL_H_

#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>
#include <tinynet/base/CountDownLatch.h>
#include <tinynet/base/Logger.h>
#include <tinynet/base/Timestamp.h>
#include <tinynet/net/Buffer.h>
#include <tinynet/net/Callbacks.h>
#include <tinynet/net/EventLoop.h>
#include <tinynet/net/InetAddress.h>
#include <tinynet/net/TcpClient.h>
#include <tinynet/net/TcpConnection.h>
#include <tinynet/net/TcpServer.h>

#include <string>
#include <string_view>

namespace tinyrpc {

using namespace std::literals::string_view_literals;
using namespace std::literals::chrono_literals;

using rapidjson::Document;
using rapidjson::Value;
using JsonType = rapidjson::Type;

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using std::placeholders::_4;

using namespace tinynet; // FIXME
using tinynet::CountDownLatch;
using tinynet::noncopyable;
using tinynet::net::Buffer;
using tinynet::net::ConnectionCallback;
using tinynet::net::EventLoop;
using tinynet::net::InetAddress;
using tinynet::net::TcpClient;
using tinynet::net::TcpConnection;
using tinynet::net::TcpConnectionPtr;
using tinynet::net::TcpServer;

typedef std::function<void(Value& response)> RpcDoneCallback;

class UserDoneCallback {
 public:
  UserDoneCallback(Value &request, const RpcDoneCallback &callback)
    : callback_(callback) {
    id.CopyFrom(request["id"], id.GetAllocator());
  }

  void operator()(Value &&result) const {
    Document response(rapidjson::kObjectType);
    LOG_TRACE("tag");
    response.AddMember("tinyrpc", "2.0", response.GetAllocator());
    response.AddMember("id", id.GetInt64(), response.GetAllocator());
    LOG_TRACE("tag");
    response.AddMember("result", result, response.GetAllocator());
    callback_(response);
  }

 private:
  mutable Document id;
  RpcDoneCallback callback_;
};

}  // namespace tinyrpc

#endif  // TINYRPC_UTIL_H_
