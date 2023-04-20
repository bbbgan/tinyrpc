#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <tinyrpc/server/BaseServer.h>
#include <tinyrpc/server/RpcServer.h>
#include <tinyrpc/util.h>

#define kMaxMessageLen 65536

using namespace tinyrpc;
using namespace tinynet;
namespace tinyrpc {
template class BaseServer<RpcServer>;
}

template <typename ProtocolServer>
BaseServer<ProtocolServer>::BaseServer(EventLoop* loop,
                                       const InetAddress& listen)
    : server_(loop, listen, "RpcServer") {
  server_.setConnectionCallback(std::bind(&BaseServer::onConnection, this, _1));
  server_.setMessageCallback(
      std::bind(&BaseServer::onMessage, this, _1, _2, _3));
}

template <typename ProtocolServer>
void BaseServer<ProtocolServer>::onConnection(const TcpConnectionPtr& conn) {
  if (conn->connected()) {
    LOG_DEBUG("connection %s is [up]", conn->peerAddress().toIpPort().c_str());
  }
  else {
    LOG_DEBUG("connection %s is [down]",
              conn->peerAddress().toIpPort().c_str());
  }
}

template <typename ProtocolServer>
void BaseServer<ProtocolServer>::onMessage(const TcpConnectionPtr& conn,
                                           Buffer* buffer, Timestamp) {
  while (true) {
    const char* crlf = buffer->findCRLF();
    if (crlf == nullptr)
      break;
    if (crlf == buffer->peek()) {
      buffer->retrieve(2);
      break;
    }
    // length is INT32
    size_t headerLen = crlf - buffer->peek() + 2;
    if (headerLen != 6) {
      LOG_ERROR("BaseServer::onMessage : invalid message length : headerLen");
    }
    const void* data = buffer->peek();
    int32_t bigEnd32 = *static_cast<const int32_t*>(data);
    const int32_t contentLen = be32toh(bigEnd32);

    if (contentLen >= kMaxMessageLen)
      LOG_ERROR("message is too long  : length[%d] ", contentLen);

    if (buffer->readableBytes() < headerLen + contentLen)
      break;
    buffer->retrieve(headerLen);
    auto json = buffer->retrieveAsString(contentLen);
    LOG_INFO("recv %d bytes: %s", json.size(), json.c_str());
    convert().handleRequest(json, [conn, this](Value& response) {
      if (!response.IsNull()) {
        sendResponse(conn, response);
        LOG_TRACE("BaseServer::handleMessage() %s request success",
                  conn->peerAddress().toIpPort().c_str());
      }
      else {
        LOG_TRACE("BaseServer::handleMessage() %s notify success",
                  conn->peerAddress().toIpPort().c_str());
      }
    });
  }
}
template <typename ProtocolServer>
void BaseServer<ProtocolServer>::sendResponse(const TcpConnectionPtr& conn,
                                              const Value& response) {
  rapidjson::StringBuffer strbuf;
  rapidjson::Writer<rapidjson::StringBuffer> jWriter(strbuf);
  response.Accept(jWriter);
  std::string msg = strbuf.GetString();

  int32_t len = static_cast<int32_t>(msg.size()) + 2;
  int32_t be32 = htobe32(len);
  tinynet::net::Buffer buf;
  buf.append("\r\n");
  buf.append(msg.data(), msg.size());
  buf.append("\r\n");
  buf.prepend(&be32, sizeof be32);
  LOG_INFO("send : %s", buf.peek());
  conn->send(&buf);
}

template <typename ProtocolServer>
ProtocolServer& BaseServer<ProtocolServer>::convert() {
  return static_cast<ProtocolServer&>(*this);
}

template <typename ProtocolServer>
const ProtocolServer& BaseServer<ProtocolServer>::convert() const {
  return static_cast<const ProtocolServer&>(*this);
}
