#include "Message.h"
#include "Message.h"

Message::Message(rapidjson::Document& document)
{
	// 메시지에 대한 Json Value에서 값을 불러와서 초기화
	access = ResponsiveMessage(document["access"]["request"].GetString(), document["access"]["accept"].GetString());
	respawn = ResponsiveMessage(document["respawn"]["request"].GetString(), document["respawn"]["accept"].GetString());
	close = document["close"].GetString();
	death = document["death"].GetString();
	clear = document["clear"].GetString();
}

Message* Message::ReadDataFromFile()
{
	// 메시지 데이터 파일을 연다
	std::ifstream fIn("data/message.json");
	std::string str;
	Message* loadedMessageData = nullptr;

	// 파일을 불러와서 문자열을 읽어오고, 데이터를 초기화하여 반환
	if (fIn.is_open())
	{
		rapidjson::Document doc;
		fIn >> str;
		doc.Parse(const_cast<char*>(str.c_str()));

		loadedMessageData = new Message(doc);

		fIn.close();
	}

	return loadedMessageData;
}

std::string Message::GetMessageContent(MessageType type)
{
	// 메시지 종류에 맞는 케이스에 따라 메시지 반환
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

ResponsiveMessage::ResponsiveMessage() {}
ResponsiveMessage::ResponsiveMessage(std::string request, std::string accept) : request(request), accept(accept) {}

inline std::string ResponsiveMessage::GetRequest()
{
	return request;
}

inline std::string ResponsiveMessage::GetAccept()
{
	return accept;
}
