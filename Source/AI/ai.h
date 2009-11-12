/*
 * Filename      : AI/ai.h
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Sunday, May 20, 2007
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : AI for all sprites using Lua
 * Notes         :
 */
 
#ifndef AI_H_
#define AI_H_

#include "Sprites/ship.h"

// Sprites have an AI object which is used to manipulate their attributes
// to run an AI simulation
class AI : public Ship{
	public:
		AI();
		void Update();
		void SetScript(string script);

	private:
		string my_script;
};

#endif /*AI_H_*/
