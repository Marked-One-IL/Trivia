#include "GameManager.h"
#include <algorithm>

GameManager::GameManager()
{
}

GameManager::GameManager(IDataBase* DB) : m_database(DB)
{
}

Game GameManager::createGame(const Room& room)
{
	std::lock_guard<std::mutex> lock(this->m_mtx);

	for (auto it = this->m_games.begin(); it != this->m_games.end(); ++it)
	{
		if (it->getGameId() == room.getRoomData().id)
		{
			return *it;
		}
	}
	this->m_games.emplace_back(room, this->m_database);
	return this->m_games.back();
}

void GameManager::deleteGame(unsigned int gameId)
{
	std::lock_guard<std::mutex> lock(this->m_mtx);

	for (auto it = this->m_games.begin(); it != this->m_games.end(); ++it)
	{
		if (gameId == it->getGameId())
		{
			it->delete_game();
			this->m_games.erase(it);
			return;
		}
	}
	throw std::runtime_error("Game not found");
}

Game& GameManager::getGame(unsigned int id)
{
	std::lock_guard<std::mutex> lock(this->m_mtx);

	for (unsigned int i = 0; i < this->m_games.size(); ++i)
	{
		if (this->m_games[i].getGameId() == id)
		{
			return this->m_games[i];
		}
	}

	throw std::exception("Game is closed");
}
