#include "heartbeat.h"

namespace IslandSurvival{

Heartbeat* Heartbeat::m_pInstance = NULL;

Heartbeat* Heartbeat::Instance()
{
	if(NULL == m_pInstance)
	{
		m_pInstance = new Heartbeat;
	}
	return m_pInstance;
}


void Heartbeat::InitFD(int32_t fd)
{
	if(m_fd_to_sec.find(fd) != m_fd_to_sec.end())
	{
		PRINT_LOG("ERROR: fd is already connected\n");
		return;
	}
	int32_t nowsec = GetNowTimeSecond();
	std::pair<int32_t, int32_t> pa = std::make_pair(nowsec, fd);
	m_sec_fd.insert(pa);
	m_fd_to_sec[fd] = nowsec;
}

void Heartbeat::UpdateFD(int32_t fd)
{
	int32_t ret = DeleteFD(fd);
	if(fail == ret)
	{
		return;
	}
	int32_t nowsec = GetNowTimeSecond();
	std::pair<int32_t, int32_t> pa = std::make_pair(nowsec,fd);
	m_sec_fd.insert(pa);
	m_fd_to_sec[fd] = nowsec;
}

void Heartbeat::OnTick(int32_t nowsec)
{
	std::set< std::pair<int32_t, int32_t> >::iterator it;
	//printf("nowsec:%d\n", nowsec);
	for(it = m_sec_fd.begin(); it != m_sec_fd.end(); )
	{
		if(nowsec - (*it).first > 40)
		{
			int32_t tmp = (*it).second;
			it++;
			printf("user_id:%d does not send heartbeat\n", tmp);
			SERVERFRAME->DeleteClientByFD(tmp);
		}
		else 
		{
			break;
		}
	}
}

Heartbeat::Heartbeat()
{
	
}

Heartbeat::~Heartbeat()
{
	
}

int32_t Heartbeat::DeleteFD(int32_t fd)
{
	if(m_fd_to_sec.find(fd) == m_fd_to_sec.end())
	{
		PRINT_LOG("ERROR: Can't find this fd\n");
		return fail;
	}
	int32_t sec = m_fd_to_sec[fd];
	std::pair<int32_t, int32_t> pa = std::make_pair(sec, fd);
	if(m_sec_fd.find(pa) == m_sec_fd.end())
	{
		PRINT_LOG("ERROR: This fd second is wrong\n");
		return fail;
	}
	m_sec_fd.erase(pa);
	m_fd_to_sec.erase(fd);
	return success;
}

}
