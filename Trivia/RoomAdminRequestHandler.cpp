#include "RoomAdminRequestHandler.h"
#include "CodeConstants.h"
#include "JsonResponsePacketSerializer.h"
#include "safe.h"
#include <iterator>

RoomAdminRequestHandler::RoomAdminRequestHandler(LoggedUser& user, Room& room, RoomManager& roomManager, RequestHandlerFactory& handlerFactory) : 
	m_user(user), m_room(room), m_roomManager(roomManager), m_handlerFactory(handlerFactory)
{
}

bool RoomAdminRequestHandler::isRequestRelevant(const RequestInfo& info) const
{
	if (info.buffer[0] == client::CLOSE_ROOM || info.buffer[0] == client::START_GAME || info.buffer[0] == client::GET_ROOM_STATE)
	{
		return true;
	}
	return false;
}

RequestResult RoomAdminRequestHandler::handleRequest(const RequestInfo& info)
{
	RequestResult result;

	if (this->isRequestRelevant(info))
	{
		try
		{
			if (info.buffer[0] == client::CLOSE_ROOM)
			{
				return this->closeRoom(info);
			}
			else if (info.buffer[0] == client::START_GAME)
			{
				return this->startGame(info);
			}
			else if (info.buffer[0] == client::GET_ROOM_STATE)
			{
				return this->getRoomState(info);
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

void RoomAdminRequestHandler::logout(void)
{
	this->m_handlerFactory.getLoginManager().logout(this->m_user.getUsername());
	this->m_handlerFactory.getRoomManager().removeUser(this->m_user.getUsername());
	this->m_handlerFactory.getRoomManager().deleteRoom(this->m_room.getRoomData().id);
	this->m_handlerFactory.getRoomManager().removeUser(this->m_user.getUsername());
}

RequestResult RoomAdminRequestHandler::closeRoom(const RequestInfo& info)
{
	RequestResult result;
	result.newHandler = this->m_handlerFactory.createMenuRequestHandler(this->m_user);
	result.response = JsonResponsePacketSerializer::serializeResponse(CloseRoomReponse{ 1 });

	this->m_handlerFactory.getRoomManager().deleteRoom(this->m_room.getRoomData().id);
	this->m_handlerFactory.getRoomManager().removeUser(this->m_user.getUsername());

	return result;
}

RequestResult RoomAdminRequestHandler::startGame(const RequestInfo& info)
{
	RequestResult result;
	Game game = this->m_handlerFactory.getGameManager().createGame(this->m_roomManager.getRoom(this->m_room.getRoomData().id));
	result.newHandler = this->m_handlerFactory.CreateGameRequestHandler(this->m_user, game);
	result.response = JsonResponsePacketSerializer::serializeResponse(StartGameReponse{ 1 });

	this->m_handlerFactory.getRoomManager().startGame(this->m_room.getRoomData().id);
	return result;
}

RequestResult RoomAdminRequestHandler::getRoomState(const RequestInfo& info)
{
	GetRoomStateResponse response;
	RoomData roomData = this->m_handlerFactory.getRoomManager().getRoom(this->m_room.getRoomData().id).getRoomData();
	
	std::vector<LoggedUser> users = this->m_handlerFactory.getRoomManager().getRoom(this->m_room.getRoomData().id).getAllUsers();
	for (int i = 0; i < users.size(); i++)
	{
		response.players.push_back(users[i].getUsername());
	}

	response.answerTimeout = roomData.timePerQuestion;
	response.questionCount = roomData.numOfQuestionsInGame;
	response.status = 1;
	response.room_status = roomData.status;
	response.maxPlayers = roomData.maxPlayers;

	if (roomData.status == room_status::IN_GAME)
	{
		response.hasGameBegun = true;
	}
	else
	{
		response.hasGameBegun = false;
	}

	RequestResult result;
	result.response = JsonResponsePacketSerializer::serializeResponse(response);
	result.newHandler = nullptr;

	return result;
}
