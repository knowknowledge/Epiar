/**\file			ship.cpp
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Friday, November 14, 2009
 * \brief
 * \details
 */

#include "includes.h"
#include "common.h"
#include "Sprites/ship.h"
#include "Engine/camera.h"
#include "Engine/simulation_lua.h"
#include "Utilities/timer.h"
#include "Utilities/trig.h"
#include "Sprites/spritemanager.h"
#include "Utilities/xml.h"
#include "Sprites/effects.h"
#include "Audio/sound.h"
#include "Engine/hud.h"

/** \addtogroup Sprites
 * @{
 */

#define NON_PLAYER_SOUND_RATIO 0.4f ///< Ratio used to quiet NON-PLAYER Ship Sounds.

/**\class Ship
 * \brief A Ship Sprite that moves, Fires Weapons, has cargo, and ultimately explodes.
 * \details
 *
 * Ships are at the core of the Epiar gameplay.  Each Ship is a Sprite using
 * the Image from their Model.  Although each Model has a default Engine, each
 * ship can swap out the engine for a different one.
 *
 * Ships themselves are inert objects and are alway controlled by either a
 * Player or an AI.
 *
 * \b Movement:
 *
 *   Like all Sprites, Ships move because they have non-zero momentum, but
 *   unlike most other Sprites Ships control their momentum with their Engines.
 *   The Engines can only accelerate the ship in the direction that the ship is
 *   facing.  The ship accelerates according to Newtonian Physics: A=F/M; where
 *   F is the Engine's Force and M is the Mass of the Ship. In order for a ship
 *   to turn around, ships must rotate to face the direction oposite their
 *   momentum before accelerating.  Unlike the real world Epiar ships have a
 *   maximum velocity.
 *   \see Ship::Accelerate
 *   \see Ship::Rotate
 *
 * \b Jumping:
 *
 *   Some Engines are equipped with "Jump Drives".  These Faster-Than-Light
 *   (FTL) Drives will move the ship nearly instantly to any point in the
 *   Universe.  Ships without Jump Drives can still 'Jump' by using the Gate
 *   network.
 *   \see Ship::JumpDrive
 *   \see Ship::Jump
 *
 * \b Outfit:
 *
 *   The parts of the ship are called Outfit.  Each Outfit affects a ships
 *   stats and can give the ship extra abilities.  There are four main types of
 *   Outfit:
 *   - The Model: This is the design of the ship.  The Model describes what a
 *     ship looks like, where its weapons are mounted, and sets the baseline
 *     for most of the ship stats.
 *   - The Engine: This is how the ship moves around.
 *   - The Weapons: This is how the ships fight one another.  Weapons are
 *     attached to specific WeaponSlots on the Model and are seperated into
 *     either Primary or Secondary firing groups.
 *   - Other: There are other kinds of Ship Outfits that affect their
 *     statistics.  These other outfits have no represention on the Sprite.
 *   \see ComputeShipStats
 *
 * \b Combat:
 *
 *   Ships fight by releasing Projectiles from their Weapons.
 *   \see Ship::Fire
 *   \see Ship::FirePrimary
 *   \see Ship::FireSecondary
 *
 * \fn Ship::GetOutfits
 * \return A List of all Outfits on this Ship.
 *
 * \fn Ship::GetEngine
 * \return The Engine Attached to this Ship.
 *
 * \fn Ship::GetCredits
 * \return The Credits held by this Ship.
 *
 * \fn Ship::GetCargoSpaceUsed
 * \return The total tonnage of cargo space consumed.
 */

/**\brief Ship constructor that initializes default values.
 */
Ship::Ship()
{
	model = NULL;
	engine = NULL;
	flareAnimation = NULL;
	
	/* Initalize ship's condition */
	status.damageBooster=1.0;
	status.engineBooster=1.0;
	status.shieldBooster=1.0;
	status.hullDamage = 0;
	status.shieldDamage = 0;
	status.lastWeaponChangeAt = 0;

	memset(status.lastFiredAt, 0, sizeof(status.lastFiredAt));

	status.cargoSpaceUsed = 0;
	status.isAccelerating = false;
	status.isRotatingLeft = false;
	status.isRotatingRight = false;
	status.isDisabled = false;
	status.isJumping = false;
	for(int a=0;a<max_ammo;a++){
		ammo[a]=0;
	}

	shipStats = Outfit();

	SetRadarColor( RED );
	SetAngle( float( rand() %360 ) );
}

