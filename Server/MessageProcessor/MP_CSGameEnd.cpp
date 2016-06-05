#include "../Frame/processor_base.h"
#include "../Include/server_base.h"
#include "../proto_out/proto_islandsurvival.h"
#include "../Frame/message_processor_register.h"
#include "../Frame/room.h"
#include "../Frame/room_manager.h"

namespace IslandSurvival{

class MP_CSGameEnd : public MessageProcessorBase
{
public:
	virtual int32_t ProcessRequest(int32_t user_id, const CMessage& mess)
	{
		printf("user_id:%d GameEnd\n", user_id);
		
		Room* room = ROOMMANAGER->GetRoomByUserID(user_id);
		if(NULL == room)
		{
			//error
			printf("error: %d not in the room\n", user_id);
			return 0;
		}
		SCGameEnd* proto = new SCGameEnd;
		CMessage newmess;
		newmess.m_head.m_framenum = room->m_syn->GetNowFrame();
		newmess.m_head.m_message_id = MSG_SCGameEnd;
		newmess.m_proto = proto;
		
		SERVERFRAME->SendMessageToClient(user_id, newmess);
		
		ROOMMANAGER->GameEndFromRoom(user_id);
		return 1;
	}
}MP_CSGameEnd_instance;

REGISTE_MESSAGE_ID(MSG_CSGameEnd, CSGameEnd, &MP_CSGameEnd_instance);

}

