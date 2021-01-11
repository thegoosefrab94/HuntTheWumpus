#pragma once
#include "../Actors/Actors.h"
#include "../CaveBuilder/CaveBuilder.h"
#include "../utils/ObserverBases.h"
#include <fstream>
#include <memory>

namespace Wump {
	// Implements the observer interface class
	// This is the "central nervous system"
	class CaveSystem : public Observer {
	public:
		CaveSystem(std::ostream& os);
		virtual void OnNotify(Actor*, Event);
		virtual void CreateSystem(CaveBuilder&);
		bool Done() const { return areWeDone; }
		void Restart();
		void AttachPlayer(Player*);
		Room* Start() const { return m_pCave->RoomNumber(0); }
		virtual ~CaveSystem();
	private:
		using CavePtr = std::unique_ptr<Cave>;
		using ActorPtr = std::unique_ptr<Actor>;
		void HandlePlayer(Actor* actor, Event e);
		std::ostream& output;
		std::ofstream wumpFile;

		Actor* m_pEnemyWumpus;
		ActorPtr m_upPlayer;
		ActorPtr m_upEnemyWumpus;
		Cave* m_pCave;
		CavePtr m_upCave;
		bool areWeDone;
	};
}
