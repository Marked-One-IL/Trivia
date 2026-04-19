#pragma comment(lib, "winhttp.lib")
#include <windows.h>
#include <winhttp.h>

#include "SqliteDataBase.h"
#include <iostream>
#include <exception>
#include <format> // Only available at newer versions of C++.
#include <algorithm> 
#include <random>
#include <ctime>

// Used this library HTTPS in the end because using other libraries required installing extra programs.
// Which made it difficult to link and make it runnable on other users.

nlohmann::json SqliteDataBase::getTenQuestions(void)
{
	HINTERNET hSession = NULL;
	HINTERNET hConnect = NULL;
	HINTERNET hRequest = NULL;
	nlohmann::json result;

	try 
	{
		// Init WinHTTP.
		hSession = WinHttpOpen(L"WinHTTP Example/1.0",
			WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
			WINHTTP_NO_PROXY_NAME,
			WINHTTP_NO_PROXY_BYPASS, 0);
		if (!hSession) 
		{
			throw std::runtime_error("WinHttpOpen failed: " + std::to_string(GetLastError()));
		}

		// Create HTTPS connection.
		hConnect = WinHttpConnect(hSession, L"opentdb.com",
			INTERNET_DEFAULT_HTTPS_PORT, 0);
		if (!hConnect) 
		{
			throw std::runtime_error("WinHttpConnect failed: " + std::to_string(GetLastError()));
		}

		// Create HTTP request.
		hRequest = WinHttpOpenRequest(hConnect, L"GET",
			L"/api.php?amount=10&type=multiple&encode=base64",
			NULL, WINHTTP_NO_REFERER,
			WINHTTP_DEFAULT_ACCEPT_TYPES,
			WINHTTP_FLAG_SECURE);
		if (!hRequest) 
		{
			throw std::runtime_error("WinHttpOpenRequest failed: " + std::to_string(GetLastError()));
		}

		// Send the request.
		if (!WinHttpSendRequest(hRequest,
			WINHTTP_NO_ADDITIONAL_HEADERS, 0,
			WINHTTP_NO_REQUEST_DATA, 0,
			0, 0)) 
		{
			throw std::runtime_error("WinHttpSendRequest failed: " + std::to_string(GetLastError()));
		}

		// Receive the response.
		if (!WinHttpReceiveResponse(hRequest, NULL)) 
		{
			throw std::runtime_error("WinHttpReceiveResponse failed: " + std::to_string(GetLastError()));
		}

		// Check status code.
		DWORD statusCode = 0;
		DWORD statusCodeSize = sizeof(statusCode);
		if (!WinHttpQueryHeaders(hRequest,
			WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
			WINHTTP_HEADER_NAME_BY_INDEX,
			&statusCode, &statusCodeSize,
			WINHTTP_NO_HEADER_INDEX)) {
			throw std::runtime_error("WinHttpQueryHeaders failed: " + std::to_string(GetLastError()));
		}

		if (statusCode != 200) // 200 means success.
		{
			throw std::runtime_error("HTTPS request failed with status: " + std::to_string(statusCode));
		}

		// Read response data.
		std::string responseBody;
		DWORD bytesRead = 0;
		char buffer[4096]; // Should be enough data.
		do 
		{
			if (!WinHttpReadData(hRequest, buffer, sizeof(buffer), &bytesRead)) 
			{
				throw std::runtime_error("WinHttpReadData failed: " + std::to_string(GetLastError()));
			}
			if (bytesRead > 0) 
			{
				responseBody.append(buffer, bytesRead);
			}
		} while (bytesRead > 0);

		try 
		{
			result = nlohmann::json::parse(responseBody);

			if (result["response_code"] != 0)
			{
				throw std::runtime_error("API call returned failure");
			}
		}
		catch (const nlohmann::json::exception&) 
		{
			throw std::runtime_error("HTTPS JSON parsing failed");
		}
	}
	catch (const std::exception& error) 
	{
		if (hRequest) WinHttpCloseHandle(hRequest);
		if (hConnect) WinHttpCloseHandle(hConnect);
		if (hSession) WinHttpCloseHandle(hSession);
		throw error;
	}

	// Clean up
	if (hRequest) WinHttpCloseHandle(hRequest);
	if (hConnect) WinHttpCloseHandle(hConnect);
	if (hSession) WinHttpCloseHandle(hSession);

	return result;
}

