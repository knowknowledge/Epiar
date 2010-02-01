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
#include "Utilities/timer.h"
#include "Utilities/trig.h"
#include "Sprites/spritemanager.h"
#include "Utilities/xml.h"


/**\class Ship
 * \brief Ship handling. */

/**\brief Ship constructor that initializes default values.
 */
Ship::Ship() {
	model = NULL;
	flareAnimation = NULL;
	
	/* Initalize ship's condition */
	status.hullEnergyAbsorbed = 0;
	status.lastWeaponChangeAt = 0;
	status.lastFiredAt = 0;
	status.selectedWeapon = 0;

	SetRadarColor(Color::Get(255,0,0));
	SetAngle( float( rand() %360 ) );
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
		SetFlareAnimation( model->GetFlareAnimation() );
		
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

/**\brief Rotates the ship in the given direction (relative angle).
 * \param direction Relative angle to rotate by
 * \sa Model::GetRotationsperSecond()
 */
void Ship::Rotate( float direction ) {
	float rotPerSecond, timerDelta, maxturning;
	float angle = GetAngle();
	
	if( !model ) {
		Log::Warning( "Attempt to rotate sprite with no model." );
		return;
	}

	// Compute the maximum amount that the ship can turn
	rotPerSecond = model->GetRotationsPerSecond();
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
	float speed = model->GetMaxSpeed();

	momentum += Coordinate( trig->GetCos( angle ) * model->GetAcceleration() * Timer::GetDelta(),
	                         -1 * trig->GetSin( angle ) * model->GetAcceleration() * Timer::GetDelta() );

	momentum.EnforceBoundaries(speed,speed,speed,speed);
	
	SetMomentum( momentum );
	
	status.isAccelerating = true;
}


/**\brief Adds damage to hull.
 */
void Ship::Damage(short int damage) {
	status.hullEnergyAbsorbed += damage;
}

/**\brief Update function on every frame.
 */
void Ship::Update( void ) {
	Sprite::Update(); // update momentum and other generic sprite attributes
	
	if( status.isAccelerating == false ) {
		flareAnimation->Reset();
	}
	flareAnimation->Update();

	// Shiw the hits taken as part of the radar color
	SetRadarColor(Color::Get(int(255 *getHullIntegrityPct()),0 ,0));
	
	// Ship has taken as much damage as possible...
	// It Explodes!
	if( status.hullEnergyAbsorbed >=  (float)model->getMaxEnergyAbsorption() ) {
		SpriteManager *sprites = SpriteManager::Instance();
		sprites->Delete( (Sprite*)this );
	}
}

/**\brief Set's the engine flare animation.
 * \param filename A string to a new file
 */
bool Ship::SetFlareAnimation( string filename ) {
	if( flareAnimation )
		delete flareAnimation;
	
	flareAnimation = new Animation( filename );
	
	return true;
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
	else if( ammo.find(currentWeapon->GetAmmoType())->second < currentWeapon->GetAmmoConsumption() ) { 
		return FireNoAmmo;
	} else {
		//Calculate the offset needed by the ship to fire infront of the ship
		Trig *trig = Trig::Instance();
		float angle = static_cast<float>(trig->DegToRad( GetAngle() ));		
		Coordinate worldPosition  = GetWorldPosition();
		int offset = model->GetImage()->GetHalfHeight();
		worldPosition += Coordinate(trig->GetCos( angle ) * offset, -trig->GetSin( angle ) * offset);

		//Fire the weapon
		SpriteManager *sprites = SpriteManager::Instance();
		Projectile *projectile = new Projectile(GetAngle(), worldPosition, GetMomentum(), currentWeapon);
		projectile->SetOwnerID( this->GetID() );
		sprites->Add( (Sprite*)projectile );

		//track number of ticks the last fired occured
		status.lastFiredAt = Timer::GetTicks();
		//reduce ammo
		ammo.find(currentWeapon->GetAmmoType())->second -=  currentWeapon->GetAmmoConsumption();

		return FireSuccess;
	}
	
}

/**\brief Adds a new weapon to the ship.
 * \param i Pointer to Weapon instance
 * \sa Weapon
 */
void Ship::addShipWeapon(Weapon *i){
	shipWeapons.push_back(i);
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
 * \param weaponName String naming the weapon
 * \param qty Quantity to add
 */
void Ship::addAmmo(string weaponName, int qty){
	Weapons *weapons = Weapons::Instance();
	Weapon* currentWeapon = weapons->GetWeapon(weaponName);
	
	if (ammo.find(currentWeapon->GetAmmoType()) == ammo.end() ) {
		ammo.insert ( pair<int,int>(currentWeapon->GetAmmoType(),qty) );
	} else {
		ammo.find(currentWeapon->GetAmmoType())->second += qty;
	}
	
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
	//Log::Message("Angle towards target (%f,%f) is %f.",target.GetX(),target.GetY(),theta);
	//Log::Message("Current Angle %f",this->GetAngle());
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
	float remaining =  ( (float)model->getMaxEnergyAbsorption() - (float)status.hullEnergyAbsorbed ) / (float)model->getMaxEnergyAbsorption();
	//Log::Message("Ship has taken %d damage out of %d possibile. %02f%% Remaining",status.hullEnergyAbsorbed,model->getMaxEnergyAbsorption(),remaining);
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
	return ammo.find(currentWeapon->GetAmmoType())->second;
}

/**\brief Gets a std::map of the current weapon system.
 * \return std:map with pointer to weapon as the key, ammo quantity as the data
 */
map<Weapon*,int> Ship::getWeaponsAndAmmo() {
	map<Weapon*,int> weaponPack;
	Weapon* thisWeapon;
	int thisAmmo;
	for(unsigned int i=0; i<shipWeapons.size(); i++){
		thisWeapon = this->shipWeapons[i];
		thisAmmo = this->ammo.find(thisWeapon->GetAmmoType())->second;
		weaponPack.insert( make_pair(thisWeapon,thisAmmo) );
	}
	return weaponPack;
}

