#include "StatisticsManager.h"

StatisticsManager::StatisticsManager() : m_database(nullptr)
{
}
StatisticsManager::StatisticsManager(IDataBase* db) : m_database(db)
{
}

std::vector<UserScore> StatisticsManager::getHighScore()
{
	return this->m_database->getHighScores();
}

std::vector<std::string> StatisticsManager::getUserStatistics(const std::string& username)
{
	std::vector<std::string> result;

	result.push_back(std::to_string(this->m_database->getNumOfPlayerGames(username)));
	result.push_back(std::to_string(this->m_database->getNumOfCorrectAnswers(username)));
	result.push_back(std::to_string(this->m_database->getNumOfTotalAnswers(username)));
	result.push_back(std::to_string(this->m_database->getPlayerAverageAnswerTime(username)));
	result.push_back(std::to_string(this->m_database->getPlayerScore(username)));

	return result;
}
