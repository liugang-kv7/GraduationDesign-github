#include "person.h"

namespace IslandSurvival{

Person::Person()
{
	
}


Person::~Person()
{

}

void Person::Init(int32_t user_id, std::string name)
{
	m_name = name;
	m_user_id = user_id;
	m_status = person_in_hall;
}

int32_t Person::ChangeStatus(int32_t status)
{
	if(status == m_status)
	{
		PRINT_LOG("ERROR: user_id: %d 's status is as same as you wanting to change, maybe it's code logic problem\n", m_user_id);
		return fail;
	}
	m_status = status;
	return success;
}

void Person::OnExit()
{
	if(person_in_hall == m_status)
	{
		//do nothing
	}
	else if(person_in_room == m_status)
	{
		ROOMMANAGER->DeletePlayerFromRoom(m_user_id);
	}
}

std::string Person::GetName()
{	
	return m_name;
}

}
