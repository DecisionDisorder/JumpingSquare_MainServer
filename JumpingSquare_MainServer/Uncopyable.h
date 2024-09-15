#pragma once

namespace dedi
{
	/// <summary>
	/// ���縦 �����ϰ��� �ϴ� Ŭ������ ���� �� �ִ� Ŭ����
	/// </summary>
	class Uncopyable
	{
	protected:
		Uncopyable();
		~Uncopyable();
	private:
		Uncopyable(const Uncopyable&);
		Uncopyable& operator=(const Uncopyable&);
	};
}