/**\brief Ship Destructor
 */
Ship::~Ship() {
	if(flareAnimation) {
		delete flareAnimation;
		flareAnimation=NULL;
	}
		
}

/**\brief Sets the ship model.
 * \param Model Ship model to use
 * \return true if successful
 * \sa Model
 */
bool Ship::SetModel( Model *model) {
	assert( model );
	if( model ) {
		this->model = model;

		// Copy default weapon slot arrangement from model,
		this->weaponSlots = model->GetWeaponSlots();

		// go ahead and build a weapon list from it
		for(unsigned int i = 0; i < weaponSlots.size(); i++){
			this->AddToShipWeaponList( weaponSlots[i].content );
		}

		SetImage( model->GetImage() );

		ComputeShipStats();
		
		return( true );
	}
	
	return( false );
}

/**\brief Sets the ship engine.
 * \param newEngine Engine to use
 * \return true if successful
 * \sa Engine
 */
bool Ship::SetEngine( Engine *newEngine ) {
	assert( newEngine );
	if( newEngine ){
		engine = newEngine;
		
		// Creates a new Flare Animation specific for this Ship
		if( flareAnimation )
			delete flareAnimation;
		flareAnimation = new Animation( engine->GetFlareAnimation() );
		flareAnimation->Reset();
		flareAnimation->SetLoopPercent(0.25f);

		ComputeShipStats();
		
		return( true );
	}
	
	return( false );
}

/**\brief Returns the sprite associated with the ship.
 * \return Pointer to a Sprite object.
 * \sa Sprite
 */
Sprite *Ship::GetSprite() {
	return( (Sprite *)this );
}

/**\brief Returns the model name.
 * \return string containing model name
 * \sa Model::Getname()
 */
string Ship::GetModelName() {
	if (model){
		return model->GetName();
	} else {
		return "";
	}
}

/**\brief Returns the engine name.
 * \return string containing model name
 * \sa Engine::Getname()
 */
string Ship::GetEngineName() {
	if (engine){
		return engine->GetName();
	} else {
		return "";
	}
}

/**\brief Rotates the ship in the given direction (relative angle).
 * \param direction Relative angle to rotate by
 * \sa Model::GetRotationsperSecond()
 */
void Ship::Rotate( float direction ) {
	float rotPerSecond, timerDelta, maxturning;
	float angle = GetAngle();
	
	if( !model ) {
		LogMsg(WARN, "Attempt to rotate sprite with no model." );
		return;
	}

	if(  status.isJumping == true ) {
		return;
	}

	// Compute the maximum amount that the ship can turn
	rotPerSecond = shipStats.GetRotationsPerSecond();
	timerDelta = Timer::GetDelta();
	maxturning = static_cast<float>((rotPerSecond * timerDelta) * 360.);

	// Cap the ship rotation
	if (fabs(direction) > maxturning){
		if (direction > 0 ){
			angle += maxturning;
			status.isRotatingLeft = true;
		}else{
			angle -= maxturning;
			status.isRotatingRight = true;
		}
	} else {
		angle += direction;
	}
	
	// Normalize
	angle = normalizeAngle(angle);
	
	SetAngle( angle );
	
	// Play engine sound
	/*
	if( engine->GetSound() != NULL)
	{
		float engvol = OPTION(float,"options/sound/engines");
		Coordinate offset = GetWorldPosition() - Camera::Instance()->GetFocusCoordinate();
		if ( this->GetDrawOrder() == DRAW_ORDER_SHIP )
			engvol = engvol * NON_PLAYER_SOUND_RATIO ;
		this->engine->GetSound()->SetVolume( engvol );
		this->engine->GetSound()->PlayNoRestart( offset );
	}
	*/
}

/**\brief Accelerates the ship.
 * \sa Model::GetAcceleration
 */
