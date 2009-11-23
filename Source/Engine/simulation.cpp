/**\filename		simulation.cpp
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: July 2006
 * \date			Modified: Tuesday, June 23, 2009
 * \brief			Contains the main game loop
 * \details
 */

#include "includes.h"
#include "common.h"
#include "Engine/hud.h"
#include "Engine/simulation.h"
#include "Engine/alliances.h"
#include "Engine/starfield.h"
#include "Graphics/video.h"
#include "Input/input.h"
#include "Sprites/player.h"
#include "Sprites/spritemanager.h"
#include "UI/ui.h"
#include "Utilities/camera.h"
#include "Utilities/file.h"
#include "Utilities/log.h"
#include "Utilities/timer.h"
#include "Utilities/lua.h"
#include "AI/ai.h"

/**\class Simulation
 * \brief Handles main game loop. */

float Simulation::currentFPS = 0.;
bool Simulation::paused = false;

Simulation::Simulation( void ) {
	engines = Engines::Instance();
	planets = Planets::Instance();
	models = Models::Instance();
	alliances = Alliances::Instance();
	currentFPS = 0.;
}

Simulation::Simulation( string filename ) {
	engines = Engines::Instance();
	planets = Planets::Instance();
	models = Models::Instance();
	alliances = Alliances::Instance();
	currentFPS = 0.;

	this->filename = filename;
	
	Parse();
}

bool Simulation::Load( string filename ) {
	this->filename = filename;
	
	return Parse();
}

void Simulation::pause(){
	paused = true;
}
void Simulation::unpause(){
	paused = false;
}

bool Simulation::Run( void ) {
	bool quit = false;
	Input inputs;
	int fpsCount = 0; // for FPS calculations
	int fpsTotal= 0; // for FPS calculations
	Uint32 fpsTS = 0; // timestamp of last FPS printing

	// Grab the camera and give it coordinates
	Camera *camera = Camera::Instance();
	camera->Focus(0, 0);

	// Generate a starfield
	Starfield starfield( OPTION(int, "options/simulation/starfield-density") );

	// Create a spritelist
	SpriteManager sprites;

	Player *player = Player::Instance();

	// Set player model based on simulation xml file settings
	player->SetModel( models->GetModel( playerDefaultModel ) );
	sprites.Add( player->GetSprite() );

	// Focus the camera on the sprite
	camera->Focus( player->GetSprite() );

	// Add the planets
	planets->RegisterAll( &sprites );

	// Start the Lua Universe
	Lua::SetSpriteList( &sprites );
	Lua::Load("Resources/Scripts/universe.lua");

	// Start the Lua Scenarios
	Lua::Run("Start()");

	// Ensure correct drawing order
	sprites.Order();
	
	// Create the hud
	Hud::Hud();

	Hud::Alert( "Captain, we don't have the power! Pow = %d", 3 );

	fpsTS = Timer::GetTicks();
	// main game loop
	while( !quit ) {
		quit = inputs.Update();
		
		if( !paused ) {
			Lua::Update();
			// Update cycle
			starfield.Update();
			camera->Update();
			sprites.Update();
			camera->Update();
			Hud::Update();
			UI::Run(); // runs only a few loops
			
			// Keep this last (I think)
			Timer::Update();
		}

		// Erase cycle
		Video::Erase();
		
		// Draw cycle
		starfield.Draw();
		sprites.Draw();
		Hud::Draw( sprites );
		UI::Draw();
		Video::Update();
		
		// Don't kill the CPU (play nice)
		Timer::Delay();
		
		Coordinate playerPos = player->GetWorldPosition();

		// Counting Frames
		fpsCount++;
		fpsTotal++;

		// Update the fps once per second
		if( (Timer::GetTicks() - fpsTS) >1000 ) { 
			Simulation::currentFPS = static_cast<float>(1000.0 *
					((float)fpsCount / (Timer::GetTicks() - fpsTS)));
			fpsTS = Timer::GetTicks();
			fpsCount = 0;
		}
	}

	Log::Message("Average Framerate: %f Frames/Second", 1000.0 *((float)fpsTotal / Timer::GetTicks() ) );
	return true;
}

float Simulation::GetFPS() {
	return Simulation::currentFPS;
}

