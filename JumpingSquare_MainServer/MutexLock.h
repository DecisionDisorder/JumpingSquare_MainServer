#pragma once
#include <mutex>
#include "Uncopyable.h"

namespace dedi
{
	class Uncopyable;
}

namespace dedi
{
	/// <summary>
	/// Mutex Lock/Unlock을 블록 단위로 자동으로 해주는 클래스
	/// </summary>
	class MutexLockHelper : private Uncopyable
	{
	public:
		explicit MutexLockHelper(std::mutex* pm);

		~MutexLockHelper();

	private:
		// 잠금 해제 대상 Mutex 포인터
		std::mutex* mutexPtr;
	};
}