void Ship::Accelerate( void ) {
	if(  status.isJumping == true ) {
		return;
	}

	Trig *trig = Trig::Instance();
	Coordinate momentum = GetMomentum();
	float angle = static_cast<float>(trig->DegToRad( GetAngle() ));
	float speed = shipStats.GetMaxSpeed() * status.engineBooster;

	float acceleration = (shipStats.GetForceOutput() * status.engineBooster ) / shipStats.GetMass();

	momentum += Coordinate( trig->GetCos( angle ) * acceleration * Timer::GetDelta(),
	                -1 * trig->GetSin( angle ) * acceleration * Timer::GetDelta() );

	momentum.EnforceMagnitude(speed);
	
	SetMomentum( momentum );
	
	status.isAccelerating = true;

	// Play engine sound
	if( engine->GetSound() != NULL)
	{
		float engvol = OPTION(float,"options/sound/engines");
		Coordinate offset = GetWorldPosition() - Camera::Instance()->GetFocusCoordinate();
		if ( this->GetDrawOrder() == DRAW_ORDER_SHIP )
			engvol = engvol * NON_PLAYER_SOUND_RATIO ;
		this->engine->GetSound()->SetVolume( engvol );
		this->engine->GetSound()->PlayNoRestart( offset );
	}
}


/**\brief Adds damage to hull or shield
 */
void Ship::Damage(short int damage) {
	if(status.shieldDamage >= ((float)shipStats.GetShieldStrength()) * status.shieldBooster)
		status.hullDamage += damage;
	else
		status.shieldDamage += damage;

	if( GetHullIntegrityPct() < .15 ) {
		status.isDisabled = true;
		SetMomentum( GetMomentum() * .75 );
	}
}


/**\brief Repairs the ship.
 */
void Ship::Repair(short int damage) {
	if( damage >= status.hullDamage) {
		damage -= status.hullDamage;
		status.hullDamage = 0;
		status.shieldDamage -= damage;
		if( status.shieldDamage < 0 )
			status.shieldDamage = 0;
	} else {
		status.hullDamage -= damage;
		if( status.hullDamage < 0 )
			status.hullDamage = 0;
	}

	if( GetHullIntegrityPct() >= .15 ) {
		status.isDisabled = false;
	}
}

/**\brief Begin a Jump
 */
bool Ship::Jump( Coordinate position ) {
	if(  status.isJumping == true ) { // Already Jumping
		// TODO Play a failure sound.
		return false;
	}
	
	status.isJumping = true;
	status.jumpStartTime = Timer::GetTicks();
	status.jumpDestination = position;
	// TODO Start playing a sound
	SetAngle( (position - GetWorldPosition()).GetAngle() );
	return true;
}

/**\brief Begin a Jump using the Ships Engines
 * \note This is how most Ships should Jump under their own power.
 *       Calling Jump Directly should only be done by Gates.
 */
bool Ship::JumpDrive( Coordinate position ) {
	// Can the ship's Engine Jump?
	if( engine->GetFoldDrive() ) {
		return Jump( position );
	} else {
		// TODO Play a failure sound.
		return false;
	}
}

/**\brief Update function on every frame.
 */
void Ship::Update( lua_State *L ) {
	Sprite::Update( L ); // update momentum and other generic sprite attributes
	
	// Movement Changes
	if( status.isAccelerating == false 
		&& status.isRotatingLeft == false
		&& status.isRotatingRight == false) {
		flareAnimation->Reset();
	}
	flareAnimation->Update();
	Coordinate momentum	= GetMomentum();
	momentum.EnforceMagnitude( shipStats.GetMaxSpeed() * status.engineBooster );

	// Show the hits taken as part of the radar color
	if(IsDisabled()) SetRadarColor( GREY );
	else SetRadarColor( RED * GetHullIntegrityPct() );

	if( status.isJumping ) {
		// When the Jump is complete
		if( Timer::GetTicks() - status.jumpStartTime > 1000 ) {
			status.isJumping = false;
			SetWorldPosition( status.jumpDestination );
		}
	}
	
	// Ship has taken as much damage as possible...
	if( status.hullDamage >=  (float)shipStats.GetHullStrength() ) {
		// It Explodes!
		Explode( L );
	}
}

/**\brief Draw function.
 * \sa Sprite::Draw()
 */
