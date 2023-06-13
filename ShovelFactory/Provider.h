#pragma once
#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>
#include "Storage.h"
#include "ShovelPart.h"

using namespace std;

template <typename T>
class Provider
{
private:
	int speed;
	atomic<bool> stop;
	Storage<T>& storage;
	thread newThread;
	mutex providerMutex;
	condition_variable cvProvider;
public:
	Provider(Storage<T>& _storage, int _speed) : storage(_storage), speed(_speed), stop(true) {}

	void StartWork()
	{
		stop = false;
		newThread = thread([this]()
			{
				while (!stop)
				{
					unique_lock<mutex> lock(providerMutex);
					while (storage.IsFull())
					{
						storage.cvStorageNotFull.wait(lock);
					}
					T item;
					storage.AddItem(item);
					cvProvider.notify_one();
					lock.unlock();
					this_thread::sleep_for(chrono::milliseconds(speed));
				}
			}
		);
		newThread.detach();

	}

	void StopWork()
	{
		stop = true;
		if (newThread.joinable())
		{
			newThread.join();
		}
	}

};

