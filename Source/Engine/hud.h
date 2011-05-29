/**\file			hud.h
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created  : Sunday, July 23, 2006
 * \date			Modified: Sunday, November 22, 2009
 * \brief			Handles the Heads-Up-Display
 * \details
 */

#ifndef __h_hud__
#define __h_hud__

#include "includes.h"
#include "Graphics/image.h"
#include "Graphics/font.h"
#include "Utilities/lua.h"
#include "Utilities/quadtree.h"
#include "Input/input.h"
#include "Engine/camera.h"
#include "Sprites/spritemanager.h"

#define EPIAR_HUD_TABLE "Epiar.HUD"
#define EPIAR_HUD "HUD"
#define MAX_STATUS_BARS 20
#define MAX_ALERTS 10

// Hud Bitflags to determine what should be drawn
#define HUD_NONE        0x0000
#define HUD_Target      0x0001
#define HUD_Shield      0x0002
#define HUD_Radar       0x0004
#define HUD_Messages    0x0008
#define HUD_FPS         0x0010
#define HUD_StatusBars  0x0020
#define HUD_Map         0x0040
#define HUD_ALL         0xFFFF


enum HudMap{
	NoMap, ///< Do not display any kind of Map.
	QuadrantMap, ///< Draw a Map of the Sprites in this Quadrant.
	UniverseMap, ///< Draw a Map of all the Sprite in the Universe.
	};

class AlertMessage {
	public:
		AlertMessage( string message, Uint32 start );
		bool operator ==(const AlertMessage& other) {return (start == other.start) && (message == other.message);}
		string message;
		Uint32 start;
};

class StatusBar {
	public:
		StatusBar(string _title, int _width, QuadPosition _pos, string _updater);
		StatusBar& operator=( StatusBar& object );
		void Update( lua_State *L );
		void Draw(int x, int y);

		void SetName( string n );
		void SetRatio(float _ratio ) { ratio = _ratio; }

		string GetTitle() { return string(title); }
		string GetName() { return string(name); }
		float GetRatio() { return ratio; }
		QuadPosition GetPosition(){ return pos; }
		
	protected:
		char title[40];
		const int width;
		const QuadPosition pos;
		char name[100]; // TODO: the name 'name' is bad
		float ratio;
		string lua_updater;
	private:
		static Font *font;
};

class Hud {
	public:
		static void Init( void );
		static void Close( void );

		static void Update( lua_State *L );
		static void Draw( int flags, float fps, Camera* camera, SpriteManager* sprites );

		static void HandleInput( list<InputEvent> & events, Camera* camera, SpriteManager* sprites );
		
		static void Alert( const char *, ... );
		static void Target(int id);
		static int GetTarget() {return targetID;}
		
		static void AddStatus( StatusBar* bar );
		static bool DeleteStatus( StatusBar* bar );
		static bool DeleteStatus( string deleteTitle );
		static void DeleteStatusIfExists( string deleteTitle );
		static bool HasStatusMatching( string deletePattern );

		static void SetMapDisplay( HudMap _newMapDisplay );

		// Lua functionality
		static void RegisterHud(lua_State *L);
		static int setVisibity(lua_State *L);
		static int newAlert(lua_State *L);
		static int newStatus(lua_State *L);
		static int closeStatus(lua_State *L);
		static int closeStatusIfExists(lua_State *L);
		static int HudHasStatusMatching(lua_State *L);
		static int getTarget(lua_State *L);
		static int setTarget(lua_State *L);
		static int setMapDisplay(lua_State *L);
		static int getMapDisplay(lua_State *L);

	private:
		static void DrawShieldIntegrity();
		static void DrawRadarNav( Camera* camera, SpriteManager* sprites );
		static void DrawMessages();
		static void DrawFPS( float fps, SpriteManager* sprites );
		static void DrawStatusBars();
		static void DrawTarget(SpriteManager* sprites);
		static void DrawMap( Camera* camera, SpriteManager* sprites );
		static void DrawUniverseMap( Camera* camera, SpriteManager* sprites );

		static list<AlertMessage> AlertMessages;

		static StatusBar* Bars[MAX_STATUS_BARS];
		static int targetID;
		static int timeTargeted;
		static HudMap mapDisplay;
		static Font *AlertFont;
		static Font *MapFont;
		static Color AlertColor;
};

class Radar {
	public:
		Radar( void );
		static void Draw( Camera* camera, SpriteManager* sprites );
		static void SetVisibility( int visibility );
		static int GetVisibility() { return visibility;}
	
	private:
		static void WorldToBlip( Coordinate focus, Coordinate &w, Coordinate &b );
	
		static int visibility;
};

#endif // __h_hud__
