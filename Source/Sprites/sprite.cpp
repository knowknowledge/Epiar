/**\file			sprite.cpp
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Saturday, January 5, 2008
 * \brief
 * \details
 */

#include "includes.h"
#include "common.h"
#include "Sprites/sprite.h"
#include "Utilities/log.h"
#include "Utilities/timer.h"

/** \addtogroup Sprites
 * @{
 */

// Sprite ID 0 is only used as a NULL
long int Sprite::sprite_ids = 1;

/**\class Sprite
 * \brief Supertype for all objects in the world.
 * \details Sprites are the objects that move around the universe.
 *          They may be created and destroyed.
 *          Each Sprite has a Unique ID.
 *
 *          Only the SpriteManager should ever store pointers to Sprite
 *          objects.  This is because only the SpriteManager is informed when a
 *          Sprite is removed, so a Sprite object may or may not be valid from
 *          one point to the next.
 *
 *          Sprites are drawn based on their Draw Order and their id.  This
 *          means that all Planets are drawn below all ships, which are drawn
 *          below all Effects.  The Draw Order should also be used to detect
 *          the kind of Sprite given just a Sprite pointer.
 * 
 *          Sprites share Image objects to save on memory usage.
 *
 * \TODO Move function implementations to the .cpp file.
 * \warn NEVER STORE SPRITE POINTERS (unless you are the SpriteManager).
 * \sa SpriteManager
 */



/**\brief Default Sprite Constructor
 * \details Gets a unique ID.
 *          Sets the radarColor as Grey.
 */
Sprite::Sprite() {
	id = sprite_ids++;

	// Momentum caps

	angle = 0.;
	
	image = NULL;
	
	radarSize = 1;
	radarColor = WHITE * 0.7f;

	lastUpdateFrame = Timer::GetLogicalFrameCount();
}

Coordinate Sprite::GetWorldPosition( void ) const {
	return worldPosition;
}

void Sprite::SetWorldPosition( Coordinate coord ) {
	worldPosition = coord;
}


/**\brief Move this Sprite in the direction of their current momentum.
 * \details Since this is a space simulation, there is no Friction; momentum does not decrease over time.
 */
void Sprite::Update( lua_State *L ) {
	Uint32 currentFrame = Timer::GetLogicalFrameCount();

	Uint32 framesSinceUpdate = (currentFrame > lastUpdateFrame) 
						? (currentFrame - lastUpdateFrame) 
						: (lastUpdateFrame - currentFrame);

	lastUpdateFrame = currentFrame;

	// Apply their momentum to change their coordinates - apply it as often as the num frames that we've skipped
	worldPosition += (momentum * framesSinceUpdate);
	
	// update acceleration - we do not care about the framesSinceUpdate for updating thesef
	acceleration = lastMomentum - momentum; 
	lastMomentum = momentum;
}

/**\brief Draw
 * \details The Sprite is drawn centered on wx,wy.
 *          This will attempt to Draw the sprite even if wx,wy are completely off the Screen.
 *          Avoid drawing sprites that are too far off the Screen.
 * \sa SpriteManager::Draw
 */
void Sprite::Draw( void ) {
	int wx, wy;

	wx = worldPosition.GetScreenX();
	wy = worldPosition.GetScreenY();
	
	if( image ) {
		image->DrawCentered( wx, wy, angle );
	} else {
		LogMsg(WARN, "Attempt to draw a sprite before an image was assigned." );
	}
}

/** \brief Comparator function for ordering Sprites
 *
 * \details The goal here is to order the sprites in a deterministic way.
 *          We also need the Sprites to be ordered by their DRAW_ORDER.
 *          Since the Sprite ID is unique, and monotonically increasing, this
 *          will sort older sprites below newer sprites.
 *
 * \param a A pointer to a Sprite.
 * \param b A pointer to another Sprite.
 * \returns True if Sprite a should be below Sprite b.
 *
 * \relates Sprite
 */

bool compareSpritePtrs(Sprite* a, Sprite* b){
	if(a->GetDrawOrder() != b->GetDrawOrder()) {
		return a->GetDrawOrder() < b->GetDrawOrder();
	} else {
		return a->GetID() < b->GetID();
	}
}

/** @} */

