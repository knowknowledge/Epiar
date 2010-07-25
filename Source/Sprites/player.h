/**\file			player.h
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Wednesday, July 5, 2006
 * \date			Modified: Saturday, January 5, 2008
 * \brief			Main player-specific functions and handle
 * \details
 */


#ifndef __H_PLAYER__
#define __H_PLAYER__

#include "includes.h"
#include "Sprites/ship.h"

class Player : public Ship , public Component {
	public:
		static Player *Instance();

		static bool IsLoaded() { return pInstance!=NULL; }
		static void CreateNew(string playerName);
		static void LoadLast();
		static void Load(string playerName);
		string GetName() { return name; }

		bool FromXMLNode( xmlDocPtr doc, xmlNodePtr node );
		xmlNodePtr ToXMLNode(string componentName);

		friend class Players;

	protected:
		Player();
		Player( const Player & );
		Player& operator= (const Player&);
		~Player();

		void Update( void );

		Color GetRadarColor( void ) { return Color::Get(0xFF,0xD7,0); }
		virtual int GetDrawOrder( void ) {
			return( DRAW_ORDER_PLAYER );
		}

	private:
		static Player *pInstance;
};

class Players : public Components {
	public:
		static Players *Instance();
		Player* GetPlayer(string name) { return (Player*) this->Get(name); }
		Component* newComponent() { return new Player(); }

	protected:
		Players() {};
		Players( const Players & );
		Players& operator= (const Players&);

	private:
		static Players *pInstance;
};

#endif // __H_PLAYER__
