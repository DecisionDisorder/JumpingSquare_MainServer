#include "MutexLock.h"

MutexLockHelper::MutexLockHelper(std::mutex* pm) : mutexPtr(pm)
{
	mutexPtr->lock();
}

MutexLockHelper::~MutexLockHelper()
{
	mutexPtr->unlock();
}