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
#include "Sprites/ai.h"
#include "Sprites/ai_lua.h"
#include "Sprites/effects.h"
#include "Sprites/player.h"
#include "Sprites/planets.h"
#include "Sprites/planets_lua.h"
#include "Sprites/gate.h"
#include "Sprites/spritemanager.h"
#include "UI/ui.h"
#include "UI/widgets.h"
#include "Utilities/file.h"
#include "Utilities/log.h"
#include "Utilities/timer.h"
#include "Utilities/lua.h"

/**\class Simulation
 * \brief Handles main game loop. */

/**\brief Loads an empty Simulation.
 */
Simulation::Simulation( void ) {
	// Start the Lua Universe
	// Register these functions to their own lua namespaces
	Lua::Init();
	L = Lua::CurrentState();
	Simulation_Lua::StoreSimulation(L,this);

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
	player = NULL;

	camera = Camera::Instance();
  calendar = new Calendar();
	console = new Console( L );

	folderpath = "";

	currentFPS = 0.0f;
	paused = false;
	loaded = false;
	quit = false;

	mapScale = -1.0f;
}

bool Simulation::New( string newname ) {
	LogMsg(INFO, "New Simulation: '%s'.", newname.c_str() );

	folderpath = "Resources/Simulation/" + newname + "/";

	XMLFile::New( folderpath + string("simulation.xml"), "simulation" );

	Set("simulation/name", newname );
	Set("simulation/description", "" );

	// Set the File Names
	commodities->SetFileName( folderpath + "commodities.xml" );
	engines->SetFileName( folderpath + "engines.xml" );
	planets->SetFileName( folderpath + "planets.xml" );
	gates->SetFileName( folderpath + "gates.xml" );
	models->SetFileName( folderpath + "models.xml" );
	weapons->SetFileName( folderpath + "weapons.xml" );
	alliances->SetFileName( folderpath + "alliances.xml" );
	technologies->SetFileName( folderpath + "technologies.xml" );
	outfits->SetFileName( folderpath + "outfits.xml" );

	Set("simulation/commodities", "commodities.xml" );
	Set("simulation/engines", "engines.xml" );
	Set("simulation/planets", "planets.xml" );
	Set("simulation/gates", "gates.xml" );
	Set("simulation/models", "models.xml" );
	Set("simulation/weapons", "weapons.xml" );
	Set("simulation/alliances", "alliances.xml" );
	Set("simulation/technologies", "technologies.xml" );
	Set("simulation/outfits", "outfits.xml" );

	Set("defaultPlayer/start", "");
	Set("defaultPlayer/model", "");
	Set("defaultPlayer/engine", "");
	Set("defaultPlayer/credits", 0);

	Set("simulation/players", "Resources/Definitions/saved-games.xml" );

	loaded = true;
	return true;
}

/**\brief Loads the XML file.
 * \param filename Name of the file
 * \return true if success
 */
bool Simulation::Load( string simName ) {
	folderpath = "Resources/Simulation/" + simName + "/";
	if( !Open( folderpath + string("simulation.xml") ) ) {
		return false;
	}
	loaded = Parse();
	return loaded;
}

/**\brief Pauses the simulation
 */
void Simulation::pause(){
	LogMsg(INFO, "Pausing.");
	paused = true;
}

void Simulation::Save(){
	if( PHYSFS_mkdir( ("Resources/Simulation/" + GetName() + "/").c_str() ) == 0) {
		LogMsg(ERR, "Cannot create folder '%s'.", folderpath.c_str() );
		return;
	}

	// Check Defaults
	if( planets->Get( Get("defaultPlayer/start")) == NULL)
		LogMsg(WARN, "Bad Default Player Start Location '%s'.", Get("defaultPlayer/start").c_str() );
	if( models->Get( Get("defaultPlayer/model")) == NULL)
		LogMsg(WARN, "Bad Default Player Start Model '%s'.", Get("defaultPlayer/model").c_str() );
	if( engines->Get( Get("defaultPlayer/engine")) == NULL)
		LogMsg(WARN, "Bad Default Player Start Engine '%s'.", Get("defaultPlayer/engine").c_str() );
	// TODO Somehow check that starting credits is actually an integer.

	XMLFile::Save();
	GetAlliances()->Save();
	GetCommodities()->Save();
	GetGates()->Save();
	GetModels()->Save();
	GetWeapons()->Save();
	GetEngines()->Save();
	GetPlanets()->Save();
	GetOutfits()->Save();
	GetTechnologies()->Save();
}

