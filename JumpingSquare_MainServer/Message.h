#pragma once
#include <string>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <fstream>

namespace dedi
{
	/// <summary>
	/// 응답이 필요한 메시지 데이터 클래스
	/// </summary>
	class ResponsiveMessage
	{
	private:
		// 요청 메시지
		std::string request;
		// 승인 메시지
		std::string accept;

	public:
		ResponsiveMessage();
		ResponsiveMessage(std::string request, std::string accept);

		/// <summary>
		/// 요청 메시지 불러오기
		/// </summary>
		inline std::string GetRequest();
		/// <summary>
		/// 승인 메시지 불러오기
		/// </summary>
		inline std::string GetAccept();
	};

	/// <summary>
	/// 메시지 데이터 총괄 클래스
	/// </summary>
	class Message
	{
	private:
		// 서버 접속 관련 메시지
		ResponsiveMessage access;
		// 리스폰 관련 메시지
		ResponsiveMessage respawn;

		// 접속 종료 메시지
		std::string close;
		// 사망 처리 메시지
		std::string death;
		// 클리어 처리 메시지
		std::string clear;

	public:
		Message(rapidjson::Document& document);

		/// <summary>
		/// 메시지 데이터를 파일로부터 불러와서 반환한다.
		/// </summary>
		static Message* readDataFromFile();

		/// <summary>
		/// 메시지 종류
		/// </summary>
		enum MessageType { AccessRequest, AccessAccept, RespawnRequest, RespawnAccept, Close, Death, Clear };
		/// <summary>
		/// 메시지 종류에 따른 메시지를 불러온다.
		/// </summary>
		/// <param name="type">메시지 종류</param>
		/// <returns>메시지 내용</returns>
		std::string GetMessageContent(MessageType type);
	};
}