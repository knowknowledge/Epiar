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
#include "Audio/audio_lua.h"
#include "Engine/hud.h"
#include "Engine/simulation.h"
#include "Engine/simulation_lua.h"
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
#include "AI/ai_lua.h"

/**\class Simulation
 * \brief Handles main game loop. */

/**\brief Loads an empty Simulation.
 */
Simulation::Simulation( void ) {
	sprites = SpriteManager::Instance();
	commodities = Commodities::Instance();
	engines = Engines::Instance();
	planets = Planets::Instance();
	gates = Gates::Instance();
	models = Models::Instance();
	weapons = Weapons::Instance();
	alliances = Alliances::Instance();
	technologies = Technologies::Instance();
	outfits = Outfits::Instance();
	players = Players::Instance();
	camera = Camera::Instance();

	folderpath = "";
	currentFPS = 0.;
	paused = false;
	willsave = false;
}

/**\brief Loads the XML file.
 * \param filename Name of the file
 * \return true if success
 */
bool Simulation::Load( string _folderpath ) {
	folderpath = _folderpath + string("/");
	if( !Open( folderpath + string("simulation.xml") ) ) {
		return false;
	}
	return Parse();
}

/**\brief Pauses the simulation
 */
void Simulation::pause(){
	paused = true;
}

