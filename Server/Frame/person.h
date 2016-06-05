#ifndef _PERSON_H__
#define _PERSON_H__

#include "../Include/server_base.h"
#include "room_manager.h"

namespace IslandSurvival{

enum 
{
	person_in_hall,
	person_in_room,
};

class Person
{
public:
	Person();
	~Person();
	void Init(int32_t user_id, std::string name);
	int32_t ChangeStatus(int32_t status);
	void OnExit();
	std::string GetName();
private:
	std::string m_name;
	int32_t m_status;
	int32_t m_user_id;
};



}

#endif