/**\brief Unpauses the simulation
 */
void Simulation::unpause(){
	LogMsg(INFO, "Unpausing.");
	Timer::Update();
	paused = false;
}

bool Simulation::SetupToRun(){
	bool luaLoad = true;

	LogMsg(INFO, "Simulation Setup Started");

	Timer::Update(); // Start the Timer

	// Load default Lua registers
	LuaRegisters(L);
	// Load ::Run()-specific Lua registers
	AI_Lua::RegisterAI(L);

	luaLoad = Lua::Load("Resources/Scripts/utilities.lua")
	       && Lua::Load("Resources/Scripts/universe.lua")
	       && Lua::Load("Resources/Scripts/landing.lua")
	       && Lua::Load("Resources/Scripts/commands.lua")
	       && Lua::Load("Resources/Scripts/ai.lua")
	       && Lua::Load("Resources/Scripts/missions.lua")
	       && Lua::Load("Resources/Scripts/player.lua")
	       && Lua::Load("Resources/Scripts/autopilot.lua")
	       && Lua::Load("Resources/Scripts/fleet.lua");

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
		// Add Planets
	    list<string>* planetNames = planets->GetNames();
	    for( list<string>::iterator pname = planetNames->begin(); pname != planetNames->end(); ++pname){
			Planet* p = planets->GetPlanet(*pname);
		    sprites->Add( p );

#ifdef ADD_ASTEROIDS
			// Ticket #44
			// TODO: These asteroids just wander to the edges of the universe, and only slow down the game when not visible.  Fix that.
			// Add Asteroids near planets
			unsigned int a;
			for( a = 0; a < 100; a++ ){
				Effect* asteroid = new Effect( p->GetWorldPosition() + GaussianCoordinate() * p->GetInfluence(), "Resources/Animations/asteroid.ani", 1.0 );
				asteroid->SetMomentum( GaussianCoordinate() *2 );
				asteroid->SetAngle( float( rand() %360 ) );
				sprites->Add( asteroid );
			}
#endif
	    }

		// Add Gates
	    list<string>* gateNames = gates->GetNames();
	    for( list<string>::iterator gname = gateNames->begin(); gname != gateNames->end(); ++gname){
		    sprites->Add(  gates->GetGate(*gname) );
	    }

		
	}

	// Preloading this animation prevents an FPS
	// drop the first time that a ship explodes.
	Ani::Get("Resources/Animations/explosion1.ani");

	// Randomize the Lua Seed
	Lua::Call("randomizeseed");

	LogMsg(INFO, "Simulation Setup Complete");

	return true;
}

/**\brief Callback for Death dialog UI
 * \return void
 */
void ConfirmDeath(void *simulationInstance) {
	((Simulation *)simulationInstance)->SetQuit(true);
}

void Pause(void *simulationInstance) {
	((Simulation *)simulationInstance)->pause();
}

void Unpause(void *simulationInstance) {
	((Simulation *)simulationInstance)->unpause();
}

void SaveMapScale( void *simulationInstance ) {
	Map* map = (Map*)UI::Search("/Window'Navigation'/Map/");
	if( map != NULL) {
		((Simulation *)simulationInstance)->SetMapScale( map->GetScale() );
	} else {
		LogMsg(WARN, "Could not saving scale because the Map could not be found.\n" );
	}
}

/**\brief Main game loop
 * \return true if the player is still alive
 */
