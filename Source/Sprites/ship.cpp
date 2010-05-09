/**\file			ship.cpp
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Friday, November 14, 2009
 * \brief
 * \details
 */

#include "includes.h"
#include "common.h"
#include "Sprites/ship.h"
#include "Utilities/camera.h"
#include "Utilities/timer.h"
#include "Utilities/trig.h"
#include "Sprites/spritemanager.h"
#include "Utilities/xml.h"
#include "Sprites/effects.h"
#include "Audio/sound.h"

#include "Engine/hud.h"

/**\class Ship
 * \brief Ship handling. */

/**\brief Ship constructor that initializes default values.
 */
Ship::Ship() : nonplayersound( 0.4f )
{
	model = NULL;
	engine = NULL;
	flareAnimation = NULL;
	
	/* Initalize ship's condition */
	status.hullDamage = 0;
	status.lastWeaponChangeAt = 0;
	status.lastFiredAt = 0;
	status.selectedWeapon = 0;
	status.cargoSpaceUsed = 0;
	status.isAccelerating = false;
	for(int a=0;a<max_ammo;a++){
		ammo[a]=0;
	}

	shipStats = Outfit();

	SetRadarColor(Color::Get(255,0,0));
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
bool Ship::SetModel( Model *model ) {
	if( model ) {
		this->model = model;
		
		SetImage( model->GetImage() );

		ComputeShipStats();
		
		return( true );
	}
	
	return( false );
}

/**\brief Sets the ship engine.
 * \param Engine Engine to use
 * \return true if successful
 * \sa Engine
 */
bool Ship::SetEngine( Engine *engine ) {
	assert( engine );
	{
		this->engine = engine;
		
		// Creates a new Flare Animation specific for this Ship
		if( flareAnimation )
			delete flareAnimation;
		flareAnimation = new Animation( engine->GetFlareAnimation() );
		flareAnimation->Reset();

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

	// Compute the maximum amount that the ship can turn
	rotPerSecond = shipStats.GetRotationsPerSecond();
	timerDelta = Timer::GetDelta();
	maxturning = static_cast<float>((rotPerSecond * timerDelta) * 360.);

	// Cap the ship rotation
	if (fabs(direction) > maxturning){
		if (direction > 0 )
			angle += maxturning;
		else
			angle -= maxturning;
	} else {
		angle += direction;
	}
	
	// Normalize
	angle = normalizeAngle(angle);
	
	SetAngle( angle );
}

/**\brief Accelerates the ship.
 * \sa Model::GetAcceleration
 */
void Ship::Accelerate( void ) {
	Trig *trig = Trig::Instance();
	Coordinate momentum = GetMomentum();
	float angle = static_cast<float>(trig->DegToRad( GetAngle() ));
	float speed = shipStats.GetMaxSpeed();

	float acceleration = shipStats.GetForceOutput() / shipStats.GetMass();

	momentum += Coordinate( trig->GetCos( angle ) * acceleration * Timer::GetDelta(),
	                   -1 * trig->GetSin( angle ) * acceleration * Timer::GetDelta() );

	momentum.EnforceMagnitude(speed);
	
	SetMomentum( momentum );
	
	status.isAccelerating = true;
	// Play engine sound
	float engvol = OPTION(float,"options/sound/engines");
	Coordinate offset = GetWorldPosition() - Camera::Instance()->GetFocusCoordinate();
	if ( this->GetDrawOrder() == DRAW_ORDER_SHIP )
		engvol = engvol * this->nonplayersound;
	this->engine->thrustsound->SetVolume( engvol );
	this->engine->thrustsound->PlayNoRestart( offset );
}


/**\brief Adds damage to hull.
 */
void Ship::Damage(short int damage) {
	status.hullDamage += damage;
}

/**\brief Repairs the ship.
 */
void Ship::Repair(short int damage) {
	status.hullDamage -= damage;
	if( status.hullDamage<0 )
		status.hullDamage=0;
}

/**\brief Update function on every frame.
 */
void Ship::Update( void ) {
	Sprite::Update(); // update momentum and other generic sprite attributes
	
	if( status.isAccelerating == false ) {
		flareAnimation->Reset();
	}
	flareAnimation->Update();

	// Show the hits taken as part of the radar color
	SetRadarColor(Color::Get(int(255 *getHullIntegrityPct()),0 ,0));
	
	// Ship has taken as much damage as possible...
	// It Explodes!
	if( status.hullDamage >=  (float)shipStats.GetHullStrength() ) {
		SpriteManager *sprites = SpriteManager::Instance();

		// Play explode sound
		if(OPTION(int, "options/sound/explosions")) {
			Sound *explodesnd = Sound::Get("Resources/Audio/Effects/18384__inferno__largex.wav.ogg");
			explodesnd->Play(
				this->GetWorldPosition() - Camera::Instance()->GetFocusCoordinate());
		}

		// Create Explosion
		sprites->Add(
			new Effect(this->GetWorldPosition(), "Resources/Animations/explosion1.ani", 0) );

		// Remove this Sprite from the
		sprites->Delete( (Sprite*)this );
	}
}

/**\brief Draw function.
 * \sa Sprite::Draw()
 */
void Ship::Draw( void ) {
	Trig *trig = Trig::Instance();
	
	Sprite::Draw();
	
	// Draw the flare animation, if required
	if( status.isAccelerating ) {
		Coordinate position = GetWorldPosition();
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
}

/**\brief Fire's ship current weapon.
 * \return FireStatus
 */
FireStatus Ship::Fire() {
	// Check  that some weapon is attached
	if ( shipWeapons.empty() ) {
		return FireNoWeapons;
	}

	// Check that we are always selecting a real weapon
	assert( (status.selectedWeapon>=0 && status.selectedWeapon < shipWeapons.size() ) );

	Weapon* currentWeapon = shipWeapons.at(status.selectedWeapon);
	// Check that the weapon has cooled down;
	if( !( (int)(currentWeapon->GetFireDelay()) < (int)(Timer::GetTicks() - status.lastFiredAt)) ) {
		return FireNotReady;
	}
	// Check that there is sufficient ammo
	else if( ammo[currentWeapon->GetAmmoType()] < currentWeapon->GetAmmoConsumption() ) {
		return FireNoAmmo;
	} else {
		//Calculate the offset needed by the ship to fire infront of the ship
		Trig *trig = Trig::Instance();
		float angle = static_cast<float>(trig->DegToRad( GetAngle() ));		
		Coordinate worldPosition  = GetWorldPosition();
		int offset = model->GetImage()->GetHalfHeight();
		worldPosition += Coordinate(trig->GetCos( angle ) * offset, -trig->GetSin( angle ) * offset);

		//Play weapon sound
		float weapvol = OPTION(float,"options/sound/weapons");
		if ( this->GetDrawOrder() == DRAW_ORDER_SHIP )
			currentWeapon->sound->SetVolume( weapvol * this->nonplayersound );
		else
			currentWeapon->sound->SetVolume( weapvol );
		currentWeapon->sound->Play(
			GetWorldPosition() - Camera::Instance()->GetFocusCoordinate() );


		//Fire the weapon
		SpriteManager *sprites = SpriteManager::Instance();
		Projectile *projectile = new Projectile(GetAngle(), worldPosition, GetMomentum(), currentWeapon);
		projectile->SetOwnerID( this->GetID() );
		if( this->GetDrawOrder() & DRAW_ORDER_PLAYER ) { // TEMP Targeting
			projectile->SetTargetID( Hud::GetTarget() );
		}
		sprites->Add( (Sprite*)projectile );

		//track number of ticks the last fired occured
		status.lastFiredAt = Timer::GetTicks();
		//reduce ammo
		ammo[currentWeapon->GetAmmoType()] -=  currentWeapon->GetAmmoConsumption();

		return FireSuccess;
	}
	
}

/**\brief Adds a new weapon to the ship.
 * \param i Pointer to Weapon instance
 * \sa Weapon
 */
void Ship::addShipWeapon(Weapon *i){
	shipWeapons.push_back(i);

	ComputeShipStats();
}

/**\brief Adds a new weapon to the ship by name.
 * \param weaponName Name of the Weapon
 * \sa Weapon
 */
void Ship::addShipWeapon(string weaponName){
	Weapons *weapons = Weapons::Instance();
	addShipWeapon(weapons->GetWeapon(weaponName));	
}

/**\brief Changes the ship's weapon.
 * \return true if successful.
 */
bool Ship::ChangeWeapon() {
	if (shipWeapons.size() && (250 < Timer::GetTicks() - status.lastWeaponChangeAt)){
		status.selectedWeapon = (status.selectedWeapon+1)%shipWeapons.size();
		return true;
	}
	return false;
}

/**\brief Removes a weapon from the ship.
 * \param pos Index of the weapon
 */
void Ship::removeShipWeapon(int pos){
	shipWeapons.erase(shipWeapons.begin()+pos);
}

/**\brief Adds ammo to the ship.
 * \param AmmoType Type of ammo that should be added.
 * \param qty Quantity to add
 */
void Ship::addAmmo(AmmoType ammoType, int qty){
	ammo[ammoType] += qty;
}

/**\brief Set the number of credits
 */
void Ship::SetCredits(unsigned int _credits) {
	credits = _credits;
}

/**\brief Set the number of credits
 */
map<Commodity*,unsigned int> Ship::getCargo() {
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

	if( count == iter->second ) {
		commodities.erase(com);
	}

	return count;
}

/**\brief Get angle to rotate towards target.
 * \param target Coordinate of target
 * \return angle towards target
 * \sa directionTowards(float)
 */
float Ship::directionTowards(Coordinate target){
	float theta;
	//Trig *trig = Trig::Instance();
	Coordinate position = target - GetWorldPosition();

	theta = position.GetAngle();//trig->RadToDeg(atan2( - position.GetY(), position.GetX()));
	//LogMsg(INFO,"Angle towards target (%f,%f) is %f.",target.GetX(),target.GetY(),theta);
	//LogMsg(INFO,"Current Angle %f",this->GetAngle());
	return this->directionTowards(theta);
}

/**\brief Returns the best direction to turn in order to aim in a certain direction.
 * \param angle Angle of target
 * \return angle towards target
 */
float Ship::directionTowards(float angle){
	return normalizeAngle(angle - this->GetAngle());
}

/**\brief Returns the ship's integrity as a percentage (0.0-1.0, where 1.0 = 100%).
 * \return Hull remaining
 */
float Ship::getHullIntegrityPct() {
	assert( model );
	float remaining =  ( (float)shipStats.GetHullStrength() - (float)status.hullDamage ) / (float)shipStats.GetHullStrength();
	return(remaining);
}

/**\brief Gets the current weapon.
 * \return Pointer to Weapon object.
 */
Weapon* Ship::getCurrentWeapon() {
	if(shipWeapons.size()==0) return (Weapon*)NULL;
	return shipWeapons.at(status.selectedWeapon);
}

/**\brief Gets the current ammo left.
 * \return Integer count of ammo
 */
int Ship::getCurrentAmmo() {
	if(shipWeapons.size()==0) return 0;
	Weapon* currentWeapon = shipWeapons.at(status.selectedWeapon);
	return ammo[currentWeapon->GetAmmoType()];
}

/**\brief Gets the ammo of a certain type.
 * \return Integer count of ammo
 */
int Ship::getAmmo(AmmoType type) {
	assert(type<max_ammo);
	return ammo[type];
}

/**\brief Gets a std::map of the current weapon system.
 * \return std:map with pointer to weapon as the key, ammo quantity as the data
 */
map<Weapon*,int> Ship::getWeaponsAndAmmo() {
	map<Weapon*,int> weaponPack;
	Weapon* thisWeapon;
	for(unsigned int i=0; i<shipWeapons.size(); i++){
		thisWeapon = this->shipWeapons[i];
		weaponPack.insert( make_pair(thisWeapon,ammo[thisWeapon->GetAmmoType()]) );
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
}

