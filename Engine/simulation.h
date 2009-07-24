/*
 * Filename      : simulation.h
 * Author(s)     : Chris Thielen (chris@luethy.net)
 * Date Created  : Unknown (2006?)
 * Last Modified : Saturday, January 5, 2008
 * Purpose       : 
 * Notes         :
 */

#ifndef __H_SIMULATION__
#define __H_SIMULATION__

#include "Engine/alliances.h"
#include "Engine/engines.h"
#include "Engine/models.h"
#include "Sprites/planets.h"

class Simulation {
	public:
		Simulation();
		Simulation( string filename );
		
		bool Load( string filename );
		bool Run( void );

		static float GetFPS();

	private:
		bool Parse( void );
	
		string filename;
		string planetsFilename, modelsFilename, enginesFilename, alliancesFilename;
		
		Planets *planets;
		Engines *engines;
		Models *models;
		Alliances *alliances;
		
		string playerDefaultModel;

		static float currentFPS;
};

#endif // __H_SIMULATION__
