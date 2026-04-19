#pragma once
#include "IDataBase.h"
#include "sqlite3.h"
#include <functional>
#include "json.hpp"

#define DB_FILENAME "data.db"

class SqliteDataBase : public IDataBase
{
public:
	SqliteDataBase(void);
	virtual ~SqliteDataBase(void) override;
	virtual bool doesUserExist(const std::string& username) override; // No const because the compiler think it could affect m_db.
	virtual bool doesPasswordMatch(const std::string& username, const std::string& password) override; // No const because the compiler think it could affect m_db.
	virtual void addNewUser(const std::string& email, const std::string& username, const std::string& password) override;
	virtual std::vector<Question> getQuestions(unsigned int questions_num) override;
	virtual float getPlayerAverageAnswerTime(const std::string& username) override;
	virtual unsigned int getNumOfCorrectAnswers(const std::string& username) override;
	virtual unsigned int getNumOfTotalAnswers(const std::string& username) override;
	virtual unsigned int getNumOfPlayerGames(const std::string& username) override;
	virtual unsigned int getPlayerScore(const std::string& username) override;
	virtual std::vector<UserScore> getHighScores(void) override;
	virtual void submitGameStatistics(const GameData& data, const LoggedUser& logged_user) override;

private:
	void exec_sql(const char* command);
	void exec_sql(const std::string& command);
	void exec_sql(const char* command, std::function<int(void*, int, char**, char**)> callback);
	void exec_sql(const std::string& command, std::function<int(void*, int, char**, char**)> callback);
	static int callback_wrapper(void* data, int argc, char** argv, char** colName);
	void addNewQuestion(const Question& question);
	bool doesTableExists(const char* tableName);
	static nlohmann::json getTenQuestions(void);
	static void base64_decode(std::string& input);
	static unsigned int calc_score(unsigned int correct_answers, float average_time, unsigned int total_answers);

	sqlite3* m_db;
};