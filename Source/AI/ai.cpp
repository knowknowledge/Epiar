/**\file			ai.cpp
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Saturday, January 5, 2008
 * \brief
 * \details
 */

#include "includes.h"
#include "common.h"
#include "AI/ai.h"
#include "Sprites/player.h"
#include "Sprites/spritemanager.h"
#include "Utilities/lua.h"

/**\class AI
 * \brief AI controls the non-player shipts.
 *
 * */

/** \brief AI Constructor
 */

AI::AI(string _name, string machine) :
	name(_name),
	stateMachine(machine),
	state("default"),
	allegiance(NULL)
{
	target=0;
}

/** \brief Run the Lua Statemachine to act and possibly change state.
 */

void AI::Decide() {
	string newstate;
	// Decide
	lua_State *L = Lua::CurrentState();
	const int initialStackTop = lua_gettop(L);

	// Get the current state machine
	lua_getglobal(L, stateMachine.c_str() );
	int machineIndex = lua_gettop(L);
	if( ! lua_istable(L, machineIndex) )
	{
		LogMsg(ERR, "There is no State Machine named '%s'!", stateMachine.c_str() );
		return; // This ship will just sit idle...
	}

	// Get the current state
	lua_pushstring(L, state.c_str() );
	lua_gettable(L, machineIndex);
	if( ! lua_isfunction(L,lua_gettop(L)) )
	{
		LogMsg(WARN, "The State Machine '%s' has no state '%s'.", stateMachine.c_str(), state.c_str() );
		lua_getglobal(L, stateMachine.c_str() );
		lua_pushstring(L, "default" );
		lua_gettable(L,machineIndex);
		if( !lua_isfunction(L,lua_gettop(L)) )
		{
			LogMsg(ERR, "The State Machine '%s' has no default state.", stateMachine.c_str() );
			lua_settop(L, initialStackTop);
			return; // This ship will just sit idle...
		}
	}

	// Push Current AI Variables
	lua_pushinteger( L, this->GetID() );
	lua_pushnumber( L, this->GetWorldPosition().GetX() );
	lua_pushnumber( L, this->GetWorldPosition().GetY() );
	lua_pushnumber( L, this->GetAngle() );
	lua_pushnumber( L, this->GetMomentum().GetMagnitude() ); // Speed
	lua_pushnumber( L, this->GetMomentum().GetAngle() ); // Vector

	// Run the current AI state
	//printf("Call:"); Lua::stackDump(L); // DEBUG
	if( lua_pcall(L, 6, 1, 0) != 0)
	{
		LogMsg(ERR,"Failed to run %s(%s): %s\n", stateMachine.c_str(), state.c_str(), lua_tostring(L, -1));
		lua_settop(L, initialStackTop);
		return;
	}
	//printf("Return:"); Lua::stackDump(L); // DEBUG

	if( lua_isstring( L, lua_gettop(L) ) )
	{
		newstate = (string)luaL_checkstring(L, lua_gettop(L));

		// Verify that this new state exists
		lua_pushstring(L, newstate.c_str() );
		lua_gettable(L,machineIndex);
		if( lua_isfunction(L, lua_gettop(L) ))
		{
			state = newstate;
		} else {
			LogMsg(ERR, "The State Machine '%s' has no state '%s'. Could not transition from '%s'. Resetting StateMachine.", stateMachine.c_str(), newstate.c_str(), state.c_str() );
			state = "default"; // Reset the state
		}
		//printf("Changing State:"); Lua::stackDump(L); // DEBUG
	}

	//printf("Complete:");Lua::stackDump(L); // DEBUG
	lua_settop(L,initialStackTop);
}

/**\brief Updates the AI controlled ship by first calling the Lua function
 * and then calling Ship::Update()
 */
void AI::Update() {
	//Update enemies
	int t;
	if(enemies.size()>0){
		t=ChooseTarget();
		if(t!=-1){
			target=t;
			RegisterTarget(t);
		}
	}
	if( !this->IsDisabled() ) {
		this->Decide();
	}

	// Now act like a normal ship
	this->Ship::Update();
}

/**\brief Draw the AI Ship, and possibly debugging information.
 *
 * When the "options/development/debug-ai" flag is set, this will display the
 * current stateMachine and state below the Ship Spite.
 *
 * \see OPTION
 */
void AI::Draw(){
	this->Ship::Draw();
	if( OPTION(int,"options/development/debug-ai") ) {
		Coordinate position = this->GetWorldPosition();
		SansSerif->SetColor( WHITE );
		SansSerif->Render(position.GetScreenX(),position.GetScreenY()+GetImage()->GetHalfHeight(),stateMachine);
		SansSerif->Render(position.GetScreenX(),position.GetScreenY()+GetImage()->GetHalfHeight()+20,state);
	}
}

