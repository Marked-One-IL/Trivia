#pragma once
#include "LoggedUser.h"
#include <vector>
#include <iostream>
#include "CodeConstants.h"

struct RoomData
{
	unsigned int id;
	std::string name;
	unsigned int maxPlayers;
	unsigned int numOfQuestionsInGame;
	unsigned int timePerQuestion;
	unsigned int status;
};

struct UserScore
{
	std::string name;
	unsigned int score;
};

class Room
{
public:
	Room();
	Room(const LoggedUser&, const RoomData&);
	void addUser(const LoggedUser&);
	void removeUser(const LoggedUser&);
	std::vector<LoggedUser> getAllUsers() const;
	RoomData getRoomData() const;
	void setRoomStatus(unsigned int newRoomStatus);

private:
	RoomData m_metadata;
	std::vector<LoggedUser> m_users;
};