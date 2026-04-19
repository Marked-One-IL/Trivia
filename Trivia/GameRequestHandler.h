#pragma once
#include "RequestHandlerFactory.h"
#include "JsonRequestPacketDeserializer.h"
#include "GameManager.h"

class RequestHandlerFactory;

class GameRequestHandler : public IRequestHandler
{
public:
	GameRequestHandler(LoggedUser& user, Game& game, GameManager& gameManager, RequestHandlerFactory& handlerFactory);
	virtual bool isRequestRelevant(const RequestInfo& info) const override;
	virtual RequestResult handleRequest(const RequestInfo& info) override;
	virtual void logout(void);

private:
	RequestResult getQuestion(const RequestInfo& info);
	RequestResult submitAnswer(const RequestInfo& info);
	RequestResult getGameResult(const RequestInfo& info);
	RequestResult leaveGame(const RequestInfo& info);
	RequestResult areAllPlayersDone(const RequestInfo& info);

	Game m_game;
	LoggedUser m_user;
	GameManager& m_gameManager;
	RequestHandlerFactory& m_handlerFactory;
	std::map<GameData, LoggedUser> m_permData;
	unsigned int m_userCount;
};