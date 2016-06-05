#include "../../Frame/processor_base.h"
#include "../../Include/server_base.h"
#include "../../proto_out/proto_islandsurvival.h"
#include "../../Frame/message_processor_register.h"
#include "../../Frame/room.h"
#include "../../Frame/room_manager.h"

namespace IslandSurvival {

class MP_CSChangeTeam : public MessageProcessorBase
{
public:
    virtual int32_t ProcessRequest(int32_t user_id, const CMessage& mess)
    {
		printf("user_id:%d change team or option\n", user_id);
		SCPullOneRoomDetail* proto = new SCPullOneRoomDetail;

        CMessage message;
        message.m_head.m_message_id = MSG_SCPullOneRoomDetail;
        message.m_head.m_framenum = -1;
		message.m_proto = proto;
		
		CSChangeTeam* oldproto = (CSChangeTeam*)(mess.m_proto);

		Room* room = ROOMMANAGER->GetRoomByUserID(user_id);
		if(NULL == room)
		{
			PRINT_LOG("ERROR:user_id:%d does not in room\n", user_id);
			return fail;
		}
		if(room_status_waiting != room->GetGameStatus())
		{
			PRINT_LOG("ERROR:user_id:%d want to changeTeam in Game\n", user_id);
			return fail;
		}
		if(oldproto->has_team_id())
		{
			room->ChangeTeamID(user_id, oldproto->team_id());
		}
		else if(oldproto->has_game_option())
		{
			room->ChangeGameOption(user_id, oldproto->game_option());
		}
		
		proto->CopyFrom(room->GetDetailInfo());
		room->SendMessageToEveryone(message);
		
		return success;
    }
}CSChangeTeam_instance;

REGISTE_MESSAGE_ID(MSG_CSChangeTeam, CSChangeTeam, &CSChangeTeam_instance);

}