void SqliteDataBase::base64_decode(std::string& input) 
{
	const std::string table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	std::vector<unsigned char> decoded;
	int val = 0, valb = -8;

	// Decode from Base64 to a string.
	for (unsigned char c : input) 
	{
		if (c == '=' || table.find(c) == std::string::npos) break;

		val = (val << 6) + static_cast<int>(table.find(c));
		valb += 6;

		if (valb >= 0) 
		{
			decoded.push_back((val >> valb) & 0xFF);
			valb -= 8;
		}
	}

	input.assign(decoded.begin(), decoded.end());

	// Remove chars that can cause errors in sqlite eg: (').
	const std::string chars_to_remove = "'\";--";
	input.erase(std::remove_if(input.begin(), input.end(),
		[&chars_to_remove](char c) {
			return chars_to_remove.find(c) != std::string::npos;
		}), input.end());
}

SqliteDataBase::SqliteDataBase(void)
{
	if (sqlite3_open(DB_FILENAME, &this->m_db))
	{
		this->m_db = nullptr;
		throw std::exception("Could not open " DB_FILENAME);
	}

	// Table 1: user.
	// Table 2: statistics (Connected to user by username).
	// Table 3: question.

	this->exec_sql("PRAGMA foreign_keys = ON;");

	this->exec_sql("CREATE TABLE IF NOT EXISTS user ( "
				   "username TEXT PRIMARY KEY NOT NULL, "
				   "email TEXT NOT NULL, "
				   "password TEXT NOT NULL);");

	this->exec_sql("CREATE TABLE IF NOT EXISTS statistics ( "
				   "owner_username TEXT PRIMARY KEY NOT NULL," // Connect to user Table.
				   "answers INTEGER DEFAULT 0, " // Questions answered in general (not necessarily correct)
				   "correct_answers INTEGER DEFAULT 0, " // Correct questions answered in general.
				   "games INTEGER DEFAULT 0, " // How many times did he played a game (match).
				   "score INTEGER DEFAULT 0, " // How much times did the player answered correctly in a game (temporarily).
				   "time INTEGER DEFAULT 0, " // Total time spend playing.
				   "FOREIGN KEY (owner_username) REFERENCES user(username) ON DELETE CASCADE);");

	// Adds the questions only on the first time.
	if (!this->doesTableExists("question"))
	{
		this->exec_sql("CREATE TABLE question ("
					   "the_question TEXT PRIMARY KEY NOT NULL, "
					   "answer0 TEXT NOT NULL, "
					   "answer1 TEXT NOT NULL, "
					   "answer2 TEXT NOT NULL, "
					   "answer3 TEXT NOT NULL, "
					   "correct_answer_index INTEGER NOT NULL);");
 
		std::cout << "Getting 10 questions from opentdb.com" << std::endl;
		std::cout << std::endl;

		nlohmann::json questions = SqliteDataBase::getTenQuestions();
		auto begin = questions["results"].begin();
		auto end = questions["results"].end();

		std::random_device rd;
		std::mt19937 gen(rd());

		for (auto it = begin; it != end; ++it)
		{
			const auto& question = *it;

			// Extract all wrong answers and right answer and them all in a vector.
			// Shuffle them randomly.
			// Get correct answer index in vector.
			// Add new question to database.

			std::string correct_answer = question["correct_answer"].get<std::string>();
			SqliteDataBase::base64_decode(correct_answer);

			std::vector<std::string> answers;
			
			for (uint8_t i = 0; i < question["incorrect_answers"].size(); ++i)
			{
				std::string answer = question["incorrect_answers"][i];
				SqliteDataBase::base64_decode(answer);

				answers.push_back(answer);
			}

			answers.push_back(correct_answer);
			std::shuffle(answers.begin(), answers.end(), gen);
			uint8_t correct_answer_index = static_cast<uint8_t>(std::distance(answers.begin(), std::find(answers.begin(), answers.end(), correct_answer)));

			std::string corrected_question = question["question"];
			SqliteDataBase::base64_decode(corrected_question);

			this->addNewQuestion(Question(corrected_question, answers, correct_answer_index));

			// Print for debugging.
			// std::cout << "Question: " << corrected_question << std::endl;
			// std::cout << "Answers: " << std::endl;
			// 
			// for (uint8_t i = 0; i != answers.size(); ++i)
			// {
			//     std::cout << static_cast<uint16_t>(i) << ". " << answers[i] << std::endl;
			// }
			// 
			// std::cout << "Correct Answer: " << correct_answer << std::endl;
			// std::cout << "Correct Answer index: " << static_cast<uint16_t>(correct_answer_index) << std::endl;
			// std::cout << std::endl;
		}
	}
}
SqliteDataBase::~SqliteDataBase(void)
{
	sqlite3_close(this->m_db);
}
bool SqliteDataBase::doesUserExist(const std::string& username)
{
	bool doesExists = false;

	auto callback = [&doesExists](void* data, int argc, char** argv, char** colName) -> int
	{
		doesExists = true;
		return 0;
	};

	this->exec_sql(std::format("SELECT NULL FROM user WHERE username = '{}' LIMIT 1;", username), callback);
	return doesExists;
}
bool SqliteDataBase::doesPasswordMatch(const std::string& username, const std::string& password)
{
	bool doesMatch = false;

	auto callback = [&doesMatch](void* data, int argc, char** argv, char** colName) -> int
	{
		doesMatch = true;
		return 0;
	};

	this->exec_sql(std::format("SELECT NULL FROM user WHERE username = '{}' AND password = '{}' LIMIT 1;", username, password), callback);
	return doesMatch;
}
void SqliteDataBase::addNewUser(const std::string& email, const std::string& username, const std::string& password)
{
	this->exec_sql(std::format("INSERT INTO user (email, username, password) VALUES ('{}', '{}', '{}');", email, username, password));
	this->exec_sql(std::format("INSERT INTO statistics (owner_username) VALUES ('{}')", username));
}

