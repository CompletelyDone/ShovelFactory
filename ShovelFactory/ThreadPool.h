#pragma once
#include <vector>
#include <mutex>
#include <condition_variable>
#include <thread>
#include "Task.h"

using namespace std;

class ThreadPool
{
private:
	int currentTaskId;
	bool stop;


	mutex taskMutex;


	condition_variable cvTask;
	condition_variable cvWaitTask;


	vector<int> completedTask;
	vector<thread> threads;
	vector<Task> tasks;

	void TaskRun()
	{
		while (!stop)
		{
			unique_lock<mutex> lock(taskMutex);
			while (tasks.empty())
			{
				cvTask.wait(lock);
			}
			auto newTask = move(tasks.back());
			tasks.pop_back();
			lock.unlock();
			newTask.Execute();
			completedTask.push_back(newTask.id);
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
	int AddTask(Task& task)
	{
		lock_guard<mutex> lock(taskMutex);
		int newId = ++currentTaskId;
		task.id = newId;
		tasks.push_back(task);
		cvTask.notify_all();
		return newId;
	}
	void Wait(const int taskId)
	{
		unique_lock<mutex> lock(taskMutex);
		cvWaitTask.wait(lock, [this, taskId] {return stop || find(completedTask.begin(), completedTask.end(), taskId) != completedTask.end(); });
		cvWaitTask.notify_all();
		lock.unlock();
	}
	void WaitAll()
	{
		unique_lock<mutex> lock(taskMutex);
		cvWaitTask.wait(lock, [this] {return stop || tasks.size() == currentTaskId; });
		cvTask.notify_all();
		lock.unlock();
	}
	bool IsCalculated(int taskId)
	{
		lock_guard<mutex> lock(taskMutex);
		bool result = find(completedTask.begin(), completedTask.end(), taskId) != completedTask.end();
		cvTask.notify_all();
		return result;
	}
	void Shutdown()
	{
		lock_guard<std::mutex> lock(taskMutex);
		stop = true;
		cvTask.notify_all();
		for (auto& thread : threads) 
		{
			thread.join();
		}
	}
};

