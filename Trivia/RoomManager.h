#pragma once
#include "Room.h"
#include <map>
#include <mutex>

class RoomManager
{
public:
    RoomManager();
    void createRoom(const LoggedUser&, const RoomData&);
    void deleteRoom(unsigned int id);
    void startGame(unsigned int id);
    std::vector<RoomData> getRooms();
    unsigned int getRoomStatus(unsigned int id);
    Room& getRoom(unsigned int id);

    unsigned int getNextId(void);
    void removeUser(const std::string& username);
    void throwExceptionIfUserJoinedAnotherRoom(const LoggedUser& username);

private:
    std::map<unsigned int, Room> m_rooms;
    unsigned int m_pos;
    std::mutex m_lock;
};