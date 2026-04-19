#include "LoggedUser.h"

LoggedUser::LoggedUser(const std::string& username) : m_username(username)
{
}
LoggedUser::LoggedUser(const LoggedUser& logged_user) : LoggedUser(logged_user.getUsername())
{
}
LoggedUser::LoggedUser()
{
}
std::string LoggedUser::getUsername(void) const
{
	return this->m_username;
}
bool LoggedUser::operator < (const LoggedUser& other) const
{
	return this->m_username < other.getUsername();
}
bool LoggedUser::operator == (const LoggedUser& other) const
{
	return this->m_username == other.getUsername();
}