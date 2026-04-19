#include "RoomManager.h"

RoomManager::RoomManager() : m_pos(0)
{
}

void RoomManager::createRoom(const LoggedUser& user, const RoomData& roomData)
{
    std::lock_guard<std::mutex> lock(this->m_lock);

    for (auto it = this->m_rooms.begin(); it != this->m_rooms.end(); ++it)
    {
        if (it->first == roomData.id || it->second.getRoomData().name == roomData.name)
        {
            throw std::exception("Room already exists");
        }
    }

    this->m_rooms.emplace(roomData.id, Room(user, roomData));
    this->m_pos++;
}

void RoomManager::deleteRoom(unsigned int id)
{
    std::lock_guard<std::mutex> lock(this->m_lock);

    auto it = this->m_rooms.find(id);

    if (it == this->m_rooms.end())
    {
        throw std::exception("Room not found");
    }

    it->second.setRoomStatus(room_status::CLOSED);
}

void RoomManager::startGame(unsigned int id)
{
    std::lock_guard<std::mutex> lock(this->m_lock);

    auto it = this->m_rooms.find(id);

    if (it == this->m_rooms.end())
    {
        throw std::exception("Room not found");
    }

    it->second.setRoomStatus(room_status::IN_GAME);
}

std::vector<RoomData> RoomManager::getRooms()
{
    std::lock_guard<std::mutex> lock(this->m_lock);

    std::vector<RoomData> result;

    for (auto it = this->m_rooms.begin(); it != this->m_rooms.end(); ++it)
    {
        result.push_back(it->second.getRoomData());
    }

    return result;
}

unsigned int RoomManager::getRoomStatus(unsigned int id)
{
    std::lock_guard<std::mutex> lock(this->m_lock);

    return this->m_rooms[id].getRoomData().status;
}

Room& RoomManager::getRoom(unsigned int id)
{
    std::lock_guard<std::mutex> lock(this->m_lock);

    if (this->m_rooms.find(id) == this->m_rooms.end())
    {
        throw std::exception("Room not found");
    }
    return this->m_rooms[id];
}

unsigned int RoomManager::getNextId()
{
    std::lock_guard<std::mutex> lock(this->m_lock);
    return this->m_pos + 1;
}

void RoomManager::removeUser(const std::string& username)
{
    std::lock_guard<std::mutex> lock(this->m_lock);

    for (auto it = this->m_rooms.begin(); it != this->m_rooms.end(); ++it)
    {
        std::vector<LoggedUser> users = it->second.getAllUsers();

        auto user = std::find(users.begin(), users.end(), LoggedUser(username));

        if (user != users.end())
        {
            it->second.removeUser(LoggedUser(username));
            
            if (users.size() - 1 == 0) // If there is no more users left.
            {
                this->m_rooms.erase(it);
            }

            return;
        }
    }
}

void RoomManager::throwExceptionIfUserJoinedAnotherRoom(const LoggedUser& username)
{
    std::lock_guard<std::mutex> lock(this->m_lock);

    for (auto it = this->m_rooms.begin(); it != this->m_rooms.end(); ++it)
    {
        std::vector<LoggedUser> users = it->second.getAllUsers();

        if (std::find(users.begin(), users.end(), username) != users.end())
        {
            throw std::exception("You already joined a room");
        }
    }
}
