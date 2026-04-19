#include "RoomMemberRequestHandler.h"
#include "safe.h" 

RoomMemberRequestHandler::RoomMemberRequestHandler(LoggedUser& user, Room& room, RoomManager& roomManager, RequestHandlerFactory& handlerFactory):
	m_user(user), m_room(room), m_roomManager(roomManager), m_handlerFactory(handlerFactory)
{
}
bool RoomMemberRequestHandler::isRequestRelevant(const RequestInfo& info) const
{
	if (info.buffer[0] == client::LEAVE_ROOM || info.buffer[0] == client::GET_ROOM_STATE)
	{
		return true;
	}
	return false;
}

RequestResult RoomMemberRequestHandler::handleRequest(const RequestInfo& info)
{
	RequestResult result;

	if (this->isRequestRelevant(info))
	{
		try
		{
			if (info.buffer[0] == client::LEAVE_ROOM)
			{
				return this->leaveRoom(info);
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

void RoomMemberRequestHandler::logout(void)
{
	this->m_handlerFactory.getLoginManager().logout(this->m_user.getUsername());
	this->m_handlerFactory.getRoomManager().removeUser(this->m_user.getUsername());
}

RequestResult RoomMemberRequestHandler::getRoomState(const RequestInfo& info)
{
	GetRoomStateResponse response;
	RequestResult result;
	RoomData roomData = this->m_handlerFactory.getRoomManager().getRoom(this->m_room.getRoomData().id).getRoomData();

	response.answerTimeout = roomData.timePerQuestion;
	response.maxPlayers = roomData.maxPlayers;
	response.questionCount = roomData.numOfQuestionsInGame;

	std::vector<LoggedUser> users = this->m_handlerFactory.getRoomManager().getRoom(this->m_room.getRoomData().id).getAllUsers();
	for (int i = 0; i < users.size(); i++)
	{
		response.players.push_back(users[i].getUsername());
	}

	response.status = 1;
	response.room_status = roomData.status;

	if (roomData.status == room_status::IN_GAME)
	{
		response.hasGameBegun = true;
		Game game = this->m_handlerFactory.getGameManager().createGame(this->m_room);
		result.newHandler = this->m_handlerFactory.CreateGameRequestHandler(this->m_user, game);
	}
	else if (roomData.status == room_status::OPEN)
	{
		response.hasGameBegun = false;
		result.response = JsonResponsePacketSerializer::serializeResponse(response);
		result.newHandler = nullptr;
	}
	else if (roomData.status == room_status::CLOSED)
	{
		this->m_handlerFactory.getRoomManager().removeUser(this->m_user.getUsername());
		result.response = JsonResponsePacketSerializer::serializeResponse(response);
		result.newHandler = this->m_handlerFactory.createMenuRequestHandler(this->m_user);
		return result;
	}

	result.response = JsonResponsePacketSerializer::serializeResponse(response);

	return result;
}

RequestResult RoomMemberRequestHandler::leaveRoom(const RequestInfo& info)
{
	RequestResult result;
	result.newHandler = this->m_handlerFactory.createMenuRequestHandler(this->m_user);
	result.response = JsonResponsePacketSerializer::serializeResponse(LeaveRoomResponse{ 1 });

	this->m_handlerFactory.getRoomManager().removeUser(this->m_user.getUsername());
	return result;
}
