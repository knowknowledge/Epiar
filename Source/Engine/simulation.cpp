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
#include "Engine/console.h"
#include "Graphics/video.h"
#include "Sprites/player.h"
#include "Sprites/gate.h"
#include "Sprites/spritemanager.h"
#include "UI/ui.h"
#include "Utilities/file.h"
#include "Utilities/log.h"
#include "Utilities/timer.h"
#include "Utilities/lua.h"
#include "AI/ai.h"

/**\class Simulation
 * \brief Handles main game loop. */

bool Simulation::paused = false;

/**\brief Loads an empty Simulation.
 */
Simulation::Simulation( void ) {
	sprites = SpriteManager::Instance();
	commodities = Commodities::Instance();
	engines = Engines::Instance();
	planets = Planets::Instance();
	models = Models::Instance();
	weapons = Weapons::Instance();
	alliances = Alliances::Instance();
	technologies = Technologies::Instance();
	outfits = Outfits::Instance();
	players = Players::Instance();
	camera = Camera::Instance();
	currentFPS = 0.;
}

/**\brief Loads the XML file.
 * \param filename Name of the file
 * \return true if success
 */
bool Simulation::Load( string filename ) {
	if( !Open(filename) ) {
		return false;
	}
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
	int fpsCount = 0; // for FPS calculations
	int fpsTotal= 0; // for FPS calculations
	Uint32 fpsTS = 0; // timestamp of last FPS printing

	Timer::Update(); // Start the Timer
	Timer::Initialize();

	// Generate a starfield
	Starfield starfield( OPTION(int, "options/simulation/starfield-density") );

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

	// Message appear in reverse order, so this is upside down
	Hud::Alert("-----------------------------------");
	Hud::Alert("Please Report all bugs to epiar.net");
	Hud::Alert("Epiar is currently under development.");

	// Load the player
	if( OPTION(int,"options/simulation/automatic-load") ) {
		if( players->LoadLast()!=NULL ) {
			Hud::Alert("Loading %s.", Player::Instance()->GetName().c_str() );
			Lua::Call("playerStart");
		}
	}
	if( !Player::IsLoaded() ) {
		Lua::Call("loadingWindow");
	}

	fpsTS = Timer::GetTicks();

	// Load sample game music
	if(bgmusic && OPTION(int, "options/sound/background"))
		bgmusic->Play();

	// main game loop
	while( !quit ) {
		quit = HandleInput();
		
		int logicLoops = Timer::Update();
		if( !paused ) {
			while(logicLoops--) {
				Lua::Call("Update");
				// Update cycle
				starfield.Update( camera );
				sprites->Update();
				camera->Update( sprites );
				Hud::Update();
			}
		}

		// Erase cycle
		Video::Erase();
		
		// Draw cycle
		starfield.Draw();
		sprites->Draw();
		Hud::Draw( currentFPS );
		UI::Draw();
		console.Draw();
		Video::Update();
		
		// Don't kill the CPU (play nice)
		Timer::Delay();
		
		// Counting Frames
		fpsCount++;
		fpsTotal++;

		// Update the fps once per second
		if( (Timer::GetTicks() - fpsTS) >1000 ) {
			currentFPS = static_cast<float>(1000.0 *
					((float)fpsCount / (Timer::GetTicks() - fpsTS)));
			fpsTS = Timer::GetTicks();
			fpsCount = 0;
			if( currentFPS < 0.1f )
			{
				// The game has effectively stopped..
				LogMsg(ERR,"Sorry, the framerate has dropped to zero. Please report this as a bug to 'epiar-devel@epiar.net'");
				UI::Save();
				sprites->Save();
				quit = true;
			}

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

	Players::Instance()->Save(Get("players"));
	optionsfile->Save();

	LogMsg(INFO,"Average Framerate: %f Frames/Second", 1000.0 *((float)fpsTotal / Timer::GetTicks() ) );
	return true;
}

/**\brief Parses an XML simulation file
 * \return true if successful
 */
bool Simulation::Parse( void ) {
	LogMsg(INFO, "Simulation version %d.%d.%d.", Get("version-major").c_str(), Get("version-minor").c_str(),  Get("version-macro").c_str());

	// Now load the various subsystems
	if( commodities->Load( Get("commodities") ) != true ) {
		LogMsg(ERR, "There was an error loading the commodities from '%s'.", Get("commodities").c_str() );
		return false;
	}
	if( engines->Load( Get("engines") ) != true ) {
		LogMsg(ERR, "There was an error loading the engines from '%s'.", Get("engines").c_str() );
		return false;
	}
	if( models->Load( Get("models") ) != true ) {
		LogMsg(ERR, "There was an error loading the models from '%s'.", Get("models").c_str() );
		return false;
	}
	if( weapons->Load( Get("weapons") ) != true ) {
		LogMsg(ERR, "There was an error loading the technologies from '%s'.", Get("weapons").c_str() );
		return false;
	}
	if( outfits->Load( Get("outfits") ) != true ) {
		LogMsg(ERR, "There was an error loading the outfits from '%s'.", Get("outfits").c_str() );
		return false;
	}
	if( technologies->Load( Get("technologies") ) != true ) {
		LogMsg(ERR, "There was an error loading the technologies from '%s'.", Get("technologies").c_str() );
		return false;
	}
	if( alliances->Load( Get("alliances") ) != true ) {
		LogMsg(ERR, "There was an error loading the alliances from '%s'.", Get("alliances").c_str() );
		return false;
	}
	if( planets->Load( Get("planets") ) != true ) {
		LogMsg(WARN, "There was an error loading the planets from '%s'.", Get("planets").c_str() );
		return false;
	}
	if( players->Load( Get("players"), true ) != true ) {
		LogMsg(WARN, "There was an error loading the players from '%s'.", Get("players").c_str() );
		return false;
	}
	
	bgmusic = Song::Get( Get("music") );
	if( bgmusic == NULL ) {
		LogMsg(WARN, "There was an error loading music from '%s'.", Get("music").c_str() );
	}

	Coordinate startPos(0,0);
	string startPlanet = Get("defaultPlayer/start");
	if( planets->GetPlanet( startPlanet ) ) {
		startPos = planets->GetPlanet( startPlanet )->GetWorldPosition();
	} else {
		LogMsg(WARN, "Invalid default player: no planet named '%s'.", startPlanet.c_str() );
	}
	players->SetDefaults(
		models->GetModel( Get("defaultPlayer/model") ),
		engines->GetEngine( Get("defaultPlayer/engine") ),
		convertTo<int>( Get("defaultPlayer/credits")),
		startPos
	);

	return true;
}

/**\brief Handle User Input
 * \return true if the player wants to quit
 */
bool Simulation::HandleInput() {
	list<InputEvent> events;
	bool quitSignal;

	// Collect user input events
	events = inputs.Update( quitSignal );

	// Pass the Events to the systems that handle them.
	UI::HandleInput( events );
	console.HandleInput( events );
	Hud::HandleInput( events );

	return quitSignal;
}

/**\fn Simulation::isPaused()
 * \brief Checks to see if Simulation is paused
 */
