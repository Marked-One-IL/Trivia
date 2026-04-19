#pragma once
#include <string>

class LoggedUser
{
public:
	LoggedUser(const std::string& username);
	LoggedUser(const LoggedUser& logged_user);
	LoggedUser();
	std::string getUsername(void) const;
	bool operator == (const LoggedUser& other) const; // For std::find().
	bool operator < (const LoggedUser& other) const; // For std::map::find().

private:
	std::string m_username;
};