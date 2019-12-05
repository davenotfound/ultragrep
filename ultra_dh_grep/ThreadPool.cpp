/*
File:  ThreadPool.cpp
Author: David Harris, 0807594
Date: November 8th, 2019
Purpose: Implementation of the ThreadPool class, outlines the construction and destruction 
			of threads, and maintains the data surrounding the performance numbers, as well as the function they will launch
*/
#include "ThreadPool.hpp"

ThreadPool::ThreadPool(function<void()> funcThread, function<void(double & el)> funcReport)
{
	report_function = funcReport;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&start);

	for (int i = 0; i <= 8; i++) {
		thread_pool.push_back(thread(funcThread));
	}
}

ThreadPool::~ThreadPool()
{
	for (auto& t : thread_pool)
		t.join();

	QueryPerformanceCounter(&stop);

	elapsed = (stop.QuadPart - start.QuadPart) / double(frequency.QuadPart);
	report_function(elapsed);
}
