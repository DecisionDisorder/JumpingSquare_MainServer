#pragma once
#include <mutex>
#include "Uncopyable.h"

class MutexLockHelper : private Uncopyable
{
public:
	explicit MutexLockHelper(std::mutex* pm);

	~MutexLockHelper();

private:
	std::mutex* mutexPtr;
};