#pragma once
#include <vector>
#include <mutex>
#include <condition_variable>
#include <thread>
#include "Task.h"
#include "RequestShovelTask.h"

using namespace std;

template<typename T>
class ThreadPool
{
private:
	int currentTaskId;
	bool stop;

	mutex taskMutex;
	mutex taskCompletedMutex;

	condition_variable cvTask;
	condition_variable cvCompletedTask;

	vector<int> completedTask;
	vector<thread> threads;
	vector<T> tasks;

	void TaskRun()
	{
		while (!stop)
		{
			unique_lock<mutex> lock(taskMutex);
			while (tasks.empty())
			{
				cvTask.wait(lock);
			}
			T newTask = move(tasks.back());
			tasks.pop_back();
			cvTask.notify_all();
			taskIsEmpty.notify_one();
			lock.unlock();
			newTask.Execute();
			lock_guard<mutex> lockCompletedTask(taskCompletedMutex);
			completedTask.push_back(newTask.id);
			cvCompletedTask.notify_all();
		}
	}
public:
	ThreadPool(int n) : currentTaskId(0), stop(false)
	{
		threads.reserve(n);
		for (int i = 0; i < n; i++)
		{
			threads.emplace_back(&ThreadPool::TaskRun, this);
		}
	}
	int AddTask(T& task)
	{
		lock_guard<mutex> lock(taskMutex);
		int newId = ++currentTaskId;
		task.id = newId;
		tasks.push_back(task);
		cvTask.notify_one();
		cvCompletedTask.notify_all();
		return newId;
	}
	void Wait(const int taskId)
	{
		unique_lock<mutex> lock(taskCompletedMutex);
		cvCompletedTask.wait(lock, [this, taskId] {return stop || find(completedTask.begin(), completedTask.end(), taskId) != completedTask.end(); });
		cvCompletedTask.notify_all();
		lock.unlock();
	}
	void WaitAll()
	{
		unique_lock<mutex> lock(taskCompletedMutex);
		cvCompletedTask.wait(lock, [this] {return stop || tasks.size() == currentTaskId; });
		cvTask.notify_all();
		lock.unlock();
	}
	bool IsCalculated(int taskId)
	{
		lock_guard<mutex> lock(taskCompletedMutex);
		bool result = find(completedTask.begin(), completedTask.end(), taskId) != completedTask.end();
		cvTask.notify_all();
		return result;
	}
	void Shutdown()
	{
		unique_lock<mutex> lockTask(taskMutex, defer_lock);
		unique_lock<mutex> lockCompletedTask(taskCompletedMutex, defer_lock);
		lock(lockTask, lockCompletedTask);
		stop = true;
		cvTask.notify_all();
		lockTask.unlock();
		lockCompletedTask.unlock();
		for (auto& thread : threads) 
		{
			thread.join();
		}
	}
	int TotalCompletedTasks()
	{
		lock_guard<mutex> lockCompletedTask(taskCompletedMutex);
		int tmp = completedTask.size();
		return tmp;
	}

	condition_variable taskIsEmpty;
	bool IsEmpty()
	{
		lock_guard<mutex> lockTask(taskMutex);
		return tasks.empty();
	}
};

