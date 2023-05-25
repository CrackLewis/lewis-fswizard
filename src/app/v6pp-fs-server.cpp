/**
 * @file v6pp-fs-server.cpp
 * @author CrackLewis (ghxx040406@163.com)
 * @brief
 * @version 0.1.0
 * @date 2023-05-13 16:32:31
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <cstdio>
#include <ctime>
#include <exception>
#include <iostream>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>

#include "argparse.hpp"
#include "net_api.hpp"
#include "net_packets.hpp"
#include "v6pp_vfs.hpp"

#define LOGGING(...)                                                   \
  do {                                                                 \
    time_t lt = time(0);                                               \
    tm* localt = localtime(&lt);                                       \
    printf("[%04d-%02d-%02d %02d:%02d:%02d] ", localt->tm_year + 1900, \
           localt->tm_mon + 1, localt->tm_mday, localt->tm_hour,       \
           localt->tm_min, localt->tm_sec);                            \
    printf("[%12s:%4d] ", __FILE__, __LINE__);                         \
    printf(__VA_ARGS__);                                               \
    putchar('\n');                                                     \
  } while (0)

Request recv_req(socket_t socket) {
  Request req;
  i32 ret;
  if ((ret = Networking::recv(socket, (char*)(&req.header_),
                              sizeof(req.header_))) ==
      Networking::SOCKET_ERROR) {
    throw std::exception();
  }

  if (req.header_.length_ != 0) {
    req.text_ = new char[req.header_.length_ + 1];
    if ((ret = Networking::recv(socket, req.text_, req.header_.length_)) ==
        Networking::SOCKET_ERROR) {
      throw std::exception();
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

struct UserProfile {
  std::string cwd_;
  std::vector<u32> inode_stack_;
};

int main(int argc, char** argv) {
  std::string image_path;
  int listen_port = 7777;
  int max_sessions = 4;
  if (1) {
    ArgParseRule rule;
    rule.add_rule("image", aptype_is_str | apshow_strict);
    rule.add_rule("port", aptype_is_uint | apshow_once);
    rule.add_rule("max_sessions", aptype_is_uint | apshow_once);

    std::map<std::string, std::string> result;
    if (rule.accept(argc, argv, &result)) {
      std::cout << "Error: " << rule.error() << std::endl;
      return -1;
    }
    image_path = result["image"];
    if (result.count("port")) listen_port = atoi(result["port"].data());
    if (result.count("max_sessions"))
      max_sessions = atoi(result["max_sessions"].data());
  }

  // 先搭建监听端口。
  socket_t listen_socket = Networking::socket();
  if (listen_socket == Networking::INVALID_SOCKET) {
    LOGGING("Error: socket creation failed.");
    return -1;
  }

  if (Networking::bind(listen_socket, "0.0.0.0", listen_port) ==
      Networking::SOCKET_ERROR) {
    LOGGING("Error: binding failed. ");
    Networking::close(listen_socket);
    return -1;
  }

  // 搭建互斥设施。
  // 由于本机VFS是不支持多用户的，这里做外部改进。
  std::map<socket_t, UserProfile> users;
  std::mutex mtx_users;

  // 启动端口监听。
  if (Networking::listen(listen_socket, INT_MAX) == Networking::SOCKET_ERROR) {
    LOGGING("Error: listening failed");
    Networking::close(listen_socket);
    return -1;
  }

  // 启动文件系统。
  v6pp::FileSystemConfig config;
  std::string fsbuf;
  config.disk_path_ = image_path;
  // TODO: asker and speaker
  config.speaker_ = [&](const std::string& text) { (fsbuf += text) += '\n'; };
  v6pp::FileSystem fs(config);
  LOGGING("Info: file system initialized.");

  LOGGING("Info: listening on %s:%d", "0.0.0.0", listen_port);

  // 当前并没有分开生产者和消费者线程。
  auto client_routine = [&](socket_t csock) {
    mtx_users.lock();
    if (users.size() >= max_sessions) {
      greet(csock, ResponseCode::BUSY);
      mtx_users.unlock();
      Networking::close(csock);
      return;
    }
    greet(csock, ResponseCode::OK);
    users.insert(std::make_pair(csock, UserProfile()));
    users[csock].cwd_ = "/";
    mtx_users.unlock();

    try {
      bool is_alive = true;
      while (is_alive) {
        Request req = recv_req(csock);
        LOGGING("Info: received request from socket %lld.", csock);

        switch ((req.header_.type_)) {
          case RequestType::GETCWD: {
            reply_getcwd(csock, ResponseCode::OK, users[csock].cwd_);
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

            if (cli_cmd == "help") {
              fsbuf = "Available commands: \n";

              fsbuf += "quit\n";
#define COMMAND(comm) (fsbuf += #comm) += '\n';
#include "commands.inc"
#undef COMMAND
              reply_operation(csock, ResponseCode::OK, fsbuf);
              break;
            }
#define COMMAND(comm)                                                        \
  if (cli_cmd == #comm) {                                                    \
    fs.cd({users[csock].cwd_});                                              \
    fsbuf = "";                                                              \
    int ret = fs.comm(cli_args);                                             \
    reply_operation(csock,                                                   \
                    ret ? ResponseCode::OPERATION_FAILED : ResponseCode::OK, \
                    fsbuf);                                                  \
    users[csock].cwd_ = fs._getcwd();                                        \
    break;                                                                   \
  }
#include "commands.inc"
#undef COMMAND

            reply_operation(csock, ResponseCode::OPERATION_UNKNOWN,
                            "Unknown command. Type \"help\" for help.\n");
            break;
          }  // case OPERATION
          case RequestType::DISCONN: {
            is_alive = false;
            break;
          }
        }
      }  // while (is_alive)
    } catch (std::exception& e) {
      // do nothing.
    }
    LOGGING("Info: %lld disconnected.", csock);

    mtx_users.lock();
    users.erase(csock);
    mtx_users.unlock();
    Networking::close(csock);
  };

  while (1) {
    char client_name[25];
    i32 client_port;
    socket_t client_socket =
        Networking::accept(listen_socket, client_name, &client_port);
    if (client_socket == Networking::INVALID_SOCKET) {
      LOGGING("Error: accept failed");
      Networking::close(listen_socket);
      return -1;
    }
    LOGGING("Info: %lld connected. ", client_socket);

    std::thread t(client_routine, client_socket);
    t.detach();
  }

  LOGGING("Info: server closed.");
  Networking::close(listen_socket);

  return 0;
}