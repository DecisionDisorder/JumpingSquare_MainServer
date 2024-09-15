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
	/// Mutex Lock/Unlock�� ��� ������ �ڵ����� ���ִ� Ŭ����
	/// </summary>
	class MutexLockHelper : private Uncopyable
	{
	public:
		explicit MutexLockHelper(std::mutex* pm);

		~MutexLockHelper();

	private:
		// ��� ���� ��� Mutex ������
		std::mutex* mutexPtr;
	};
}