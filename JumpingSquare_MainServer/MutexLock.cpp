#include "MutexLock.h"

MutexLockHelper::MutexLockHelper(std::mutex* pm) : mutexPtr(pm)
{
	// �ν��Ͻ� ���� �� ��� lock
	mutexPtr->lock();
}

MutexLockHelper::~MutexLockHelper()
{
	// ������ �Ѿ�� �Ҹ��� ȣ�� �� �ڵ����� unlock
	mutexPtr->unlock();
}