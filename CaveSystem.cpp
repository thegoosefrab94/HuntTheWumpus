#include "CaveSystem.h"
#include <random>

namespace Wump {

	std::size_t NumRoll(std::size_t min, std::size_t max)		// Rolls a random number
	{
		static std::default_random_engine gen;
		static std::uniform_int_distribution<std::size_t> dis(min, max);
		return dis(gen);
	}

	void CaveSystem::OnNotify(Actor* actor, Event e) {
		if (actor->IsPlayer()) {	
			HandlePlayer(actor, e);
		}
		else if (!actor->IsPlayer()) {	// For any other actor if only handles move and win
			switch (e) {
			case Event::move:
				if (m_pEnemyWumpus->Location() == m_pPlayer->Location())
					OnNotify(m_pPlayer, Event::lose);
				break;
			case Event::win:
				OnNotify(m_pPlayer, e);
			}
		}
	}
	void CaveSystem::HandlePlayer(Actor* actor, Event e) {
		switch (e) {
		case Event::move: {
			auto* playerCave = actor->Location();
			if (playerCave->IsTrapped()) {	// Only interfere if the cave is trapped
				switch (playerCave->GetTrap()) {
				case Room::bat: {
					output << "A bat swoops down and moves you to a random cave!\n";
					auto randomRoom = NumRoll(0, m_pCave->Size());
					auto* moveTo = m_pCave->RoomNumber(randomRoom);
					moveTo->Enter(actor, output);
					break;
				}
				case Room::pit: {
					output << "You have fallen down a bottomless pit!\n";
					actor->Kill();
					break;
				}
				case Room::wumpus: {
					output << "You encounter the wumpus!\n";
					if (m_pEnemyWumpus->IsAlive())
						actor->Kill();
					else
						OnNotify(actor, Event::win);
					break;
				}
				}
			}
			break;
		}
		case Event::shoot:
			if (!m_pEnemyWumpus->Location()->IsTrapped())	// Is the cave the wumpus is in no longer trapped?
				m_pEnemyWumpus->Kill();					// Then wumpus is dead
			else {
				auto randDirection = NumRoll(std::size_t(Direction::north), std::size_t(Direction::west));
				m_pEnemyWumpus->Move(Direction(randDirection));
			}
			break;

		case Event::win:
			output << "You have defeated the Wumpus!\n";
			areWeDone = true;
			break;

		case Event::lose:
			output << "You have lost... never to be heard from again.\n";
			areWeDone = true;
		}
	}
	// Creates a dodecahedron of rooms and tunnels
	void CaveSystem::CreateSystem(CaveBuilder& builder) {
		builder.CreateCave();
		std::size_t current = 0, prev = 0, startOfLoop = 0;;
		builder.CreateRoom(current);
		prev = current++;
		for (std::size_t i = 0; i < 4; ++i) {	// 5 rooms in the most outer loop
			builder.CreateRoom(current);
			builder.CreateTunnel(prev, current, Direction::west);
			prev = current++;
		}
		builder.CreateTunnel(prev, startOfLoop, Direction::west);
		builder.CreateRoom(current);
		builder.CreateTunnel(startOfLoop, current, Direction::north);
		startOfLoop = current;
		prev = current++;
		std::size_t outLoop = 1;
		for (std::size_t i = 0; i < 9; ++i) {	// 10 rooms in the middle loop
			builder.CreateRoom(current);
			builder.CreateTunnel(prev, current, Direction::west);
			if (i % 2) {
				builder.CreateTunnel(current, outLoop, Direction::south);
				outLoop++;
			}
			prev = current++;
		}
		builder.CreateTunnel(prev, startOfLoop, Direction::west);
		builder.CreateRoom(current);
		builder.CreateTunnel(prev, current, Direction::north);
		outLoop = startOfLoop + 1;
		startOfLoop = current;
		prev = current++;
		for (std::size_t i = 0; i < 4; ++i) {	// 5 rooms in the innermost loop
			builder.CreateRoom(current);
			builder.CreateTunnel(prev, current, Direction::west);
			builder.CreateTunnel(current, outLoop, Direction::south);
			prev = current++;
			outLoop += 2;
		}
		builder.CreateTunnel(prev, startOfLoop, Direction::west);
		m_pCave = builder.GetCave();
		
		auto numOfBats = NumRoll(1, 4);
		auto numOfPits = NumRoll(1, 3);

		for (std::size_t i = 0; i < numOfBats; ++i) {
			auto randomCaveNum = NumRoll(1, m_pCave->Size()-1);
			auto* randCave = m_pCave->RoomNumber(randomCaveNum);
			randCave->SetTrap(Room::bat);
		}
		for (std::size_t i = 0; i < numOfPits; ++i) {
			auto randomCaveNum = NumRoll(1, m_pCave->Size()-1);
			auto* randCave = m_pCave->RoomNumber(randomCaveNum);
			randCave->SetTrap(Room::pit);
		}
		auto wumpCave = NumRoll(5, m_pCave->Size()-1);
		wumpFile.open("wumpus.txt");
		m_pEnemyWumpus = new Wumpus(wumpFile, m_pCave->RoomNumber(wumpCave));
		m_pEnemyWumpus->RegisterObserver(this);
	}
	void CaveSystem::AttachPlayer(Player* play) {
		m_pPlayer = play;
		m_pPlayer->RegisterObserver(this);
	}
	CaveSystem::~CaveSystem() {
		wumpFile.close();
		delete m_pEnemyWumpus;
		delete m_pCave;
	}
}