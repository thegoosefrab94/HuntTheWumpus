#include "Actors.h"

namespace Wump {

	void Actor::Kill() { m_alive = false; }

	Player::Player(std::ostream& os, Room* start) : Actor(os), m_arrows{ 5 } { start->Enter(this, output); }
	void Player::RegisterObserver(Observer* observe) {
		observers.push_back(observe);
	}
	void Player::UnregisterObserver(Observer* observe) {
		for (auto iterator = observers.begin(); iterator != observers.end(); ++iterator)
			if (*iterator == observe) observers.erase(iterator);
	}
	void Player::Move(Direction moveTo) {
		auto* move = m_pLocation->GetSide(moveTo); // Move the player first
		move->Enter(this, output);
		Notify(this, Event::move);	// Notify any observers
	}
	void Player::Sense() const {
		Report(m_pLocation->GetSide(Direction::north));
		Report(m_pLocation->GetSide(Direction::south));
		Report(m_pLocation->GetSide(Direction::east));
		Report(m_pLocation->GetSide(Direction::west));
	}
	void Player::Report(CaveSite* site)  const {
		if (site->IsRoom()) {
			Room* room = dynamic_cast<Room*>(site);
			if (room->IsTrapped()) {
				switch (room->GetTrap()) {
				case Room::bat:
					output << "You hear chirping in an adjacent room.\n";
					break;
				case Room::pit:
					output << "You feel a breeze coming from a distnace.\n";
					break;
				case Room::wumpus:
					output << "You smell something foul very close to you.\n";
					break;
				}
			}
		}
		else if (site->IsTunnel()) {
			Tunnel* tunnel = dynamic_cast<Tunnel*>(site);
			Report(tunnel->OtherSide(m_pLocation));
		}
		else
			return;
	}
	void Player::Shoot(Direction shoot) {
		if (m_arrows <= 0)	// Can't shoot when you have no arrows
			return;
		else {
			auto* shootAt = m_pLocation->GetSide(shoot);
			if (shootAt->IsTunnel()) {	// Only check if its a tunnel because all rooms are connected by tunnels
				Tunnel* shoot = dynamic_cast<Tunnel*>(shootAt);	// Ew casting, I checked it was safe before though. still...
				Room* killRoom = shoot->OtherSide(m_pLocation);	// Get the room on the other side of this one
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
			--m_arrows;	// Subtract from arrows and notify observer
			output << "You now have " << m_arrows << " arrows left.\n";
			Notify(this, Event::shoot);
		}
	}
	void Player::Kill() {
		m_alive = false;
		Notify(this, Event::lose);
	}

	Wumpus::Wumpus(std::ostream& os, Room* start) : Actor(os), m_awake{ false }, m_pastTrap{ Room::none } { 
		start->Enter(this, output); 
		m_pastTrap = m_pLocation->GetTrap();
		start->SetTrap(Room::wumpus);
	}
	void Wumpus::RegisterObserver(Observer* observe) {
		observers.push_back(observe);
	}
	void Wumpus::UnregisterObserver(Observer* observe) {
		for (auto iterator = observers.begin(); iterator != observers.end(); ++iterator)
			if (*iterator == observe) observers.erase(iterator);
	}
	void Wumpus::Move(Direction moveTo) {
		m_awake = true;
		if (m_awake && IsAlive()) {	// Only move when alive and awake
			auto* move = m_pLocation->GetSide(moveTo);
			m_pLocation->SetTrap(m_pastTrap);
			move->Enter(this, output);
			m_pastTrap = m_pLocation->GetTrap();
			m_pLocation->SetTrap(Room::wumpus);
		}
		m_awake = false;	// After every move, fall back asleep
		Notify(this, Event::move);
	}
	void Wumpus::Kill() {
		m_alive = false;
		Notify(this, Event::win);
	}
}