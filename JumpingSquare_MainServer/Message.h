#pragma once
#include <string>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <fstream>

class ResponsiveMessage
{
private:
	const char* request;
	const char* accept;

public:
	ResponsiveMessage(const char* request, const char* accept) : request(request), accept(accept) {}

	inline const char* GetRequest() { return request; }
	inline const char* GetAccept() { return accept; }
};

class Message
{
private:
	ResponsiveMessage access;
	ResponsiveMessage respawn;

	const char* close;
	const char* death;
	const char* clear;

public:
	Message(rapidjson::Document& document);

	static Message* ReadDataFromFile();

	enum MessageType { AccessRequest, AccessAccept, RespawnRequest, RespawnAccept, Close, Death, Clear };
	const char* GetMessageContent(MessageType type);
};