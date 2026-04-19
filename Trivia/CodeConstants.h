#pragma once
#include <cstdint>

namespace client
{
	constexpr uint8_t SIGNUP = 1;
	constexpr uint8_t LOGIN = 2;
	constexpr uint8_t LOGOUT = 6;
	constexpr uint8_t GET_PLAYERS_IN_ROOM = 7;
	constexpr uint8_t JOIN_ROOM = 8;
	constexpr uint8_t GET_ROOMS = 9;
	constexpr uint8_t CREATE_ROOM = 10;
	constexpr uint8_t GET_HIGH_SCORE = 11;
	constexpr uint8_t GET_PERSONAL_STATS = 12;
	constexpr uint8_t CLOSE_ROOM = 13;
	constexpr uint8_t START_GAME = 14;
	constexpr uint8_t GET_ROOM_STATE = 15;
	constexpr uint8_t LEAVE_ROOM = 16;
	constexpr uint8_t LEAVE_GAME = 17;
	constexpr uint8_t GET_QUESTION = 18;
	constexpr uint8_t SUBMIT_ANSWER = 19;
	constexpr uint8_t GET_GAME_RESULTS = 20;
	constexpr uint8_t ARE_ALL_PLAYERS_DONE = 21;
}

namespace server
{
	constexpr uint8_t SIGNUP = 3;
	constexpr uint8_t LOGIN = 4;
	constexpr uint8_t FAILURE = 5;
	constexpr uint8_t LOGOUT = 6;
	constexpr uint8_t GET_PLAYERS_IN_ROOM = 7;
	constexpr uint8_t JOIN_ROOM = 8;
	constexpr uint8_t GET_ROOMS = 9;
	constexpr uint8_t CREATE_ROOM = 10;
	constexpr uint8_t GET_HIGH_SCORE = 11;
	constexpr uint8_t GET_PERSONAL_STATS = 12;
	constexpr uint8_t CLOSE_ROOM = 13;
	constexpr uint8_t START_GAME = 14;
	constexpr uint8_t GET_ROOM_STATE = 15;
	constexpr uint8_t LEAVE_ROOM = 16;
	constexpr uint8_t LEAVE_GAME = 17;
	constexpr uint8_t GET_QUESTION = 18;
	constexpr uint8_t SUBMIT_ANSWER = 19;
	constexpr uint8_t GET_GAME_RESULTS = 20;
	constexpr uint8_t ARE_ALL_PLAYERS_DONE = 21;
}

namespace room_status
{
	constexpr uint8_t IN_GAME = 1;
	constexpr uint8_t OPEN = 2;
	constexpr uint8_t CLOSED = 3;
}