bool Simulation::Run() {
	int fpsCount = 0; // for FPS calculations
	int fpsTotal= 0; // for FPS calculations
	Uint32 fpsTS = 0; // timestamp of last FPS printing
	fpsTS = Timer::GetTicks();

	quit = false;


	LogMsg(INFO, "Simulation Started");
	Hud::Init();

	if( player != NULL )
	{
		Hud::Alert("Loading %s.", player->GetName().c_str() );
		Lua::Call("playerStart");
	}
	else
	{
		LogMsg(WARN, "No Player has been loaded!");
		assert( player != NULL );
		quit = true;
	}

	// Message appear in reverse order, so this is upside down
	Hud::Alert("Epiar is currently under development. Please report all bugs to epiar.net");

	// Generate a starfield
	Starfield starfield( OPTION(int, "options/simulation/starfield-density") );

	// Load sample game music
	if(bgmusic && OPTION(int, "options/sound/background"))
		bgmusic->Play();

	// main game loop
	bool lowFps = false;
	int lowFpsFrameCount = 0;
	while( !quit ) {
		HandleInput();

		//_ASSERTE(_CrtCheckMemory());

		//logicLoops is the number of times we need to run logical updates to get 50 logical updates per second
		//if the draw fps is >50 then logicLoops will always be 1 (ie 1 logical update per draw)
		int logicLoops = Timer::Update();
		bool anyUpdate = (logicLoops>0);
		if( !paused ) {
			if(logicLoops > 10) {
				LogMsg(WARN, "Running %d logic loops. Capping to 1", logicLoops);
				logicLoops = 1;
			}
			while(logicLoops--) {
				if (lowFps)
					lowFpsFrameCount --;
				Timer::IncrementFrameCount();
				// Logical update cycle
				sprites->Update( L, lowFps );
        
        calendar->Update();
			}
		}

		// These only need to be updated once pre Draw cycle, but they can be skipped if there are no Sprite update cycles.
		if( anyUpdate ) {
			starfield.Update( camera );
			camera->Update( sprites );
			Hud::Update( L );
		}

		// Erase cycle
		Video::Erase();

		// Draw cycle
		Video::PreDraw();
		starfield.Draw();
		sprites->Draw( camera->GetFocusCoordinate() );
		Hud::Draw( HUD_ALL, currentFPS, camera, sprites );
		UI::Draw();
		console->Draw();
		Video::PostDraw();
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
				LogMsg(ERR, "The framerate has dropped to zero. Please report this as a bug to 'epiar-devel@epiar.net'");
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

			// Check to see if the player is dead
			if( player->GetHullIntegrityPct() <= 0 ) {
				if( UI::Search("/Window'Death'/") == NULL ) {
					Window* win = new Window( Video::GetWidth()/2-125, Video::GetHeight()/2-70, 250, 140, "Death");
					Button* ok = new Button(70, 85, 100, 30, "Dang!", ConfirmDeath, this);
					UI::Add( win );

					// Player Name
					win->AddChild( (new Label(80, 30, "You have died.")) );
					win->AddChild( ok );
					win->RegisterAction(Action_Close, new ObjectAction(ConfirmDeath, this) );
					win->SetFormButton( ok );
					UI::RegisterKeyboardFocus( win );
				}
			}
		}
	}
	
	Hud::Close();

	LogMsg(INFO,"Simulation Stopped: Average Framerate: %f Frames/Second", 1000.0 *((float)fpsTotal / Timer::GetTicks() ) );

	return (player->GetHullIntegrityPct() > 0);
}

bool Simulation::SetupToEdit() {
	bool luaLoad = true;

	LogMsg(INFO, "Simulation Edit Setup Starting");

	// Load main Lua registers
	LuaRegisters(L);
	// Load ::Edit()-specific Lua registers
	Simulation_Lua::RegisterEditor(L);

	luaLoad = Lua::Load("Resources/Scripts/utilities.lua")
	       && Lua::Load("Resources/Scripts/universe.lua")
	       && Lua::Load("Resources/Scripts/commands.lua")
	       && Lua::Load("Resources/Scripts/editor.lua");

	if (!luaLoad) {
		LogMsg(ERR,"Fatal error starting Lua.");
		return false;
	}

	// Since the Random Universe Editor is currently broken, disable this feature here.
	SETOPTION( "options/simulation/random-universe", 0 );

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

	LogMsg(INFO, "Simulation Edit Setup Complete");

	return true;
}

bool Simulation::Edit() {
	quit = false;

	// Generate a starfield
	Starfield starfield( OPTION(int, "options/simulation/starfield-density") );

	LogMsg(INFO, "Simulation Edit Starting");

	Lua::Call("componentDebugger");

	while( !quit ) {
		HandleInput();

		Timer::Update();
		starfield.Update( camera );
		sprites->Update( L, true );
		camera->Update( sprites );
		Hud::Update( L );

		// Erase cycle
		Video::Erase();

		// Draw cycle
		starfield.Draw();
		sprites->Draw( camera->GetFocusCoordinate() );
		UI::Draw();
		Hud::Draw( HUD_Target | HUD_Map, 0.0f, camera, sprites );
		console->Draw();
		Video::Update();

		// Don't kill the CPU (play nice)
		Timer::Delay( 50 );
	}

	LogMsg(INFO, "Simulation Edit Stopping");

	return true;
}

/**\brief Subroutine. Calls various Lua register functions needed by both Run and Edit
 * \return true if successful
 */
