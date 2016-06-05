#ifndef _ROOM_H__
#define _ROOM_H__

#include "../Include/server_base.h"
#include "../Include/CMessage.h"
#include "message_processor_register.h"
#include "../proto_out/proto_islandsurvival.h"
#include "syn.h"
#include "person_manager.h"
#include "room_manager.h"

namespace IslandSurvival {


enum
{
    room_status_waiting = 1,
    room_status_loading = 2,
    room_status_ingame = 3,
    max_player_cnt = 8,
    max_loading_time_second = 180,
};


class Room
{
public:
    Room();
    ~Room();
    void Init(int32_t game_mode, int32_t team_mode, int32_t room_id);
    void OnTick();

    int32_t AddPlayer(int32_t user_id);
    int32_t DeletePlayer(int32_t user_id);

    void SendMessageToEveryone(CMessage& mess);
    void SendGameLoading();
    void SendGameStart();

    int32_t GetGameStatus();
    int32_t GetPlayerID(int32_t user_id);
    int32_t GetOnlinePlayerCount();
	int32_t GetRoomID();

	int32_t ChangeTeamID(int32_t user_id, int32_t team_id);
	int32_t	ChangeGameOption(int32_t user_id, int32_t game_option); 

    int32_t AddLoadingSucceed(int32_t user_id);

	int32_t GameEnd(int32_t user_id);

    PBRoomInfo GetRoomInfo();
	SCPullOneRoomDetail GetDetailInfo();
	
    class SYN *m_syn;
private:
	int32_t m_room_id;

    int32_t m_players[max_player_cnt + 1];
	int32_t m_team_id[max_player_cnt + 1];

    bool m_is_online[max_player_cnt + 1];
    bool m_is_loading_succeed[max_player_cnt + 1];//TODO to USE
	
    int32_t m_player_cnt;
    int32_t m_online_player_cnt;
    int32_t m_status;
    int32_t m_loading_succeed_cnt;
    int32_t m_loading_time_second;
	
	int32_t m_game_mode;
	int32_t m_team_mode;
	int32_t m_game_option;
};

}

#endif
