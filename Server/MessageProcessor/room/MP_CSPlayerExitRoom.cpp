#include "../../Frame/processor_base.h"
#include "../../Include/server_base.h"
#include "../../proto_out/proto_islandsurvival.h"
#include "../../Frame/message_processor_register.h"
#include "../../Frame/room.h"
#include "../../Frame/room_manager.h"

namespace IslandSurvival {

class MP_CSPlayerExitRoom : public MessageProcessorBase
{
public:
    virtual int32_t ProcessRequest(int32_t user_id, const CMessage& mess)
    {	
		printf("user_id:%d want to exit room\n", user_id);
		SCPlayerExitRoom* proto = new SCPlayerExitRoom;
		CMessage message;
		message.m_head.m_message_id = MSG_SCPlayerExitRoom;
		message.m_head.m_framenum = -1;
		message.m_proto = proto;

		Room* room = ROOMMANAGER->GetRoomByUserID(user_id);
		if(room_status_waiting == room->GetGameStatus())
		{
			int32_t ret = ROOMMANAGER->DeletePlayerFromRoom(user_id);
			if(ret == success) proto->set_result(1);
			else proto->set_result(0);
		}
		else
		{
			proto->set_result(0);
		}
		
		room->SendMessageToEveryone(message);
        return (success);
    }
}CSPlayerExitRoom_instance;

REGISTE_MESSAGE_ID(MSG_CSPlayerExitRoom, CSPlayerExitRoom, &CSPlayerExitRoom_instance);

}

