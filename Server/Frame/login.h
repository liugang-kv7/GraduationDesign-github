#ifndef _LOGIN_H__
#define _LOGIN_H__

#include "../Include/server_base.h"
#include "mysql.h"

namespace IslandSurvival{

#define dbconfig "./config/database.txt"

enum
{
	db_error		= 0,
	login_succeed	= 1,
	login_failed	= 2,
	regist_succeed	= 3,
	regist_used		= 4
};

class Login
{
public:
	static Login* Instance();
	int32_t Init();
	int32_t LoginRequest(std::string name, std::string passwd, int32_t& user_id);
	int32_t NewUserRegist(std::string name, std::string passwd, int32_t& user_id);
private:
	Login();
	~Login();
	static Login* m_pInstance;
	MYSQL* m_connection;
	MYSQL_RES* m_result;
	MYSQL_ROW m_row;
};

#define LOGIN Login::Instance()

}
#endif
