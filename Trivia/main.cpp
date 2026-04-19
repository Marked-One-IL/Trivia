#include "Server.h"
#include "safe.h"
#include "RoomManager.h"

int main()
{
	std::string str = "";

	try
	{
		Server server;
		server.run();

		while (str != "EXIT")
		{
			std::println("Press 'EXIT' to exit:");
			std::getline(std::cin, str);
		}

		server.stop();
	}
	catch (const std::exception& error)
	{
		std::println("Error: {}", error.what());
	}

	return 0;
}
