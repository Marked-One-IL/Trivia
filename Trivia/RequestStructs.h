#pragma once
#include "IRequestHandler.h"
#include <vector>
#include <string>

class IRequestHandler;

struct RequestResult
{
	std::vector<unsigned char> response;
	IRequestHandler* newHandler;
};

struct RequestInfo
{
	std::vector<unsigned char> buffer;
};

struct LoginRequest
{
	std::string username;
	std::string password;
};

struct SignupRequest
{
	std::string username;
	std::string password;
	std::string email;
};

struct GetPlayersInRoomRequest
{
	unsigned int roomId;
};

struct JoinRoomRequest
{
	unsigned int roomId;
};

struct CreateRoomRequest
{
	std::string roomName;
	unsigned int maxUsers;
	unsigned int questionCount;
	unsigned int answerTimeout;
};

struct SubmitAnswerRequest
{
	unsigned int answerId;
};