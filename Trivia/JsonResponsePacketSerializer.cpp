#include "JsonResponsePacketSerializer.h"
#include "CodeConstants.h"

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(uint8_t code, const nlohmann::json& jsonMsg)
{
	std::vector<unsigned char> message;

	constexpr std::size_t CODE_INDEX = 0;
	constexpr std::size_t MESSAGE_LENGTH_INDEX = CODE_INDEX + sizeof(unsigned char);
	constexpr std::size_t MESSAGE_INDEX = MESSAGE_LENGTH_INDEX + sizeof(unsigned int);

	const std::string json_string_response = jsonMsg.dump();
	const std::size_t response_length = json_string_response.size();
	const std::size_t total_size = sizeof(unsigned char) + sizeof(unsigned int) + response_length;

	message.resize(total_size);

	std::memcpy(&message[CODE_INDEX], &code, sizeof(unsigned char));
	std::memcpy(&message[MESSAGE_LENGTH_INDEX], &response_length, sizeof(unsigned int));
	std::memcpy(&message[MESSAGE_INDEX], json_string_response.c_str(), sizeof(char) * response_length);

	return message;
}
std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(uint8_t code)
{
	std::vector<unsigned char> message;

	constexpr std::size_t CODE_INDEX = 0;
	constexpr std::size_t MESSAGE_LENGTH_INDEX = CODE_INDEX + sizeof(unsigned char);
	constexpr std::size_t MESSAGE_INDEX = MESSAGE_LENGTH_INDEX + sizeof(unsigned int);

	nlohmann::json messageJson;
	messageJson["status"] = 1;
	messageJson["message"] = "";

	const std::string json_string_response = messageJson.dump();
	const std::size_t response_length = json_string_response.size();
	const std::size_t total_size = sizeof(unsigned char) + sizeof(unsigned int) + response_length;

	message.resize(total_size);

	std::memcpy(&message[CODE_INDEX], &code, sizeof(unsigned char));
	std::memcpy(&message[MESSAGE_LENGTH_INDEX], &response_length, sizeof(unsigned int));
	std::memcpy(&message[MESSAGE_INDEX], json_string_response.c_str(), sizeof(char) * response_length);

	return message;
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const ErrorResponse& response)
{
	nlohmann::json j;
	j["status"] = 0;
	j["message"] = response.message;
	return JsonResponsePacketSerializer::serializeResponse(server::FAILURE, j);
}
std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(LoginResponse response)
{
	if (response.status)
	{
		return JsonResponsePacketSerializer::serializeResponse(server::LOGIN);
	}

	return JsonResponsePacketSerializer::serializeResponse(ErrorResponse{ "Error" });
}
std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(SignupResponse response)
{
	if (response.status)
	{
		return JsonResponsePacketSerializer::serializeResponse(server::SIGNUP);
	}

	return JsonResponsePacketSerializer::serializeResponse(ErrorResponse{ "Error" });
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const CreateRoomResponse& response)
{
	if (response.status)
	{
		return JsonResponsePacketSerializer::serializeResponse(server::CREATE_ROOM);
	}

	return JsonResponsePacketSerializer::serializeResponse(ErrorResponse{ "Error" });
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const GetRoomsResponse& response)
{
	if (response.status)
	{
		nlohmann::json j;
		j["status"] = 1;
		j["message"] = "";

		std::vector<unsigned int> IDs;
		std::vector<std::string> names;
		std::vector<unsigned int> maxPlayers;
		std::vector<unsigned int> numOfQuestionsInGame;
		std::vector<unsigned int> timePerQuestion;
		std::vector<bool> statuses;

		for (unsigned int i = 0; i < response.rooms.size(); ++i)
		{
			IDs.push_back(response.rooms[i].id);
			names.push_back(response.rooms[i].name);
			maxPlayers.push_back(response.rooms[i].maxPlayers);
			numOfQuestionsInGame.push_back(response.rooms[i].numOfQuestionsInGame);
			timePerQuestion.push_back(response.rooms[i].timePerQuestion);
			statuses.push_back(response.rooms[i].status);
		}

		j["ids"] = IDs;
		j["names"] = names;
		j["maxPlayers"] = maxPlayers;
		j["numOfQuestionsInGame"] = numOfQuestionsInGame;
		j["timePerQuestion"] = timePerQuestion;
		j["statuses"] = statuses;
		
		return JsonResponsePacketSerializer::serializeResponse(server::GET_ROOMS, j);
	}

	return JsonResponsePacketSerializer::serializeResponse(ErrorResponse{ "Error" });
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const LogoutResponse& response)
{
	if (response.status)
	{
		return JsonResponsePacketSerializer::serializeResponse(server::LOGOUT);
	}

	return JsonResponsePacketSerializer::serializeResponse(ErrorResponse{ "Error" });
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const GetPlayersInRoomResponse& response)
{
	if (response.status)
	{
		nlohmann::json j;
		j["status"] = 1;
		j["message"] = response.players;
		return JsonResponsePacketSerializer::serializeResponse(server::GET_PLAYERS_IN_ROOM, j);
	}

	return JsonResponsePacketSerializer::serializeResponse(ErrorResponse{ "Error" });
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const JoinRoomResponse& response)
{
	if (response.status)
	{
		return JsonResponsePacketSerializer::serializeResponse(server::JOIN_ROOM);
	}

	return JsonResponsePacketSerializer::serializeResponse(ErrorResponse{ "Error" });
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const getHighScoreResponse& response)
{
	if (response.status)
	{
		nlohmann::json j;
		j["status"] = 1;
		j["message"] = "";

		for (unsigned int i = 0; i < response.statistics.size(); ++i)
		{
			j["names"].push_back(response.statistics[i].name);
			j["scores"].push_back(response.statistics[i].score);
		}

		return JsonResponsePacketSerializer::serializeResponse(server::GET_HIGH_SCORE, j);
	}

	return JsonResponsePacketSerializer::serializeResponse(ErrorResponse{ "Error" });
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const getPersonalStatsResponse& response)
{
	if (response.status)
	{
		nlohmann::json j;
		j["status"] = 1;
		j["message"] = "";

		j["gameCount"] = response.statistics[0];
		j["correctAnswers"] = response.statistics[1];
		j["totalAnswers"] = response.statistics[2];
		j["avgTimePerAnswer"] = response.statistics[3];
		j["playerScore"] = response.statistics[4];
		return JsonResponsePacketSerializer::serializeResponse(server::GET_PERSONAL_STATS, j);
	}

	return JsonResponsePacketSerializer::serializeResponse(ErrorResponse{ "Error" });
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(CloseRoomReponse response)
{
	if (response.status)
	{
		return JsonResponsePacketSerializer::serializeResponse(server::CLOSE_ROOM);
	}

	return JsonResponsePacketSerializer::serializeResponse(ErrorResponse{ "Error" });
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(StartGameReponse response)
{
	if (response.status)
	{
		return JsonResponsePacketSerializer::serializeResponse(server::START_GAME);
	}

	return JsonResponsePacketSerializer::serializeResponse(ErrorResponse{ "Error" });
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const GetRoomStateResponse& response)
{
	if (response.status)
	{
		nlohmann::json j;
		j["status"] = 1;
		j["message"] = "";

		j["roomStatus"] = response.room_status;
		j["hasGameBegun"] = response.hasGameBegun;
		j["players"] = response.players;
		j["questionCount"] = response.questionCount;
		j["answerTimeout"] = response.answerTimeout;
		j["maxPlayers"] = response.maxPlayers;
		return JsonResponsePacketSerializer::serializeResponse(server::GET_ROOM_STATE, j);
	}

	return JsonResponsePacketSerializer::serializeResponse(ErrorResponse{ "Error" });
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(LeaveRoomResponse response)
{
	if (response.status)
	{
		nlohmann::json j;
		j["status"] = 1;
		j["message"] = "";
		return JsonResponsePacketSerializer::serializeResponse(server::LEAVE_ROOM, j);
	}

	return JsonResponsePacketSerializer::serializeResponse(ErrorResponse{ "Error" });
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(LeaveGameResponse response)
{
	if (response.status)
	{
		nlohmann::json j;
		j["status"] = 1;
		return JsonResponsePacketSerializer::serializeResponse(server::LEAVE_GAME, j);
	}

	return JsonResponsePacketSerializer::serializeResponse(ErrorResponse{ "Error" });
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const SubmitAnswerResponse& response)
{
	if (response.status)
	{
		nlohmann::json j;
		j["status"] = 1;
		j["correctIndex"] = response.correctAnswerId;
		return JsonResponsePacketSerializer::serializeResponse(server::SUBMIT_ANSWER, j);
	}

	return JsonResponsePacketSerializer::serializeResponse(ErrorResponse{ "Error" });
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const GetQuestionResponse& response)
{
	if (response.status)
	{
		nlohmann::json j;
		j["status"] = 1;
		j["answers"] = response.answers;
		j["question"] = response.question;

		return JsonResponsePacketSerializer::serializeResponse(server::GET_QUESTION, j);
	}

	return JsonResponsePacketSerializer::serializeResponse(ErrorResponse{ "Error" });
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const GetGameResultsResponse& response)
{
	if (response.status)
	{
		nlohmann::json j;
		j["status"] = 1;

		std::vector<std::string> names;
		std::vector<unsigned int> correctAnswers;
		std::vector<unsigned int> wrongAnswers;
		std::vector<float> avgTime;
		for (unsigned int i = 0; i < response.results.size(); ++i)
		{
			correctAnswers.push_back(response.results[i].correctAnswerCount);
			names.push_back(response.results[i].username);
			wrongAnswers.push_back(response.results[i].wrongAnswerCount);
			avgTime.push_back(response.results[i].avrageAnswerTime);
		}

		j["usernames"] = names;
		j["correctAnswers"] = correctAnswers;
		j["wrongAnswers"] = wrongAnswers;
		j["avgTimePerQuestion"] = avgTime;

		return JsonResponsePacketSerializer::serializeResponse(server::GET_GAME_RESULTS, j);
	}

	return JsonResponsePacketSerializer::serializeResponse(ErrorResponse{ "Error" });
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const AreAllPlayersDoneResponse& response)
{
	if (response.status)
	{
		nlohmann::json j;
		j["status"] = 1;
		j["areAllPlayersDone"] = static_cast<int>(response.areTheyDone);

		return JsonResponsePacketSerializer::serializeResponse(server::ARE_ALL_PLAYERS_DONE, j);
	}

	return JsonResponsePacketSerializer::serializeResponse(ErrorResponse{ "Error" });
}
