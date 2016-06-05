#include "message_handler.h"

namespace IslandSurvival {

MessageHandler* MessageHandler::m_pInstance = NULL;

MessageHandler* MessageHandler::Instance()
{
    if(NULL == m_pInstance)
    {
        m_pInstance = new MessageHandler;
    }
    return m_pInstance;
}


MessageHandler::MessageHandler()
{
    m_epollfd = -1;
}


MessageHandler::~MessageHandler()
{
    
}

int32_t MessageHandler::Init()
{
    m_epollfd = epoll_create(MAX_EPOLL_SIZE);
    listen_sock.Init();
    if(success != listen_sock.ReadyForListen(SERV_PORT))
    {
        PRINT_LOG("Init error , ReadyForListen fail!\n");
        return (fail);
    }
    AddToEpoll(listen_sock.GetFd());
    return int32_t(success);
}

int32_t AssignPlayerID()
{
    static int32_t user_id = 1;
    return user_id++;
}

int32_t MessageHandler::ReceiveMessage()
{
    int32_t message_count = 0;
    int32_t event_cnt = epoll_wait(m_epollfd, events, MAX_EPOLL_SIZE, 5);
    for(int32_t i = 0; i < event_cnt; i++)
    {
        int32_t fd = events[i].data.fd;
        if((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || !(events[i].events & EPOLLIN))
        {
			printf("%d %d %d\n", events[i].events & EPOLLERR, events[i].events & EPOLLHUP, events[i].events & EPOLLIN);
            if(m_connects.find(fd) != m_connects.end())
			{
				printf("fd:%d logout\n",fd);
                DeleteClientByFD(fd);
            }
            continue;
        }   

        if(fd == listen_sock.GetFd())  // Accept New Clients
        {
            TcpSocket* new_client = GetFreeTcpSocket();
            if(success == new_client->AcceptClient(listen_sock.GetFd()))
            {
                int connfd = new_client->GetFd();
                if(m_fd_to_user_id.find(connfd) != m_fd_to_user_id.end()) // login users
                {
					PRINT_LOG("ERROR:this connfd:%d has been logined\n",connfd);
				}
				else
				{
					AddToEpoll(connfd);
					m_connects[connfd] = new_client;
					HEARTBEAT->InitFD(connfd);
				}
            }
            else
            {
                free_conns.push(new_client);
            }
            continue;
        }
        if(m_connects.find(fd) == m_connects.end())
        {
            PRINT_LOG("ERROR:unknow error , connfd(%d) not exist\n" , fd);
            continue;
        }
        
        int32_t recv_ret = m_connects[fd]->RecvData();             // Recv Messages
        int32_t msg_len = 0;
        while(success == m_connects[fd]->get_one_message(recv_data_, MAX_MESSAGE_LEN, msg_len))
        {
            CMessage* message = ParseStrToMessage(recv_data_, msg_len);
            if(NULL != message)
            {
                ++message_count;
				if(MSG_CSHeartbeat == message->m_head.m_message_id)
				{
					REGISTER->ProcessRequest(fd, message);
				}
                else if(m_fd_to_user_id.find(fd) == m_fd_to_user_id.end())
                {
					if(MSG_CSLogin == message->m_head.m_message_id)
					{
						REGISTER->ProcessRequest(fd, message);
					}
					else if(MSG_CSRegist == message->m_head.m_message_id)
					{
						REGISTER->ProcessRequest(fd, message);
					}
                    else
					{
						PRINT_LOG("ERROR:user not login , don't process this message, connfd = %d\n", fd);
					}
				}
                else
                {
                    REGISTER->ProcessRequest(m_fd_to_user_id[fd], message);
                }
                if(NULL != message)
                {
					delete message;
                    message = NULL;
                }
            }
            else
            {
                PRINT_LOG("ERROR:ParseStrToMessage error\n");
            }
        }
        
        if(read_status_error == recv_ret) // 已断开连接
        {
            DeleteClientByFD(fd);
        }
    }
    return message_count;
}

int32_t MessageHandler::SendMessageToClient(int32_t user_id, CMessage& message)
{
    if(m_user_id_to_fd.find(user_id) == m_user_id_to_fd.end())
    {
        PRINT_LOG("ERROR:user not online ,  user_id = %d\n", user_id);
        return (fail);
    }
    int32_t fd = m_user_id_to_fd[user_id];
    int32_t ret = SendMessageByConnfd(fd, message);
	if(write_status_error == ret)
	{
		DeleteClientByFD(fd);
	}
    return int32_t(ret);
}

int32_t MessageHandler::SendMessageByConnfd(int32_t fd, CMessage& message)
{
    if(m_connects.find(fd) == m_connects.end())
    {
        return fail;
    }
    
    int32_t len = 0;
    message.encode(send_data_, len);
    int32_t ret = m_connects[fd]->SendData(send_data_, len);

    return int32_t(ret);
}

int32_t MessageHandler::PlayerLogin(int32_t fd, int32_t user_id)
{
    if(m_user_id_to_fd.find(user_id) == m_user_id_to_fd.end())
    {
        m_fd_to_user_id[fd] = user_id;
        m_user_id_to_fd[user_id] = fd;
		PRINT_LOG("Player Login: user_id = %d, connfd = %d\n", user_id, fd);
        return (success);
    }
    else
    {
        PRINT_LOG("player already logined!\n");
        return (fail);
    }
}

int32_t MessageHandler::PlayerLogout(int32_t user_id)
{
    if(m_user_id_to_fd.find(user_id) != m_user_id_to_fd.end())
    {
        int fd = m_user_id_to_fd[user_id];
        m_user_id_to_fd.erase(user_id);
		PERSONMANAGER->PersonLogout(user_id);
        if(m_fd_to_user_id.find(fd) != m_fd_to_user_id.end())
        {
            PRINT_LOG("Player Logout: user_id = %d, sockfd = %d\n", user_id, fd);
            m_fd_to_user_id.erase(fd);
        }
    }
    return int32_t(success);
}

void MessageHandler::DeleteClientByUserID(int32_t user_id)
{
	if(m_user_id_to_fd.find(user_id) != m_user_id_to_fd.end())
	{
		int fd = m_user_id_to_fd[user_id];
		DeleteClientByFD(fd);
	}
}

TcpSocket * MessageHandler::GetFreeTcpSocket()
{
    TcpSocket* ret = NULL;
    if(!free_conns.empty())
    { 
        ret = free_conns.front();
        free_conns.pop();
    }
    else
    {
        ret = new TcpSocket;
    }
    ret->Init();
    return ret;
}

void MessageHandler::AddToEpoll(int32_t fd)
{
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;
    if(0 != epoll_ctl(m_epollfd, EPOLL_CTL_ADD, fd, &event) )
    {
        PRINT_LOG("ERROR:Add to epoll failed , fd = %d, errno: %d\n", fd, errno);
    }
}

void MessageHandler::DeleteFromEpoll(int32_t fd)
{
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;
	if(0 != epoll_ctl(m_epollfd, EPOLL_CTL_DEL, fd, &event) )
    {
        PRINT_LOG("ERROR:Delete from epoll failed , connfd = %d, errno: %d\n", fd, errno);
    }
}

void MessageHandler::DeleteClientByFD(int32_t fd)
{
    if(m_connects.find(fd) == m_connects.end())
    {
        PRINT_LOG("ERROR:delete client error , fd(%d) not exist\n", fd);
        return;
    }
	HEARTBEAT->DeleteFD(fd);

    DeleteFromEpoll(fd);
	
	TcpSocket* client = m_connects[fd];
	client->close_socket();

    m_connects.erase(fd);
    free_conns.push(client);

    if(m_fd_to_user_id.find(fd) != m_fd_to_user_id.end())
    {
        int32_t user_id = m_fd_to_user_id[fd];
        PlayerLogout(user_id);
    }
}

CMessage* MessageHandler::ParseStrToMessage(const char * data, const int size)
{
    CMessage* ret = new CMessage;
    ret->m_proto = (google::protobuf::Message*)REGISTER->CreateMessage(ntohl(*(int*)&data[4]));
    if(success != ret->decode(data, size))
    {
        PRINT_LOG("cmessage decode error\n");
        delete ret;
        ret = NULL;
    }
    return ret;
}



}
