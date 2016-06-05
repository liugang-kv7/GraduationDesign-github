#include "syn.h"

namespace IslandSurvival{

enum 
{
	SYN_AllowedDelay = 3,
	SYN_AllowedExist = 20,
}; 

OneFrame::OneFrame()
{
	Init();
}

OneFrame::~OneFrame()
{
	
}

void OneFrame::Init()
{
	m_num = 0;
}

int32_t OneFrame::Add(CMessage& mess)
{
    if(m_num >= OneFrameBufferSize)
    {
        return fail;
    }
	m_mess[m_num++] = mess;
    return success;
}

SYN::SYN(Room *room)
{
	m_room = room;
}

SYN::~SYN()
{
	
}

void SYN::Init()
{
    for(int32_t i = 0; i < SYNBufferSize; ++i)
    {
        m_oneframe[i].Init();
    }
	m_init_time = GetNowTimeUs();
	m_head = 0;
    // m_tail = 0;
	m_head_frame = 1;
	UpdateTime();
}

void SYN::Add(CMessage& mess)
{
	UpdateTime();

	int32_t messframe = mess.m_head.m_framenum;
	if(messframe > m_now_frame + 3)
	{
		printf("oh shit: messframe = %d is bigger than nowframe = %d\n",
		        messframe, m_now_frame);
	}
	else if(messframe + SYN_AllowedDelay >= m_now_frame)
	{
		int32_t diff = messframe - m_head_frame;
		m_oneframe[(m_head+diff)%SYNBufferSize].Add(mess);
	}
	else if(messframe + SYN_AllowedExist >= m_now_frame)
	{
		m_oneframe[m_head].Add(mess);
	}
	else
	{
		printf("oh shit: throw this message away, messframe = %d, 
		       nowframe = %d\n", messframe, m_now_frame);
	}
}

void SYN::Run()
{
	UpdateTime();
	while(m_head_frame + SYN_AllowedDelay + 1 <= m_now_frame)
	{
		SendHeadFrameToAll();
		m_head = (m_head+1)%SYNBufferSize;
		m_head_frame++;
	}
}

void SYN::SendHeadFrameToAll()
{
	if(0 == m_oneframe[m_head].m_num)
	{
		SCFrameMess* proto = new SCFrameMess;
		proto->set_operation(m_oneframe[m_head].m_num);
		
		CMessage mess;
		mess.m_head.m_message_id = MSG_SCFrameMess;
		mess.m_head.m_framenum = m_head_frame;
		mess.m_proto = proto;
		m_room->SendMessageToEveryone(mess);
	}

	for(int32_t i = 0; i < m_oneframe[m_head].m_num; ++i)
	{
		m_oneframe[m_head].m_mess[i].m_head.m_framenum = m_head_frame;
		if(m_oneframe[m_head].m_mess[i].m_head.m_message_id == MSG_SCChoseRole)
		{
			SCChoseRole* role = (SCChoseRole*)m_oneframe[m_head].m_mess[i].m_proto;
			printf("roleid:%d playid:%d chose role!!!!!!!!!!!!\n", role->role_id(), role->player_id());
		}
		m_room->SendMessageToEveryone(m_oneframe[m_head].m_mess[i]);
	}
    m_oneframe[m_head].Init();
}

int32_t SYN::GetNowFrame()
{
	UpdateTime();
	return m_now_frame;
}

void SYN::UpdateTime()
{
	int64_t nowtime = GetNowTimeUs();
	m_now_frame = (int)((nowtime - m_init_time)/20000 + 1);
    /*
	int32_t lastframe = m_head_frame+(m_tail-m_head+SYNBufferSize)%SYNBufferSize;
	for(int32_t i = 1; i <= m_now_frame-lastframe+5; i++)
	{
		//assert(0<=m_tail && m_tail<SYNBufferSize);
		m_oneframe[m_tail].Init();
		m_tail = (m_tail+1)%SYNBufferSize;
	}
    */
}


}
