/**
 * @file net_packets.hpp
 * @author CrackLewis (ghxx040406@163.com)
 * @brief
 * @version 0.1.0
 * @date 2023-05-13 19:51:05
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef NET_PACKETS_
#define NET_PACKETS_

#include "defines.hpp"

enum RequestType { GETCWD, OPERATION, DISCONN };

enum ResponseType { ON_CONNECT, ON_GETCWD, ON_OPERATION };

enum ResponseCode {
  OK = 200,
  NEED_LOGIN = 401,
  DENIED = 402,
  BUSY = 403,
  OPERATION_UNKNOWN = 404,
  OPERATION_FAILED = 405,
};

// �ͻ�������ͷ��
class RequestHeader {
 public:
  // ħ�����ƻ����ڼ���ð��Ƿ�Դ�������˳������׵Ŀͻ��ˡ�
  u64 magic_ = 76975721490249893ull;
  u64 token_ = 0;
  RequestType type_;
  u32 length_;  // ��header���ֵĳ���
};

// �������Ӧͷ��
class ResponseHeader {
 public:
  u64 magic_ = 76975721490249893ull;
  ResponseType type_;
  ResponseCode code_;
  u32 length_;  // ��header���ֵĳ���
};

class Request {
 public:
  ~Request() {
    if (text_) {
      delete[] text_;
      text_ = nullptr;
    }
  }

 public:
  RequestHeader header_;
  char* text_ = nullptr;
};

class Response {
 public:
  ~Response() {
    if (text_) {
      delete[] text_;
      text_ = nullptr;
    }
  }

 public:
  ResponseHeader header_;
  char* text_ = nullptr;
};

#endif