#ifndef _ROOM_MANAGER_H__
#define _ROOM_MANAGER_H__

#include "../Include/server_base.h"
#include "room.h"
#include "syn.h"
#include "processor_base.h"

namespace IslandSurvival {

enum
{
	join_room_not_exist = 0,
	add_player_no_waiting = 1,
	add_player_full = 2,
	add_player_succeed = 3,
	join_room_error_fail = 4
};
class Room;

class RoomManager
{
public:
    static RoomManager* Instance();

    int32_t CreateRoom(int32_t user_id, int32_t game_mode, int32_t team_mode, int32_t& room_id);
    int32_t JoinRoom(int32_t user_id, int32_t room_id);
    int32_t DeletePlayerFromRoom(int32_t user_id);
	int32_t GameEndFromRoom(int32_t user_id);

    Room* GetRoomByUserID(int32_t user_id);
	Room* GetRoomByRoomID(int32_t room_id);

    SCPullAllRoomInfo GetAllRoomInfo();
    SCPullOneRoomDetail GetDetailRoomInfo(int32_t room_id);	

    void OnTick();

private:
    static RoomManager* m_pInstance;
	std::queue<Room*> m_free_room; 
    std::map<int32_t, Room*> belong_room_;  
    std::map<int32_t, Room*> rooms_;        // key : room_id
};

#define ROOMMANAGER RoomManager::Instance()

}


#endif
