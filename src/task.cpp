#include "task.h"
#include <iostream>
using namespace std;
long Fact(long n)
{
    if (n == 0)
        return 1;
    else
        return n * Fact(n - 1);
}



Client::Client(int avgItems)
{
    timeInQueue = 0;
    timeService = 0;
    countProduct = rand() % (2 * avgItems);
    id = allC++;
}

void Client::EnterQueue()
{
   tp = chrono::system_clock::now();
}
void Client::ExitQueue()
{
    timeInQueue = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - tp).count();
}



void Client::ExitCashbox(int time)
{
    timeService = time;
}










Shop::Shop(int cashboxCount, int requestRate, int servicespeed, int avgItems, int maxC)
{
    countCB = cashboxCount;
    this->requestRate = requestRate;
    this->serviceSpeed = servicespeed;
    this->avgItems = avgItems;
    maxQueue = maxC;

    completedCount = 0;
    rejectedCount = 0;

    queueLen = 0;
    countProc = 0;

    cashBoxes = vector<CashBox>();
    queueClients = deque<Client*>();

    for (int i = 0; i < countCB; i++)
    {
        cashBoxes.push_back(CashBox());
        cashBoxes[i].thrd = thread(&Shop::Start, this, i);
    }
}

void Shop::Start(int index)
{
    while (completedCount + rejectedCount < requestRate)
    {
        mu.lock();
        queueLen += queueClients.size();
        countProc++;
        if (queueClients.size() > 0)
        {
            queueClients.at(0)->ExitQueue();
            int items = queueClients.at(0)->countProduct;
            int id = queueClients.at(0)->id;
            cashBoxes[index].timeWork += items * serviceSpeed;
            completedCount++;
            queueClients.at(0)->ExitCashbox(items * serviceSpeed);
            queueClients.pop_front();
            mu.unlock();
            while (items > 0)
            {
                this_thread::sleep_for(chrono::milliseconds(serviceSpeed));
                items -= 1;
            }
             cout<< "������ � id " << id << " ��������" << endl;
        }
        else
        {
            cashBoxes[index].timeWait += 10;
            mu.unlock();
            this_thread::sleep_for(chrono::milliseconds(10));
        }
    }
}

void Shop::Simulation()
{
    srand(time(0));

    string out = "";
    int countClients = 0;
    vector<Client> clients;

    for (int i = 0; i < requestRate; i++)
    {
        clients.push_back(Client(avgItems));
    }

    double avgQueueLen = 0.0;

    while (countClients < requestRate)
    {
        int time = rand() % 1000;
        this_thread::sleep_for(std::chrono::milliseconds(time));
        mu.lock();
        avgQueueLen += queueClients.size();
        if (queueClients.size() < maxQueue)
        {
            queueClients.push_back(&clients.at(countClients));
            clients.at(countClients).EnterQueue();
            mu.unlock();
        }
        else
        {
            mu.unlock();
            rejectedCount++;
            cout << "������ � id " << clients.at(countClients).id << " �� ��������" << endl;
        }
        countClients++;
    }

    double avgWait = 0.0;
    double avgWork = 0.0;
    double avgTimeQueue = 0.0;
    double avgTimeCashBox = 0.0;
    double workTime = 0.0;

    for (int i = 0; i < countCB; i++)
    {
        cashBoxes[i].thrd.join();
        avgWait += cashBoxes[i].timeWait;
        avgWork += cashBoxes[i].timeWork;
    }
    for (int i = 0; i < requestRate; i++)
    {
        if (clients.at(i).timeInQueue == 0 && clients.at(i).timeService == 0)
            continue;
        avgTimeQueue += clients.at(i).timeInQueue;
        avgTimeCashBox += clients.at(i).timeService;
    }

    avgQueueLen /= (double)countClients;
    workTime = avgWork + avgWait;
    avgWait /= (double)countCB;
    avgWork /= (double)countCB;
    avgTimeQueue /= (double)completedCount;
    avgTimeCashBox /= (double)completedCount;

    double l = requestRate * 1000.0 / avgWork;
    double m = 1000.0 / avgTimeCashBox;
    double r = l / m;
    double P0 = 1.0;
    for (int i = 1; i <= countCB; i++)
        P0 += pow(r, i) / Fact(i);
    for (int i = countCB + 1; i < countCB + maxQueue; i++)
        P0 += pow(r, i) / (Fact(countCB) * pow(countCB, i - countCB));

    P0 = 1.0 / P0;
    double Prej = pow(r, countCB + maxQueue) * P0 / (double)(pow(countCB, maxQueue) * Fact(countCB));
    double Q = 1.0 - Prej;
    double A = l * Q;

    

    cout<< "\n����������� �����������: " << to_string(completedCount);
    cout <<"\n������������� �����������: " << to_string(rejectedCount);
    cout  <<"\n������� ����� �������: " << to_string(avgQueueLen);
    cout  <<"\n������� ����� ���������� ���������� � ������� + �� �����: " << to_string(avgTimeQueue) << " + " << to_string(avgTimeCashBox);
    cout <<"\n������� ����� ������ �����: " << to_string(avgWork);
    cout  <<"\n������� ����� ��������� �����: " << to_string(avgWait);

    cout << "\n����������� ������: " << to_string((double)rejectedCount / (double)requestRate);
    cout<< "\n������������� ���������� ����������� ��������: " << to_string((double)completedCount / (double)requestRate);
    cout<< "\n���������� ���������� �����������: " << to_string(l * (double)completedCount / (double)requestRate);

    cout << "\n\n������������� ������\n";

    cout << "\n����������� ������: " << to_string(Prej);
    cout << "\n������������� ���������� �����������: " << to_string(Q);
    cout << "\n���������� ���������� �����������: " << to_string(A);

    
}