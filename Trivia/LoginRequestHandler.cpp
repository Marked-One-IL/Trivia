#include "LoginRequestHandler.h"
#include "JsonRequestPacketDeserializer.h"
#include "JsonResponsePacketSerializer.h"
#include "MenuRequestHandler.h"
#include "CodeConstants.h"
#include "json.hpp"
#include "safe.h"

LoginRequestHandler::LoginRequestHandler(LoginManager& login_manager, RequestHandlerFactory& handlerFactory) :
	m_loginManager(login_manager), m_handlerFactory(handlerFactory),
	m_currentLoggedInUsername("")
{
}

bool LoginRequestHandler::isRequestRelevant(const RequestInfo& info) const
{
	uint8_t code;
	std::memcpy(&code, &info.buffer[0], sizeof(code));

	if (code == client::LOGIN)
	{
		// This will may throw an exception if the json format is not correct.
		try
		{
			JsonRequestPacketDeserializer::deserializeLoginRequest(info.buffer);
		}
		catch (...)
		{
			return false;
		}

		return true;
	}
	else if (code == client::SIGNUP)
	{
		try
		{
			JsonRequestPacketDeserializer::deserializeSignupRequest(info.buffer);
		}
		catch (...)
		{
			return false;
		}

		return true;
	}

	return false;
}

RequestResult LoginRequestHandler::handleRequest(const RequestInfo& info)
{
	RequestResult request_result;

	uint8_t code;
	std::memcpy(&code, &info.buffer[0], sizeof(code));

	if (!this->isRequestRelevant(info))
	{
		return RequestResult{ JsonResponsePacketSerializer::serializeResponse(ErrorResponse{ "You message is not formated correctly" }), nullptr};
	}

	if (code == client::LOGIN)
	{
		LoginRequest login_request = JsonRequestPacketDeserializer::deserializeLoginRequest(info.buffer);

		safe::println("\nUsername: {}"
					  "\nPassword: {}",
					  login_request.username, login_request.password);

		request_result = this->login(login_request);
	}
	else if (code == client::SIGNUP)
	{
		SignupRequest signup_request = JsonRequestPacketDeserializer::deserializeSignupRequest(info.buffer);

		safe::println("\nEmail: {}"
					  "\nUsername: {}"
					  "\nPassword: {}",
					  signup_request.email, signup_request.username, signup_request.password);

		request_result = this->signup(signup_request);
	}
	else
	{
		// Should never happen.
		throw std::exception("Invalid code");
	}

	return request_result;
}
void LoginRequestHandler::logout(void)
{
	this->m_loginManager.logout(this->m_currentLoggedInUsername);
}

RequestResult LoginRequestHandler::login(const LoginRequest& login_request)
{
	RequestResult result;
	
	try
	{
		this->m_loginManager.login(login_request.username, login_request.password);

		safe::println("Login succeeded");
		result.response = JsonResponsePacketSerializer::serializeResponse(LoginResponse{ true });
		LoggedUser user(login_request.username);
		result.newHandler = this->m_handlerFactory.createMenuRequestHandler(user);

		this->logout();
		this->m_currentLoggedInUsername = login_request.username;
	}
	catch (const std::exception& error)
	{
		safe::println("{}", error.what());
		result.response = JsonResponsePacketSerializer::serializeResponse(ErrorResponse{ error.what() });

		LoggedUser user(this->m_currentLoggedInUsername);
		result.newHandler = nullptr;
	}

	return result;
}

RequestResult LoginRequestHandler::signup(const SignupRequest& signup_request)
{
	RequestResult result;

	try
	{
		this->m_loginManager.signup(signup_request.email, signup_request.username, signup_request.password);
		safe::println("Signup succeeded");
		result.response = JsonResponsePacketSerializer::serializeResponse(SignupResponse{ true });
	}
	catch (const std::exception& error)
	{
		safe::println("{}", error.what());
		result.response = JsonResponsePacketSerializer::serializeResponse(ErrorResponse{ error.what() });
	}

	result.newHandler = nullptr;

	return result;
}
