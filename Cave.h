#pragma once
#include <set>

namespace Wump
{
	class Cave
	{
		friend class CaveSystem;
	public:
		enum Trap	// possible traps
		{
			None,
			Pit,
			Bat,
			Wumpus
		};
		Cave();

		void SetTrap(const Trap tr) { m_Trap = tr; }
		void SetTrap(int tr) { m_Trap = Trap(tr); }
		bool IsTrapped() const;

		int Number() const { return m_Num; }

		const Trap CaveTrap() const { return m_Trap; }
		Cave* ToLeft() const { return left; }
		Cave* ToMiddle() const { return mid; }
		Cave* ToRight() const { return right; }
		~Cave();
	private:
		static std::set<std::size_t> m_Set;	// used to make sure no cave has the same number
		std::size_t m_Num;
		Trap m_Trap;
		Cave* left;		// not used naming convention to make it more intuitive
		Cave* mid;
		Cave* right;
	};
}