void Simulation::save(){
	willsave=true;
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
	bool luaLoad = true;
	int fpsCount = 0; // for FPS calculations
	int fpsTotal= 0; // for FPS calculations
	Uint32 fpsTS = 0; // timestamp of last FPS printing
	lua_State *L;

	Timer::Update(); // Start the Timer
	
	Hud::Init();

	// Generate a starfield
	Starfield starfield( OPTION(int, "options/simulation/starfield-density") );


	// Start the Lua Universe
	// Register these functions to their own lua namespaces
	Lua::Init();
	L = Lua::CurrentState();

	Simulation_Lua::StoreSimulation(L,this);

	Simulation_Lua::RegisterSimulation(L);
	AI_Lua::RegisterAI(L);
	UI_Lua::RegisterUI(L);
	Audio_Lua::RegisterAudio(L);
	Planets_Lua::RegisterPlanets(L);
	Hud::RegisterHud(L);

	luaLoad = Lua::Load("Resources/Scripts/utilities.lua")
		   && Lua::Load("Resources/Scripts/universe.lua")
	       && Lua::Load("Resources/Scripts/commands.lua")
	       && Lua::Load("Resources/Scripts/ai.lua")
	       && Lua::Load("Resources/Scripts/missions.lua")
	       && Lua::Load("Resources/Scripts/player.lua");
	if (!luaLoad) {
		LogMsg(ERR,"Fatal error starting Lua.");
		return false;
	}

	if( OPTION(int, "options/simulation/random-universe") ) {
		if( OPTION(int, "options/simulation/random-seed") ) {
			Lua::Call("createSystems", "i", OPTION(int, "options/simulation/random-seed") );
		} else {
			Lua::Call("createSystems");
		}
	} else {
	    list<string>* planetNames = planets->GetNames();
	    for( list<string>::iterator pname = planetNames->begin(); pname != planetNames->end(); ++pname){
		    sprites->Add(  planets->GetPlanet(*pname) );
	    }

	    list<string>* gateNames = gates->GetNames();
	    for( list<string>::iterator gname = gateNames->begin(); gname != gateNames->end(); ++gname){
		    sprites->Add(  gates->GetGate(*gname) );
	    }
	}

	// Randomize the Lua Seed
	Lua::Call("randomizeseed");

	// Message appear in reverse order, so this is upside down
	Hud::Alert("Epiar is currently under development. Please report all bugs to epiar.net");

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
	bool lowFps = false;
	int lowFpsFrameCount = 0;
	while( !quit ) {
		quit = HandleInput();
//_ASSERTE(_CrtCheckMemory());
		//logicLoops is the number of times we need to run logical updates to get 50 logical updates per second
		//if the draw fps is >50 then logicLoops will always be 1 (ie 1 logical update per draw)
		int logicLoops = Timer::Update();
		bool anyUpdate = (logicLoops>0);
		if( !paused ) {
			while(logicLoops--) {
				if (lowFps)
					lowFpsFrameCount --;
				Timer::IncrementFrameCount();
				// Update cycle
				sprites->Update( lowFps );
			}
		}

		// These only need to be updated once pre Draw cycle, but they can be skipped if there are no Sprite update cycles.
		if( anyUpdate ) {
			starfield.Update( camera );
			camera->Update( sprites );
			Hud::Update();
		}

		// Erase cycle
		Video::Erase();

		// Draw cycle
		starfield.Draw();
		sprites->Draw();
		Hud::Draw( HUD_ALL, currentFPS );
		UI::Draw();
		console.Draw();
		Video::Update();

		// Don't kill the CPU (play nice)
		if( paused ) {
			Timer::Delay(50);
		} else {
			Timer::Delay(10);
		}

		// Counting Frames
		fpsCount++;
		fpsTotal++;

		// Update the fps once per second
		if( (Timer::GetTicks() - fpsTS) >1000 ) {
			currentFPS = static_cast<float>(1000.0 *
					((float)fpsCount / (Timer::GetTicks() - fpsTS)));
			fpsTS = Timer::GetTicks();
			fpsCount = 0;
			if( currentFPS < -0.1f )
			{
				// The game has effectively stopped..
				LogMsg(ERR,"Sorry, the framerate has dropped to zero. Please report this as a bug to 'epiar-devel@epiar.net'");
				UI::Save();
				sprites->Save();
				quit = true;
			}


				/**************************
				 * Low FPS calculation
				 *  - if fps goes below 15, set lowFps to true for 600 logical frames
				 *  - after 600 frames, either turn it off or leave it on for another 600
				 **************************/
			if (lowFps)
			{
				if (lowFpsFrameCount <= 0)
				{
					LogMsg (DEBUG4, "Turning off wave-updates for sprites as 600 frames have passed");
					lowFps = false;
				}
			}

			if (!lowFps && currentFPS < 15)
			{
				LogMsg (DEBUG4, "Turning on wave-updates for sprites as FPS has gone below 15");
				lowFps = true;			//if FPS has dropped below 15 then switch to wave-update method for 600 frames
				lowFpsFrameCount = 600;
			}
				/************************
				 * End Low FPS calculation
				 ************************/

			if( OPTION(int, "options/log/ui") )
			{
				UI::Save();
			}

			if( OPTION(int, "options/log/sprites") )
			{
				sprites->Save();
			}
		}
		if(willsave){
			Players::Instance()->Save();
			willsave=false;
		}
	}
	optionsfile->Save();
	
	Hud::Close();

	LogMsg(INFO,"Average Framerate: %f Frames/Second", 1000.0 *((float)fpsTotal / Timer::GetTicks() ) );
	return true;
}

