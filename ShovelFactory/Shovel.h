#pragma once
#include "ShovelBlade.h"
#include "ShovelHandle.h"

class Shovel
{
private:
	int uid;
	ShovelBlade& blade;
	ShovelHandle& handle;
public:
	Shovel(ShovelBlade& _blade, ShovelHandle& _handle) : blade(_blade), handle(_handle)
	{
		uid = rand() * rand() * rand() * rand();
	}
	string ToString()
	{
		stringstream ss;
		ss << "Id Shovel: " << uid << " Id Blade: " << blade.ToString() << " Id Handle: " << handle.ToString() << endl;
		return ss.str();
	}
};