bool Simulation::Parse( void ) {
	xmlDocPtr doc;
	xmlNodePtr cur;
	int versionMajor = 0, versionMinor = 0, versionMacro = 0;

	File xmlfile = File( filename.c_str() );
	long filelen = xmlfile.GetLength();
	char *buffer = xmlfile.Read();
	doc = xmlParseMemory( buffer, static_cast<int>(filelen) );
	delete [] buffer;

	if( doc == NULL ) {
		Log::Warning( "Could not load '%s' simulation file.", filename.c_str() );
		return false;
	}

	cur = xmlDocGetRootElement( doc );

	if( cur == NULL ) {
		Log::Warning( "'%s' file appears to be empty.", filename.c_str() );
		xmlFreeDoc( doc );
		return false;
	}
	
	if( xmlStrcmp( cur->name, (const xmlChar *)"simulation" ) ) {
		Log::Warning( "'%s' appears to be invalid. Root element was %s.", filename.c_str(), (char *)cur->name );
		xmlFreeDoc( doc );
		return false;
	} else {
		Log::Message( "'%s' file found and valid, parsing...", filename.c_str() );
	}
	
	cur = cur->xmlChildrenNode;
	while( cur != NULL ) {
		// Parse for the version information and any children nodes
		if( ( !xmlStrcmp( cur->name, (const xmlChar *)"version-major" ) ) ) {
			xmlChar *key = xmlNodeListGetString( doc, cur->xmlChildrenNode, 1 );
			versionMajor = atoi( (char *)key );
			xmlFree( key );
		} else if( ( !xmlStrcmp( cur->name, (const xmlChar *)"version-minor" ) ) ) {
			xmlChar *key = xmlNodeListGetString( doc, cur->xmlChildrenNode, 1 );
			versionMinor = atoi( (char *)key );
			xmlFree( key );
		} else if( ( !xmlStrcmp( cur->name, (const xmlChar *)"version-macro" ) ) ) {
			xmlChar *key = xmlNodeListGetString( doc, cur->xmlChildrenNode, 1 );
			versionMacro = atoi( (char *)key );
			xmlFree( key );
		} else {
			char *sectionName = (char *)cur->name;

			if( !strcmp( sectionName, "planets" ) ) {
				xmlChar *key = xmlNodeListGetString( doc, cur->xmlChildrenNode, 1 );
				planetsFilename = (char *)key;
				xmlFree( key );
				Log::Message( "Planets filename is %s.", planetsFilename.c_str() );
			}
			if( !strcmp( sectionName, "models" ) ) {
				xmlChar *key = xmlNodeListGetString( doc, cur->xmlChildrenNode, 1 );
				modelsFilename = (char *)key;
				xmlFree( key );
				Log::Message( "Models filename is %s.", modelsFilename.c_str() );
			}
			if( !strcmp( sectionName, "engines" ) ) {
				xmlChar *key = xmlNodeListGetString( doc, cur->xmlChildrenNode, 1 );
				enginesFilename = (char *)key;
				xmlFree( key );
				Log::Message( "Engines filename is %s.", enginesFilename.c_str() );
			}
			if( !strcmp( sectionName, "alliances" ) ) {
				xmlChar *key = xmlNodeListGetString( doc, cur->xmlChildrenNode, 1 );
				alliancesFilename = (char *)key;
				xmlFree( key );
				Log::Message( "Alliances filename is %s.", alliancesFilename.c_str() );
			}
			if( !strcmp( sectionName, "playerDefaultModel" ) ) {
				xmlChar *key = xmlNodeListGetString( doc, cur->xmlChildrenNode, 1 );
				playerDefaultModel = (char *)key;
				xmlFree( key );
				Log::Message( "playerDefaultModel is %s.", playerDefaultModel.c_str() );
			}
		}
		
		cur = cur->next;
	}
	
	xmlFreeDoc( doc );
	
	Log::Message( "'%s' parsing done. File is version %d.%d.%d.", filename.c_str(), versionMajor, versionMinor, versionMacro );

	// Now load the various subsystems
	if( planets->Load( planetsFilename ) != true ) {
		Log::Warning( "There was an error loading the planets from '%s'.", planetsFilename.c_str() );
	}
	if( engines->Load( enginesFilename ) != true ) {
		Log::Error( "There was an error loading the engines from '%s'.", enginesFilename.c_str() );
	}
	if( models->Load( modelsFilename ) != true ) {
		Log::Error( "There was an error loading the models from '%s'.", modelsFilename.c_str() );
	}
	if( alliances->Load( alliancesFilename ) != true ) {
		Log::Error( "There was an error loading the alliances from '%s'.", alliancesFilename.c_str() );
	}

	return true;
}

