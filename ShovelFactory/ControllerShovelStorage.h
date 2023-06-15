#pragma once
#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>
#include "Storage.h"
#include "Shovel.h"
#include "ThreadPool.h"
#include "RequestShovelTask.h"

using namespace std;

class ControllerShovelStorage
{
private:
	int speed;
	atomic<bool> stop;

	ThreadPool<RequestShovelTask>& thrdPool;

	Storage<ShovelBlade>& storageBlade;
	Storage<ShovelHandle>& storageHandle;
	Storage<Shovel>& storageShovel;

	mutex controllerMutex;
	thread controllerThread;
public:
	ControllerShovelStorage(ThreadPool<RequestShovelTask>& _thrdPool,
		Storage<Shovel>& _storageShovel, 
		Storage<ShovelBlade>& _storageBlade, 
		Storage<ShovelHandle>& _storageHandle, 
		int _speed)
	: thrdPool(_thrdPool), 
		storageShovel(_storageShovel), 
		storageBlade(_storageBlade), 
		storageHandle(_storageHandle), 
		speed(_speed), 
		stop(true) {}
	~ControllerShovelStorage()
	{
		StopWork();
		if (controllerThread.joinable())
		{
			controllerThread.join();
		}
		controllerMutex.unlock();
	}
	void StartWork()
	{
		stop = false;
		controllerThread = thread([this]
			{
				while (!stop)
				{
					unique_lock<mutex> lockController(controllerMutex);
					while (!thrdPool.IsEmpty())
					{
						thrdPool.taskIsEmpty.wait(lockController);
					}
					RequestShovelTask newTask(storageBlade, storageHandle, storageShovel, speed);
					thrdPool.AddTask(newTask);
					lockController.unlock();
				}
			});
		controllerThread.detach();
	}
	void StopWork()
	{
		stop = true;
		if (controllerThread.joinable())
		{
			controllerThread.join();
		}
	}
};

