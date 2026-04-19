#include "JsonRequestPacketDeserializer.h"
#include <iostream>
#include <string>
#include <cstdint>
#include "json.hpp"

LoginRequest JsonRequestPacketDeserializer::deserializeLoginRequest(const std::vector<unsigned char>& buffer)
{
    LoginRequest result;

    std::string jsonSTR(buffer.begin() + sizeof(uint8_t) + sizeof(unsigned int), buffer.end());
    nlohmann::json j = nlohmann::json::parse(jsonSTR);
    result.password = j["password"];
    result.username = j["username"];

    return result;
}

SignupRequest JsonRequestPacketDeserializer::deserializeSignupRequest(const std::vector<unsigned char>& buffer)
{
    SignupRequest result;

    std::string jsonSTR(buffer.begin() + sizeof(uint8_t) + sizeof(unsigned int), buffer.end());

    nlohmann::json j = nlohmann::json::parse(jsonSTR);

    result.password = j["password"];
    result.username = j["username"];
    result.email = j["email"];

    return result;
}

GetPlayersInRoomRequest JsonRequestPacketDeserializer::deserializeGetPlayersRequest(const std::vector<unsigned char>& buffer)
{
    GetPlayersInRoomRequest result;

    std::string jsonSTR(buffer.begin() + sizeof(uint8_t) + sizeof(unsigned int), buffer.end());
    nlohmann::json j = nlohmann::json::parse(jsonSTR);

    result.roomId = j["roomId"];

    return result;
}
JoinRoomRequest JsonRequestPacketDeserializer::deserializeJoinRoomRequest(const std::vector<unsigned char>& buffer)
{
    JoinRoomRequest result;

    std::string jsonSTR(buffer.begin() + sizeof(uint8_t) + sizeof(unsigned int), buffer.end());
    nlohmann::json j = nlohmann::json::parse(jsonSTR);

    result.roomId = j["roomId"];

    return result;
}
CreateRoomRequest JsonRequestPacketDeserializer::deserializeCreateRoomRequest(const std::vector<unsigned char>& buffer)
{
    CreateRoomRequest result;

    std::string jsonSTR(buffer.begin() + sizeof(uint8_t) + sizeof(unsigned int), buffer.end());
    nlohmann::json j = nlohmann::json::parse(jsonSTR);

    result.answerTimeout = (unsigned int)j["answerTimeout"];
    result.maxUsers = (unsigned int)j["maxUsers"];
    result.questionCount = (unsigned int)j["questionCount"];
    result.roomName = j["roomName"];

    return result;
}

SubmitAnswerRequest JsonRequestPacketDeserializer::deserializeSubmitAnswerRequest(const std::vector<unsigned char>& buffer)
{
    SubmitAnswerRequest result;

    std::string jsonSTR(buffer.begin() + sizeof(uint8_t) + sizeof(unsigned int), buffer.end());
    nlohmann::json j = nlohmann::json::parse(jsonSTR);

    result.answerId = j["answerId"];

    return result;
}
