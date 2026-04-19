#pragma once
#include "IRequestHandler.h"
#include "RequestHandlerFactory.h"

class RequestHandlerFactory;

class MenuRequestHandler : public IRequestHandler
{
public:
    MenuRequestHandler(const LoggedUser& user, RequestHandlerFactory& factoryHandler);
    virtual bool isRequestRelevant(const RequestInfo& info) const override;
    virtual RequestResult handleRequest(const RequestInfo& info) override;
    virtual void logout(void) override;

private:
    LoggedUser m_user;
    RequestHandlerFactory& m_handlerFactory;

    RequestResult signout();
    RequestResult getRooms(const RequestInfo& info);
    RequestResult getPlayersInRoom(const RequestInfo& info);
    RequestResult getPersonalStats(const RequestInfo& info);
    RequestResult getHighScore(const RequestInfo& info);
    RequestResult joinRoom(const RequestInfo& info);
    RequestResult createRoom(const RequestInfo& info);
};