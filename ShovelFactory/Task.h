#pragma once

class Task
{
private:
public:
	int id = 0;
	virtual void Execute() = 0;
	virtual ~Task() {}
};

