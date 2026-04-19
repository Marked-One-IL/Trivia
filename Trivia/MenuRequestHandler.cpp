#include "MenuRequestHandler.h"
#include "json.hpp"
#include "JsonResponsePacketSerializer.h"
#include "JsonRequestPacketDeserializer.h"
#include "CodeConstants.h"
#include <algorithm>
#include "safe.h"

MenuRequestHandler::MenuRequestHandler(const LoggedUser& user, RequestHandlerFactory& factoryHandler) : m_handlerFactory(factoryHandler), m_user(user)
{
}

bool MenuRequestHandler::isRequestRelevant(const RequestInfo& info) const
{
    if (info.buffer[0] == client::LOGOUT or info.buffer[0] == client::GET_ROOMS or info.buffer[0] == client::GET_PERSONAL_STATS or info.buffer[0] == client::GET_HIGH_SCORE)
    {
        return true;
    }
    else if (info.buffer[0] == client::JOIN_ROOM)
    {
        try
        {
            JsonRequestPacketDeserializer::deserializeJoinRoomRequest(info.buffer);
        }
        catch (...)
        {
            return false;
        }
    }
    else if (info.buffer[0] == client::GET_PLAYERS_IN_ROOM)
    {
        try
        {
            JsonRequestPacketDeserializer::deserializeGetPlayersRequest(info.buffer);
        }
        catch (...)
        {
            return false;
        }
    }
    else if (info.buffer[0] == client::CREATE_ROOM)
    {
        try
        {
            JsonRequestPacketDeserializer::deserializeCreateRoomRequest(info.buffer);
        }
        catch (...)
        {
            return false;
        }
    }
    else
    {
        return false;
    }
    return true;
}

RequestResult MenuRequestHandler::handleRequest(const RequestInfo& info)
{
    if (this->isRequestRelevant(info))
    {
        if (info.buffer[0] == client::LOGOUT)
        {
            return this->signout();
        }
        else if (info.buffer[0] == client::JOIN_ROOM)
        {
            return this->joinRoom(info);
        }
        else if (info.buffer[0] == client::GET_ROOMS)
        {
            return this->getRooms(info);
        }
        else if (info.buffer[0] == client::GET_PLAYERS_IN_ROOM)
        {
            return this->getPlayersInRoom(info);
        }
        else if (info.buffer[0] == client::GET_PERSONAL_STATS)
        {
            return this->getPersonalStats(info);
        }
        else if (info.buffer[0] == client::GET_HIGH_SCORE)
        {
            return this->getHighScore(info);
        }
        else if (info.buffer[0] == client::CREATE_ROOM)
        {
            return this->createRoom(info);
        }
    }
    else
    {
        RequestResult result;
        result.newHandler = nullptr;
        result.response = JsonResponsePacketSerializer::serializeResponse(ErrorResponse{ "You message is not formated correctly" });
        return result;
    }

    return RequestResult{};
}

RequestResult MenuRequestHandler::signout()
{
    RequestResult result;

    this->logout();

    result.newHandler = this->m_handlerFactory.createLoginRequestHandler(this->m_handlerFactory.getLoginManager());
    result.response = JsonResponsePacketSerializer::serializeResponse(LogoutResponse{ 1 });
    return result;
}

RequestResult MenuRequestHandler::getRooms(const RequestInfo& info)
{
    RequestResult result;

    result.newHandler = nullptr;
    result.response = JsonResponsePacketSerializer::serializeResponse(GetRoomsResponse{ 1, this->m_handlerFactory.getRoomManager().getRooms() });
    return result;
}

RequestResult MenuRequestHandler::getPlayersInRoom(const RequestInfo& info)
{
    int roomId = JsonRequestPacketDeserializer::deserializeGetPlayersRequest(info.buffer).roomId;

    RequestResult result;
    result.newHandler = nullptr;

    std::vector<std::string> usernames;
    std::vector<LoggedUser> userVector = this->m_handlerFactory.getRoomManager().getRoom(roomId).getAllUsers();

    for (int i = 0; i < userVector.size(); i++)
    {
        usernames.push_back(userVector[i].getUsername());
    }

    result.response = JsonResponsePacketSerializer::serializeResponse(GetPlayersInRoomResponse{ 1, usernames });

    return result;
}

RequestResult MenuRequestHandler::getPersonalStats(const RequestInfo& info)
{
    RequestResult result;

    result.newHandler = nullptr;

    getPersonalStatsResponse response;
    response.statistics = this->m_handlerFactory.getStatisticsManager().getUserStatistics(this->m_user.getUsername());
    response.status = 1;

    result.response = JsonResponsePacketSerializer::serializeResponse(response);

    return result;
}

RequestResult MenuRequestHandler::getHighScore(const RequestInfo& info)
{
    RequestResult result;
    result.newHandler = nullptr;

    getHighScoreResponse response;
    response.status = 1;
    response.statistics = this->m_handlerFactory.getStatisticsManager().getHighScore();

    result.response = JsonResponsePacketSerializer::serializeResponse(response);

    return result;
}

RequestResult MenuRequestHandler::joinRoom(const RequestInfo& info)
{
    RequestResult result;

    int roomId = JsonRequestPacketDeserializer::deserializeJoinRoomRequest(info.buffer).roomId;

    try
    {
        this->m_handlerFactory.getRoomManager().throwExceptionIfUserJoinedAnotherRoom(LoggedUser(this->m_user));
        this->m_handlerFactory.getRoomManager().getRoom(roomId).addUser(LoggedUser(this->m_user));
        result.newHandler = this->m_handlerFactory.createRoomMemberRequestHandler(this->m_user, this->m_handlerFactory.getRoomManager().getRoom(roomId));

        result.response = JsonResponsePacketSerializer::serializeResponse(JoinRoomResponse{ 1 });
        return result;
    }
    catch (const std::exception& error)
    {
        safe::println("Error: {}", error.what());
        result.newHandler = nullptr;
        result.response = JsonResponsePacketSerializer::serializeResponse(ErrorResponse{ error.what() });
        return result;
    }
}

RequestResult MenuRequestHandler::createRoom(const RequestInfo& info)
{
    RequestResult result;   
    try
    {
        RoomData roomData;
        CreateRoomRequest request = JsonRequestPacketDeserializer::deserializeCreateRoomRequest(info.buffer);
        roomData.maxPlayers = request.maxUsers;
        roomData.name = request.roomName;
        roomData.numOfQuestionsInGame = request.questionCount;
        roomData.timePerQuestion = request.answerTimeout;
        roomData.status = 1;
        roomData.id = this->m_handlerFactory.getRoomManager().getNextId();

        this->m_handlerFactory.getRoomManager().throwExceptionIfUserJoinedAnotherRoom(LoggedUser(this->m_user));
        this->m_handlerFactory.getRoomManager().createRoom(this->m_user, roomData);

        CreateRoomResponse response;
        response.status = 1;

        result.newHandler = this->m_handlerFactory.createRoomAdminRequestHandler(this->m_user, this->m_handlerFactory.getRoomManager().getRoom(roomData.id));
        result.response = JsonResponsePacketSerializer::serializeResponse(response);
        return result;
    }
    catch (const std::exception& error)
    {
        safe::println("Error: {}", error.what());
        result.newHandler = nullptr;
        result.response = JsonResponsePacketSerializer::serializeResponse(ErrorResponse{ error.what() });
        return result;
    }
}

void MenuRequestHandler::logout(void)
{
    this->m_handlerFactory.getLoginManager().logout(this->m_user.getUsername());
    this->m_handlerFactory.getRoomManager().removeUser(this->m_user.getUsername());
}