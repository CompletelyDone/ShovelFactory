#pragma once
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdlib.h>

using namespace std;

class ShovelPart
{
private:
	int uid;
public:
	ShovelPart()
	{
		uid = rand() * rand() * rand();
	}
	string ToString()
	{
		stringstream ss;
		ss << "Id Part: " << uid;
		return ss.str();
	}
};

