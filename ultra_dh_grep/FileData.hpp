/*
File:  FileData.hpp
Author: David Harris, 0807594
Date: November 5th, 2019
Purpose: Class declaration of the FileData class, used to store file specific information from a file scan.
*/
#pragma once
#include "LineData.hpp"
#include <vector>
#include <string>
using namespace std;

class FileData {
public:
	string file_name;
	vector<LineData> match_data;
	FileData() = default;
	FileData(string fileName) {
		file_name = fileName;
	}
};