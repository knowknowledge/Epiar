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
#include "Utilities/camera.h"
#include "Utilities/timer.h"
#include "Utilities/trig.h"
#include "Sprites/spritemanager.h"
#include "Utilities/xml.h"
#include "Sprites/effects.h"
#include "Audio/sound.h"
#include "Engine/hud.h"

#define NON_PLAYER_SOUND_RATIO 0.4f ///< Ratio used to quiet NON-PLAYER Ship Sounds.

/**\class Ship
 * \brief A Ship Sprite that moves, Fires Weapons, has cargo, and ultimately explodes.
 * \sa Player, AI
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
	damageBooster=1.0;
	engineBooster=1.0;
	shieldBooster=1.0;
	status.hullDamage = 0;
	status.shieldDamage = 0;
	status.lastWeaponChangeAt = 0;
	//status.lastFiredAt = 0;
	status.selectedWeapon = 0;
	status.cargoSpaceUsed = 0;
	status.isAccelerating = false;
	status.isDisabled = false;
	for(int a=0;a<max_ammo;a++){
		ammo[a]=0;
	}

	attackedBy = -1; 

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
bool Ship::SetModel( Model *model) {
	assert( model );
	if( model ) {
		this->model = model;

		// Copy default weapon slot arrangement from model,
		this->weaponSlots = model->GetWeaponSlots();
		// but leave it up to Lua to decide how the default weapon
		// slot arrangement influences the weapons onboard the ship.

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
	if( engine ){
		this->engine = engine;
		
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
	float speed = shipStats.GetMaxSpeed()*engineBooster;

	float acceleration = (shipStats.GetForceOutput() *engineBooster ) / shipStats.GetMass();

	momentum += Coordinate( trig->GetCos( angle ) * acceleration * Timer::GetDelta(),
	                -1 * trig->GetSin( angle ) * acceleration * Timer::GetDelta() );

	momentum.EnforceMagnitude(speed);
	
	SetMomentum( momentum );
	
	status.isAccelerating = true;
	// Play engine sound
	float engvol = OPTION(float,"options/sound/engines");
	Coordinate offset = GetWorldPosition() - Camera::Instance()->GetFocusCoordinate();
	if ( this->GetDrawOrder() == DRAW_ORDER_SHIP )
		engvol = engvol * NON_PLAYER_SOUND_RATIO ;
	this->engine->GetSound()->SetVolume( engvol );
	this->engine->GetSound()->PlayNoRestart( offset );
}


/**\brief Adds damage to hull.
 */
