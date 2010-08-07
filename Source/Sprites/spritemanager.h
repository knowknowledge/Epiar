/**\file			spritemanager.h
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Unknown (2006?)
 * \brief
 * \details
 */


#ifndef __H_SPRITEMANAGER__
#define __H_SPRITEMANAGER__

#include "Sprites/sprite.h"
#include "Utilities/quadtree.h"


class SpriteManager {
	public:
		static SpriteManager *Instance();

		void Add( Sprite *sprite );
		bool Delete( Sprite *sprite );
		
		void Update();
		void Draw();

		Sprite *GetSpriteByID(int id);
		list<Sprite*> *GetSprites(int type = DRAW_ORDER_ALL);
		list<Sprite*> *GetSpritesNear(Coordinate c, float r, int type = DRAW_ORDER_ALL);
		Sprite* GetNearestSprite(Sprite *obj, float r, int type = DRAW_ORDER_ALL);

		Coordinate GetQuadrantCenter( Coordinate point );
		int GetNumQuadrants() { return trees.size(); }
		int GetNumSprites();
		void GetBoundaries(float *northEdge, float *southEdge, float *eastEdge, float *westEdge);

		void Save();

	protected:
		SpriteManager();
	private:
		// Use the tree when referring to the sprites at a location.
		map<Coordinate,QuadTree*> trees;
		// Use the list when referring to all sprites.
		list<Sprite*> *spritelist;
		// Use the map when referring to sprites by their unique ID.
		map<int,Sprite*> *spritelookup;
		
		list<Sprite *> spritesToDelete;
		static SpriteManager *pInstance;

				//counts number of ticks to track updates to quadrants
				//max value is the number of ticks to update all quadrants
		int tickCount;
		const int semiRegularPeriod;		//the period at which every semi-regular quadrant is updated
		const int fullUpdatePeriod;			//the period at which every quadrant is updated regardless of distance

		const int numRegularBands;			//the number of bands surrounding the centre point that are updated every tick
		const int numSemiRegularBands;		//the number of bands surrounding the centre point that are updated semi-regularly
		map<int, int> ticksToBandNum;		//the key is the tick# that the value band# will be updated at


		float northEdge, southEdge, eastEdge, westEdge;

		bool DeleteSprite( Sprite *sprite );
		void DeleteEmptyQuadrants( void );
		QuadTree* GetQuadrant( Coordinate point );
		list<QuadTree*> GetQuadrantsNear( Coordinate c, float r);
		list<QuadTree*> GetQuadrantsInBand ( Coordinate c, int bandIndex);
		void AdjustBoundaries();
		void UpdateTickCount();

		void GetAllQuadrants (list<QuadTree*> *newTree);
};

#endif // __H_SPRITEMANAGER__
	