bool Simulation::Edit() {
	bool quit = false;
	bool luaLoad = true;
	lua_State *L;

	// Generate a starfield
	Starfield starfield( OPTION(int, "options/simulation/starfield-density") );

	// Start the Lua Universe
	// Register these functions to their own lua namespaces
	Lua::Init();
	L = Lua::CurrentState();

	Simulation_Lua::StoreSimulation(L,this);

	Simulation_Lua::RegisterSimulation(L);
	Simulation_Lua::RegisterEditor(L);
	UI_Lua::RegisterUI(L);
	Audio_Lua::RegisterAudio(L);
	Planets_Lua::RegisterPlanets(L);
	Hud::RegisterHud(L);

	luaLoad = Lua::Load("Resources/Scripts/utilities.lua")
		   && Lua::Load("Resources/Scripts/universe.lua")
	       && Lua::Load("Resources/Scripts/commands.lua")
	       && Lua::Load("Resources/Scripts/editor.lua");

	if (!luaLoad) {
		LogMsg(ERR,"Fatal error starting Lua.");
		return false;
	}

	if( OPTION(int, "options/simulation/random-universe") ) {
		Lua::Call("createSystems");
	} else {
	    list<string>* planetNames = planets->GetNames();
	    for( list<string>::iterator pname = planetNames->begin(); pname != planetNames->end(); ++pname){
		    sprites->Add(  planets->GetPlanet(*pname) );
	    }

	    list<string>* gateNames = gates->GetNames();
	    for( list<string>::iterator gname = gateNames->begin(); gname != gateNames->end(); ++gname){
		    sprites->Add(  gates->GetGate(*gname) );
	    }
	}

	while( !quit ) {
		quit = HandleInput();

		Timer::Update();
		starfield.Update( camera );
		camera->Update( sprites );
		Hud::Update();

		// Erase cycle
		Video::Erase();

		// Draw cycle
		starfield.Draw();
		sprites->Draw();
		UI::Draw();
		Hud::Draw( HUD_Target | HUD_Map, 0.0f );
		console.Draw();
		Video::Update();

		// Don't kill the CPU (play nice)
		Timer::Delay( 50 );
	}

	return true;
}

/**\brief Parses an XML simulation file
 * \return true if successful
 */
bool Simulation::Parse( void ) {
	LogMsg(INFO, "Simulation version %s.%s.%s.", Get("version-major").c_str(), Get("version-minor").c_str(),  Get("version-macro").c_str());

	// Now load the various subsystems
	if( commodities->Load( (folderpath + Get("commodities")) ) != true ) {
		LogMsg(ERR, "There was an error loading the commodities from '%s'.", (folderpath + Get("commodities")).c_str() );
		return false;
	}
	if( engines->Load( (folderpath + Get("engines")) ) != true ) {
		LogMsg(ERR, "There was an error loading the engines from '%s'.", (folderpath + Get("engines")).c_str() );
		return false;
	}
	if( models->Load( (folderpath + Get("models")) ) != true ) {
		LogMsg(ERR, "There was an error loading the models from '%s'.", (folderpath + Get("models")).c_str() );
		return false;
	}
	if( weapons->Load( (folderpath + Get("weapons")) ) != true ) {
		LogMsg(ERR, "There was an error loading the technologies from '%s'.", (folderpath + Get("weapons")).c_str() );
		return false;
	}
	if( outfits->Load( (folderpath + Get("outfits")) ) != true ) {
		LogMsg(ERR, "There was an error loading the outfits from '%s'.", (folderpath + Get("outfits")).c_str() );
		return false;
	}
	if( technologies->Load( (folderpath + Get("technologies")) ) != true ) {
		LogMsg(ERR, "There was an error loading the technologies from '%s'.", (folderpath + Get("technologies")).c_str() );
		return false;
	}
	if( alliances->Load( (folderpath + Get("alliances")) ) != true ) {
		LogMsg(ERR, "There was an error loading the alliances from '%s'.", (folderpath + Get("alliances")).c_str() );
		return false;
	}
	if( 0 == OPTION(int, "options/simulation/random-universe")) {
		if( planets->Load( (folderpath + Get("planets")) ) != true ) {
		    LogMsg(WARN, "There was an error loading the planets from '%s'.", (folderpath + Get("planets")).c_str() );
		    return false;
	    }
		if( gates->Load( (folderpath + Get("gates")) ) != true ) {
		    LogMsg(WARN, "There was an error loading the gates from '%s'.", (folderpath + Get("gates")).c_str() );
		    return false;
	    }
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
	bool quitSignal = false;

	// Collect user input events
	events = inputs.Update( quitSignal );

	// Pass the Events to the systems that handle them.
	UI::HandleInput( &events );
	console.HandleInput( events );
	Hud::HandleInput( events );

	inputs.HandleLuaCallBacks( events );

	return quitSignal;
}

/**\fn Simulation::isPaused()
 * \brief Checks to see if Simulation is paused
 */
