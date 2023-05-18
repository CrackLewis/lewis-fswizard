/**
 * @file net_api.hpp
 * @author CrackLewis (ghxx040406@163.com)
 * @brief
 * @version 0.1.0
 * @date 2023-05-13 19:54:24
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef NET_API_HPP_
#define NET_API_HPP_

#include <functional>
#include <vector>

#include "defines.hpp"
#include "net_packets.hpp"

/**
 * @brief
 *
 * ����м�����������ṩ��ƽ̨�޹ص�������֧�֡�
 *
 * Windowsɶ����֧�ֵ�APIȷʵ�������ˡ�
 */

using socket_t = u64;

class Networking {
 public:
  static constexpr socket_t INVALID_SOCKET = (u64)(~0);
  static constexpr i32 SOCKET_ERROR = -1;

 public:
  // socket������2,1,6��ʾIPv4Э���壬��ʽ�׽��֣�TCPЭ��
  static socket_t socket(i32 af = 2, i32 type = 1, i32 protocol = 6);
  static void close(socket_t socket);

  // ͨ�÷������շ�����
  static i32 send(socket_t socket, const char* buf, i32 len, i32 flags = 0);
  static i32 recv(socket_t socket, char* buf, i32 len, i32 flags = 0);

  // ����˷�����
  static i32 bind(socket_t socket, const char* addr, i32 port);
  static i32 listen(socket_t socket, i32 backlog);
  static socket_t accept(socket_t socket, char* pname, i32* pport);

  // �ͻ��˷�����
  static i32 connect(socket_t socket, const char* pname, i32 port);
};

#endif