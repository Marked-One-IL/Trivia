#pragma once
#include "RequestHandlerFactory.h"
#include "CodeConstants.h"
#include "JsonResponsePacketSerializer.h"

class RequestHandlerFactory;

class RoomMemberRequestHandler : public IRequestHandler
{
public:
	RoomMemberRequestHandler(LoggedUser& user, Room& room, RoomManager& roomManager, RequestHandlerFactory& handlerFactory);
	virtual bool isRequestRelevant(const RequestInfo& info) const override;
	virtual RequestResult handleRequest(const RequestInfo& info) override;
	virtual void logout(void) override;
private:
	RequestResult leaveRoom(const RequestInfo& info);
	RequestResult getRoomState(const RequestInfo& info);


	Room m_room;
	LoggedUser m_user;
	RoomManager& m_roomManager;
	RequestHandlerFactory& m_handlerFactory;
};