#include "../Frame/processor_base.h"
#include "../Include/server_base.h"
#include "../proto_out/proto_islandsurvival.h"
#include "../Frame/message_processor_register.h"
#include "../Frame/heartbeat.h"

namespace IslandSurvival{

class MP_CSHeartbeat : public MessageProcessorBase
{
public:
	virtual int32_t ProcessRequest(int32_t fd, const CMessage& mess)
	{
		//printf("%d send heartbeat\n", fd);
		HEARTBEAT->UpdateFD(fd);
		return 1;
	}
}MP_CSHeartbeat_instance;

REGISTE_MESSAGE_ID(MSG_CSHeartbeat, CSHeartbeat, &MP_CSHeartbeat_instance);

}
