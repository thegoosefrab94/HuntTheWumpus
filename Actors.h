#pragma once
#include "CaveSites.h"
#include "ObserverBases.h"

namespace Wump
{
	class Actor : public Subject {
		friend class Room;
		friend class Tunnel;
	public:
		Actor(std::ostream& os) : output{ os }, alive { true }, pLocation{ nullptr } {  }
		virtual bool IsPlayer() const { return false; }
		virtual void Move(Direction) = 0;
		bool IsAlive() const { return alive; }
		virtual void Kill();
		Room* Location()  { return pLocation; }	// Return is not const because the Actors could change it
		virtual ~Actor() {  }
	protected:
		std::ostream& output;
		bool alive;
		Room* pLocation;
	};

	class Player : public Actor {
	public:
		explicit Player(std::ostream&, Room*);
		virtual void RegisterObserver(Observer*) override;
		virtual void UnregisterObserver(Observer*) override;
		virtual bool IsPlayer() const override { return true; }
		virtual void Move(Direction) override;
		virtual void Kill() override;
		void Shoot(Direction);
		virtual ~Player() {  }
	private:
		std::size_t arrows;
	};
	
	class Wumpus : public Actor {
	public:
		explicit Wumpus(std::ostream&, Room*);
		virtual void RegisterObserver(Observer*) override;
		virtual void UnregisterObserver(Observer*) override;
		virtual bool IsPlayer() const override { return false; }
		virtual void Move(Direction);
		virtual void Kill() override;
		void WakeUp() { awake = true; }
		virtual ~Wumpus() {  }
	private:
		bool awake;
		Room::Trap pastTrap;
	};
}
