/*
File:  LineData.hpp
Author: David Harris, 0807594
Date: November 5th, 2019
Purpose: Class declaration for the LineData object. Used as a sub object data container of the FileData class.
*/

#pragma once
#include <string>
using namespace std;

class LineData {
public:
	int line_number;
	int num_matches;
	wstring line_content;
	LineData(int lineNumber, int numMatches, wstring lineContent) {
		line_number = lineNumber;
		num_matches = numMatches;
		line_content = lineContent;
	}
};