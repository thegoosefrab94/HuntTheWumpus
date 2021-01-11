#pragma once
#include <iostream>
#include <fstream>
#include <unordered_set>

namespace Wump {
	enum class Direction { north, south, east, west };
	enum class SiteType {Room, Tunnel, Wall};
	
	Direction OtherDirection(Direction d);

	class Actor; // Forward declare of the Actor subclass

	class CaveSite {	// Base class for all parts of a cave
	public:
		using Output = std::ostream;
		CaveSite() {  }
		virtual void Enter(Actor*, Output&) = 0;
		virtual SiteType GetType() const = 0;
		virtual ~CaveSite() {  }
	};


	class Room : public CaveSite {
	public:
		// Helper enums to help the user
		enum Trap { none, pit, bat, wumpus };
		Room(std::size_t num);

		virtual void Enter(Actor*, Output&) override;
		virtual SiteType GetType() const override { return SiteType::Room; }

		void SetSide(Direction dir, CaveSite* side);
		void SetTrap(Trap newTrap) { m_Trap = newTrap; }
		CaveSite* GetSide(Direction dir) const;
		Trap GetTrap() const { return m_Trap; }
		std::size_t GetNumber() const { return m_RoomNum; }
		bool IsTrapped() const;

		virtual  ~Room() {  }
	protected:
		CaveSite* m_pNorth;
		CaveSite* m_pSouth;
		CaveSite* m_pEast;
		CaveSite* m_pWest;
		std::size_t m_RoomNum;
		Trap m_Trap;
	};

	class Tunnel : public CaveSite {	// Tunnel connects two rooms together
	public:
		Tunnel(Room* l, Room* r);

		virtual void Enter(Actor*, Output&) override;
		virtual SiteType GetType() const override { return SiteType::Tunnel; }
		Room* OtherSide(Room* curr);

		virtual ~Tunnel() {  }
	protected:
		Room* m_pLeft;
		Room* m_pRight;
	};

	class Wall : public CaveSite {
	public:
		Wall() {  }
		virtual void Enter(Actor*, Output&) override;
		virtual SiteType GetType() const override { return SiteType::Wall; }
		virtual ~Wall() {  }
	};

	class Cave {	// Handle class for the all the rooms and tunnels
	public:
		using SitesSet = std::unordered_set<CaveSite*>;	// Use a set to ensure that you arent accidently deleting already deleted memory
		using RoomSet = std::unordered_set<Room*>;
		Cave();
		virtual void AddSite(CaveSite* newSite);
		virtual void AddRoom(Room* newRoom);
		auto Size() const { return m_Rooms.size(); }
		
		Room* RoomNumber(std::size_t num) const;
		CaveSite* FindSite(CaveSite* findThis) const;
		void ChangeSite(CaveSite* oldSite, CaveSite* newSite);
		virtual ~Cave(); // Cave assumes responsibility for all parts of the cave and will delete them
	protected:
		SitesSet m_Sites;
		RoomSet m_Rooms;
	};

}