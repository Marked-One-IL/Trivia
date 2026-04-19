#pragma once
#include "Game.h"
#include "IDataBase.h"
#include "GameManager.h"
#include <mutex>

class GameManager
{
public:
	GameManager();
	GameManager(IDataBase* DB);
	Game createGame(const Room& room);
	void deleteGame(unsigned int gameId);
	Game& getGame(unsigned int id);

private:
	IDataBase* m_database;
	std::vector<Game> m_games;
	std::mutex m_mtx;
};