std::vector<Question> SqliteDataBase::getQuestions(unsigned int questions_num)
{
	std::vector<Question> questions;
	constexpr uint8_t NUM_OF_ANSWERS = 4;

	auto callback = [&questions](void* data, int argc, char** argv, char** colName) -> int
	{
		std::string the_question = argv[0];
		std::vector<std::string> answers = { argv[1], argv[2], argv[3], argv[4] };
		uint8_t correct_answer_index = std::stoi(argv[5]);

		questions.push_back(Question(the_question, answers, correct_answer_index));
		return 0;
	};

	this->exec_sql(std::format("SELECT * from question LIMIT {};", questions_num), callback);
	return questions;
}
float SqliteDataBase::getPlayerAverageAnswerTime(const std::string& username)
{
	float time = 0;
	int answers = 0;

	auto callback = [&time, &answers](void* data, int argc, char** argv, char** colName) -> int
	{
		time = static_cast<float>(std::stoi(argv[0]));
		answers = std::stoi(argv[1]);
		return 0;
	};

	this->exec_sql(std::format("SELECT time, answers from statistics WHERE owner_username = '{}' LIMIT 1;", username), callback);

	if (answers == 0)
	{
		return 0;
	}

	return time / answers;
}
unsigned int SqliteDataBase::getNumOfCorrectAnswers(const std::string& username)
{
	unsigned int correctAnswers = 0;

	auto callback = [&correctAnswers](void* data, int argc, char** argv, char** colName) -> int
	{
		correctAnswers = std::stoi(argv[0]);
		return 0;
	};

	this->exec_sql(std::format("SELECT correct_answers FROM statistics WHERE owner_username = '{}' LIMIT 1;", username), callback);
	return correctAnswers;
}
unsigned int SqliteDataBase::getNumOfTotalAnswers(const std::string& username)
{
	unsigned int answers = 0;

	auto callback = [&answers](void* data, int argc, char** argv, char** colName) -> int
	{
		answers = std::stoi(argv[0]);
		return 0;
	};

	this->exec_sql(std::format("SELECT answers FROM statistics WHERE owner_username = '{}' LIMIT 1;", username), callback);
	return answers;
}
unsigned int SqliteDataBase::getNumOfPlayerGames(const std::string& username)
{
	unsigned int games = 0;

	auto callback = [&games](void* data, int argc, char** argv, char** colName) -> int
	{
		games = std::stoi(argv[0]);
		return 0;
	};

	this->exec_sql(std::format("SELECT games FROM statistics WHERE owner_username = '{}' LIMIT 1;", username), callback);
	return games;
}
unsigned int SqliteDataBase::getPlayerScore(const std::string& username)
{
	unsigned int score = 0;

	auto callback = [&score](void* data, int argc, char** argv, char** colName) -> int
	{
		score = std::stoi(argv[0]);
		return 0;
	};

	this->exec_sql(std::format("SELECT score FROM statistics WHERE owner_username = '{}' LIMIT 1;", username), callback);
	return score;
}
std::vector<UserScore> SqliteDataBase::getHighScores(void)
{
	std::vector<UserScore> user_scores;

	auto callback = [&user_scores](void* data, int argc, char** argv, char** colName) -> int
	{
		user_scores.emplace_back(argv[0], std::stoi(argv[1]));
		return 0;
	};

	this->exec_sql("SELECT owner_username, score FROM statistics ORDER BY score DESC LIMIT 5;", callback);

	return user_scores;
}

