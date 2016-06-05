#include "person_manager.h"

namespace IslandSurvival{

PersonManager* PersonManager::m_pInstance = NULL;

PersonManager* PersonManager::Instance()
{
	if(NULL == m_pInstance)
	{
		m_pInstance = new PersonManager;
	}
	return m_pInstance;
}

int32_t PersonManager::PersonLogin(int32_t user_id, std::string name)
{
	std::map<int32_t, Person*>::iterator it = m_user_id_to_person.find(user_id);
	if(it == m_user_id_to_person.end())
	{
		Person* per = new Person;
		per->Init(user_id, name);
		m_user_id_to_person[user_id] = per;
		return success;
	}
	else
	{
		PRINT_LOG("ERROR: user_id:%d is already login\n", user_id);
	}
	return fail;
}

int32_t PersonManager::PersonLogout(int32_t user_id)
{
	std::map<int32_t, Person*>::iterator it = m_user_id_to_person.find(user_id);
	if(it != m_user_id_to_person.end())
	{
		m_user_id_to_person[user_id]->OnExit();
		delete it->second;
		m_user_id_to_person.erase(user_id);
		return success;
	}
	else 
	{
		PRINT_LOG("ERROR: user_id:%d is not exist(PersonLogout)\n", user_id);
	}
	return fail;
}

int32_t PersonManager::ChangePersonStatus(int32_t user_id, int32_t status)
{
	std::map<int32_t, Person*>::iterator it = m_user_id_to_person.find(user_id);
	if(it != m_user_id_to_person.end())
	{
		return m_user_id_to_person[user_id]->ChangeStatus(status);
	}
	else 
	{
		PRINT_LOG("ERROR: user_id:%d is not exist(ChangePersonStatus)\n", user_id);
	}
	return fail;
}

std::string PersonManager::GetPersonName(int32_t user_id)
{
	std::map<int32_t, Person*>::iterator it = m_user_id_to_person.find(user_id);
	if(it != m_user_id_to_person.end())
	{
		return m_user_id_to_person[user_id]->GetName();
	}
	return "default";
}


}
