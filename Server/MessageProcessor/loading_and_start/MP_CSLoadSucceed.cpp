#include "../../Frame/processor_base.h"
#include "../../Include/server_base.h"
#include "../../proto_out/proto_islandsurvival.h"
#include "../../Frame/message_processor_register.h"
#include "../../Frame/room.h"
#include "../../Frame/room_manager.h"

namespace IslandSurvival{

class MP_CSLoadSucceed : public MessageProcessorBase
{
public:
	virtual int32_t ProcessRequest(int32_t user_id, const CMessage& mess)
	{
		printf("%d Get LoadSucceed\n", user_id);
		Room* room = ROOMMANAGER->GetRoomByUserID(user_id);
		if(NULL == room)
		{
			//error
			printf("oh shit: fd in wrong room\n");
			return 0;
		}
		if(room->GetGameStatus() == room_status_loading)
		{
			if(room->AddLoadingSucceed(user_id))
			{
				room->SendGameStart();
				printf("Game Start!\n");
			}
		}
		return 1;
	}
}MP_CSLoadSucceed_instance;

REGISTE_MESSAGE_ID(MSG_CSLoadSucceed, CSLoadSucceed, &MP_CSLoadSucceed_instance);

}
