#include "Actors.h"
#include "Cave.h"

namespace Wump
{
	Player::Player(const Cave* cv) : m_pHere{ cv }, m_bAlive{ true }, m_bIsdone{ false } {   }
	const Cave* Player::Here() const { return m_pHere; }
	void Player::ThisCave()
	{
		using std::cout;
		using std::endl;
		cout << endl;
		cout << "You are in cave " << m_pHere->Number() << endl << endl;
		cout << "To the left is cave " << m_pHere->ToLeft()->Number() << endl;
		cout << "To the center is cave " << m_pHere->ToMiddle()->Number() << endl;
		cout << "To the right is cave " << m_pHere->ToRight()->Number() << endl;
		cout << endl;
	}
	void Player::Win()
	{
		m_bIsdone = true;
		std::cout << "Congraulations you have killed the Wumpus!" << std::endl;
	}
	void Player::Move(const Cave* cv)
	{
		if (cv == nullptr)
			return;
		m_pHere = cv;

		if (m_pHere->IsTrapped())	// default std::cout
		{
			switch (m_pHere->CaveTrap())	// player only needs to worry about two traps
			{
			case Cave::Pit:
				std::cout << "You fell to your death in a pit!" << std::endl;
				Die();
				break;
			case Cave::Bat:
				std::cout << "A giant bat swoops down and moves you to a new cave!" << std::endl;
				Move(m_pHere->ToLeft()->ToMiddle());	// TODO randomize
				break;
			default:			// CaveSystem worries about what happens when the wumpus and player meet
				break;
			}
		}
	}
	void Player::Shoot(Cave* cv)
	{
		if (!cv)
			return;

		if (cv != m_pHere->ToLeft() &&	// if the given cave is not one connected
			cv != m_pHere->ToMiddle() &&
			cv != m_pHere->ToRight())
			return;

		if (m_Arrows <= 0)		// can't shoot if there aren't any arrows
		{
			std::cout << "No more arrows to fire!" << std::endl;
			return;
		}
		if (cv == m_pHere->ToLeft())
			ShootLeft(cv);
		else if (cv == m_pHere->ToRight())
			ShootRight(cv);
		else if (cv == m_pHere->ToMiddle()) 
		{					// if you shoot to the middle only goes one cave in
			if (cv->IsTrapped())	// arrows can't turn corners
			{
				auto tr = cv->CaveTrap();
				switch (tr)
				{
					case Cave::Bat:
					{
						cv->SetTrap(Cave::None);
						break;
					}
					case Cave::Wumpus:
					{
						cv->SetTrap(Cave::None);
						break;
					}
					default:
						break;
				}
			}
		}
		--m_Arrows;
	}
	void Player::ShootLeft(Cave* cv)	// no need to test for null
	{
		// only called after the test for null and connections
		int distance = 3;
		while (distance > 0) 
		{
			if (cv->IsTrapped()) 
			{
				auto tr = cv->CaveTrap();
				switch (tr) 
				{
					case Cave::Bat:
					{
						cv->SetTrap(Cave::None);
						break;
					}
					case Cave::Wumpus:
					{
						cv->SetTrap(Cave::None);
						break;
					}
					default:
						break;
				}
			}
			cv = const_cast<Cave*>(cv->ToLeft());
			--distance;
		}
	}
	void Player::ShootRight(Cave * cv)
	{
		int distance = 3;
		while (distance > 0) 
		{
			if (cv->IsTrapped()) 
			{
				auto tr = cv->CaveTrap();
				switch (tr) 
				{
					case Cave::Bat:
					{
						cv->SetTrap(Cave::None);
						break;
					}
					case Cave::Wumpus:
					{
						cv->SetTrap(Cave::None);
						break;
					}
					default:
						break;
					}
			}

			cv = const_cast<Cave*>(cv->ToRight());
			--distance;
		}
	}
	void Player::Die()
	{
		if (m_bAlive)
		{
			m_bAlive = false;
			m_bIsdone = true;
		}
		else return;
	}
	void Player::Sense() const	// default sends to basic output
	{
		Report(std::cout, m_pHere->ToLeft());
		Report(std::cout, m_pHere->ToMiddle());
		Report(std::cout, m_pHere->ToRight());
	}
	void Player::Report(std::ostream& os, const Cave* cv) const
	{
		using std::endl;
		if (cv->IsTrapped())
		{
			switch (cv->CaveTrap())
			{
			case Cave::Pit:
				os << "There is a draft coming through." << endl;
				break;
			case Cave::Bat:
				os << "You hear chirping not far off." << endl;
				break;
			case Cave::Wumpus:
				os << "You smell something foul." << endl;
				break;
			default:
				break;
			}
		}
	}

	Wumpus::Wumpus(Cave* cv) : m_bAlive{ true }, m_bAwake{ false }
	{
		m_pHere = cv;
		m_PastTrap = m_pHere->CaveTrap();	// capture old trap if any
		m_pHere->SetTrap(Cave::Wumpus);
	}
	void Wumpus::Move(Cave* cv)
	{
		if (!cv)
			return;

		if (cv != m_pHere->ToLeft() &&
			cv != m_pHere->ToMiddle() &&
			cv != m_pHere->ToRight())
			return;

		if (m_bAlive && m_bAwake)	// wumpus must be awake AND alive to move
		{
			m_pHere->SetTrap(m_PastTrap);	// reset to the old trap regardless what it is
			m_PastTrap = cv->CaveTrap();	// capture new caves trap
			m_pHere = cv;					// move
			m_pHere->SetTrap(Cave::Wumpus);	// set new trap
			m_bAwake = false;				// wumpus falls asleep after a move
		}
	}
	void Wumpus::Die()
	{
		if (m_bAlive)
			m_bAlive = false;
		m_bAwake = false;
	}
	void Wumpus::WakeUp()	// wumpus needs to be alive and asleep
	{
		if (m_bAlive && !m_bAwake)
			m_bAwake = true;
	}
}
