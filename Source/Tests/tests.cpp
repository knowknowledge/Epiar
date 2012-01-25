/**\file		tests.cpp
 * \author		Maoserr
 * \date		Created: Saturday, March 20, 2010
 * \date		Modified: Saturday, March 20, 2010
 * \brief		Tests framework.
 * \details
 * This file implements functionality to test individual components of Epiar
 */

#include "includes.h"
#include "common.h"
#include "Tests/tests.h"
// Tests
#include "Tests/graphics.h"
#include "Tests/argparser.h"
#include "Tests/ui.h"
#include "Tests/font.h"
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
// Test requirements
#define REQUIRE_OPTIONS		(1L << 0)
#define REQUIRE_VIDEO		(1L << 1)
#define REQUIRE_AUDIO		(1L << 2)
#define REQUIRE_FONTS		(1L << 3)
 
/**\class Test
 * \brief Encompasses a single test.*/

/**\brief Initializes a Test.*/
Test::Test( const string& testname ):testname( testname ){
	tests["graphics"]=make_pair(test_graphics,
		REQUIRE_VIDEO|REQUIRE_OPTIONS);
	tests["argparser"]=make_pair(test_argparser,0);
	tests["UI"]=make_pair(test_ui,
		REQUIRE_VIDEO|REQUIRE_AUDIO|REQUIRE_OPTIONS|REQUIRE_FONTS);
	tests["font"]=make_pair(test_font,
		REQUIRE_VIDEO|REQUIRE_OPTIONS|REQUIRE_FONTS);

}

/**\brief Runs the Test.*/
int Test::RunTest( int argc, char **argv){
	// Check to see if we just want a list of the tests
	if ( this->testname == "list-tests"){
		map<string,pair<testFunc,long> >::iterator it;
		cout<<"Test available-----------------------------------"<<endl;
		for ( it=this->tests.begin(); it != this->tests.end(); it++ ){
			cout<<"\t"<<(*it).first<<endl;
		}
		cout<<"-------------------------------------------------"<<endl;
		return 0;
	}
	// Runs the test
	if( tests.count(testname) > 0 ) {
		LoadRequirements();
		cout<<"Running test: "<<this->testname<<"..."<<endl;
		int retval = tests[this->testname].first(argc, argv);
		UnloadRequirements();
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

/**\brief Loads requirements for the Test.*/
void Test::LoadRequirements( void ){
	long testreqs = tests[this->testname].second;
	if( testreqs & REQUIRE_OPTIONS ){
		cout<<"  Initializing options subsystem..."<<endl;
		optionsfile = new XMLFile( "Resources/Definitions/options.xml" );
	}
	if( testreqs & REQUIRE_VIDEO ){
		cout<<"  Initializing video subsystem..."<<endl;
		Video::Initialize();
		Video::SetWindow( 640, 480, 32 );
	}
	if( testreqs & REQUIRE_AUDIO ){
		cout<<"  Initializing audio subsystem..."<<endl;
		Audio::Instance().Initialize();
		Audio::Instance().SetMusicVol ( 0.5f );
		Audio::Instance().SetSoundVol ( 0.5f );
	}
	if( testreqs & REQUIRE_FONTS ){
		cout<<"  Initializing font subsystem..."<<endl;
		SansSerif       = new Font( "Resources/Fonts/FreeSans.ttf" );
		BitType         = new Font( "Resources/Fonts/visitor2.ttf" );
		Serif           = new Font( "Resources/Fonts/FreeSerif.ttf" );
		Mono            = new Font( "Resources/Fonts/ConsolaMono.ttf" );
	}
}

void Test::UnloadRequirements( void ){
	long testreqs = tests[this->testname].second;
	if( testreqs & REQUIRE_FONTS ){
		cout<<"  Shutting down font subsystem..."<<endl;		delete SansSerif;
		delete BitType;
		delete Serif;
		delete Mono;
	}
	if( testreqs & REQUIRE_AUDIO ){
		cout<<"  Shutting down audio subsystem..."<<endl;
		Audio::Instance().Shutdown();
	}
	if( testreqs & REQUIRE_VIDEO ){
		cout<<"  Shutting down video subsystem..."<<endl;
		Video::Shutdown();
	}
	if( testreqs & REQUIRE_OPTIONS ){
		cout<<"  Shutting down options subsystem..."<<endl;
		delete optionsfile;
	}
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

		Video::Erase();
		Video::Update();
		UI::Draw();
		Timer::Delay();
	}
}
