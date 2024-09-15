#include "MutexLock.h"
#include "Uncopyable.h"

namespace dedi
{
	MutexLockHelper::MutexLockHelper(std::mutex* pm) : mutexPtr(pm)
	{
		// 인스턴스 생성 때 즉시 lock
		mutexPtr->lock();
	}

	MutexLockHelper::~MutexLockHelper()
	{
		// 블록을 넘어가서 소멸자 호출 때 자동으로 unlock
		mutexPtr->unlock();

	}
}