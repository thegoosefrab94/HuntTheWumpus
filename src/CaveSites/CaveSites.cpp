#include "CaveSites.h"
#include "../Actors/Actors.h"

namespace Wump {

	Direction OtherDirection(Direction d) {
		switch (d) {
			case Direction::north:
				return Direction::south;
				break;
			case Direction::south:
				return Direction::north;
				break;
			case Direction::east:
				return Direction::west;
				break;
			case Direction::west:
				return Direction::east;
				break;
			default:
				return Direction::north;
				break;
		}
	}

	Room::Room(std::size_t num) : m_RoomNum{ num }, m_Trap{ Trap::none },
		m_pNorth{ nullptr }, m_pSouth{ nullptr }, m_pEast{ nullptr }, m_pWest{ nullptr } {	}

	void Room::Enter(Actor* act, Output& os) {
		os << "You enter room " << GetNumber()<<'\n';
		act->m_pLocation = this;
	}

	void Room::SetSide(Direction dir, CaveSite* newSite) {
		switch (dir) {
		case Direction::north:
			m_pNorth = newSite;
			return;
		case Direction::south:
			m_pSouth = newSite;
			return;
		case Direction::east:
			m_pEast = newSite;
			return;
		case Direction::west:
			m_pWest = newSite;
			return;
		}
	}

	CaveSite* Room::GetSide(Direction lookHere) const {
		switch (lookHere)
		{
		case Direction::north:
			return m_pNorth;
		case Direction::south:
			return m_pSouth;
		case Direction::east:
			return m_pEast;
		case Direction::west:
			return m_pWest;
		default:
			return nullptr;
		}
	}

	bool Room::IsTrapped() const {
		return (m_Trap == pit || m_Trap == bat || m_Trap == wumpus);
	}

	Tunnel::Tunnel(Room* l, Room* r) : m_pLeft(l), m_pRight(r) {  }

	void Tunnel::Enter(Actor* act, Output& os) {
		os << "You walk down the tunnel...\n";
		OtherSide(act->Location())->Enter(act, os);
	}

	Room* Tunnel::OtherSide(Room * curr) {
		if (curr)
		{
			if (curr == m_pLeft)
				return m_pRight;
			else if (curr == m_pRight)
				return m_pLeft;
			else return nullptr;
		}
		return nullptr;
	}

	void Wall::Enter(Actor* act, Output& os) {
		os << "You hit a wall!\n";
	}

	Cave::Cave() {
		m_Sites = SitesSet{};
		m_Rooms = RoomSet{};
	}

	void Cave::AddSite(CaveSite* newSite) {
		m_Sites.insert(newSite);
	}

	void Cave::AddRoom(Room* newRoom) {
		m_Rooms.insert(newRoom);
	}

	Room* Cave::RoomNumber(std::size_t num) const {
		for (auto iterator = m_Rooms.begin(); iterator != m_Rooms.end(); ++iterator)
		{
			if ((*iterator)->GetNumber() == num) return *iterator;
		}
		return nullptr;
	}

	CaveSite* Cave::FindSite(CaveSite* findThis) const {
		for (auto iterator = m_Sites.begin(); iterator != m_Sites.end(); ++iterator)
		{
			if (*iterator == findThis) return *iterator;
		}
		return nullptr;
	}

	void Cave::ChangeSite(CaveSite* oldSite, CaveSite* newSite) {
		auto iterator = m_Sites.begin();
		while (iterator != m_Sites.end()) {
			// Find the old site in the set
			if (*iterator == oldSite) break;
			++iterator;
		}
		if (iterator == m_Sites.end()) return;
		// Erase the old one if its in the set
		delete *iterator;
		m_Sites.erase(iterator);
		// Now insert the new site
		// Ensures that if multiple sites are changed into one site, it is only added once
		//  and hence deleted once
		m_Sites.insert(newSite);
		// Dont need the return from insert or erase, theres not any work to do with them
	}

	Cave::~Cave() { // Cave assumes all resposibility for deleting sites and rooms
		for (auto* site : m_Sites)
			delete site;
		for (auto* room : m_Rooms)
			delete room;
	}

}