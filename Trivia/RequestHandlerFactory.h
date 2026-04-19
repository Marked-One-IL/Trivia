#pragma once
#include "IDataBase.h"
#include "LoginManager.h"
#include "LoginRequestHandler.h"
#include "MenuRequestHandler.h"
#include "LoggedUser.h"
#include "RoomManager.h"
#include "GameManager.h"
#include "StatisticsManager.h"
#include "RoomAdminRequestHandler.h"
#include "RoomMemberRequestHandler.h"
#include "GameRequestHandler.h"

class LoginRequestHandler;
class MenuRequestHandler;
class RoomAdminRequestHandler;
class RoomMemberRequestHandler;
class GameRequestHandler;
class GameManager;

class RequestHandlerFactory
{
public:
	RequestHandlerFactory(IDataBase* db);
	LoginManager& getLoginManager();
	LoginRequestHandler* createLoginRequestHandler(LoginManager& login_manager);
	MenuRequestHandler* createMenuRequestHandler(LoggedUser& loggedUser);
	RoomManager& getRoomManager();
	StatisticsManager& getStatisticsManager();
	RoomAdminRequestHandler* createRoomAdminRequestHandler(LoggedUser& loggedUser, Room& room);
	RoomMemberRequestHandler* createRoomMemberRequestHandler(LoggedUser& loggedUser, Room& room);
	GameRequestHandler* CreateGameRequestHandler(LoggedUser& loggedUser, Game& game);
	GameManager& getGameManager();

private:
	GameManager m_gameManager;
	RoomManager m_roomManager;
	StatisticsManager m_StatisticsManager;
	LoginManager m_loginManager;
	IDataBase* m_database;
};