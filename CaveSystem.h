#pragma once
#include "Actors.h"
#include "CaveBuilder.h"
#include "ObserverBases.h"
#include <fstream>

namespace Wump {
	class CaveSystem : public Observer {
	public:
		CaveSystem(std::ostream& os) : output{ os }, pEnemyWumpus{ nullptr }, pPlayer{ nullptr }, pCave{ nullptr }, areWeDone{ false } {  }
		virtual void OnNotify(Actor*, Event);
		virtual void CreateSystem(CaveBuilder&);
		void AttachPlayer(Player*);
		virtual ~CaveSystem();
	private:
		void HandlePlayer(Actor* actor, Event e);
		std::ostream& output;
		std::ofstream wumpFile{ "wumpus.txt"};
		Actor* pEnemyWumpus;
		Actor* pPlayer;
		Cave* pCave;
		bool areWeDone;
	};
}
