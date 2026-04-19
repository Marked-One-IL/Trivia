#pragma once
#include <vector>
#include "json.hpp"
#include "ResponseStructs.h"

class JsonResponsePacketSerializer
{
public:
	static std::vector<unsigned char> serializeResponse(const ErrorResponse& response);
	static std::vector<unsigned char> serializeResponse(LoginResponse response);
	static std::vector<unsigned char> serializeResponse(SignupResponse response);
	static std::vector<unsigned char> serializeResponse(const CreateRoomResponse& response);
	static std::vector<unsigned char> serializeResponse(const GetRoomsResponse& response);
	static std::vector<unsigned char> serializeResponse(const LogoutResponse& response);
	static std::vector<unsigned char> serializeResponse(const GetPlayersInRoomResponse& response);
	static std::vector<unsigned char> serializeResponse(const JoinRoomResponse& response);
	static std::vector<unsigned char> serializeResponse(const getHighScoreResponse& response);
	static std::vector<unsigned char> serializeResponse(const getPersonalStatsResponse& response);
	static std::vector<unsigned char> serializeResponse(CloseRoomReponse response);
	static std::vector<unsigned char> serializeResponse(StartGameReponse response);
	static std::vector<unsigned char> serializeResponse(const GetRoomStateResponse& response);
	static std::vector<unsigned char> serializeResponse(LeaveRoomResponse response);
	static std::vector<unsigned char> serializeResponse(LeaveGameResponse response);
	static std::vector<unsigned char> serializeResponse(const SubmitAnswerResponse& response);
	static std::vector<unsigned char> serializeResponse(const GetQuestionResponse& response);
	static std::vector<unsigned char> serializeResponse(const GetGameResultsResponse& response);
	static std::vector<unsigned char> serializeResponse(const AreAllPlayersDoneResponse& response);

private:
	static std::vector<unsigned char> serializeResponse(uint8_t code, const nlohmann::json& j);
	static std::vector<unsigned char> serializeResponse(uint8_t code);
};