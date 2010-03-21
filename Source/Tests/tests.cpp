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
#include "Tests/lua_test.h"

/**\class Test
 * \brief Encompasses a single test.*/

/**\brief Initializes a Test.*/
Test::Test( const string& testname ):testname( testname ){
	tests["graphics"]=graphics_demo;
	tests["lua_test"]=lua_test;

	if( tests.count(testname) > 0 )
		this->validtest=true;
	else
		this->validtest=false;
}

/**\brief Runs the Test.*/
int Test::RunTest( int argc, char **argv){
	if ( validtest ){
		cout<<"Running test: "<<this->testname<<"..."<<endl;
		return tests[this->testname](argc, argv);
	}
	else{
		cout<<"Unknown test specified: "<<this->testname<<endl;
		return -1;
	}
}
