#pragma once

namespace dedi
{
	/// <summary>
	/// 복사를 금지하고자 하는 클래스에 붙일 수 있는 클래스
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