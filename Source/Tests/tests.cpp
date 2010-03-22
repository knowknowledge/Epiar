/**\file		tests.cpp
 * \author		Maoserr
 * \date		Created: Saturday, March 20, 2010
 * \date		Modified: Saturday, March 20, 2010
 * \brief		Tests framework.
 * \details
 * This file implements functionality to test individual components of Epiar
 */

#include "includes.h"
#include "Tests/tests.h"
#include "Tests/graphics.h"
#include "Tests/argparser.h"

/**\class Test
 * \brief Encompasses a single test.*/

/**\brief Initializes a Test.*/
Test::Test( const string& testname ):testname( testname ){
	tests["graphics"]=test_graphics;
	tests["argparser"]=test_argparser;

}

/**\brief Runs the Test.*/
int Test::RunTest( int argc, char **argv){
	// Check to see if we just want a list of the tests
	if ( this->testname == "list-tests"){
		map<string,testFunc>::iterator it;
		cout<<"Test available-----------------------------------"<<endl;
		for ( it=this->tests.begin(); it != this->tests.end(); it++ ){
			cout<<"\t"<<(*it).first<<endl;
		}
		cout<<"-------------------------------------------------"<<endl;
		return 0;
	}
	// Runs the test
	if( tests.count(testname) > 0 ) {
		cout<<"Running test: "<<this->testname<<"..."<<endl;
		int retval = tests[this->testname](argc, argv);
		if (retval == 0)
			cout<<"Result: "<<this->testname<< " test succeeded."<<endl;
		else
			cout<<"Result: "<<this->testname<< " test failed. (returned "<<retval<<")"<<endl;
		return retval;
	}

	// Not a valid test
	cout<<"Unknown test specified: "<<this->testname<<endl;
	cout<<"Use '--run-test=list-tests' to show available tests"<<endl;
	return -1;
}