void Ship::Draw( void ) {
	Trig *trig = Trig::Instance();
	Coordinate position = GetWorldPosition();

	/*
    // Shields
	Video::DrawFilledCircle(
			position.GetScreenX(), position.GetScreenY(),
			static_cast<float>(GetRadarSize()), 0.0f,0.0f,0.3f,0.3f);
	*/

	if( status.isJumping ) {
		// When the ship is jumping, move it to the screen edge
		glPushMatrix();
		Coordinate jumpDir = (status.jumpDestination - position);
		jumpDir.EnforceMagnitude( Video::GetHalfWidth() );
		jumpDir *= ((float)Timer::GetRealTicks() - (float)status.jumpStartTime) / 1000.0;
		//cout << "Jump:" << status.jumpDestination << " Pos:" << position << " dir:" << jumpDir <<endl;
		glTranslatef( jumpDir.GetX(), jumpDir.GetY(), 0.0);
	}

	Sprite::Draw();
	
	// Draw the flare animation, if required
	if( status.isAccelerating ) {
		float direction = GetAngle();
		float tx, ty;
		
		trig->RotatePoint( static_cast<float>((position.GetScreenX() -
						(flareAnimation->GetHalfWidth() + model->GetThrustOffset()) )),
				static_cast<float>(position.GetScreenY()),
				static_cast<float>(position.GetScreenX()),
				static_cast<float>(position.GetScreenY()), &tx, &ty,
				static_cast<float>( trig->DegToRad( direction ) ));
		flareAnimation->Draw( (int)tx, (int)ty, direction );
		
		status.isAccelerating = false;
	}
#if defined(ROTATE_ENGINE)
	//TODO: draw flare tilted in the coresponding direction
	if( status.isRotatingLeft || status.isRotatingRight ) {
		float direction = GetAngle();
		float tx, ty;
		
		trig->RotatePoint( static_cast<float>((position.GetScreenX() -
						(flareAnimation->GetHalfWidth() + model->GetThrustOffset()) )),
				static_cast<float>(position.GetScreenY()),
				static_cast<float>(position.GetScreenX()),
				static_cast<float>(position.GetScreenY()), &tx, &ty,
				static_cast<float>( trig->DegToRad( direction ) ));
		flareAnimation->Draw( (int)tx, (int)ty, direction );
		
		status.isRotatingLeft = false;
		status.isRotatingRight = false;
	}
#endif

	if( status.isJumping ) {
		glPopMatrix();
	}
}

/**\brief Fire's ship Primary weapons.
 * \return FireStatus
 */
FireStatus Ship::FirePrimary( int target ) {
	return Fire(PRIMARY,target);
}

/**\brief Fire's ship Secondary weapons.
 * \return FireStatus
 */
FireStatus Ship::FireSecondary( int target ) {
	return Fire(SECONDARY,target);
}

/**\brief Fire a ship's weapon group.
 * \return FireStatus
 */
FireStatus Ship::Fire( Group group, int target ) {
	FireStatus error = FireUnknown;
	bool fired = false; ///< Flag if anything was fired
	bool emptyFiringGroup = true;

	// Check that some weapons are attached
	if ( shipWeapons.empty() ) {
		return FireNoWeapons;
	}
	// Check that the ship is not jumping
	if(  status.isJumping == true ) {
		return FireNotReady;
	}

	for(unsigned int slot = 0; slot < weaponSlots.size(); slot++) {
		if( (weaponSlots[slot].firingGroup == group)
		 && (weaponSlots[slot].content != NULL) )
		{
			emptyFiringGroup = false;
			FireStatus result = FireSlot( slot, target );
			if( result == FireSuccess ) {
				fired = true;
			} else {
				error = result;
			}
		}
	} // End For Each Slot

	if(fired) return FireSuccess;
	if(emptyFiringGroup) return FireEmptyGroup;

	// When nothing fires return the last error that occurred
	return error; 
}

/**\brief Fire a single weapon slot.
 * \return FireStatus
 */
