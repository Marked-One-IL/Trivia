#include "LoginManager.h"
#include "SqliteDataBase.h"
#include <algorithm>
#include <iostream>

LoginManager::LoginManager(IDataBase* database) : m_database(database)
{
}

void LoginManager::signup(const std::string& email, const std::string& username, const std::string& password)
{
	std::lock_guard<std::mutex> lg(this->m_lock);
	if (this->m_database->doesUserExist(username))
	{
		throw std::exception("User already signed up");
	}

	this->m_database->addNewUser(email, username, password);
}
void LoginManager::login(const std::string& username, const std::string& password)
{
	std::lock_guard<std::mutex> lg(this->m_lock);
	const bool found_user = std::find(this->m_loggedUsers.begin(), this->m_loggedUsers.end(), LoggedUser(username)) != this->m_loggedUsers.end();
	const bool user_dont_exists = !this->m_database->doesUserExist(username);
	const bool password_dont_match = !this->m_database->doesPasswordMatch(username, password);

	if (found_user)
	{
		throw std::exception("User already logged in");
	}
	else if (user_dont_exists)
	{
		throw std::exception("User is not signed up");
	}
	else if (password_dont_match)
	{
		throw std::exception("Username or Password do not match");
	}

	this->m_loggedUsers.push_back(LoggedUser(username));
}
void LoginManager::logout(const std::string& username)
{
	std::lock_guard<std::mutex> lg(this->m_lock);
	auto it = std::find(this->m_loggedUsers.begin(), this->m_loggedUsers.end(), LoggedUser(username));

	if (it != this->m_loggedUsers.end())
	{
		this->m_loggedUsers.erase(it);
	}
}