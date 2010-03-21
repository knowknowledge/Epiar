/**\file		tests.h
 * \author		Maoserr
 * \date		Created: Saturday, March 20, 2010
 * \date		Modified: Saturday, March 20, 2010
 * \brief		Tests framework.
 * \details
 * This file sets-up the functionality to test individual components of Epiar
 */

// Function pointer to the test function
typedef int (*testFunc)(int argc, char **argv);

class Test{
	public:
		Test( const string& testname );
		int RunTest( int argc, char **argv);

	private:
		map<string,testFunc> tests;
		bool validtest;
		string testname;
};