void Simulation::LuaRegisters(lua_State *L) {
	Simulation_Lua::RegisterSimulation(L);
	UI_Lua::RegisterUI(L);
	Audio_Lua::RegisterAudio(L);
	Planets_Lua::RegisterPlanets(L);
	Hud::RegisterHud(L);
	Video::RegisterVideo(L);
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
	if( weapons->Load( (folderpath + Get("weapons")) ) != true ) {
		LogMsg(ERR, "There was an error loading the weapons from '%s'.", (folderpath + Get("weapons")).c_str() );
		return false;
	}
	if( models->Load( (folderpath + Get("models")) ) != true ) {
		LogMsg(ERR, "There was an error loading the models from '%s'.", (folderpath + Get("models")).c_str() );
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

	// Check the Music
	bgmusic = Song::Get( Get("music") );
	if( bgmusic == NULL ) {
		LogMsg(WARN, "There was an error loading music from '%s'.", Get("music").c_str() );
	}

	// Check the Player Defaults
	if( planets->Get( Get("defaultPlayer/start")) == NULL) {
		LogMsg(ERR, "Bad Default Player Start Location '%s'.", Get("defaultPlayer/start").c_str() );
		return false;
	}
	if( models->Get( Get("defaultPlayer/model")) == NULL) {
		LogMsg(ERR, "Bad Default Player Start Model '%s'.", Get("defaultPlayer/model").c_str() );
		return false;
	}
	if( engines->Get( Get("defaultPlayer/engine")) == NULL) {
		LogMsg(WARN, "Bad Default Player Start Engine '%s'.", Get("defaultPlayer/engine").c_str() );
		return false;
	}

	return true;
}

/**\brief Handle User Input
 */
void Simulation::HandleInput() {
	list<InputEvent> events;

	// Collect user input events
	events = inputs.Update();

	// Pass the Events to the systems that handle them.
	UI::HandleInput( events );
	console->HandleInput( events );
	Hud::HandleInput( events, camera, sprites );

	if( !paused )
	{
		inputs.HandleLuaCallBacks( events );
	}

	//if( Input::HandleSpecificEvent( events, InputEvent( KEY, KEYTYPED, SDLK_PERIOD ) ) )
	//{
		//Video::SaveScreenshot();
	//}

	if( Input::HandleSpecificEvent( events, InputEvent( KEY, KEYTYPED, 'n') ) )
	{
		CreateNavMap();
	}

	if( Input::HandleSpecificEvent( events, InputEvent( KEY, KEYTYPED, 'p') ) )
	{
		if ( UI::Search("/Window'Epiar is Paused'/") )
		{
			UI::Close( UI::Search("/Window'Epiar is Paused'/") );
		}
		else
		{
			Window* win = new Window(
				TO_INT(Video::GetWidth() * 0.4),
				TO_INT(Video::GetHeight() * 0.4),
				TO_INT(Video::GetWidth() * 0.2),
				TO_INT(Video::GetHeight() * 0.2),
				"Epiar is Paused" );
			Button* ok = new Button(
				TO_INT(win->GetW()/2) -50,
				TO_INT(win->GetH()/2) -15,
				100,
				30,
				"Unpause", UI::Close, win);
			win->AddChild( ok );
			win->RegisterAction(Action_Close, new ObjectAction(Unpause, this) );
			win->SetFormButton( ok );

			pause();
			UI::Add( win );
			UI::RegisterKeyboardFocus( win );
		}
	}

	if( Input::HandleSpecificEvent( events, InputEvent( KEY, KEYTYPED, '?') ) )
	{
		Dialogs::OptionsWindow();
	}

	if( Input::HandleSpecificEvent( events, InputEvent( KEY, KEYTYPED, '/') ) )
	{
		Lua::Call("keyboardCommands");
	}

	if( Input::HandleSpecificEvent( events, InputEvent( KEY, KEYTYPED, 'j') ) )
	{
		list<string>* names = planets->GetNames();
		int i = 0;
		int x = rand() % names->size();
		list<string>::iterator pName = names->begin();
		while( i++ < x ){ pName++; }
		Planet* p = planets->GetPlanet(*pName);
		player->Jump( p->GetWorldPosition() + GaussianCoordinate()*p->GetInfluence(), true );
	}
	
	if( Input::HandleSpecificEvent( events, InputEvent( KEY, KEYTYPED, SDLK_ESCAPE ) ) )
	{
		quit = true;
	}
}

void Simulation::CreateNavMap( void )
{
	// Toggle NavMap off if it already exists
	if( UI::Search("/Window'Navigation'/") )
	{
		UI::Close( UI::Search("/Window'Navigation'/") );
		return;
	}

	Window* win = new Window(
		TO_INT(Video::GetWidth() * 0.1),
		TO_INT(Video::GetHeight() * 0.1),
		TO_INT(Video::GetWidth() * 0.8),
		TO_INT(Video::GetHeight() * 0.8),
		"Navigation" );

	Map* map = new Map( 10, 26,
		TO_INT(win->GetW()) - 18,
		TO_INT(win->GetH()) - 33,
		camera->GetFocusCoordinate(),
		sprites );

	// Restore Saved Scale
	if( mapScale > 0.0f ) {
		map->SetScale( mapScale );
	}
	map->RegisterAction(Action_MouseWDown, new ObjectAction(SaveMapScale, this) );
	map->RegisterAction(Action_MouseWUp, new ObjectAction(SaveMapScale, this) );

	win->AddChild( map );
	win->AddCloseButton();


	// Pause now, but unpause when this window is closed.
	pause();
	win->RegisterAction(Action_Close, new ObjectAction(Unpause, this) );
	UI::Add( win );

	// Alternatively: make the Map a Modal Widget.  But this is not as nice.
	//win->RegisterAction(Widget::Action_Close, new VoidAction(UI::ReleaseModality) );
	//UI::ModalDialog( win );
}

/**\fn Simulation::isPaused()
 * \brief Checks to see if Simulation is paused
 * \fn Simulation::isLoaded()
 * \brief Checks to see if Simulation is Loaded Successfully
 */

/**\brief Define the new Player Attributes
 * \warn This will log any issues, but it will let
 * \param[in] playerName The player's name.
 */
void Simulation::SetDefaultPlayer( string startPlanet, string modelName, string engineName, int credits ) {
	LogMsg(INFO, "Setting the Player Defaults" );

	// Log disrepencies, but don't fix.
	if( planets->Get( startPlanet ) == NULL )
		LogMsg(WARN, "Setting the Player's start planet to '%s', but this planet does not exist.", startPlanet.c_str() );
	if( models->Get( modelName ) == NULL )
		LogMsg(WARN, "Setting the Player's start model to '%s', but this model does not exist.", modelName.c_str() );
	if( engines->Get( engineName ) == NULL )
		LogMsg(WARN, "Setting the Player's start engine to '%s', but this engine does not exist.", engineName.c_str() );
	
	// Set player defaults in the simulation xml
	Set("defaultPlayer/start", startPlanet);
	Set("defaultPlayer/model", modelName);
	Set("defaultPlayer/engine", engineName);
	Set("defaultPlayer/credits", credits);
}

/**\brief Create and Remember a new Player
 * \note This does not run the player related Lua code.
 * \warn Don't calling this more than once.
 * \param[in] playerName The player's name.
 */
void Simulation::CreateDefaultPlayer(string playerName) {
	Coordinate startPos(0,0);
	string startPlanet = Get("defaultPlayer/start");
	if( planets->GetPlanet( startPlanet ) ) {
		startPos = planets->GetPlanet( startPlanet )->GetWorldPosition();
	}

	assert( player == NULL );
	assert( models->GetModel( Get("defaultPlayer/model") ) );
	assert( engines->GetEngine( Get("defaultPlayer/engine") ) );
	player = players->CreateNew(
        GetName(),
		playerName,
		models->GetModel( Get("defaultPlayer/model") ),
		engines->GetEngine( Get("defaultPlayer/engine") ),
		convertTo<int>( Get("defaultPlayer/credits")),
		startPos
	);

	sprites->AddPlayer( player );
	camera->Focus( player );
}

/**\brief Load Create and Remember a new Player
 * \note This does not run any of the Lua code.
 * \warn Don't calling this more than once.
 * \param[in] playerName The player's name.
 */
void Simulation::LoadPlayer(string playerName) {
	assert( player == NULL );
	player = players->LoadPlayer( playerName );
	sprites->Add( player );
	camera->Focus( player );
}

/**\brief 
 * \return true if the player wants to quit
 */
Player *Simulation::GetPlayer() {
	if ( player == NULL ) {
	    LogMsg(WARN, "No Player has been loaded!");
	}
	assert( player != NULL );
	return player;
}