FireStatus Ship::FireSlot( int slot, int target )
{
	Trig *trig = Trig::Instance();
	SpriteManager *sprites = SpriteManager::Instance();
	Weapon* currentWeapon = weaponSlots[slot].content;
	float projectileAngle = 0.0f;

	// Check that the weapon has cooled down
	if( !( currentWeapon->GetFireDelay() < (int)(Timer::GetTicks() - status.lastFiredAt[slot])) ) {
		return FireNotReady;
	}
	// Check that there is sufficient ammo
	if( ammo[currentWeapon->GetAmmoType()] < currentWeapon->GetAmmoConsumption() ) {
		return FireNoAmmo;
	}

	Sprite *targetSprite = sprites->GetSpriteByID(target);

	// A regular fixed weapon slot
	if(weaponSlots[slot].motionAngle == 0) {
		projectileAngle = GetAngle() + weaponSlots[slot].angle;
	}

	// A Turret weapon slot
	else if(targetSprite != NULL) {
		float angleToTarget = GetDirectionTowards(targetSprite->GetWorldPosition());
		float angleFromSlot = fabs(weaponSlots[slot].angle - angleToTarget);
		if( (weaponSlots[slot].motionAngle == 360)
		 || (angleFromSlot < weaponSlots[slot].motionAngle/2)
		 || (angleFromSlot > (360-weaponSlots[slot].motionAngle/2))
		) {
			projectileAngle = GetAngle() + angleToTarget;
		} else {
			return FireNoClearShot; // This Slot cannot aim at the target
		}
	}

	// A Turret but no target
	else {
		return FireNoTarget;
	}

	// Play weapon sound
	if( currentWeapon->GetSound() != NULL ) {
		float weapvol = OPTION(float,"options/sound/weapons");
		if ( this->GetDrawOrder() == DRAW_ORDER_SHIP ) {
			weapvol *= NON_PLAYER_SOUND_RATIO;
		}
		currentWeapon->GetSound()->SetVolume( weapvol );
		currentWeapon->GetSound()->Play( GetWorldPosition() - Camera::Instance()->GetFocusCoordinate() );
	}

	// Find the world position of this slot
	float angle = static_cast<float>(trig->DegToRad( GetAngle()));
	Coordinate slotPosition = Coordinate( weaponSlots[slot].x, weaponSlots[slot].y ).RotateTo( angle ) + GetWorldPosition();

	// Fire the weapon
	Projectile *projectile = new Projectile(status.damageBooster, projectileAngle, slotPosition, GetMomentum(), currentWeapon);
	projectile->SetOwnerID( this->GetID() );
	projectile->SetTargetID( target );
	sprites->Add( (Sprite*)projectile );

	// Consume ammo
	ammo[currentWeapon->GetAmmoType()] -=  currentWeapon->GetAmmoConsumption();

	// Track number of ticks the last fired occured for this weapon
	status.lastFiredAt[slot] = Timer::GetTicks();

	return FireSuccess;
}

void Ship::Explode( lua_State *L )
{
	SpriteManager *sprites = Simulation_Lua::GetSimulation(L)->GetSpriteManager();
	Camera* camera = Simulation_Lua::GetSimulation(L)->GetCamera();

	// Play explode sound
	if(OPTION(int, "options/sound/explosions")) {
		Sound *explodesnd = Sound::Get("Resources/Audio/Effects/18384__inferno__largex.wav.ogg");
		explodesnd->Play( GetWorldPosition() - camera->GetFocusCoordinate());
	}

	// Create Explosion
	sprites->Add( new Effect( GetWorldPosition(), "Resources/Animations/explosion1.ani", 0) );

	// Remove this Sprite from the SpriteManager
	sprites->Delete( (Sprite*)this );
}

/**\brief Adds a new weapon to the ship WITHOUT updating weaponSlots.
 * \param i Pointer to Weapon instance
 * \sa Weapon
 */
void Ship::AddToShipWeaponList(Weapon *w){
	if( w ) {
		shipWeapons.push_back(w);
		ComputeShipStats();
	}
}


/**\brief Adds a new weapon to the ship by name.
 * \param weaponName Name of the Weapon
 * \sa Weapon
 */
void Ship::AddToShipWeaponList(string weaponName){
	if(weaponName == "") return; // not a weapon

	Weapons *weapons = Weapons::Instance();

	if(weapons->GetWeapon(weaponName)){
		AddToShipWeaponList(weapons->GetWeapon(weaponName));
	} else {
		LogMsg(INFO, "Failed to add weapon '%s', it doesn't exist.", weaponName.c_str());
	}
}

