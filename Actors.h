#pragma once
#include <iostream>
#include "Cave.h"

namespace Wump
{
	class Player
	{
	public:
		explicit Player(const Cave* cv);	// Constructor will not change the cave its given
		Player(const Player&) = delete;				// No copying
		Player& operator=(const Player&) = delete;	// No assigning

		void Move(const Cave* cv);
		void Shoot(Cave* cv);
		void Die();
		void Sense() const;
		void ThisCave();
		
		void Win();

		bool IsAlive() const { return m_bAlive; }
		bool IsDone() const { return m_bIsdone; }
		const Cave* Here() const;

		~Player() {  }								// Destructor doesn't do anything
	private:
		void Report(std::ostream& os, const Cave* cv) const;
		void ShootLeft(Cave* cv);
		void ShootRight(Cave * cv);
		const Cave* m_pHere;
		int m_Arrows{ 5 };	// Always five
		bool m_bAlive;
		bool m_bIsdone;
	};

	class Wumpus
	{
	public:
		explicit Wumpus(Cave* cv);
		Wumpus(const Wumpus&) = delete;
		Wumpus& operator=(const Wumpus&) = delete;

		void Move(Cave* cv);
		void Die();
		void WakeUp();

		bool IsAlive() const { return m_bAlive; }
		bool IsAwake() const { return m_bAwake; }
		const Cave* Here() const { return m_pHere; }

	private:
		Cave* m_pHere;
		// m_PastTrap used to make sure the wumpus will be main trap but when it moves will resest the trap
		Cave::Trap m_PastTrap;
		bool m_bAlive;
		bool m_bAwake;
	};
}