void Ship::Damage(short int damage) {
	if(status.shieldDamage >=   ((float)shipStats.GetShieldStrength()) * shieldBooster)
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

/**\brief Update function on every frame.
 */
void Ship::Update( void ) {
	Sprite::Update(); // update momentum and other generic sprite attributes
	
	if( status.isAccelerating == false ) {
		flareAnimation->Reset();
	}
	flareAnimation->Update();
	Coordinate momentum	= GetMomentum();
	momentum.EnforceMagnitude( shipStats.GetMaxSpeed()*engineBooster );
	// Show the hits taken as part of the radar color
	if(IsDisabled()) SetRadarColor( Color::Get( 128, 128, 128 ) );
	else SetRadarColor( Color::Get(int(255 *GetHullIntegrityPct()), 0, 0) );
	
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

		// Remove this Sprite from the SpriteManager
		sprites->Delete( (Sprite*)this );
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
}

/**\brief Fire's ship current weapon.
 * \return FireStatus
 */
FireStatus Ship::Fire( int target ) {
	// Check  that some weapon is attached
	if ( shipWeapons.empty() ) {
		return FireNoWeapons;
	}

	// Check that we are always selecting either the primary or the secondary firing group
	assert( status.selectedWeapon == 0 || status.selectedWeapon == 1 );

	Weapons *weapons = Weapons::Instance();

	bool fnr = false;
	bool fna = false;
	bool emptySlot = false;
	bool fired = false;

	float randAngleVariation = 0.0;

	//for(int weap = 0; weap < shipWeapons.size(); weap++){
	for(int slot = 0; slot < weaponSlots.size(); slot++){

		string weapName = weaponSlots[slot].content;
		short int slotFiringGroup = weaponSlots[slot].firingGroup;

		if(weapName == ""){
			cout << "can't fire an empty weapon slot! Therefore, " << endl;
		}

		Weapon* currentWeapon = NULL;
		for(int weap = 0; weap < shipWeapons.size(); weap++){ // inefficient
			if(shipWeapons[weap]->GetName() == weapName){
				currentWeapon = shipWeapons.at(weap);
			}
		}

		if(currentWeapon == NULL){
			cout << "currentWeapon is NULL; this may be a problem." << endl;
			cout << "\twanted to fire slot" << slot << ", and I count " << shipWeapons.size() << " installed weapons" << endl;
		}
		else {
			cout << "\n\ncurrentWeapon is " << currentWeapon << endl;
			cout << "slot firing group is " << slotFiringGroup << " and status.selectedWeapon is " << status.selectedWeapon << "; we want them to be equal" << endl;

			if( slotFiringGroup == status.selectedWeapon ){ // status.selectedWeapon now refers to the firing group

				// Check that the weapon has cooled down;
				if( !( (int)(currentWeapon->GetFireDelay()) < (int)(Timer::GetTicks() - status.lastFiredAt[slot])) ) {
					fnr = true;
				}
				// Check that there is sufficient ammo
				else if( ammo[currentWeapon->GetAmmoType()] < currentWeapon->GetAmmoConsumption() ) {
					fna = true;
				}
				else if(emptySlot){
					// do nothing
				} else {
					//Calculate the offset needed to fire at the position specified for this slot in the XML file
					Trig *trig = Trig::Instance();
					float angle = static_cast<float>(trig->DegToRad( GetAngle()));		
					Coordinate worldPosition  = GetWorldPosition();
					int y_offset = (int)(weaponSlots[slot].y);
					int x_offset = (int)(weaponSlots[slot].x);

					if(weaponSlots[slot].mode == "manual"){
						// adjust for y offset
						worldPosition += Coordinate(
							trig->GetCos( angle ) * y_offset,
							-trig->GetSin( angle ) * y_offset
						);

						// adjust for x offset
						worldPosition += Coordinate(
							trig->GetSin(angle) * x_offset,
							trig->GetCos(angle) * x_offset
						);
					}
					else if(weaponSlots[slot].mode == "auto"){
						int offset = model->GetImage()->GetHalfHeight();
						worldPosition += Coordinate(
							trig->GetCos( angle ) * offset,
							-trig->GetSin( angle ) * offset
						);
					}

					//Play weapon sound
					float weapvol = OPTION(float,"options/sound/weapons");
					if ( this->GetDrawOrder() == DRAW_ORDER_SHIP )
						currentWeapon->sound->SetVolume( weapvol * NON_PLAYER_SOUND_RATIO );
					else
						currentWeapon->sound->SetVolume( weapvol );
					currentWeapon->sound->Play(
						GetWorldPosition() - Camera::Instance()->GetFocusCoordinate() );

					// vary angle randomly 3 degrees in either direction
					srand(time(NULL) + slotFiringGroup + (int)randAngleVariation);
					randAngleVariation = (float)( (357+(rand()%6))%360 );

					//Fire the weapon
					SpriteManager *sprites = SpriteManager::Instance();
					Projectile *projectile = new Projectile(damageBooster, GetAngle() + randAngleVariation, worldPosition, GetMomentum(), currentWeapon);
					projectile->SetOwnerID( this->GetID() );
					projectile->SetTargetID( target );
					sprites->Add( (Sprite*)projectile );

					//reduce ammo
					ammo[currentWeapon->GetAmmoType()] -=  currentWeapon->GetAmmoConsumption();

					//track number of ticks the last fired occured for this weapon
					status.lastFiredAt[slot] = Timer::GetTicks();

					fired = true;
				}
			}
		}
	}

	//return FireNotReady;
	if(fired) return FireSuccess;
	if(fna) return FireNoAmmo;
	if(fnr) return FireNotReady;
}

/**\brief Adds a new weapon to the ship WITHOUT updating weaponSlots.
 * \param i Pointer to Weapon instance
 * \sa Weapon
 */
void Ship::AddShipWeapon(Weapon *w){
	shipWeapons.push_back(w);

	ComputeShipStats();
}


/**\brief Adds a new weapon to the ship by name.
 * \param weaponName Name of the Weapon
 * \sa Weapon
 */
void Ship::AddShipWeapon(string weaponName){
	Weapons *weapons = Weapons::Instance();
	if(weapons->GetWeapon(weaponName)){
		AddShipWeapon(weapons->GetWeapon(weaponName));
	} else {
		LogMsg(INFO, "Failed to add weapon '%s', it doesn't exist.", weaponName.c_str());
	}
}

/**\brief Adds a new weapon to the ship AND update weaponSlots.
 * \param i Pointer to Weapon instance
 * \sa Weapon
 */
void Ship::AddShipWeaponAndInstall(Weapon *w){
	AddShipWeapon(w);
	for(int s = 0; s < weaponSlots.size(); s++){
		struct Outfit::ws *slot = &weaponSlots[s];
		if(slot->content == ""){
			slot->content = w->GetName(); // this will edit-in-place, so no need to shove a struct back into weaponSlots
			cout << "Ship::AddShipWeaponAndInstall() calling WSDebug on the slot it chose for the " << w->GetName() << " ..." << endl;
			model->WSDebug(*slot);
			return;
		}
	}
	printf("BUG! AddShipWeaponAndInstall() was called, but there is not empty slot. Perhaps a weapon was sold without clearing its slot.\n");
}

/**\brief Adds a new weapon to the ship by name AND updates weaponSlots
 * \param weaponName Name of the Weapon
 * \sa Weapon
 */
void Ship::AddShipWeaponAndInstall(string weaponName){
	Weapons *weapons = Weapons::Instance();
	if(weapons->GetWeapon(weaponName)){
		AddShipWeaponAndInstall(weapons->GetWeapon(weaponName));
	} else {
		LogMsg(INFO, "Failed to add/install weapon '%s', it doesn't exist.", weaponName.c_str());
	}
}

/**\brief Changes the ship's weapon.
 * \return true if successful.
 */
bool Ship::ChangeWeapon() {
	// alternate between primary and secondary firing groups
	status.selectedWeapon = (status.selectedWeapon+1) % 2;
	status.selectedWeaponName = (status.selectedWeapon == 0 ? "Primary firing group" : "Secondary firing group");
	return true;
}

/**\brief Removes a weapon from the ship.
 * \param pos Index of the weapon
 */
void Ship::RemoveShipWeapon(int pos){
	cout << "Ship::RemoveWeapon(): removing weapon at pos=" << pos << endl;
	shipWeapons.erase(shipWeapons.begin()+pos);
}
/**\brief Removes a weapon from the ship
 * \param i Pointer to Weapon instance
 */
void Ship::RemoveShipWeapon(Weapon *i){
	for(unsigned int pos = 0; pos < shipWeapons.size(); pos++){
		if(shipWeapons[pos]->GetName() == i->GetName()){
			cout << "found weapon to remove, " << i->GetName() << ", at position " << pos << endl;
			RemoveShipWeapon(pos);
			return;
		}
	}
	printf("Ship::RemoveShipWeapon(): did not find weapon to remove\n");
}

/**\brief Removes a weapon from the ship
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


/**\brief Removes a weapon from the ship AND updates weaponSlots
 * \param i Pointer to Weapon instance
 */
void Ship::DeinstallShipWeaponAndRemove(Weapon *w){
	for(unsigned int pos = 0; pos < shipWeapons.size(); pos++){
		if(shipWeapons[pos]->GetName() == w->GetName()){
			cout << "found weapon to remove, " << w->GetName() << ", at position " << pos << endl;
			RemoveShipWeapon(pos);
		}
	}
	for(int s = 0; s < weaponSlots.size(); s++){
		struct Outfit::ws *slot = &weaponSlots[s];
		if(slot->content == w->GetName()){
			slot->content = ""; // this will edit-in-place, so no need to shove a struct back into weaponSlots
			cout << "Ship DSWAR calling WSDebug on the slot it chose for the " << w->GetName() << " ..." << endl;
			model->WSDebug(*slot);
			return;
		}
	}
}

/**\brief Removes a weapon from the ship AND updates weaponSlots
 * \param weaponName Name of the Weapon
 */
void Ship::DeinstallShipWeaponAndRemove(string weaponName){
	Weapons *weapons = Weapons::Instance();
	if(weapons->GetWeapon(weaponName)){
		DeinstallShipWeaponAndRemove(weapons->GetWeapon(weaponName));
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
	cout << "Ship::RemoveOutfit() was not able to remove the outfit!" << endl;
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
	float remaining =  ( ((float)shipStats.GetShieldStrength() * shieldBooster) - (float)status.shieldDamage ) / ((float)shipStats.GetShieldStrength() * shieldBooster);
	return( remaining > 0.0f ? remaining : 0.0f );
}

/* these functions no longer make sense now that weapon selection is done by firing group */

///**\brief Gets the current weapon.
// * \return Pointer to Weapon object.
// */
//Weapon* Ship::GetCurrentWeapon() {
//	if(shipWeapons.size()==0) return (Weapon*)NULL;
//	return shipWeapons.at(status.selectedWeapon);
//}
//
///**\brief Gets the current ammo left.
// * \return Integer count of ammo
// */
//int Ship::GetCurrentAmmo() {
//	if(shipWeapons.size()==0) return 0;
//	Weapon* currentWeapon = shipWeapons.at(status.selectedWeapon);
//	return ammo[currentWeapon->GetAmmoType()];
//}
//


/**\brief Gets the ammo of a certain type.
 * \return Integer count of ammo
 */
int Ship::GetAmmo(AmmoType type) {
	assert(type<max_ammo);
	return ammo[type];
}

/**\brief Gets a std::map of the current weapon system.
 * \return std:map with pointer to weapon as the key, ammo quantity as the data
 */
map<Weapon*,int> Ship::GetWeaponsAndAmmo() {
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

	list<Outfit*>::iterator iter;
	for(iter=outfits.begin(); iter!=outfits.end(); ++iter)
	{
		shipStats += *(*iter);
	}
}

/**\brief The total number of weapon slots on this ship
 */
int Ship::GetWeaponSlotCount() {
	cout << "calling Outfit::WSDebug() from Ship::GetWeaponSlotCount() ..." << endl;
	model->WSDebug(this->weaponSlots);
	return this->weaponSlots.size();
}

/**\brief The name of weapon slot i
 */
string Ship::GetWeaponSlotName(int i) {
	if(i >= weaponSlots.size())
		return "[BUG! slot does not exist]";
	return ((struct Outfit::ws)(this->weaponSlots[i])).name;
}

/**\brief The status of weapon slot i
 */
string Ship::GetWeaponSlotStatus(int i) {
	if(i >= weaponSlots.size())
		return "[BUG! slot does not exist]";
	return ((struct Outfit::ws)(this->weaponSlots[i])).content;
}

/**\brief Set the status of weapon slot i
 */
void Ship::SetWeaponSlotStatus(int i, string s) {
	if(i >= weaponSlots.size()){
		cout << "[BUG! slot does not exist]" << endl;
		return;
	}
	cout << "Ship SWSS calling WSDebug before and after for attempt to set content to " << s << endl;
	model->WSDebug(this->weaponSlots[i]);
	this->weaponSlots[i].content = s;
	model->WSDebug(this->weaponSlots[i]);
}

/**\brief The firing group of weapon slot i
 */
short int Ship::GetWeaponSlotFG(int i) {
	if(i >= weaponSlots.size())
		cout << "[BUG! slot does not exist]";
	return ((struct Outfit::ws)(this->weaponSlots[i])).firingGroup;
}

/**\brief Set the firing group of weapon slot i
 */
void Ship::SetWeaponSlotFG(int i, short int fg) {
	if(i >= weaponSlots.size())
		cout << "[BUG! slot does not exist]";
	this->weaponSlots[i].firingGroup = fg;
}


//list<Weapon*> Ship::GetWeaponSlotContents(){
map<string,string> Ship::GetWeaponSlotContents(){
	//Weapons *weapons = Weapons::Instance();

	map<string,string> weaps;

	for(int i = 0; i < weaponSlots.size(); i++){
		if(weaponSlots[i].content != ""){
			weaps.insert( make_pair(weaponSlots[i].name, weaponSlots[i].content) );
		}
		else {
			cout << "GWSC: slot is empty by default" << endl;
		}
	}

	cout << "We think the weap list from ship.cpp is okay" << endl;

	return weaps;
}

