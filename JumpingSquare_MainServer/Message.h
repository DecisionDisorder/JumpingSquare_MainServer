#pragma once
#include <string>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <fstream>

class ResponsiveMessage
{
private:
	std::string request;
	std::string accept;

public:
	ResponsiveMessage();
	ResponsiveMessage(std::string request, std::string accept);

	inline std::string GetRequest();
	inline std::string GetAccept();
};

class Message
{
private:
	ResponsiveMessage access;
	ResponsiveMessage respawn;

	std::string close;
	std::string death;
	std::string clear;

public:
	Message(rapidjson::Document& document);

	static Message* ReadDataFromFile();

	enum MessageType { AccessRequest, AccessAccept, RespawnRequest, RespawnAccept, Close, Death, Clear };
	std::string GetMessageContent(MessageType type);
};