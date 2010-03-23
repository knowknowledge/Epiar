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
// Tests
#include "Tests/graphics.h"
#include "Tests/argparser.h"
// Header files for various subsystems
#include "Audio/audio.h"
#include "Graphics/font.h"
#include "Graphics/video.h"
#include "Input/input.h"
#include "UI/ui.h"
#include "Utilities/timer.h"
#include "Utilities/xml.h"

// main configuration file, used through the tree (extern in common.h)
extern XMLFile *optionsfile;
// main font used throughout the game
extern Font *SansSerif, *BitType, *Serif, *Mono;

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

/**\brief Initializes Epiar options.*/
void Test::InitializeOptions( void ){
	optionsfile = new XMLFile( "Resources/Definitions/options.xml" );
}

/**\brief Initializes Epiar Video.*/
void Test::InitializeVideo( void ){
	Video::Initialize();
	Video::SetWindow( 1024, 768, 32 );
}

/**\brief Initializes Epiar Audio.*/
void Test::InitializeAudio( void ){
	Audio::Instance().Initialize();
	Audio::Instance().SetMusicVol ( 0.5f );
	Audio::Instance().SetSoundVol ( 0.5f );
}

/**\brief Initializes Epiar fonts.*/
void Test::InitializeFonts( void ){
	SansSerif       = new Font( "Resources/Fonts/FreeSans.ttf" );
	BitType         = new Font( "Resources/Fonts/visitor2.ttf" );
	Serif           = new Font( "Resources/Fonts/FreeSerif.ttf" );
	Mono            = new Font( "Resources/Fonts/FreeMono.ttf" );
}

/**\brief Simple Game loop.*/
void Test::GameLoop( void ){
	bool quit=false;
	Input inputs;
	Timer::Update();
	while( !quit ) {
		quit = inputs.Update();
		
		int logicLoops = Timer::Update();
		while(logicLoops--) {
				// Update cycle
		}

		UI::Run();
		Video::Erase();
		Video::Update();
		UI::Draw();
		Timer::Delay();
	}
}
