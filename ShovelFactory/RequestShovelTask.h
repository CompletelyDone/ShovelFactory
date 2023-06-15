#pragma once
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include "ThreadPool.h"
#include "RequestShovelTask.h"
#include "Task.h"
#include "Storage.h"
#include "Shovel.h"

using namespace std;

class RequestShovelTask : public Task
{
private:
	int speed;

	mutex taskMutex;

	Storage<ShovelBlade>& storageBlade;
	Storage<ShovelHandle>& storageHandle;
	Storage<Shovel>& storageShovel;

	condition_variable cvController;
public:
	RequestShovelTask(Storage<ShovelBlade>& _storageBlade, 
		Storage<ShovelHandle>& _storageHandle, 
		Storage<Shovel>& _storageShovel,
		int _speed) : 
		storageBlade(_storageBlade),
		storageHandle(_storageHandle),
		storageShovel(_storageShovel),
		speed(_speed) {}
	RequestShovelTask(const RequestShovelTask& other) : 
		storageBlade(other.storageBlade),
		storageHandle(other.storageHandle),
		storageShovel(other.storageShovel),
		speed(other.speed)
	{}
	void Execute() override
	{
		unique_lock<mutex> lock(taskMutex);
		while (storageShovel.IsFull())
		{
			storageShovel.cvStorageShovelLeft.wait(lock);
		}
		ShovelBlade blade = storageBlade.GetItem();
		ShovelHandle handle = storageHandle.GetItem();
		Shovel shovel(blade, handle);
		storageShovel.AddItem(shovel);
		lock.unlock();

		this_thread::sleep_for(chrono::milliseconds(speed));
	}
};

