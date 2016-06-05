#include "../../Frame/processor_base.h"
#include "../../Include/server_base.h"
#include "../../proto_out/proto_islandsurvival.h"
#include "../../Frame/message_processor_register.h"
#include "../../Frame/login.h"
#include "../../Frame/message_handler.h"
#include "../../Frame/person_manager.h"

namespace IslandSurvival {

class MP_CSRegist : public MessageProcessorBase
{
public:
    virtual int32_t ProcessRequest(int32_t fd, const CMessage& mess)
    {
		CMessage message;
		SCRegist* proto = new SCRegist;
		message.m_head.m_message_id = MSG_SCRegist;
		message.m_head.m_framenum = -1;

		CSRegist* oldproto = (CSRegist*)(mess.m_proto);
		int32_t user_id;
		int32_t ret = LOGIN->NewUserRegist(oldproto->id(), oldproto->passwd(), user_id);
		if(ret == regist_succeed)
		{
			message.m_proto = proto;
			int32_t ret = PERSONMANAGER->PersonLogin(user_id, oldproto->id());
			if(fail == ret)
			{
				proto->set_result(3);
				SERVERFRAME->SendMessageByConnfd(fd, message);
			}
			else
			{
				proto->set_result(2);
				SERVERFRAME->PlayerLogin(fd, user_id);
				SERVERFRAME->SendMessageToClient(user_id, message);
			}
		}
		else if(ret == regist_used)
		{
			proto->set_result(0);
			message.m_proto = proto;
			SERVERFRAME->SendMessageByConnfd(fd, message);
		}
		else if(ret == db_error)
		{
			proto->set_result(1);
			message.m_proto = proto;
			SERVERFRAME->SendMessageByConnfd(fd, message);
		}
        return (success);
    }
}CSRegist_instance;

REGISTE_MESSAGE_ID(MSG_CSRegist, CSRegist, &CSRegist_instance);

}


