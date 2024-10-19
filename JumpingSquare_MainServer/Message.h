#pragma once
#include <string>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <fstream>

namespace dedi
{
	/// <summary>
	/// ������ �ʿ��� �޽��� ������ Ŭ����
	/// </summary>
	class ResponsiveMessage
	{
	private:
		// ��û �޽���
		std::string request;
		// ���� �޽���
		std::string accept;

	public:
		ResponsiveMessage();
		ResponsiveMessage(std::string request, std::string accept);

		/// <summary>
		/// ��û �޽��� �ҷ�����
		/// </summary>
		inline std::string GetRequest();
		/// <summary>
		/// ���� �޽��� �ҷ�����
		/// </summary>
		inline std::string GetAccept();
	};

	/// <summary>
	/// �޽��� ������ �Ѱ� Ŭ����
	/// </summary>
	class Message
	{
	private:
		// ���� ���� ���� �޽���
		ResponsiveMessage access;
		// ������ ���� �޽���
		ResponsiveMessage respawn;

		// ���� ���� �޽���
		std::string close;
		// ��� ó�� �޽���
		std::string death;
		// Ŭ���� ó�� �޽���
		std::string clear;

	public:
		Message(rapidjson::Document& document);

		/// <summary>
		/// �޽��� �����͸� ���Ϸκ��� �ҷ��ͼ� ��ȯ�Ѵ�.
		/// </summary>
		static Message* readDataFromFile();

		/// <summary>
		/// �޽��� ����
		/// </summary>
		enum MessageType { AccessRequest, AccessAccept, RespawnRequest, RespawnAccept, Close, Death, Clear };
		/// <summary>
		/// �޽��� ������ ���� �޽����� �ҷ��´�.
		/// </summary>
		/// <param name="type">�޽��� ����</param>
		/// <returns>�޽��� ����</returns>
		std::string GetMessageContent(MessageType type);
	};
}