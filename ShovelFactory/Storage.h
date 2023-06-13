#pragma once
#include <mutex>
#include <condition_variable>
#include <vector>
#include <exception>
#include "ShovelPart.h"

using namespace std;

template <typename T>
class Storage
{
private:
	int capacity;
	vector<T> items;
	mutex storageMutexAdd;
	condition_variable cvStorage;
public:
	mutex storageMutex;
	condition_variable cvStorageNotEmpty;
	condition_variable cvStorageNotFull;
	Storage(int _capacity) : capacity(_capacity) {}
	size_t Count()
	{
		lock_guard<mutex> lock(storageMutex);
		size_t tmp = items.size();
		cvStorage.notify_all();
		return tmp;
	}
	bool IsFull()
	{
		lock_guard<mutex> lock(storageMutex);
		bool tmp = items.size() == capacity;
		cvStorage.notify_all();
		return tmp;
	}
	bool IsEmpty()
	{
		lock_guard<mutex> lock(storageMutex);
		bool tmp = items.empty();
		cvStorage.notify_all();
		return tmp;
	}
	void AddItem(const T& item)
	{
		lock_guard<mutex> lock(storageMutex);
		if (items.size() < capacity)
		{
			items.push_back(item);
		}
		cvStorageNotEmpty.notify_all();
		cvStorage.notify_all();
	}
	T GetItem()
	{
		unique_lock<mutex> lock(storageMutexAdd);
		while (items.empty())
		{
			cvStorageNotEmpty.wait(lock);
		}
		T item = items.back();
		items.pop_back();
		cvStorageNotFull.notify_all();
		lock.unlock();
		return item;
	}
};

