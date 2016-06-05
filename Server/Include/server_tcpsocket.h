#pragma once
#ifndef _SERVER_TCPSOCKET_H
#define _SERVER_TCPSOCKET_H

#include "server_socket.h"

namespace IslandSurvival{

enum
{
    recv_buffer_size = 0x30000,
    send_buffer_size = 0x30000,
    listen_que_size = 1024,
};
enum
{
    read_status_success,
    read_status_not_finish,
    read_status_error,

    write_status_success,
    write_status_not_finish,
    write_status_error,
};

class TcpSocket
{
public:
	// 构造函数
    TcpSocket();
	
	// 析构函数
    ~TcpSocket();
	
	// 初始化
    void Init();
	
	// 准备进入监听状态
    int32_t ReadyForListen(uint16_t port);
	
	// 请求连接服务器
	int32_t ConnectToServer(const char* ip, uint16_t port, int32_t msecond);
	
	// 接受来自客户端的连接
    int32_t AcceptClient(int32_t listenfd);
	
	// 接收数据
    int32_t RecvData();
	
	// 发送数据
    int32_t SendData(const char* data, int32_t size);
	
	// 从缓冲区分隔出一个消息协议
    int32_t get_one_message(char* data, int32_t max_size, int32_t& size);
	
	// 获得套接字描述符
    int32_t GetFd();
	
	// 关闭套接字
    void close_socket();

private:
	
    int32_t m_iSockfd;							// 套接字描述符
    struct sockaddr_in sock_addr;				// 对端套接字地址

    int32_t m_recv_head;						// 接收缓冲区头指针
    int32_t m_recv_tail;						// 接收缓冲区尾指针
    char m_recv_buffer[recv_buffer_size+1]; 	// 接收缓冲区空间

    int32_t m_send_head;						// 发送缓冲区头指针
    int32_t m_send_tail;						// 发送缓冲区尾指针
    char m_send_buffer[send_buffer_size+1];		// 发送缓冲区空间

	bool m_is_using;							// 是否使用标志
};

}

#endif //_SERVER_TCPSOCKET_H
