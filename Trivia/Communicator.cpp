#include "Communicator.h"
#include "JsonRequestPacketDeserializer.h"
#include "JsonResponsePacketSerializer.h"
#include "CodeConstants.h"
#include "RequestStructs.h"
#include "LoginRequestHandler.h"
#include "safe.h"

// Must be static (exe binary) and not on the stack.
// Because when the program ends it frees the stack corrupting this value while testing.
// The binary data does not get free until the entire process is fully closed so this value would not get corrupted.
std::atomic<bool> Communicator::m_do_run = true;

Communicator::Communicator(RequestHandlerFactory* rhf) : m_handlerFactory(*rhf)
{
	std::println("IP: {}", Communicator::SERVER_IP);
	std::print("Port: {}\n\n", Communicator::SERVER_PORT);
	std::println("Creating Server...");

	// this server use TCP. that why SOCK_STREAM & IPPROTO_TCP
	// if the server use UDP we will use: SOCK_DGRAM & IPPROTO_UDP
	this->m_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (this->m_serverSocket == INVALID_SOCKET)
		throw std::exception(__FUNCTION__ " - socket");

	this->bindAndListen();
}
Communicator::~Communicator()
{
	for (auto it = m_clients.begin(); it != m_clients.end(); ++it)
	{
		closesocket(it->first);
		delete it->second;
	}

	closesocket(this->m_serverSocket);
}

void Communicator::bindAndListen() const
{
	struct sockaddr_in sa = { 0 };

	sa.sin_port = htons(Communicator::SERVER_PORT); // port that server will listen for
	sa.sin_family = AF_INET;   // must be AF_INET
	sa.sin_addr.s_addr = INADDR_ANY;    // when there are few ip's for the machine. We will use always "INADDR_ANY"

	std::println("Binding...");
	// Connects between the socket and the configuration (port and etc..)
	if (bind(this->m_serverSocket, (struct sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
		throw std::exception(__FUNCTION__ " - bind");

	// Start listening for incoming requests of clients
	if (listen(this->m_serverSocket, SOMAXCONN) == SOCKET_ERROR)
		throw std::exception(__FUNCTION__ " - listen");
	std::print("Listening...\n\n");
}

void Communicator::startHandleRequests()
{
	while (this->m_do_run)
	{
		try
		{
			SOCKET socket = this->acceptClient();
			this->addUser(socket);
			std::thread newThread(&Communicator::handleNewClient, this, socket);
			newThread.detach();
			
			safe::println("\nClient Accepted");
		}
		catch (const std::exception& error)
		{
			if (this->m_do_run)
			{
				safe::println("Error occurred: {}", error.what());
			}
		}
	}
}

SOCKET Communicator::acceptClient(void) const
{
	SOCKET client_socket = accept(this->m_serverSocket, NULL, NULL);

	if (client_socket == INVALID_SOCKET)
		throw std::exception(__FUNCTION__);

	return client_socket;
}

void Communicator::handleNewClient(SOCKET clientSocket)
{
	try
	{
		while (this->m_do_run)
		{
			std::vector<unsigned char> buffer = Communicator::extract_full_message(clientSocket);
			
			RequestInfo request_info{ buffer };
			IRequestHandler* handler = this->getUserHandler(clientSocket);
			RequestResult request_result = handler->handleRequest(request_info);

			if (request_result.newHandler)
			{
				this->setNewRequestHandler(clientSocket, request_result.newHandler);
			}
			Communicator::sendMessage(clientSocket, request_result.response);
		}
	}
	catch (const std::exception& error)
	{
		if (this->m_do_run) // Could happen when ~Communicator() is called.
		{
			safe::println("{}", error.what());
			this->removeUser(clientSocket);
		}
	}
}

IRequestHandler* Communicator::getUserHandler(SOCKET userSocket)
{
	std::lock_guard<std::mutex> lg (this->lock);
	
	auto it = this->m_clients.find(userSocket);

	if (it == this->m_clients.end())
	{
		// Should never happen.
		throw std::exception("Cannot find user handler");
	}

	return it->second;
}
void Communicator::removeUser(SOCKET userSocket)
{
	std::lock_guard<std::mutex> lg (this->lock);

	auto it = this->m_clients.find(userSocket);

	if (it == this->m_clients.end())
	{
		return;
	}

	it->second->logout();
	closesocket(it->first);
	delete it->second;

	this->m_clients.erase(it);
}
void Communicator::addUser(SOCKET userSocket)
{
	std::lock_guard<std::mutex> lg(this->lock);

	LoginRequestHandler* new_login_handler = this->m_handlerFactory.createLoginRequestHandler(this->m_handlerFactory.getLoginManager());

	this->m_clients.insert({ userSocket, new_login_handler });
}
void Communicator::setNewRequestHandler(SOCKET clientSocket, IRequestHandler* request_handler)
{
	std::lock_guard<std::mutex> lg(this->lock);


	auto it = this->m_clients.find(clientSocket);

	if (it == this->m_clients.end())
	{
		return;
	}

	delete it->second;
	it->second = request_handler;
}

void Communicator::stop()
{
	this->m_do_run = false;
}

void Communicator::sendMessage(SOCKET userSocket, const std::vector<unsigned char>& message)
{
	if (send(userSocket, reinterpret_cast<const char*>(message.data()), static_cast<int>(message.size()), 0) == SOCKET_ERROR)
	{
		throw std::exception("Client disconnected");
	}
}
void Communicator::getMessage(SOCKET userSocket, unsigned char* buffer, int size)
{
	int results = recv(userSocket, reinterpret_cast<char*>(buffer), size, 0);

	if (results == SOCKET_ERROR || results == 0)
	{
		throw std::exception("Client disconnected");
	}
}
std::vector<unsigned char> Communicator::extract_full_message(SOCKET clientSocket)
{
	constexpr std::size_t MAX_SIZE = 1024 * 1024; // 1MB.
	constexpr std::size_t START_SIZE = sizeof(uint8_t) + sizeof(unsigned int);
	constexpr std::size_t CODE_INDEX = 0;
	constexpr std::size_t MESSAGE_LENGTH_INDEX = CODE_INDEX + sizeof(uint8_t);
	constexpr std::size_t MESSAGE_INDEX = MESSAGE_LENGTH_INDEX + sizeof(unsigned int);

	std::vector<unsigned char> buffer(START_SIZE);
	uint8_t code;
	unsigned int message_length;

	Communicator::getMessage(clientSocket, &buffer[0], START_SIZE);

	std::memcpy(&code, &buffer[CODE_INDEX], sizeof(code));
	std::memcpy(&message_length, &buffer[MESSAGE_LENGTH_INDEX], sizeof(message_length));

	if (message_length > MAX_SIZE)
	{
		throw std::exception("Message is too big");
	}

	if (message_length != 0)
	{
		buffer.resize(START_SIZE + message_length);

		Communicator::getMessage(clientSocket, &buffer[START_SIZE], message_length);
	}
	return buffer;
}