/*
 * Filename      : hud.h
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Unknown (2006?)
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : 
 * Notes         :
 */

#ifndef __h_hud__
#define __h_hud__

#include "Graphics/image.h"
#include "includes.h"
#include "Sprites/spritelist.h"

class AlertMessage {
	public:
		AlertMessage( string message, Uint32 length );

		string message;
		Uint32 length;
};

class Hud {
	public:
		Hud( void );

		static Hud *Instance();

		static void Update( void );
		static void Draw( SpriteList &sprites );
		
		static void Alert( const char *, ... );

	private:
		static void DrawHullIntegrity();
		static void DrawShieldIntegrity();
		static void DrawRadarNav( SpriteList &sprites );
		static void DrawMessages();
		static void DrawConsole();
	
		static Hud *pInstance;
		
		/* Hull integrity images */
		/* Image backing */
		static Image *im_hullstr;
		/* Strength bar (in three pieces, left, middle (repeated over x), right */
		static Image *im_hullstr_leftbar, *im_hullstr_rightbar, *im_hullstr_bar;
		/* Shield integrity status */
		static Image *im_shieldstat;
		/* Radar and navigation */
		static Image *im_radarnav;

		static vector<AlertMessage> AlertMessages;
};

class Radar {
	public:
		Radar( void );
		static void Draw( SpriteList &sprites );
		static void SetVisibility( int visibility );
	
	private:
		static void WorldToBlip( Coordinate &w, Coordinate &b );
	
		static int visibility;
};

#endif // __h_hud__
