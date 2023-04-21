#include <example/Arithmetic/ArithmeticClientStub.h>

#include <iostream>
#include <random>

using namespace tinyrpc;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution dis(0, 10);

void run(ArithmeticClientStub& client) {
  double lhs = dis(gen);
  double rhs = dis(gen);

  client.Add(lhs, rhs, [=](Value& response, bool isError, bool timeout) {
    // if (!isError) {
    //   std::cout << lhs << "/" << rhs << "=" << response.GetDouble() << "\n";
    // } else if (timeout) {
    //   std::cout << "timeout\n";
    // } else {
    //   std::cout << "response: " << response["message"].GetString() << ": "
    //             << response["data"].GetString() << "\n";
    // }
  });
}

int main() {
  Logger::setLogLevel(Logger::LogLevel::INFO);
  EventLoop loop;
  InetAddress addr(9999);
  ArithmeticClientStub client(&loop, addr);

  client.setConnectionCallback([&](const TcpConnectionPtr& conn) {
    if (conn->disconnected()) {
      loop.quit();
    } else {
      loop.runEvery(1s, [&] { run(client); });
    }
  });
  client.start();
  loop.loop();
}