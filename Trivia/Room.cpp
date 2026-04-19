#include "Room.h"

Room::Room()
{
}

Room::Room(const LoggedUser& user, const RoomData& roomData)
{
	this->m_users.push_back(user);
	this->m_metadata = roomData;
	this->m_metadata.status = room_status::OPEN;
}

void Room::addUser(const LoggedUser& user)
{
	if (this->m_metadata.status == room_status::CLOSED)
	{
		throw std::exception("Room is closing");
	}
	else if (this->m_metadata.status == room_status::IN_GAME)
	{
		throw std::exception("Room is in game");
	}

	if (this->m_users.size() == this->m_metadata.maxPlayers)
	{
		throw std::exception("Room's limit user is reached");
	}

	for (unsigned int i = 0; i < this->m_users.size(); i++)
	{
		if (user.getUsername() == this->m_users[i].getUsername())
		{
			throw std::exception("You already joined a room");
		}
	}
	this->m_users.push_back(user);
}

void Room::removeUser(const LoggedUser& user)
{
	for (auto it = this->m_users.begin(); it != this->m_users.end(); ++it)
	{
		if (user.getUsername() == it->getUsername())
		{
			this->m_users.erase(it);
			return;
		}
	}
	throw std::exception("You are not in that room");
}

std::vector<LoggedUser> Room::getAllUsers() const
{
	return this->m_users;
}

RoomData Room::getRoomData() const
{
	return this->m_metadata;
}

void Room::setRoomStatus(unsigned int newRoomStatus)
{
	if (this->m_metadata.status == newRoomStatus)
	{
		if (this->m_metadata.status == room_status::IN_GAME)
		{
			throw std::exception("You already started the game");
		}
		throw std::exception("You set the same status again");
	}

	this->m_metadata.status = newRoomStatus;
}