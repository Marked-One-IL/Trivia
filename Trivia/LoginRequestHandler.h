#pragma once
#include "IRequestHandler.h"
#include "LoginManager.h"
#include "RequestHandlerFactory.h"

class RequestHandlerFactory;

class LoginRequestHandler : public IRequestHandler
{
public:
	LoginRequestHandler(LoginManager& login_manager, RequestHandlerFactory& handlerFactory);
	virtual bool isRequestRelevant(const RequestInfo& info) const override;
	virtual RequestResult handleRequest(const RequestInfo& info) override;
	virtual void logout(void) override;

private:
	RequestResult login(const LoginRequest& login_request);
	RequestResult signup(const SignupRequest& signup_request);

	LoginManager& m_loginManager;
	RequestHandlerFactory& m_handlerFactory;
	std::string m_currentLoggedInUsername;
};