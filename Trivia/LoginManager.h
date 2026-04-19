#pragma once
#include "IDataBase.h"
#include "LoggedUser.h"
#include <vector>
#include <mutex>

class LoginManager
{
public:
	LoginManager(IDataBase* database);
	void signup(const std::string& email, const std::string& username, const std::string& password);
	void login(const std::string& username, const std::string& password);
	void logout(const std::string& username);

private:
	IDataBase* m_database;
	std::vector<LoggedUser> m_loggedUsers;
	std::mutex m_lock;
};