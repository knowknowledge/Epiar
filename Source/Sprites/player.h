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
#include "Sprites/planets.h"
#include "Engine/mission.h"

class Player : public Ship {
	public:
		static Player *Load( string filename );

		// Saving and Loading this Player to XML
		void Save();
		bool FromXMLNode( xmlDocPtr doc, xmlNodePtr node );
		xmlNodePtr ToXMLNode(string componentName);

		void SetName( string _name ) { name = _name; }

		void Update( lua_State *L );
		void Land( lua_State *L, Planet * );

		// Generic Getters
		string GetLastPlanet() { return lastPlanet; }
		string GetName() { return name; }
		string GetFileName() { return "Resources/Definitions/"+ GetName() +".xml"; }
		time_t GetLoadTime() { return lastLoadTime; }
		virtual int GetDrawOrder( void ) { return( DRAW_ORDER_PLAYER ); }
		Color GetRadarColor( void ) { return GOLD; }

		// Autopilot Related Functions
		void SetLuaControlFunc( string _luaControlFunc );
		void RemoveLuaControlFunc();

		// Mission Related Functions
		void AcceptMission( Mission *mission );
		void RejectMission( string missionName );
		list<Mission*>* GetMissions() { return &missions; }

		// Favor Related Functions
		int GetFavor( Alliance* alliance );
		void UpdateFavor( string allianceName, int deltaFavor );

		// Escort-related functions (needed for XML saving/loading)
		void AddHiredEscort(string type, int pay, int spriteID);

		friend class Players;

	protected:
		Player();
		Player( const Player & );
		Player& operator= (const Player&);
		~Player();

		bool ConfigureWeaponSlots(xmlDocPtr, xmlNodePtr);
	private:
		string name;
		time_t lastLoadTime; // TODO This may need to be renamed
		string lastPlanet;
		list<Mission*> missions;
		map<Alliance*,int> favor;
		string luaControlFunc;

		// This list of hired escorts is only needed for XML saving/loading and doesn't control the game itself.
		// Escorts from missions should not be listed here.
		class HiredEscort {
			public:
				string type; ///< The ship Model
				int pay; ///< The cost per day (zero is acceptable)
				int spriteID;	///< This number is not saved but is used to check the status of the sprite when saving
				
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
		string simulation; ///< The Simulation that this Player is playing.
		int seed; ///< The Seed for this Simulation.
		time_t lastLoadTime; ///< The last time that this file was loaded.
	private:
};

class Players : public Components {
	public:
		static Players *Instance();
		PlayerInfo* GetPlayerInfo(string name) { return (PlayerInfo*)Components::Get( name ); }
		Component* newComponent() { return new PlayerInfo(); }

		Player* CreateNew(string playerName,
			Model *model,
			Engine *engine,
			int credits,
			Coordinate location);
		Player* LoadLast();
		Player* LoadPlayer(string playerName);
		bool    DeletePlayer(string playerName);
		bool	PlayerExists(string playerName);

	protected:
		Players() {};
		Players( const Players & );
		Players& operator= (const Players&);

	private:
		static Players *pInstance;
};

#endif // __H_PLAYER__
