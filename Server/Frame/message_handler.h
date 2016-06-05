#ifndef _MESSAGE_HANDLER_H__
#define _MESSAGE_HANDLER_H__

#include "../Include/server_base.h"
#include "../Include/server_tcpsocket.h"
#include "../Include/CMessage.h"
#include "../Frame/message_processor_register.h"
#include "../Include/server_tcpsocket.h"
#include "person_manager.h"
#include "heartbeat.h"
namespace IslandSurvival {


#define  MAX_EPOLL_SIZE  1024
#define  SERV_PORT 20000
#define  MAX_MESSAGE_LEN  4096


class MessageHandler
{
public:
    static MessageHandler* Instance();

    int32_t Init();

    int32_t ReceiveMessage();

    int32_t SendMessageToClient(int32_t user_id, CMessage& message); // 因为需要修改消息头包长度，所以不能用const
	int32_t SendMessageByConnfd(int32_t fd, CMessage& message);

	int32_t PlayerLogin(int32_t fd, int32_t user_id);    
	int32_t PlayerLogout(int32_t user_id); //
	
	void DeleteClientByUserID(int32_t user_id);
	void DeleteClientByFD(int32_t fd);

private:
    MessageHandler(); 
	~MessageHandler();
    TcpSocket* GetFreeTcpSocket();

    void AddToEpoll(int32_t fd);
    void DeleteFromEpoll(int32_t fd);
   
    CMessage* ParseStrToMessage(const char* data, const int size);

    static MessageHandler* m_pInstance;

    TcpSocket listen_sock;
    std::map<int32_t, TcpSocket*> m_connects; // key : fd
    std::map<int32_t, int32_t> m_user_id_to_fd;   // key: user_id, value : connfd  ; just logined users
    std::map<int32_t, int32_t> m_fd_to_user_id;   // key: connfd , value : user_id

    std::queue<TcpSocket*> free_conns;        // 空闲的连接
    int32_t m_epollfd;
    struct epoll_event events[MAX_EPOLL_SIZE];

    char recv_data_[MAX_MESSAGE_LEN];
    char send_data_[MAX_MESSAGE_LEN];

};

#define SERVERFRAME MessageHandler::Instance()

}



#endif
