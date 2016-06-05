#include "../Frame/processor_base.h"
#include "../Include/server_base.h"
#include "../proto_out/proto_islandsurvival.h"
#include "../Frame/message_processor_register.h"
#include "../Frame/room.h"
#include "../Frame/room_manager.h"

namespace IslandSurvival {

class MP_CSGamePause : public MessageProcessorBase
{
public:
    virtual int32_t ProcessRequest(int32_t user_id, const CMessage& mess)
    {
        printf("%d pause\n", user_id);
        Room* room = ROOMMANAGER->GetRoomByRoomID(user_id);
        if(NULL == room)
        {
            //error
            printf("error: %d not in the room\n", user_id);
            return fail;
        }
        int32_t playerid = room->GetPlayerID(user_id);
        if(playerid == 0)
        {
            //error
            printf("error: %d is not in his room\n", user_id);
            return fail;
        }
        SCGamePause* proto = new SCGamePause;

        CMessage newmess;
        newmess.m_head.m_framenum = mess.m_head.m_framenum;
        newmess.m_head.m_message_id = MSG_SCGamePause;
        newmess.m_proto = proto;

        room->m_syn->Add(newmess);
        return 1;
    }
}MP_CSGamePause_instance;

REGISTE_MESSAGE_ID(MSG_CSGamePause, CSGamePause, &MP_CSGamePause_instance);

}
