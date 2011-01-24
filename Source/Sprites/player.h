/**\file			player.h
 * \author			Chris Thielen (chris@epiar.net)
 * \date			Created: Wednesday, July 5, 2006
 * \date			Modified: Saturday, January 5, 2008
 * \brief			Main player-specific functions and handle
 * \details
 */


#ifndef __H_PLAYER__
#define __H_PLAYER__

#include "includes.h"
#include "Sprites/ship.h"
#include "Engine/mission.h"

class Player : public Ship {
	public:
		static Player *Instance();
		static bool IsLoaded() { return pInstance!=NULL; }
		static Player *Load( string filename );

		// Saving and Loading this Player to XML
		void Save();
		bool FromXMLNode( xmlDocPtr doc, xmlNodePtr node );
		xmlNodePtr ToXMLNode(string componentName);

		void SetName( string _name ) { name = _name; }
		void setLastPlanet( string planetName);

		// Generic Getters
		string GetLastPlanet() { return lastPlanet; }
		string GetName() { return name; }
		string GetFileName() { return "Resources/Definitions/"+ GetName() +".xml"; }
		time_t GetLoadTime() { return lastLoadTime; }

		// Autopilot Related Functions
		void SetLuaControlFunc( string _luaControlFunc );
		void RemoveLuaControlFunc();

		// Mission Related Functions
		void AcceptMission( Mission *mission );
		void RejectMission( string missionName );
		list<Mission*>* GetMissions() { return &missions; }

		// Escort-related functions (needed for XML saving/loading)
		void AddHiredEscort(string type, int pay, int spriteID);

		friend class Players;

	protected:
		Player();
		Player( const Player & );
		Player& operator= (const Player&);
		~Player();

		void Update( void );

		virtual int GetDrawOrder( void ) {
			return( DRAW_ORDER_PLAYER );
		}
		Color GetRadarColor( void ) { return GOLD; }

		bool ConfigureWeaponSlots(xmlDocPtr, xmlNodePtr);
	private:
		string name;
		static Player *pInstance;
		time_t lastLoadTime; // TODO This may need to be renamed
		string lastPlanet;
		list<Mission*> missions;
		string luaControlFunc;

		// This list of hired escorts is only needed for XML saving/loading and doesn't control the game itself.
		// Escorts from missions should not be listed here.
		class HiredEscort {
			public:
				string type;	// ship type
				int pay;	// cost per day (zero is acceptable)
				int spriteID;	// this number is not saved but is used to check the status of the sprite when saving
				
				HiredEscort(string _type, int _pay, int _spriteID);
				void Lua_Initialize(int playerID, Coordinate playerPos);

		};
		list<HiredEscort*> hiredEscorts;
};

class PlayerInfo : public Component {
	public:
		PlayerInfo();
		PlayerInfo( Player* player );
		void Update( Player* player );

		// Saving and Loading this Player to XML
		bool FromXMLNode( xmlDocPtr doc, xmlNodePtr node );
		xmlNodePtr ToXMLNode(string componentName);
		xmlNodePtr ConvertOldVersion( xmlDocPtr doc, xmlNodePtr node );

		// name is implicit from Component
		Image* avatar; ///< Image for this player (Usually the ship's model)
		string file; ///< The xml file associated with this player.
		time_t lastLoadTime; ///< The last time that this file was loaded.
	private:
};

class Players : public Components {
	public:
		static Players *Instance();
		PlayerInfo* GetPlayerInfo(string name) { return (PlayerInfo*)Components::Get( name ); }
		Component* newComponent() { return new PlayerInfo(); }

		bool Save();

		Player* CreateNew(string playerName);
		Player* LoadLast();
		Player* LoadPlayer(string playerName);

		// Default Player values
		void SetDefaults(
			Model *_defaultModel,
			Engine *_defaultEngine,
			int _defaultCredits,
			Coordinate _defaultLocation);
		Model* GetDefaultModel() { return defaultModel; }
		Engine* GetDefaultEngine() { return defaultEngine; }
		int GetDefaultCredits() { return defaultCredits; }
		Coordinate GetDefaultLocation() { return defaultLocation; }

	protected:
		Players() {};
		Players( const Players & );
		Players& operator= (const Players&);

	private:
		static Players *pInstance;
		Model *defaultModel;
		Engine *defaultEngine;
		int defaultCredits;
		Coordinate defaultLocation;
};

#endif // __H_PLAYER__
