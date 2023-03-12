#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <tinyrpc/client/BaseClient.h>

#define kMaxMessageLen 65536
using namespace tinyrpc;

void BaseClient::sendCall(const TcpConnectionPtr& conn, Document& request,
                          const ResponseCallback& cb) {
  Value id;
  id.SetInt64(id_);
  request.AddMember("id", id, request.GetAllocator());
  callbacks_[id_] = cb;
  ++id_;
  sendRequest(conn, request);
}

void BaseClient::sendNotify(const TcpConnectionPtr& conn, Value& notify) {
  sendRequest(conn, notify);
}

void BaseClient::sendRequest(const TcpConnectionPtr& conn, Value& request) {
  rapidjson::StringBuffer strbuf;
  rapidjson::Writer<rapidjson::StringBuffer> jWriter(strbuf);
  request.Accept(jWriter);
  std::string_view msg = strbuf.GetString();

  int32_t len = static_cast<int32_t>(msg.size()) + 2;
  int32_t be32 = htobe32(len);
  tinynet::net::Buffer buf;
  buf.append("\r\n");
  buf.append(msg.data(), msg.size());
  buf.append("\r\n");
  buf.prepend(&be32, sizeof be32);
  LOG_INFO("send %d bytes :%s ", msg.size() + 2, msg.data());
  conn->send(&buf);
}

void BaseClient::onMessage(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp) {
  while (true) {
    const char* crlf = buffer->findCRLF();
    if (crlf == nullptr) break;
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

    if (buffer->readableBytes() < headerLen + contentLen) break;
    buffer->retrieve(headerLen);
    auto json = buffer->retrieveAsString(contentLen);
    handleResponse(json);
  }
}

void BaseClient::handleResponse(std::string& json) {
  LOG_INFO("recv: %s", json.c_str());
  Document response;
  response.Parse(json.c_str());
  LOG_INFO(" recv : %s\n", json.c_str());
  if (response.HasParseError()) {
    LOG_ERROR("RpcServer::handleRequest : Parse error");
    return;
  }
  switch (response.GetType()) {
    case rapidjson::kObjectType:
      handleSingleResponse(response);
      break;
    case rapidjson::kArrayType:
      LOG_ERROR("kArrayType is not support");
      break;
    default:
      LOG_ERROR("RpcServer::handleRequest : unknown type");
  }
}

void BaseClient::handleSingleResponse(Value& response) {
  auto id = response["id"].GetInt64();

  auto it = callbacks_.find(id);
  if (it == callbacks_.end()) {
    LOG_WARN("response %d not found in stub", id);
    return;
  }

  auto result = response.FindMember("result");
  if (result != response.MemberEnd()) {
    it->second(result->value, false, false);
  } else {
    auto error = response.FindMember("error");
    assert(error != response.MemberEnd());
    it->second(error->value, true, false);
  }
  callbacks_.erase(it);
}