/**\brief Adds a new weapon to the ship AND update weaponSlots.
 * \param i Pointer to Weapon instance
 * \sa Weapon
 */
int Ship::AddShipWeapon(Weapon *w){
	for(unsigned int s = 0; s < weaponSlots.size(); s++){
		WeaponSlot *slot = &weaponSlots[s];
		if(slot->content == NULL){
			slot->content = w; // this will edit-in-place, so no need to shove a struct back into weaponSlots
			AddToShipWeaponList(w);
			return 1;
		}
	}
	return 0;
}

/**\brief Adds a new weapon to the ship by name AND updates weaponSlots
 * \param weaponName Name of the Weapon
 * \sa Weapon
 */
int Ship::AddShipWeapon(string weaponName){
	Weapons *weapons = Weapons::Instance();
	if(weapons->GetWeapon(weaponName)){
		if(AddShipWeapon(weapons->GetWeapon(weaponName)))
			return 1;
	} else {
		LogMsg(INFO, "Failed to add/install weapon '%s', it doesn't exist.", weaponName.c_str());
		return 0;
	}
	return 0;
}

/**\brief Removes a weapon from the ship.
 * \param pos Index of the weapon
 */
void Ship::RemoveFromShipWeaponList(int pos){
	if( (unsigned int)pos >= shipWeapons.size() ) pos = 0;
	shipWeapons.erase(shipWeapons.begin()+pos);
}

/**\brief Removes a weapon from the ship
 * \param i Pointer to Weapon instance
 */
void Ship::RemoveFromShipWeaponList(Weapon *i){
	for(unsigned int pos = 0; pos < shipWeapons.size(); pos++){
		if(shipWeapons[pos]->GetName() == i->GetName()){
			RemoveFromShipWeaponList(pos);
			return;
		}
	}
}

/**\brief Removes a weapon from the ship
 * \param weaponName Name of the Weapon
 */
void Ship::RemoveFromShipWeaponList(string weaponName){
	Weapons *weapons = Weapons::Instance();
	if(weapons->GetWeapon(weaponName)){
		RemoveFromShipWeaponList(weapons->GetWeapon(weaponName));
	} else {
		LogMsg(INFO, "Failed to remove weapon '%s', it doesn't exist.", weaponName.c_str());
	}
}


/**\brief Removes a weapon from the ship AND updates weaponSlots
 * \param i Pointer to Weapon instance
 */
void Ship::RemoveShipWeapon(Weapon *w){
	for(unsigned int pos = 0; pos < shipWeapons.size(); pos++){
		if(shipWeapons[pos]->GetName() == w->GetName()){
			RemoveFromShipWeaponList(pos);
			break;
		}
	}
	for(unsigned int s = 0; s < weaponSlots.size(); s++){
		WeaponSlot *slot = &weaponSlots[s];
		if(slot->content == w){
			slot->content = NULL; // this will edit-in-place, so no need to shove a struct back into weaponSlots
			return;
		}
	}
}

/**\brief Removes a weapon from the ship AND updates weaponSlots
 * \param weaponName Name of the Weapon
 */
void Ship::RemoveShipWeapon(string weaponName){
	Weapons *weapons = Weapons::Instance();
	if(weapons->GetWeapon(weaponName)){
		RemoveShipWeapon(weapons->GetWeapon(weaponName));
	} else {
		LogMsg(INFO, "Failed to remove weapon '%s', it doesn't exist.", weaponName.c_str());
	}
}

/**\brief Adds ammo to the ship.
 * \param AmmoType Type of ammo that should be added.
 * \param qty Quantity to add
 */
void Ship::AddAmmo(AmmoType ammoType, int qty){
	ammo[ammoType] += qty;
}

void Ship::AddOutfit(Outfit *outfit){
	assert(outfit!=NULL);
	outfits.push_back(outfit);
	ComputeShipStats();
}

void Ship::AddOutfit(string outfitName){
	if( Outfits::Instance()->GetOutfit(outfitName) ){
		AddOutfit( Outfits::Instance()->GetOutfit(outfitName) );
	} else {
		LogMsg(INFO, "Failed to Attach outfit '%s', it doesn't exist.", outfitName.c_str());
	}
}

