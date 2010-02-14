/**\filename		simulation.h
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: July 2006
 * \date			Modified: Tuesday, June 23, 2009
 * \brief			Contains the main game loop
 * \details
 */

#ifndef __H_SIMULATION__
#define __H_SIMULATION__

#include "Engine/alliances.h"
#include "Engine/engines.h"
#include "Engine/models.h"
#include "Sprites/planets.h"
#include "Engine/weapons.h"
#include "Engine/technologies.h"

class Simulation {
	public:
		Simulation();
		Simulation( string filename );
		
		bool Load( string filename );
		bool Run( void );

		static float GetFPS();
		
		static void pause();
		static void unpause();
		static bool isPaused() {return paused;}

	private:
		bool Parse( void );
	
		string filename;
		string planetsFilename, modelsFilename, enginesFilename,weaponsFilename, alliancesFilename, technologiesFilename;
		
		Planets *planets;
		Engines *engines;
		Models *models;
		Weapons *weapons;
		Alliances *alliances;
		Technologies *technologies;
		
		string playerDefaultModel;

		static float currentFPS;
		static bool paused;
};

#endif // __H_SIMULATION__
