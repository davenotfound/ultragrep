#pragma once
/*!	\file		ConsoleApplication.hpp
	\author		Garth Santor
	\date		2019-01-05

	ConsoleApplication class declaration.
*/

// Setup DBG environment
#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>

// Create a dbg version of new that provides more information
#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#else
#define DBG_NEW new
#endif

// Remaining includes
#include <string>
#include <vector>



/*!	\brief	ConsoleApplication is the base-class of the framework.
*/
class ConsoleApplication {
	using Args = std::vector<std::wstring>;
	static ConsoleApplication* thisApp_sm;
	friend int wmain(int argc, wchar_t* argv[]);
	int wmain(int argc, wchar_t* argv[]);
	Args args_m;
protected:
	ConsoleApplication();
	virtual ~ConsoleApplication() {}
	virtual int execute();

	/*!	Access th ecommand-line arguments container.
	*/
	Args const& get_args() const { return args_m; }
};