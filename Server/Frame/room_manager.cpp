#include "room_manager.h"

namespace IslandSurvival {



RoomManager* RoomManager::m_pInstance = NULL;

RoomManager* RoomManager::Instance()
{
    if(NULL == m_pInstance)
    {
        m_pInstance = new RoomManager;
    }
    return m_pInstance;
}

int32_t RoomManager::CreateRoom(int32_t user_id, int32_t game_mode, int32_t team_mode, int32_t& room_id)
{
    static int32_t s_room_id = 1000;
    if(belong_room_.find(user_id) != belong_room_.end())
    {
        PRINT_LOG("Create room error, user(%d) already in room\n", user_id);
        return fail;
    }
	Room* new_room;
	if(!m_free_room.empty())
	{
		new_room = m_free_room.front();
		m_free_room.pop();
		room_id = new_room->GetRoomID();
	}
	else
	{
		new_room = new Room;
		room_id = s_room_id++;
	}
    if(NULL == new_room)
    {
        return (fail);
    }
    belong_room_[user_id] = new_room;
    rooms_[room_id] = new_room;
    new_room->Init(game_mode, team_mode, room_id);
    new_room->AddPlayer(user_id);
    return (success);
}

int32_t RoomManager::JoinRoom(int32_t user_id, int32_t room_id)
{
    if(rooms_.find(room_id) == rooms_.end())
    {
        PRINT_LOG("JoinRoom error, room_id(%d) no exist!\n", room_id);
        return join_room_not_exist;
    }
    if(belong_room_.find(user_id) != belong_room_.end())
    {
        PRINT_LOG("JoinRoom error, user_id=%d  already in another room!\n", user_id);
        return join_room_error_fail;
    }
    int32_t ret = rooms_[room_id]->AddPlayer(user_id);
    if(add_player_succeed == ret)
    {
        belong_room_[user_id] = rooms_[room_id];
    }
    return (ret);
}

int32_t RoomManager::DeletePlayerFromRoom(int32_t user_id)
{
    if(belong_room_.find(user_id) != belong_room_.end())
    {
        Room* room = belong_room_[user_id];
        belong_room_.erase(user_id);
		return room->DeletePlayer(user_id);
    }
	return fail;
}

int32_t RoomManager::GameEndFromRoom(int32_t user_id)
{
    if(belong_room_.find(user_id) != belong_room_.end())
    {
        Room* room = belong_room_[user_id];
        belong_room_.erase(user_id);
		return room->GameEnd(user_id);
    }
	return fail;
}

Room* RoomManager::GetRoomByUserID(int32_t user_id)
{
    if(belong_room_.find(user_id) == belong_room_.end())
    {
        PRINT_LOG("GetRoomByUserID error, id=%d not in any room!\n", user_id);
        return NULL;
    }
    return belong_room_[user_id];
}

Room* RoomManager::GetRoomByRoomID(int32_t room_id)
{
	if(rooms_.find(room_id) == rooms_.end())
	{
		PRINT_LOG("This room_id:%d is not exist\n", room_id);
		return NULL;
	}
	return rooms_[room_id];
}

SCPullAllRoomInfo RoomManager::GetAllRoomInfo()
{
    SCPullAllRoomInfo ret;
    std::map<int32_t, Room*>::iterator it = rooms_.begin();
    for(; it != rooms_.end(); ++it)
    {
		if(room_status_waiting != it->second->GetGameStatus())
		{
			continue;
		}
        PBRoomInfo proto = it->second->GetRoomInfo();
        ret.add_rooms()->CopyFrom(proto);
    }
    return ret;
}

SCPullOneRoomDetail RoomManager::GetDetailRoomInfo(int32_t room_id)
{
	SCPullOneRoomDetail ret;
	std::map<int32_t, Room*>::iterator it = rooms_.find(room_id);
	if(it != rooms_.end())
	{
		ret = it->second->GetDetailInfo();
	}
	return ret;
}

void RoomManager::OnTick()
{
    std::map<int32_t, Room*>::iterator it = rooms_.begin();
    for(; it != rooms_.end();)
    {
        it->second->OnTick();
        std::map<int32_t, Room*>::iterator pre_room = it++;
        //  全不在线了。删除房间
        if((pre_room->second)->GetOnlinePlayerCount() <= 0)
        {
			m_free_room.push(pre_room->second);
            rooms_.erase(pre_room);
        }
    }
}

}
