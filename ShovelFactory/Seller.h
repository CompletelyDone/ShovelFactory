#pragma once
#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>
#include <fstream>
#include "Storage.h"
#include "Shovel.h"
#include "Storage.h"

class Seller
{
private:
	int speed;
	atomic<bool> stop;

	Storage<Shovel>& storage;

	mutex sellerMutex;
	thread sellerThread;
	condition_variable cvSeller;
public:
	Seller(Storage<Shovel>& _storage, int _speed) : storage(_storage), speed(_speed), stop(true) {}
	void StartWork()
	{
		stop = false;
		sellerThread = thread([this]()
			{
				while (!stop)
				{
					unique_lock<mutex> lockSeller(sellerMutex);
					while (storage.IsEmpty())
					{
						storage.cvStorageNotEmpty.wait(lockSeller);
					}
					Shovel shovel = storage.GetItem();
					stringstream ss;
					ss << shovel.ToString() << endl;
					{
						ofstream out("C:\\logs\\mylog.txt", ios::app);
						if (out.is_open())
						{
							out << ss.str();
						}
						out.close();
					}
					cvSeller.notify_all();
					lockSeller.unlock();

					this_thread::sleep_for(chrono::milliseconds(speed));
				}
			}
		);
		sellerThread.detach();
	}
	void StopWork()
	{
		stop = true;
		if (sellerThread.joinable()) {
			sellerThread.join();
		}
	}
};

