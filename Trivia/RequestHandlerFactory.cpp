#include "RequestHandlerFactory.h"

RequestHandlerFactory::RequestHandlerFactory(IDataBase* db) :m_database(db), m_loginManager(db), m_gameManager(db), m_StatisticsManager(db)
{
}

LoginManager& RequestHandlerFactory::getLoginManager()
{
    return this->m_loginManager;
}

LoginRequestHandler* RequestHandlerFactory::createLoginRequestHandler(LoginManager& login_manager)
{
    return new LoginRequestHandler(login_manager, *this);
}

MenuRequestHandler* RequestHandlerFactory::createMenuRequestHandler(LoggedUser& loggedUser)
{
    return new MenuRequestHandler(loggedUser, *this);
}

RoomManager& RequestHandlerFactory::getRoomManager()
{
    return this->m_roomManager;
}

StatisticsManager& RequestHandlerFactory::getStatisticsManager()
{
    return this->m_StatisticsManager;
}

RoomAdminRequestHandler* RequestHandlerFactory::createRoomAdminRequestHandler(LoggedUser& loggedUser, Room& room)
{
    return new RoomAdminRequestHandler(loggedUser, room, this->m_roomManager, *this);
}

RoomMemberRequestHandler* RequestHandlerFactory::createRoomMemberRequestHandler(LoggedUser& loggedUser, Room& room)
{
    return new RoomMemberRequestHandler(loggedUser, room, this->m_roomManager, *this);
}

GameRequestHandler* RequestHandlerFactory::CreateGameRequestHandler(LoggedUser& loggedUser, Game& game)
{
    return new GameRequestHandler(loggedUser, game, this->m_gameManager, *this);
}

GameManager& RequestHandlerFactory::getGameManager()
{
    return this->m_gameManager;
}