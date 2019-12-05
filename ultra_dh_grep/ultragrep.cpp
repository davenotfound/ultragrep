/*
File:  ultragrep.cpp
Author: David Harris, 0807594
Date: November 5th, 2019
Purpose: Implementation of the ultragrep class. Responsible for recieving command line input and 
			scanning files in a directory for a regex keyword through a multithreaded system and reporting the results.
*/
#include "ultragrep.hpp"
#include <vector>
#include <iostream>
#include <regex>
#include <sstream>
#include <fstream>
#include <thread>
#include "LineData.hpp"
#include "FileData.hpp"
#include <filesystem>
#include <mutex>
#include <queue>
#include "ThreadPool.hpp"
#include <map>
#include <Windows.h>
using namespace std;

void readFile();
void threadReport(double& el);

// thread behaviour variables
mutex wakeMtx;
mutex taskMutex;
mutex pushMtx;
condition_variable wakeCond;
bool morePossibleWork = true;

// command line related variables
bool verbose = false;
wstring directory;
wregex searchParam;
vector<wstring> extensions;
map<string, FileData> fileList;

// threadpool construction alongside the functions it needs to call
ThreadPool threadPool(readFile, threadReport);

// queue of files to be read
queue<std::filesystem::directory_entry> files;

// barrier related variables
unsigned barrierThreshold = 10;
unsigned barrierCount = barrierThreshold;
unsigned barrierGeneration = 0;
mutex barrierMtx;
condition_variable barrierCond;

// barrier to halt threads until all are at this point and may proceed
void barrier() {
	unique_lock<mutex> lock(barrierMtx);
	unsigned gen = barrierGeneration;
	if (--barrierCount == 0) {
		++barrierGeneration;
		barrierCount = barrierThreshold;
		barrierCond.notify_all();
	}
	else {
		while (gen == barrierGeneration)
			barrierCond.wait(lock);
	}
}

// responsible for relaying user errors regarding commandline input
void invalidInput(string info, wstring folderName) {
	if (info == "regex") {
		cout << "Error: No regular expression";
	}
	else if (info == "folder") {
		wcout << "Folder " << "<" << folderName << " > doesn't exist" << endl;
	}
}

// responsible for instruction regarding how to use the program
void howToUse() {
	cout << "ultragrep d. harris 2019, for g. santor" << endl;
	cout << "Usage: [-v] folder regex [.ext]*" << endl;
}

// function to verify if the -v input was found within the command line at the correct position in the input
// through a boolean return
bool verboseCheck(wstring & arg) {
	if (arg == L"-v")
		return true;
	else
		return false;
}

// function to parse the potential file extension list provided in the command line args, returns a vector of the parsed
// file extensions
vector<wstring> parseExtensions(wstring extensionList) {
	vector<wstring> extensionsVec;
	std::wstringstream wss(extensionList);
	wstring temp;

	while (std::getline(wss, temp, L'.'))
		extensionsVec.push_back(temp);

	return extensionsVec;
}

// function that will iterate through the directory based on the path passed to it
void getFiles(wstring path) {
	// ensures threads are ready prior to continuing to ensure the directory is not read before the threads are initialized 
	barrier();

	// loop through the directory and add files to the queue, if there is nothing we default to text files
	for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(path)) {
		wstring ext = std::filesystem::path(dirEntry.path()).extension().wstring();
		if (extensions.size() == 0) {
			if (ext == L".txt") {
				files.push(dirEntry);
				wakeCond.notify_one();
			}
		}
		else {
			// remove the . from the extension so we can do a find call on our existing container
			ext = ext.substr(1);
			if (find(extensions.begin(), extensions.end(), ext) != extensions.end()) {
				// add to the file queue (tasks)
				files.push(dirEntry);
				// task to be completed, notify a thread to start executing
				wakeCond.notify_one();
			}
		}
	}
	
	// we know that there are no other files to be read from the directory at this point
	morePossibleWork = false;
	wakeCond.notify_all();
}

