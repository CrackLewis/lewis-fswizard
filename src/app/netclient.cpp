/**
 * @file netclient.cpp
 * @author CrackLewis (ghxx040406@163.com)
 * @brief
 * @version 0.1.0
 * @date 2023-05-16 16:02:29
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <iostream>

#include "net_api.hpp"
#include "net_packets.hpp"

Response recv_resp(socket_t socket) {
  Response resp;
  i32 ret;
  if ((ret = Networking::recv(socket, (char*)(&resp.header_),
                              sizeof(resp.header_))) ==
      Networking::SOCKET_ERROR) {
    throw std::runtime_error("broken socket");
  }

  if (resp.header_.length_ != 0) {
    resp.text_ = new char[resp.header_.length_ + 1];
    if ((ret = Networking::recv(socket, resp.text_, resp.header_.length_)) ==
        Networking::SOCKET_ERROR) {
      throw std::runtime_error("broken socket");
    }
  }

  return resp;
}

void send_req(socket_t socket, RequestType type, const char* text, i32 len) {
  RequestHeader reqhead;
  reqhead.type_ = type;
  reqhead.length_ = len;
  // TODO: token

  if ((Networking::send(socket, (char*)&reqhead, sizeof(reqhead)) ==
       Networking::SOCKET_ERROR) ||
      (len > 0 &&
       Networking::send(socket, text, len) == Networking::SOCKET_ERROR)) {
    throw std::runtime_error("broken socket");
  }
}

void send_getcwd(socket_t socket) {
  send_req(socket, RequestType::GETCWD, nullptr, 0);
}

void send_operation(socket_t socket, const std::string& text) {
  send_req(socket, RequestType::OPERATION, text.data(), text.length() + 1);
}

void send_disconn(socket_t socket) {
  send_req(socket, RequestType::DISCONN, nullptr, 0);
}

int main() {
  socket_t client_socket = Networking::socket();
  if (client_socket == Networking::INVALID_SOCKET) {
    std::cerr << "Error: socket creation failed" << std::endl;
    return -1;
  }

  if (Networking::connect(client_socket, "127.0.0.1", 8888) ==
      Networking::SOCKET_ERROR) {
    std::cerr << "Error: connection failed" << std::endl;
    Networking::close(client_socket);
    return -1;
  }

  std::cout << "Info: Connected to the server. " << std::endl;
  Response resp = recv_resp(client_socket);
  if (resp.header_.code_ != ResponseCode::OK) {
    std::cerr << "Error: Connection failed, either because the server is busy "
                 "or because you're blocked. "
              << std::endl;
    Networking::close(client_socket);
    return -1;
  }

  while (1) {
    send_getcwd(client_socket);
    Response cwd_resp = recv_resp(client_socket);

    std::cout << cwd_resp.text_ << ":#";
    std::string line_;
    std::getline(std::cin, line_);

    if (line_ == "quit") break;

    send_operation(client_socket, line_);
    Response cwd_op = recv_resp(client_socket);

    std::cout << cwd_op.text_;
  }

  send_disconn(client_socket);
  Networking::close(client_socket);

  return 0;
}