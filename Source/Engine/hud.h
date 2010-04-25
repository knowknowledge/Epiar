/**\file			hud.h
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created  : Sunday, July 23, 2006
 * \date			Modified: Sunday, November 22, 2009
 * \brief			Handles the Heads-Up-Display
 * \details
 */

#ifndef __h_hud__
#define __h_hud__

#include "includes.h"
#include "Graphics/image.h"
#include "Utilities/lua.h"
#include "Input/input.h"

#define EPIAR_HUD_TABLE "Epiar.HUD"
#define EPIAR_HUD "HUD"

class AlertMessage {
	public:
		AlertMessage( string message, Uint32 start );
		bool operator ==(const AlertMessage& other) {return (start == other.start) && (message == other.message);}
		string message;
		Uint32 start;
};

class StatusBar {
	public:
		StatusBar(string _title, int _width, QuadPosition _pos, string _name, float _ratio);
		void Draw(int x, int y);
		static Image *im_infobar_left,
				*im_infobar_right,
				*im_infobar_middle;
		void SetName( string n ) { name = n; }
		string GetName() { return name; }
		void SetRatio(float _ratio ) { ratio = _ratio; }
		float GetRatio() { return ratio; }
		QuadPosition GetPosition(){ return pos; }
	protected:
		string title;
		int width;
		QuadPosition pos;
		string name; // TODO: the name 'name' is bad
		float ratio;
};

class Hud {
	public:
		Hud( void );

		static Hud *Instance();

		static void Update( void );
		static void Draw( void );

		static void HandleInput( list<InputEvent> & events );
		
		static void Alert( const char *, ... );
		static void Target(int id);
		static int GetTarget() {return targetID;}
		
		static void AddStatus( StatusBar* bar );
		static void DeleteStatus( StatusBar* bar );

		// Lua functionality
		static void RegisterHud(lua_State *L);
		static int setVisibity(lua_State *L);
		static int newAlert(lua_State *L);
		static int newStatus(lua_State *L);
		static StatusBar* checkStatus(lua_State *L, int index);
		static int setStatus(lua_State *L);
		static int closeStatus(lua_State *L);
		static int getTarget(lua_State *L);
		static int setTarget(lua_State *L);

	private:
		static void DrawShieldIntegrity();
		static void DrawRadarNav( void );
		static void DrawMessages();
		static void DrawFPS();
		static void DrawStatusBars();
		static void DrawTarget();
	
		static Hud *pInstance;
		
		static list<AlertMessage> AlertMessages;

		static list<StatusBar*> Bars;
		static int targetID;
		static int timeTargeted;
};

class Radar {
	public:
		Radar( void );
		static void Draw( void );
		static void SetVisibility( int visibility );
		static int GetVisibility() { return visibility;}
	
	private:
		static void WorldToBlip( Coordinate &w, Coordinate &b );
	
		static int visibility;
};

#endif // __h_hud__
