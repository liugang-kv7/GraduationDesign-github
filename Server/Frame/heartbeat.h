#ifndef _HEARTBEAT_H__
#define _HEARTBEAT_H__

#include "../Include/server_base.h"
#include "message_handler.h"
namespace IslandSurvival{


class Heartbeat
{
public:
	static Heartbeat* Instance();
	void InitFD(int32_t fd);
	void UpdateFD(int32_t fd);
	int32_t DeleteFD(int32_t fd);
	void OnTick(int32_t nowsec);
private:
	static Heartbeat* m_pInstance;
	Heartbeat();
	~Heartbeat();

	std::set< std::pair<int32_t, int32_t> >m_sec_fd;
	std::map<int32_t, int32_t>m_fd_to_sec;
};

#define HEARTBEAT Heartbeat::Instance()

	
}


#endif
