/**\file		argparser.cpp
 * \author		Maoserr
 * \date		Created: Sunday, March 21, 2010
 * \date		Modified: Sunday, March 21, 2010
 * \brief		Tests the command line parser.
 */

#include "includes.h"
#include "Utilities/argparser.h"

/**\brief This function tests all the available options of the ArgParser
 * class.*/
int test_argparser(int argc, char **argv){
	// Create an artifical argc and argv for testing
	int test_argc = 5;
	char *test_argv[]={(char*)"programname.exe",
			(char*)"-h",
			(char*)"--help",
			(char*)"--answer=blah",
			(char*)"an-argument"};
	ArgParser argpInst(test_argc,test_argv);

	// Test for arguments
	argpInst.SetOpt(SHORTOPT,"h","Display help screen");
	argpInst.SetOpt(LONGOPT,"help","Display help screen");
	argpInst.SetOpt(VALUEOPT,"answer","Display the answer to life");

	if ( argpInst.HaveShort("h"))
		cout<<"Success: Short option."<<endl;
	else{
		cout<<"Failed: Short option failed to parse."<<endl;
		return -1;
	}

	if ( argpInst.HaveLong("help") )
		cout<<"Success: Long option."<<endl;
	else{
		cout<<"Failed: Long option failed to parse."<<endl;
		return -1;
	}

	string ans=argpInst.HaveValue("answer");
	if ( !ans.empty() )
		cout<<"Success: Value option answer: "<<ans<<endl;
	else{
		cout<<"Failed: Value option failed to parse."<<endl;
		return -1;
	}

	// Note that we don't have to use SetOpt for arguments without "-"
	if ( argpInst.HaveArg("an-argument") )
		cout<<"Success: Argument"<<endl;
	else{
		cout<<"Failed: Argument failed to parse."<<endl;
		return -1;
	}

	return 0;
}
