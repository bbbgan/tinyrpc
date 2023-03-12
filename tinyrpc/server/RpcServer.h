#ifndef TINYRPC_RPCSERVER_H_
#define TINYRPC_RPCSERVER_H_
#include <tinyrpc/server/BaseServer.h>
#include <tinyrpc/server/RpcService.h>
namespace tinyrpc {

class RpcServer : public BaseServer<RpcServer> {
 public:
  RpcServer(EventLoop* loop, const InetAddress& listen)
      : BaseServer(loop, listen) {}
  ~RpcServer() = default;

  void addService(std::string_view serviceName, RpcService* service);
  void handleRequest(const std::string& json, const RpcDoneCallback& done);

 private:
  void handleSingleRequest(Value& request, const RpcDoneCallback& done);
  void handleSingleNotify(Value& request);
  typedef std::unique_ptr<RpcService> RpcServeicPtr;
  typedef std::unordered_map<std::string_view, RpcServeicPtr> ServiceList;

  ServiceList services_;
};

}  // namespace tinyrpc

#endif  // TINYRPC_RPCSERVER_H_