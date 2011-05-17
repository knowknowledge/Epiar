/**\file			ai.h
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \date			Modified: Saturday, January 5, 2008
 * \brief
 * \details
 */

#ifndef AI_H_
#define AI_H_

#include "Sprites/ship.h"
#include "Engine/alliances.h"
#include "includes.h"

#define COMBAT_RANGE 1000 ///< Radius of ships involved in any specific battle
#define COMBAT_RANGE_SQUARED (COMBAT_RANGE*COMBAT_RANGE) ///< Used for fast range checking.

class AI : public Ship {
	public:
		AI(string name, string machine);

		// Overloaded Sprite Mechanics:
		void Update( lua_State *L );
		void Draw();

		// Flavor Mechanics:

		string GetName() { return name; }
		void SetName(string newName) { name = newName; }

		Alliance* GetAlliance() { return allegiance; }
		void SetAlliance(Alliance* alliance) { allegiance = alliance; }

		// State Machine Mechanics:

		string GetStateMachine() { return stateMachine; }
		void SetStateMachine(string _machine) { stateMachine = _machine; }

		string GetState() { return state; }
		void SetState(string _state)  { state = _state; }

		// Combat Mechanics:

		void SetTarget(int t);
		int GetTarget(){return target;}

		void AddEnemy(int spriteID, int damage);
		void RemoveEnemy(int spriteID);

		void SetMerciful(int f) { merciful = (f == 1); }
		int GetMerciful() { return (merciful ? 1 : 0 ); }

	private:
		string name; ///< The AI's name.  This should be the name of the ship's pilot.
		Alliance* allegiance; ///< Which Alliance this ship hails to.

		// The AI is controlled by a Lua state Machine
		// The state machine is essentially a flow chart
		string stateMachine; ///< The name of the State Machine.
		string state; ///< The current state of the state machine.
		void Decide( lua_State *L );

		// AI Combat Mechanics:

		typedef struct{
			int damage; ///< Damage received by this ship
			int id; ///< The enemy ship's unique id
		}enemy; ///< Simple tracker for how much damage has been taken from other ships

		int target; ///< The enemy that this AI is currently fighting
		bool merciful; ///< Is this ship merciful to the player?
		list<enemy> enemies; ///< A list of combatants.  The AI should keep fighting until everything on this list is dead.

		int CalcCost(int threat, int damage);
		int ChooseTarget( lua_State *L );
		void RegisterTarget( lua_State *L, int t );

		static bool EnemyComp(AI::enemy a, AI::enemy b){return(a.id<b.id);}
		static bool CompareAI(Sprite* a, Sprite* b);
		static bool InRange(Coordinate a, Coordinate b);
};



#endif /*AI_H_*/