bool EnemyComp(enemy a, enemy b){return(a.id<b.id);}	

/**\brief chooses who the AI should target given the list of the AI's enemies
 *
 */


int AI::ChooseTarget(){
	//printf("choosing target\n");
	SpriteManager *sprites=SpriteManager::Instance();
	list<Sprite*> *nearbySprites = sprites->GetSpritesNear(this->GetWorldPosition(), COMBAT_RANGE, DRAW_ORDER_SHIP);
	
	
	nearbySprites->sort(CompAI);
	list<Sprite*>::iterator it;
	list<enemy>::iterator enemyIt=enemies.begin();
	//printf("printing list of enemies\n");
	//printf("the size of enemies = %d\n", enemies.size() );
	for(enemyIt=enemies.begin(); enemyIt!=enemies.end();){
		if(sprites->GetSpriteByID( enemyIt->id)==NULL){
			enemyIt=enemies.erase(enemyIt);
		}
		else {
			enemyIt++;
		}

	}
	
	//printf("printing list of nearby it->GetTarget()\n");
	//int nearbySpritesSize=(*nearbySprites).size();
	//printf("the size of nearbySprites = %d\n",nearbySpritesSize);
	/*for(it=nearbySprites->begin(); it!=nearbySprites->end(); it++){
		if( (*it)->GetDrawOrder() ==DRAW_ORDER_SHIP )
			printf("it->GetTarget() = %d\n",((AI*)(*it))->GetTarget() );
		printf("it->GetID() = %d\n",(*it)->GetID() );

	}*/
	it=nearbySprites->begin();
	enemyIt=enemies.begin();
	int max=0,currTarget=-1;
	int threat=0;
	//printf("starting sprite iteration\n");
		
	for(it= nearbySprites->begin(); it!=nearbySprites->end() && enemyIt!=enemies.end() ; it++){
		if( (*it)->GetID()== this->GetID() )
			continue;


		if( (*it)->GetDrawOrder() ==DRAW_ORDER_SHIP){

			if( enemyIt->id < ((AI*) (*it))->GetTarget() ){
				//printf("starting enemyIt iteration\n");
				while ( enemyIt!=enemies.end() && enemyIt->id < ( (AI*) (*it) )->GetTarget() ) {
				//	printf("enemyIt = %d\n",enemyIt->id);
				//	printf("it->GetTarget() = %d\n", ((AI*) (*it))->GetTarget() );
					if ( !InRange( sprites->GetSpriteByID(enemyIt->id)->GetWorldPosition() , this->GetWorldPosition() ) ) {
						enemyIt=enemies.erase(enemyIt);
						threat=0;
						continue;
					}
					int cost= CalcCost( threat + ( (Ship*)sprites->GetSpriteByID(enemyIt->id))->GetTotalCost() , enemyIt->damage); //damage might need to be scaled so that the damage can be adquately compared to the treat level
					if(currTarget==-1 || max<cost){
						currTarget=enemyIt->id;
						max=cost;
					}
					threat=0;
					enemyIt++;
				}
				//printf("successfully completed enemyIt iteration\n");
				it--;
				continue;
			}
			if( enemyIt->id == ((AI*) (*it))->GetTarget() )
				threat-= ( (Ship*)(*it) )->GetTotalCost();
		}
		else{
			LogMsg( ERR, "Error Sprite %d is not an AI\n", (*it)->GetID() );
		}
			
	}
//	printf("finished sprite iteration\n");

//	cout<<"enemies.size()="<<enemies.size()<<'\n';	
	while (enemyIt!=enemies.end()) {
		//printf("starting enemyIt iteration mark2\n");
		if ( !InRange( sprites->GetSpriteByID(enemyIt->id)->GetWorldPosition() , this->GetWorldPosition() ) ) {
			enemyIt=enemies.erase(enemyIt);
			threat=0;
			continue;			
		}
		//printf("finished In range check\n");
		//printf("calculate cost\n");
		int cost= CalcCost( threat + ( (Ship*)sprites->GetSpriteByID(enemyIt->id))->GetTotalCost() , enemyIt->damage); //damage might need to be scaled so that the damage can be adquately compared to the treat level
		threat=0;
		//printf("finished calculating cost\n");
		if( currTarget==-1 || max < cost){
			max=cost;
			currTarget=enemyIt->id;
		}
		//printf("successfully completed enemyIt iteration mark 2\n");

		enemyIt++;
	}
	//printf("finished choosing target.  %d is the target\n",currTarget);
	return currTarget;
	
}

