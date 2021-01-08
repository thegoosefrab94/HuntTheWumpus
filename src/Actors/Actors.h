#pragma once
#include "../CaveSites/CaveSites.h"
#include "../utils/ObserverBases.h"

namespace Wump
{
	// Actor is a base class for everything that moves within the cave
	// All actors are to be watched for something interesting
	class Actor : public Subject {
		friend class Room;
		friend class Tunnel;
	public:
		explicit Actor(std::ostream& os) : output{ os }, m_alive{ true }, m_pLocation{ nullptr } { }
		virtual bool IsPlayer() const { return false; }
		virtual void Move(Direction) = 0;
		bool IsAlive() const { return m_alive; }
		virtual void Kill();
		Room* Location()  { return m_pLocation; }	// Return is not const because the Actors could change it
		virtual ~Actor() {  }
	protected:
		std::ostream& output;
		bool m_alive;
		Room* m_pLocation;
	};
	// Player implements Actor and Subject interfaces
	// Player also only cares about rooms because those are the only thing it interacts with
	class Player : public Actor {
	public:
		explicit Player(std::ostream&, Room*);
		virtual void RegisterObserver(Observer*) override;
		virtual void UnregisterObserver(Observer*) override;
		virtual bool IsPlayer() const override { return true; }
		virtual void Move(Direction) override;
		virtual void Kill() override;
		void Sense() const;
		void Shoot(Direction);
		virtual ~Player() { }
	private:
		void Report(CaveSite*) const;
		std::size_t m_arrows;
	};
	
	class Wumpus : public Actor {
	public:
		explicit Wumpus(std::ostream&, Room*);
		virtual void RegisterObserver(Observer*) override;
		virtual void UnregisterObserver(Observer*) override;
		virtual bool IsPlayer() const override { return false; }
		virtual void Move(Direction);
		virtual void Kill() override;
		void WakeUp() { m_awake = true; }
		virtual ~Wumpus() { }
	private:
		bool m_awake;
		Room::Trap m_pastTrap;
	};
}
