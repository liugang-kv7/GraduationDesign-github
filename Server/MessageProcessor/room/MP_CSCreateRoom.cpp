#include "../../Frame/processor_base.h"
#include "../../Include/server_base.h"
#include "../../proto_out/proto_islandsurvival.h"
#include "../../Frame/message_processor_register.h"
#include "../../Frame/room.h"
#include "../../Frame/room_manager.h"

namespace IslandSurvival {

class MP_CSCreateRoom : public MessageProcessorBase
{
public:
    virtual int32_t ProcessRequest(int32_t user_id, const CMessage& mess)
    {
        printf("user_id:%d Create Room\n", user_id);
        CMessage message;
        SCCreateRoom* proto = new SCCreateRoom;
        message.m_head.m_message_id = MSG_SCCreateRoom;
        message.m_head.m_framenum = -1;
        message.m_proto = proto;

		CSCreateRoom* oldproto = (CSCreateRoom*)(mess.m_proto);
		int32_t game_mode = oldproto->game_mode();
		int32_t team_mode = oldproto->team_mode();
	printf("game_mode = %d,team_mode = %d\n", game_mode, team_mode);
        int32_t room_id = 0;
        if(success == ROOMMANAGER->CreateRoom(user_id, game_mode, team_mode, room_id))
        {
        	proto->set_result(true);
        	proto->set_room_id(room_id);
		SERVERFRAME->SendMessageToClient(user_id, message);
		SCPullOneRoomDetail* proto2 = new SCPullOneRoomDetail;

			CMessage message2;
			message2.m_head.m_message_id = MSG_SCPullOneRoomDetail;
			message2.m_head.m_framenum = -1;
			message2.m_proto = proto2;
			proto2->CopyFrom(ROOMMANAGER->GetDetailRoomInfo(room_id));
			SERVERFRAME->SendMessageToClient(user_id, message2);
        }
        else
        {
            proto->set_result(false);
            proto->set_room_id(room_id);
			SERVERFRAME->SendMessageToClient(user_id, message);
        }
        return (success);
    }
}CSCreateRoom_instance;

REGISTE_MESSAGE_ID(MSG_CSCreateRoom, CSCreateRoom, &CSCreateRoom_instance);

}
