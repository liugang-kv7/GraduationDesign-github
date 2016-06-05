#pragma once
#ifndef _SERVER_SOCKET_H
#define _SERVER_SOCKET_H

#include "server_base.h"

namespace IslandSurvival{

int32_t set_non_block(int32_t fd);

//����socket���ӷ��ͺͽ��ܻ�������С��������connect��listenǰ
int32_t set_socket_buffer_size(int32_t fd, int32_t size);

int32_t socket_addr2str(const struct sockaddr_in& addr, char* pszIpPort, int32_t size);

int32_t ip_str2addr(const char*ip_str, uint32_t& ip_addr);

int32_t ip_addr2str(uint32_t ip_addr, char* ip_str, int32_t bufsize);

}

#endif