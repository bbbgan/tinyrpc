#ifndef TINYRPC_BASESERVER_H_
#define TINYRPC_BASESERVER_H_

#include <tinyrpc/util.h>

namespace tinyrpc {

template <typename ProtocolServer>
class BaseServer : noncopyable 
{
 public:
  void setNumThread(int n) { server_.setThreadNum(n); }
  void start() { server_.start(); }

 protected:
  BaseServer(EventLoop* loop, const InetAddress& listen);
  ~BaseServer() = default;
 private:
  void onConnection(const TcpConnectionPtr& conn);
  void onMessage(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp);
  void sendResponse(const TcpConnectionPtr& conn, const Value& response);

  ProtocolServer& convert();
  const ProtocolServer& convert() const;
 private:
  TcpServer server_;
};

}  // namespace tinyrpc

#endif  // TINYRPC_BASESERVER_H_
