#include "CaveSystem.h"
#include <random>

namespace Wump {

	std::size_t NumRoll(std::size_t min, std::size_t max)		// Rolls a random number
	{
		static std::mt19937_64 gen;
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
				if (pEnemyWumpus->Location() == pPlayer->Location())
					OnNotify(pPlayer, Event::lose);
				break;
			case Event::win:
				OnNotify(pPlayer, e);
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
					auto randomRoom = NumRoll(0, pCave->Size());
					auto* moveTo = pCave->RoomNumber(randomRoom);
					moveTo->Enter(actor, output);
					break;
				}
				case Room::pit: {
					output << "You have fallen down a bottomless pit!\n";
					actor->Kill();
					break;
				}
				case Room::wumpus: {
					if (pEnemyWumpus->IsAlive())
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
			if (!pEnemyWumpus->Location()->IsTrapped())	// Is the cave the wumpus is in no longer trapped?
				pEnemyWumpus->Kill();					// Then wumpus is dead
			else {
				auto randDirection = NumRoll(std::size_t(Direction::north), std::size_t(Direction::west));
				pEnemyWumpus->Move(Direction(randDirection));
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
	void CaveSystem::CreateSystem(CaveBuilder& builder) {
		builder.CreateCave();
		std::size_t current = 0, prev = 0, startOfLoop = 0;;
		builder.CreateRoom(current);
		prev = current++;
		for (std::size_t i = 0; i < 4; ++i) {
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
		for (std::size_t i = 0; i < 9; ++i) {
			builder.CreateRoom(current);
			builder.CreateTunnel(prev, current, Direction::west);
			if (!(i % 2) && i != 0) {
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
		for (std::size_t i = 0; i < 4; ++i) {
			builder.CreateRoom(current);
			builder.CreateTunnel(prev, current, Direction::west);
			builder.CreateTunnel(current, outLoop, Direction::south);
			prev = current++;
			outLoop += 2;
		}
		builder.CreateTunnel(prev, startOfLoop, Direction::west);
		pCave = builder.GetCave();
		
		auto numOfBats = NumRoll(1, 3);
		auto numOfPits = NumRoll(1, 4);

		for (std::size_t i = 0; i < numOfBats; ++i) {
			auto randomCaveNum = NumRoll(1, pCave->Size());
			auto* randCave = pCave->RoomNumber(randomCaveNum);
			randCave->SetTrap(Room::bat);
		}
		for (std::size_t i = 0; i < numOfPits; ++i) {
			auto randomCaveNum = NumRoll(1, pCave->Size());
			auto* randCave = pCave->RoomNumber(randomCaveNum);
			randCave->SetTrap(Room::pit);
		}
		auto wumpCave = NumRoll(5, pCave->Size());
		pEnemyWumpus = new Wumpus(wumpFile, pCave->RoomNumber(wumpCave));
	}
	void CaveSystem::AttachPlayer(Player* play) {
		pPlayer = play;
		pPlayer->RegisterObserver(this);
	}
	CaveSystem::~CaveSystem() {
		pPlayer->UnregisterObserver(this);
		delete pEnemyWumpus;
		delete pCave;
	}
}