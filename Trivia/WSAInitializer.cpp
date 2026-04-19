#pragma comment (lib, "ws2_32.lib")

#include "WSAInitializer.h"

#include <WinSock2.h>
#include <Windows.h>
#include <exception>

WSAInitializer::WSAInitializer(void)
{
	WSADATA wsa_data = { };
	if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
		throw std::exception("WSAStartup Failed");
}
WSAInitializer::~WSAInitializer(void)
{
	try
	{
		WSACleanup();
	}
	catch (...) {}
}