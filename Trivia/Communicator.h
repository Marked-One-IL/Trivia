#pragma once
#include <Windows.h>
#include <map>
#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include "LoginRequestHandler.h"
#include "WSAInitializer.h"
#include "RequestHandlerFactory.h"
#include "IDataBase.h"
#include "json.hpp"

class Communicator
{
public:
	Communicator(RequestHandlerFactory* rhf);
	~Communicator();
	void startHandleRequests();
	void stop();

private:
	static constexpr const char* SERVER_IP = "127.0.0.1";
	static constexpr unsigned short SERVER_PORT = 8826;

	void handleNewClient(SOCKET clientSocket);
	void bindAndListen() const;
	SOCKET acceptClient(void) const;
	IRequestHandler* getUserHandler(SOCKET userSocket);
	void removeUser(SOCKET userSocket);
	void addUser(SOCKET userSocket);
	void setNewRequestHandler(SOCKET clientSocket, IRequestHandler* request_handler);
	
	static void getMessage(SOCKET userSocket, unsigned char* buffer, int size);
	static void sendMessage(SOCKET userSocket, const std::vector<unsigned char>& message);
	static std::vector<unsigned char> extract_full_message(SOCKET clientSocket);

	SOCKET m_serverSocket;
	std::map<SOCKET, IRequestHandler*> m_clients;
	WSAInitializer m_wsaInit;
	std::mutex lock;
	static std::atomic<bool> m_do_run;
	RequestHandlerFactory& m_handlerFactory;
};