/**\brief determines the potenital of an enemy as a target
 *
 */
int AI::CalcCost( int threat, int damage){
	return( threat * (damage +1) ); //damage might need to be scaled so that the damage can be adquately compared to the threat level
}

/**\brief sets the AI's target
 *
 */
void AI::SetTarget(int t){
	AddEnemy(t,0);
	target=t;
}

/**\brief Adds an enemy to the AI's list of enemies
 *
 */
void AI::AddEnemy(int e, int damage){
	//printf("Adding Enemy %d with damage %d\n",e,damage);
	Sprite *spr = SpriteManager::Instance()->GetSpriteByID(e);
	if(!spr){
		this->RemoveEnemy(e);
		return;
	}
	int drawOrder = spr->GetDrawOrder();
	if( !(drawOrder==DRAW_ORDER_SHIP || drawOrder == DRAW_ORDER_PLAYER) ){
		//printf("Error %d is not a ship/player\n",e);
		return;
	}
//	printf("Adding Enemy %d with damage %d\n",e,damage);
	enemy newE;
	newE.id=e;
	newE.damage=damage;
	list<enemy>::iterator it=lower_bound(enemies.begin() , enemies.end(), newE, EnemyComp);
	if( (*it).id==e )
		(*it).damage+=damage;
	else
		enemies.insert(it,newE);
	//printf("list of enemies:\n");
	bool success=false;
	for(it=enemies.begin(); it!=enemies.end(); it++){
		//printf("%d\n",it->id);
		if(it->id ==e)
			success=true;
	}
	assert(success);
	//printf("Successfully Added Enemy %d with damage %d\n",e,damage);
}

//removes an enemy from the AI's list of enemies
void AI::RemoveEnemy(int e){
	//printf("removing enemy %d\n",e);
	enemy newE;
	newE.id=e;
	list<enemy>::iterator it=lower_bound(enemies.begin(),enemies.end(),newE,EnemyComp);
	if( (*it).id == e)
		enemies.erase(it);
	for(it=enemies.begin(); it!=enemies.end(); it++){
		//printf("%d\n",it->id);
		assert(e!= it->id);
	}
	//printf("successfully removed %d\n",e);
}

/**\brief sets the AI to hunt the current target using the lua function setHuntHostile
*
*/
void AI::RegisterTarget(int t){
	//printf("Registering target %d\n",t);
	lua_State *L = Lua::CurrentState();
	lua_getglobal(L, "setHuntHostile" );
	lua_pushnumber(L, this->GetID());
	lua_pushnumber(L, t);
	if(lua_pcall(L,2,0,0)!=0) {
		LogMsg(ERR,"Failed to run setHuntHostile\n");
	} else {
		//printf("Successfully Registered target %d\n",t);
	}
}

/**\brief checks if a potential target is within targeting range
 *
 */
bool InRange(Coordinate a, Coordinate b){
	//printf("InRange check\n");
	int x=a.GetX() - b.GetX();
	int y=a.GetY() - b.GetY();
	//printf("finished InRange check\n");
	return(x*x + y*y <=COMBAT_RANGE_SQUARED);
	
}


/**\brief sorts a list of AI's by their current target
 *
 */

bool CompAI(Sprite* a, Sprite* b){
	//printf("Start Comp AI\n");
	if(a->GetDrawOrder()==DRAW_ORDER_SHIP && b->GetDrawOrder()==DRAW_ORDER_SHIP)
		return(  ( (AI*)a )->GetTarget() < ( (AI*)b )->GetTarget() );
	else{
		if(a->GetDrawOrder()!=DRAW_ORDER_SHIP)
			LogMsg( ERR, "Error Sprite %d is not a ship\n", (a)->GetID() );
		if(b->GetDrawOrder()!=DRAW_ORDER_SHIP)
			LogMsg( ERR, "Error Sprite %d is not a ship\n", (b)->GetID() );
		return false;
	}
	//printf("finished Comp AI\n");
}
/**\fn AI::SetStateMachine(string _machine)
 * \brief Sets the state machine.
 */

/**\fn AI::SetState(string _state)
 * \brief Sets the current state.
 */

/**\fn AI::SetAlliance(Alliance* alliance)
 * \brief Sets the current alliance.
 */

/**\fn AI::GetStateMachine
 * \brief Retrieves the state machine.
 */

/**\fn AI::GetState
 * \brief Retrieves the current state.
 */

/**\fn AI::GetAlliance
 * \brief Retrieves the current alliance.
 * \warning Alliance may be NULL.
 */


