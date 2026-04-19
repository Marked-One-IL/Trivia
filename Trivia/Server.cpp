#include "Server.h"
#include "Communicator.h"
#include <exception>

Server::Server()
{
	// No use of : x(), y() because the compiler fucks the order.
	this->m_database = new SqliteDataBase();
	this->m_handlerFactory = new RequestHandlerFactory(this->m_database);
	this->m_communicator = new Communicator(this->m_handlerFactory);
}
Server::~Server()
{
	delete this->m_database;
	delete this->m_handlerFactory;
	delete this->m_communicator;
}

void Server::run()
{
	std::thread server_thread(&Communicator::startHandleRequests, m_communicator);
	server_thread.detach();
}

void Server::stop()
{
	this->m_communicator->stop();
}