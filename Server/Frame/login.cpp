#include "login.h"

namespace IslandSurvival{

Login* Login::m_pInstance = NULL;

Login* Login::Instance()
{
	if(NULL == m_pInstance)
	{
		m_pInstance = new Login;
	}
	return m_pInstance;
}

Login::Login()
{
	
}

int32_t Login::Init()
{
	m_connection = mysql_init(NULL);
	if(m_connection == NULL)
	{
		PRINT_LOG("ERROR:mysql_init error : %s\n", mysql_error(m_connection));
		return fail;
	}
	std::ifstream in(dbconfig);
	if(!in.is_open())
	{
		PRINT_LOG("ERROR:can't open database config\n");
		return fail;
	}

	std::string ip, user, passwd, db_name;
	int32_t port;
	in>>ip>>port>>user>>passwd>>db_name;
	in.close();
	//printf("%s %d %s %s %s\n", ip.c_str(), port, user.c_str(), passwd.c_str(), db_name.c_str());

	m_connection = mysql_real_connect(m_connection, ip.c_str(), user.c_str(),
		passwd.c_str(), db_name.c_str(), port, NULL, 0);
	
	if(NULL == m_connection)
	{
		PRINT_LOG("ERROR: mysql connect error : %s", mysql_error(m_connection));
		return fail;
	}
	return success;
}

int32_t Login::LoginRequest(std::string name, std::string passwd, int& user_id)
{
	std::string sql = "select * from login where id = '" + name + "';";
	//std::cout<<sql<<std::endl;
	int32_t ret = mysql_real_query(m_connection, sql.c_str(), 60);
	if(0 != ret)
	{
		PRINT_LOG("DB query error : %s\n", mysql_error(m_connection));
		return db_error;
	}
	m_result = mysql_use_result(m_connection);
	m_row = mysql_fetch_row(m_result);
	if(NULL == m_row)
	{
		printf("LoginLog : ID:%s is not exist\n", name.c_str());
		mysql_free_result(m_result);
		return login_failed;
	}
	if(strcmp(passwd.c_str(), m_row[1]) != 0)
	{
		printf("LoginLog : ID:%s passwd:%s is wrong\n", name.c_str(), passwd.c_str());
		mysql_free_result(m_result);
		return login_failed;
	}
	mysql_free_result(m_result);
	
	sql = "select user_id from login where id = '" + name + "';";
	ret = mysql_real_query(m_connection, sql.c_str(), 60);
	if(0 != ret)
	{
		PRINT_LOG("DB query error : %s\n", mysql_error(m_connection));
		return db_error;
	}
	m_result = mysql_use_result(m_connection);
	m_row = mysql_fetch_row(m_result);
	sscanf(m_row[0], "%d", &user_id);
	mysql_free_result(m_result);

	printf("LoginLog : ID : %s login in\n", name.c_str()); 
	return login_succeed;
}

int32_t Login::NewUserRegist(std::string name, std::string passwd, int& user_id)
{
	std::string sql = "select * from login where id = '" + name + "';";
	//std::cout<<sql<<std::endl;
	int32_t ret = mysql_real_query(m_connection, sql.c_str(), 60);
	if(0 != ret)
	{
		PRINT_LOG("DB query error : %s\n", mysql_error(m_connection));
		return db_error;
	}
	m_result = mysql_use_result(m_connection);
	m_row = mysql_fetch_row(m_result);
	if(NULL != m_row)
	{
		printf("RegistLog : ID:%s is exist\n", name.c_str());
		mysql_free_result(m_result);
		return regist_used;
	}
	mysql_free_result(m_result);

	sql = "insert into login(id, passwd) values('" + name + "','" + passwd + "');";
	ret = mysql_real_query(m_connection, sql.c_str(), 60);
	if(0 != ret)
	{
		PRINT_LOG("DB query error : %s\n", mysql_error(m_connection));
		return db_error;
	}
	
	sql = "select user_id from login where id = '" + name + "';";
	ret = mysql_real_query(m_connection, sql.c_str(), 60);
	if(0 != ret)
	{
		PRINT_LOG("DB query error : %s\n", mysql_error(m_connection));
		return db_error;
	}
	m_result = mysql_use_result(m_connection);
	m_row = mysql_fetch_row(m_result);
	sscanf(m_row[0], "%d", &user_id);
	mysql_free_result(m_result);

	printf("RegistLog : ID:%s register succeed\n", name.c_str());
	return regist_succeed;
}

}
