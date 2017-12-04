#include "Actors.h"

namespace Wump {

	void Actor::Kill() { alive = false; }

	Player::Player(std::ostream& os, Room* start) : Actor(os), arrows{ 5 } { pLocation = start; }
	void Player::RegisterObserver(Observer* observe) {
		observers.push_back(observe);
	}
	void Player::UnregisterObserver(Observer* observe) {
		for (auto iterator = observers.begin(); iterator != observers.end(); ++iterator)
			if (*iterator == observe) observers.erase(iterator);
	}
	void Player::Move(Direction moveTo) {
		auto* move = pLocation->GetSide(moveTo); // Move the player first
		move->Enter(this, output);
		Notify(this, Event::move);	// Notify any observers
	}
	void Player::Shoot(Direction shoot) {
		if (arrows <= 0)	// Can't shoot when you have no arrows
			return;
		else {
			auto* shootAt = pLocation->GetSide(shoot);
			if (shootAt->IsTunnel()) {	// Only check if its a tunnel because all rooms are connected by tunnels
				Tunnel* shoot = dynamic_cast<Tunnel*>(shootAt);	// Ew casting, I checked it was safe before though. still...
				Room* killRoom = shoot->OtherSide(pLocation);	// Get the room on the other side of this one
				if (killRoom->IsTrapped()) {
					switch (killRoom->GetTrap()) {
					case Room::bat:
						killRoom->SetTrap(Room::none);
						break;
					case Room::wumpus:
						killRoom->SetTrap(Room::none);
						break;
					default:	// You cant kill a pit
						break;
					}
				}
			}
			--arrows;	// Subtract from arrows and notify observer
			Notify(this, Event::shoot);
		}
	}
	void Player::Kill() {
		alive = false;
		Notify(this, Event::lose);
	}

	Wumpus::Wumpus(std::ostream& os, Room* start) : Actor(os), awake{ false }, pastTrap{ Room::none } { pLocation = start; }
	void Wumpus::RegisterObserver(Observer* observe) {
		observers.push_back(observe);
	}
	void Wumpus::UnregisterObserver(Observer* observe) {
		for (auto iterator = observers.begin(); iterator != observers.end(); ++iterator)
			if (*iterator == observe) observers.erase(iterator);
	}
	void Wumpus::Move(Direction moveTo) {
		awake = true;
		if (awake && IsAlive()) {	// Only move when alive and awake
			auto* move = pLocation->GetSide(moveTo);
			pLocation->SetTrap(pastTrap);
			move->Enter(this, output);
			pastTrap = pLocation->GetTrap();
			pLocation->SetTrap(Room::wumpus);
		}
		awake = false;	// After every move, fall back asleep
		Notify(this, Event::move);
	}
	void Wumpus::Kill() {
		alive = false;
		Notify(this, Event::win);
	}
}