/**\brief Removes an outfit from the ship
 * \param i Pointer to Outfit instance
 */
void Ship::RemoveOutfit(Outfit *i){
	list<Outfit*> new_list;
	bool done_removing = false;
	while(outfits.size() > 0){
		Outfit *o = outfits.back();
		outfits.pop_back();
		if(o != i || done_removing)
			new_list.push_back(o);
		else
			done_removing = true;
	}
	this->SetOutfits(&new_list);
}

	
/**\brief Removes an outfit from the ship
 * \param outfitName Name of the Outfit
 */
void Ship::RemoveOutfit(string outfitName){
	Outfits *o = Outfits::Instance();
	if(o->GetOutfit(outfitName)){
		RemoveOutfit(o->GetOutfit(outfitName));
	} else {
		LogMsg(INFO, "Failed to remove outfit '%s', it doesn't exist.", outfitName.c_str());
	}
}

/**\brief Set the number of credits
 */
void Ship::SetCredits(unsigned int _credits) {
	credits = _credits;
}

/**\brief Set the number of credits
 */
map<Commodity*,unsigned int> Ship::GetCargo() {
	map<Commodity*,unsigned int> retMap(commodities);
	return retMap;
}

/**\brief Store a number of tons of a commodity
 */
int Ship::StoreCommodities(string commodity, unsigned int count) {
	Commodity* com = Commodities::Instance()->GetCommodity(commodity);

	LogMsg(INFO, "Storing %d tons of %s.", count, commodity.c_str());

	// Ensure that we have enough space to store this cargo
	unsigned int cargoSpaceRemaining = (shipStats.GetCargoSpace() - status.cargoSpaceUsed);
	if( count > cargoSpaceRemaining ) {
		LogMsg(INFO, "Cannot Store all %d tons of %s. Only enough room for %d", count, commodity.c_str(), cargoSpaceRemaining);
		count = cargoSpaceRemaining;
	}

	// Store this cargo with similar cargo
	map<Commodity*,unsigned int>::iterator iter = commodities.find(com);
	if(iter!=commodities.end()){
		iter->second += count;
	} else {
		commodities[com] = count;
	}

	status.cargoSpaceUsed+=count;
	return count;
}

/**\brief Discard a number of tons of a commodity
 */
int Ship::DiscardCommodities(string commodity, unsigned int count) {
	Commodity* com = Commodities::Instance()->GetCommodity(commodity);

	LogMsg(INFO, "Discarding %d tons of %s.", count, commodity.c_str());

	// Ensure that we have some of this cargo
	map<Commodity*,unsigned int>::iterator iter = commodities.find(com);
	if(iter==commodities.end()){
		LogMsg(INFO, "This ship does not have any %s to discard", commodity.c_str());
		return 0;
	}

	// Never discard more cargo than exists
	if( iter->second < count ){
		LogMsg(INFO, "Cannot Discard all %d tons of %s. Only has %d tons.", count, commodity.c_str(), iter->second);
		count = iter->second;
	}

	// Remove this many tons of cargo
	iter->second -= count;
	status.cargoSpaceUsed-=count;
	LogMsg(INFO, "Discarding %d tons of %s. %d tons remaining.", count, commodity.c_str(), iter->second);

	if( iter->second == 0 ) {
		commodities.erase(com);
	}

	return count;
}

/**\brief Get angle to rotate towards target.
 * \param target Coordinate of target
 * \return angle towards target
 * \sa directionTowards(float)
 */
float Ship::GetDirectionTowards(Coordinate target){
	float theta;
	//Trig *trig = Trig::Instance();
	Coordinate position = target - GetWorldPosition();

	theta = position.GetAngle();//trig->RadToDeg(atan2( - position.GetY(), position.GetX()));
	//LogMsg(INFO,"Angle towards target (%f,%f) is %f.",target.GetX(),target.GetY(),theta);
	//LogMsg(INFO,"Current Angle %f",this->GetAngle());
	return GetDirectionTowards(theta);
}

/**\brief Returns the best direction to turn in order to aim in a certain direction.
 * \param angle Angle of target
 * \return angle towards target
 */
float Ship::GetDirectionTowards(float angle){
	return normalizeAngle(angle - this->GetAngle());
}

