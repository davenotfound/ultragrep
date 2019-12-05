/*
File:  ThreadPool.hpp
Author: David Harris, 0807594
Date: November 5th, 2019
Purpose: Declaration of the ThreadPool class that outlines the general behaviour and data of a ThreadPool object that manages 
			this application's threads.
*/
#pragma once
#include <thread>
#include <vector>
#include <functional>
#include <Windows.h>
using namespace std;

class ThreadPool {
private:
	vector<thread> thread_pool;
	LARGE_INTEGER frequency, start, stop;
	double elapsed;
public: 
	function<void(double& el)> report_function;
	ThreadPool(std::function<void()> funcThread, function<void(double & el)> funcReport);
	~ThreadPool();
};