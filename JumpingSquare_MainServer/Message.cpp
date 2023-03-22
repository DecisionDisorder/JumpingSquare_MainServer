#include "Message.h"
#include "Message.h"

Message::Message(rapidjson::Document& document) :
	access(ResponsiveMessage(document["access"]["request"].GetString(), document["access"]["accept"].GetString())),
	respawn(ResponsiveMessage(document["respawn"]["request"].GetString(), document["respawn"]["accept"].GetString())),
	close(document["close"].GetString()),
	death(document["death"].GetString()),
	clear(document["clear"].GetString())
{}

Message* Message::ReadDataFromFile()
{
	std::ifstream fIn("data/message.json");
	std::string str;
	Message* messageData = nullptr;

	if (fIn.is_open())
	{
		rapidjson::Document doc;
		fIn >> str;
		doc.Parse(const_cast<char*>(str.c_str()));

		messageData = new Message(doc);

		fIn.close();
	}

	return messageData;
}

const char* Message::GetMessageContent(MessageType type)
{
	switch (type)
	{
	case Message::AccessRequest:
		return access.GetRequest();
	case Message::AccessAccept:
		return access.GetAccept();
	case Message::RespawnRequest:
		return respawn.GetRequest();
	case Message::RespawnAccept:
		return respawn.GetAccept();
	case Message::Close:
		return close;
	case Message::Death:
		return death;
	case Message::Clear:
		return clear;
	default:
		break;
	}
	return "Null";
}
