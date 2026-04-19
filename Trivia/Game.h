#pragma once
#include "Question.h"
#include <map>
#include "LoggedUser.h"
#include "IDataBase.h"
#include <mutex>

class IDataBase;

// For the CL Compiler creators.
// I swear to God I will come to your house and fucking kill you.
// How senior developers like you managed to create the most dog shit compiler ever.
// The error messages require a Monolithian God to fucking understand.

struct GameData
{
	// Those constructors are essentials. Without them the compiler will cry like a little boy.
	GameData(void);
	GameData(const Question& p_question, unsigned int p_correctAnswerCount, unsigned int p_wrongAnswerCount, float p_averageAnswerTime, std::time_t p_startTime);
	bool operator < (const GameData& other) const; // For std::map::find().

	Question currentQuestion;
	unsigned int correctAnswerCount;
	unsigned int wrongAnswerCount;
	float avgAnswerTime;
	std::time_t startTime;
	bool isSubmitted;
};

class Game
{
public:
	Game(const Room& room, IDataBase* DB);
	Game(const Game& game);
	Game& operator=(const Game& game);

	void delete_game(void);

	Question getNextQuestionForUser(const LoggedUser& user);
	void submitAnswer(int answer, const LoggedUser& user);
	void removePlayer(const LoggedUser& user);
	unsigned int getGameId() const;
	GameData getGameData(const LoggedUser& user);
	std::map<LoggedUser, GameData> getAllData();
	bool areAllPlayersDone();

private:
	void submitGameStatsToDB(const GameData& data, const LoggedUser& logged_user);


	std::map<LoggedUser, GameData> m_players;
	std::mutex playersMTX;

	std::map<LoggedUser, std::time_t> m_questionsSent;
	std::mutex questionsSentMTX;

	std::vector<Question> m_question;
	unsigned int m_gameId;
	IDataBase* m_database;

	unsigned int m_deleteCount;
	unsigned int m_submittedCount;
};