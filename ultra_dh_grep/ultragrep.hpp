/*
File:  ultragrep.hpp
Author: David Harris, 0807594
Date: November 5th, 2019
Purpose: Declaration of the ultragrep class. Primary focus is inheriting from the ConsoleApplication class in
		 order to override the execute() method to provide a simple way to access command line arguments
*/

#pragma once
#include "ConsoleApplication.hpp"


class ultragrep : public ConsoleApplication {
	int execute() override;
}ultragrep;