void readFile() {
	// ensures threads are ready prior to continuing to ensure the directory is not read before the threads are initialized 
	barrier();

	while (morePossibleWork) {

		// puts thread to sleep until it is notified to being working on the file queue
		{
			unique_lock<mutex> lk(wakeMtx);
			wakeCond.wait(lk);
		}

		// if we have files in our queue we pop
		while (!files.empty()) {

			//dclp to ensure the thread avoids race conditions when popping a file off the queue
			std::filesystem::directory_entry file;
			bool haveTask = false;

			{
				lock_guard<mutex> lk(taskMutex);
				if (!files.empty()) {
					file = files.front();
					files.pop();
					haveTask = true;
				}
			}

			if (haveTask) {
				if (verbose) {
					cout << "thread [" << this_thread::get_id() << "]  starting grep of: " << file.path().c_str() << endl;
				}

				// temp filedata object to store file data for this threads file
				FileData tempFd(file.path().string());

				wifstream infile(file);
				wsmatch match;
				int lineCount = 0;
				if (infile.good()) {
					// loop through file lines and perform regex search based on search paramater
					for (wstring line; getline(infile, line); )
					{
						lineCount++;
						int matchCount = 0;
						while (regex_search (line, match, searchParam)) {
							// loop through matches on a specific line and store them in a temp line data object and append to
							// temp file data parent object
							for (auto x : match) {
								matchCount++;
								if (verbose) {
									wcout << "thread [" << this_thread::get_id() << "] matched " << matchCount << ": " << file.path().c_str() << " on line [" << lineCount << "] " << line << endl;
								}
								LineData tempLine(lineCount, matchCount, line);
								tempFd.match_data.push_back(tempLine);
							}
							line = match.suffix().str();
						}
					}
					// if the scan yielded matches we append it to our global container for the final report later
					if (tempFd.match_data.size() > 0) {
						{
							lock_guard<mutex> lk(pushMtx);
							fileList[tempFd.file_name] = tempFd;
						}
					}
				}
			}
		}
	}
}

// function responsible for the final report, will loop through the completed file list container for the 
// information stored within each filedata object, recieves the elapsed time from the destructed threadpool object
// to report the performance of the total scan
void threadReport(double & el) {
	cout << "\nGrep Report:" << endl;

	int count = 0;
	map<string, FileData>::iterator it;
	for (it = fileList.begin(); it != fileList.end(); it++) {
		cout << endl;
		cout << it->first << endl;
		for (auto ld : it->second.match_data) {
			wcout << "[" << ld.line_number << "] " << ld.line_content << endl;
			count = count + ld.num_matches;
		}
	}

	cout << endl;
	cout << "Files with matches: " << fileList.size() << endl;
	cout << "Total number of matches: " << count << endl;
	cout << "Scan completed in: " << el << "s" << endl;
}

int ultragrep::execute() {

	// if tree for user input scenarios
	// accounts for different valid launching scenarios as well as general invalid input scenarios
	if (get_args().size() == 1) {
		howToUse();
	}
	else if (get_args().size() == 2) {
		invalidInput("regex", L"");
		return EXIT_FAILURE;
	}
	else if (get_args().size() > 2) {
		wstring arg1 = get_args()[1];
		if (verboseCheck(arg1)) {
			verbose = true;
			if (std::filesystem::exists(get_args()[2])) {
				directory = get_args()[2];
			}
			else {
				invalidInput("folder", directory);
				return EXIT_FAILURE;
			}
			searchParam = wregex(get_args()[3]);
			if (get_args().size() >= 5) {
				wstring extensionList = get_args()[4];
				extensions = parseExtensions(extensionList);
			}
			getFiles(directory);
		}
		else {
			if (std::filesystem::exists(get_args()[1])) {
				directory = get_args()[1];
			}
			else {
				invalidInput("folder", directory);
				return EXIT_FAILURE;
			}
			searchParam = wregex(get_args()[2]);
			if (get_args().size() >= 4) {
				wstring extensionList = get_args()[3];
				extensions = parseExtensions(extensionList);
			}
			getFiles(directory);
		}
	}

	return 0;
}