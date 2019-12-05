/*!	\file		ConsoleApplication.cpp
	\author		Garth Santor
	\date		2019-01-05

	ConsoleApplication class implementation.
*/


#include "ConsoleApplication.hpp"
#include <iostream>
#include <exception>
using namespace std;



/*!	Process entry point.
	Calls the ConsoleApplication singleton's wmain.
*/
int wmain(int argc, wchar_t* argv[]) try {
#ifdef _DEBUG
	// Enable CRT memory leak checking.
	int dbgFlags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	dbgFlags |= _CRTDBG_CHECK_ALWAYS_DF;
	dbgFlags |= _CRTDBG_DELAY_FREE_MEM_DF;
	dbgFlags |= _CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag(dbgFlags);
#endif
	return ConsoleApplication::thisApp_sm->wmain(argc, argv);
}
catch (char const * msg) {
	wcerr << L"exception string: " << msg << endl;
}
catch (exception const& e) {
	wcerr << L"std::exception: " << e.what() << endl;
}
catch (...) {
	wcerr << L"Error: an exception has been caught...\n";
	return EXIT_FAILURE;
}



/*!	ConsoleApplication singleton instance pointer.
*/
ConsoleApplication* ConsoleApplication::thisApp_sm = nullptr;



/*!	wmain configures the application.
*/
int ConsoleApplication::wmain(int argc, wchar_t* argv[]) {
	args_m.assign(argv, argv + argc);
	return execute();
}



/*!	Singleton initialization and confirmation.
	Throws a logic_error if multiple instances are created.
*/
ConsoleApplication::ConsoleApplication() {
	if (thisApp_sm)
		throw std::logic_error("Error: ConsoleApplication already initialized!");
	thisApp_sm = this;
}



/*!	execute the application.
	Override this method in the derived class.
*/
int ConsoleApplication::execute() {
	wcout << "Console application framework: (c) 2019, Garth Santor\n";
	return EXIT_SUCCESS;
}