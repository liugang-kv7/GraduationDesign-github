#include "room.h"

namespace IslandSurvival {

Room::Room()
{
    m_syn = new SYN(this);
    srand(time(NULL));
}

Room::~Room()
{
    if(NULL != m_syn)
    {
        delete m_syn;
    }
    m_syn = NULL;
}

void Room::Init(int32_t game_mode, int32_t team_mode, int32_t room_id)
{
    memset(m_is_online, 0x0, sizeof(m_is_online));
    m_status = room_status_waiting;
    m_player_cnt = 0;
    m_online_player_cnt = 0;
	m_game_mode = game_mode;
	m_team_mode = team_mode;
	m_room_id = room_id;
	m_game_option = 10;
}

void Room::OnTick()
{
    if(room_status_loading == m_status)
    {
        if(GetNowTimeSecond() - m_loading_time_second >= max_loading_time_second)
        {
            for(int i = 1; i <= m_player_cnt; ++i) //把加载时间过长的踢出房间
            {
                if(m_is_online[i] && false == m_is_loading_succeed[i])
                {
					SERVERFRAME->DeleteClientByUserID(m_players[i]);
                }
            }
			SendGameStart();
        }
    }
    else if(room_status_ingame == m_status)
    {
        m_syn->Run();
    }
}

int32_t Room::AddPlayer(int32_t user_id)
{
    if(m_status == room_status_waiting)
    {
        if(m_player_cnt >= max_player_cnt)
        {
            PRINT_LOG("user_id:%d room is full\n", user_id);
            return add_player_full;
        }
        m_player_cnt++;
        m_online_player_cnt++;
        m_players[m_player_cnt] = user_id;
		if(m_team_mode == 1)
		{
			m_team_id[m_player_cnt] = m_player_cnt;
			//printf("teamid is %d\n",m_player_cnt);
		}
		else
		{
			m_team_id[m_player_cnt] = 1;
		}
        m_is_online[m_player_cnt] = true;

		PERSONMANAGER->ChangePersonStatus(user_id, person_in_room);
        return add_player_succeed;
    }
    else
    {
        PRINT_LOG("ERROR:user_id:%d room status is wrong\n", user_id);
        return add_player_no_waiting;
    }
}

int32_t Room::DeletePlayer(int32_t user_id)
{
    int32_t pos = GetPlayerID(user_id);
    if(0 == pos)
    {
        PRINT_LOG("ERROR:user_id:%d is not in this room(DeletePlayer)\n", user_id);
        return fail;
    }

    if(room_status_waiting == m_status)
    {
        for(int32_t i = pos + 1; i <= m_player_cnt; i++)
        {
            m_players[i - 1] = m_players[i];
			if(m_team_mode == 2) //组队模式
			{
				m_team_id[i-1] = m_team_id[i];
			}
        }
        m_is_online[m_player_cnt] = false;
        m_player_cnt--;
        m_online_player_cnt--;
		
		SCPullOneRoomDetail* proto = new SCPullOneRoomDetail;
		proto->CopyFrom(GetDetailInfo());
		CMessage mess;
		mess.m_head.m_message_id = MSG_SCPullOneRoomDetail;
		mess.m_head.m_framenum = -1;
		mess.m_proto = proto;
		SendMessageToEveryone(mess);

		PERSONMANAGER->ChangePersonStatus(user_id, person_in_hall);
		printf("user_id:%d is exit room\n", user_id);
        return success;
    }
    else if(room_status_loading == m_status)
    {
        m_is_online[pos] = false;
        m_online_player_cnt--;

		SCPlayerExitGame* proto = new SCPlayerExitGame;
		proto->set_player(pos);
		CMessage mess;
		mess.m_head.m_message_id = MSG_SCPlayerExitGame;
		mess.m_head.m_framenum = -1;
		mess.m_proto = proto;
		SendMessageToEveryone(mess);

		if(m_is_loading_succeed[pos])
        {
            m_loading_succeed_cnt--;
            m_is_loading_succeed[pos] = false;
        }
		else
		{
			if(m_loading_succeed_cnt >= m_online_player_cnt)
			{
				SendGameStart();
			}
		}
		printf("user_id:%d is disconnected\n", user_id);
        return success;
    }
    else if(room_status_ingame == m_status)
    {
        m_is_online[pos] = false;
        m_online_player_cnt--;

		SCPlayerExitGame* proto = new SCPlayerExitGame;
		proto->set_player(pos);
		CMessage mess;
		mess.m_head.m_message_id = MSG_SCPlayerExitGame;
		mess.m_head.m_framenum = m_syn->GetNowFrame();
		mess.m_proto = proto;
		m_syn->Add(mess);
		printf("user_id:%d is disconnected\n", user_id);
		
        return success;
    }
    return fail;
}

void Room::SendMessageToEveryone(CMessage& mess)
{
    for(int32_t i = 1; i <= m_player_cnt; i++)
    {
        if(m_is_online[i])
        {
            SERVERFRAME->SendMessageToClient(m_players[i], mess);
        }
    }
}

void Room::SendGameLoading()
{
    if(room_status_waiting != m_status)
    {
        PRINT_LOG("ERROR : room status wrong(SendGameLoading)\n");
        return;
    }
    int32_t _rand = rand() % 10007;
    printf("random seed is %d\n", _rand);
    
    SCGameLoading* proto = new SCGameLoading;
    proto->set_rand(_rand);

    CMessage mess;
	mess.m_head.m_message_id = MSG_SCGameLoading;
    mess.m_head.m_framenum = -1;
    mess.m_proto = proto;
    SendMessageToEveryone(mess);

    m_status = room_status_loading;
    m_loading_succeed_cnt = 0;
    m_loading_time_second = GetNowTimeSecond();
    memset(m_is_loading_succeed, 0x0, sizeof(m_is_loading_succeed));
}

void Room::SendGameStart()
{
    if(room_status_loading != m_status)
    {
        PRINT_LOG("ERROR : room status wrong(SendGameStart)\n");
        return;
    }
    SCGameStart* proto = new SCGameStart;

    CMessage mess;
    mess.m_head.m_message_id = MSG_SCGameStart;
    mess.m_head.m_framenum = -1;
    mess.m_proto = proto;

    SendMessageToEveryone(mess);

    m_syn->Init();
    m_status = room_status_ingame;

    printf("Game Start!\n");
}

int32_t Room::GetGameStatus()
{
    return m_status;
}

int32_t Room::GetPlayerID(int32_t user_id)
{
    int32_t pos = 0;
    for(int32_t i = 1; i <= m_player_cnt; i++)
    {
        if(m_is_online[i] && m_players[i] == user_id)
        {
            pos = i;
            break;
        }
    }
    return pos;
}

int32_t Room::GetOnlinePlayerCount()
{
    return 	m_online_player_cnt;
}

int32_t Room::GetRoomID()
{
	return m_room_id;
}

int32_t Room::ChangeTeamID(int32_t user_id, int32_t team_id)
{
	int32_t pos = GetPlayerID(user_id);
	if(0 == pos)
    {
        PRINT_LOG("ERROR:user_id:%d is not in this room(ChangeTeamID)\n", user_id);
        return fail;
    }
	m_team_id[pos] = team_id;
	return success;
}

int32_t	Room::ChangeGameOption(int32_t user_id, int32_t game_option)
{
	int32_t pos = GetPlayerID(user_id);
	if(0 == pos)
	{
		PRINT_LOG("ERROR:user_id:%d is not in this room(ChangeGameOption)\n", user_id);
		return fail;
	}
	if(1 == pos)
	{
		m_game_option = game_option;
		return success;
	}
	else
	{
		PRINT_LOG("ERROR:user_id:%d is not room owner\n", user_id);
	}
	return fail;
}

int32_t Room::AddLoadingSucceed(int32_t user_id)
{
    if(room_status_loading != m_status)
    {
        ROOMMANAGER->DeletePlayerFromRoom(user_id);
        return (0);
    }
    int32_t pos = GetPlayerID(user_id);
    if(0 == pos)
    {
        PRINT_LOG("ERROR:%d is not in this room(AddLoadingSucceed)\n", user_id);
        return (0);
    }
    if(true == m_is_loading_succeed[pos])
    {
        PRINT_LOG("ERROR:%d is already loading succeed(AddLoadingSucceed)\n", user_id);
        return (0);
    }
    m_is_loading_succeed[pos] = true;
    m_loading_succeed_cnt++;
    if(m_loading_succeed_cnt >= m_online_player_cnt)
    {
        return (1);
    }
    return (0);
}

int32_t Room::GameEnd(int32_t user_id)
{
	if(room_status_ingame == m_status)
	{
		int pos = GetPlayerID(user_id);
		if(0 != pos)
		{
			m_is_online[pos] = false;
			m_online_player_cnt--;
			PERSONMANAGER->ChangePersonStatus(user_id, person_in_hall);
			return success;
		}
		else
		{
			PRINT_LOG("ERROR: user_id:%d is not in this room\n", user_id);
			return fail;
		}
	}
	else
	{
		PRINT_LOG("ERROR: room is not in game(GameEnd)\n");
	}
	return fail;
}

PBRoomInfo Room::GetRoomInfo()
{
    PBRoomInfo ret;
	ret.set_id(m_room_id);
    ret.set_have(m_online_player_cnt);
    ret.set_limit(max_player_cnt);
	ret.set_game_mode(m_game_mode);
	ret.set_team_mode(m_team_mode);
    return ret;
}

SCPullOneRoomDetail Room::GetDetailInfo()
{
	PBDetailRoomInfo *proto = new PBDetailRoomInfo;
	proto->set_game_option(m_game_option);
	for(int32_t i = 1; i <= m_player_cnt; i++)
	{
		if(m_is_online[i])
		{
			std::string name = PERSONMANAGER->GetPersonName(m_players[i]);
			proto->add_players(name);
			proto->add_team_id(m_team_id[i]);
			//printf("%s's teamid = %d\n", name.c_str(), m_team_id[i]);
			proto->add_player_id(i);
		}
	}
	
	SCPullOneRoomDetail ret;

	ret.set_allocated_room_info(proto);
	return ret;
}

}

