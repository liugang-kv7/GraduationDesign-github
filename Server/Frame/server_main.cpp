#include "../Include/server_tcpsocket.h"
#include "../Include/CMessage.h"
#include "message_processor_register.h"
#include "message_handler.h"
#include "../proto_out/proto_islandsurvival.h"
#include "room_manager.h"
#include "login.h"
#include "heartbeat.h"
using namespace IslandSurvival;




void RunOnTick(int32_t& presec)
{
    ROOMMANAGER->OnTick();
	int32_t nowsec = GetNowTimeSecond();
	if(nowsec - presec < 40)
	{
		return;
	}
	presec = nowsec;
	HEARTBEAT->OnTick(nowsec);
}


int main()
{ 
    if(success != SERVERFRAME->Init())
    {
        return 0;
    }
	if(success != LOGIN->Init())
	{
		return 0;
	}
	//int user_id;
	//LOGIN->NewUserRegist("haha","123", user_id);
	//printf("%d\n",user_id);
	//LOGIN->LoginRequest("zhu","123");
	int32_t sec = GetNowTimeSecond();
    for(;;)
    {
        //struct timeval tvTimeNow;
        //gettimeofday(&tvTimeNow, NULL);
        RunOnTick(sec);

        SERVERFRAME->ReceiveMessage();  
    }
    return 0;
}