/**\brief Returns the ship's shield integrity as a percentage (0.0-1.0, where 1.0 = 100%).
 * \return Hull remaining
 */
float Ship::GetHullIntegrityPct() {
	float remaining =  ( (float)shipStats.GetHullStrength() - (float)status.hullDamage ) / (float)shipStats.GetHullStrength();
	return( remaining > 0.0f ? remaining : 0.0f );
}

/**\brief Returns the ship's shield integrity as a percentage (0.0-1.0, where 1.0 = 100%).
 * \return Shield remaining
 */
float Ship::GetShieldIntegrityPct() {
	float remaining =  ( ((float)shipStats.GetShieldStrength() * status.shieldBooster) - (float)status.shieldDamage ) / ((float)shipStats.GetShieldStrength() * status.shieldBooster);
	return( remaining > 0.0f ? remaining : 0.0f );
}

/* Note:
 *
 * There used to be functions called GetCurrentWeapon() and GetCurrentAmmo(),
 * but these functions don't make much sense anymore now that firing groups
 * are used instead of individual weapons. A GetCurrentFiringGroup() function
 * might be a good thing, though.
 *
 */


/**\brief Gets the ammo of a certain type.
 * \return Integer count of ammo
 */
int Ship::GetAmmo(AmmoType type) {
	assert(type<max_ammo);
	return ammo[type];
}

/**\brief Gets a std::map of the current weapon system
 * \return std:map with pointer to weapon as the key, ammo quantity as the data
 */
map<Weapon*,int> Ship::GetWeaponsAndAmmo() {
	map<Weapon*,int> weaponPack;
	Weapon* thisWeapon;
	for(unsigned int i = 0; i < weaponSlots.size(); i++){
		thisWeapon = weaponSlots[i].content;
		if(thisWeapon != NULL){
			weaponPack.insert( make_pair(thisWeapon,ammo[thisWeapon->GetAmmoType()]) );
		}
	}
	return weaponPack;
}


/**\brief Computes the Ship Statistics based on equiped Outfit.
 */
void Ship::ComputeShipStats() {
	// Start with an empty Outfit
	shipStats = Outfit();

	// Add the single Outfits
	// Since there it is possible that a ship doesn't have a model or engine,
	// only add them if they exist.
	if(model){ shipStats += *model; }
	if(engine){ shipStats += *engine; }

	// Add any Outfit Collections
	for(unsigned int i=0; i<shipWeapons.size(); i++){
		shipStats += *shipWeapons[i];
	}

	list<Outfit*>::iterator iter;
	for(iter=outfits.begin(); iter!=outfits.end(); ++iter)
	{
		shipStats += *(*iter);
	}
}

/**\brief The total number of weapon slots on this ship
 */
int Ship::GetWeaponSlotCount() {
	return this->weaponSlots.size();
}

/**\brief The name of weapon slot i
 */
string Ship::GetWeaponSlotName(int i) {
	return ((WeaponSlot)(this->weaponSlots[i])).name;
}

/**\brief The status of weapon slot i.
 * \note The "status" naming of this function has nothing to do with statusbars.
 */
string Ship::GetWeaponSlotContent(int i) {
	if( weaponSlots[i].content == NULL) {
		return "";
	} else {
		return weaponSlots[i].content->GetName().c_str();
	}
}

/**\brief Set the status of weapon slot i
 */
void Ship::SetWeaponSlotContent(int i, Weapon *w) {
	weaponSlots[i].content = w;
}

/**\brief The firing group of weapon slot i
 */
short int Ship::GetWeaponSlotFG(int i) {
	return weaponSlots[i].firingGroup;
}

/**\brief Set the firing group of weapon slot i
 */
void Ship::SetWeaponSlotFG(int i, short int fg) {
	weaponSlots[i].firingGroup = fg;
}

/**\brief returns a map<string,string> of with slotname/content pairs for use in Lua
 */
map<string,string> Ship::GetWeaponSlotContents(){

	map<string,string> weaps;

	for(unsigned int i = 0; i < weaponSlots.size(); i++){
		if( weaponSlots[i].content != NULL )
			weaps[ GetWeaponSlotName(i) ] = GetWeaponSlotContent(i);
	}

	return weaps;
}

/** @} */

