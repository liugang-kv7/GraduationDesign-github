#include "../Frame/processor_base.h"
#include "../Include/server_base.h"
#include "../proto_out/proto_islandsurvival.h"
#include "../Frame/message_processor_register.h"
#include "../Frame/room.h"
#include "../Frame/room_manager.h"

namespace IslandSurvival{

class MP_CSSkill : public MessageProcessorBase
{
public:
	virtual int32_t ProcessRequest(int32_t user_id, const CMessage& mess)
	{
		//printf("%d use skill\n", user_id);
		Room* room = ROOMMANAGER->GetRoomByUserID(user_id);
		if(NULL == room)
		{
			//error
			printf("error: %d not in the room\n", user_id);
			return 0;
		}
		int32_t playerid = room->GetPlayerID(user_id);
		if(playerid == 0)
		{
			//error
			printf("error: %d is not in his room\n", user_id);
			return 0;
		}
		CSSkill* oldproto = (CSSkill*)mess.m_proto;
		SCSkill* proto = new SCSkill;

		proto->set_skill_id(oldproto->skill_id());
		proto->set_pos_x(oldproto->pos_x());
		proto->set_pos_z(oldproto->pos_z());
		proto->set_target_id(oldproto->target_id());
		proto->set_attacker_id(playerid);

		CMessage newmess;
		newmess.m_head.m_framenum = mess.m_head.m_framenum;
		newmess.m_head.m_message_id = MSG_SCSkill;
		newmess.m_proto = proto;

		room->m_syn->Add(newmess);
		return 1;
	}
}MP_CSSkill_instance;

REGISTE_MESSAGE_ID(MSG_CSSkill, CSSkill, &MP_CSSkill_instance);

}
