#include "GameRequestHandler.h"
#include "safe.h"

GameRequestHandler::GameRequestHandler(LoggedUser& user, Game& game, GameManager& gameManager, RequestHandlerFactory& handlerFactory) : m_game(game), m_gameManager(gameManager), m_handlerFactory(handlerFactory), m_user(user)
{
}

bool GameRequestHandler::isRequestRelevant(const RequestInfo& info) const
{
	if (info.buffer[0] == client::SUBMIT_ANSWER)
	{
		try
		{
			JsonRequestPacketDeserializer::deserializeSubmitAnswerRequest(info.buffer);
			return true;
		}
		catch (...)
		{
			return false;
		}
	}
	else if (info.buffer[0] == client::GET_QUESTION || info.buffer[0] == client::GET_GAME_RESULTS || info.buffer[0] == client::LEAVE_GAME || info.buffer[0] == client::ARE_ALL_PLAYERS_DONE)
	{
		return true;
	}
	return false;
}

RequestResult GameRequestHandler::handleRequest(const RequestInfo& info)
{
	RequestResult result;

	if (this->isRequestRelevant(info))
	{
		try
		{
			switch (info.buffer[0])
			{
			case client::SUBMIT_ANSWER:
				return this->submitAnswer(info);
			case client::GET_QUESTION:
				return this->getQuestion(info);
			case client::GET_GAME_RESULTS:
				return this->getGameResult(info);
			case client::LEAVE_GAME:
				return this->leaveGame(info);
			case client::ARE_ALL_PLAYERS_DONE:
				return this->areAllPlayersDone(info);
			}
		}
		catch (const std::exception& e)
		{
			safe::println("{}", e.what());
			result.newHandler = nullptr;
			result.response = JsonResponsePacketSerializer::serializeResponse(ErrorResponse{ e.what() });
		}
	}
	else
	{
		result.newHandler = nullptr;
		result.response = JsonResponsePacketSerializer::serializeResponse(ErrorResponse{ "You message is not formated correctly" });
	}

	return result;
}

void GameRequestHandler::logout(void)
{
	this->m_handlerFactory.getLoginManager().logout(this->m_user.getUsername());
	this->m_handlerFactory.getRoomManager().removeUser(this->m_user.getUsername());

	try
	{
		this->m_gameManager.getGame(this->m_game.getGameId()).removePlayer(this->m_user);
	}
	catch (...) {}
}

RequestResult GameRequestHandler::getQuestion(const RequestInfo& info)
{
	RequestResult result;
	result.newHandler = nullptr;

	Question newQuestion = this->m_gameManager.getGame(this->m_game.getGameId()).getNextQuestionForUser(this->m_user);

	GetQuestionResponse response;
	response.answers = newQuestion.getPossibleAnswers(); 
	response.question = newQuestion.getQuestion();
	response.status = 1;

	result.response = JsonResponsePacketSerializer::serializeResponse(response);

	return result;
}

RequestResult GameRequestHandler::submitAnswer(const RequestInfo& info)
{
	SubmitAnswerRequest request = JsonRequestPacketDeserializer::deserializeSubmitAnswerRequest(info.buffer);
	RequestResult result;
	
	this->m_gameManager.getGame(this->m_game.getGameId()).submitAnswer(request.answerId, this->m_user);

	SubmitAnswerResponse response;
	response.status = 1;
	response.correctAnswerId = this->m_gameManager.getGame(this->m_game.getGameId()).getGameData(this->m_user).currentQuestion.getCorrectIndex();

	result.newHandler = nullptr;
	result.response = JsonResponsePacketSerializer::serializeResponse(response);

	return result;
}

RequestResult GameRequestHandler::getGameResult(const RequestInfo& info)
{
	RequestResult result;
	result.newHandler = this->m_handlerFactory.createMenuRequestHandler(this->m_user);

	GetGameResultsResponse response;
	std::vector<PlayerResults> playerResults;
	std::map<LoggedUser, GameData> data = this->m_gameManager.getGame(this->m_game.getGameId()).getAllData();

	for (auto it = data.begin(); it != data.end(); ++it)
	{
		PlayerResults playerResult;

		playerResult.avrageAnswerTime = it->second.avgAnswerTime;
		playerResult.correctAnswerCount = it->second.correctAnswerCount;
		playerResult.wrongAnswerCount = it->second.wrongAnswerCount;
		playerResult.username = it->first.getUsername();

		playerResults.push_back(playerResult);
	}
	response.status = 1;
	response.results = playerResults;

	result.response = JsonResponsePacketSerializer::serializeResponse(response);

	this->m_gameManager.getGame(this->m_game.getGameId()).delete_game();
	this->m_handlerFactory.getRoomManager().removeUser(this->m_user.getUsername());

	return result;
}

RequestResult GameRequestHandler::leaveGame(const RequestInfo& info)
{
	RequestResult result;
	result.newHandler = this->m_handlerFactory.createMenuRequestHandler(this->m_user);
	this->m_gameManager.getGame(this->m_game.getGameId()).removePlayer(this->m_user);
	this->m_handlerFactory.getRoomManager().removeUser(this->m_user.getUsername());

	result.response = JsonResponsePacketSerializer::serializeResponse(LeaveGameResponse{ 1 });
	return result;
}

RequestResult GameRequestHandler::areAllPlayersDone(const RequestInfo& info)
{
	RequestResult result;

	result.newHandler = nullptr;
	
	AreAllPlayersDoneResponse response;
	response.status = 1;
	response.areTheyDone = this->m_gameManager.getGame(this->m_game.getGameId()).areAllPlayersDone();

	result.response = JsonResponsePacketSerializer::serializeResponse(response);
	
	return result;
}
