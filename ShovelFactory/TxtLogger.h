#pragma once
#include <fstream>
#include <iostream>
#include <ctime>

using namespace std;

class TxtLogger
{
private:
	string title;
public:
	TxtLogger(string _title) : title(_title) {}
	void Log(string text)
	{
		ofstream logfile(title, ios::app);
		time_t now = time(nullptr);
		struct tm timeinfo;
		localtime_s(&timeinfo, &now);
		char timestamp[100];
		strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &timeinfo);
		string message = text;
		logfile << timestamp << " " << message << "\n";
		logfile.close();
	}
};

