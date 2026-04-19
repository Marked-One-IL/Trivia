#pragma once
#include "IDataBase.h"
#include "Communicator.h"
#include "RequestHandlerFactory.h"
#include "WSAInitializer.h"
#include "SqliteDataBase.h"

class Server
{
public:
	Server();
	~Server();
	void run();
	void stop();

private:
	SqliteDataBase* m_database;
	Communicator* m_communicator;
	RequestHandlerFactory* m_handlerFactory;
};