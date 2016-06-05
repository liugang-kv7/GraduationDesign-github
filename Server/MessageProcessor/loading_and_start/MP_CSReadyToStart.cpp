#include "../../Frame/processor_base.h"
#include "../../Include/server_base.h"
#include "../../proto_out/proto_islandsurvival.h"
#include "../../Frame/message_processor_register.h"
#include "../../Frame/room.h"
#include "../../Frame/room_manager.h"

namespace IslandSurvival {

class MP_CSReadyToStart : public MessageProcessorBase
{
public:
    virtual int32_t ProcessRequest(int32_t user_id, const CMessage& mess)
    {
        Room* room = ROOMMANAGER->GetRoomByUserID(user_id);
        if(NULL == room)
        {
            PRINT_LOG("error: %d not in any room\n", user_id);
            return (fail);
        }
        if(room->GetPlayerID(user_id) != 1)
        {
            PRINT_LOG("player(%d) is not the room owner, can't start game!\n", user_id);
            return (fail);
        }
        room->SendGameLoading();
        printf("Ready to Start Game\n");
        return (success);
    }
}CSReadyToStart_instance;

REGISTE_MESSAGE_ID(MSG_CSReadyToStart, CSReadyToStart, &CSReadyToStart_instance);

}
