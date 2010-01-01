/**\file			hud.h
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created  : Sunday, July 23, 2006
 * \date			Modified: Sunday, November 22, 2009
 * \brief			Handles the Heads-Up-Display
 * \detailsNotes
 */

#ifndef __h_hud__
#define __h_hud__

#include "Graphics/image.h"
#include "includes.h"
#include "Sprites/spritemanager.h"

class AlertMessage {
	public:
		AlertMessage( string message, Uint32 start );
		bool operator ==(const AlertMessage& other) {return (start == other.start) && (message == other.message);}
		string message;
		Uint32 start;
};

class Hud {
	public:
		Hud( void );

		static Hud *Instance();

		static void Update( void );
		static void Draw( SpriteManager *sprites );
		
		static void Alert( const char *, ... );

	private:
		static void DrawHullIntegrity();
		static void DrawShieldIntegrity();
		static void DrawRadarNav( SpriteManager *sprites );
		static void DrawMessages();
		static void DrawFPS();
	
		static Hud *pInstance;
		
		static list<AlertMessage> AlertMessages;
};

class Radar {
	public:
		Radar( void );
		static void Draw( SpriteManager *sprites );
		static void SetVisibility( int visibility );
	
	private:
		static void WorldToBlip( Coordinate &w, Coordinate &b );
	
		static int visibility;
};

#endif // __h_hud__
