#pragma once
#include "RequestStructs.h"

struct RequestInfo;
struct RequestResult;

class IRequestHandler
{
public:
	virtual ~IRequestHandler() = default;
	virtual bool isRequestRelevant(const RequestInfo& info) const = 0;
	virtual RequestResult handleRequest(const RequestInfo& info) = 0;
	virtual void logout(void) = 0;
};