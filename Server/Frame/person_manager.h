#ifndef _PERSONMANAGER_H__
#define _PERSONMANAGER_H__

#include "../Include/server_base.h"
#include "room_manager.h"
#include "person.h"
namespace IslandSurvival{

class Person;
class PersonManager
{
public:
	static PersonManager* Instance();
	int32_t PersonLogin(int32_t user_id, std::string name);
	int32_t PersonLogout(int32_t user_id);
	int32_t ChangePersonStatus(int32_t user_id, int32_t status);
	std::string GetPersonName(int32_t user_id);
private:
	static PersonManager* m_pInstance;
	std::map<int32_t, Person*> m_user_id_to_person;
};

#define PERSONMANAGER PersonManager::Instance()

}

#endif
