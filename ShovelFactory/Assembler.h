#pragma once
#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>
#include "Storage.h"
#include "Shovel.h"


class Assembler
{
private:
	int speed;
	atomic<bool> stop;
	Storage<ShovelBlade>& storageBlade;
	Storage<ShovelHandle>& storageHandle;
	Storage<Shovel>& storageShovel;
	thread assemblerThread;
	condition_variable cvAssembler;
public:
	mutex assemblerMutex;
	Assembler(Storage<Shovel>& _storageShovel, Storage<ShovelBlade>& _storageBlade, Storage<ShovelHandle>& _storageHandle, int _speed)
		: storageShovel(_storageShovel), storageBlade(_storageBlade), storageHandle(_storageHandle), speed(_speed), stop(true) {}
	void StartWork()
	{
		stop = false;
		assemblerThread = thread([this]()
			{
				while (!stop)
				{
					unique_lock<mutex> lockBlade(storageBlade.storageMutex, defer_lock);
					unique_lock<mutex> lockHandle(storageHandle.storageMutex, defer_lock);
					unique_lock<mutex> lockAssembler(assemblerMutex, defer_lock);
					lock(lockBlade, lockHandle,lockAssembler);
					while (storageShovel.IsFull())
					{
						storageShovel.cvStorageNotFull.wait(lockAssembler);
					}
					ShovelBlade blade = storageBlade.GetItem();
					ShovelHandle handle = storageHandle.GetItem();
					Shovel shovel(blade, handle);
					storageShovel.AddItem(shovel);
					cvAssembler.notify_one();
					lockBlade.unlock();
					lockHandle.unlock();
					lockAssembler.unlock();

					this_thread::sleep_for(chrono::milliseconds(speed));
				}
			}
		);
		assemblerThread.detach();
	}
	void StopWork()
	{
		stop = true;
		if (assemblerThread.joinable()) {
			assemblerThread.join();
		}
	}
};

