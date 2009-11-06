/*
 * Filename      : hud.cpp
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Sunday, July 23, 2006
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : Handles the Heads-Up-Display
 * Notes         :
 */

#include "common.h"
#include "Engine/console.h"
#include "Engine/hud.h"
#include "Engine/simulation.h"
#include "Graphics/video.h"
#include "includes.h"
#include "Sprites/player.h"
#include "Utilities/log.h"
#include "Utilities/timer.h"

#define ALERT_DELAY 3500
/* Length of the hull integrity bar (pixels) + 6px (the left+right side imgs) */
#define HULL_INTEGRITY_BAR  65
/* Location on screen of hull integrity bar (x,y) coord is top-left */
#define HULL_INTEGRITY_X     5
#define HULL_INTEGRITY_Y     5
/* Center of radar in px coords. Derrived from hud_radarnav.png */
#define RADAR_MIDDLE_X      61
#define RADAR_MIDDLE_Y      61
/* Width/height of radar. Derrived from hud_radarnav.png */
#define RADAR_WIDTH        122
#define RADAR_HEIGHT       122

vector<AlertMessage> Hud::AlertMessages;
Image *Hud::im_hullstr = NULL;
Image *Hud::im_hullstr_leftbar = NULL;
Image *Hud::im_hullstr_rightbar = NULL;
Image *Hud::im_hullstr_bar = NULL;
Image *Hud::im_shieldstat = NULL;
Image *Hud::im_radarnav = NULL;
int Radar::visibility = 4000;

AlertMessage::AlertMessage( string message, Uint32 length )
{
	this->message = message;
	this->length = length;
}

Hud *Hud::pInstance = 0; // initialize pointer

Hud *Hud::Instance( void ) {
	if( pInstance == 0 ) { // is this the first call?
		pInstance = new Hud; // create the sold instance
	}
	return( pInstance );
}

Hud::Hud( void ) {
	/* Load hull strength images */
	im_hullstr = new Image( "hud_hullstr.png" );
	im_hullstr_leftbar = new Image( "hud_hullstr_leftbar.png" );
	im_hullstr_rightbar = new Image( "hud_hullstr_rightbar.png" );
	im_hullstr_bar = new Image( "hud_hullstr_bar.png" );
	/* Load shield integrity images */
	im_shieldstat = new Image( "hud_shieldintegrity.png" );
	/* Load radar and navigation images */
	im_radarnav = new Image( "hud_radarnav.png" );
}

void Hud::Update( void ) {
	Console::Update();

}

void Hud::Draw( SpriteManager &sprites ) {
	Hud::DrawHullIntegrity();
	Hud::DrawShieldIntegrity();
	Hud::DrawRadarNav( sprites );
	Hud::DrawMessages();
	Console::Draw();
	Hud::DrawFPS();
}

// Draw HUD messages (eg Welcome to Epiar)
void Hud::DrawMessages() {
	Vera10->Render( 15, Video::GetHeight() - 15, "Welcome to Epiar 0.7.0-alpha" );
}

// Draw the current framerate (calculated in simulation.cpp)
void Hud::DrawFPS() {
	const char *frameRate[16] = {0};
	memset(frameRate, 0, sizeof(char) * 10);
	sprintf((char *)frameRate, "%f fps", Simulation::GetFPS());
	Vera10->Render( 30, Video::GetHeight() - 30, (const char *)frameRate );
}

void Hud::DrawHullIntegrity() {
	short int pen_x = HULL_INTEGRITY_X;
	short int pen_y = HULL_INTEGRITY_Y;
	
	/* Draw the backing */
	im_hullstr->Draw( pen_x, pen_y );
	/* Draw the left side of the bar */
	pen_x += 40;
	pen_y += 5;
	im_hullstr_leftbar->Draw( pen_x, pen_y );
	
	/* Calculate how long the bar shouuld be based on player's hull health */
	Player *player = Player::Instance();
	
	short int bar_len = (short int)(player->getHullIntegrityPct() * (float)HULL_INTEGRITY_BAR);
	
	pen_x += 3;
	for( int i = 0; i < bar_len; i++ ) {
		im_hullstr_bar->Draw( pen_x + i, pen_y );
	}
	
	/* Draw the right side of the bar (43 is where the left bar ends,
	 * plus bar_len, the length of the middle part of the bar, which decreases
	 * with the player's health */
	pen_x += bar_len;
	im_hullstr_rightbar->Draw( pen_x, pen_y );
}

void Hud::DrawShieldIntegrity() {
	im_shieldstat->Draw( 35, 30 );
}

void Hud::DrawRadarNav( SpriteManager &sprites ) {
	im_radarnav->Draw( Video::GetWidth() - 129, 5 );
	
	Radar::Draw( sprites );
}

void Hud::Alert( const char *message, ... )
{
	va_list args;
	char msgBuffer[ 4096 ] = {0};

	va_start( args, message );

	vsnprintf( msgBuffer, 4095, message, args );

	va_end( args );

	AlertMessages.push_back( AlertMessage( msgBuffer, Timer::GetTicks() + ALERT_DELAY ) );
	printf( "Message: msgBuffer\n" );
}

Radar::Radar( void ) {
}

void Radar::SetVisibility( int visibility ) {
	Radar::visibility = visibility;
}

void Radar::Draw( SpriteManager &sprites ) {
	short int radar_mid_x = RADAR_MIDDLE_X + Video::GetWidth() - 129;
	short int radar_mid_y = RADAR_MIDDLE_Y + 5;
	int radarSize;

	const list<Sprite*>& spriteList = sprites.GetSprites();
	for( list<Sprite*>::const_iterator iter = spriteList.begin(); iter != spriteList.end(); iter++)
	{
		Coordinate blip( -(RADAR_HEIGHT / 2.0), (RADAR_WIDTH / 2.0), (RADAR_HEIGHT / 2.0), -(RADAR_WIDTH / 2.0) );
		Sprite *sprite = *iter;
		
		if( sprite->GetDrawOrder() == DRAW_ORDER_PLAYER ) continue;
		
		// Calculate the blip coordinate for this sprite
		Coordinate wpos = sprite->GetWorldPosition();
		WorldToBlip( wpos, blip );
		
		if( blip.ViolatesBoundary() == false ) {
			/* blip is on the radar */
			
			/* Convert to screen coords */
			blip.SetX( blip.GetX() + radar_mid_x );
			blip.SetY( blip.GetY() + radar_mid_y );

			radarSize = (sprite->GetRadarSize() / float(visibility)) * (RADAR_HEIGHT/4.0);
			
			
			Video::DrawCircle(
				blip,
				(radarSize>=1) ?radarSize: 1,
				1,
				sprite->GetRadarColor() );
		}
	}
}

void Radar::WorldToBlip( Coordinate &w, Coordinate &b ) {
	Player *p = Player::Instance();
	Coordinate player = p->GetWorldPosition();
	
	b.SetX( ( ( w.GetX() - player.GetX() ) / float(visibility) ) * ( RADAR_WIDTH / 2.0 ) );
	b.SetY( ( ( w.GetY() - player.GetY() ) / float(visibility) ) * ( RADAR_HEIGHT / 2.0 ) );
}
