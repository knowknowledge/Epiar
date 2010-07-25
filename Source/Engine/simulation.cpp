/**\file			simulation.cpp
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: July 2006
 * \date			Modified: Tuesday, June 23, 2009
 * \brief			Contains the main game loop
 * \details
 */

#include "includes.h"
#include "common.h"
#include "Audio/music.h"
#include "Engine/hud.h"
#include "Engine/simulation.h"
#include "Engine/commodities.h"
#include "Engine/alliances.h"
#include "Engine/technologies.h"
#include "Engine/starfield.h"
#include "Graphics/video.h"
#include "Input/input.h"
#include "Sprites/player.h"
#include "Sprites/gate.h"
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

/**\brief Loads an empty Simulation.
 */
Simulation::Simulation( void ) {
	commodities = Commodities::Instance();
	engines = Engines::Instance();
	planets = Planets::Instance();
	models = Models::Instance();
	weapons = Weapons::Instance();
	alliances = Alliances::Instance();
	technologies = Technologies::Instance();
	outfits = Outfits::Instance();
	players = Players::Instance();
	currentFPS = 0.;
}

/**\brief Loads a simulation based on the XML file.
 */
Simulation::Simulation( string filename ) {
	commodities = Commodities::Instance();
	engines = Engines::Instance();
	planets = Planets::Instance();
	models = Models::Instance();
	weapons = Weapons::Instance();
	alliances = Alliances::Instance();
	technologies = Technologies::Instance();
	outfits = Outfits::Instance();
	players = Players::Instance();
	currentFPS = 0.;

	this->filename = filename;
	
	Parse();
}

/**\brief Loads the XML file.
 * \param filename Name of the file
 * \return true if success
 */
bool Simulation::Load( string filename ) {
	this->filename = filename;
	
	return Parse();
}

/**\brief Pauses the simulation
 */
void Simulation::pause(){
	paused = true;
}

/**\brief Unpauses the simulation
 */
void Simulation::unpause(){
	paused = false;
}

/**\brief Main game loop
 * \return true
 */
bool Simulation::Run() {
	bool quit = false;
	Input inputs;
	int fpsCount = 0; // for FPS calculations
	int fpsTotal= 0; // for FPS calculations
	Uint32 fpsTS = 0; // timestamp of last FPS printing

	Timer::Update(); // Start the Timer

	// Grab the camera and give it coordinates
	Camera *camera = Camera::Instance();
	camera->Focus(0, 0);
	
	Timer::Initialize();

	// Generate a starfield
	Starfield starfield( OPTION(int, "options/simulation/starfield-density") );

	// Create a spritelist
	SpriteManager *sprites = SpriteManager::Instance();

	Planets *planets = Planets::Instance();
	list<string>* planetNames = planets->GetNames();
	for( list<string>::iterator pname = planetNames->begin(); pname != planetNames->end(); ++pname){
		sprites->Add(  planets->GetPlanet(*pname) );
	}

	// Start the Lua Universe
	if( !( Lua::Load("Resources/Scripts/universe.lua") ))
	{
		LogMsg(ERR,"Fatal error starting Lua.");
		quit = true;
	}
    if( 0 == OPTION(int,"options/development/editor-mode") ){
        if( !( Lua::Load("Resources/Scripts/player.lua") ))
        {
            LogMsg(ERR,"Fatal error starting Lua.");
            quit = true;
        }
    } else {
        if( !( Lua::Load("Resources/Scripts/editor.lua") ))
        {
            LogMsg(ERR,"Fatal error starting Lua.");
            quit = true;
        }
    }
    Lua::Call("Start");

	// Create the hud
	Hud::Hud();

	// Message appear in reverse order, so this is upside down
	Hud::Alert("-----------------------------------");
	Hud::Alert("Please Report all bugs to epiar.net");
	Hud::Alert("Epiar is currently under development.");

	fpsTS = Timer::GetTicks();

	// Load sample game music
	Song* bgmusic = Song::Get( OPTION(string,"options/simulation/bgmusic") );
	if(OPTION(int, "options/sound/background"))
		bgmusic->Play();

	// main game loop
	while( !quit ) {
		quit = inputs.Update();
		
		int logicLoops = Timer::Update();
		if( !paused ) {
			while(logicLoops--) {
				Lua::Call("Update");
				// Update cycle
				starfield.Update();
				sprites->Update();
				camera->Update();
				Hud::Update();
			}
		}

		// Erase cycle
		Video::Erase();
		
		// Draw cycle
		starfield.Draw();
		sprites->Draw();
		Hud::Draw();
		UI::Draw();
		Video::Update();
		
		// Don't kill the CPU (play nice)
		Timer::Delay();
		
		// Counting Frames
		fpsCount++;
		fpsTotal++;

		// Update the fps once per second
		if( (Timer::GetTicks() - fpsTS) >1000 ) {
			Simulation::currentFPS = static_cast<float>(1000.0 *
					((float)fpsCount / (Timer::GetTicks() - fpsTS)));
			fpsTS = Timer::GetTicks();
			fpsCount = 0;

			if( OPTION(int, "options/log/ui") )
			{
				UI::Save();
			}

			if( OPTION(int, "options/log/sprites") )
			{
				sprites->Save();
			}
		}
	}

	Players::Instance()->Save(playersFilename);

	LogMsg(INFO,"Average Framerate: %f Frames/Second", 1000.0 *((float)fpsTotal / Timer::GetTicks() ) );
	return true;
}

/**\brief Returns the current frames per second
 */
float Simulation::GetFPS() {
	return Simulation::currentFPS;
}

