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
#define COMBAT_RANGE 1000 //radius of ships involved in any specific battle
#define COMBAT_RANGE_SQUARED 1000000

// Sprites have an AI object which is used to manipulate their attributes
// to run an AI simulation


//relevant enemy information
typedef struct{
	int damage;
	int id; 
}enemy;

class AI : public Ship {
	public:
		AI(string name, string machine);
		void Update( lua_State *L );
		void Draw();

		void Decide( lua_State *L );

		void SetTarget(int t);
		int GetTarget(){return target;}

		void SetStateMachine(string _machine) { stateMachine = _machine; }
		void SetState(string _state)  { state = _state; }

		void SetAlliance(Alliance* alliance) { allegiance = alliance; }
		string GetName() { return name; }
		void SetName(string newName) { name = newName; }

		string GetStateMachine() { return stateMachine; }
		string GetState() { return state; }

		Alliance* GetAlliance() { return allegiance; }

		void AddEnemy(int e, int damage);
		void RemoveEnemy(int e);

		void SetMerciful(int f) { merciful = (f == 1); }
		int GetMerciful() { return (merciful ? 1 : 0 ); }

	private:
		int CalcCost(int threat, int damage);
		int ChooseTarget( lua_State *L );
		void RegisterTarget( lua_State *L, int t );
		list<enemy> enemies;
		int target;
		string name;
		string stateMachine;
		string state;
		Alliance* allegiance;

		bool merciful; ///< Is this ship merciful to the player?
};

bool CompAI(Sprite* a, Sprite* b);

bool InRange(Coordinate a, Coordinate b);

#endif /*AI_H_*/
