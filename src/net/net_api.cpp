/**
 * @file net_api.cpp
 * @author CrackLewis (ghxx040406@163.com)
 * @brief
 * @version 0.1.0
 * @date 2023-05-13 20:01:42
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <cstdlib>
#include <stdexcept>

#include "net_api.hpp"

/**
 * @brief
 *
 * 因为各个平台下的网络操作不尽相同，所以这里尽量为计划支持的平台各准备了一套实现。
 */

#if defined(_WIN32) && defined(__MINGW32__)  // Windows POSIX

#include <winsock2.h>

static void __wsa_teardown() { WSACleanup(); };

static i32 __wsa_init__ = []() {
  atexit(__wsa_teardown);
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    throw std::runtime_error("WSA initialization failed");
  }
  return 0;
}();

socket_t Networking::socket(i32 af, i32 type, i32 protocol) {
  SOCKET ret = ::socket(af, type, protocol);
  return ret;
}

void Networking::close(socket_t socket) { ::closesocket(socket); }

i32 Networking::send(socket_t socket, const char* buf, i32 len, i32 flags) {
  return ::send(socket, buf, len, flags);
}

i32 Networking::recv(socket_t socket, char* buf, i32 len, i32 flags) {
  return ::recv(socket, buf, len, flags);
}

i32 Networking::bind(socket_t socket, const char* addr, i32 port) {
  sockaddr_in name;
  name.sin_family = AF_INET;
  name.sin_port = port;
  name.sin_addr.s_addr = ::inet_addr(addr);
  return ::bind(socket, (sockaddr*)&name, sizeof(name));
}

i32 Networking::listen(socket_t socket, i32 backlog) {
  return ::listen(socket, backlog);
}

socket_t Networking::accept(socket_t socket, char* pname, i32* pport) {
  sockaddr_in name;
  u32 _;
  socket_t retval = ::accept(socket, (sockaddr*)&name, (i32*)&_);
  _ = name.sin_port;
  sprintf(pname, "%d.%d.%d.%d", _ >> 24, (_ >> 16) & 0xffu, (_ >> 8) & 0xffu,
          _ & 0xffu);
  *pport = _;
  return retval;
}

i32 Networking::connect(socket_t socket, const char* pname, i32 port) {
  sockaddr_in name;
  name.sin_family = AF_INET;
  name.sin_port = port;
  name.sin_addr.s_addr = ::inet_addr(pname);
  return ::connect(socket, (sockaddr*)&name, sizeof(name));
}

#elif defined(_WIN32)  // Windows 非POSIX

#elif defined(__linux__) || defined(__linux)  // Linux
#endif