/**\brief Parses an XML simulation file
 * \return true if successful
 */
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
		LogMsg(WARN, "Could not load '%s' simulation file.", filename.c_str() );
		return false;
	}

	cur = xmlDocGetRootElement( doc );

	if( cur == NULL ) {
		LogMsg(WARN, "'%s' file appears to be empty.", filename.c_str() );
		xmlFreeDoc( doc );
		return false;
	}
	
	if( xmlStrcmp( cur->name, (const xmlChar *)"simulation" ) ) {
		LogMsg(WARN, "'%s' appears to be invalid. Root element was %s.", filename.c_str(), (char *)cur->name );
		xmlFreeDoc( doc );
		return false;
	} else {
		LogMsg(INFO, "'%s' file found and valid, parsing...", filename.c_str() );
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

			if( !strcmp( sectionName, "commodities" ) ) {
				xmlChar *key = xmlNodeListGetString( doc, cur->xmlChildrenNode, 1 );
				commoditiesFilename = (char *)key;
				xmlFree( key );
				LogMsg(INFO, "Commodities filename is %s.", commoditiesFilename.c_str() );
			}
			if( !strcmp( sectionName, "planets" ) ) {
				xmlChar *key = xmlNodeListGetString( doc, cur->xmlChildrenNode, 1 );
				planetsFilename = (char *)key;
				xmlFree( key );
				LogMsg(INFO, "Planets filename is %s.", planetsFilename.c_str() );
			}
			if( !strcmp( sectionName, "models" ) ) {
				xmlChar *key = xmlNodeListGetString( doc, cur->xmlChildrenNode, 1 );
				modelsFilename = (char *)key;
				xmlFree( key );
				LogMsg(INFO, "Models filename is %s.", modelsFilename.c_str() );
			}
			if( !strcmp( sectionName, "engines" ) ) {
				xmlChar *key = xmlNodeListGetString( doc, cur->xmlChildrenNode, 1 );
				enginesFilename = (char *)key;
				xmlFree( key );
				LogMsg(INFO, "Engines filename is %s.", enginesFilename.c_str() );
			}
			if( !strcmp( sectionName, "weapons" ) ) {
				xmlChar *key = xmlNodeListGetString( doc, cur->xmlChildrenNode, 1 );
				weaponsFilename = (char *)key;
				xmlFree( key );
				LogMsg(INFO, "Weapons filename is %s.", weaponsFilename.c_str() );
			}
			if( !strcmp( sectionName, "outfits" ) ) {
				xmlChar *key = xmlNodeListGetString( doc, cur->xmlChildrenNode, 1 );
				outfitsFilename = (char *)key;
				xmlFree( key );
				LogMsg(INFO, "Outfits filename is %s.", outfitsFilename.c_str() );
			}
			if( !strcmp( sectionName, "alliances" ) ) {
				xmlChar *key = xmlNodeListGetString( doc, cur->xmlChildrenNode, 1 );
				alliancesFilename = (char *)key;
				xmlFree( key );
				LogMsg(INFO, "Alliances filename is %s.", alliancesFilename.c_str() );
			}
			if( !strcmp( sectionName, "technologies" ) ) {
				xmlChar *key = xmlNodeListGetString( doc, cur->xmlChildrenNode, 1 );
				technologiesFilename = (char *)key;
				xmlFree( key );
				LogMsg(INFO, "Technologies filename is %s.", technologiesFilename.c_str() );
			}
			if( !strcmp( sectionName, "players" ) ) {
				xmlChar *key = xmlNodeListGetString( doc, cur->xmlChildrenNode, 1 );
				playersFilename = (char *)key;
				xmlFree( key );
				LogMsg(INFO, "Players filename is %s.", playersFilename.c_str() );
			}
		}
		
		cur = cur->next;
	}
	
	xmlFreeDoc( doc );
	
	LogMsg(INFO, "'%s' parsing done. File is version %d.%d.%d.", filename.c_str(), versionMajor, versionMinor, versionMacro );

	// Now load the various subsystems
	if( commodities->Load( commoditiesFilename ) != true ) {
		LogMsg(ERR, "There was an error loading the commodities from '%s'.", commoditiesFilename.c_str() );
	}
	if( engines->Load( enginesFilename ) != true ) {
		LogMsg(ERR, "There was an error loading the engines from '%s'.", enginesFilename.c_str() );
	}
	if( models->Load( modelsFilename ) != true ) {
		LogMsg(ERR, "There was an error loading the models from '%s'.", modelsFilename.c_str() );
	}
	if( weapons->Load( weaponsFilename ) != true ) {
		LogMsg(ERR, "There was an error loading the technologies from '%s'.", weaponsFilename.c_str() );
	}
	if( outfits->Load( outfitsFilename ) != true ) {
		LogMsg(ERR, "There was an error loading the outfits from '%s'.", outfitsFilename.c_str() );
	}
	if( technologies->Load( technologiesFilename ) != true ) {
		LogMsg(ERR, "There was an error loading the technologies from '%s'.", technologiesFilename.c_str() );
	}
	if( alliances->Load( alliancesFilename ) != true ) {
		LogMsg(ERR, "There was an error loading the alliances from '%s'.", alliancesFilename.c_str() );
	}
	if( planets->Load( planetsFilename ) != true ) {
		LogMsg(WARN, "There was an error loading the planets from '%s'.", planetsFilename.c_str() );
	}
	if( players->Load( playersFilename ) != true ) {
		LogMsg(WARN, "There was an error loading the players from '%s'.", playersFilename.c_str() );
	}

	return true;
}

/**\fn Simulation::isPaused()
 * \brief Checks to see if Simulation is paused
 */
