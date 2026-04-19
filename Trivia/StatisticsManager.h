#pragma once
#include "SqliteDataBase.h"

class StatisticsManager
{
public:
	StatisticsManager();
	StatisticsManager(IDataBase* db);
	std::vector<UserScore> getHighScore();
	std::vector<std::string> getUserStatistics(const std::string& username);

private:
	IDataBase* m_database;
};