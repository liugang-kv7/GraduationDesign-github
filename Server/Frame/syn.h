#ifndef _SYN_H__
#define _SYN_H__

#include "../Include/server_base.h"
#include "../Include/CMessage.h"
#include "message_handler.h"
#include "room.h"
#include "../proto_out/proto_islandsurvival.h"
 
namespace IslandSurvival{

enum
{
    OneFrameBufferSize = 32,
    SYNBufferSize      = 64,
};

class OneFrame
{
public:
	OneFrame();
	~OneFrame();
	void Init();
	int32_t Add(CMessage& mess);
	CMessage m_mess[OneFrameBufferSize];
	int32_t m_num;
};


class SYN
{
public:
	SYN(class Room *room);
	~SYN();
	void Init();
	void Add(CMessage& mess);
	void Run();
	void SendHeadFrameToAll();

	int32_t GetNowFrame();
private:
	class Room *m_room;
	void UpdateTime();
	OneFrame m_oneframe[SYNBufferSize];
	int32_t m_head;
	//int32_t m_tail;
	int32_t m_head_frame;
	int32_t m_now_frame;
	long int m_init_time;

};

}

#endif
