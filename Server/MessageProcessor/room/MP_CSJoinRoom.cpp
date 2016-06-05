#include "../../Frame/processor_base.h"
#include "../../Include/server_base.h"
#include "../../proto_out/proto_islandsurvival.h"
#include "../../Frame/message_processor_register.h"
#include "../../Frame/room.h"
#include "../../Frame/room_manager.h"

namespace IslandSurvival {

class MP_CSJoinRoom : public MessageProcessorBase
{
public:
    virtual int32_t ProcessRequest(int32_t user_id, const CMessage& mess)
    {	
		SCJoinRoom* proto = new SCJoinRoom;
		CMessage message;
		message.m_head.m_message_id = MSG_SCJoinRoom;
		message.m_head.m_framenum = -1;
		message.m_proto = proto;
		
		CSJoinRoom* oldproto = (CSJoinRoom*)(mess.m_proto);
		int room_id = oldproto->room_id();

		int result = ROOMMANAGER->JoinRoom(user_id, room_id);
		if(result != join_room_error_fail)
		{
			proto->set_result(result);
			SERVERFRAME->SendMessageToClient(user_id, message);
			if(result == add_player_succeed)
			{
				SCPullOneRoomDetail* proto2 = new SCPullOneRoomDetail;
				CMessage message2;
				message2.m_head.m_message_id = MSG_SCPullOneRoomDetail;
				message2.m_head.m_framenum = -1;
				message2.m_proto = proto2;
				proto2->CopyFrom(ROOMMANAGER->GetDetailRoomInfo(room_id));

				ROOMMANAGER->GetRoomByRoomID(room_id)->SendMessageToEveryone(message2);
				printf("user_id:%d Join Room succeed\n", user_id);
			}
		}
		else
		{
			PRINT_LOG("user_id:%d Join Room error\n", user_id);
		}
        return (success);
    }
}CSJoinRoom_instance;

REGISTE_MESSAGE_ID(MSG_CSJoinRoom, CSJoinRoom, &CSJoinRoom_instance);

}
