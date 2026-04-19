#include "Game.h"
#include <iostream>
#include <ctime>

Game::Game(const Room& room, IDataBase* DB) : m_database(DB), m_deleteCount(0), m_submittedCount(0)
{
    this->m_gameId = room.getRoomData().id;
    this->m_question = this->m_database->getQuestions(room.getRoomData().numOfQuestionsInGame);
    std::vector<LoggedUser> userVec = room.getAllUsers();

    for (int i = 0; i < userVec.size(); i++)
    {
        std::vector<std::string> preAnswers;
        std::string name = "preplacement";
        Question preplacement(name, preAnswers, 3);
        this->m_players.insert({ userVec[i], GameData(preplacement, 0, 0, 0 , std::time(nullptr)), });
        this->m_questionsSent.insert({ userVec[i], 0 });
    }
}

Game::Game(const Game& game) : m_deleteCount(0), m_submittedCount(0)
{
    this->m_players = game.m_players;
    this->m_questionsSent = game.m_questionsSent;
    this->m_gameId = game.m_gameId;
    this->m_database = game.m_database;
    this->m_question = game.m_question;
}

Game& Game::operator=(const Game& game)
{
    this->m_players = game.m_players;
    this->m_questionsSent = game.m_questionsSent;
    this->m_gameId = game.m_gameId;
    this->m_database = game.m_database;
    this->m_question = game.m_question;
    this->m_submittedCount = 0;
    return *this;
}

void Game::delete_game(void)
{
    std::lock_guard<std::mutex> mtxGuard(this->questionsSentMTX);

    ++this->m_deleteCount;

    if (this->m_deleteCount < this->m_players.size() - this->m_submittedCount)
    {
        return;
    }

    for (unsigned int i = 0; i < this->m_players.size();)
    {
        auto it = this->m_players.begin();
        std::advance(it, i);
        if (it->second.isSubmitted == false)
        {
            this->removePlayer(it->first);
        }
        this->m_players.erase(it);
    }
}

Question Game::getNextQuestionForUser(const LoggedUser& user)
{
    std::lock_guard<std::mutex> mtxGuard(this->questionsSentMTX);

    auto it = this->m_players.find(user);

    if (it == this->m_players.end())
    {
        throw std::runtime_error("User not in game");
    }

    if (this->m_questionsSent[user] == 0)
    {
        this->m_questionsSent[user] = std::time(nullptr);
    }
    char buffer[26];
    ctime_s(buffer, sizeof(buffer), &this->m_questionsSent[user]);
    std::cout << "question sent to user - " << user.getUsername() << " at " << buffer;

    std::string currQuestion = it->second.currentQuestion.getQuestion();

    if (currQuestion == "preplacement")
    {
        this->m_players[user].currentQuestion = this->m_question[0];
        return this->m_question[0];
    }

    unsigned int que_pos = std::distance(this->m_question.begin(), std::find(this->m_question.begin(), this->m_question.end(), it->second.currentQuestion));

    if (que_pos == this->m_question.size() - 1)
    {
        return it->second.currentQuestion;
    }

    que_pos++;
    it->second.currentQuestion = this->m_question[que_pos];

    return this->m_question[que_pos];
}

void Game::submitAnswer(int answer, const LoggedUser& user)
{
    std::lock_guard<std::mutex> mtxGuard(this->playersMTX);

    constexpr int8_t GAME_FINISHED = -2;

    auto it = this->m_players.find(user);

    char buffer[26];
    std::time_t temp = std::time(nullptr);
    ctime_s(buffer, sizeof(buffer), &temp);
    std::cout << "question answered by user - " << user.getUsername() << " at " << buffer;

    if (it == this->m_players.end())
    {
        throw std::runtime_error("User not in game");
    }

    unsigned int que_pos = std::distance(this->m_question.begin(), std::find(this->m_question.begin(), this->m_question.end(), it->second.currentQuestion)) + 1;

    if (it->second.correctAnswerCount + it->second.wrongAnswerCount >= que_pos)
    {
        return;
    }

    
    
    float diff = (float)(std::difftime(std::time(nullptr), this->m_questionsSent[user]));
    if (answer == 2)
    {
        int i = 0;
    }
    unsigned int totalAnswers = it->second.correctAnswerCount + it->second.wrongAnswerCount;
    it->second.avgAnswerTime = (it->second.avgAnswerTime * totalAnswers + diff) / (totalAnswers + 1);
    std::string currQuestion = it->second.currentQuestion.getQuestion();
   

    if (answer == it->second.currentQuestion.getCorrectIndex())
    {
        it->second.correctAnswerCount++;
    }
    else if (answer != GAME_FINISHED && it->second.wrongAnswerCount < this->m_question.size())
    {
        it->second.wrongAnswerCount++;
    }
    this->m_questionsSent[user] = 0;
}

void Game::removePlayer(const LoggedUser& user)
{
    auto it = this->m_players.find(user);

    if (it == this->m_players.end())
    {
        throw std::runtime_error("User not in game");
    }

    this->submitGameStatsToDB(it->second, it->first);
    it->second.isSubmitted = true;
    this->m_submittedCount++;
}

unsigned int Game::getGameId() const
{
    return this->m_gameId;
}

GameData Game::getGameData(const LoggedUser& user)
{
    std::lock_guard<std::mutex> mtxGuard(this->playersMTX);
    return this->m_players[user];
}

std::map<LoggedUser, GameData> Game::getAllData()
{
    std::lock_guard<std::mutex> mtxGuard(this->questionsSentMTX);

    return this->m_players;
}

bool Game::areAllPlayersDone()
{
    std::lock_guard<std::mutex> mtx(this->playersMTX);

    for (auto it = this->m_players.begin(); it != this->m_players.end(); ++it)
    {
        if ((it->second.correctAnswerCount + it->second.wrongAnswerCount) != this->m_question.size() && !it->second.isSubmitted)
        {
            return false;
        }
    }
    return true;
}

void Game::submitGameStatsToDB(const GameData& data, const LoggedUser& logged_user)
{
    this->m_database->submitGameStatistics(data, logged_user);
}

GameData::GameData(void)
    : currentQuestion("", {}, 0), correctAnswerCount(0), wrongAnswerCount(0), avgAnswerTime(0), startTime(0)
{
}

GameData::GameData(const Question& p_question, unsigned int p_correctAnswerCount, unsigned int p_wrongAnswerCount, float p_averageAnswerTime, std::time_t p_startTime)
    : currentQuestion(p_question), correctAnswerCount(p_correctAnswerCount), wrongAnswerCount(p_wrongAnswerCount), avgAnswerTime(p_averageAnswerTime), startTime(p_startTime)
{
    this->isSubmitted = false;
}

bool GameData::operator < (const GameData& other) const
{
    return this->startTime < other.startTime;
}