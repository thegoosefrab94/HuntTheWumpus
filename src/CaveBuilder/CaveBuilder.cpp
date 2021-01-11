#include "CaveBuilder.h"
#include <iostream>

namespace Wump {

	void CaveBuilder::CreateCave() {
		if (!m_pCave)
			m_pCave = new Cave();
		else
			return;
	}

	void CaveBuilder::CreateRoom(std::size_t num) {
		// Create all the necessary parts of the room
		Room* newRoom = new Room(num);
		CaveSite* nWall = new Wall();
		CaveSite* sWall = new Wall();
		CaveSite* eWall = new Wall();
		CaveSite* wWall = new Wall();
		// All rooms start with four walls
		newRoom->SetSide(Direction::north, nWall);
		newRoom->SetSide(Direction::south, sWall);
		newRoom->SetSide(Direction::east, eWall);
		newRoom->SetSide(Direction::west, wWall);
		// Add all parts to the cave to be deleted
		m_pCave->AddSite(nWall);
		m_pCave->AddSite(sWall);
		m_pCave->AddSite(eWall);
		m_pCave->AddSite(wWall);
		m_pCave->AddRoom(newRoom);
	}

	void CaveBuilder::CreateTunnel(std::size_t fromRoom, std::size_t toRoom, Direction dir) {
		auto* left = m_pCave->RoomNumber(fromRoom);
		auto* right = m_pCave->RoomNumber(toRoom);
		Tunnel* tun = new Tunnel(left, right);
		m_pCave->ChangeSite(left->GetSide(dir), tun);
		left->SetSide(dir, tun);
		m_pCave->ChangeSite(right->GetSide(OtherDirection(dir)), tun);
		right->SetSide(OtherDirection(dir), tun);
	}

	void CaveBuilder::CreateWall() {
		CaveSite* newWall = new Wall();
		m_pCave->AddSite(newWall);
	}
}