#include "../../Frame/processor_base.h"
#include "../../Include/server_base.h"
#include "../../proto_out/proto_islandsurvival.h"
#include "../../Frame/message_processor_register.h"
#include "../../Frame/room.h"
#include "../../Frame/room_manager.h"

namespace IslandSurvival {

class MP_CSPullAllRoomInfo : public MessageProcessorBase
{
public:
    virtual int32_t ProcessRequest(int32_t user_id, const CMessage& mess)
    {
        SCPullAllRoomInfo* proto = new SCPullAllRoomInfo;
		
		CMessage message;
        message.m_head.m_message_id = MSG_SCPullAllRoomInfo;
        message.m_head.m_framenum = -1;
        message.m_proto = proto;

        proto->CopyFrom(ROOMMANAGER->GetAllRoomInfo());

        SERVERFRAME->SendMessageToClient(user_id, message);
        return (success);
    }
}CSPullAllRoomInfo_instance;

REGISTE_MESSAGE_ID(MSG_CSPullAllRoomInfo, CSPullAllRoomInfo, &CSPullAllRoomInfo_instance);

}
