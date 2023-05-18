/**
 * @file testserver.cpp
 * @author CrackLewis (ghxx040406@163.com)
 * @brief
 * @version 0.1.0
 * @date 2023-05-16 16:01:28
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <iostream>
#include <map>
#include <mutex>
#include <thread>

#include "net_api.hpp"
#include "net_packets.hpp"

Request recv_req(socket_t socket) {
  Request req;
  i32 ret;
  if ((ret = Networking::recv(socket, (char*)(&req.header_),
                              sizeof(req.header_))) ==
      Networking::SOCKET_ERROR) {
    throw std::runtime_error("broken socket");
  }

  if (req.header_.length_ != 0) {
    req.text_ = new char[req.header_.length_ + 1];
    if ((ret = Networking::recv(socket, req.text_, req.header_.length_)) ==
        Networking::SOCKET_ERROR) {
      throw std::runtime_error("broken socket");
    }
  }

  return req;
}

void send_resp(socket_t socket, ResponseType type, ResponseCode code,
               const char* text, i32 length) {
  ResponseHeader resphead;
  resphead.code_ = code;
  resphead.type_ = type;
  resphead.length_ = length;

  if ((Networking::send(socket, (char*)&resphead, sizeof(resphead)) ==
       Networking::SOCKET_ERROR) ||
      (length > 0 &&
       Networking::send(socket, text, length) == Networking::SOCKET_ERROR)) {
    throw std::runtime_error("broken socket");
  }
}

void greet(socket_t socket, ResponseCode code) {
  send_resp(socket, ResponseType::ON_CONNECT, code, nullptr, 0);
}

void reply_getcwd(socket_t socket, ResponseCode code, const std::string& text) {
  send_resp(socket, ResponseType::ON_GETCWD, code, text.data(),
            text.length() + 1);
}

void reply_operation(socket_t socket, ResponseCode code,
                     const std::string& text) {
  send_resp(socket, ResponseType::ON_OPERATION, code, text.data(),
            text.length() + 1);
}

int main() {
  socket_t listen_socket = Networking::socket();
  if (listen_socket == Networking::INVALID_SOCKET) {
    std::cerr << "Error: socket creation failed. " << std::endl;
    return -1;
  }

  if (Networking::bind(listen_socket, "127.0.0.1", 8888) ==
      Networking::SOCKET_ERROR) {
    std::cerr << "Error: binding failed. " << std::endl;
    Networking::close(listen_socket);
    return -1;
  }

  if (Networking::listen(listen_socket, INT_MAX) == Networking::SOCKET_ERROR) {
    std::cerr << "Error: listening failed" << std::endl;
    Networking::close(listen_socket);
    return -1;
  }

  std::cout << "Info: listening. " << std::endl;

  std::map<socket_t, i32> sessions;
  std::mutex mtx_sessions;
  auto client_routine = [&](socket_t csock) {
    // 会话数量检查。
    mtx_sessions.lock();
    if (sessions.size() >= 4u) {
      greet(csock, ResponseCode::BUSY);
      mtx_sessions.unlock();
      return;
    }
    greet(csock, ResponseCode::OK);
    sessions[csock] = 1;
    mtx_sessions.unlock();

    bool is_alive = true;

    while (is_alive) {
      Request req = recv_req(csock);
      switch (req.header_.type_) {
        case RequestType::GETCWD: {
          reply_getcwd(csock, ResponseCode::OK, "mahosojo");
          break;
        }
        case RequestType::OPERATION: {
          std::string cli_seg = "", cli_cmd;
          std::vector<std::string> cli_args;
          bool has_cmd = false;
          char ch;

          for (i32 idx = 0; idx < req.header_.length_ - 1; ++idx) {
            ch = req.text_[idx];
            if (ch == ' ') {
              if (cli_seg.length() == 0) continue;
              if (!has_cmd) {
                has_cmd = true;
                cli_cmd = cli_seg;
              } else
                cli_args.emplace_back(cli_seg);
              cli_seg = "";
            } else {
              cli_seg += ch;
            }
          }
          if (cli_seg.length() > 0) {
            if (!has_cmd)
              cli_cmd = cli_seg;
            else
              cli_args.emplace_back(cli_seg);
          }

          std::string result = "Argsplit result of " + cli_cmd + ": \n";
          for (const auto& arg : cli_args) {
            result += arg + '\n';
          }

          reply_operation(csock, ResponseCode::OK, result);
          break;
        }
        case RequestType::DISCONN: {
          is_alive = false;
          break;
        }
      }
    }  // while (is_alive)

    // 退出前，注销会话。
    mtx_sessions.lock();
    sessions.erase(csock);
    mtx_sessions.unlock();
    Networking::close(csock);
  };

  while (1) {
    char client_name[25];
    i32 client_port;
    socket_t client_socket =
        Networking::accept(listen_socket, client_name, &client_port);
    if (client_socket == Networking::INVALID_SOCKET) {
      std::cerr << "Error: accept failed. " << std::endl;
      Networking::close(listen_socket);
      return -1;
    }

    std::thread t(client_routine, client_socket);
    t.detach();
  }

  Networking::close(listen_socket);

  return 0;
}