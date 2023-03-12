#include <example/Arithmetic/ArithmeticServiceStub.h>
#include <tinyrpc/server/RpcServer.h>
#include <tinyrpc/server/RpcService.h>
#include <tinyrpc/util.h>

using namespace tinyrpc;

class ArithmeticService : public ArithmeticServiceStub<ArithmeticService> {
 public:
  explicit ArithmeticService(RpcServer& server)
      : ArithmeticServiceStub(server) {}

  void Add(double lhs, double rhs, const UserDoneCallback& cb) {
    cb(Value(lhs + rhs));
  }

  void Sub(double lhs, double rhs, const UserDoneCallback& cb) {
    cb(Value(lhs - rhs));
  }

  void Mul(double lhs, double rhs, const UserDoneCallback& cb) {
    cb(Value(lhs * rhs));
  }

  void Div(double lhs, double rhs, const UserDoneCallback& cb) 
  {
    cb(Value(lhs / rhs));
  }
};

int main() {
  Logger::setLogLevel(Logger::LogLevel::TRACE);
  EventLoop loop;
  InetAddress addr(9999);

  RpcServer rpcServer(&loop, addr);
  ArithmeticService service(rpcServer);

  rpcServer.start();
  loop.loop();
}