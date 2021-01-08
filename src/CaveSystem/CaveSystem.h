#pragma once
#include "../Actors/Actors.h"
#include "../CaveBuilder/CaveBuilder.h"
#include "../utils/ObserverBases.h"
#include <fstream>

namespace Wump {
	// Implements the observer interface class
	// This is the "central nervous system"
	class CaveSystem : public Observer {
	public:
		CaveSystem(std::ostream& os) : output{ os }, m_pEnemyWumpus{ nullptr }, m_pPlayer{ nullptr }, m_pCave{ nullptr }, areWeDone{ false } {  }
		virtual void OnNotify(Actor*, Event);
		virtual void CreateSystem(CaveBuilder&);
		bool Done() const { return areWeDone; }
		void Restart() { areWeDone = false; }
		void AttachPlayer(Player*);
		Room* Start() const { return m_pCave->RoomNumber(0); }
		virtual ~CaveSystem();
	private:
		void HandlePlayer(Actor* actor, Event e);
		std::ostream& output;
		std::ofstream wumpFile;
		Actor* m_pEnemyWumpus;
		Actor* m_pPlayer;
		Cave* m_pCave;
		bool areWeDone;
	};
}
