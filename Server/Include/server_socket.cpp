
#include "server_socket.h"
#include <netinet/tcp.h>
#include <poll.h>

namespace IslandSurvival{


int32_t set_non_block(int32_t fd)
{
    int32_t flag = 0;
    flag = fcntl(fd, F_GETFL, 0);
    if(flag < 0)
    {
        return error;
    }

    if(fcntl(fd, F_SETFL, flag | O_NONBLOCK | O_NDELAY) < 0)
    {
        return error;
    }
    return success;
}

int32_t set_socket_buffer_size(int32_t fd, int32_t size)
{
    if(fd < 0 || size <= 0)
    {
        return error;
    }
    
    int result = 0;
    int32_t recv_buf_size = size;
    socklen_t len = sizeof(recv_buf_size);
    result = setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &recv_buf_size, len);
    if(success != result)
    {
        return error;
    }
    
    int32_t send_buf_size = size;
    len = sizeof(send_buf_size);
    result = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &send_buf_size, len);
    if(success != result)
    {
        return error;
    }

    return success;
}

int32_t socket_addr2str(const struct sockaddr_in& addr, char* pszIpPort, int32_t size)
{
    if(NULL == pszIpPort || size <= 0)
    {
        return error;
    }
    
    char* ip = NULL;
    ip = inet_ntoa(addr.sin_addr);
    if(NULL == ip)
    {
        return error;
    }
    uint16_t port = ntohs(addr.sin_port);
    
    snprintf(pszIpPort, size - 1, "%s:%d", ip, port);
    pszIpPort[size - 1] = '\0';
    
    return success;
}

int32_t ip_str2addr(const char*ip_str, uint32_t& ip_addr)
{
    struct in_addr addr;
    if(NULL == ip_str || inet_pton(AF_INET, ip_str, &addr) <= 0)
    {
        return error;
    }
    ip_addr = (uint32_t)addr.s_addr;
    return success;
}

int32_t ip_addr2str(uint32_t ip_addr, char* ip_str, int32_t bufsize)
{
    if(NULL == ip_str || bufsize <= INET_ADDRSTRLEN)
    {
        return error;
    }
    if(NULL == inet_ntop(AF_INET, (const void*)&ip_addr, ip_str, (socklen_t)bufsize))
    {
        return error;
    }
    return 0;
}


}