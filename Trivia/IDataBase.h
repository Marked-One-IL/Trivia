#pragma once
#include <string>
#include <list>
#include <vector>
#include "Question.h"
#include "Room.h"
#include "Game.h"

struct GameData;
class LoggedUser;
struct UserScore;
class Room;

class IDataBase
{
public:
	virtual ~IDataBase() = default;
	virtual bool doesUserExist(const std::string& username) = 0;
	virtual bool doesPasswordMatch(const std::string& username, const std::string& password) = 0;
	virtual void addNewUser(const std::string& email, const std::string& username, const std::string& password) = 0;
	virtual std::vector<Question> getQuestions(unsigned int questions_num) = 0;
	virtual float getPlayerAverageAnswerTime(const std::string& username) = 0;
	virtual unsigned int getNumOfCorrectAnswers(const std::string& username) = 0;
	virtual unsigned int getNumOfTotalAnswers(const std::string& username) = 0;
	virtual unsigned int getNumOfPlayerGames(const std::string& username) = 0;
	virtual unsigned int getPlayerScore(const std::string& username) = 0;
	virtual std::vector<UserScore> getHighScores(void) = 0;
	virtual void submitGameStatistics(const GameData& data, const LoggedUser& logged_user) = 0;
};