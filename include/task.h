#pragma once
#include <vector>
#include <string>
#include <deque>
#include <mutex>
#include <chrono>
#include <cmath>
#include <thread>
using namespace std;
class Client
{
public:
	int id;
	int allC;
	int timeInQueue;
	int countProduct;
	int timeService;
	chrono::system_clock::time_point tp;
	Client(int avg);
	void ExitCashbox(int t);
	void EnterQueue();
	void ExitQueue();
};
class Shop
{
private:
		int countCB;
		int maxQueue;
		int serviceSpeed;
		int requestRate;
		int avgItems;
		int completedCount;
		int rejectedCount;
		int queueLen;
		int countProc;
		struct CashBox
		{
			int timeWait;
			int timeWork;
			std::thread thrd;
		};
	vector<CashBox> cashBoxes;
	deque<Client*> queueClients;
	mutex mu;
public:
	Shop(int countCashBox, int requestRate, int serviceSpeed, int avgItems, int maxCount);
	void Start(int index);
	void Simulation();
	
};