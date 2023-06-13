#pragma once

class Task
{
private:
public:
	int id;
	virtual void Execute() = 0;
	virtual ~Task() {}
};

