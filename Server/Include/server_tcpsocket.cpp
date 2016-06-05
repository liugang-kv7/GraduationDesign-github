
#include "server_tcpsocket.h"
#include <sys/socket.h>
#include <netinet/tcp.h>

namespace IslandSurvival {

TcpSocket::TcpSocket()
{
    Init();
}

void TcpSocket::Init()
{
    m_iSockfd = -1;
    m_recv_head = 0;
    m_recv_tail = 0;
    m_send_head = 0;
    m_send_tail = 0;
    m_is_using = false;
}

TcpSocket::~TcpSocket()
{
    close_socket();
}

void TcpSocket::close_socket()
{
    if(m_is_using == true)
    {
        close(m_iSockfd);
        m_is_using = false;
    }
}

int32_t TcpSocket::GetFd()
{
    return m_iSockfd;
}

int32_t TcpSocket::ReadyForListen(uint16_t port)
{
    m_iSockfd = socket(AF_INET, SOCK_STREAM, 0);
    int32_t flags = 1;
    setsockopt(m_iSockfd, IPPROTO_TCP, TCP_NODELAY, &flags, sizeof(flags));
    if(success != setsockopt(m_iSockfd, SOL_SOCKET, SO_REUSEADDR, &flags, (int32_t)sizeof(flags)))
    {
        printf("Set reuse error\n");
        return error;
    }

    struct sockaddr_in servaddr;
    memset(&servaddr, 0x0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    socklen_t socklen = sizeof(servaddr);
    if(success != bind(m_iSockfd, (const sockaddr*)&servaddr, socklen))
    {
        printf("Bind error\n");
        return error;
    }
    if(success != listen(m_iSockfd, listen_que_size))
    {
        printf("Listen error\n");
        return error;
    }
    if(success != set_non_block(m_iSockfd))
    {
        printf("Set non_block error\n");
        return error;
    }
	m_is_using = true;
    return success;
}

int32_t TcpSocket::ConnectToServer(const char* ip, uint16_t port, int32_t msecond)
{
    m_iSockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in servaddr;
    memset(&servaddr, 0x0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ip);
    servaddr.sin_port = htons(port);
    socklen_t socksize = (socklen_t)sizeof(servaddr);

    if(success != connect(m_iSockfd, (const sockaddr*)&servaddr, socksize))
    {
        return error;
    }
    if(success != set_non_block(m_iSockfd))
    {
        return error;
    }
    return success;
}

int32_t TcpSocket::AcceptClient(int32_t listenfd)
{
    socklen_t socklen = sizeof(sock_addr);
    m_iSockfd = accept(listenfd, (struct sockaddr*)&sock_addr, &socklen);
    if(m_iSockfd < 0)
    {
		PRINT_LOG("ERROR: accept error : %d\n", errno);
        return error;
    }
    if(success != set_non_block(m_iSockfd))
    {
        return error;
    }
	m_is_using = true;
    return success;
}

#define TryMoveReceDataAhead do{ \
    if (m_recv_tail == (int32_t)recv_buffer_size) \
            {  \
            if (m_recv_head > 0) \
                        {  \
                memmove(&m_recv_buffer[0], &m_recv_buffer[m_recv_head], int32_t(m_recv_tail - m_recv_head)); \
                m_recv_tail -= m_recv_head; \
                m_recv_head = 0; \
                        } \
                        else \
            { \
                return (read_status_not_finish); \
            } \
            } \
}while(0)

int32_t TcpSocket::RecvData()
{
    if(false == m_is_using)
    {
        return read_status_error;
    }

    if(m_recv_tail == m_recv_head)
    {
        m_recv_head = 0;
        m_recv_tail = 0;
    }

    int32_t received_byte = 0;
    do
    {
        TryMoveReceDataAhead;
        received_byte = recv(m_iSockfd, &m_recv_buffer[m_recv_tail], int32_t(recv_buffer_size - m_recv_tail), 0);
        //printf("recv: fd = %d, len = %d , errno = %d\n", m_iSockfd, received_byte, errno);
        if(received_byte > 0) // ok
        {
            m_recv_tail += received_byte;
        }
        else if(received_byte == 0 || EAGAIN != errno)  // disconnected
        {
            //close_socket();
            return (read_status_error);
        }

    } while(received_byte > 0);

    return (read_status_success);
}

int32_t TcpSocket::SendData(const char* data, int32_t size)
{
    //printf("should send len = %d\n", size);
    assert(size <= send_buffer_size);
    if(NULL == data || size <= 0)
    {
        return (write_status_success);
    }
    if(false == m_is_using)
    {
        return (write_status_error);
    }

    char* pszDataPtr = &m_send_buffer[m_send_head];

    int32_t sended_byte = 0;
    while((m_send_tail - m_send_head) > 0)
    {
        sended_byte = send(m_iSockfd, pszDataPtr, (size_t)(m_send_tail - m_send_head), 0);
		//printf("send buffer: sended_byte: %d, errno: %d, EAGAIN:%d, EINTR:%d\n", sended_byte, errno, EAGAIN, EINTR);
        if(sended_byte > 0)
        {
            m_send_head += sended_byte;
            pszDataPtr += sended_byte;
        }
        else if(EAGAIN != errno)
        {
            //close_socket();
            return (write_status_error);
        }
        else
        {
            memmove(&m_send_buffer[0], &m_send_buffer[m_send_head], (size_t)(m_send_tail - m_send_head));
            m_send_tail -= m_send_head;
            m_send_head = 0;

            if(send_buffer_size - m_send_tail >= size)
            {
                memcpy(&m_send_buffer[m_send_tail], data, (size_t)size);
                m_send_tail += size;
                return (write_status_success);
            }
            else
            {
                return (write_status_not_finish);
            }
        }
    }

    if(m_send_head == m_send_tail)
    {
        m_send_head = 0;
        m_send_tail = 0;
    }
    else
    {
        return (write_status_success);
    }

    int32_t data_remaind_byte = (int32_t)size;
    const char* pDataPtr = data;

    while(data_remaind_byte > 0)
    {
        sended_byte = send(m_iSockfd, pDataPtr, (size_t)data_remaind_byte, 0);
		//printf("send message: sended_byte: %d, errno: %d, EAGAIN:%d, EINTR:%d\n", sended_byte, errno, EAGAIN, EINTR);
        if(sended_byte > 0)
        {
            pDataPtr += sended_byte;
            data_remaind_byte -= sended_byte;
        }
        else if(EAGAIN != errno)
        {
            //close_socket();
            return (write_status_error);
        }
        else if(send_buffer_size - m_send_tail >= data_remaind_byte)
        {
            memcpy(&m_send_buffer[m_send_tail], pDataPtr, (size_t)data_remaind_byte);
            m_send_tail += data_remaind_byte;
            return (write_status_success);
        }
        else
        {
            // GG 缓存不下， 前面又发了不完整的包 ,没设计处理方法，踢下线吧
            //close_socket();
            return (write_status_error);
        }
    }
    return (write_status_success);
}


int32_t TcpSocket::get_one_message(char* data, int32_t max_size, int32_t& size)
{
    if(NULL == data)
    {
        return fail;
    }
    if(m_recv_tail - m_recv_head <= (int32_t)sizeof(int32_t))
    {
        return fail;
    }
    int32_t msg_size = ntohl(*(int32_t*)&m_recv_buffer[m_recv_head]);
    if(msg_size <= 0)
    {
        return error;
    }
    if(msg_size > m_recv_tail - m_recv_head || msg_size > max_size)
    {
        return fail;
    }
    size = msg_size;
    memcpy(data, &m_recv_buffer[m_recv_head], size);
    m_recv_head += size;
    return  success;
}


}