void SqliteDataBase::submitGameStatistics(const GameData& data, const LoggedUser& logged_user)
{
	this->exec_sql(std::format("UPDATE statistics SET "
							   "answers = answers + {}, "
							   "correct_answers = correct_answers + {}, "
							   "games = games + 1, "
						       "score = score + {}, "
							   "time = time + {} "
							   "WHERE owner_username = '{}';"

								,data.correctAnswerCount + data.wrongAnswerCount,
								data.correctAnswerCount,
								this->calc_score(data.correctAnswerCount, data.avgAnswerTime, data.correctAnswerCount + data.wrongAnswerCount),
								data.avgAnswerTime * (data.correctAnswerCount + data.wrongAnswerCount),
								logged_user.getUsername()));
}

void SqliteDataBase::addNewQuestion(const Question& question)
{
	std::vector<std::string> questions = question.getPossibleAnswers();
	uint8_t correct_answer_index = static_cast<uint8_t>(std::distance(questions.begin(), std::find(questions.begin(), questions.end(), question.getCorrectAnswer())));

	this->exec_sql(std::format("INSERT INTO question (the_question, answer0, answer1, answer2, answer3, correct_answer_index) VALUES "
							   "('{}', '{}', '{}', '{}', '{}', {});", 
								question.getQuestion(), questions[0], questions[1], questions[2], questions[3], correct_answer_index));
}
bool SqliteDataBase::doesTableExists(const char* tableName)
{
	bool exists = false;

	auto callback = [&exists](void* data, int argc, char** argv, char** colName) -> int
	{
		exists = true;
		return 0;
	};

	this->exec_sql(std::format("SELECT name FROM sqlite_master WHERE type = 'table' AND name= '{}' LIMIT 1;", tableName), callback);
	return exists;
}

unsigned int SqliteDataBase::calc_score(unsigned int correct_answers, float average_time, unsigned int total_answers)
{
	if (correct_answers == 0)
		return 0;

	float time_factor = (average_time > 0.0f) ? (10.0f / average_time) : 10.0f;
	float total_score = (correct_answers * time_factor * 10) / total_answers;

	return static_cast<unsigned int>(std::round(total_score));
}

// Simplfy the execution of a sql command.
void SqliteDataBase::exec_sql(const char* command)
{
	char* errorMsg = nullptr;
	if (sqlite3_exec(this->m_db, command, nullptr, nullptr, &errorMsg) != SQLITE_OK)
	{
		std::string error = errorMsg;
		sqlite3_free(errorMsg);
		throw std::runtime_error(error);
	}
}
// Simplfy the execution of a sql command (No need to use c_str()).
void SqliteDataBase::exec_sql(const std::string& command)
{
	this->exec_sql(command.c_str());
}
// Simplfy the execution of a sql command and simplfy the use of a lambda function.
void SqliteDataBase::exec_sql(const char* command, std::function<int(void*, int, char**, char**)> callback)
{
	char* errorMsg = nullptr;
	if (sqlite3_exec(this->m_db, command, callback_wrapper, &callback, &errorMsg) != SQLITE_OK)
	{
		std::string error = errorMsg;
		sqlite3_free(errorMsg);
		throw std::runtime_error(error);
	}
}
// Simplfy the execution of a sql command and simplfy the use of a lambda function (No need to use c_str()).
void SqliteDataBase::exec_sql(const std::string& command, std::function<int(void*, int, char**, char**)> callback)
{
	this->exec_sql(command.c_str(), callback);
}
// Make lambda function useable as a regular function (Used for passing a lambda instead of a function pointer).
int SqliteDataBase::callback_wrapper(void* data, int argc, char** argv, char** colName)
{
	// Cast it back to a reference to an std::function object.
	auto& callback = *static_cast<std::function<int(void*, int, char**, char**)>*>(data); // Ugly ass casting. But it simplfy the other code. So it worths it.
	return callback(data, argc, argv, colName); // Call the function object.
}