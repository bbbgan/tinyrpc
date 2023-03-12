#ifndef TINYRPC_BASECLIENT_H_
#define TINYRPC_BASECLIENT_H_

#include <tinyrpc/util.h>

#include <functional>

namespace tinyrpc {

using ResponseCallback =
    std::function<void(Value&, bool isError, bool isTimeout)>;

class BaseClient : noncopyable {
 public:
  BaseClient(EventLoop* loop, const InetAddress& serverAddress)
      : id_(0), client_(loop, serverAddress) {
    client_.setMessageCallback(std::bind(&BaseClient::onMessage, this, _1, _2, _3));
  }

  void start() { client_.startconnect(); }

  void setConnectionCallback(const ConnectionCallback& cb) {
    client_.setConnectionCallback(cb);
  }

  void sendCall(const TcpConnectionPtr& conn, Document& request,
                const ResponseCallback& cb);

  void sendNotify(const TcpConnectionPtr& conn, Value& notify);

 private:
  void onMessage(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp);
  void handleResponse(std::string& json);
  void handleSingleResponse(Value& response);
  void sendRequest(const TcpConnectionPtr& conn, Value& request);

 private:
  typedef std::unordered_map<int64_t, ResponseCallback> Callbacks;
  int64_t id_;
  Callbacks callbacks_;
  TcpClient client_;
};

}  // namespace tinyrpc

#endif  // TINYRPC_BASECLIENT_H_
